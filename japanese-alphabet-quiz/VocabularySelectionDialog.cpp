#include "VocabularySelectionDialog.h"
#include <QFont>
#include <QIcon>

VocabularySelectionDialog::VocabularySelectionDialog(const std::vector<Vocabulary> &vocabularies, QWidget *parent)
    : QDialog(parent), practiceAll(false), selectedIndex(-1), vocabularies(vocabularies) {
    
    setWindowTitle("Select Vocabulary");
    setFixedSize(400, 340);
    
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/appicon.ico"));
#else
    setWindowIcon(QIcon(":/appicon.png"));
#endif

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
    mainLayout->addWidget(selectLabel);
    
    vocabularyComboBox = new QComboBox(this);
    for (const Vocabulary &vocab : vocabularies) {
        vocabularyComboBox->addItem(QString("%1 (%2 words)").arg(vocab.name).arg(vocab.words.size()));
    }
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
    
    // Enable/disable practice button based on selection
    practiceButton->setEnabled(!vocabularies.empty());
    practiceAllButton->setEnabled(!vocabularies.empty());
}

void VocabularySelectionDialog::onPracticeClicked() {
    practiceAll = false;
    selectedIndex = vocabularyComboBox->currentIndex();
    accept();
}

void VocabularySelectionDialog::onPracticeAllClicked() {
    practiceAll = true;
    selectedIndex = -1;
    accept();
}

void VocabularySelectionDialog::onBackClicked() {
    reject();
}