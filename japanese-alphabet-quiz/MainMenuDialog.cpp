#include "MainMenuDialog.h"
#include <QFont>
#include <QIcon>

MainMenuDialog::MainMenuDialog(const QString &profileName, QWidget *parent)
    : QDialog(parent), choice(Exit) {
    
    setWindowTitle("Japanese Learning - Main Menu");
    setFixedSize(400, 340);
    
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/appicon.ico"));
#else
    setWindowIcon(QIcon(":/appicon.png"));
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Welcome label
    QLabel *welcomeLabel = new QLabel(QString("Welcome, %1!").arg(profileName), this);
    QFont welcomeFont;
    welcomeFont.setPointSize(16);
    welcomeFont.setBold(true);
    welcomeLabel->setFont(welcomeFont);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(welcomeLabel);
    
    mainLayout->addSpacing(20);
    
    // Instructions
    QLabel *instructionLabel = new QLabel("What would you like to practice today?", this);
    QFont instructionFont;
    instructionFont.setPointSize(12);
    instructionLabel->setFont(instructionFont);
    instructionLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(instructionLabel);
    
    mainLayout->addSpacing(30);
    
    // Buttons
    alphabetQuizButton = new QPushButton("Alphabet Quiz", this);
    vocabulariesButton = new QPushButton("Vocabularies", this);
    exitButton = new QPushButton("Exit", this);
    
    // Style buttons
    QString buttonStyle = 
        "QPushButton {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    padding: 10px 20px;"
        "    margin: 5px;"
        "    border: 2px solid #3498db;"
        "    border-radius: 8px;"
        "    background-color: #535353ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: #3498db;"
        "    color: white;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #2980b9;"
        "}";
    
    alphabetQuizButton->setStyleSheet(buttonStyle);
    vocabulariesButton->setStyleSheet(buttonStyle);
    exitButton->setStyleSheet(buttonStyle);
    
    mainLayout->addWidget(alphabetQuizButton);
    mainLayout->addWidget(vocabulariesButton);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(exitButton);
    
    mainLayout->addStretch();
    
    // Connect signals
    connect(alphabetQuizButton, &QPushButton::clicked, this, &MainMenuDialog::onAlphabetQuizClicked);
    connect(vocabulariesButton, &QPushButton::clicked, this, &MainMenuDialog::onVocabulariesClicked);
    connect(exitButton, &QPushButton::clicked, this, &MainMenuDialog::onExitClicked);
}

void MainMenuDialog::onAlphabetQuizClicked() {
    choice = AlphabetQuiz;
    accept();
}

void MainMenuDialog::onVocabulariesClicked() {
    choice = Vocabularies;
    accept();
}

void MainMenuDialog::onExitClicked() {
    choice = Exit;
    reject();
}