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
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "kumir2.Editor" FILE "")
#endif
    Q_INTERFACES(Shared::EditorInterface)
public:
    friend class EditorInstance;
    friend class EditorPlane;
    EditorPlugin();
    ~EditorPlugin();

    Shared::Editor::InstanceInterface * newDocument(
            const QString & canonicalLanguageName = "",
            const QString & documentDir = "");

    Shared::Editor::InstanceInterface * loadDocument(
            QIODevice * device,
            const QString & fileNameSuffix,
            const QString & sourceEncoding,
            const QUrl & sourceUrl, QString * error
            ) override;

    Shared::Editor::InstanceInterface * loadDocument(
            const QString & fileName, QString * error) override;

    Shared::Editor::InstanceInterface * loadDocument(
            const Shared::Analizer::SourceFileInterface::Data &data, QString * error) override;

    int analizerDocumentId(int editorDocumentId) const;
    void closeDocument(int documentId);
    QWidget* settingsEditorPage();
    bool hasUnsavedChanges(int documentId) const;
    void setDocumentChangesSaved(int documentId);
    QString loadDocument(int documentId, const QString & fileName, bool keepIndents, QString * error);
    QString loadDocument(int documentId, const Shared::Analizer::SourceFileInterface::Data &data, QString * error);
    QString saveDocument(int documentId, const QString & fileName, QString * error);
    Shared::Analizer::SourceFileInterface::Data documentContent(int documentId) const;
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
    void updateSettings(const QStringList & keys);

signals:
    void settingsUpdateRequest(const QStringList & keys);
    void globalStateUpdateRequest(quint32, quint32);
    void updateInsertMenuRequest();

protected:

    void connectGlobalSignalsToEditor(class EditorInstance * editor);

    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine &);
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    inline void changeCurrentDirectory(const QString & path) {
        currentDirectory_ = path;
        updateSettings(QStringList());
        updateUserMacros(QString(), QList<Macro>(), false);
    }
    void start();
    void stop();
    void updateUserMacros(const QString & analizerName, const QList<Macro> & macros, bool rewrite);
private:
    struct Ed {
        inline Ed() { e = 0; a=0; id =-1; }
        inline Ed(class EditorInstance *ee, Shared::AnalizerInterface *aa, int i) { a=aa;e=ee;id=i; }
        class EditorInstance * e;
        Shared::AnalizerInterface * a;
        int id;
    };

    QVector< Ed > editors_;
    class SettingsPage * settingsPage_;
    bool teacherMode_;
//    DocBookViewer::DocBookView * helpViewer_;
    QString currentDirectory_;
};

}

#endif
