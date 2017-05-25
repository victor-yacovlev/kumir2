#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <kumir2-libs/extensionsystem/pluginmanager.h>
#include <kumir2-libs/extensionsystem/logger.h>


static
QString resolvePath(const char *what)
{
    static QString ExecDir = QString::fromLatin1(KUMIR2_EXEC_DIR);
    static QString MyDir = QCoreApplication::applicationDirPath();
    static int RootDistLevelUp = ExecDir.isEmpty()
            ? 0
            : ExecDir.count("/") + 1;
    QString result = MyDir;
    for (int i=0; i<RootDistLevelUp; ++i) {
        result += "/../";
    }
    result += QString::fromLatin1(what);
    result = QDir::cleanPath(result);
    return result;
}

#if QT_VERSION < 0x050000
void GuiMessageOutput(QtMsgType type, const char *msg)
#else
void GuiMessageOutput(QtMsgType type, const QMessageLogContext &, const QString & msg)
#endif
{
    ExtensionSystem::Logger * logger = ExtensionSystem::Logger::instance();
    switch (type) {
    case QtDebugMsg:
        logger->debug(msg);
        break;
    case QtWarningMsg:
        logger->warning(msg);
        break;
    case QtCriticalMsg:
        logger->critical(msg);
        break;
    case QtFatalMsg:
        logger->fatal(msg);
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
#ifdef Q_OS_LINUX
    gui = gui && getenv("DISPLAY")!=0;
#endif

    if (gui) {
        QMessageBox::critical(0, "Kumir 2 Launcher", text);
    }
    else {
        fprintf(stderr, "%s", qPrintable(text));
    }
}

QString getLanguage()
{
    return "ru"; // TODO implement sometime in future...
}

static
QList<QDir> translationsDirs()
{
    QList<QDir> result;

    // Translations dir from base distribution
    const QString sharePath = resolvePath(KUMIR2_RESOURCES_DIR);
    QDir baseTranslationsDir(sharePath+"/translations");
    if (baseTranslationsDir.exists()) {
        result.append(baseTranslationsDir);
    }

#ifdef Q_OS_LINUX
    // Search additional paths
    const QString homePath = QString::fromLocal8Bit(::getenv("HOME"));
    const QStringList extraPaths = QStringList()
            << "/usr/share/kumir2/translations"
            << "/usr/local/share/kumir2/translations"
            << "/opt/kumir2/share/translations"
            << "/opt/kumir/share/translations"
            << homePath + "/.local/share/kumir2/translations"
            << QDir::currentPath()+"/translations"
               ;

    Q_FOREACH(const QString & path, extraPaths) {
        QDir candidate(path);
        if (candidate.exists()) {
            result.append(candidate);
        }
    }
#endif

    return result;
}

class Application : QObject
{
public:
    inline explicit Application(int & argc, char **argv, bool gui)
        : QObject()
        , _qApp(0)
        , _timerId(-1)
        , _splashScreen(nullptr)
        , _started(false)
    {
        if (gui) {
            _qApp = new QApplication(argc, argv);
        }
        else {
            _qApp = new QCoreApplication(argc, argv);
        }
        _qApp->installEventFilter(this);
    }

    inline void setSplashScreen(QSplashScreen * s) {
        _splashScreen = s;
    }

    inline void initialize() {
        const QStringList arguments = _qApp->arguments();
        qDebug() << "Arguments: " << arguments;
        bool mustShowHelpAndExit = false;
        bool mustShowVersionAndExit = false;
        for (int i=1; i<arguments.size(); i++) {
            const QString & argument = arguments[i];
            if (argument=="--help" || argument=="-h" || argument=="/?") {
                mustShowHelpAndExit = true;
                break;
            }
            else if (argument=="--version") {
                mustShowVersionAndExit = true;
                break;
            }
            else if (!argument.startsWith("-")) {
                break;
            }
        }

        bool gui = true;
#ifdef Q_OS_LINUX
        gui = gui && getenv("DISPLAY")!=0;
#endif
        const QList<QDir> tsDirs = translationsDirs();
        Q_FOREACH(const QDir & translationsDir, tsDirs) {
            QStringList ts_files = translationsDir.entryList(QStringList() << "*_"+getLanguage()+".qm");
            foreach (QString tsname, ts_files) {
                tsname = tsname.mid(0, tsname.length()-3);
                QTranslator * tr = new QTranslator(_qApp);
                if (tr->load(tsname, translationsDir.absolutePath())) {
                    _qApp->installTranslator(tr);
                }
            }
        }

        const QString sharePath = resolvePath(KUMIR2_RESOURCES_DIR);
        _qApp->setProperty("sharePath", sharePath);

        QSettings::setDefaultFormat(QSettings::IniFormat);
        _qApp->addLibraryPath(resolvePath(KUMIR2_LIBS_DIR));
        _qApp->addLibraryPath(resolvePath(KUMIR2_PLUGINS_DIR));
        ExtensionSystem::PluginManager * manager = ExtensionSystem::PluginManager::instance();
        manager->setPluginPath(resolvePath(KUMIR2_PLUGINS_DIR));
        manager->setSharePath(sharePath);
        QString error;
        qDebug() << "Initialized plugin manager";
    #ifdef CONFIGURATION_TEMPLATE
        const QByteArray defaultTemplate = CONFIGURATION_TEMPLATE;
    #else
    #error No default configuration passed to GCC
    #endif
        QByteArray templ = defaultTemplate;
        for (int i=1; i<arguments.size(); i++) {
            QByteArray arg = arguments[i].toLatin1();
            if (arg.startsWith("[") && arg.endsWith("]")) {
                templ = arg.mid(1, arg.length()-2);
            }
        }
        qDebug() << "Loading plugins by template: " << templ;
        error = manager->loadPluginsByTemplate(templ);
        if (!gui && manager->isGuiRequired()) {
            if (_splashScreen)
                _splashScreen->finish(0);
            showErrorMessage("Requires X11 session to run this configuration");
            _qApp->exit(1);
        }

//        qInstallMsgHandler(manager->isGuiRequired()
//                           ? GuiMessageOutput
//                           : ConsoleMessageOutput);

        if (!error.isEmpty()) {
            if (_splashScreen)
                _splashScreen->finish(0);
            showErrorMessage(error);
            _qApp->exit(1);
        }

        qDebug() << "Done loading all plugins by template";

        if (mustShowHelpAndExit) {
            if (_splashScreen)
                _splashScreen->finish(0);
            const QString msg = manager->commandLineHelp();
#ifdef Q_OS_WIN32
            QTextCodec * codec = QTextCodec::codecForName("CP866");
            fprintf(stderr, "%s", codec->fromUnicode(msg).constData());
#else
            fprintf(stderr, "%s", msg.toLocal8Bit().data());
#endif
            _qApp->exit(0);
            return;
        }

        if (mustShowVersionAndExit) {
            fprintf(stderr, "%s\n", qPrintable(_qApp->applicationVersion()));
            _qApp->exit(0);
            return;
        }        
        qDebug() << "Begin plugins initialization";
        error = manager->initializePlugins();
        if (!error.isEmpty()) {
            if (_splashScreen)
                _splashScreen->finish(0);
            showErrorMessage(error);
            _qApp->exit(_qApp->property("returnCode").isValid()
                    ? _qApp->property("returnCode").toInt() : 1);
        }
        // GUI requirement may be changed as result of plugins initialization,
        // so check it again
        qDebug() << "Plugins initialization done";
        if (!gui && manager->isGuiRequired()) {
            showErrorMessage("Requires X11 session to run this configuration");
            _qApp->exit(_qApp->property("returnCode").isValid()
                    ? _qApp->property("returnCode").toInt() : 1);
        }
        if (_splashScreen)
            _splashScreen->finish(0);
        qDebug() << "Starting entry point plugin";
        error = manager->start();
        if (!error.isEmpty()) {
            if (_splashScreen)
                _splashScreen->finish(0);
            showErrorMessage(error);
            _qApp->exit(_qApp->property("returnCode").isValid()
                    ? _qApp->property("returnCode").toInt() : 1);
        }
        if (!manager->isGuiRequired()) {
            _qApp->quit();
        }
    }

    inline bool eventFilter(QObject *obj, QEvent * event) {
        if (event->type()==QEvent::Timer && obj==_qApp && !_started) {
            _started = true;
            _qApp->killTimer(_timerId);
            qDebug() << "Begin initialization";
            initialize();
            qDebug() << "Initialization done";
            return true;
        }
        else {
            return QObject::eventFilter(obj, event);
        }
    }

    inline int main() {        
        _timerId = _qApp->startTimer(250);
        int ret = _qApp->exec();
        if (ret == 0) {
            return _qApp->property("returnCode").isValid()
                    ? _qApp->property("returnCode").toInt() : 0;
        }
        else {
            return ret;
        }        
    }   

private:
    QCoreApplication *_qApp;
    int _timerId;
    QSplashScreen * _splashScreen;
    bool _started;

};


bool
setup_custom_vendor_information(QCoreApplication * app)
{
    QString appName;
    QString appVendor;
    QString appVersion;
    QString appLicenseFileName;
    QString appAboutFileName;
    bool result = false;
#ifdef APP_NAME
    appName = QString::fromLatin1(APP_NAME);
    result = true;
#endif
#ifdef APP_NAME_ru
    appName = QString::fromUtf8(APP_NAME_ru);
    result = true;
#endif
#ifdef APP_VENDOR
    appVendor = QString::fromLatin1(APP_VENDOR);
#endif
#ifdef APP_VENDOR_RU
    appVendor = QString::fromUtf8(APP_VENDOR_ru);
#endif
#ifdef APP_VERSION
    appVersion = QString::fromLatin1(APP_VERSION);
#endif
#ifdef APP_LICENSE
    appLicenseFileName = ":/kumir2-launcher/" + QString::fromLatin1(APP_LICENSE);
#endif
#ifdef APP_LICENSE_ru
    appLicenseFileName = ":/kumir2-launcher/" + QString::fromLatin1(APP_LICENSE_ru);
#endif
#ifdef APP_ABOUT
    appAboutFileName = ":/kumir2-launcher/" + QString::fromLatin1(APP_ABOUT);
#endif
#ifdef APP_ABOUT_ru
    appAboutFileName = ":/kumir2-launcher/" + QString::fromLatin1(APP_ABOUT_ru);
#endif
    if (appName.length() > 0) {
        app->setProperty("customAppName", appName);
    }
    if (appVendor.length() > 0) {
        app->setProperty("customAppVendor", appVendor);
    }
    if (appVersion.length() > 0) {
        app->setProperty("customAppVersion", appVersion);
    }
    if (appLicenseFileName.length() > 0) {
        app->setProperty("customAppLicense", appLicenseFileName);
    }
    if (appAboutFileName.length() > 0) {
        app->setProperty("customAppAbout", appAboutFileName);
    }
    return result;
}

int main(int argc, char **argv)
{ 
#if QT_VERSION < 0x050000
    qInstallMsgHandler(GuiMessageOutput);
#else
    qInstallMessageHandler(GuiMessageOutput);
#endif
    QString gitHash = QString::fromLatin1(GIT_HASH);
    QString gitTag = QString::fromLatin1(GIT_TAG);
    QString gitBranch = QString::fromLatin1(GIT_BRANCH);
    QDateTime gitTimeStamp = QDateTime::fromTime_t(QString::fromLatin1(GIT_TIMESTAMP).toUInt());


    bool gui = true;
#ifdef Q_OS_LINUX
    gui = gui && getenv("DISPLAY")!=0;
#endif
    Application * app = new Application(argc, argv, gui);
#ifdef WINDOW_ICON
    if (gui) {
        QApplication* guiApp = qobject_cast<QApplication*>(qApp);
        QString imgPath = ":/kumir2-launcher/" + QString::fromLatin1(WINDOW_ICON);
        QIcon icon(imgPath);
        guiApp->setWindowIcon(icon);
    }
#endif
    QLocale russian = QLocale("ru_RU");
    QLocale::setDefault(russian);

    qApp->addLibraryPath(resolvePath(KUMIR2_LIBS_DIR));
    qApp->addLibraryPath(resolvePath(KUMIR2_PLUGINS_DIR));

    qApp->setApplicationVersion(gitTag.length() > 0 && gitTag!="unknown"
                               ? gitTag : gitBranch + "/" + gitHash);
    qApp->setProperty("gitTimeStamp", gitTimeStamp);
    QSplashScreen * splashScreen = 0;

    const QString sharePath = resolvePath(KUMIR2_RESOURCES_DIR);

    const QStringList arguments = QCoreApplication::instance()->arguments();
    bool mustShowHelpAndExit = false;
    bool mustShowVersionAndExit = false;
    for (int i=1; i<arguments.size(); i++) {
        const QString & argument = arguments[i];
        if (argument=="--help" || argument=="-h" || argument=="/?") {
            mustShowHelpAndExit = true;
            break;
        }
        else if (argument=="--version") {
            mustShowVersionAndExit = true;
            break;
        }
        else if (!argument.startsWith("-")) {
            break;
        }
    }

    const bool customAppAndVendorInformation = setup_custom_vendor_information(qobject_cast<QCoreApplication*>(qApp));
    Q_UNUSED(customAppAndVendorInformation);

#ifdef SPLASHSCREEN
    if (gui && !mustShowHelpAndExit && !mustShowVersionAndExit) {
        QString imgPath = ":/kumir2-launcher/" + QString::fromLatin1(SPLASHSCREEN);
        splashScreen = new QSplashScreen();
        QImage img(imgPath);
        QPainter p(&img);
        p.setPen(QColor(Qt::black));
        p.setBrush(QColor(Qt::black));
        QFont f = p.font();

        f.setPixelSize(12);
        p.setFont(f);

        QString v;
        if (customAppAndVendorInformation) {
            v = qApp->property("customAppVersion").toString() + " ";
            v += "[based on Kumir ";
        }
        v = qApp->applicationVersion();
        if (qApp->property("gitHash").isValid()) {
            v += " (GIT "+qApp->property("gitHash").toString()+")";
        }
        if (customAppAndVendorInformation) {
            v += "]";
        }
        int tw = QFontMetrics(f).width(v);
        int th = QFontMetrics(f).height() * 5;
        int x = img.width() - tw - 8;
        int y = 8;
        p.drawText(x, y, tw, th, 0, v);
        p.end();
        QPixmap px = QPixmap::fromImage(img);
        splashScreen->setPixmap(px);
        splashScreen->show();
        qApp->processEvents();
        app->setSplashScreen(splashScreen);
    }
#endif    
    int ret = app->main();
    ExtensionSystem::PluginManager::destroy();
    delete app;
    return ret;
}

