#ifndef VOCABULARYSELECTIONDIALOG_H
#define VOCABULARYSELECTIONDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "VocabularyData.h"

class VocabularySelectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit VocabularySelectionDialog(const std::vector<Vocabulary> &vocabularies, QWidget *parent = nullptr);
    
    bool isPracticeAll() const { return practiceAll; }
    int getSelectedVocabularyIndex() const { return selectedIndex; }

private slots:
    void onPracticeClicked();
    void onPracticeAllClicked();
    void onBackClicked();

private:
    QComboBox *vocabularyComboBox;
    QPushButton *practiceButton;
    QPushButton *practiceAllButton;
    QPushButton *backButton;
    
    bool practiceAll;
    int selectedIndex;
    const std::vector<Vocabulary> &vocabularies;
};

#endif // VOCABULARYSELECTIONDIALOG_H