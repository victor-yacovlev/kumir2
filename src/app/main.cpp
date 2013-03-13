#include <QtCore>
#include <QtGui>

#include "VERSION.h"
#include "GITINFO.h"

#include "extensionsystem/pluginmanager.h"

#ifdef Q_OS_MAC
#  define PLUGINS_PATH QDir(QApplication::applicationDirPath()+"/../PlugIns").canonicalPath()
#  define SHARE_PATH "/../Resources"
#else
#  define PLUGINS_PATH QDir(QApplication::applicationDirPath()+"/../"+IDE_LIBRARY_BASENAME+"/kumir2/plugins").canonicalPath()
#  define SHARE_PATH "/../share/kumir2"
#endif


void GuiMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    default:
        break;
    }
}

void ConsoleMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
//        fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
//        fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    default:
        break;
    }
}

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


class Application
        : public QApplication
{
public:
    inline explicit Application(int argc, char **argv, bool gui)
        : QApplication(argc, argv, gui) {}
    inline bool notify(QObject * receiver, QEvent * event) {
        bool result = false;
        try {
            result = QApplication::notify(receiver, event);
        }
        catch (...) {
            qDebug() << "Exception caught in QApplication::notify!!!";
//            abort();
        }
        return result;
    }
};

int main(int argc, char **argv)
{ 
    bool gui = true;
#ifdef Q_WS_X11
    gui = gui && getenv("DISPLAY")!=0;
#endif
    QApplication * app = new Application(argc, argv, gui);
    QLocale russian = QLocale("ru_RU");
    QLocale::setDefault(russian);
#ifdef Q_OS_WIN32
//    app->setAttribute(Qt::AA_DontShowIconsInMenus);
    app->addLibraryPath(app->applicationDirPath());
#endif
#ifdef Q_OS_MAC
   //  app->setAttribute(Qt::AA_DontUseNativeMenuBar, true);
#endif
#ifndef Q_OS_WIN32
    app->addLibraryPath(QDir::cleanPath(app->applicationDirPath()+"/../"+IDE_LIBRARY_BASENAME+"/kumir2/"));
#endif
#ifdef GIT_BRANCH
    static const QString branch = QString::fromAscii(GIT_BRANCH);
#else
    static const QString branch = QString::fromAscii("release");
#endif


    app->setApplicationVersion(QString("%1.%2.%3-%4")
                               .arg(VERSION_MAJOR)
                               .arg(VERSION_MINOR)
                               .arg(VERSION_RELEASE)
                               .arg(branch));

#ifdef GIT_HASH
    app->setProperty("gitHash", QString::fromAscii(GIT_HASH));
#endif
#ifdef GIT_LAST_MODIFIED
    app->setProperty("lastModified", QString::fromAscii(GIT_LAST_MODIFIED));
#endif

    QSplashScreen * splashScreen = 0;

    const QString sharePath = QDir(app->applicationDirPath()+SHARE_PATH).canonicalPath();

    bool allowDebugMessages = false;

#ifdef SPLASHSCREEN
    if (gui) {
        allowDebugMessages = true;
        QString imgPath = sharePath+QString("/")+SPLASHSCREEN;
        splashScreen = new QSplashScreen();
        QImage img(imgPath);
        QPainter p(&img);
        p.setPen(QColor(Qt::black));
        p.setBrush(QColor(Qt::black));
        QFont f = p.font();

        f.setPixelSize(12);
        p.setFont(f);
        QString v = qApp->applicationVersion();
        if (app->property("svnRev").isValid()) {
            v += " (rev. "+app->property("svnRev").toString()+")";
        }
        int tw = QFontMetrics(f).width(v);
        int th = QFontMetrics(f).height();
        int x = img.width() - tw - 8;
        int y = 8;
        p.drawText(x, y, tw, th, 0, v);
        p.end();
        QPixmap px = QPixmap::fromImage(img);
        splashScreen->setPixmap(px);
        splashScreen->show();
    }
#endif

    if (allowDebugMessages) {
        qInstallMsgHandler(GuiMessageOutput);
    }
    else {
        qInstallMsgHandler(ConsoleMessageOutput);
    }

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
    // GUI requirement may be changed as result of plugins initialization,
    // so check it again
    if (!gui && manager->isGuiRequired()) {
        showErrorMessage("Requires X11 session to run this configuration");
        delete manager;
        delete app;
        return 1;
    }
    app->setProperty("returnCode", 0);
    error = manager->start();
    if (!error.isEmpty()) {
        showErrorMessage(error);
        delete manager;
        delete app;
        return 1;
    }
    int ret = 0;
    if (splashScreen) {
        splashScreen->finish(0);
    }
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

    // delete manager;
    // delete app;
    if (splashScreen) {
//        delete splashScreen;
    }
    return ret;
}

