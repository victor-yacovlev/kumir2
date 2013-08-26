#include "pluginmanager_impl.h"

namespace ExtensionSystem {

QString PluginManagerImpl::loadSpecs(const QStringList &names)
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
        //#ifdef QT_NO_DEBUG
#ifdef Q_OS_WIN
        spec.libraryFileName = QString("%1/%2.dll").arg(path).arg(names[i]);
#endif
#ifdef Q_OS_UNIX
        spec.libraryFileName = QString("%1/lib%2.so").arg(path).arg(names[i]);
#endif
#ifdef Q_OS_MAC
        spec.libraryFileName = QString("%1/lib%2.dylib").arg(path).arg(names[i]);
#endif
        //#else
        //#ifdef Q_OS_WIN
        //        spec.libraryFileName = QString("%1/%2d.dll").arg(path).arg(names[i]);
        //#endif
        //#ifdef Q_OS_UNIX
        //        spec.libraryFileName = QString("%1/lib%2.so").arg(path).arg(names[i]);
        //#endif
        //#ifdef Q_OS_MAC
        //        spec.libraryFileName = QString("%1/lib%2_debug.dylib").arg(path).arg(names[i]);
        //#endif
        //#endif
        QString error = readSpecFromFile(fileName, spec);
        if (!error.isEmpty()) {
            return error;
        }
        specs << spec;
        QStringList deps;
        for (int j=0; j<spec.dependencies.size(); j++) {
            deps << spec.dependencies[j];
        }
        error = loadSpecs(deps);
        if (!error.isEmpty()) {
            return error;
        }
    }
    return "";
}

QString PluginManagerImpl::loadPlugins()
{
    for (int i=0; i<specs.size(); i++) {
        QPluginLoader loader(specs[i].libraryFileName);
        //        qDebug()<<specs[i].libraryFileName;
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
        settings[i] = SettingsPtr(new Settings(specs[i].name));
        plugin->updateSettings();
    }
    return "";
}

QString PluginManagerImpl::makeDependencies(const QString &entryPoint,
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
        qDebug()<<"Spec not loaded for |"+entryPoint+"|";
        return "Spec not loaded for "+entryPoint;
    }

    for (int i=0; i<spec.dependencies.size(); i++) {
        QString dep = spec.dependencies[i];
        QString error = makeDependencies(dep, orderedList);
        if (!error.isEmpty()) {
            return error;
        }
    }
    return "";
}

bool PluginManagerImpl::extractRuntimeParametersForPlugin(const KPlugin *plugin, CommandLine &parameters)
{
    parameters = CommandLine(plugin->acceptableCommandLineParameters());

    // Eat named parameters
    for (int i=0; i<parameters.data_.size(); i++) {
        CommandLineParameter & param = parameters.data_[i];
        typedef std::list<QString>::iterator It;
        for (It it = namedProgramArguments.begin(); it != namedProgramArguments.end(); ++it) {
            QString & arg = *it;
            if (arg.startsWith("--"+param.longName_) || arg.startsWith("-"+QString(param.shortName_))) {
                param.fillValue(arg);
                namedProgramArguments.erase(it);
                break;
            }
        }
    }

    // Eat unnamed parameters in case if plugin is startup module
    if (plugin == objects.last()) {
        CommandLineParameter * chain = nullptr;
        for (int i=0; i<parameters.data_.size(); i++) {
            CommandLineParameter & param = parameters.data_[i];
            if (param.shortDescription_.length() > 0) {
                if (unnamedProgramArguments.size() > 0) {
                    param.value_ = unnamedProgramArguments.front();
                    unnamedProgramArguments.pop_front();
                }
                chain = param.valueRequired_ ? nullptr : &param;
            }
        }

        // Eat a list of unnamed parameters
        while (chain && unnamedProgramArguments.size() > 0) {
            CommandLineParameter param(
                        chain->allowInGui_,
                        chain->shortDescription_,
                        chain->description_,
                        chain->acceptType_,
                        false);
            param.value_ = unnamedProgramArguments.front();
            unnamedProgramArguments.pop_front();
        }
    }

    bool success = true;
    for (int i=0; i<parameters.data_.size(); i++) {
        CommandLineParameter & param = parameters.data_[i];
        if (param.shortDescription_.length() > 0) {
            success = success && (param.isValid() || plugin!=objects.last());
        }
        else {
            success = success && param.isValid();
        }
    }

    return success;
}

void PluginManagerImpl::changeWorkingDirectory(const QString &path)
{
    workspacePath = path;
    for (int i=0; i<objects.size(); i++) {
        KPlugin * p = objects[i];
        SettingsPtr s = settings[i];
        if (s) {
            p->saveSession();
            s->changeWorkingDirectory(path);
        }
    }
    QDir::root().mkpath(path);
    QDir::setCurrent(path);
    QDir::current().mkdir(".settings");
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, path+"/.settings");
    for (int i=0; i<objects.size(); i++) {
        KPlugin * p = objects[i];
        p->changeCurrentDirectory(path);
        p->updateSettings();
        p->restoreSession();
    }
    mySettings->setValue(PluginManager::CurrentWorkspaceKey, path);
}

QString PluginManagerImpl::reorderSpecsAndCreateStates(const QStringList &orderedList)
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
#ifndef Q_OS_MAC
        const QString applicationLanucher = qApp->arguments().at(0);
        QString applicationName = applicationLanucher;
        if (applicationLanucher.startsWith(qApp->applicationDirPath())) {
            applicationName = applicationLanucher.mid(qApp->applicationDirPath().length() + 1);
        }
#else
        QString applicationName = "kumir2";
#endif
        settings << SettingsPtr(new Settings(spec.name));
    }
    specs = newSpecs;
    return "";
}

QString PluginManagerImpl::parsePluginsRequest(const QString &templ, QList<PluginRequest> &plugins, QStringList & names)
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
            inBr = false;
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
    // Qt bug !!! Use STL instead and then convert back to QList
    std::list<PluginRequest> stdPlugins = plugins.toStdList();
    std::list<PluginRequest>::iterator it=stdPlugins.begin();
    while (it!=stdPlugins.end()) {
        PluginRequest p = (*it);
        if (p.name.contains("*") || p.name.contains("?")) {
            if (p.start) {
                return "Entry point defined for masked by * name in plugins template";
            }
            QDir dir(path);
            QStringList entries = dir.entryList(QStringList() << p.name+".pluginspec", QDir::Files);
            it = stdPlugins.erase(it);
            foreach (const QString & e, entries) {
                PluginRequest pp;
                pp.name = e.left(e.size()-11);
                pp.arguments = p.arguments;
                pp.start = false;
                stdPlugins.insert(it, pp);
            }
        }
        else {
            ++it;
        }
    }
    plugins = QList<PluginRequest>::fromStdList(stdPlugins);
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




} // namespace ExtensionSystem
