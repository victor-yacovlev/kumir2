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

#ifdef HAS_CONFIGURATION_TEMPLATE
#include "cofiguration_template.h"
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


int main(int argc, char **argv)
{ 
    bool gui = true;
#ifdef Q_WS_X11
    gui = gui && getenv("DISPLAY")!=0;
#endif
    QApplication * app = new QApplication(argc, argv, gui);
    const QString sharePath = QDir(app->applicationDirPath()+SHARE_PATH).canonicalPath();
    app->setProperty("sharePath", sharePath);
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, SHARE_PATH);
    app->addLibraryPath(PLUGINS_PATH);
    ExtensionSystem::PluginManager * manager = new ExtensionSystem::PluginManager;
    QObject::connect (app, SIGNAL(aboutToQuit()), manager, SLOT(shutdown()));
    manager->setPluginPath(PLUGINS_PATH);
    manager->setSharePath(SHARE_PATH);
    QString error;

#ifdef HAS_CONFIGURATION_TEMPLATE
    const QString defaultTemplate = CONFIGURATION_TEMPLATE;
#else
    const QString defaultTemplate = "&KumirCompiler";
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
    if (manager->isGuiRequired())
        ret = app->exec();
    else {
        manager->shutdown();
    }
    delete manager;
    delete app;
    return ret;
}

