#include "VocabularyQuizWindow.h"
#include "VocabularyResultsDialog.h"
#include "FeedbackDialog.h"
#include <QFont>
#include <QApplication>
#include <QMessageBox>
#include <QIcon>
#include <algorithm>
#include <random>
#include <cstddef>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

VocabularyQuizWindow::VocabularyQuizWindow(const std::vector<VocabularyWord> &words, const QString &profileName, const QString &vocabularyName, const QString &scoresFilePath, const int messageDuration,QWidget *parent)
    : QWidget(parent), vocabularyWords(words), currentWordIndex(0), 
      expectingRomaji(true), expectingEnglish(false), 
      correctRomajiCount(0), incorrectRomajiCount(0),
      correctEnglishCount(0), incorrectEnglishCount(0), hintCount(0),
      profileName(profileName), vocabularyName(vocabularyName), 
      scoresFilePath(scoresFilePath), quizStarted(false), messageDuration(messageDuration) {
    
    // Shuffle the words for random order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vocabularyWords.begin(), vocabularyWords.end(), g);
    
    setupUI();
}

void VocabularyQuizWindow::setupUI() {
    setWindowTitle("Vocabulary Quiz");
    setGeometry(200, 200, 500, 250);
    
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/appicon.ico"));
#else
    setWindowIcon(QIcon(":/appicon.png"));
#endif

    mainLayout = new QVBoxLayout(this);
    
    // Title row with settings button
    QHBoxLayout *titleRow = new QHBoxLayout();
    settingsButton = new QToolButton(this);
    settingsButton->setText("⚙");
    settingsButton->setToolTip("Quiz settings");
    settingsButton->setPopupMode(QToolButton::InstantPopup);
    settingsButton->setStyleSheet("QToolButton { font-size: 16px; padding: 4px 8px; } QToolButton::menu-indicator { image: none; }");

    titleLabel = new QLabel("Vocabulary Quiz Setup", this);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleRow->addWidget(settingsButton, 0, Qt::AlignLeft | Qt::AlignVCenter);
    titleRow->addStretch();
    titleRow->addWidget(titleLabel, 1);
    titleRow->addStretch();
    mainLayout->addLayout(titleRow);
    
    mainLayout->addSpacing(20);
    
    // Checkbox section
    instructionLabel = new QLabel("Choose what to practice (at least one must be selected):", this);
    mainLayout->addWidget(instructionLabel);
    
    checkboxLayout = new QHBoxLayout();
    romajiCheckbox = new QCheckBox("Romaji", this);
    englishCheckbox = new QCheckBox("English", this);
    romajiCheckbox->setChecked(true);
    englishCheckbox->setChecked(true);
    
    checkboxLayout->addWidget(romajiCheckbox);
    checkboxLayout->addWidget(englishCheckbox);
    checkboxLayout->addStretch();
    
    mainLayout->addLayout(checkboxLayout);
    mainLayout->addSpacing(20);
    
    // Start button
    startButton = new QPushButton("Start Quiz", this);
    startButton->setStyleSheet(
        "QPushButton {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    padding: 12px 24px;"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}"
    );
    mainLayout->addWidget(startButton);
    
    // Quiz elements (initially hidden)
    questionLabel = new QLabel("", this);
    QFont questionFont;
    questionFont.setPointSize(24);
    questionLabel->setFont(questionFont);
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLabel->setStyleSheet("padding: 20px; border: 2px solid #bdc3c7; border-radius: 8px;");
    questionLabel->hide();
    mainLayout->addWidget(questionLabel);
    
    answerInput = new QLineEdit(this);
    answerInput->setPlaceholderText("Enter your answer...");
    answerInput->setStyleSheet("font-size: 16px; padding: 8px;");
    answerInput->hide();
    mainLayout->addWidget(answerInput);
    // Event filter to allow early dismissal of messages when typing
    answerInput->installEventFilter(this);
    
    // Create horizontal layout for buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    checkButton = new QPushButton("Check Answer", this);
    checkButton->setStyleSheet(
        "QPushButton {"
        "    font-size: 12px;"
        "    padding: 8px 16px;"
        "    background-color: #27ae60;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #219a52;"
        "}"
    );
    checkButton->hide();
    
    hintButton = new QPushButton("Hint", this);
    hintButton->setStyleSheet(
        "QPushButton {"
        "    font-size: 12px;"
        "    padding: 8px 16px;"
        "    background-color: #3498db;"
        "    color: black;"
        "    border: none;"
        "    border-radius: 6px;"
        "    margin-left: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdc3c7;"
        "    color: #7f8c8d;"
        "}"
    );
    hintButton->hide();
    
    buttonLayout->addWidget(checkButton);
    buttonLayout->addWidget(hintButton);
    buttonLayout->addStretch(); // Push buttons to the left
    
    mainLayout->addLayout(buttonLayout);
    
    // (Inline feedback labels removed; using modal feedback dialog)

    // Score label
    scoreLabel = new QLabel("", this);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setStyleSheet("font-size: 12px; color: #7f8c8d; padding: 5px;");
    scoreLabel->hide();
    mainLayout->addWidget(scoreLabel);
    
    // Back button (initially hidden)
    backButton = new QPushButton("Back to Menu", this);
    backButton->setStyleSheet(
        "QPushButton {"
        "    font-size: 12px;"
        "    padding: 8px 16px;"
        "    background-color: #95a5a6;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #7f8c8d;"
        "}"
    );
    backButton->hide();
    mainLayout->addWidget(backButton);
    
    mainLayout->addStretch();
    
    // Connect signals
    connect(romajiCheckbox, &QCheckBox::checkStateChanged, this, &VocabularyQuizWindow::onRomajiCheckboxChanged);
    connect(englishCheckbox, &QCheckBox::checkStateChanged, this, &VocabularyQuizWindow::onEnglishCheckboxChanged);
    connect(startButton, &QPushButton::clicked, this, &VocabularyQuizWindow::onStartClicked);
    connect(checkButton, &QPushButton::clicked, this, &VocabularyQuizWindow::onCheckAnswer);
    connect(hintButton, &QPushButton::clicked, this, &VocabularyQuizWindow::onHintClicked);
    connect(answerInput, &QLineEdit::returnPressed, this, &VocabularyQuizWindow::onCheckAnswer);
    connect(backButton, &QPushButton::clicked, this, &QWidget::close);

    // Settings menu
    settingsMenu = new QMenu(this);
    toggleCommentsAction = settingsMenu->addAction("Show comments after correct answers");
    toggleCommentsAction->setCheckable(true);
    toggleCommentsAction->setChecked(showCommentsOnCorrect);
    connect(toggleCommentsAction, &QAction::toggled, this, [this](bool checked){
        showCommentsOnCorrect = checked;
        // Persist immediately to profile JSON
        QFile profileFile(QDir::currentPath() + "/profiles/" + profileName + ".json");
        QJsonObject obj;
        if (profileFile.open(QIODevice::ReadOnly)) {
            QByteArray data = profileFile.readAll();
            profileFile.close();
            QJsonParseError parseError; 
            QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
            if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
                obj = doc.object();
            }
        }
        obj["showCommentsOnCorrect"] = checked;
        if (profileFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QJsonDocument outDoc(obj);
            profileFile.write(outDoc.toJson(QJsonDocument::Indented));
            profileFile.close();
        }
    });
    settingsButton->setMenu(settingsMenu);
}

