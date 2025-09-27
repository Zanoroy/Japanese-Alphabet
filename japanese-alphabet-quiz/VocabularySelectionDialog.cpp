#include "VocabularySelectionDialog.h"
#include <QFont>
#include <QIcon>
#include <QComboBox>

VocabularySelectionDialog::VocabularySelectionDialog(const std::vector<Vocabulary> &vocabularies, 
                                                   const ProfileScores &scores,
                                                   const QString &profileName,
                                                   QWidget *parent)
    : QDialog(parent), practiceAll(false), selectedIndex(-1), vocabularies(vocabularies), scores(scores), profileName(profileName) {
    
    setWindowTitle("Select Vocabulary");
    resize(550, 400);
    setMinimumSize(500, 300);
    
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/appicon.ico"));
#else
    setWindowIcon(QIcon(":/appicon.png"));
#endif
    QPalette palette = this->palette();
    QColor bgColor = palette.color(QPalette::Window);
    QColor baseColor = palette.color(QPalette::Base);

    QString bgColorHex = bgColor.name(); // Returns "#rrggbb" format
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *titleLabel = new QLabel("Select Vocabulary to Practice", this);
    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    mainLayout->addSpacing(20);
    
    // Vocabulary selection
    QLabel *selectLabel = new QLabel("Choose a vocabulary:", this);
    selectLabel->setStyleSheet("font-size: 14px; margin-bottom: 5px;");
    mainLayout->addWidget(selectLabel);
    
    vocabularyComboBox = new QComboBox(this);
    vocabularyComboBox->setStyleSheet(
        "QComboBox {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    background-color: #" + bgColorHex + ";"
        "    color: #2c3e50;"
        "    padding: 8px;"
        "    font-size: 12px;"
        "    min-height: 20px;"
        "}"
        "QComboBox:hover {"
        "    border: 2px solid #3498db;"
        "}"
        "QComboBox QAbstractItemView {"
        "    border: 1px solid #bdc3c7;"
        "    background-color: #" + bgColorHex + ";"
        "    color: #2c3e50;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "}"
    );
    
    populateVocabularyComboBox();
    

    
    mainLayout->addWidget(vocabularyComboBox);
    
    mainLayout->addSpacing(20);
    
    // Buttons
    practiceButton = new QPushButton("Practice Selected", this);
    practiceAllButton = new QPushButton("Practice All", this);
    backButton = new QPushButton("Back to Main Menu", this);
    
    // Style buttons
    QString buttonStyle = 
        "QPushButton {"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "    padding: 15px 20px;"
        "    margin: 5px;"
        "    border: 2px solid #27ae60;"
        "    border-radius: 8px;"
        "    background-color: #535353ff;"
        // "    min-height: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "    color: white;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #219a52;"
        "}";
    
    QString backButtonStyle = 
        "QPushButton {"
        "    font-size: 12px;"
        "    padding: 15px 20px;"
        "    margin: 5px;"
        "    border: 2px solid #95a5a6;"
        "    border-radius: 8px;"
        "    background-color: #535353ff;"
        // "    min-height: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #95a5a6;"
        "    color: white;"
        "}";
    
    practiceButton->setStyleSheet(buttonStyle);
    practiceAllButton->setStyleSheet(buttonStyle);
    backButton->setStyleSheet(backButtonStyle);
    
    mainLayout->addWidget(practiceButton);
    mainLayout->addWidget(practiceAllButton);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(backButton);
    
    // Connect signals
    connect(practiceButton, &QPushButton::clicked, this, &VocabularySelectionDialog::onPracticeClicked);
    connect(practiceAllButton, &QPushButton::clicked, this, &VocabularySelectionDialog::onPracticeAllClicked);
    connect(backButton, &QPushButton::clicked, this, &VocabularySelectionDialog::onBackClicked);
    
    // Connect combo box selection
    connect(vocabularyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        selectedIndex = index;
        practiceButton->setEnabled(index >= 0);
    });
    
    // Enable/disable practice button based on selection
    practiceButton->setEnabled(!vocabularies.empty());
    practiceAllButton->setEnabled(!vocabularies.empty());
    
    // Select first item by default if available
    if (!vocabularies.empty()) {
        vocabularyComboBox->setCurrentIndex(0);
        selectedIndex = 0;
    }
}

void VocabularySelectionDialog::onPracticeClicked() {
    practiceAll = false;
    selectedIndex = vocabularyComboBox->currentIndex();
    if (selectedIndex >= 0) {
        accept();
    }
}

void VocabularySelectionDialog::populateVocabularyComboBox() {
    for (size_t i = 0; i < vocabularies.size(); ++i) {
        const Vocabulary &vocab = vocabularies[i];
        VocabularyScore score = VocabularyData::getProfileVocabularyScore(scores, profileName, vocab.name);
        
        // Create formatted text with scores
        QString itemText = QString("%1 (%2 words)")
                          .arg(vocab.name)
                          .arg(vocab.words.size());
        
        QString scoreText = "";
        if (score.bestRomajiPercent > 0 || score.bestEnglishPercent > 0) {
            QStringList scoreParts;
            if (score.bestRomajiPercent > 0) {
                scoreParts << QString("📝 %1%").arg(QString::number(score.bestRomajiPercent, 'f', 1));
            }
            if (score.bestEnglishPercent > 0) {
                scoreParts << QString("🇦🇺 %1%").arg(QString::number(score.bestEnglishPercent, 'f', 1));
            }
            scoreText = " - Best: " + scoreParts.join(", ");
        }
        
        vocabularyComboBox->addItem(itemText + scoreText);
    }
}

void VocabularySelectionDialog::onPracticeAllClicked() {
    practiceAll = true;
    selectedIndex = -1;
    accept();
}

void VocabularySelectionDialog::onBackClicked() {
    reject();
}