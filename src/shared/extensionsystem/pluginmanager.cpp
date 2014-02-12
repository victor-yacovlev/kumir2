#include "pluginmanager.h"
#include "pluginmanager_impl.h"
#include "kplugin.h"

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
            pImpl_->namedProgramArguments.push_back(arg);
        }
        else {
            unnamedArgumentsIndexBegin = i;
            break;
        }
    }

    if (unnamedArgumentsIndexBegin) {
        for (int i=unnamedArgumentsIndexBegin; i<arguments.size(); i++) {
            const QString & arg = arguments[i];
            if (!arg.startsWith("["))
                pImpl_->unnamedProgramArguments.push_back(arg);
        }
    }

#ifdef Q_WS_X11
    bool gui = getenv("DISPLAY")!=0;
    if (!gui)
        return;
#endif
}

PluginManager::~PluginManager()
{

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

void PluginManager::switchToDefaultWorkspace()
{
    const QString workDir = pImpl_->mySettings->value(
                CurrentWorkspaceKey,
                QString(QDir::homePath()+"/Kumir/")).toString();
    pImpl_->changeWorkingDirectory(workDir, false);
}

void PluginManager::switchToWorkspace(const QString &path)
{
    pImpl_->changeWorkingDirectory(path, true);
}


void PluginManager::setPluginPath(const QString &path)
{
    pImpl_->path = path;
}

void PluginManager::setSharePath(const QString &path)
{
    pImpl_->sharePath = path;
}


QString PluginManager::sharePath() const
{
    return pImpl_->sharePath;
}




QString PluginManager::loadPluginsByTemplate(const QString &templ)
{
    QList<PluginRequest> requests;
    QStringList names;
    QString error = "";
    error = pImpl_->parsePluginsRequest(templ, requests, names);
    if (!error.isEmpty())
        return error;
    //QScriptEngine engine;
    //engine.evaluate("var data = null;\n");
    //error = d->loadSpecs(names, &engine);
    error = pImpl_->loadSpecs(names);
    if (!error.isEmpty())
        return error;

    bool console = false;
#ifdef Q_WS_X11
    console = getenv("DISPLAY")==0;
#endif
    //    console = true; // !!! was here for debug purposes
    if (console) {
        // Remove GUI plugins
        QList<PluginSpec>::iterator it;
        for (it=pImpl_->specs.begin(); it!=pImpl_->specs.end(); ) {
            PluginSpec spec = (*it);
            if (spec.gui) {
                names.removeAll(spec.name);
                QList<PluginRequest>::iterator rit;
                for (rit=requests.begin(); rit!=requests.end(); ) {
                    PluginRequest r = *(rit);
                    if (r.name==spec.name) {
                        rit = requests.erase(rit);
                    }
                    else {
                        rit ++;
                    }
                }
                it = pImpl_->specs.erase(it);
            }
            else {
                it++;
            }
        }
    }

    // orderedList will contain names in order of load and initialization
    QStringList orderedList;
    // make dependencies for entry point plugin first
    error = pImpl_->makeDependencies(pImpl_->mainPluginName,orderedList);
    if (!error.isEmpty())
        return error;
    // make dependencies for other requests
    for (int i=0; i<requests.size(); i++) {
        error = pImpl_->makeDependencies(requests[i].name,orderedList);
        if (!error.isEmpty())
            return error;
    }
    error = pImpl_->reorderSpecsAndCreateStates(orderedList);
    if (!error.isEmpty())
        return error;
    error = pImpl_->loadPlugins();
    if (!error.isEmpty())
        return error;
    pImpl_->requests = requests;
    return "";
}

QString PluginManager::loadExtraModule(const std::string &canonicalFileName)
{
    QString moduleName = QString::fromStdString(canonicalFileName);
    if ( moduleName.length()>0 )
        moduleName[0] = moduleName[0].toUpper();

    QString libraryFileName = moduleName;
#if defined(Q_OS_WIN32)
    libraryFileName += ".dll";
#elif defined(Q_OS_MACX)
    libraryFileName = "lib"+libraryFileName+".dylib";
#else
    libraryFileName = "lib"+libraryFileName+".so";
#endif
    QString fullPath;
    if (QFile::exists(pImpl_->path+"/"+libraryFileName))
        fullPath = pImpl_->path + "/" + libraryFileName;
    else if (QFile::exists(QDir::currentPath()+"/"+libraryFileName))
        fullPath = QDir::currentPath()+ "/" + libraryFileName;
    else {
        const QString errorMessage = tr("Can't load module %1: file not found")
                .arg(libraryFileName);
        return errorMessage;
    }

    QPluginLoader loader(fullPath);

    if (!loader.load()) {
        return QString("Can't load module %1: %2")
                .arg(libraryFileName)
                .arg(loader.errorString());
    }
    KPlugin * plugin = qobject_cast<KPlugin*>(loader.instance());
    if (!plugin) {
        return QString("Plugin is not valid (does not implement interface KPlugin)");
        loader.unload();
    }
    pImpl_->objects.push_back(plugin);
    pImpl_->states.push_back(KPlugin::Loaded);
    PluginSpec spec;
    spec.name = QString::fromStdString(canonicalFileName);
    spec.libraryFileName = libraryFileName;
    spec.gui = plugin->isGuiRequired();
    pImpl_->specs.push_back(spec);
    Settings * sett = new Settings(moduleName);
    sett->changeWorkingDirectory(pImpl_->workspacePath);
    pImpl_->settings.push_back(SettingsPtr(sett));
    plugin->updateSettings(QStringList());
    CommandLine runtimeParameters;
    if (!pImpl_->extractRuntimeParametersForPlugin(plugin, runtimeParameters)) {
        QString error = tr("The following command line parameters required, but not set:\n");
        for (int i=0; i<runtimeParameters.data_.size(); i++) {
            const CommandLineParameter & param = runtimeParameters.data_[i];
            if (param.shortDescription_.length() > 0) {
                if (!param.isValid() && plugin==pImpl_->objects.last())
                    error += "  " + param.toHelpLine() + "\n";
            }
            else if (!param.isValid()) {
                error += "  " + param.toHelpLine() + "\n";
            }
        }
        return error;
    }
    QString result = plugin->initialize(QStringList(), runtimeParameters);
    pImpl_->states.last() = KPlugin::Initialized;
    return result;
}

bool PluginManager::isGuiRequired() const
{
    KPlugin * runtimePlugin = qobject_cast<KPlugin*>(pImpl_->objects.last());
    if (runtimePlugin)
        return runtimePlugin->isGuiRequired();
    else
        return false;
}

QString PluginManager::commandLineHelp() const
{
    const PluginSpec &mainSpec = pImpl_->specs.last();
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

QString PluginManager::initializePlugins()
{
    Q_ASSERT(pImpl_->specs.size()==pImpl_->objects.size());
    for (int i=0; i<pImpl_->objects.size(); i++) {
        if (pImpl_->states[i] != KPlugin::Loaded)
            continue;
        const QString name = pImpl_->specs[i].name;
        QStringList arguments;
        for (int j=0; j<pImpl_->requests.size(); j++) {
            if (pImpl_->requests[j].name==name) {
                arguments = pImpl_->requests[j].arguments;
            }
        }
        CommandLine runtimeParameters;
        if (!pImpl_->extractRuntimeParametersForPlugin(pImpl_->objects[i], runtimeParameters)) {
            QString error = tr("The following command line parameters required, but not set:\n");
            for (int i=0; i<runtimeParameters.data_.size(); i++) {
                const CommandLineParameter & param = runtimeParameters.data_[i];
                if (!param.isValid()) {
                    error += "  " + param.toHelpLine() + "\n";
                }
            }
            error += tr("Run with --help for more details.\n");
            return error;
        }
        QString error = pImpl_->objects[i]->initialize(arguments, runtimeParameters);
        if (!error.isEmpty()) {
            return QString("Error initializing %1: %2")
                    .arg(name)
                    .arg(error);
        }
        pImpl_->states[i] = KPlugin::Initialized;
    }

    std::list<QString> remainingParameters = pImpl_->namedProgramArguments;
    remainingParameters.merge(pImpl_->unnamedProgramArguments);;

    if (remainingParameters.size() > 0) {
        QString error = tr("Extra command line arguments:\n");

        for (std::list<QString>::const_iterator it=remainingParameters.begin();
             it!=remainingParameters.end(); ++it)
        {
            error += "  " + (*it) + "\n";
        }

        error += tr("Run with --help for more details.\n");
        return error;
    }

    return "";
}

QList<const KPlugin*> PluginManager::loadedConstPlugins(const QString &pattern) const
{
    QList<const KPlugin*> result;
    const QRegExp rx = QRegExp(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
    for (int i=0; i<pImpl_->specs.size(); i++) {
        if (rx.exactMatch(pImpl_->specs[i].name)) {
            result << pImpl_->objects[i];
        }
    }
    return result;
}

QList<KPlugin*> PluginManager::loadedPlugins(const QString &pattern)
{
    QList<KPlugin*> result;
    const QRegExp rx = QRegExp(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
    for (int i=0; i<pImpl_->specs.size(); i++) {
        if (rx.exactMatch(pImpl_->specs[i].name)) {
            result << pImpl_->objects[i];
        }
    }
    return result;
}

KPlugin* PluginManager::loadedPlugin(const QString &name)
{
    for (int i=0; i<pImpl_->specs.size(); i++) {
        if (pImpl_->specs[i].name==name)
            return pImpl_->objects[i];
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
    int index = pImpl_->objects.indexOf(p);
    p->start();
    pImpl_->states[index] = KPlugin::Started;
    return "";
}

PluginSpec PluginManager::specByObject(const KPlugin *p) const
{
    Q_ASSERT(pImpl_->specs.size()==pImpl_->objects.size());
    for (int i=0; i<pImpl_->specs.size(); i++) {
        if (pImpl_->objects[i]==p) {
            return pImpl_->specs[i];
        }
    }
    return PluginSpec();
}

KPlugin * PluginManager::dependentPlugin(const QString &name, const KPlugin *p) const
{
    PluginSpec spec;
    Q_ASSERT(pImpl_->specs.size()==pImpl_->objects.size());
    for (int i=0; i<pImpl_->specs.size(); i++) {
        if (pImpl_->objects[i]==p) {
            spec = pImpl_->specs[i];
        }
    }
    bool hasDep = false;
    QSet<QString> depAliases;
    for (int i=0; i<spec.dependencies.size(); i++) {
        for (int j=0; j<pImpl_->specs.size(); j++) {
            if (pImpl_->specs[j].provides.contains(spec.dependencies[i]))
                depAliases |= QSet<QString>::fromList(pImpl_->specs[j].provides);
        }
    }
    for (int i=0; i<spec.dependencies.size(); i++) {
        if (depAliases.contains(spec.dependencies[i])) {
            hasDep = true;
            break;
        }
    }
    if (!hasDep) {
        return 0;
    }
    for (int i=0; i<pImpl_->specs.size(); i++) {
        PluginSpec ss=pImpl_->specs[i];
        if (pImpl_->specs[i].provides.contains(name)) {
            return pImpl_->objects[i];
        }
    }
    return 0;
}

KPlugin::State PluginManager::stateByObject(const KPlugin *p) const
{
    Q_ASSERT(pImpl_->states.size()==pImpl_->objects.size());
    for (int i=0; i<pImpl_->objects.size(); i++) {
        if (pImpl_->objects[i]==p) {
            return pImpl_->states[i];
        }
    }
    return KPlugin::Disabled;
}

SettingsPtr PluginManager::settingsByObject(const KPlugin *p) const
{
    Q_ASSERT(pImpl_->settings.size()==pImpl_->objects.size());
    for (int i=0; i<pImpl_->objects.size(); i++) {
        if (pImpl_->objects[i]==p) {
            //            qDebug()<<"sett"<<d->settings[i];
            return pImpl_->settings[i];
        }
    }
    return SettingsPtr();
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

    for (int i=pImpl_->objects.size()-1; i>=0; i--) {
        if (pImpl_->states[i] != KPlugin::Stopped) {
            KPlugin * p = pImpl_->objects[i];
            SettingsPtr s = pImpl_->settings[i];
            if (s) {
                pImpl_->objects[i]->saveSession();
                s->flush();
            }
            p->stop();
            pImpl_->states[i] = KPlugin::Stopped;
            pImpl_->settings[i].clear();
        }
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
