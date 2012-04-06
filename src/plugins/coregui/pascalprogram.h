#ifndef COREGUI_PASCALPROGRAM_H
#define COREGUI_PASCALPROGRAM_H

#include <QtCore>
#include <QtGui>
#include "interfaces/generatorinterface.h"
#include "interfaces/actorinterface.h"
#include "interfaces/runinterface.h"
#include "interfaces/editorinterface.h"
#include "terminal.h"
#include "extensionsystem/kplugin.h"

namespace CoreGUI {

using Shared::GeneratorInterface;
using Shared::ActorInterface;
using Shared::RunInterface;
using Shared::EditorInterface;
using Terminal::Term;
using namespace ExtensionSystem;


class PascalProgram : public QObject
{
    Q_OBJECT
public:
    explicit PascalProgram(QObject *parent = 0);
    inline void setEditorPlugin(EditorInterface * ed) { plugin_editor = ed; }
    void setTerminal(Term * t, QDockWidget * w);
    inline QActionGroup * actions() { return gr_actions; }
    inline bool isRunning() const { return e_state!=Idle; }
    inline void setSourceFileName(const QString & s) { s_sourceFileName = s; }
    inline void setDocumentId(int id) { if (e_state==Idle) i_documentId = id; }
    inline int documentId() const { return i_documentId; }
    inline void setMainWidget(QWidget * w) { w_mainWidget = w; }

public slots:
    void blindRun();
    void stop();
    void switchGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState cur);

private:

    enum State { Idle, FastRun, RegularRun, StepRun } e_state;
    QAction * a_blindRun;
    QAction * a_stop;
    EditorInterface * plugin_editor;
    QWidget * w_mainWidget;
    QString s_endStatus;
    QProcess * m_process;
    Term * m_terminal;
    QDockWidget * m_terminalWindow;
    QActionGroup * gr_actions;
    QString s_sourceFileName;
    int i_documentId;
};

} // namespace CoreGUI

#endif // COREGUI_PASCALPROGRAM_H
