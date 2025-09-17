#include "QuizWindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QHeaderView>
#include <QJsonArray>

#include <QEvent>


QuizWindow::QuizWindow(QWidget *parent) : QWidget(parent) {
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/appicon.ico"));
#else
    setWindowIcon(QIcon(":/appicon.png"));
#endif

    setWindowTitle("Japanese Alphabet Quiz");
    setGeometry(100, 100, 1200, 700);
    prefsFile = QDir::currentPath() + "/japanese_alphabet_quiz_prefs.json";

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    hideTablesCB = new QCheckBox("Hide alphabet tables and script checkboxes", this);
    mainLayout->addWidget(hideTablesCB);
    connect(hideTablesCB, &QCheckBox::checkStateChanged, this, &QuizWindow::savePreferences);

    // Weighted practice option
    weightedPracticeCB = new QCheckBox("Practice hard characters more often", this);
    weightedPracticeCB->setChecked(false);
    mainLayout->addWidget(weightedPracticeCB);

    // Reset hard characters button
    resetHardCharsButton = new QPushButton("Reset Hard Characters", this);
    mainLayout->addWidget(resetHardCharsButton);

    // Now connect signals for widgets that are initialized
    connect(weightedPracticeCB, &QCheckBox::checkStateChanged, this, &QuizWindow::savePreferences);
    connect(resetHardCharsButton, &QPushButton::clicked, this, [this]() {
        Q_EMIT resetHardCharactersRequested();
    });

    QHBoxLayout *timesLayout = new QHBoxLayout();
    timesLabel = new QLabel("Times to show:", this);
    timesLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    timesSpin = new QSpinBox(this);
    timesSpin->setMinimum(1);
    timesSpin->setMaximum(20);
    timesSpin->setValue(1);
    timesSpin->setFixedWidth(35);
    timesLayout->addWidget(timesLabel);
    timesLayout->addWidget(timesSpin);
    mainLayout->addLayout(timesLayout);
    connect(timesSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &QuizWindow::savePreferences);

    QHBoxLayout *tablesLayout = new QHBoxLayout();
    auto createTable = [this](const std::vector<std::vector<KanaEntry>> &alphabet, std::vector<QCheckBox*> &rowChecks, const QString &title) {
        int n = static_cast<int>(alphabet.size());
        int maxRowLen = 0;
        for (const auto &row : alphabet) maxRowLen = std::max(maxRowLen, static_cast<int>(row.size()));
        QTableWidget *table = new QTableWidget(n, maxRowLen + 1);
        table->horizontalHeader()->setVisible(false);
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        QFont font;
        font.setPointSize(12);
        for (int row = 0; row < n; ++row) {
            QCheckBox *cb = new QCheckBox();
            cb->setChecked(true);
            rowChecks.push_back(cb);
            QWidget *w = new QWidget();
            QHBoxLayout *l = new QHBoxLayout(w);
            l->addWidget(cb);
            l->setAlignment(cb, Qt::AlignCenter);
            l->setContentsMargins(0,0,0,0);
            w->setLayout(l);
            table->setCellWidget(row, 0, w);
            // Connect row checkbox to savePreferences
            QObject::connect(cb, &QCheckBox::checkStateChanged, this, &QuizWindow::savePreferences);
        }
        for (int row = 0; row < n; ++row) {
            for (int col = 0; col < static_cast<int>(alphabet[row].size()); ++col) {
                const auto &entry = alphabet[row][col];
                if (entry.kana.isEmpty()) continue;
                QTableWidgetItem *item = new QTableWidgetItem(entry.kana + " (" + entry.romaji + ")");
                item->setFont(font);
                table->setItem(row, col + 1, item);
            }
        }
        table->setEditTriggers(QTableWidget::NoEditTriggers);
        table->setSelectionMode(QTableWidget::NoSelection);
        table->resizeColumnsToContents();
        table->resizeRowsToContents();
        return table;
    };
    hiraganaTable = createTable(AlphabetData::Hiragana, hiraganaRowChecks, "Hiragana");
    katakanaTable = createTable(AlphabetData::Katakana, katakanaRowChecks, "Katakana");
    kanjiTable = createTable(AlphabetData::Kanji, kanjiRowChecks, "Kanji");
    tablesLayout->addWidget(hiraganaTable);
    tablesLayout->addWidget(katakanaTable);
    tablesLayout->addWidget(kanjiTable);
    mainLayout->addLayout(tablesLayout);

    // Horizontal layout for checkboxes and countdown label
    QHBoxLayout *cbRowLayout = new QHBoxLayout();
    QHBoxLayout *cbLayout = new QHBoxLayout();
    hiraganaCB = new QCheckBox("Hiragana", this);
    hiraganaCB->setChecked(true);
    cbLayout->addWidget(hiraganaCB);
    connect(hiraganaCB, &QCheckBox::checkStateChanged, this, &QuizWindow::savePreferences);
    katakanaCB = new QCheckBox("Katakana", this);
    katakanaCB->setChecked(true);
    cbLayout->addWidget(katakanaCB);
    connect(katakanaCB, &QCheckBox::checkStateChanged, this, &QuizWindow::savePreferences);
    kanjiCB = new QCheckBox("Kanji", this);
    kanjiCB->setChecked(true);
    cbLayout->addWidget(kanjiCB);
    connect(kanjiCB, &QCheckBox::checkStateChanged, this, &QuizWindow::savePreferences);

    cbRowLayout->addLayout(cbLayout);
    cbRowLayout->addStretch();
    countdownLabel = new QLabel("Characters left: 0", this);
    QFont countdownFont;
    countdownFont.setPointSize(14);
    countdownFont.setBold(true);
    countdownLabel->setFont(countdownFont);
    countdownLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    cbRowLayout->addWidget(countdownLabel);
    mainLayout->addLayout(cbRowLayout);

    scoreLabel = new QLabel("Correct: 0 | Retries: 0", this);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreFont.setPointSize(18);
    scoreFont.setBold(true);
    scoreLabel->setFont(scoreFont);
    scoreLabel->hide();

    charLabel = new QLabel("", this);
    charLabel->setAlignment(Qt::AlignCenter);
    charFont.setPointSize(64);
    charLabel->setFont(charFont);
    mainLayout->addWidget(charLabel);

    feedbackLabel = new QLabel("", this);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    QFont feedbackFont;
    feedbackFont.setPointSize(16);
    feedbackLabel->setFont(feedbackFont);
    feedbackLabel->setStyleSheet("color: red;");
    // Set a fixed minimum height to prevent layout shifting
    QFontMetrics fm(feedbackFont);
    int minHeight = fm.height() + 6; // add a little padding
    feedbackLabel->setMinimumHeight(minHeight);
    mainLayout->addWidget(feedbackLabel);

    input = new QLineEdit(this);
    input->setPlaceholderText("Enter Romaji...");
    mainLayout->addWidget(input);

    setLayout(mainLayout);

    // Save preferences on window resize
    installEventFilter(this);
}