void VocabularyQuizWindow::onRomajiCheckboxChanged(int state) {
    updateCheckboxStates();
}

void VocabularyQuizWindow::onEnglishCheckboxChanged(int state) {
    updateCheckboxStates();
}

void VocabularyQuizWindow::updateCheckboxStates() {
    // Ensure at least one checkbox is always checked
    if (!romajiCheckbox->isChecked() && !englishCheckbox->isChecked()) {
        // If both are unchecked, check the other one
        if (sender() == romajiCheckbox) {
            englishCheckbox->setChecked(true);
        } else {
            romajiCheckbox->setChecked(true);
        }
    }
}

void VocabularyQuizWindow::onStartClicked() {
    expectingRomaji = romajiCheckbox->isChecked();
    expectingEnglish = englishCheckbox->isChecked();
    
    // Hide setup UI
    // checkboxLayout->hide();
    titleLabel->hide();
    romajiCheckbox->hide();
    englishCheckbox->hide();
    startButton->hide();
    instructionLabel->hide();

    // Show quiz UI
    questionLabel->show();
    answerInput->show();
    checkButton->show();
    hintButton->show();
    scoreLabel->show();
    
    quizStarted = true;
    currentWordIndex = 0;
    
    // Reset statistics
    correctRomajiCount = incorrectRomajiCount = 0;
    correctEnglishCount = incorrectEnglishCount = 0;
    incorrectWords.clear();
    
    showNextWord();
}

