#include "WordSelectionDialog.h"
#include <QFont>
#include <QIcon>
#include <QMessageBox>
#include <QJsonParseError>

WordSelectionDialog::WordSelectionDialog(const std::vector<Vocabulary> &vocabularies,
                                        const QString &profileName,
                                        QWidget *parent)
    : QDialog(parent), vocabularies(vocabularies), profileName(profileName) {
    
    setWindowTitle("Select Words to Practice");
    setFixedSize(800, 600);
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/appicon.ico"));
#else
    setWindowIcon(QIcon(":/appicon.png"));
#endif
    
    setupUI();
    populateWords();
    loadSelectedWords();
}

void WordSelectionDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *titleLabel = new QLabel("Select Words to Practice", this);
    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    mainLayout->addSpacing(15);
    
    // Instruction
    QLabel *instructionLabel = new QLabel("Check the words you want to include in your custom practice session:", this);
    instructionLabel->setStyleSheet("font-size: 12px; color: #7f8c8d;");
    instructionLabel->setWordWrap(true);
    mainLayout->addWidget(instructionLabel);
    
    mainLayout->addSpacing(10);
    
    // Select/Clear buttons
    QHBoxLayout *selectButtonsLayout = new QHBoxLayout();
    selectAllButton = new QPushButton("Select All", this);
    clearAllButton = new QPushButton("Clear All", this);
    
    QString selectButtonStyle = 
        "QPushButton {"
        "    font-size: 11px;"
        "    padding: 8px 15px;"
        "    margin: 2px;"
        "    border: 2px solid #3498db;"
        "    border-radius: 6px;"
        "    background-color: #535353ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: #3498db;"
        "    color: white;"
        "}";
    
    selectAllButton->setStyleSheet(selectButtonStyle);
    clearAllButton->setStyleSheet(selectButtonStyle);
    
    selectButtonsLayout->addWidget(selectAllButton);
    selectButtonsLayout->addWidget(clearAllButton);
    selectButtonsLayout->addStretch();
    mainLayout->addLayout(selectButtonsLayout);
    
    mainLayout->addSpacing(10);
    
    // Scroll area for words
    scrollArea = new QScrollArea(this);
    scrollWidget = new QWidget();
    scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setAlignment(Qt::AlignTop);
    
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "}"
    );
    
    mainLayout->addWidget(scrollArea);
    
    mainLayout->addSpacing(15);
    
    // OK/Cancel buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);
    
    QString okButtonStyle = 
        "QPushButton {"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "    padding: 12px 25px;"
        "    margin: 5px;"
        "    border: 2px solid #27ae60;"
        "    border-radius: 8px;"
        "    background-color: #535353ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: #27ae60;"
        "    color: white;"
        "}";
    
    QString cancelButtonStyle = 
        "QPushButton {"
        "    font-size: 12px;"
        "    padding: 12px 25px;"
        "    margin: 5px;"
        "    border: 2px solid #95a5a6;"
        "    border-radius: 8px;"
        "    background-color: #535353ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: #95a5a6;"
        "    color: white;"
        "}";
    
    okButton->setStyleSheet(okButtonStyle);
    cancelButton->setStyleSheet(cancelButtonStyle);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(selectAllButton, &QPushButton::clicked, this, &WordSelectionDialog::onSelectAllClicked);
    connect(clearAllButton, &QPushButton::clicked, this, &WordSelectionDialog::onClearAllClicked);
    connect(okButton, &QPushButton::clicked, this, &WordSelectionDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &WordSelectionDialog::onCancelClicked);
}

