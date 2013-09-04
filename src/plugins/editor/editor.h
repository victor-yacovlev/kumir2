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

#include <QtGui>

namespace Editor {

class Editor
        : public QWidget
        , public Shared::Editor::InstanceInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::Editor::InstanceInterface)
public:
    friend class EditorPlane;
    friend class TextCursor;
    friend class TextDocument;

    explicit Editor(
            EditorPlugin * plugin,
            bool initiallyNotSaved = false,
            Shared::AnalizerInterface * analizer = 0,
            Shared::Analizer::InstanceInterface * analizerInstance = 0
            );

    ~Editor();

    QScrollBar * scrollBar(Qt::Orientation orientation);

    bool isTeacherMode() const;
    QList<QAction*> toolBarActions() const;
    QSize minimumSizeHint() const;
    QList<QMenu*> menus() const;

    KumFile::Data documentContents() const;

    void loadDocument(QIODevice * device,
                              const QString & fileNameSuffix = "",
                              const QString & sourceEncoding = "",
                              const QUrl & sourceUrl = QUrl()
            ) /* throws QString */;
    void loadDocument(const QString & fileName) /* throws QString */;
    void loadDocument(const KumFile::Data &data) /* throws QString */;

    void saveDocument(const QString &fileName) const;
    void saveDocument(QIODevice * device) const;
    void setKumFile(const KumFile::Data & data);
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
    QByteArray saveState() const;
    void restoreState(const QByteArray &data);
    void unsetAnalizer();
    bool forceNotSavedFlag() const;
    void setForceNotSavedFlag(bool v);
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
    void updateInsertMenu();
    inline QWidget * widget() { return this; }
public slots:
    void undo();
    void redo();

    void changeGlobalState(quint32 prevv, quint32 currentt);
    void updateSettings(const QStringList & keys);

signals:
    void urlsDragAndDropped(const QList<QUrl> &);
    void documentCleanChanged(bool v);
    void cursorPositionChanged(uint row, uint column);
    void keyboardLayoutChanged(QLocale::Language lang, bool capslock, bool shift, bool alt);
    void message(const QString &);
    void requestHelpForAlgorithm(const QString & text);
    void recordMacroChanged(bool on);


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
    class SuggestionsWindow * autocompleteWidget_;

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

    QList<Macro> systemMacros_;
    QList<Macro> userMacros_;

    int timerId_;
    int autoScrollTimerId_;
    char autoScrollStateY_;
    char autoScrollStateX_;

    bool notSaved_;

    mutable QUrl documentUrl_;
};

QDataStream & operator<< (QDataStream & stream, const Editor & editor);
QDataStream & operator>> (QDataStream & stream, Editor & editor);

} // namespace Editor

#endif // EDITOR_H
