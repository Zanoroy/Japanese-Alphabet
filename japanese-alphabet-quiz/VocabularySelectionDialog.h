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
#include <QCheckBox>
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
    bool getShowCommentsOnCorrect() const { return showCommentsOnCorrectCheckBox ? showCommentsOnCorrectCheckBox->isChecked() : true; }

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
    QCheckBox *showCommentsOnCorrectCheckBox;
    
    bool practiceAll;
    int selectedIndex;
    const std::vector<Vocabulary> &vocabularies;
    const ProfileScores &scores;
    const QString &profileName;
    int initialMessageDurationSeconds;
    bool initialShowCommentsOnCorrect { true };
};

#endif // VOCABULARYSELECTIONDIALOG_H