void QuizWindow::setCountdown(int remaining) {
    countdownLabel->setText(QString("Characters left: %1").arg(remaining));
}

// Save preferences on window resize event
bool QuizWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Resize) {
        savePreferences();
    }
    return QWidget::eventFilter(obj, event);
}

void QuizWindow::savePreferences() {
    QJsonObject prefs;
    // Script checkboxes
    prefs["hiragana_cb"] = hiraganaCB->isChecked();
    prefs["katakana_cb"] = katakanaCB->isChecked();
    prefs["kanji_cb"] = kanjiCB->isChecked();
    prefs["hide_tables_cb"] = hideTablesCB->isChecked();

    // Row checkboxes
    QJsonArray hiraRows, kataRows, kanjiRows;
    for (auto cb : hiraganaRowChecks) hiraRows.append(cb->isChecked());
    for (auto cb : katakanaRowChecks) kataRows.append(cb->isChecked());
    for (auto cb : kanjiRowChecks) kanjiRows.append(cb->isChecked());
    prefs["hiragana_rows"] = hiraRows;
    prefs["katakana_rows"] = kataRows;
    prefs["kanji_rows"] = kanjiRows;

    // Window size
    QJsonArray winSize;
    winSize.append(width());
    winSize.append(height());
    prefs["window_size"] = winSize;

    // Times to show
    prefs["times_to_show"] = timesSpin->value();

    // Save weighted practice option
    prefs["weighted_practice"] = weightedPracticeCB->isChecked();

    // Save error stats if present (to be filled by QuizGame)
    if (errorStatsJson.size() > 0) {
        prefs["error_stats"] = errorStatsJson;
    }

    QFile file(prefsFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument doc(prefs);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}
void QuizWindow::loadPreferences() {
    QFile file(prefsFile);
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QByteArray data = file.readAll();
    file.close();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return;
    QJsonObject prefs = doc.object();

    // Script checkboxes
    if (prefs.contains("hiragana_cb")) hiraganaCB->setChecked(prefs["hiragana_cb"].toBool(true));
    if (prefs.contains("katakana_cb")) katakanaCB->setChecked(prefs["katakana_cb"].toBool(true));
    if (prefs.contains("kanji_cb")) kanjiCB->setChecked(prefs["kanji_cb"].toBool(true));
    if (prefs.contains("hide_tables_cb")) hideTablesCB->setChecked(prefs["hide_tables_cb"].toBool(false));

    // Row checkboxes
    auto setRowChecks = [](const QJsonArray &arr, std::vector<QCheckBox*> &checks) {
        int n = std::min(static_cast<int>(arr.size()), static_cast<int>(checks.size()));
        for (int i = 0; i < n; ++i) checks[i]->setChecked(arr[i].toBool(true));
    };
    if (prefs.contains("hiragana_rows") && prefs["hiragana_rows"].isArray())
        setRowChecks(prefs["hiragana_rows"].toArray(), hiraganaRowChecks);
    if (prefs.contains("katakana_rows") && prefs["katakana_rows"].isArray())
        setRowChecks(prefs["katakana_rows"].toArray(), katakanaRowChecks);
    if (prefs.contains("kanji_rows") && prefs["kanji_rows"].isArray())
        setRowChecks(prefs["kanji_rows"].toArray(), kanjiRowChecks);

    // Times to show
    if (prefs.contains("times_to_show")) {
        int t = prefs["times_to_show"].toInt(1);
        if (t > 0) timesSpin->setValue(t);
    }
    // Weighted practice
    if (prefs.contains("weighted_practice")) weightedPracticeCB->setChecked(prefs["weighted_practice"].toBool(false));
    // Load error stats if present
    if (prefs.contains("error_stats") && prefs["error_stats"].isObject()) {
        errorStatsJson = prefs["error_stats"].toObject();
    } else {
        errorStatsJson = QJsonObject();
    }

    // Disable tables if their script checkbox is not checked
    hiraganaTable->setDisabled(!hiraganaCB->isChecked());
    katakanaTable->setDisabled(!katakanaCB->isChecked());
    kanjiTable->setDisabled(!kanjiCB->isChecked());

    // Window size
    if (prefs.contains("window_size") && prefs["window_size"].isArray()) {
        QJsonArray arr = prefs["window_size"].toArray();
        if (arr.size() == 2) {
            int w = arr[0].toInt(1200);
            int h = arr[1].toInt(700);
            if (w > 100 && h > 100) resize(w, h);
        }
    }
}
void QuizWindow::updateScore(int correct, int retries) {
    scoreLabel->setText(QString("Correct: %1 | Retries: %2").arg(correct).arg(retries));
}
void QuizWindow::setChar(const QString &kana) {
    charLabel->setText(kana);
}
void QuizWindow::setFeedback(const QString &msg, bool error) {
    feedbackLabel->setText(msg);
    feedbackLabel->setStyleSheet(error ? "color: red;" : "color: green;");
}
void QuizWindow::clearInput() {
    input->clear();
}
void QuizWindow::setInputEnabled(bool enabled) {
    input->setEnabled(enabled);
}
void QuizWindow::highlightTableChar(const QString &kana, const QString &romaji, const QString &color) {
    auto highlight = [&](QTableWidget *table, const std::vector<std::vector<KanaEntry>> &alphabet) {
        for (int row = 0; row < static_cast<int>(alphabet.size()); ++row) {
            for (int col = 0; col < static_cast<int>(alphabet[row].size()); ++col) {
                const auto &entry = alphabet[row][col];
                if (entry.kana == kana && entry.romaji == romaji) {
                    QTableWidgetItem *item = table->item(row, col + 1);
                    if (item) item->setBackground(QColor(color));
                }
            }
        }
    };
    highlight(hiraganaTable, AlphabetData::Hiragana);
    highlight(katakanaTable, AlphabetData::Katakana);
    highlight(kanjiTable, AlphabetData::Kanji);
}
void QuizWindow::resetTableHighlights() {
    auto reset = [](QTableWidget *table) {
        for (int row = 0; row < table->rowCount(); ++row) {
            for (int col = 1; col < table->columnCount(); ++col) {
                QTableWidgetItem *item = table->item(row, col);
                if (item) item->setBackground(Qt::NoBrush);
            }
        }
    };
    reset(hiraganaTable);
    reset(katakanaTable);
    reset(kanjiTable);
}
std::vector<bool> QuizWindow::getRowChecks(const std::vector<QCheckBox*> &checks) const {
    std::vector<bool> res;
    for (auto cb : checks) res.push_back(cb->isChecked());
    return res;
}
