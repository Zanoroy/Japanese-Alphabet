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
    explicit VocabularyQuizWindow(const std::vector<VocabularyWord> &words, QWidget *parent = nullptr);

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
    void showError(const QString &message, const QString &correctAnswer = "");
    void updateScore();
    void showResults();

    // UI elements
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QCheckBox *romajiCheckbox;
    QCheckBox *englishCheckbox;
    QPushButton *startButton;
    QLabel *questionLabel;
    QLineEdit *answerInput;
    QPushButton *checkButton;
    QLabel *errorLabel;
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
    
    // Timer for error display
    QTimer *errorTimer;
    bool quizStarted;
};

#endif // VOCABULARYQUIZWINDOW_H