#ifndef EDITOR_P_H
#define EDITOR_P_H

#include "extensionsystem/settings.h"

#include <QtCore>
#include <QtGui>
#include "interfaces/editorinterface.h"
#include "interfaces/analizerinterface.h"
#include "macro.h"
#include "findreplace.h"

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
    ExtensionSystem::SettingsPtr settings;

    FindReplace * findReplace;

    QLabel * keybStatus;
    QLabel * positionStatus;

    QAction * copy;
    QAction * paste;
    QAction * cut;
    QAction * selectAll;
    QAction * deleteLine;
    QAction * deleteTail;
    QAction * toggleComment;

    QAction * find;
    QAction * replace;

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
    char autoScrollStateY;
    char autoScrollStateX;

    bool notSaved;

    void loadMacros();
    void updateInsertMenu();
    void createActions();
    void updateFromAnalizer();
    void timerEvent(QTimerEvent *e);
public slots:
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
};

}

#endif // EDITOR_P_H
