#pragma once
#include <QDialog>
#include <QStringList>

class QComboBox;
class QLineEdit;
class QPushButton;

class ProfileDialog : public QDialog {
    Q_OBJECT
public:
    ProfileDialog(const QStringList &profiles, QWidget *parent = nullptr);
    QString selectedProfile() const;
private:
    QComboBox *profileCombo;
    QLineEdit *newProfileEdit;
    QPushButton *okButton;
};
