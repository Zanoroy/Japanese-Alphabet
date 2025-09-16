#include "ProfileDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

ProfileDialog::ProfileDialog(const QStringList &profiles, QWidget *parent) : QDialog(parent)
{
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/appicon.ico"));
#else
    setWindowIcon(QIcon(":/appicon.png"));
#endif
    setWindowTitle("Select or Create Profile");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Choose a profile or enter a new name:"));
    profileCombo = new QComboBox(this);
    profileCombo->addItems(profiles);
    profileCombo->setEditable(false);
    layout->addWidget(profileCombo);
    newProfileEdit = new QLineEdit(this);
    newProfileEdit->setPlaceholderText("Or enter new profile name");
    layout->addWidget(newProfileEdit);
    okButton = new QPushButton("OK", this);
    layout->addWidget(okButton);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    // Make dialog auto-size to minimum size that fits all contents
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    adjustSize();
}

QString ProfileDialog::selectedProfile() const {
    QString name = newProfileEdit->text().trimmed();
    if (!name.isEmpty()) return name;
    return profileCombo->currentText();
}
