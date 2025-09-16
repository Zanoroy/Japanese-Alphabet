
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

    // Use profile-specific preferences file
    QuizWindow window;
    window.prefsFile = profilesDir + "/" + profileName + ".json";
    window.loadPreferences();
    QuizGame game(&window);

    // Keep splash for a minimum time (e.g. 1s), then show main window
    QTimer::singleShot(1000, [&]() {
        splash.close();
        window.show();
    });

    return app.exec();
}
