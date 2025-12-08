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
#include <QMessageBox>
#include <QToolButton>
#include <QMenu>
#include <map>
#include "VocabularyData.h"

class VocabularyQuizWindow : public QWidget {
    Q_OBJECT

public:
    explicit VocabularyQuizWindow(const std::vector<VocabularyWord> &words, const QString &profileName, const QString &vocabularyName, const QString &scoresFilePath, int messageDuration = 2, QWidget *parent = nullptr);
    void setShowCommentsOnCorrect(bool enabled) { showCommentsOnCorrect = enabled; }
    void resetQuiz();

signals:
    void quizCompleted();
    void returnToMenuRequested();

private slots:
    void onRomajiCheckboxChanged(int state);
    void onEnglishCheckboxChanged(int state);
    void onStartClicked();
    void onCheckAnswer();
    void onHintClicked();
    void hideErrorMessage();
    void showNextWord();
    void onMessageTimeout();

private:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void setupUI();
    void updateCheckboxStates();
    void checkRomajiAnswer();
    void checkEnglishAnswer();
    void showError(const QString &correctAnswer = "", const QString &comment = "");
    void showComment(const QString &comment);
    void updateScore();
    void showResults();
    void advanceAfterMessage();

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
    QPushButton *hintButton;
    QLabel *scoreLabel;
    QPushButton *backButton;
    QToolButton *settingsButton;
    QMenu *settingsMenu;
    QAction *toggleCommentsAction;

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
    int hintCount; // Track hints used during quiz
    std::map<VocabularyWord*, int> incorrectWords; // Track incorrect attempts per word

    // Profile and scoring data
    QString profileName;
    QString vocabularyName;
    QString scoresFilePath;

    int messageDuration; // Duration to show messages in seconds
    bool quizStarted;
    bool showCommentsOnCorrect { true };
    bool initialExpectRomaji { true };
    bool initialExpectEnglish { true };
};

#endif // VOCABULARYQUIZWINDOW_H