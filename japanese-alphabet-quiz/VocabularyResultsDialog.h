#ifndef VOCABULARYRESULTSDIALOG_H
#define VOCABULARYRESULTSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <map>
#include "VocabularyData.h"

class VocabularyResultsDialog : public QDialog {
    Q_OBJECT

public:
    enum ResultChoice {
        TryAgain,
        ReturnToMenu
    };

    explicit VocabularyResultsDialog(
        bool expectingRomaji,
        bool expectingEnglish,
        int correctRomajiCount,
        int incorrectRomajiCount,
        int correctEnglishCount,
        int incorrectEnglishCount,
        const std::map<VocabularyWord*, int> &incorrectWords,
        QWidget *parent = nullptr
    );

    ResultChoice getChoice() const { return choice; }

private slots:
    void onTryAgainClicked();
    void onReturnToMenuClicked();

private:
    ResultChoice choice;
    QPushButton *tryAgainButton;
    QPushButton *returnToMenuButton;
};

#endif // VOCABULARYRESULTSDIALOG_H