void VocabularyQuizWindow::showNextWord() {
    if (currentWordIndex >= vocabularyWords.size()) {
        showResults();
        return;
    }
    
    const VocabularyWord &word = vocabularyWords[currentWordIndex];
    questionLabel->setText(word.japanese);
    answerInput->clear();
    answerInput->setFocus();
    // (No inline labels to clear)

    // Enable/disable hint button based on whether the current word has a hint
    hintButton->setEnabled(!word.hint.isEmpty());

    // Update title based on what we're expecting
    if (expectingRomaji && expectingEnglish) {
        titleLabel->setText("Enter Romaji and English (separated by comma)");
    } else if (expectingRomaji) {
        titleLabel->setText("Enter Romaji");
    } else {
        titleLabel->setText("Enter English");
    }
    titleLabel->show();
    
    updateScore();
}

void VocabularyQuizWindow::onCheckAnswer() {
    if (currentWordIndex >= vocabularyWords.size()) return;
    
    QString userInput = answerInput->text().trimmed().toLower();
    if (userInput.isEmpty()) return;
    
    const VocabularyWord &currentWord = vocabularyWords[currentWordIndex];
    bool correct = false;
    
    if (expectingRomaji && expectingEnglish) {
        // Both romaji and english expected, separated by comma
        if(userInput.isEmpty()) {
            incorrectEnglishCount++;
            incorrectRomajiCount++;
            // "Please enter both romaji and english separated by comma",
            showError(QString("<span style='color:#e74c3c;'>%1</span>, <span style='color:#e74c3c;'>%2</span>")
                          .arg(currentWord.romaji, currentWord.english),
                      currentWord.comment);
            return;
        } else {
            QStringList parts = userInput.split(',');
            QString userRomaji, userEnglish;
            if (parts.size() == 2) {
                userRomaji = parts[0].trimmed();
                userEnglish = parts[1].trimmed();
            } else {
                userRomaji = parts[0].trimmed();
                userEnglish = parts[0].trimmed();
            }
            bool romajiCorrect = (userRomaji == currentWord.romaji.toLower());
            bool englishCorrect = (userEnglish == currentWord.english.toLower());
            
            if (romajiCorrect) correctRomajiCount++;
            else incorrectRomajiCount++;
            
            if (englishCorrect) correctEnglishCount++;
            else incorrectEnglishCount++;
            
            if (romajiCorrect && englishCorrect) {
              correct = true;
            } else {
                QString errorMsg;
                if (!romajiCorrect && !englishCorrect) {
                    errorMsg = QString("Correct: <span style='color:#e74c3c;'>%1</span>, <span style='color:#e74c3c;'>%2</span>")
                                   .arg(currentWord.romaji, currentWord.english);
                } else if (!romajiCorrect) {
                    errorMsg = QString("Romaji should be: <span style='color:#e74c3c;'>%1</span>").arg(currentWord.romaji);
                } else {
                    errorMsg = QString("English should be: <span style='color:#e74c3c;'>%1</span>").arg(currentWord.english);
                }
                showError(errorMsg, currentWord.comment);
            }
        }
    } else if (expectingRomaji) {
        checkRomajiAnswer();
        return;
    } else if (expectingEnglish) {
        checkEnglishAnswer();
        return;
    }
    
    if (correct) {
                if(!currentWord.comment.isEmpty() && showCommentsOnCorrect) {
                    showComment(currentWord.comment); // increments index after dismissal
                } else {
                    currentWordIndex++;
                    QTimer::singleShot(200, this, &VocabularyQuizWindow::showNextWord);
                }
    } else {
        incorrectWords[const_cast<VocabularyWord*>(&currentWord)]++;
    }
}

