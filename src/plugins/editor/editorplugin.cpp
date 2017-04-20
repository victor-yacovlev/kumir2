#include <kumir2-libs/extensionsystem/pluginmanager.h>

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "editorplugin.h"
#include "editor.h"
#include "settingspage.h"

#include <kumir2/guiinterface.h>

namespace Editor {

using namespace Shared;


EditorPlugin::EditorPlugin()
{
    editors_ = QVector< Ed > ( 128, Ed(0,0,-1));
    settingsPage_ = 0;
    teacherMode_ = false;
    bundledFontsLoaded_ = false;
//    helpViewer_ = nullptr;
}

EditorPlugin::~EditorPlugin()
{
    foreach (Ed e, editors_) {
        if (e.e)
            delete e.e;
    }
    if (settingsPage_) {
        delete settingsPage_;
    }
}



EditorInstance::InstanceInterface * EditorPlugin::newDocument(
        const QString & canonicalLanguageName,
        const QString & documentDir)
{
    Shared::AnalizerInterface * analizerPlugin = nullptr;
    Shared::Analizer::InstanceInterface * analizerInstance = nullptr;

    QList<Shared::AnalizerInterface*> analizers =
            ExtensionSystem::PluginManager::instance()
            ->findPlugins<Shared::AnalizerInterface>();

    for (int i=0; i<analizers.size(); i++) {
        if (analizers[i]->defaultDocumentFileNameSuffix() == canonicalLanguageName) {
            analizerPlugin = analizers[i];
            analizerInstance = analizerPlugin->createInstance();
            analizerInstance->setSourceDirName(documentDir);
            break;
        }
    }

    EditorInstance * editor = new EditorInstance(this, true, analizerPlugin, analizerInstance);
    connectGlobalSignalsToEditor(editor);

    if (analizerPlugin) {
        QString initialTextFileName =
                mySettings()->value(SettingsPage::KeyProgramTemplateFile,
                                    SettingsPage::DefaultProgramTemplateFile
                                    + "." + analizerPlugin->defaultDocumentFileNameSuffix()
                                    ).toString();
        static const QString resourcesRoot = QDir(qApp->applicationDirPath()+"/../share/kumir2/").canonicalPath();
        initialTextFileName.replace("${RESOURCES}", resourcesRoot);
        QFile f(initialTextFileName);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            const QByteArray bytes = f.readAll();
            f.close();
            const Shared::Analizer::SourceFileInterface::Data data =
                    analizerPlugin->sourceFileHandler()->fromBytes(bytes);
            editor->setKumFile(data);
        }
        else {
            Shared::Analizer::SourceFileInterface::Data empty;
            editor->setKumFile(empty);
        }
    }

    editor->setNotModified();
    return editor;
}

Shared::Editor::InstanceInterface * EditorPlugin::loadDocument(const Shared::Analizer::SourceFileInterface::Data &data, QString * error)
{
    EditorInstance * editor = new EditorInstance(this, true, nullptr, nullptr);
    connectGlobalSignalsToEditor(editor);
    editor->loadDocument(data, error);
    return editor;
}

Shared::Editor::InstanceInterface * EditorPlugin::loadDocument(
        QIODevice *device,
        const QString &fileNameSuffix,
        const QString &sourceEncoding,
        const QUrl & sourceUrl, QString * error
        )
{
    Shared::AnalizerInterface * analizerPlugin = nullptr;

    QList<Shared::AnalizerInterface*> analizers =
            ExtensionSystem::PluginManager::instance()
            ->findPlugins<Shared::AnalizerInterface>();

    for (int i=0; i<analizers.size(); i++) {
        const QString suffix = analizers[i]->defaultDocumentFileNameSuffix();
        if (suffix == fileNameSuffix) {
            analizerPlugin = analizers[i];
            break;
        }
    }

    EditorInstance * editor = new EditorInstance(this, true, analizerPlugin, nullptr);
    connectGlobalSignalsToEditor(editor);
    editor->loadDocument(device, fileNameSuffix, sourceEncoding, sourceUrl, error);
    return editor;
}

Shared::Editor::InstanceInterface * EditorPlugin::loadDocument(const QString &fileName, QString * error)
{
    Shared::AnalizerInterface * analizerPlugin = nullptr;

    QList<Shared::AnalizerInterface*> analizers =
            ExtensionSystem::PluginManager::instance()
            ->findPlugins<Shared::AnalizerInterface>();

    for (int i=0; i<analizers.size(); i++) {
        const QString suffix = "." + analizers[i]->defaultDocumentFileNameSuffix();
        if (fileName.endsWith(suffix)) {
            analizerPlugin = analizers[i];
            break;
        }
    }

    EditorInstance * editor = new EditorInstance(this, true, analizerPlugin, nullptr);
    connectGlobalSignalsToEditor(editor);
    editor->loadDocument(fileName, error);
    return editor;
}

