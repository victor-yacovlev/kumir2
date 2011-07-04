#include "pluginmanager.h"
#include "kplugin.h"
#include <QScriptEngine>
#include "settingsdialog.h"
#include "switchworkspacedialog.h"

namespace ExtensionSystem {

struct PluginRequest {
    QString name;
    bool start;
    QStringList arguments;
};

struct PluginManagerPrivate {
    QList<KPlugin*> objects;
    QString path;
    QString sharePath;
    QList<PluginSpec> specs;
    QList<KPlugin::State> states;
    QList<QSettings*> settings;
    QList<PluginRequest> requests;
    QString mainPluginName;
    GlobalState globalState;

    SettingsDialog * settingsDialog;
    SwitchWorkspaceDialog * switchWorkspaceDialog;
    QSettings * mySettings;


    QString parsePluginsRequest(const QString &templ, QList<PluginRequest> & plugins, QStringList & names);
    QString loadSpecs(const QStringList &names, QScriptEngine * engine);
    QString makeDependencies(const QString &entryPoint,
                             const QString &minVersion,
                             const QString &maxVersion,
                             QStringList &orderedList);
    QString reorderSpecsAndCreateStates(const QStringList & orderedList);
    void createSettingsDialog();
    QString loadPlugins();
    void changeWorkingDirectory(const QString &path);
};

PluginManager::PluginManager()
    : QObject()
    , d(new PluginManagerPrivate)
{
    m_instance = this;
    d->globalState = GS_Unlocked;
    d->mySettings = new QSettings("kumir2", "ExtensionSystem");
#ifdef Q_WS_X11
    bool gui = getenv("DISPLAY")!=0;
    if (!gui)
        return;
#endif
    d->switchWorkspaceDialog = new SwitchWorkspaceDialog(d->mySettings);
}

PluginManager::~PluginManager()
{
    d->mySettings->deleteLater();
    if (d->settingsDialog)
        d->settingsDialog->deleteLater();
    if (d->switchWorkspaceDialog)
        d->switchWorkspaceDialog->deleteLater();
    d->settingsDialog = 0;
    delete d;
}

void PluginManagerPrivate::createSettingsDialog()
{
    settingsDialog = 0;

#ifdef Q_WS_X11
    bool gui = getenv("DISPLAY")!=0;
    if (!gui)
        return;
#endif
    settingsDialog = new SettingsDialog;
    for (int i=0; i<objects.size(); i++) {
        settingsDialog->addPage(objects[i]->settingsEditorPage());
    }

}

void PluginManager::showSettingsDialog()
{
    if (d->settingsDialog) {
        d->settingsDialog->exec();
    }
}

bool PluginManager::showWorkspaceChooseOnLaunch() const
{
    return ! d->mySettings->value(SwitchWorkspaceDialog::SkipChooseWorkspaceKey, false).toBool();
}

void PluginManager::switchToDefaultWorkspace()
{
    d->changeWorkingDirectory(d->mySettings->value(SwitchWorkspaceDialog::CurrentWorkspaceKey, QString(QDir::homePath()+"/Kumir/")).toString());
}

bool PluginManager::showWorkspaceChooseDialog()
{
    if (d->switchWorkspaceDialog) {
        if (d->switchWorkspaceDialog->exec()==QDialog::Accepted) {
            d->changeWorkingDirectory(d->switchWorkspaceDialog->currentWorkspace());
            return true;
        }
    }
    return false;
}



void PluginManager::setPluginPath(const QString &path)
{
    d->path = path;
}

void PluginManager::setSharePath(const QString &path)
{
    d->sharePath = path;
}

QString PluginManagerPrivate::reorderSpecsAndCreateStates(const QStringList &orderedList)
{
    QList<PluginSpec> newSpecs;
    for (int i=0; i<orderedList.size(); i++) {
        bool found = false;
        PluginSpec spec;
        for (int j=0; j<specs.size(); j++) {
            if (specs[j].name==orderedList[i]) {
                spec = specs[j];
                found = true;
                break;
            }
        }
        if (!found) {
            return "Spec not loaded for plugin " + orderedList[i];
        }
        newSpecs << spec;
        objects << 0;
        states << KPlugin::Disabled;
        settings << 0;
    }
    specs = newSpecs;
    return "";
}

QString PluginManagerPrivate::parsePluginsRequest(const QString &templ, QList<PluginRequest> &plugins, QStringList & names)
{
    if (templ.trimmed().isEmpty()) {
        return "Plugins template is empty";
    }
    bool inBr = false;
    PluginRequest cur;
    QString argument;
    cur.start = false;
    for (int i=0 ; i<templ.size(); i++) {
        if (templ[i]=='!' && cur.name.isEmpty() && !inBr) {
            cur.start = true;
        }
        else if (templ[i]=='(' && !inBr) {
            inBr = true;
        }
        else if (templ[i]==')' && inBr) {
            inBr = true;
            cur.arguments << argument;
            argument = "";
        }
        else if (templ[i]==',' && inBr) {
            cur.arguments << argument;
            argument = "";
        }
        else if (templ[i]==',' && !inBr) {
            plugins << cur;
            cur.start = false;
            cur.arguments.clear();
            cur.name = "";
        }
        else if (inBr) {
            argument += templ[i];
        }
        else {
            cur.name += templ[i];
        }
    }
    plugins << cur;
    int starts = 0;

    for (QList<PluginRequest>::iterator it=plugins.begin(); it!=plugins.end(); ) {
        PluginRequest p = (*it);
        if (p.name.contains("*") || p.name.contains("?")) {
            if (p.start) {
                return "Entry point defined for masked by * name in plugins template";
            }
            QDir dir(path);
            QStringList entries = dir.entryList(QStringList() << p.name+".pluginspec", QDir::Files);
            it = plugins.erase(it);
            foreach (const QString & e, entries) {
                PluginRequest pp;
                pp.name = e.left(e.size()-11);
                pp.arguments = p.arguments;
                pp.start = false;
                plugins.insert(it, pp);
                it++;
            }
        }
        else {
            it++;
        }
    }

    for (int i=0; i<plugins.size(); i++) {
        if (plugins[i].start) {
            starts ++;
            mainPluginName = plugins[i].name;
        }
        names << plugins[i].name;
    }

    if (starts>1) {
        return "More than one entry point defined in plugins template";
    }
    if (starts==0) {
        return "Entry point is not defined in plugins template";
    }
    return "";
}

QString PluginManagerPrivate::loadSpecs(const QStringList &names, QScriptEngine * engine)
{
    for (int i=0; i<names.size(); i++) {
        bool loaded = false;
        for (int j=0; j<specs.size(); j++) {
            if (specs[j].name==names[i]) {
                loaded = true;
                break;
            }
        }
        if (loaded)
            continue;
        const QString fileName = path+"/"+names[i]+".pluginspec";
        PluginSpec spec;
#ifdef QT_NO_DEBUG
#ifdef Q_OS_WIN
        spec.libraryFileName = QString("%1/%2.dll").arg(path).arg(names[i]);
#endif
#ifdef Q_OS_UNIX
        spec.libraryFileName = QString("%1/lib%2.so").arg(path).arg(names[i]);
#endif
#ifdef Q_OS_MAC
        spec.libraryFileName = QString("%1/lib%2.dylib").arg(path).arg(names[i]);
#endif
#else
#ifdef Q_OS_WIN
        spec.libraryFileName = QString("%1/%2d.dll").arg(path).arg(names[i]);
#endif
#ifdef Q_OS_UNIX
        spec.libraryFileName = QString("%1/lib%2.so").arg(path).arg(names[i]);
#endif
#ifdef Q_OS_MAC
        spec.libraryFileName = QString("%1/lib%2_debug.dylib").arg(path).arg(names[i]);
#endif
#endif
        QString error = readSpecFromFile(fileName, spec, engine);
        if (!error.isEmpty()) {
            return error;
        }
        specs << spec;
        QStringList deps;
        for (int j=0; j<spec.dependencies.size(); j++) {
            deps << spec.dependencies[j].name;
        }
        error = loadSpecs(deps, engine);
        if (!error.isEmpty()) {
            return error;
        }
    }
    return "";
}

QString PluginManagerPrivate::loadPlugins()
{
    for (int i=0; i<specs.size(); i++) {
        QPluginLoader loader(specs[i].libraryFileName);
        if (!loader.load()) {
            return QString("Can't load module %1: %2")
                    .arg(specs[i].name)
                    .arg(loader.errorString());
        }
        KPlugin * plugin = qobject_cast<KPlugin*>(loader.instance());
        if (!plugin) {
            return QString("Plugin %1 is not valid (does not implement interface KPlugin)");
            loader.unload();
        }
        objects[i] = plugin;
        states[i] = KPlugin::Loaded;
        settings[i] = new QSettings("kumir2", specs[i].name);
        settings[i]->setIniCodec("UTF-8");
    }
    return "";
}

QString PluginManagerPrivate::makeDependencies(const QString &entryPoint,
                                               const QString &minVersion,
                                               const QString &maxVersion,
                                               QStringList &orderedList)
{
    if (orderedList.contains(entryPoint)) {
        return "";
    }
    orderedList.prepend(entryPoint);
    PluginSpec spec;
    bool found = false;
    for (int i=0; i<specs.size(); i++) {
        if (specs[i].provides.contains(entryPoint)) {
            spec = specs[i];
            orderedList.pop_front();
            orderedList.prepend(spec.name);
            found = true;
            break;
        }
    }
    if (!found) {
        return "Spec not loaded for "+entryPoint;
    }
    if (!minVersion.isEmpty()) {
        if (spec.version<minVersion) {
            return QString("Plugin %1 version too old: %2, but required at least %3")
                    .arg(entryPoint)
                    .arg(spec.version)
                    .arg(minVersion);
        }
    }
    if (!maxVersion.isEmpty()) {
        if (spec.version>maxVersion) {
            return QString("Plugin %1 version too new: %2, but required not greater %3")
                    .arg(entryPoint)
                    .arg(spec.version)
                    .arg(maxVersion);
        }
    }
    for (int i=0; i<spec.dependencies.size(); i++) {
        Dependency dep = spec.dependencies[i];
        QString error = makeDependencies(dep.name, dep.minVersion, dep.maxVersion, orderedList);
        if (!error.isEmpty()) {
            return error;
        }
    }
    return "";
}

QString PluginManager::loadPluginsByTemplate(const QString &templ)
{
    QList<PluginRequest> requests;
    QStringList names;
    QString error = "";
    error = d->parsePluginsRequest(templ, requests, names);
    if (!error.isEmpty())
        return error;
    QScriptEngine engine;
    engine.evaluate("var data = null;\n");
    error = d->loadSpecs(names, &engine);
    if (!error.isEmpty())
        return error;

    // orderedList will contain names in order of load and initialization
    QStringList orderedList;
    // make dependencies for entry point plugin first
    error = d->makeDependencies(d->mainPluginName,"","",orderedList);
    if (!error.isEmpty())
        return error;
    // make dependencies for other requests
    for (int i=0; i<requests.size(); i++) {
        error = d->makeDependencies(requests[i].name,"","",orderedList);
        if (!error.isEmpty())
            return error;
    }
    error = d->reorderSpecsAndCreateStates(orderedList);
    if (!error.isEmpty())
        return error;
    error = d->loadPlugins();
    if (!error.isEmpty())
        return error;
    d->requests = requests;
    return "";
}

bool PluginManager::isGuiRequired() const
{
    return d->specs.last().gui;
}

QString PluginManager::initializePlugins()
{
    Q_ASSERT(d->specs.size()==d->objects.size());
    for (int i=0; i<d->objects.size(); i++) {
        const QString name = d->specs[i].name;
        QStringList arguments;
        for (int j=0; j<d->requests.size(); j++) {
            if (d->requests[j].name==name) {
                arguments = d->requests[j].arguments;
            }
        }
        QString error = d->objects[i]->initialize(arguments);
        if (!error.isEmpty()) {
            return QString("Error initializing %1: %2")
                    .arg(name)
                    .arg(error);
        }
        d->states[i] = KPlugin::Initialized;
    }
    d->createSettingsDialog();
    return "";
}


QList<KPlugin*> PluginManager::loadedPlugins(const QString &pattern)
{
    QList<KPlugin*> result;
    const QRegExp rx = QRegExp(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
    for (int i=0; i<d->specs.size(); i++) {
        if (rx.exactMatch(d->specs[i].name)) {
            result << d->objects[i];
        }
    }
    return result;
}

KPlugin* PluginManager::loadedPlugin(const QString &name)
{
    for (int i=0; i<d->specs.size(); i++) {
        if (d->specs[i].name==name)
            return d->objects[i];
    }
    return 0;
}

QString PluginManager::start()
{
    KPlugin * p = d->objects.last();
    p->start();
    d->states[d->states.size()-1] = KPlugin::Started;
    return "";
}

PluginSpec PluginManager::specByObject(const KPlugin *p) const
{
    Q_ASSERT(d->specs.size()==d->objects.size());
    for (int i=0; i<d->specs.size(); i++) {
        if (d->objects[i]==p) {
            return d->specs[i];
        }
    }
    return PluginSpec();
}

KPlugin * PluginManager::dependentPlugin(const QString &name, const KPlugin *p) const
{
    PluginSpec spec;
    Q_ASSERT(d->specs.size()==d->objects.size());
    for (int i=0; i<d->specs.size(); i++) {
        if (d->objects[i]==p) {
            spec = d->specs[i];
        }
    }
    bool hasDep = false;
    QSet<QString> depAliases;
    for (int i=0; i<spec.dependencies.size(); i++) {
        for (int j=0; j<d->specs.size(); j++) {
            if (d->specs[j].provides.contains(spec.dependencies[i].name))
                depAliases |= QSet<QString>::fromList(d->specs[j].provides);
        }
    }
    for (int i=0; i<spec.dependencies.size(); i++) {
        if (depAliases.contains(spec.dependencies[i].name)) {
            hasDep = true;
            break;
        }
    }
    if (!hasDep) {
        return 0;
    }
    for (int i=0; i<d->specs.size(); i++) {
        PluginSpec ss=d->specs[i];
        if (d->specs[i].provides.contains(name)) {
            return d->objects[i];
        }
    }
    return 0;
}

KPlugin::State PluginManager::stateByObject(const KPlugin *p) const
{
    Q_ASSERT(d->states.size()==d->objects.size());
    for (int i=0; i<d->objects.size(); i++) {
        if (d->objects[i]==p) {
            return d->states[i];
        }
    }
    return KPlugin::Disabled;
}

QSettings * PluginManager::settingsByObject(const KPlugin *p) const
{
    Q_ASSERT(d->settings.size()==d->objects.size());
    for (int i=0; i<d->objects.size(); i++) {
        if (d->objects[i]==p) {
            return d->settings[i];
        }
    }
    return 0;
}

PluginManager * PluginManager::m_instance = 0;

PluginManager * PluginManager::instance()
{
    return m_instance;
}

void PluginManager::shutdown()
{
    for (int i=d->objects.size()-1; i>=0; i--) {
        KPlugin * p = d->objects[i];
        p->stop();
        d->states[i] = KPlugin::Stopped;
        d->settings[i]->sync();
    }
}

void PluginManagerPrivate::changeWorkingDirectory(const QString &path)
{
    for (int i=0; i<objects.size(); i++) {
        settings[i]->sync();
        settings[i]->deleteLater();
    }
    QDir::root().mkpath(path);
    QDir::setCurrent(path);
    QDir::current().mkdir(".settings");
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, path+"/.settings");
    for (int i=0; i<objects.size(); i++) {
        KPlugin * p = objects[i];
        settings[i] = new QSettings(path+"/.settings/"+specs[i].name+".conf", QSettings::IniFormat);
        settings[i]->setIniCodec("UTF-8");
        settings[i]->sync();
        p->changeCurrentDirectory(path);
        p->reloadSettings();
    }
}

void PluginManager::switchGlobalState(GlobalState state)
{
    for (int i=0; i<d->objects.size(); i++) {
        KPlugin * p = d->objects[i];
        p->changeGlobalState(d->globalState, state);
    }
    d->globalState = state;
}

GlobalState PluginManager::currentGlobalState() const
{
    return d->globalState;
}

} // namespace ExtensionSystem
