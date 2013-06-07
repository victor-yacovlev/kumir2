#ifndef EDITOR_PLUGIN_H
#define EDITOR_PLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/editorinterface.h"
#include "dataformats/kumfile.h"

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

    struct Shared::EditorComponent newDocument(const QString &analizerName
            , const QString &documentDir
            , bool initiallyNotSaved);

    int analizerDocumentId(int editorDocumentId) const;
    void closeDocument(int documentId);
    QWidget* settingsEditorPage();
    bool hasUnsavedChanges(int documentId) const;
    void setDocumentChangesSaved(int documentId);
    QString loadDocument(int documentId, const QString & fileName, bool keepIndents);
    QString loadDocument(int documentId, const KumFile::Data &data);
    QString saveDocument(int documentId, const QString & fileName);
    KumFile::Data documentContent(int documentId) const;
    Shared::AnalizerInterface * analizer(int documentId);
    quint32 errorsLinesCount(int documentId) const;
    void highlightLineGreen(int documentId, int lineNo, quint32 colStart, quint32 colEnd);
    void highlightLineRed(int documentId, int lineNo, quint32 colStart, quint32 colEnd);
    void unhighlightLine(int documentId);
    void appendMarginText(int documentId, int lineNo, const QString & text);
    void setMarginText(int documentId, int lineNo, const QString & text, const QColor & fgColor);
    void clearMargin(int documentId);
    void clearMargin(int documentId, int fromLine, int toLine);
    void ensureAnalized(int documentId);
    QByteArray saveState(int documentId);
    void restoreState(int documentId, const QByteArray & data);
    void updateSettings();
protected:
    QString initialize(const QStringList &arguments);
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    inline void changeCurrentDirectory(const QString & path) { Q_UNUSED(path); updateSettings(); }
    void start();
    void stop();
private:
    struct EditorPluginPrivate * d;

};

}

#endif
