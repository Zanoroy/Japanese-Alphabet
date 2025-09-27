#ifndef MAINMENUDIALOG_H
#define MAINMENUDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class MainMenuDialog : public QDialog {
    Q_OBJECT

public:
    enum MenuChoice {
        AlphabetQuiz,
        Vocabularies,
        Exit
    };

    explicit MainMenuDialog(const QString &profileName, QWidget *parent = nullptr);
    MenuChoice getChoice() const { return choice; }

private slots:
    void onAlphabetQuizClicked();
    void onVocabulariesClicked();
    void onExitClicked();

private:
    MenuChoice choice;
    QPushButton *alphabetQuizButton;
    QPushButton *vocabulariesButton;
    QPushButton *exitButton;
};

#endif // MAINMENUDIALOG_H