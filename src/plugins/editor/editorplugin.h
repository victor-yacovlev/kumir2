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
    quint32 errorsCount(int documentId) const;
    void highlightLineGreen(int documentId, int lineNo);
    void highlightLineRed(int documentId, int lineNo);
    void unhighlightLine(int documentId);
    void appendMarginText(int documentId, int lineNo, const QString & text);
    void setMarginText(int documentId, int lineNo, const QString & text);
    void clearMargin(int documentId);
    void clearMargin(int documentId, int fromLine, int toLine);
    void ensureAnalized(int documentId);
    QString saveState(int documentId);
    void restoreState(int documentId, const QString & data);
    void updateSettings();
protected:
    QString initialize(const QStringList &arguments);
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void start();
    void stop();
private:
    struct EditorPluginPrivate * d;

};

}

#endif
