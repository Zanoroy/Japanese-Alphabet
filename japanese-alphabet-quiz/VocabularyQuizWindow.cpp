#include "VocabularyQuizWindow.h"
#include "VocabularyResultsDialog.h"
#include <QFont>
#include <QApplication>
#include <QMessageBox>
#include <QIcon>
#include <algorithm>
#include <random>
#include <cstddef>

VocabularyQuizWindow::VocabularyQuizWindow(const std::vector<VocabularyWord> &words, const QString &profileName, const QString &vocabularyName, const QString &scoresFilePath, QWidget *parent)
    : QWidget(parent), vocabularyWords(words), currentWordIndex(0), 
      expectingRomaji(true), expectingEnglish(false), 
      correctRomajiCount(0), incorrectRomajiCount(0),
      correctEnglishCount(0), incorrectEnglishCount(0),
      profileName(profileName), vocabularyName(vocabularyName), 
      scoresFilePath(scoresFilePath), quizStarted(false) {
    
    // Shuffle the words for random order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vocabularyWords.begin(), vocabularyWords.end(), g);
    
    setupUI();
    
    msgTimer = new QTimer(this);
    msgTimer->setSingleShot(true);
    connect(msgTimer, &QTimer::timeout, this, &VocabularyQuizWindow::hideErrorMessage);
}

void VocabularyQuizWindow::setupUI() {
    setWindowTitle("Vocabulary Quiz");
    setGeometry(200, 200, 500, 400);
    
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/appicon.ico"));
#else
    setWindowIcon(QIcon(":/appicon.png"));
#endif

    mainLayout = new QVBoxLayout(this);
    
    // Title
    titleLabel = new QLabel("Vocabulary Quiz Setup", this);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
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
    mainLayout->addWidget(checkButton);
    
    // Error/feedback label
    errorLabel = new QLabel("", this);
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setStyleSheet("color: red; font-size: 14px; font-weight: bold; padding: 10px;");
    errorLabel->setMinimumHeight(30);
    mainLayout->addWidget(errorLabel);
    
    // Comment label
    commentLabel = new QLabel("", this);
    commentLabel->setAlignment(Qt::AlignCenter);
    commentLabel->setStyleSheet("font-size: 12px; font-weight: bold; padding: 10px;");
    commentLabel->setMinimumHeight(30);
    mainLayout->addWidget(commentLabel);

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
    connect(answerInput, &QLineEdit::returnPressed, this, &VocabularyQuizWindow::onCheckAnswer);
    connect(backButton, &QPushButton::clicked, this, &QWidget::close);
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
    errorLabel->clear();
    commentLabel->clear();

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
            showError("Please enter both romaji and english separated by comma", 
                      QString("%1, %2").arg(currentWord.romaji, currentWord.english), currentWord.comment);
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
                    errorMsg = QString("Correct: %1, %2").arg(currentWord.romaji, currentWord.english);
                } else if (!romajiCorrect) {
                    errorMsg = QString("Romaji should be: %1").arg(currentWord.romaji);
                } else {
                    errorMsg = QString("English should be: %1").arg(currentWord.english);
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
        currentWordIndex++;
        if(!currentWord.comment.isEmpty()) {
          showComment(currentWord.comment);
        } else {
          QTimer::singleShot(200, this, &VocabularyQuizWindow::showNextWord);
        }
    } else {
        incorrectWords[const_cast<VocabularyWord*>(&currentWord)]++;
    }
}

void VocabularyQuizWindow::checkRomajiAnswer() {
    const VocabularyWord &currentWord = vocabularyWords[currentWordIndex];
    QString userInput = answerInput->text().trimmed().toLower();
    QString correctRomaji = currentWord.romaji.toLower();
    
    if (userInput == correctRomaji) {
        correctRomajiCount++;
        currentWordIndex++;
        if(!currentWord.comment.isEmpty()) {
          showComment(currentWord.comment);
        } else {
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
        currentWordIndex++;        
        if(!currentWord.comment.isEmpty()) {
          showComment(currentWord.comment);
        } else {
          QTimer::singleShot(200, this, &VocabularyQuizWindow::showNextWord);
        }
    } else {
        incorrectEnglishCount++;
        incorrectWords[const_cast<VocabularyWord*>(&currentWord)]++;
        showError(QString("Correct English: <span style='color: red;'>%1</span>").arg(currentWord.english), currentWord.comment);
    }
}

void VocabularyQuizWindow::showComment(const QString &comment) {
    if (!comment.isEmpty()) {
        commentLabel->setText(comment);
    }

    msgTimer->start(2000); // Show for 2 seconds

    // Move to next word after showing error
    QTimer::singleShot(2000, this, [this]() {
        currentWordIndex++;
        showNextWord();
    });
}

void VocabularyQuizWindow::showError(const QString &message, const QString &correctAnswer, const QString &comment) {
    if (!correctAnswer.isEmpty()) {
        errorLabel->setText(correctAnswer);
    } else {
        errorLabel->setText(message);
    }

    if (!correctAnswer.isEmpty()) {
        commentLabel->setText(comment);
    }

    msgTimer->start(2000); // Show for 2 seconds

    // Move to next word after showing error
    QTimer::singleShot(2000, this, [this]() {
        currentWordIndex++;
        showNextWord();
    });
}

void VocabularyQuizWindow::hideErrorMessage() {
    errorLabel->clear();
    commentLabel->clear();
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
    scoreLabel->hide();
    backButton->hide();
    errorLabel->clear();
    commentLabel->clear();
    
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
    incorrectWords.clear();
    
    // Shuffle words again for new quiz
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(vocabularyWords.begin(), vocabularyWords.end(), g);
}