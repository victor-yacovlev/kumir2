#ifndef EDITOR_PLUGIN_H
#define EDITOR_PLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/editorinterface.h"

namespace Editor {

class EditorPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::EditorInterface

{
    Q_OBJECT
    Q_INTERFACES(Shared::EditorInterface)
public:
    EditorPlugin();
    ~EditorPlugin();

    struct Shared::EditorComponent newDocument(const QString &analizerName = "Analizer"
                                                     , const QString &initialText = "");

    void closeDocument(int documentId);
    ExtensionSystem::SettingsEditorPage settingsEditorPage();
    bool hasUnsavedChanges(int documentId) const;
    void setDocumentChangesSaved(int documentId);
    QString saveDocument(int documentId, const QString & fileName);
    Shared::AnalizerInterface * analizer(int documentId);
protected:
    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    struct EditorPluginPrivate * d;

};

}

#endif
