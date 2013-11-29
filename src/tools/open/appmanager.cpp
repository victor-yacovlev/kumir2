#include "appmanager.h"
#include "procmanager.h"
#include "messager.h"
#include "settings.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>


ApplicationManager & ApplicationManager::get()
{
    static ApplicationManager manager;
    return manager;
}

ApplicationManager::ApplicationManager()
{
    const QString appsDir =
            QDir::cleanPath(QApplication::applicationDirPath() +
                            "/../share/applications");
    const QString iconsDir =
            QDir::cleanPath(QApplication::applicationDirPath() +
                            "/../share/icons/hicolor");
    scanForApplications(appsDir, iconsDir);
}

QList<Application> ApplicationManager::applications() const
{
    QList<Application> result;
    foreach (const ApplicationPrivate & app_p, applications_) {
        Application app;
        app.name = app_p.name;
        app.icon = app_p.icon;
        app.id = quintptr(&app_p);
        app.key = QFileInfo(app_p.executableFileName).baseName();
        result.push_back(app);
    }
    return result;
}

void ApplicationManager::scanForApplications(const QString &appsDirPath,
                                             const QString &iconsDirPath)
{
    QDir appsDir(appsDirPath);
    QDir iconsDir(iconsDirPath);
    QStringList files = appsDir.entryList(QStringList() << "kumir2-*.desktop");
    foreach (const QString & fileName, files) {
        QFile f(appsDir.absoluteFilePath(fileName));
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream ts(&f);
            ts.setCodec("UTF-8");
            QStringList lines = ts.readAll().split("\n");
            f.close();
            ApplicationPrivate app;
            app.mdiInterface = false;
            bool isApp = false;
            bool noDysplay = false;
            foreach (const QString & line, lines) {
                int eqPos = line.indexOf('=');
                if (eqPos != -1) {
                    const QString key = line.left(eqPos).trimmed().toLower();
                    const QString value = line.mid(eqPos + 1).trimmed();
                    if (key == "type") {
                        isApp = value.toLower() == "application";
                    }
                    else if (key.startsWith("name")) {
                        if (app.name.isEmpty() || key=="name[ru]") {
                            app.name = value;
                        }
                    }
                    else if (key == "icon") {
                        const QString iconFileName =
                                iconsDir.absoluteFilePath(
                                    QString("32x32/apps/%1.png").arg(value)
                                    );
                        app.icon = QIcon(iconFileName);
                    }
                    else if (key == "exec") {
                        QString cleanName = value;
                        int spacePos = cleanName.indexOf(' ');
                        if (spacePos != -1) {
                            cleanName = cleanName.left(spacePos);
                        }
                        app.executableFileName =
                                QDir::cleanPath(
                                    QApplication::applicationDirPath() +
                                    "/" + cleanName
                                    );
#ifdef Q_OS_WIN32
                        app.executableFileName += ".exe";
#endif
                    }
                    else if (key == "x-kumir-mdi") {
                        app.mdiInterface =
                                value.toLower() == "true" ||
                                value.toLower() == "1";
                    }
                    else if (key == "nodysplay") {
                        noDysplay =
                                value.toLower() == "true" ||
                                value.toLower() == "1";
                    }
                }
            }
            if (isApp && !noDysplay) {
                applications_.push_back(app);
            }
        }
    }
}

void ApplicationManager::open(quintptr applicationId, const QUrl &url)
{
    ApplicationPrivate * app = reinterpret_cast<ApplicationPrivate*>(applicationId);
    const QString sUrl = url.toLocalFile();
    bool startNewProcess = true;
    Pid pid = 0;
    if (app->mdiInterface) {
        ProcessManager & procMan = ProcessManager::get();
        pid = procMan.find(app->executableFileName);
        startNewProcess = pid == 0;
    }
    if (startNewProcess) {
        const QString executable = app->executableFileName;
        const QStringList arguments = QStringList() << sUrl;
        QProcess::startDetached(executable, arguments);
    }
    else {
        Messager & messager = Messager::get();
        messager.sendMessage(pid, "OPEN " + sUrl);
    }

    Settings & sett = Settings::get();
    sett.setLastSelectedKey(QFileInfo(app->executableFileName).baseName());
}

Application ApplicationManager::find(const QString &key) const
{
    QList<Application> apps = applications();
    Application result; result.id = 0u;
    foreach (const Application & app, apps) {
        if (app.key == key) {
            result = app;
            break;
        }
    }
    return result;
}
