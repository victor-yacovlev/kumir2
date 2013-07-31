#ifndef EDITOR_H
#define EDITOR_H

#include "macro.h"
#include "dataformats/kumfile.h"
#include "extensionsystem/settings.h"
#include "docbookviewer/docbookview.h"
#include "interfaces/analizerinterface.h"
#include "editorplugin.h"

#include <QtGui>

namespace Editor {

class Editor
        : public QWidget
{
    Q_OBJECT
public:
    friend class EditorPlane;
    friend class TextCursor;
    explicit Editor(
            EditorPlugin * plugin,
            bool initiallyNotSaved = false,
            ExtensionSystem::SettingsPtr settings = ExtensionSystem::SettingsPtr(),
            Shared::AnalizerInterface * analizer = 0,
            int documentId = 0,
            QWidget *parent = 0
            );
    ~Editor();
    void setHelpViewer(DocBookViewer::DocBookView * viewer);
    void setTeacherMode(bool v);
    bool isTeacherMode() const;
    QList<QAction*> toolbarActions();
    QSize minimumSizeHint() const;
    void setSettings(ExtensionSystem::SettingsPtr s);
    QList<QMenu*> menuActions();

    KumFile::Data toKumFile() const;
    void setKumFile(const KumFile::Data & data);
    void setPlainText(const QString & data);
    void setDocumentId(int id);

    const class TextCursor * cursor() const;
    const class TextDocument * document() const;
    const Shared::AnalizerInterface * analizer() const;
    Shared::AnalizerInterface * analizer();
    class TextCursor * cursor();
    class TextDocument * document();
    bool isModified() const;
    void appendMarginText(int lineNo, const QString & text);
    void setMarginText(int lineNo, const QString & text, const QColor &fgColor);
    void clearMarginText();
    void clearMarginText(uint fromLine, uint toLine);
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
public slots:
    void undo();
    void redo();
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
    void focusInEvent(QFocusEvent *e);
    void loadMacros();    
    void createActions();
    void updateFromAnalizer();
    void timerEvent(QTimerEvent *e);

private /* fields */:
    EditorPlugin * plugin_;
    Shared::AnalizerInterface * analizer_;
    class TextDocument * doc_;
    class TextCursor * cursor_;
    class EditorPlane * plane_;

    static class Clipboard * clipboard_;
    QScrollBar * horizontalScrollBar_;
    QScrollBar * verticalScrollBar_;
    ExtensionSystem::SettingsPtr settings_;

    class FindReplace * findReplace_;

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

    bool teacherMode_;

    DocBookViewer::DocBookView * helpViewer_;

    int timerId_;
    int autoScrollTimerId_;
    char autoScrollStateY_;
    char autoScrollStateX_;

    bool notSaved_;
};

QDataStream & operator<< (QDataStream & stream, const Editor & editor);
QDataStream & operator>> (QDataStream & stream, Editor & editor);

} // namespace Editor

#endif // EDITOR_H
