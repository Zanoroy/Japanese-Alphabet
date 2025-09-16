#ifndef QUIZGAME_H
#define QUIZGAME_H
#include <QObject>
#include <vector>
#include <set>
#include <map>
#include "QuizWindow.h"
#include "AlphabetData.h"

class QuizGame : public QObject {
    Q_OBJECT
public:
    explicit QuizGame(QuizWindow *window);
    void newQuiz();
    void newQuestion(bool excludeCurrent = false);
    void checkAnswer();
    void handleScriptCheckbox(const QString &script, int state);
    void handleRowCheckbox(const QString &script, QCheckBox *cb, int state);
    void updateScore();
    void showSummaryAndReset(const std::vector<std::pair<QString, QString>> &allChars);
    std::vector<std::vector<KanaEntry>> getEnabledAlphabets() const;
private:
    QuizWindow *window;
    int correctCount = 0;
    int retryCount = 0;
    QString currentKana, currentRomaji;
    std::vector<std::pair<QString, QString>> roundChars;
    std::vector<std::pair<QString, QString>> unansweredChars;
    std::set<std::pair<QString, QString>> answeredOnce;
    std::map<std::pair<QString, QString>, int> charStatsCorrect;
    std::map<std::pair<QString, QString>, int> charStatsIncorrect;
    int timesToShow = 1;
};

#endif // QUIZGAME_H