void VocabularyQuizWindow::onHintClicked() {
    if (currentWordIndex >= vocabularyWords.size()) return;
    
    const VocabularyWord &currentWord = vocabularyWords[currentWordIndex];
    
    // Check if the current word has a hint
    if (currentWord.hint.isEmpty()) {
        return; // Button should be disabled, but just in case
    }
    
    // Increment hint counter
    hintCount++;
    
    // Show hint dialog
    QMessageBox hintDialog;
    hintDialog.setWindowTitle("Hint");
    hintDialog.setText(currentWord.hint);
    hintDialog.setIcon(QMessageBox::Information);
    hintDialog.setStandardButtons(QMessageBox::Ok);
    hintDialog.exec();
}

void VocabularyQuizWindow::checkRomajiAnswer() {
    const VocabularyWord &currentWord = vocabularyWords[currentWordIndex];
    QString userInput = answerInput->text().trimmed().toLower();
    QString correctRomaji = currentWord.romaji.toLower();
    
    if (userInput == correctRomaji) {
        correctRomajiCount++;
                if(!currentWord.comment.isEmpty() && showCommentsOnCorrect) {
                    showComment(currentWord.comment); // increments index after dismissal
                } else {
                    currentWordIndex++;
                    QTimer::singleShot(200, this, &VocabularyQuizWindow::showNextWord);
                }
    } else {
        incorrectRomajiCount++;
        incorrectWords[const_cast<VocabularyWord*>(&currentWord)]++;
        
        // Highlight incorrect characters in red
        QString errorMsg = "Correct romaji: ";
        for (int i = 0; i < correctRomaji.length(); ++i) {
            if (i < userInput.length() && userInput[i] == correctRomaji[i]) {
                errorMsg += correctRomaji[i];
            } else {
                errorMsg += QString("<span style='color: red;'>%1</span>").arg(correctRomaji[i]);
            }
        }
        showError(errorMsg, currentWord.comment);
    }
}

void VocabularyQuizWindow::checkEnglishAnswer() {
    const VocabularyWord &currentWord = vocabularyWords[currentWordIndex];
    QString userInput = answerInput->text().trimmed().toLower();
    QString correctEnglish = currentWord.english.toLower();
    
    if (userInput == correctEnglish) {
        correctEnglishCount++;
                if(!currentWord.comment.isEmpty() && showCommentsOnCorrect) {
                    showComment(currentWord.comment); // increments index after dismissal
                } else {
                    currentWordIndex++;
                    QTimer::singleShot(200, this, &VocabularyQuizWindow::showNextWord);
                }
    } else {
        incorrectEnglishCount++;
        incorrectWords[const_cast<VocabularyWord*>(&currentWord)]++;
        showError(QString("Correct English: <span style='color: red;'>%1</span>").arg(currentWord.english), currentWord.comment);
    }
}

void VocabularyQuizWindow::showComment(const QString &comment) {
    QString primary = QString("Correct ✅");
    FeedbackDialog dlg(primary, comment, messageDuration, this);
    dlg.exec();
    currentWordIndex++; // advance once after dialog dismissed or auto-closed
    showNextWord();
}

void VocabularyQuizWindow::showError(const QString &correctAnswer, const QString &comment) {
    QString primary = correctAnswer.isEmpty()
                           ? QString("<span style='color:#e74c3c;'>Incorrect ❌</span>")
                           : correctAnswer;
    FeedbackDialog dlg(primary, comment, messageDuration, this);
    dlg.exec();
    currentWordIndex++; // advance once after dialog dismissed or auto-closed
    showNextWord();
}

