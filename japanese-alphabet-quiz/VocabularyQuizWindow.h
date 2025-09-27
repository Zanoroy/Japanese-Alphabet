#ifndef VOCABULARYQUIZWINDOW_H
#define VOCABULARYQUIZWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTimer>
#include <map>
#include "VocabularyData.h"

class VocabularyQuizWindow : public QWidget {
    Q_OBJECT

public:
    explicit VocabularyQuizWindow(const std::vector<VocabularyWord> &words, const QString &profileName, const QString &vocabularyName, const QString &scoresFilePath, QWidget *parent = nullptr);
    void resetQuiz();

signals:
    void quizCompleted();
    void returnToMenuRequested();

private slots:
    void onRomajiCheckboxChanged(int state);
    void onEnglishCheckboxChanged(int state);
    void onStartClicked();
    void onCheckAnswer();
    void hideErrorMessage();
    void showNextWord();

private:
    void setupUI();
    void updateCheckboxStates();
    void checkRomajiAnswer();
    void checkEnglishAnswer();
    void showError(const QString &message, const QString &correctAnswer = "", const QString &comment = "");
    void showComment(const QString &comment);
    void updateScore();
    void showResults();

    // UI elements
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QHBoxLayout *checkboxLayout;
    QCheckBox *romajiCheckbox;
    QCheckBox *englishCheckbox;
    QPushButton *startButton;
    QLabel *questionLabel;
    QLabel *instructionLabel;
    QLineEdit *answerInput;
    QPushButton *checkButton;
    QLabel *errorLabel;
    QLabel *commentLabel;
    QLabel *scoreLabel;
    QPushButton *backButton;

    // Quiz data
    std::vector<VocabularyWord> vocabularyWords;
    int currentWordIndex;
    bool expectingRomaji;
    bool expectingEnglish;
    
    // Statistics
    int correctRomajiCount;
    int incorrectRomajiCount;
    int correctEnglishCount;
    int incorrectEnglishCount;
    std::map<VocabularyWord*, int> incorrectWords; // Track incorrect attempts per word

    // Profile and scoring data
    QString profileName;
    QString vocabularyName;
    QString scoresFilePath;

    // Timer for message/error display
    QTimer *msgTimer;
    bool quizStarted;
};

#endif // VOCABULARYQUIZWINDOW_H