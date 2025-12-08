#include "VocabularySelectionDialog.h"
#include "WordSelectionDialog.h"
#include <QFont>
#include <QIcon>
#include <QComboBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFile>
#include <QDir>
#include <QSet>

VocabularySelectionDialog::VocabularySelectionDialog(const std::vector<Vocabulary> &vocabularies, 
                                                   const ProfileScores &scores,
                                                   const QString &profileName,
                                                   int initialMessageDurationSeconds,
                                                   QWidget *parent)
    : QDialog(parent), practiceAll(false), practiceSelected(false), selectedIndex(-1), vocabularies(vocabularies), scores(scores), profileName(profileName), initialMessageDurationSeconds(initialMessageDurationSeconds) {
    
    setWindowTitle("Select Vocabulary");
    setFixedSize(550, 500);
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

    populateVocabularyComboBox();
    
    // Layout for vocabulary selection and cog button
    QHBoxLayout *vocabLayout = new QHBoxLayout();
    vocabLayout->addWidget(vocabularyComboBox);
    
    // Add cog button for word selection
    selectWordsButton = new QPushButton(this);
    selectWordsButton->setText("⚙");  // Gear/cog character
    selectWordsButton->setToolTip("Select Words");
    selectWordsButton->setFixedSize(40, 30);
    selectWordsButton->setStyleSheet(
        "QPushButton {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    border: 2px solid #3498db;"
        "    border-radius: 6px;"
        "    background-color: #303030;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0980cf;"
        "    color: white;"
        "}"
    );
    
    vocabLayout->addWidget(selectWordsButton);
    mainLayout->addLayout(vocabLayout);
    
    mainLayout->addSpacing(15);
    
    // Message duration setting
    QLabel *durationLabel = new QLabel("Error/Comment Display Duration:", this);
    durationLabel->setStyleSheet("font-size: 14px; margin-bottom: 5px;");
    mainLayout->addWidget(durationLabel);
    
    QHBoxLayout *durationLayout = new QHBoxLayout();
    messageDurationSpinBox = new QSpinBox(this);
    messageDurationSpinBox->setRange(1, 10); // 1 to 10 seconds
    messageDurationSpinBox->setSingleStep(1);
    if (initialMessageDurationSeconds < 1) initialMessageDurationSeconds = 1;
    if (initialMessageDurationSeconds > 10) initialMessageDurationSeconds = 10;
    messageDurationSpinBox->setValue(initialMessageDurationSeconds); // Use stored preference
    messageDurationSpinBox->setSuffix(" seconds");
    messageDurationSpinBox->setStyleSheet(
        "QSpinBox {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 6px;"
        "    padding: 5px;"
        "    font-size: 12px;"
        "    min-width: 100px;"
        "}"
        "QSpinBox:hover {"
        "    border: 2px solid #3498db;"
        "}"
        "QSpinBox:focus {"
        "    border: 2px solid #3498db;"
        "}"
        /* Make any selected text appear unhighlighted (keeps original color) */
        "QSpinBox QLineEdit {"
        "    selection-background-color: transparent;"
        "    selection-color: #2c3e50;"
        "}"
    );
    
    QLabel *durationInfo = new QLabel("(How long error messages and comments stay visible)", this);
    durationInfo->setStyleSheet("font-size: 11px; color: #7f8c8d; font-style: italic;");
    
    durationLayout->addWidget(messageDurationSpinBox);
    durationLayout->addWidget(durationInfo);
    durationLayout->addStretch();
    
    mainLayout->addLayout(durationLayout);
    
    mainLayout->addSpacing(20);
    
    // Buttons
    practiceButton = new QPushButton("Practice Selected", this);
    practiceAllButton = new QPushButton("Practice All", this);
    practiceSelectedButton = new QPushButton("Practice Selected Words", this);
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
    practiceSelectedButton->setStyleSheet(buttonStyle);
    backButton->setStyleSheet(backButtonStyle);
    
    mainLayout->addWidget(practiceButton);
    mainLayout->addWidget(practiceAllButton);
    mainLayout->addWidget(practiceSelectedButton);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(backButton);
    
    // Connect signals
    connect(practiceButton, &QPushButton::clicked, this, &VocabularySelectionDialog::onPracticeClicked);
    connect(practiceAllButton, &QPushButton::clicked, this, &VocabularySelectionDialog::onPracticeAllClicked);
    connect(practiceSelectedButton, &QPushButton::clicked, this, &VocabularySelectionDialog::onPracticeSelectedClicked);
    connect(selectWordsButton, &QPushButton::clicked, this, &VocabularySelectionDialog::onSelectWordsClicked);
    connect(backButton, &QPushButton::clicked, this, &VocabularySelectionDialog::onBackClicked);
    
    // Connect combo box selection
    connect(vocabularyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        selectedIndex = index;
        practiceButton->setEnabled(index >= 0);
    });
    
    // Enable/disable practice button based on selection
    practiceButton->setEnabled(!vocabularies.empty());
    practiceAllButton->setEnabled(!vocabularies.empty());
    
    // Load selected words and enable/disable the practice selected button
    loadSelectedWords();
    practiceSelectedButton->setEnabled(!selectedWords.empty());
    
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
    practiceSelected = false;
    selectedIndex = -1;
    accept();
}

void VocabularySelectionDialog::onPracticeSelectedClicked() {
    practiceSelected = true;
    practiceAll = false;
    selectedIndex = -1;
    if (!selectedWords.empty()) {
        accept();
    }
}

void VocabularySelectionDialog::onSelectWordsClicked() {
    WordSelectionDialog dialog(vocabularies, profileName, this);
    if (dialog.exec() == QDialog::Accepted) {
        loadSelectedWords(); // Reload after selection dialog closes
        practiceSelectedButton->setEnabled(!selectedWords.empty());
    }
}

void VocabularySelectionDialog::onBackClicked() {
    reject();
}

int VocabularySelectionDialog::getMessageDuration() const {
    if (messageDurationSpinBox) {
        return messageDurationSpinBox->value();
    }
    return 2; // default seconds fallback
}

std::vector<VocabularyWord> VocabularySelectionDialog::getSelectedWords() const {
    return selectedWords;
}

void VocabularySelectionDialog::loadSelectedWords() {
    selectedWords.clear();
    
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
    
    // Find the selected words in vocabularies
    for (const Vocabulary &vocab : vocabularies) {
        for (const VocabularyWord &word : vocab.words) {
            QString keyWord = word.hiragana.isEmpty() ? (word.katakana.isEmpty() ? word.kanji : word.katakana) : word.hiragana;
            QString wordKey = QString("%1|%2|%3").arg(keyWord, word.romaji, word.english);
            if (selectedWordKeys.contains(wordKey)) {
                selectedWords.push_back(word);
            }
        }
    }
}