int EditorPlugin::analizerDocumentId(int editorDocumentId) const
{
    Ed ed = editors_[editorDocumentId];
    return ed.id;
}

void EditorPlugin::updateSettings(const QStringList & keys)
{
    if (settingsPage_) {
        settingsPage_->changeSettings(mySettings());
    }

    emit settingsUpdateRequest(keys);
}

QFont EditorPlugin::defaultEditorFont() const
{
    QFont fnt;
    if (!bundledFontsLoaded_)
        fnt = QApplication::font();
    else
        fnt = ptMonoFont_;
    int fntSize = mySettings()->value(SettingsPage::KeyFontSize, SettingsPage::DefaultFontSize).toInt();
    Shared::GuiInterface * gui = ExtensionSystem::PluginManager::instance()->findPlugin<Shared::GuiInterface>();
    if (gui && gui->overridenEditorFontSize()>0) {
        fntSize = gui->overridenEditorFontSize();
    }
    fnt.setPointSize(fntSize);
    return fnt;
}

void EditorPlugin::createPluginSpec()
{
    _pluginSpec.name = "Editor";
    _pluginSpec.dependencies.append("Analizer");
    _pluginSpec.gui = true;
}


QString EditorPlugin::initialize(const QStringList &arguments, const ExtensionSystem::CommandLine &)
{
    if (arguments.contains("teacher"))
        teacherMode_ = true;
    else
        teacherMode_ = false;    
    const QDir fontsDir = myResourcesDir();
    const QStringList ttfFiles = fontsDir.entryList(QStringList() << "*.ttf" << "*.otf");
    foreach (const QString & fileName, ttfFiles) {
        const QString filePath = fontsDir.absoluteFilePath(fileName);
        int id = QFontDatabase::addApplicationFont(filePath);
        if (id == -1) {
            qWarning() << "Can't load font " << filePath;
        }
    }
    bundledFontsLoaded_ = true;
    ptMonoFont_ = QFont("PT Mono");
    return 0;
}

void EditorPlugin::start()
{

}

QWidget* EditorPlugin::settingsEditorPage()
{
    settingsPage_ = new SettingsPage(mySettings());
    settingsPage_->setWindowTitle(tr("Editor"));
    connect(settingsPage_, SIGNAL(settingsChanged(QStringList)),
            this, SIGNAL(settingsUpdateRequest(QStringList)), Qt::DirectConnection);
    return settingsPage_;
}

void EditorPlugin::stop()
{

}


void EditorPlugin::changeGlobalState(ExtensionSystem::GlobalState prev, ExtensionSystem::GlobalState current)
{
    emit globalStateUpdateRequest(quint32(prev), quint32(current));
}

void EditorPlugin::connectGlobalSignalsToEditor(EditorInstance *editor)
{
    connect(this, SIGNAL(settingsUpdateRequest(QStringList)),
            editor, SLOT(updateSettings(QStringList)), Qt::DirectConnection);

    connect(this, SIGNAL(globalStateUpdateRequest(quint32,quint32)),
            editor, SLOT(changeGlobalState(quint32, quint32)), Qt::DirectConnection);

    connect(this, SIGNAL(updateInsertMenuRequest()),
            editor, SLOT(updateInsertMenu()), Qt::DirectConnection);

    QList<ExtensionSystem::KPlugin*> actors = ExtensionSystem::PluginManager
            ::instance()->loadedPlugins("Actor*");

    Q_FOREACH(QObject* actor, actors) {
        connect(actor, SIGNAL(notifyOnTemplateParametersChanged()),
                editor, SLOT(forceCompleteCompilation()));
    }
}


void EditorPlugin::updateUserMacros(const QString & analizerName, const QList<QSharedPointer<Macro> > &macros, bool rewrite)
{
    if (rewrite) {
        QString fileName = analizerName.length() > 0
                ? QString::fromLatin1(".user-macros-%1.xml").arg(analizerName)
                : QString::fromLatin1(".user-macros.xml");

        const QString dirName = mySettings()->locationDirectory();
        if (dirName.startsWith(QDir::homePath() + "/."))
            fileName.remove(0, 1);
        QDir dir(dirName);
        if (!dir.exists()) {
            dir.mkpath("");
        }

        const QString filePath = dir.absoluteFilePath(fileName);
        if (macros.size() > 0) {
            saveToFile(filePath, macros);
        }
        else {
            QFile::remove(filePath);
        }
    }

    emit updateInsertMenuRequest();
}

}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(Editor::EditorPlugin)
#endif
