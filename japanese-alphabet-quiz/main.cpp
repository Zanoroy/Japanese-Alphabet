
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
        profiles << QFileInfo(file).baseName();
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
            
            // Show vocabulary selection dialog
            VocabularySelectionDialog vocabDialog(vocabularies);
            if (vocabDialog.exec() == QDialog::Accepted) {
                // Get selected vocabulary words
                std::vector<VocabularyWord> wordsToQuiz;
                
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
                    // Start vocabulary quiz
                    VocabularyQuizWindow *vocabQuiz = new VocabularyQuizWindow(wordsToQuiz);
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
