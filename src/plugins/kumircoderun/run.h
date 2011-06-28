#ifndef KUMIRCODERUN_RUN_H
#define KUMIRCODERUN_RUN_H

#include <QtCore>
#include "vm.h"
#include "bytecode/data.h"
#include "variant.h"

namespace KumirCodeRun {

class Run : public QThread
{
    Q_OBJECT
public:
    enum RunMode { RM_StepOver, RM_ToEnd, RM_StepOut, RM_StepIn };
    explicit Run(QObject *parent = 0);
    VM * vm;
    inline bool stopped() const { return b_stopping; }

public slots:
    void stop();
    void runStepOver();
    void runStepIn();
    void runStepOut();
    void runContinuous();


    void handleInputRequest(const QString & format, const QList<quintptr> & references);
    void handleOutputRequest(const QString & output );
    void handleDeepChanged();
    void handleLineChanged(int lineNo);
    void finishInput(const QVariantList & data);

signals:
    void lineChanged(int lineNo);
    void output(const QString & text);
    void error(const QString & message);
    void input(const QString & format);

protected :
    void run();

    bool mustStop();

    RunMode e_runMode;

    bool b_stopping;
    QMutex * mutex_stopping;

    bool b_stepDone;
    QMutex * mutex_stepDone;

    int i_functionDeep;
    QMutex * mutex_functionDeep;

    int i_originFunctionDeep;

    QMutex * mutex_inputDone;
    bool b_inputDone;

    QVariantList list_inputResult;


};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_RUN_H