void VocabularyQuizWindow::hideErrorMessage() {
    // (No inline labels)
}

void VocabularyQuizWindow::advanceAfterMessage() {
    // Not used with dialog approach
}

void VocabularyQuizWindow::onMessageTimeout() {
    // Not used with dialog approach
}

void VocabularyQuizWindow::updateScore() {
    QString scoreText = QString("Progress: %1/%2").arg(currentWordIndex + 1).arg(vocabularyWords.size());
    
    if (expectingRomaji || expectingEnglish) {
        scoreText += " | ";
        if (expectingRomaji) {
            scoreText += QString("Romaji - Correct: %1, Incorrect: %2").arg(correctRomajiCount).arg(incorrectRomajiCount);
        }
        if (expectingEnglish) {
            if (expectingRomaji) scoreText += " | ";
            scoreText += QString("English - Correct: %1, Incorrect: %2").arg(correctEnglishCount).arg(incorrectEnglishCount);
        }
    }
    
    scoreLabel->setText(scoreText);
}

void VocabularyQuizWindow::showResults() {
    // Calculate percentages
    double romajiPercent = 0.0;
    double englishPercent = 0.0;
    
    if (expectingRomaji) {
        int totalRomaji = correctRomajiCount + incorrectRomajiCount;
        if (totalRomaji > 0) {
            romajiPercent = (double(correctRomajiCount) / totalRomaji) * 100.0;
        }
    }
    
    if (expectingEnglish) {
        int totalEnglish = correctEnglishCount + incorrectEnglishCount;
        if (totalEnglish > 0) {
            englishPercent = (double(correctEnglishCount) / totalEnglish) * 100.0;
        }
    }
    
    // Save scores if we have a vocabulary name (not "All Vocabularies")
    if (!vocabularyName.isEmpty() && vocabularyName != "All Vocabularies") {
        ProfileScores profileScores;
        VocabularyData::loadProfileScores(scoresFilePath, profileScores);
        VocabularyData::updateProfileVocabularyScore(profileScores, profileName, vocabularyName, romajiPercent, englishPercent);
        VocabularyData::saveProfileScores(scoresFilePath, profileScores);
    }
    
    // Show results dialog
    VocabularyResultsDialog resultsDialog(
        expectingRomaji,
        expectingEnglish,
        correctRomajiCount,
        incorrectRomajiCount,
        correctEnglishCount,
        incorrectEnglishCount,
        hintCount,
        incorrectWords,
        this
    );
    
    int result = resultsDialog.exec();
    
    if (result == QDialog::Accepted) {
        VocabularyResultsDialog::ResultChoice choice = resultsDialog.getChoice();
        
        if (choice == VocabularyResultsDialog::TryAgain) {
            resetQuiz();
        } else {
            emit returnToMenuRequested();
            close();
        }
    } else {
        // Dialog was closed, default to return to menu
        emit returnToMenuRequested();
        close();
    }
}

void VocabularyQuizWindow::resetQuiz() {
    // Reset all UI to initial state
    questionLabel->hide();
    answerInput->hide();
    checkButton->hide();
    hintButton->hide();
    scoreLabel->hide();
    backButton->hide();
    // (No inline labels)
    
    // Show setup UI
    titleLabel->setText("Vocabulary Quiz Setup");
    titleLabel->show();
    instructionLabel->show();
    romajiCheckbox->show();
    englishCheckbox->show();
    startButton->show();
    
    // Reset quiz state
    quizStarted = false;
    currentWordIndex = 0;
    
    // Reset statistics
    correctRomajiCount = incorrectRomajiCount = 0;
    correctEnglishCount = incorrectEnglishCount = 0;
    hintCount = 0;
    incorrectWords.clear();
    
    // Shuffle words again for new quiz
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vocabularyWords.begin(), vocabularyWords.end(), g);
}

bool VocabularyQuizWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == answerInput && event->type() == QEvent::KeyPress) {
        // If any message text is present and timer active, advance immediately
        // With dialog approach, no early dismissal needed here
    }
    return QWidget::eventFilter(obj, event);
}