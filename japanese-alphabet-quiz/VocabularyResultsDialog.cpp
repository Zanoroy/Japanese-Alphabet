#include "VocabularyResultsDialog.h"
#include <QFont>
#include <QIcon>
#include <QScrollArea>

VocabularyResultsDialog::VocabularyResultsDialog(
    bool expectingRomaji,
    bool expectingEnglish,
    int correctRomajiCount,
    int incorrectRomajiCount,
    int correctEnglishCount,
    int incorrectEnglishCount,
    int hintCount,
    const std::map<VocabularyWord*, int> &incorrectWords,
    QWidget *parent)
    : QDialog(parent), choice(ReturnToMenu) {
    
    setWindowTitle("Quiz Results");
    resize(500, 600);  // Set initial size but allow resizing
    setMinimumSize(400, 400);  // Set minimum size to prevent too small
    
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/appicon.ico"));
#else
    setWindowIcon(QIcon(":/appicon.png"));
#endif
    int percentageMedium = 60; // Define medium percentage threshold
    int percentageHigh = 85; // Define high percentage threshold

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);
    
    QString labelStyle = "font-size: 14px; padding: 5px; border-radius: 4px; margin: 2px; border: none;";

    // Title
    QLabel *titleLabel = new QLabel("Quiz Complete!", this);
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #ffffffff; padding: 20px; border: none;");
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainLayout->addWidget(titleLabel);
    
    // Results summary (always visible)
    QWidget *summaryWidget = new QWidget(this);
    summaryWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryWidget);
    summaryLayout->setContentsMargins(10, 10, 10, 10);
    
    QLabel *summaryLabel = new QLabel("Results Summary:", this);
    QFont summaryFont;
    summaryFont.setPointSize(16);
    summaryFont.setBold(true);
    summaryLabel->setFont(summaryFont);
    summaryLabel->setStyleSheet("color: #ffffffff; padding: 10px 0; border: none;");
    summaryLayout->addWidget(summaryLabel);
    
    // Score details
    if (expectingRomaji) {
        int totalRomaji = correctRomajiCount + incorrectRomajiCount;
        double romajiPercent = totalRomaji > 0 ? (double(correctRomajiCount) / totalRomaji * 100) : 0;
        
        // Color code percentage based on performance
        QString percentColor = "#27ae60"; // Green for good (85%+)
        if (romajiPercent < percentageMedium) percentColor = "#e74c3c"; // Red for poor (<60%)
        else if (romajiPercent < percentageHigh) percentColor = "#f39c12"; // Orange for okay (60-84%)

        QLabel *romajiLabel = new QLabel(
            QString("<span style='font-size: 18px;'>📝</span> Romaji: %1 correct, %2 incorrect (<span style='color: %3; font-weight: bold; font-size: 16px;'>%4%</span>)")
            .arg(correctRomajiCount)
            .arg(incorrectRomajiCount)
            .arg(percentColor)
            .arg(QString::number(romajiPercent, 'f', 1)),
            this
        );
        romajiLabel->setStyleSheet(labelStyle);
        summaryLayout->addWidget(romajiLabel);
    }
    
    if (expectingEnglish) {
        int totalEnglish = correctEnglishCount + incorrectEnglishCount;
        double englishPercent = totalEnglish > 0 ? (double(correctEnglishCount) / totalEnglish * 100) : 0;
        
        // Color code percentage based on performance
        QString percentColor = "#27ae60"; // Green for good (85%+)
        if (englishPercent < percentageMedium) percentColor = "#e74c3c"; // Red for poor (<70%)
        else if (englishPercent < percentageHigh) percentColor = "#f39c12"; // Orange for okay (70-84%)
        
        QLabel *englishLabel = new QLabel(
            QString("<span style='font-size: 18px;'>🇦🇺</span> English: %1 correct, %2 incorrect (<span style='color: %3; font-weight: bold; font-size: 16px;'>%4%</span>)")
            .arg(correctEnglishCount)
            .arg(incorrectEnglishCount)
            .arg(percentColor)
            .arg(QString::number(englishPercent, 'f', 1)),
            this
        );
        englishLabel->setStyleSheet(labelStyle);
        summaryLayout->addWidget(englishLabel);
    }
    
    // Overall accuracy
    int totalCorrect = correctRomajiCount + correctEnglishCount;
    int totalIncorrect = incorrectRomajiCount + incorrectEnglishCount;
    int totalAnswers = totalCorrect + totalIncorrect;
    
    if (totalAnswers > 0) {
        double overallPercent = (double(totalCorrect) / totalAnswers) * 100;
        QString accuracyText = QString("🎯 Overall Accuracy: %1%").arg(QString::number(overallPercent, 'f', 1));
        
        QLabel *accuracyLabel = new QLabel(accuracyText, this);
        QFont accuracyFont;
        accuracyFont.setPointSize(14);
        accuracyFont.setBold(true);
        accuracyLabel->setFont(accuracyFont);
        
        // Color code based on performance
        QString color = "#27ae60"; // Green for good
        if (overallPercent < percentageMedium) color = "#e74c3c"; // Red for poor
        else if (overallPercent < percentageHigh) color = "#f39c12"; // Orange for okay

        accuracyLabel->setStyleSheet(QString("color: %1; border-radius: 6px; margin: 5px; border: none;").arg(color));
        summaryLayout->addWidget(accuracyLabel);
    }
    
    // Hint count display
    if (hintCount > 0) {
        QString hintText = QString("💡 Hints Used: %1").arg(hintCount);
        
        QLabel *hintLabel = new QLabel(hintText, this);
        QFont hintFont;
        hintFont.setPointSize(12);
        hintFont.setBold(true);
        hintLabel->setFont(hintFont);
        hintLabel->setStyleSheet("color: #3498db; border-radius: 6px; margin: 5px; border: none;");
        summaryLayout->addWidget(hintLabel);
    }
    
    // Add summary widget to main layout (always visible)
    mainLayout->addWidget(summaryWidget);
    
        // Create scrollable area for vocabulary words only
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: 1px solid #bdc3c7; border-radius: 8px;");
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    QWidget *wordsWidget = new QWidget();
    QVBoxLayout *wordsLayout = new QVBoxLayout(wordsWidget);
    wordsLayout->setContentsMargins(10, 10, 10, 10);
    
    // Words to practice more
    if (!incorrectWords.empty()) {
        QLabel *practiceLabel = new QLabel("Words to Practice More:", this);
        QFont practiceFont;
        practiceFont.setPointSize(14);
        practiceFont.setBold(true);
        practiceLabel->setFont(practiceFont);
        practiceLabel->setStyleSheet("color: #e74c3c; padding: 10px 0; border: none;");
        wordsLayout->addWidget(practiceLabel);
        
        // Sort incorrect words by mistake count (descending)
        std::vector<std::pair<const VocabularyWord*, int>> sortedIncorrect;
        for (const auto &pair : incorrectWords) {
            sortedIncorrect.push_back({pair.first, pair.second});
        }
        std::sort(sortedIncorrect.begin(), sortedIncorrect.end(),
                  [](const auto &a, const auto &b) { return a.second > b.second; });
        
        for (const auto &pair : sortedIncorrect) {
            const VocabularyWord *word = pair.first;
            int count = pair.second;
            
            QString wordText = QString("• %1 (%2) → %3")
                              .arg(word->japanese, word->romaji, word->english);
            
            if (count > 1) {
                wordText += QString(" - %1 mistake%2").arg(count).arg(count > 1 ? "s" : "");
            }
            
            QLabel *wordLabel = new QLabel(wordText, this);
            wordLabel->setStyleSheet(
                "font-size: 12px; padding: 8px; margin: 2px; "
                "border-left: 4px solid #e74c3c; "
                "border-radius: 4px;"
            );
            wordLabel->setWordWrap(true);
            wordsLayout->addWidget(wordLabel);
        }
    } else {
        QLabel *perfectLabel = new QLabel("🎉 Perfect! No mistakes!", this);
        QFont perfectFont;
        perfectFont.setPointSize(16);
        perfectFont.setBold(true);
        perfectLabel->setFont(perfectFont);
        perfectLabel->setAlignment(Qt::AlignCenter);
        perfectLabel->setStyleSheet(
            "color: #27ae60; padding: 20px; "
            "background-color: #d5f4e6; border-radius: 8px; margin: 10px;"
        );
        wordsLayout->addWidget(perfectLabel);
    }
    
    wordsLayout->addStretch();
    scrollArea->setWidget(wordsWidget);
    mainLayout->addWidget(scrollArea);
    
    // Buttons (always visible at bottom)
    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    
    tryAgainButton = new QPushButton("🔄 Try Again", this);
    returnToMenuButton = new QPushButton("🏠 Return to Main Menu", this);
    
    // Style buttons
    QString tryAgainStyle = 
        "QPushButton {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    padding: 12px 20px;"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    margin: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}";
    
    QString menuStyle = 
        "QPushButton {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    padding: 12px 20px;"
        "    background-color: #95a5a6;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    margin: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #7f8c8d;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #6c7b7d;"
        "}";
    
    tryAgainButton->setStyleSheet(tryAgainStyle);
    returnToMenuButton->setStyleSheet(menuStyle);
    tryAgainButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    returnToMenuButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    buttonLayout->addWidget(tryAgainButton);
    buttonLayout->addWidget(returnToMenuButton);
    mainLayout->addWidget(buttonWidget);
    
    // Connect signals
    connect(tryAgainButton, &QPushButton::clicked, this, &VocabularyResultsDialog::onTryAgainClicked);
    connect(returnToMenuButton, &QPushButton::clicked, this, &VocabularyResultsDialog::onReturnToMenuClicked);
}

void VocabularyResultsDialog::onTryAgainClicked() {
    choice = TryAgain;
    accept();
}

void VocabularyResultsDialog::onReturnToMenuClicked() {
    choice = ReturnToMenu;
    accept();
}