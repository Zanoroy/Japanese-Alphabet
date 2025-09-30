#include "FeedbackDialog.h"

FeedbackDialog::FeedbackDialog(const QString &primaryText,
                               const QString &commentText,
                               int autoCloseSeconds,
                               QWidget *parent)
    : QDialog(parent), countdown(autoCloseSeconds) {
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setModal(true);
    setMinimumWidth(420);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *primaryLabel = new QLabel(this);
    primaryLabel->setWordWrap(true);
    primaryLabel->setTextFormat(Qt::RichText);
    primaryLabel->setText(primaryText);
    primaryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    primaryLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(primaryLabel);

    if (!commentText.isEmpty()) {
        QLabel *commentLabel = new QLabel(this);
        commentLabel->setWordWrap(true);
        commentLabel->setTextFormat(Qt::RichText);
        commentLabel->setOpenExternalLinks(true);
        commentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
        commentLabel->setText(commentText);
        commentLabel->setStyleSheet("font-size: 13px; color: #2c3e50; background: #ecf0f1; padding: 8px; border-radius: 6px;");
        layout->addWidget(commentLabel);
    }

    countdownLabel = new QLabel(this);
    countdownLabel->setStyleSheet("font-size: 12px; color: #7f8c8d; margin-top: 4px;");
    layout->addWidget(countdownLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    continueButton = new QPushButton("Continue", this);
    continueButton->setDefault(true);
    continueButton->setStyleSheet(
        "QPushButton { font-size: 14px; padding: 8px 22px; background-color: #3498db; color: white; border: none; border-radius: 6px; }"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:pressed { background-color: #21618c; }"
    );
    buttonLayout->addWidget(continueButton);
    layout->addLayout(buttonLayout);

    connect(continueButton, &QPushButton::clicked, this, &QDialog::accept);

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &FeedbackDialog::onTick);

    updateCountdownLabel();
    timer->start();
}

void FeedbackDialog::onTick() {
    if (countdown <= 1) {
        timer->stop();
        emit autoClosed();
        accept();
        return;
    }
    countdown--;
    updateCountdownLabel();
}

void FeedbackDialog::updateCountdownLabel() {
    countdownLabel->setText(QString("Automatically continuing in %1 second%2...")
                             .arg(countdown)
                             .arg(countdown == 1 ? "" : "s"));
}
