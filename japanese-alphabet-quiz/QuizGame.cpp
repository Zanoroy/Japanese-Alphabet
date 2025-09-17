#include "QuizGame.h"
#include <QMap>
#include <QMessageBox>
#include <QTimer>
#include <algorithm>
#include <random>

QuizGame::QuizGame(QuizWindow *window) : QObject(window), window(window) {
    // Load error stats from window
    loadErrorStats();
    // Connect reset button
    connect(window, &QuizWindow::resetHardCharactersRequested, this, &QuizGame::resetErrorStats);
    connect(window->timesSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &QuizGame::newQuiz);
    connect(window->hiraganaCB, &QCheckBox::checkStateChanged, [this](int state){ handleScriptCheckbox("hiragana", state); });
    connect(window->katakanaCB, &QCheckBox::checkStateChanged, [this](int state){ handleScriptCheckbox("katakana", state); });
    connect(window->kanjiCB, &QCheckBox::checkStateChanged, [this](int state){ handleScriptCheckbox("kanji", state); });
    connect(window->input, &QLineEdit::returnPressed, this, &QuizGame::checkAnswer);
    for (auto cb : window->hiraganaRowChecks)
        connect(cb, &QCheckBox::checkStateChanged, [this, cb](int state){ handleRowCheckbox("hiragana", cb, state); });
    for (auto cb : window->katakanaRowChecks)
        connect(cb, &QCheckBox::checkStateChanged, [this, cb](int state){ handleRowCheckbox("katakana", cb, state); });
    for (auto cb : window->kanjiRowChecks)
        connect(cb, &QCheckBox::checkStateChanged, [this, cb](int state){ handleRowCheckbox("kanji", cb, state); });
    newQuiz();
}

void QuizGame::newQuiz() {
    timesToShow = window->timesSpin->value();
    roundChars.clear();
    answeredOnce.clear();
    charStatsCorrect.clear();
    charStatsIncorrect.clear();
    window->resetTableHighlights();
    auto enabled = getEnabledAlphabets();
    for (const auto &rows : enabled) {
        for (const auto &entry : rows) {
            if (!entry.kana.isEmpty()) {
                for (int i = 0; i < timesToShow; ++i)
                    roundChars.emplace_back(entry.kana, entry.romaji);
            }
        }
    }
    unansweredChars = roundChars;
    correctCount = 0;
    retryCount = 0;
    window->updateScore(correctCount, retryCount);
    window->setChar("");
    window->setFeedback("");
    window->setCountdown(static_cast<int>(unansweredChars.size()));
    newQuestion();
}

