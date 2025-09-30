
#include <QApplication>
#include <QIcon>
#include "QuizWindow.h"
#include "QuizGame.h"
#include "ProfileDialog.h"
#include "MainMenuDialog.h"
#include "VocabularySelectionDialog.h"
#include "VocabularyData.h"
#include "VocabularyQuizWindow.h"
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
#ifdef Q_OS_WIN
    QApplication::setWindowIcon(QIcon(":/appicon.ico"));
#else
    QApplication::setWindowIcon(QIcon(":/appicon.png"));
#endif

    // Splash screen
    QPixmap splashPixmap(":/appicon.png");
    QSplashScreen splash(splashPixmap.scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    splash.show();
    app.processEvents();

    // Profile selection and initialization while splash is visible
    QString profilesDir = QDir::currentPath() + "/profiles";
    QDir().mkpath(profilesDir);
    QStringList profiles;
    for (const QString &file : QDir(profilesDir).entryList(QStringList() << "*.json", QDir::Files)) {
        QString baseName = QFileInfo(file).baseName();
        // Skip system files that are not user profiles
        if (baseName != "vocabularies" && baseName != "vocabulary_scores") {
            profiles << baseName;
        }
    }
    ProfileDialog profileDialog(profiles);
    int dialogResult = profileDialog.exec();
    QString profileName = profileDialog.selectedProfile();
    if (dialogResult != QDialog::Accepted || profileName.isEmpty()) {
        splash.close();
        return 0;
    }

    splash.close();

    // Main application loop
    while (true) {
        // Show main menu
        MainMenuDialog mainMenu(profileName);
        if (mainMenu.exec() != QDialog::Accepted) {
            return 0; // User chose exit or closed dialog
        }

        MainMenuDialog::MenuChoice choice = mainMenu.getChoice();
        
        if (choice == MainMenuDialog::Exit) {
            return 0;
        }
        else if (choice == MainMenuDialog::AlphabetQuiz) {
            // Start alphabet quiz
            QuizWindow window;
            window.prefsFile = profilesDir + "/" + profileName + ".json";
            window.loadPreferences();
            QuizGame game(&window);
            window.show();
            app.exec();
            // After quiz window closes, return to main menu
        }
        else if (choice == MainMenuDialog::Vocabularies) {
            // Load vocabularies
            std::vector<Vocabulary> vocabularies;
            QString vocabFile = profilesDir + "/vocabularies.json";
            VocabularyData::loadVocabularies(vocabFile, vocabularies);
            
            if (vocabularies.empty()) {
                // Show message and return to main menu
                continue;
            }
            
            // Load profile scores
            ProfileScores profileScores;
            QString scoresFile = profilesDir + "/vocabulary_scores.json";
            VocabularyData::loadProfileScores(scoresFile, profileScores);
            
            // Load user preferences (message duration) from profile JSON
            int initialMessageDurationSeconds = 2; // fallback default
            bool initialShowCommentsOnCorrect = true; // default show
            {
                QFile profileFile(profilesDir + "/" + profileName + ".json");
                if (profileFile.open(QIODevice::ReadOnly)) {
                    QByteArray data = profileFile.readAll();
                    profileFile.close();
                    QJsonParseError parseError; 
                    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
                    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
                        QJsonObject obj = doc.object();
                        if (obj.contains("messageDurationSeconds") && obj["messageDurationSeconds"].isDouble()) {
                            initialMessageDurationSeconds = obj["messageDurationSeconds"].toInt();
                        }
                        if (obj.contains("showCommentsOnCorrect") && obj["showCommentsOnCorrect"].isBool()) {
                            initialShowCommentsOnCorrect = obj["showCommentsOnCorrect"].toBool();
                        }
                    }
                }
            }
            
            // Show vocabulary selection dialog (comment preference now handled only inside quiz window)
            VocabularySelectionDialog vocabDialog(vocabularies, profileScores, profileName, initialMessageDurationSeconds);
            if (vocabDialog.exec() == QDialog::Accepted) {
                // Get selected vocabulary words
                std::vector<VocabularyWord> wordsToQuiz;
                int messageDuration = vocabDialog.getMessageDuration();
                bool showCommentsOnCorrect = initialShowCommentsOnCorrect; // still pass stored preference
                // Persist updated preference back to profile JSON
                {
                    QFile profileFile(profilesDir + "/" + profileName + ".json");
                    QJsonObject obj;
                    if (profileFile.open(QIODevice::ReadOnly)) {
                        QByteArray data = profileFile.readAll();
                        profileFile.close();
                        QJsonParseError parseError; 
                        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
                        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
                            obj = doc.object();
                        }
                    }
                    obj["messageDurationSeconds"] = messageDuration; // store selection
                    obj["showCommentsOnCorrect"] = showCommentsOnCorrect;
                    if (profileFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                        QJsonDocument outDoc(obj);
                        profileFile.write(outDoc.toJson(QJsonDocument::Indented));
                        profileFile.close();
                    }
                }
                
                if (vocabDialog.isPracticeAll()) {
                    // Practice all vocabularies
                    wordsToQuiz = VocabularyData::getAllWords(vocabularies);
                } else {
                    // Practice selected vocabulary
                    int selectedIndex = vocabDialog.getSelectedVocabularyIndex();
                    if (selectedIndex >= 0 && selectedIndex < vocabularies.size()) {
                        wordsToQuiz = vocabularies[selectedIndex].words;
                    }
                }
                
                if (!wordsToQuiz.empty()) {
                    // Determine vocabulary name for score tracking
                    QString vocabName = "All Vocabularies";
                    if (!vocabDialog.isPracticeAll()) {
                        int selectedIndex = vocabDialog.getSelectedVocabularyIndex();
                        if (selectedIndex >= 0 && selectedIndex < vocabularies.size()) {
                            vocabName = vocabularies[selectedIndex].name;
                        }
                    }
                    
                    // Start vocabulary quiz
                    VocabularyQuizWindow *vocabQuiz = new VocabularyQuizWindow(
                        wordsToQuiz, profileName, vocabName, scoresFile, messageDuration);
                    vocabQuiz->setShowCommentsOnCorrect(showCommentsOnCorrect);
                    vocabQuiz->show();
                    vocabQuiz->setAttribute(Qt::WA_DeleteOnClose);
                    
                    // Connect signals to handle quiz completion
                    QObject::connect(vocabQuiz, &VocabularyQuizWindow::returnToMenuRequested, [&]() {
                        // Return to main menu will happen when the while loop continues
                    });
                    
                    // Run event loop until vocabulary quiz is closed
                    while (vocabQuiz && vocabQuiz->isVisible()) {
                        app.processEvents();
                        QThread::msleep(10);
                    }
                }
            }
            // Return to main menu after vocabulary selection
        }
    }
}
