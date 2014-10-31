#ifndef EDITOR_H
#define EDITOR_H

#include "extensionsystem/settings.h"
#include "macro.h"
#include "dataformats/kumfile.h"
#include "extensionsystem/settings.h"
#include "docbookviewer/docbookview.h"
#include "interfaces/analizerinterface.h"
#include "editorplugin.h"

#include "interfaces/editor_instanceinterface.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace Editor {

class EditorInstance
        : public QWidget
        , public Shared::Editor::InstanceInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::Editor::InstanceInterface)
public:
    friend class EditorPlane;
    friend class TextCursor;
    friend class TextDocument;

    explicit EditorInstance(
            EditorPlugin * plugin,
            bool initiallyNotSaved = false,
            Shared::AnalizerInterface * analizer = 0,
            Shared::Analizer::InstanceInterface * analizerInstance = 0
            );

    ~EditorInstance();

    QScrollBar * scrollBar(Qt::Orientation orientation);

    bool isTeacherMode() const;
    QList<QAction*> toolBarActions() const;
    QSize minimumSizeHint() const;
    QList<QMenu*> menus() const;

    Shared::Analizer::SourceFileInterface::Data documentContents() const;

    bool hasBreakpointSupport() const;

    void loadDocument(QIODevice * device,
                              const QString & fileNameSuffix = "",
                              const QString & sourceEncoding = "",
                              const QUrl & sourceUrl = QUrl()
            ) /* throws QString */;
    void loadDocument(const QString & fileName) /* throws QString */;
    void loadDocument(const Shared::Analizer::SourceFileInterface::Data &data) /* throws QString */;

    void saveDocument(const QString &fileName);
    void saveDocument(QIODevice * device);
    uint32_t currentLineNumber() const;
    void setKumFile(const Shared::Analizer::SourceFileInterface::Data & data);
    void setPlainText(const QString & data);
    void setDocumentId(int id);

    quint32 errorLinesCount() const;

    void highlightLineGreen(int lineNo, quint32 colStart, quint32 colEnd);
    void highlightLineRed(int lineNo, quint32 colStart, quint32 colEnd);
    void unhighlightLine();

    const class TextCursor * cursor() const;
    const class TextDocument * document() const;
    Shared::Analizer::InstanceInterface * analizer();
    class TextCursor * cursor();
    class TextDocument * document();
    bool isModified() const;
    void appendMarginText(int lineNo, const QString & text);
    void setMarginText(int lineNo, const QString & text, const QColor &fgColor);
    void clearMarginText();
    void clearMarginText(int fromLine, int toLine);
    void setNotModified();
    void checkForClean();
    void lock();
    void unlock();
    void setLineHighlighted(int lineNo, const QColor & color, quint32 colStart, quint32 colEnd);
    void ensureAnalized();   
    void unsetAnalizer();
    bool forceNotSavedFlag() const;
    void setForceNotSavedFlag(bool v);
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
    inline QWidget * widget() { return this; }
    inline bool supportsContextHelp() const {
        return nullptr!=analizerInstance_ && nullptr!=analizerInstance_->helper();
    }
    Shared::Analizer::ApiHelpItem contextHelpItem() const;

    QAction * toggleBreakpointAction() const;
    QList<Shared::Editor::Breakpoint> breakpoints() const;

public slots:
    void undo();
    void redo();

    void changeGlobalState(quint32 prevv, quint32 currentt);
    void updateSettings(const QStringList & keys);
    void updateInsertMenu();
    bool tryEscKeyAction(const QString & text);
    void toggleBreakpoint();

signals:
    void urlsDragAndDropped(const QList<QUrl> &);
    void documentCleanChanged(bool v);
    void cursorPositionChanged(uint row, uint column);
    void keyboardLayoutChanged(QLocale::Language lang, bool capslock, bool shift, bool alt);
    void message(const QString &);
    void requestHelpForAlgorithm(const QString & package, const QString & function);
    void recordMacroChanged(bool on);

    void breakpointCnagedOrInserted(bool enabled, quint32 lineNo, quint32 ignoreCount, const QString & condition);
    void breakpointRemoved(quint32 lineNo);


private slots:
    void updatePosition(int row, int col);
    void handleCompleteCompilationRequiest(
        const QStringList & visibleText,
        const QStringList & hiddenText,
        int hiddenBaseLine
        );
    void playMacro();
    void handleAutoScrollChange(char a);
    void handleAutoScrollChangeX(char a);
    void disableInsertActions();
    void enableInsertActions();
    void toggleRecordMacro(bool on);
    void editMacros();

private /* methods */:
    ExtensionSystem::SettingsPtr mySettings() const;
    void setupUi();
    void createConnections();
    void setupStyleSheets();


    void focusInEvent(QFocusEvent *e);
    void loadMacros();    
    void createActions();
    void updateFromAnalizer();
    void timerEvent(QTimerEvent *e);

private /* fields */:

    EditorPlugin * plugin_;
    Shared::AnalizerInterface * analizerPlugin_;
    Shared::Analizer::InstanceInterface * analizerInstance_;

    class TextDocument * doc_;
    class TextCursor * cursor_;
    class EditorPlane * plane_;

    QScrollBar * horizontalScrollBar_;
    QScrollBar * verticalScrollBar_;

    class FindReplace * findReplace_;
    QScopedPointer<class SuggestionsWindow> autocompleteWidget_;

    QAction * copy_;
    QAction * paste_;
    QAction * cut_;
    QAction * selectAll_;
    QAction * deleteLine_;
    QAction * deleteTail_;
    QAction * toggleComment_;

    QAction * find_;
    QAction * replace_;

    QAction * undo_;
    QAction * redo_;

    QAction * recordMacro_;
    QAction * editMacros_;

    QMenu * editMenu_;
    QMenu * insertMenu_;

    QAction * separatorAction_;
    mutable QAction * toggleBreakpoint_;

    QList<Macro> systemMacros_;
    QList<Macro> userMacros_;

    int timerId_;
    int autoScrollTimerId_;
    char autoScrollStateX_;
    char autoScrollStateY_;

    bool notSaved_;

    QUrl documentUrl_;
};


} // namespace Editor

#endif // EDITOR_H