void WordSelectionDialog::populateWords() {
    for (const Vocabulary &vocab : vocabularies) {
        // Create group box for each vocabulary
        QGroupBox *groupBox = new QGroupBox(vocab.name, scrollWidget);
        QVBoxLayout *groupLayout = new QVBoxLayout(groupBox);
        
        groupBox->setStyleSheet(
            "QGroupBox {"
            "    font-weight: bold;"
            "    border: 2px solid #95a5a6;"
            "    border-radius: 8px;"
            "    margin-top: 1ex;"
            "    padding-top: 10px;"
            "}"
            "QGroupBox::title {"
            "    subcontrol-origin: margin;"
            "    left: 10px;"
            "    padding: 0 5px 0 5px;"
            "}"
        );
        
        for (const VocabularyWord &word : vocab.words) {
            QCheckBox *checkBox = new QCheckBox(
                QString("%1 (%2) - %3").arg(word.hiragana.isEmpty() ? (word.katakana.isEmpty() ? word.kanji : word.katakana) : word.hiragana, word.romaji, word.english), 
                groupBox
            );
            
            checkBox->setStyleSheet(
                "QCheckBox {"
                "    font-size: 11px;"
                "    padding: 2px;"
                "}"
                "QCheckBox::indicator {"
                "    width: 16px;"
                "    height: 16px;"
                "}"
            );
            
            groupLayout->addWidget(checkBox);
            
            WordCheckBox wordCB;
            wordCB.checkBox = checkBox;
            wordCB.word = word;
            wordCB.vocabularyName = vocab.name;
            wordCheckBoxes.push_back(wordCB);
        }
        
        scrollLayout->addWidget(groupBox);
    }
}

std::vector<VocabularyWord> WordSelectionDialog::getSelectedWords() const {
    std::vector<VocabularyWord> selectedWords;
    
    for (const WordCheckBox &wordCB : wordCheckBoxes) {
        if (wordCB.checkBox->isChecked()) {
            selectedWords.push_back(wordCB.word);
        }
    }
    
    return selectedWords;
}

void WordSelectionDialog::loadSelectedWords() {
    QString profilePath = QDir::currentPath() + "/profiles/" + profileName + ".json";
    QFile profileFile(profilePath);
    
    if (!profileFile.open(QIODevice::ReadOnly)) {
        return; // No saved selection
    }
    
    QByteArray data = profileFile.readAll();
    profileFile.close();
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return;
    }
    
    QJsonObject obj = doc.object();
    if (!obj.contains("selectedWords") || !obj["selectedWords"].isArray()) {
        return;
    }
    
    QJsonArray selectedWordsArray = obj["selectedWords"].toArray();
    
    // Create a set of selected word keys for quick lookup
    QSet<QString> selectedWordKeys;
    for (const QJsonValue &value : selectedWordsArray) {
        if (value.isString()) {
            selectedWordKeys.insert(value.toString());
        }
    }
    
    // Check the checkboxes for selected words
    for (const WordCheckBox &wordCB : wordCheckBoxes) {
        QString keyWord = wordCB.word.hiragana.isEmpty() ? (wordCB.word.katakana.isEmpty() ? wordCB.word.kanji : wordCB.word.katakana) : wordCB.word.hiragana;
        QString wordKey = QString("%1|%2|%3").arg(keyWord, wordCB.word.romaji, wordCB.word.english);
        if (selectedWordKeys.contains(wordKey)) {
            wordCB.checkBox->setChecked(true);
        }
    }
}

void WordSelectionDialog::saveSelectedWords() {
    QString profilePath = QDir::currentPath() + "/profiles/" + profileName + ".json";
    QFile profileFile(profilePath);
    
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
    
    // Create array of selected words
    QJsonArray selectedWordsArray;
    for (const WordCheckBox &wordCB : wordCheckBoxes) {
        if (wordCB.checkBox->isChecked()) {
            QString keyWord = wordCB.word.hiragana.isEmpty() ? (wordCB.word.katakana.isEmpty() ? wordCB.word.kanji : wordCB.word.katakana) : wordCB.word.hiragana;
            QString wordKey = QString("%1|%2|%3").arg(keyWord, wordCB.word.romaji, wordCB.word.english);
            selectedWordsArray.append(wordKey);
        }
    }
    
    obj["selectedWords"] = selectedWordsArray;
    
    if (profileFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument outDoc(obj);
        profileFile.write(outDoc.toJson(QJsonDocument::Indented));
        profileFile.close();
    }
}

void WordSelectionDialog::onSelectAllClicked() {
    for (const WordCheckBox &wordCB : wordCheckBoxes) {
        wordCB.checkBox->setChecked(true);
    }
}

void WordSelectionDialog::onClearAllClicked() {
    for (const WordCheckBox &wordCB : wordCheckBoxes) {
        wordCB.checkBox->setChecked(false);
    }
}

void WordSelectionDialog::onOkClicked() {
    saveSelectedWords();
    accept();
}

void WordSelectionDialog::onCancelClicked() {
    reject();
}