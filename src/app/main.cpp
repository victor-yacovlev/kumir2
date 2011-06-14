#include <QtCore>
#include <QtGui>

#include "extensionsystem/pluginmanager.h"

#ifdef Q_OS_MAC
#  define PLUGINS_PATH QDir(QApplication::applicationDirPath()+"/../PlugIns").canonicalPath()
#  define SHARE_PATH "/../Resources"
#else
#  define PLUGINS_PATH QDir(QApplication::applicationDirPath()+"/../"+IDE_LIBRARY_BASENAME+"/kumir2/plugins").canonicalPath()
#  define SHARE_PATH "/../share/kumir2"
#endif

void showErrorMessage(const QString & text)
{
    bool gui = true;
#ifdef Q_WS_X11
    gui = gui && getenv("DISPLAY")!=0;
#endif

    if (gui) {
        QMessageBox::critical(0, "Kumir 2 Launcher", text);
    }
    else {
        qCritical("%s", qPrintable(text));
    }
}

QString getLanguage()
{
    return "ru"; // TODO implement sometime in future...
}


int main(int argc, char **argv)
{ 
    bool gui = true;
#ifdef Q_WS_X11
    gui = gui && getenv("DISPLAY")!=0;
#endif
    QApplication * app = new QApplication(argc, argv, gui);
    const QString sharePath = QDir(app->applicationDirPath()+SHARE_PATH).canonicalPath();
    QDir translationsDir(sharePath+"/translations");
    QStringList ts_files = translationsDir.entryList(QStringList() << "*_"+getLanguage()+".qm");
    foreach (QString tsname, ts_files) {
        tsname = tsname.mid(0, tsname.length()-3);
        QTranslator * tr = new QTranslator(app);
        if (tr->load(tsname, sharePath+"/translations")) {
//            qDebug() << "Loaded " << tsname;
            app->installTranslator(tr);
        }
    }

    app->setProperty("sharePath", sharePath);

    QSettings::setDefaultFormat(QSettings::IniFormat);
//    const QString settingsPath = QDir(app->applicationDirPath()+QString(SHARE_PATH)+"/default_settings").canonicalPath();
//    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, settingsPath);
    app->addLibraryPath(PLUGINS_PATH);
    ExtensionSystem::PluginManager * manager = new ExtensionSystem::PluginManager;
    QObject::connect (app, SIGNAL(aboutToQuit()), manager, SLOT(shutdown()));
    manager->setPluginPath(PLUGINS_PATH);
    manager->setSharePath(SHARE_PATH);
    QString error;

#ifdef CONFIGURATION_TEMPLATE
    const QString defaultTemplate = CONFIGURATION_TEMPLATE;
#else
#error No default configuration passed to GCC
#endif
    QString templ = defaultTemplate;
    for (int i=1; i<argc; i++) {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith("[") && arg.endsWith("]")) {
            templ = arg.mid(1, arg.length()-2);
        }
    }
    error = manager->loadPluginsByTemplate(templ);
    if (!gui && manager->isGuiRequired()) {
        showErrorMessage("Requires X11 session to run this configuration");
        delete manager;
        delete app;
        return 1;
    }

    if (!error.isEmpty()) {
        showErrorMessage(error);
        delete manager;
        delete app;
        return 1;
    }
    error = manager->initializePlugins();
    if (!error.isEmpty()) {
        showErrorMessage(error);
        delete manager;
        delete app;
        return 1;
    }
    error = manager->start();
    if (!error.isEmpty()) {
        showErrorMessage(error);
        delete manager;
        delete app;
        return 1;
    }
    int ret = 0;
    app->setProperty("returnCode", 0);
    if (manager->isGuiRequired()) {
        ret = app->exec();
        if (ret == 0) {
            ret = app->property("returnCode").toInt();
        }
    }
    else {
        manager->shutdown();
        ret = app->property("returnCode").toInt();
    }
    delete manager;
    delete app;
    return ret;
}