void QuizGame::newQuestion(bool excludeCurrent) {
    window->setFeedback("");
    window->setCountdown(static_cast<int>(unansweredChars.size()));
    auto enabled = getEnabledAlphabets();
    if (enabled.empty()) {
        window->setChar("");
        window->setInputEnabled(false);
        QMessageBox::warning(window, "No Script Selected", "Please select at least one script to practice.");
        return;
    } else {
        window->setInputEnabled(true);
    }
    if (unansweredChars.empty()) {
        std::vector<std::pair<QString, QString>> allChars;
        for (const auto &rows : enabled) {
            for (const auto &entry : rows) {
                if (!entry.kana.isEmpty())
                    allChars.emplace_back(entry.kana, entry.romaji);
            }
        }
        showSummaryAndReset(allChars);
        return;
    }
    std::vector<std::pair<QString, QString>> available = unansweredChars;
    if (excludeCurrent && available.size() > 1) {
        auto it = std::find(available.begin(), available.end(), std::make_pair(currentKana, currentRomaji));
        if (it != available.end()) available.erase(it);
    }
    // Weighted random selection if enabled
    bool weighted = window->weightedPracticeCB && window->weightedPracticeCB->isChecked();
    std::vector<double> weights;
    if (weighted) {
        for (const auto &pair : available) {
            int err = errorStats.value(pair, 0);
            weights.push_back(1.0 + err * 3.0); // base weight 1, +3 per error
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    int idx = 0;
    if (weighted && !weights.empty()) {
        std::discrete_distribution<> dist(weights.begin(), weights.end());
        idx = dist(g);
    } else {
        std::uniform_int_distribution<> dist(0, static_cast<int>(available.size()) - 1);
        idx = dist(g);
    }
    auto chosen = available[idx];
    currentKana = chosen.first;
    currentRomaji = chosen.second;
    window->setChar(currentKana);
    window->clearInput();
}

void QuizGame::checkAnswer() {
    QString userInput = window->input->text().trimmed().toLower();
    window->setFeedback("");
    if (userInput == currentRomaji) {
        correctCount++;
        answeredOnce.insert({currentKana, currentRomaji});
        charStatsCorrect[{currentKana, currentRomaji}]++;
        auto it = std::find(unansweredChars.begin(), unansweredChars.end(), std::make_pair(currentKana, currentRomaji));
        if (it != unansweredChars.end()) {
            unansweredChars.erase(it);
            // If this was the last time for this character, mark green
            bool stillInQuiz = false;
            for (const auto &pair : unansweredChars) {
                if (pair.first == currentKana && pair.second == currentRomaji) {
                    stillInQuiz = true;
                    break;
                }
            }
            if (!stillInQuiz) {
                window->highlightTableChar(currentKana, currentRomaji, "#4CAF50"); // green
            }
        }
    // Decrease error count by 1 (min 0) on correct answer
    auto key = std::make_pair(currentKana, currentRomaji);
    int prevErr = errorStats.value(key, 0);
    errorStats[key] = std::max(0, prevErr - 1);
    saveErrorStats();
        window->updateScore(correctCount, retryCount);
        if (unansweredChars.size() > 1)
            newQuestion(true);
        else
            newQuestion();
    } else {
        retryCount++;
        charStatsIncorrect[{currentKana, currentRomaji}]++;
    auto key = std::make_pair(currentKana, currentRomaji);
    errorStats[key] = errorStats.value(key, 0) + 5;
        saveErrorStats();
        window->highlightTableChar(currentKana, currentRomaji, "#F44336"); // red
        window->setFeedback(QString("Incorrect! %1 = %2").arg(currentKana, currentRomaji), true);
        window->updateScore(correctCount, retryCount);
        // Show next question after a short delay
        QTimer::singleShot(700, this, [this]() {
            if (unansweredChars.size() > 1)
                newQuestion(true);
            else
                newQuestion();
        });
    }
}

// --- Error stats persistence ---
void QuizGame::loadErrorStats() {
    errorStats.clear();
    if (!window->errorStatsJson.isEmpty()) {
        for (auto it = window->errorStatsJson.begin(); it != window->errorStatsJson.end(); ++it) {
            QString key = it.key();
            int val = it.value().toInt();
            int sep = key.indexOf("|");
            if (sep > 0) {
                QString kana = key.left(sep);
                QString romaji = key.mid(sep+1);
                errorStats[{kana, romaji}] = val;
            }
        }
    }
}

void QuizGame::saveErrorStats() {
    QJsonObject obj;
    for (auto it = errorStats.begin(); it != errorStats.end(); ++it) {
        QString key = it.key().first + "|" + it.key().second;
        obj[key] = it.value();
    }
    window->errorStatsJson = obj;
    window->savePreferences();
}

void QuizGame::resetErrorStats() {
    errorStats.clear();
    saveErrorStats();
}

void QuizGame::handleScriptCheckbox(const QString &script, int state) {
    if (script == "hiragana") window->hiraganaTable->setDisabled(state == 0);
    if (script == "katakana") window->katakanaTable->setDisabled(state == 0);
    if (script == "kanji") window->kanjiTable->setDisabled(state == 0);
    newQuiz();
}
void QuizGame::handleRowCheckbox(const QString &script, QCheckBox *cb, int state) {
    std::vector<QCheckBox*> *rowChecks = nullptr;
    if (script == "hiragana") rowChecks = &window->hiraganaRowChecks;
    if (script == "katakana") rowChecks = &window->katakanaRowChecks;
    if (script == "kanji") rowChecks = &window->kanjiRowChecks;
    if (!rowChecks) return;
    if (state == 0) {
        int checked = 0;
        for (auto c : *rowChecks) if (c->isChecked()) ++checked;
        if (checked == 0) {
            cb->blockSignals(true);
            cb->setChecked(true);
            cb->blockSignals(false);
            return;
        }
    }
    newQuiz();
}
void QuizGame::updateScore() {
    window->updateScore(correctCount, retryCount);
}
std::vector<std::vector<KanaEntry>> QuizGame::getEnabledAlphabets() const {
    std::vector<std::vector<KanaEntry>> enabled;
    auto getCheckedRows = [](const std::vector<QCheckBox*> &rowChecks, const std::vector<std::vector<KanaEntry>> &alphabet) {
        std::vector<KanaEntry> chars;
        for (size_t row = 0; row < rowChecks.size(); ++row) {
            if (rowChecks[row]->isChecked()) {
                for (const auto &entry : alphabet[row]) {
                    if (!entry.kana.isEmpty()) chars.push_back(entry);
                }
            }
        }
        return chars;
    };
    if (window->hiraganaCB->isChecked()) enabled.push_back(getCheckedRows(window->hiraganaRowChecks, AlphabetData::Hiragana));
    if (window->katakanaCB->isChecked()) enabled.push_back(getCheckedRows(window->katakanaRowChecks, AlphabetData::Katakana));
    if (window->kanjiCB->isChecked()) enabled.push_back(getCheckedRows(window->kanjiRowChecks, AlphabetData::Kanji));
    return enabled;
}
void QuizGame::showSummaryAndReset(const std::vector<std::pair<QString, QString>> &allChars) {
    int right = 0, wrong = 0;
    QStringList hardChars;
    for (const auto &charTuple : allChars) {
        int incorrect = charStatsIncorrect[charTuple];
        if (incorrect > 0) {
            ++wrong;
            if (incorrect > 1)
                hardChars << QString("%1 (%2)").arg(charTuple.first, charTuple.second);
        } else {
            ++right;
        }
    }
    QString msg = QString("Quiz Complete!\n\nCorrect: %1\nIncorrect: %2").arg(right).arg(wrong);
    if (!hardChars.isEmpty()) {
        msg += "\n\nYou should work on these characters (missed more than once):\n" + hardChars.join("\n");
    } else {
        msg += "\n\nGreat job!";
    }
    QMessageBox::information(window, "Quiz Summary", msg);
    window->resetTableHighlights();
    newQuiz();
    updateScore();
}
