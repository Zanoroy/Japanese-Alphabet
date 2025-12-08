#ifndef WORDSELECTIONDIALOG_H
#define WORDSELECTIONDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QCheckBox>
#include <QGroupBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include "VocabularyData.h"

class WordSelectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit WordSelectionDialog(const std::vector<Vocabulary> &vocabularies,
                                const QString &profileName,
                                QWidget *parent = nullptr);
    
    std::vector<VocabularyWord> getSelectedWords() const;
    void loadSelectedWords();
    void saveSelectedWords();

private slots:
    void onSelectAllClicked();
    void onClearAllClicked();
    void onOkClicked();
    void onCancelClicked();

private:
    void setupUI();
    void populateWords();
    
    const std::vector<Vocabulary> &vocabularies;
    const QString &profileName;
    
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVBoxLayout *scrollLayout;
    
    QPushButton *selectAllButton;
    QPushButton *clearAllButton;
    QPushButton *okButton;
    QPushButton *cancelButton;
    
    struct WordCheckBox {
        QCheckBox *checkBox;
        VocabularyWord word;
        QString vocabularyName;
    };
    
    std::vector<WordCheckBox> wordCheckBoxes;
};

#endif // WORDSELECTIONDIALOG_H