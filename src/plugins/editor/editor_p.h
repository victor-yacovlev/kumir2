#ifndef EDITOR_P_H
#define EDITOR_P_H

#include <QtCore>
#include <QtGui>
#include "interfaces/editorinterface.h"
#include "interfaces/analizerinterface.h"
#include "macro.h"

namespace Editor {

class EditorPrivate
        : public QObject
{
    Q_OBJECT
public:
    class Editor * q;
    Shared::AnalizerInterface * analizer;
    class TextDocument * doc;
    class TextCursor * cursor;
    class EditorPlane * plane;

    static class Clipboard * clipboard;
    QScrollBar * horizontalScrollBar;
    QScrollBar * verticalScrollBar;
    QSettings * settings;

    QLabel * keybStatus;
    QLabel * positionStatus;

    QAction * copy;
    QAction * paste;
    QAction * cut;
    QAction * selectAll;
    QAction * deleteLine;
    QAction * deleteTail;
    QAction * toggleComment;

    QAction * undo;
    QAction * redo;

    QMenu * menu_edit;
    QMenu * menu_insert;

    QAction * separator;

    QList<Macro> systemMacros;
    QList<Macro> userMacros;

    bool teacherMode;

    int timerId;
    int autoScrollTimerId;
    char autoScrollState;

    bool notSaved;

    void loadMacros();
    void updateInsertMenu();
    void createActions();
    void updateFromAnalizer();
    static QList<Shared::ChangeTextTransaction> mergeTransactions(QList<Shared::ChangeTextTransaction>);
    static bool mergeTransaction(Shared::ChangeTextTransaction & one, const Shared::ChangeTextTransaction & other);
    void timerEvent(QTimerEvent *e);
public slots:
    void updatePosition(int row, int col);
    void handleCompleteCompilationRequiest(
        const QStringList & visibleText,
        const QStringList & hiddenText,
        int hiddenBaseLine
        );
    void handleLineAndTextChanged(const QStack<Shared::ChangeTextTransaction> & changes);
    void playMacro();
    void handleAutoScrollChange(char a);
};

}

#endif // EDITOR_P_H
