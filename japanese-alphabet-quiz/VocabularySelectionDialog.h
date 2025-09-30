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
    int getSelectedVocabularyIndex() const { return selectedIndex; }
    int getMessageDuration() const;
    // Show-comments preference removed; handled entirely inside quiz window settings now.

private slots:
    void onPracticeClicked();
    void onPracticeAllClicked();
    void onBackClicked();

private:
    void populateVocabularyComboBox();
    
    QComboBox *vocabularyComboBox;
    QPushButton *practiceButton;
    QPushButton *practiceAllButton;
    QPushButton *backButton;
    QSpinBox *messageDurationSpinBox;
    
    bool practiceAll;
    int selectedIndex;
    const std::vector<Vocabulary> &vocabularies;
    const ProfileScores &scores;
    const QString &profileName;
    int initialMessageDurationSeconds;
};

#endif // VOCABULARYSELECTIONDIALOG_H