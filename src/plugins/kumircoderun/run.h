#ifndef KUMIRCODERUN_RUN_H
#define KUMIRCODERUN_RUN_H

#include <QtCore>
#define DO_NOT_DECLARE_STATIC
#include "vm/vm.hpp"
#include "interfaces/actorinterface.h"
#include "kumvariablesmodel.h"
#include "guirun.h"
#include <memory>

namespace KumirCodeRun {

using Kumir::String;
using Kumir::real;
using VM::Variable;
using VM::AnyValue;

typedef QPair<quint8,quint32> BreakpointLocation;

struct BreakpointData {
    bool enabled;
    quint32 ignoreCount;
    quint32 hitCount;
};

class Mutex: public VM::CriticalSectionLocker
{
public:
    inline Mutex() { m = new QMutex; }
    inline void lock() {
        m->lock();
    }
    inline void unlock() {
        m->unlock();
    }
    inline void reset() {
        m->tryLock();
        m->unlock();
    }

    inline ~Mutex() { delete m; }
private:
    QMutex * m;
};

class Run
        : public QThread
        , public VM::DebuggingInteractionHandler
{
    Q_OBJECT
public:
    enum RunMode { RM_StepOver, RM_ToEnd, RM_StepOut, RM_StepIn };
    explicit Run(QObject *parent);
    std::shared_ptr<VM::KumirVM> vm;
    bool programLoaded;
    inline bool stopped() const { return stoppingFlag_; }
    bool mustStop() const;
    bool isTestingRun() const;
    inline bool supportBreakpoints() const { return supportBreakpoints_; }
    inline void setSupportBreakpoints(bool v) { supportBreakpoints_ = v; }

    // VM Access methods
    int effectiveLineNo() const;
    bool loadProgramFromBinaryBuffer(std::list<char> & stream, const String & filename);
    inline void setProgramDirectory(const QString & dirName) { vm->setProgramDirectory(dirName.toStdWString()); }
    QString error() const;
    bool hasTestingAlgorithm() const;

    void setEntryPointToMain();
    void setEntryPointToTest();
    bool hasMoreInstructions() const;
    void reset();
    void evaluateNextInstruction();
    bool canStepOut() const;
    QVariant valueStackTopItem() const;
    inline QAbstractItemModel* variablesModel() const {
        return variablesModel_;
    }


public slots:
    void lockVMMutex();
    void unlockVMMutex();
    void stop();
    void runStepOver();
    void runStepIn();
    void runToEnd();
    void runBlind();
    void runContinuous();

    bool noticeOnLineChanged(int lineNo, uint32_t colStart, uint32_t colEnd);
    bool noticeOnFunctionReturn();
    bool noticeOnStepsChanged(quint64 stepsDone);

    bool setTextToMargin(int lineNo, const String & s, bool red);
    bool appendTextToMargin(int lineNo, const String & s);
    bool clearMargin(int from, int to);

    void debuggerReset();
    void debuggerNoticeBeforePopContext();
    void debuggerNoticeAfterPopContext();
    void debuggerNoticeBeforePushContext();
    void debuggerNoticeAfterPushContext();
    void debuggerNoticeBeforeArrayInitialize(const VM::Variable &, const int[7]);
    void debuggerNoticeAfterArrayInitialize(const VM::Variable &);
    void debuggerNoticeOnValueChanged(const VM::Variable &, const int *);
    void debuggerNoticeOnBreakpointHit(const Kumir::String & filename, const quint32 lineNo);


    void handleAlgorhitmDone(int lineNo, quint32 colStart, quint32 colEnd);
    void handlePauseRequest();

    void removeAllBreakpoints();
    void insertOrChangeBreakpoint(bool enabled, const QString &fileName, quint32 lineNo, quint32 ignoreCount, const QString &condition);
    void insertSingleHitBreakpoint(const QString &fileName, quint32 lineNo);
    void removeBreakpoint(const QString &fileName, quint32 lineNo);


signals:
    void breakpointHit(const QString & fileName, int lineNo);
    void updateStepsCounter(quint64);
    void finishInput(const QVariantList &data);
    void lineChanged(int lineNo, quint32 colStart, quint32 colEnd);
    void output(const QString &value);
    void error(const QString & message);
    void input(const QString & format);
    void marginText(int lineNo, const QString & text);
    void marginTextReplace(int lineNo, const QString & text, bool redFgColor);
    void externalFunctionCall(const QString & pluginName,
                              quint16 algId,
                              const QVariantList & arguments);
    void resetModule(const QString & pluginName);
    void aboutToStop();
    void clearMarginRequest(int,int);
    void userTerminated(); // acts like QThread::finished()

protected :
    void run();

    RunMode runMode_;

    bool stoppingFlag_;
    QMutex* stoppingMutex_;

    bool stepDoneFlag_;
    QMutex* stepDoneMutex_;

    bool algDoneFlag_;
    QMutex* algDoneMutex_;

    bool breakHitFlag_;
    bool ignoreLineChangeFlag_;
    QMutex* breakHitMutex_;

    int originFunctionDeep_;

    QMutex* interactDoneMutex_;
    bool interactDoneFlag_;

    QVariantList inputResult_;
    QVariantList funcOptResults_;
    QVariant funcResult_;
    QString funcError_;
    mutable class std::shared_ptr<Mutex> VMMutex_;
    KumVariablesModel * variablesModel_;
    QString programLoadError_;

    Gui::SimulatedInputBuffer * stdInBuffer_;

    bool supportBreakpoints_;
    QMap<BreakpointLocation,BreakpointData> breakpoints_;

};





} // namespace KumirCodeRun

#endif // KUMIRCODERUN_RUN_H
