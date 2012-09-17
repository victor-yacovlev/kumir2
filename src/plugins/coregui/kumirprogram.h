#ifndef COREGUI_KUMIRPROGRAM_H
#define COREGUI_KUMIRPROGRAM_H

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

class KumirProgram : public QObject
{
    Q_OBJECT
public:
    explicit KumirProgram(QObject *parent = 0);
    void setNativeGenerator(GeneratorInterface * cpp);
    inline void setBytecodeGenerator(GeneratorInterface * bc) { plugin_bytcodeGenerator = bc; }
    inline void setEditorPlugin(EditorInterface * ed) { plugin_editor = ed; }
    void setAST(const AST::Data * ast);
    inline QActionGroup * actions() { return gr_actions; }
    inline bool isRunning() const { return e_state!=Idle; }
    inline void setSourceFileName(const QString & s) { s_sourceFileName = s; }
    inline void setDocumentId(int id) { if (e_state==Idle) i_documentId = id; }
    inline int documentId() const { return i_documentId; }
    inline void setMainWidget(QWidget * w) { w_mainWidget = w; }
    void setTerminal(Term * t, QDockWidget * w);
    void setBytecodeRun(KPlugin * run);
    void addActor(KPlugin * a, QWidget * w);
    inline QString endStatus() const { return s_endStatus; }
    ~KumirProgram();
    class KumirVariablesWebObject * variablesWebObject();
signals:
    void giveMeAProgram();
    void activateDocumentTab(int documentId);

public slots:
    void fastRun();
    void blindRun();
    void regularRun();
    void testingRun();
    void stepRun();
    void stepIn();
    void stepOut();
    void stop();
    void switchGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState cur);
    void handleActorCommandFinished();
    void handleActorResetRequest(const QString & actorName);
    void handleLineChanged(int lineNo);
    void handleMarginTextRequest(int lineNo, const QString & text);
    void handleMarginClearRequest(int fromLine, int toLine);
private slots:
    void handleProcessFinished(int exitCode, QProcess::ExitStatus status);
    void handleProcessError(QProcess::ProcessError);
    void handleRunnerStopped(int);
    void handleInputDone(const QVariantList & data);
private:
    void timerEvent(QTimerEvent *e);
    void prepareKumirRunner();
    enum State { Idle, FastRun, RegularRun, StepRun } e_state;
    const AST::Data * m_ast;
    QString s_endStatus;
    QProcess * m_process;
    Term * m_terminal;
    QDockWidget * m_terminalWindow;
    GeneratorInterface * plugin_nativeGenerator;
    GeneratorInterface * plugin_bytcodeGenerator;
    RunInterface * plugin_bytecodeRun;
    EditorInterface * plugin_editor;
    QAction * a_fastRun;
    QAction * a_blindRun;
    QAction * a_regularRun;
    QAction * a_testingRun;
    QAction * a_stepRun;
    QAction * a_stepIn;
    QAction * a_stepOut;
    QAction * a_stop;
    QActionGroup * gr_actions;
    QString s_sourceFileName;
    QMap<QString,QWidget*> m_actorWindows;
    QMap<QString,ActorInterface*> m_actors;
    int i_documentId;
    bool b_blind;
    class KumirVariablesWebObject * m_variablesWebObject;
    int i_timerId;
    QWidget * w_mainWidget;
    bool b_processUserTerminated;
};

} // namespace CoreGui

#endif // COREGUI_KUMIRPROGRAM_H
