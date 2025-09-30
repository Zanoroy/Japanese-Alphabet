#ifndef FEEDBACKDIALOG_H
#define FEEDBACKDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>

class FeedbackDialog : public QDialog {
    Q_OBJECT
public:
    explicit FeedbackDialog(const QString &primaryText,
                            const QString &commentText,
                            int autoCloseSeconds,
                            QWidget *parent = nullptr);

signals:
    void autoClosed();

private slots:
    void onTick();

private:
    void updateCountdownLabel();
    QPushButton *continueButton{nullptr};
    QLabel *countdownLabel{nullptr};
    QTimer *timer{nullptr};
    int countdown{0};
};

#endif // FEEDBACKDIALOG_H
