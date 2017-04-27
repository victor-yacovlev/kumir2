#include "pluginmanager.h"
#include "pluginmanager_impl.h"
#include "kplugin.h"

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

namespace ExtensionSystem {

const QString PluginManager::WorkspacesListKey = "WorkspacesList";
const QString PluginManager::CurrentWorkspaceKey = "CurrentWorkspace";
const QString PluginManager::SkipChooseWorkspaceKey = "SkipChooseWorkspace";


PluginManager::PluginManager()
    : QObject()
    , pImpl_(new PluginManagerImpl)
{
    pImpl_->globalState = Shared::PluginInterface::GS_Unlocked;
    pImpl_->mySettings = SettingsPtr(new Settings("ExtensionSystem"));

    int unnamedArgumentsIndexBegin = 1;

    QStringList arguments = QCoreApplication::instance()->arguments();

    for (int i=1; i<arguments.size(); i++) {
        const QString & arg = arguments[i];
        if (arg.startsWith("-")) {
            if (arg!="--debug" && !arg.startsWith("--log=")) {
                pImpl_->namedProgramArguments.push_back(arg);
            }
        }
        else {
            unnamedArgumentsIndexBegin = i;
            break;
        }
    }

    if (unnamedArgumentsIndexBegin) {
        for (int i=unnamedArgumentsIndexBegin; i<arguments.size(); i++) {
            const QString & arg = arguments[i];
            if (!arg.startsWith("[") && !arg.startsWith("-"))
                pImpl_->unnamedProgramArguments.push_back(arg);
        }
    }

#ifdef Q_WS_X11
    bool gui = getenv("DISPLAY")!=0;
    if (!gui)
        return;
#endif
}


void PluginManager::setupAdditionalPluginPaths()
{
    QString homeLocation;
#if QT_VERSION >= 0x050000
    homeLocation = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
#else
    homeLocation = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif
    pImpl_->additionalPluginPrefixes = QStringList()
            << "/usr/"
            << "/opt/kumir2/"
            << "/opt/kumir/"
            << "/usr/local/"
            << homeLocation + "/.local/"
               ;
}

PluginManager::~PluginManager()
{

}

bool PluginManager::isPluginLoaded(const QByteArray &name) const
{
    return pImpl_->isPluginLoaded(name);
}

void PluginManager::destroy()
{
    PluginManager * pm = instance();
    foreach (KPlugin * o, pm->pImpl_->objects) {
        delete o;
    }
    pm->pImpl_->objects.clear();
}

SettingsPtr PluginManager::globalSettings() const
{
    return pImpl_->mySettings;
}

void PluginManager::updateAllSettings()
{
    foreach (KPlugin * o, pImpl_->objects) {
        o->updateSettings(QStringList());
    }
}


void PluginManager::switchToDefaultWorkspace(bool workDirOnly)
{
    const QString workDir = pImpl_->mySettings->value(
                CurrentWorkspaceKey,
                QString(QDir::homePath()+"/Kumir/")).toString();
    pImpl_->changeWorkingDirectory(workDir, false, workDirOnly);
}

void PluginManager::switchToWorkspace(const QString &path, bool workDirOnly)
{
    pImpl_->changeWorkingDirectory(path, true, workDirOnly);
}


void PluginManager::setPluginPath(const QString &path)
{
    pImpl_->path = path;
#ifdef Q_OS_LINUX
    setupAdditionalPluginPaths();
#endif
}

void PluginManager::setSharePath(const QString &path)
{
    pImpl_->sharePath = path;
}


QString PluginManager::sharePath() const
{
    return pImpl_->sharePath;
}




QString PluginManager::loadPluginsByTemplate(const QByteArray &templ)
{
    QList<PluginSpec> requests;
    QString error = "";
    error = pImpl_->parsePluginsRequest(templ, requests);
    if (!error.isEmpty())
        return error;    
    if (!error.isEmpty())
        return error;

    bool console = false;
#ifdef Q_OS_UNIX
    console = getenv("DISPLAY")==0;
#endif

    Q_FOREACH(PluginSpec spec, requests) {
        error = pImpl_->loadPlugin(spec, requests);
        if (!error.isEmpty()) {
            return error;
        }
    }
    return "";
}

QString PluginManager::loadExtraModule(const std::string &canonicalFileName)
{
    return "Not implemented yet";
//    QString moduleName = QString::fromStdString(canonicalFileName);
//    if ( moduleName.length()>0 )
//        moduleName[0] = moduleName[0].toUpper();

//    QString libraryFileName = moduleName.replace(" ", "");
//#if defined(Q_OS_WIN32)
//    libraryFileName += ".dll";
//#elif defined(Q_OS_MACX)
//    libraryFileName = "lib"+libraryFileName+".dylib";
//#else
//    libraryFileName = "lib"+libraryFileName+".so";
//#endif
//    QString fullPath;
//    QString check;
//    check = pImpl_->path + "/" + libraryFileName;
//    if (QFile::exists(check)) {
//        fullPath = check;
//    }
//    if (fullPath.isEmpty()) {
//        check = QDir::currentPath()+"/"+libraryFileName;
//        if (QFile::exists(check)) {
//            fullPath = check;
//        }
//    }

//    if (fullPath.isEmpty()) {
//        const QString errorMessage = tr("Can't load module %1: file not found")
//                .arg(libraryFileName);
//        return errorMessage;
//    }

//    QPluginLoader loader(fullPath);

//    if (!loader.load()) {
//        return QString("Can't load module %1: %2")
//                .arg(libraryFileName)
//                .arg(loader.errorString());
//    }
//    KPlugin * plugin = qobject_cast<KPlugin*>(loader.instance());
//    if (!plugin) {
//        return QString("Plugin is not valid (does not implement interface KPlugin)");
//        loader.unload();
//    }
//    pImpl_->objects.push_back(plugin);
//    pImpl_->states.push_back(KPlugin::Loaded);
//    PluginSpec spec;
//    spec.name = QString::fromStdString(canonicalFileName);
//    spec.libraryFileName = libraryFileName;
//    spec.gui = plugin->isGuiRequired();
//    pImpl_->specs.push_back(spec);
//    Settings * sett = new Settings(moduleName);
//    sett->changeWorkingDirectory(pImpl_->settingsWorkspacePath);
//    pImpl_->settings.push_back(SettingsPtr(sett));
//    plugin->updateSettings(QStringList());
//    CommandLine runtimeParameters;
//    if (!pImpl_->extractRuntimeParametersForPlugin(plugin, runtimeParameters)) {
//        QString error = tr("The following command line parameters required, but not set:\n");
//        for (int i=0; i<runtimeParameters.data_.size(); i++) {
//            const CommandLineParameter & param = runtimeParameters.data_[i];
//            if (param.shortDescription_.length() > 0) {
//                if (!param.isValid() && plugin==pImpl_->objects.last())
//                    error += "  " + param.toHelpLine() + "\n";
//            }
//            else if (!param.isValid()) {
//                error += "  " + param.toHelpLine() + "\n";
//            }
//        }
//        return error;
//    }
//    QString result = plugin->initialize(QStringList(), runtimeParameters);
//    pImpl_->states.last() = KPlugin::Initialized;
//    return result;
}

bool PluginManager::isGuiRequired() const
{
    KPlugin * runtimePlugin = qobject_cast<KPlugin*>(pImpl_->objects.last());
    if (runtimePlugin)
        return runtimePlugin->isGuiRequired();
    else
        return false;
}

QString PluginManager::initializePlugins()
{
    QString error;

    // Initialize direct dependencies of startup module, but not startup itself
    KPlugin * entryPoint = loadedPlugin(pImpl_->mainPluginName);
    const PluginSpec & spec = entryPoint->pluginSpec();
    QList<KPlugin*> directEntryPointDeps;
    Q_FOREACH(const QByteArray & depName, spec.dependencies) {
        Q_FOREACH(KPlugin* anotherPlugin, pImpl_->objects) {
            const PluginSpec & anotherSpec = anotherPlugin->pluginSpec();
            if (anotherSpec.name == depName || anotherSpec.provides.contains(depName)) {
                directEntryPointDeps.append(anotherPlugin);
            }
        }
    }
    Q_FOREACH(KPlugin * dep, directEntryPointDeps) {
        error = pImpl_->initializePlugin(dep);
        if (error.length() > 0) {
            return error;
        }
    }

    // Initialize rest of plugins, but not startup
    Q_FOREACH(KPlugin * plugin, pImpl_->objects) {
        error = pImpl_->initializePlugin(plugin);
        if (error.length() > 0) {
            return error;
        }
    }

    // Initialize startup module itself
    error = pImpl_->initializePlugin(entryPoint);
    if (error.length() > 0) {
        return error;
    }

    return "";
}

QString PluginManager::commandLineHelp() const
{
    PluginSpec mainSpec;
    Q_FOREACH(KPlugin * plugin, pImpl_->objects) {
        if (plugin->pluginSpec().main) {
            mainSpec = plugin->pluginSpec();
            break;
        }
    }
    bool guiMode = mainSpec.gui;
    QString result = tr("Usage:\n");
    QString programName = QCoreApplication::applicationFilePath();
    if (programName.startsWith(QCoreApplication::applicationDirPath())) {
        programName.remove(0, QCoreApplication::applicationDirPath().length() + 1);
    }
    QString shortLine = "  " + programName + " ";
    QString longLine = "  " + programName + " ";
    QStringList details;
    foreach (const KPlugin * plugin, pImpl_->objects) {
        const QList<CommandLineParameter> params = plugin->acceptableCommandLineParameters();
        foreach (const CommandLineParameter & param, params) {
            bool allowThisParam = guiMode ? param.allowInGui_ : true;
            if (allowThisParam && param.longName_.length() > 0) {
                if (!param.valueRequired_) {
                    shortLine += "[";
                    longLine += "[";
                }
                shortLine += "-" + QString(param.shortName_);
                longLine += "--" + QString(param.longName_);
                if (param.acceptValue_) {
                    QString a = "=<";
                    if (param.acceptType_==QVariant::Int)
                        a += tr("int");
                    else if (param.acceptType_==QVariant::Double)
                        a += tr("real");
                    else if (param.acceptType_==QVariant::Bool)
                        a += tr("bool");
                    else if (param.acceptType_==QVariant::Char)
                        a += tr("char");
                    else
                        a += tr("string");
                    a += ">";
                    shortLine += a;
                    longLine += a;
                }
                if (!param.valueRequired_) {
                    shortLine += "]";
                    longLine += "]";
                }
                shortLine += " ";
                longLine += " ";
                details.push_back(param.toHelpLine());
            }
        }
    }
    const QList<CommandLineParameter> params = pImpl_->objects.last()->acceptableCommandLineParameters();
    foreach (const CommandLineParameter & param, params) {
        if (param.shortDescription_.length() > 0) {
            if (param.valueRequired_) {
                shortLine += param.shortDescription_;
                longLine += param.shortDescription_;
            }
            else {
                shortLine += "[";
                longLine += "[";
                shortLine += param.shortDescription_.arg(1);
                longLine += param.shortDescription_.arg(1);
                shortLine += "]...[";
                longLine += "]...[";
                shortLine += param.shortDescription_.arg("n");
                longLine += param.shortDescription_.arg("n");
                shortLine += "]";
                longLine += "]";
            }
            shortLine += " ";
            longLine += " ";
            details.push_back(param.toHelpLine());
        }
    }
    if (longLine == shortLine) {
        result += shortLine + "\n";
    }
    else {
        result += longLine + "\n" + tr("or") + "\n" + shortLine + "\n";
    }
    if (details.size() > 0)
        result += "\n" + tr("Options") + ":\n" + details.join("\n") + "\n";
    return result;
}

QFont PluginManager::initialApplicationFont() const
{
    return pImpl_->initialApplicationFont;
}

QDir PluginManager::findSystemResourcesDir(const QString &subdirShouldExists) const
{
    QDir result = QDir(sharePath());
#ifdef Q_OS_LINUX
    const QString homePath = QString::fromLocal8Bit(::getenv("HOME"));
    const QStringList extraPaths = QStringList()
            << "/usr/share/kumir2/"
            << "/usr/local/share/kumir2/"
            << "/opt/kumir2/share/"
            << "/opt/kumir/share/"
            << homePath + "/.local/share/kumir2/"
            << QDir::currentPath() + "/resources/"
            << QDir::currentPath() + "/data/"
            ;

    Q_FOREACH(const QString & path, extraPaths) {
        QDir candidate(path + subdirShouldExists);
        if (candidate.exists()) {
            return QDir(path);
        }
    }
#else
    Q_UNUSED(subdirShouldExists);
#endif
    return result;
}


QList<const KPlugin*> PluginManager::loadedConstPlugins(const QByteArray &pattern) const
{
    QList<const KPlugin*> result;
    const QRegExp rx = QRegExp(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
    Q_FOREACH(const KPlugin * p, pImpl_->objects) {
        const PluginSpec & spec = p->pluginSpec();
        if ( pattern.length()==0 || (pattern.length() > 0 && rx.exactMatch(spec.name))) {
            result.append(p);
        }
    }
    return result;
}

QList<KPlugin*> PluginManager::loadedPlugins(const QByteArray &pattern)
{
    QList<KPlugin*> result;
    const QRegExp rx = QRegExp(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
    Q_FOREACH(KPlugin * p, pImpl_->objects) {
        const PluginSpec & spec = p->pluginSpec();
        if ( pattern.length()==0 || (pattern.length() > 0 && rx.exactMatch(spec.name))) {
            result.append(p);
        }
    }
    return result;
}

KPlugin* PluginManager::loadedPlugin(const QByteArray &name)
{
    Q_FOREACH(KPlugin * p, pImpl_->objects) {
        const PluginSpec & spec = p->pluginSpec();
        if (spec.name == name) {
            return p;
        }
    }
    return 0;
}

KPlugin* PluginManager::startupModule()
{
    return loadedPlugin(pImpl_->mainPluginName);
}

QString PluginManager::start()
{
    KPlugin * p = startupModule();
    p->start();
    p->_state = KPlugin::Started;
    return "";
}


KPlugin * PluginManager::dependentPlugin(const QByteArray &name, const KPlugin *p) const
{
    Q_FOREACH(KPlugin * anotherPlugin, pImpl_->objects) {
        const PluginSpec & anotherSpec = anotherPlugin->pluginSpec();
        if (anotherSpec.name == name || anotherSpec.provides.contains(name)) {
            return anotherPlugin;
        }
    }
    return 0;
}


PluginManager * PluginManager::instance()
{
    static PluginManager INSTANCE;
    return &INSTANCE;
}

bool PluginManager::shutdown()
{
    Q_FOREACH(KPlugin * plugin, pImpl_->objects) {
        bool moduleSafeToQuit = plugin->isSafeToQuit();
        if (!moduleSafeToQuit) {
            return false;
        }
    }

    Q_FOREACH(KPlugin * plugin, pImpl_->objects) {
        SettingsPtr s = plugin->mySettings();
        if (s) {
            plugin->saveSession();
            s->flush();
        }
        plugin->stop();
        plugin->_state = KPlugin::Stopped;
    }

    return true;
}



void PluginManager::switchGlobalState(GlobalState state)
{
    for (int i=0; i<pImpl_->objects.size(); i++) {
        KPlugin * p = pImpl_->objects[i];
        p->changeGlobalState(pImpl_->globalState, state);
    }
    pImpl_->globalState = state;
}

GlobalState PluginManager::currentGlobalState() const
{
    return pImpl_->globalState;
}

} // namespace ExtensionSystem
