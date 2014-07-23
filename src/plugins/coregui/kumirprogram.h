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
    enum RunEndStatus {
        Running,
        Finished,
        Terminated,
        Exception
    };

    explicit KumirProgram(QObject *parent = 0);

    inline Shared::Editor::InstanceInterface * editorInstance() const {
        return editor_;
    }
    inline void setEditorInstance(Shared::Editor::InstanceInterface * editor) {
        editor_ = editor;
    }

    inline QActionGroup * actions() { return actions_; }
    inline bool isRunning() const { return state_!=Idle; }
    inline void setMainWidget(QWidget * w) { mainWidget_ = w; }
    void setTerminal(Term * t, QDockWidget * w);
    static Shared::GeneratorInterface * kumirCodeGenerator();
    static Shared::GeneratorInterface * kumirNativeGenerator();

    inline QString endStatusText() const { return endStatusText_; }
    inline RunEndStatus endStatus() const { return endStatus_; }
    inline void setCourseManagerRequest() { courseManagerRequest_ = true; }
signals:
    void giveMeAProgram();

public slots:
    void blindRun();
    void regularRun();
    void testingRun();
    void stepRun();
    void stepIn();
    void stepOut();
    void stop();
    void switchGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState cur);
    void handleLineChanged(int lineNo, quint32 colStart, quint32 colEnd);
    void handleMarginTextRequest(int lineNo, const QString & text);
    void handleMarginClearRequest(int fromLine, int toLine);
    void handleMarginTextReplace(int,const QString&,bool);
private slots:
    void handleRunnerStopped(int);

private:
    void createActions();
    void createConnections();

    static Shared::RunInterface * runner();

    void setAllActorsAnimationFlag(bool animationEnabled);
    void prepareKumirRunner(Shared::GeneratorInterface::DebugLevel);

private /*fields*/:
    enum State { Idle, RegularRun, BlindRun, StepRun, TestingRun } state_;
    RunEndStatus endStatus_;
    QString endStatusText_;
    Term * terminal_;
    Shared::Editor::InstanceInterface * editor_;
    QWidget * mainWidget_;

    QAction * blindRunAction_;
    QAction * regularRunAction_;
    QAction * testingRunAction_;
    QAction * stepRunAction_;
    QAction * stepInAction_;
    QAction * stepOutAction_;
    QAction * stopAction_;
    QActionGroup * actions_;

    bool courseManagerRequest_;

    bool b_processUserTerminated;
};

} // namespace CoreGui

#endif // COREGUI_KUMIRPROGRAM_H
