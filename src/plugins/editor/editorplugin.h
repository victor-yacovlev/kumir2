#ifndef EDITOR_PLUGIN_H
#define EDITOR_PLUGIN_H

#include "extensionsystem/kplugin.h"
#include "interfaces/editorinterface.h"
#include "dataformats/kumfile.h"
#include "macro.h"

namespace Editor {

class EditorPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::EditorInterface

{
    Q_OBJECT
    Q_INTERFACES(Shared::EditorInterface)
public:
    friend class Editor;
    EditorPlugin();
    ~EditorPlugin();

    struct Shared::EditorComponent newDocument(const QString &analizerName
            , const QString &documentDir
            , bool initiallyNotSaved);

    void setDocBookViewer(DocBookViewer::DocBookView * viewer);
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
    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine &);
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    inline void changeCurrentDirectory(const QString & path) { currentDirectory_ = path; updateSettings(); updateUserMacros(QString(), QList<Macro>(), false);}
    void start();
    void stop();
    void updateUserMacros(const QString & analizerName, const QList<Macro> & macros, bool rewrite);
private:
    struct Ed {
        inline Ed() { e = 0; a=0; id =-1; }
        inline Ed(class Editor *ee, Shared::AnalizerInterface *aa, int i) { a=aa;e=ee;id=i; }
        class Editor * e;
        Shared::AnalizerInterface * a;
        int id;
    };

    QVector< Ed > editors_;
    class SettingsPage * settingsPage_;
    bool teacherMode_;
    DocBookViewer::DocBookView * helpViewer_;
    QString currentDirectory_;
};

}

#endif
