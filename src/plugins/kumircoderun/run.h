#ifndef KUMIRCODERUN_RUN_H
#define KUMIRCODERUN_RUN_H

#include <QtCore>
#include "vm.h"
#include "dataformats/bytecode.h"
#include "variant.h"

namespace KumirCodeRun {

class Run : public QThread
{
    Q_OBJECT
public:
    enum RunMode { RM_StepOver, RM_ToEnd, RM_StepOut, RM_StepIn };
    explicit Run(QObject *parent = 0);
    VM * vm;
    bool programLoaded;
    inline bool stopped() const { return b_stopping; }

public slots:
    void stop();
    void runStepOver();
    void runStepIn();
    void runStepOut();
    void runBlind();
    void runContinuous();


    void handleInputRequest(const QString & format,
                            const QList<quintptr> & references,
                            const QList<int> & indeces
                            );
    void handleInputArgumentRequest(int localId,
                                    const QString & varName,
                                    const QString & baseFormat,
                                    const QList<int> & bounds);
    void handleOutputArgumentRequest(const QVariant & value,
                                    const QString & varName,
                                    const QList<int> & bounds);
    void handleOutputRequest(const QString & output );

    void handleLineChanged(int lineNo);
    void finishInput(const QVariantList & data);
    void finishExternalFunctionCall(
        const QString & error,
        const QVariant & retval,
                                    const QVariantList & results);
    void handleExternalRequest(const QString & pluginName,
                               const QString & functionName,
                               const QVariantList & arguments,
                               const QList<quintptr> & references,
                               const QList<int> & indeces);
    void handlePauseRequest();

signals:
    void lineChanged(int lineNo);
    void output(const QString & text);
    void error(const QString & message);
    void input(const QString & format);
    void externalFunctionCall(const QString & pluginName,
                              const QString & functionName,
                              const QVariantList & arguments);
    void resetModule(const QString & pluginName);
    void aboutToStop();
protected :
    void run();

    bool mustStop();

    RunMode e_runMode;

    bool b_stopping;
    QMutex * mutex_stopping;

    bool b_stepDone;
    QMutex * mutex_stepDone;

    int i_originFunctionDeep;

    QMutex * mutex_interactDone;
    bool b_interactDone;

    QVariantList list_inputResult;
    QVariantList list_funcResults;
    QVariant v_funcResult;
    QString s_funcError;


};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_RUN_H
