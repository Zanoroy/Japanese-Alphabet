#ifndef QUIZWINDOW_H
#define QUIZWINDOW_H
#include <QWidget>
#include <QTableWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <vector>
#include "AlphabetData.h"

class QuizWindow : public QWidget {
    Q_OBJECT
public:
    explicit QuizWindow(QWidget *parent = nullptr);
    void savePreferences();
    void loadPreferences();
    void updateScore(int correct, int retries);
    void setChar(const QString &kana);
    void setFeedback(const QString &msg, bool error = false);
    void clearInput();
    void setInputEnabled(bool enabled);
    void highlightTableChar(const QString &kana, const QString &romaji, const QString &color);
    void resetTableHighlights();
    std::vector<bool> getRowChecks(const std::vector<QCheckBox*> &checks) const;
    QTableWidget *hiraganaTable, *katakanaTable, *kanjiTable;
    std::vector<QCheckBox*> hiraganaRowChecks, katakanaRowChecks, kanjiRowChecks;
    QCheckBox *hiraganaCB, *katakanaCB, *kanjiCB, *hideTablesCB;
    QSpinBox *timesSpin;
    QLabel *scoreLabel, *charLabel, *feedbackLabel, *timesLabel, *countdownLabel;
    void setCountdown(int remaining);
    QLineEdit *input;
    QString prefsFile;
    QFont scoreFont, charFont;
    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // QUIZWINDOW_H
