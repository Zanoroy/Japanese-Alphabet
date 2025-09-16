
#include <QApplication>
#include <QIcon>
#include "QuizWindow.h"
#include "QuizGame.h"
#include "ProfileDialog.h"
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <QDir>
#include <QDebug>

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

    // Wait 5 seconds
    QTimer::singleShot(5000, &splash, &QSplashScreen::close);
    QTimer::singleShot(5000, &app, SLOT(quit()));
    app.exec();

    // Profile selection
    QString profilesDir = QDir::currentPath() + "/profiles";
    QDir().mkpath(profilesDir);
    QStringList profiles;
    for (const QString &file : QDir(profilesDir).entryList(QStringList() << "*.json", QDir::Files)) {
        profiles << QFileInfo(file).baseName();
    }
    ProfileDialog profileDialog(profiles);
    if (profileDialog.exec() != QDialog::Accepted) return 0;
    QString profileName = profileDialog.selectedProfile();
    if (profileName.isEmpty()) return 0;

    // Use profile-specific preferences file
    QuizWindow window;
    window.prefsFile = profilesDir + "/" + profileName + ".json";
    window.loadPreferences();
    QuizGame game(&window);
    window.show();
    return app.exec();
}
