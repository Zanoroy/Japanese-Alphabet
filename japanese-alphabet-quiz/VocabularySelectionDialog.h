#ifndef VOCABULARYSELECTIONDIALOG_H
#define VOCABULARYSELECTIONDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSpinBox>
#include "VocabularyData.h"

class VocabularySelectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit VocabularySelectionDialog(const std::vector<Vocabulary> &vocabularies, 
                                     const ProfileScores &scores,
                                     const QString &profileName,
                                     int initialMessageDurationSeconds = 2,
                                     QWidget *parent = nullptr);
    
    bool isPracticeAll() const { return practiceAll; }
    bool isPracticeSelected() const { return practiceSelected; }
    int getSelectedVocabularyIndex() const { return selectedIndex; }
    int getMessageDuration() const;
    std::vector<VocabularyWord> getSelectedWords() const;

private slots:
    void onPracticeClicked();
    void onPracticeAllClicked();
    void onPracticeSelectedClicked();
    void onSelectWordsClicked();
    void onBackClicked();

private:
    void populateVocabularyComboBox();
    void loadSelectedWords();
    
    QComboBox *vocabularyComboBox;
    QPushButton *practiceButton;
    QPushButton *practiceAllButton;
    QPushButton *practiceSelectedButton;
    QPushButton *selectWordsButton;
    QPushButton *backButton;
    QSpinBox *messageDurationSpinBox;
    
    bool practiceAll;
    bool practiceSelected;
    int selectedIndex;
    std::vector<VocabularyWord> selectedWords;
    const std::vector<Vocabulary> &vocabularies;
    const ProfileScores &scores;
    const QString &profileName;
    int initialMessageDurationSeconds;
};

#endif // VOCABULARYSELECTIONDIALOG_H