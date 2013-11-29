#include "extensionsystem/pluginmanager.h"

#include <QtCore>
#include <QtGui>

#include "editorplugin.h"
#include "editor.h"
#include "settingspage.h"

namespace Editor {

using namespace Shared;


EditorPlugin::EditorPlugin()
{
    editors_ = QVector< Ed > ( 128, Ed(0,0,-1));
    settingsPage_ = 0;
    teacherMode_ = false;
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



Editor::InstanceInterface * EditorPlugin::newDocument(
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

    Editor * editor = new Editor(this, true, analizerPlugin, analizerInstance);
    connectGlobalSignalsToEditor(editor);

    if (analizerPlugin) {
        QString initialTextFileName =
                mySettings()->value(SettingsPage::KeyProgramTemplateFile
                                    + "." + analizerPlugin->defaultDocumentFileNameSuffix(),
                                    SettingsPage::DefaultProgramTemplateFile
                                    + "." + analizerPlugin->defaultDocumentFileNameSuffix()
                                    ).toString();
        static const QString resourcesRoot = QDir(qApp->applicationDirPath()+"/../share/kumir2/").canonicalPath();
        initialTextFileName.replace("${RESOURCES}", resourcesRoot);
        QFile f(initialTextFileName);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            const QByteArray bytes = f.readAll();
            f.close();
            const KumFile::Data data =
                    KumFile::fromString(
                        KumFile::readRawDataAsString(
                            bytes,
                            QString(),
                            analizerPlugin->defaultDocumentFileNameSuffix()
                            )
                        );
            editor->setKumFile(data);
        }
    }

    editor->setNotModified();
    return editor;
}

Shared::Editor::InstanceInterface * EditorPlugin::loadDocument(const KumFile::Data &data)
{
    Editor * editor = new Editor(this, true, nullptr, nullptr);
    connectGlobalSignalsToEditor(editor);
    editor->loadDocument(data);
    return editor;
}

Shared::Editor::InstanceInterface * EditorPlugin::loadDocument(
        QIODevice *device,
        const QString &fileNameSuffix,
        const QString &sourceEncoding,
        const QUrl & sourceUrl
        )
{
    Editor * editor = new Editor(this, true, nullptr, nullptr);
    connectGlobalSignalsToEditor(editor);
    editor->loadDocument(device, fileNameSuffix, sourceEncoding, sourceUrl);
    return editor;
}

Shared::Editor::InstanceInterface * EditorPlugin::loadDocument(const QString &fileName)
{
    Editor * editor = new Editor(this, true, nullptr, nullptr);
    connectGlobalSignalsToEditor(editor);
    editor->loadDocument(fileName);
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


QString EditorPlugin::initialize(const QStringList &arguments, const ExtensionSystem::CommandLine &)
{
    if (arguments.contains("teacher"))
        teacherMode_ = true;
    else
        teacherMode_ = false;
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

void EditorPlugin::connectGlobalSignalsToEditor(Editor *editor)
{
    connect(this, SIGNAL(settingsUpdateRequest(QStringList)),
            editor, SLOT(updateSettings(QStringList)), Qt::DirectConnection);

    connect(this, SIGNAL(globalStateUpdateRequest(quint32,quint32)),
            editor, SLOT(changeGlobalState(quint32, quint32)), Qt::DirectConnection);

    connect(this, SIGNAL(updateInsertMenuRequest()),
            editor, SLOT(updateInsertMenu()), Qt::DirectConnection);
}


void EditorPlugin::updateUserMacros(const QString & analizerName, const QList<Macro> &macros, bool rewrite)
{
    if (rewrite) {
        QString fileName = analizerName.length() > 0
                ? QString::fromAscii(".user-macros-%1.xml").arg(analizerName)
                : QString::fromAscii(".user-macros.xml");

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


Q_EXPORT_PLUGIN(Editor::EditorPlugin)
