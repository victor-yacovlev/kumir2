#ifndef RUN_INTERFACE
#define RUN_INTERFACE

#include <QtCore>
#include <QtGui>

namespace Shared {

class RunInterface {
public:
    enum StopReason { SR_Done, SR_UserInteraction, SR_InputRequest, SR_Error, SR_UserTerminated };
    virtual bool loadProgram(const QString &fileName, const QByteArray & source) = 0;
    virtual QDateTime loadedProgramVersion() const = 0;

    virtual bool canStepOut() const = 0;
    virtual void runBlind() = 0;
    virtual void runContinuous() = 0;
    virtual void runStepOver() = 0;
    virtual void runStepInto() = 0;
    virtual void runStepOut() = 0;
    virtual void runTesting() = 0;
    virtual bool isTestingRun() const = 0;
    virtual void terminate() = 0;
    virtual bool hasMoreInstructions() const = 0;
    virtual bool hasTestingEntryPoint() const = 0;
    virtual int currentLineNo() const = 0;
    virtual QPair<quint32,quint32> currentColumn() const = 0;
    virtual QString error() const = 0;
    virtual QVariant valueStackTopItem() const = 0;
    virtual unsigned long int stepsCounted() const = 0;
    virtual QAbstractItemModel * debuggerVariablesViewModel() const = 0;

    virtual QMap<QString,QVariant> getScalarLocalValues(int frameNo) const = 0;
    virtual QMap<QString,QVariant> getScalarGlobalValues(const QString & moduleName) const = 0;
    virtual QVariantList getLocalTableValues(int frameNo, int maxCount, const QString & name,
                                             const QList< QPair<int,int> > & ranges, bool & complete) const = 0;
    virtual QVariantList getGlobalTableValues(const QString & moduleName, int maxCount, const QString & name,
                                             const QList< QPair<int,int> > & ranges, bool & complete) const = 0;
    virtual QVariant getLocalTableValue(int frameNo, const QString & name, const QList<int> & indeces) const = 0;
    virtual QVariant getGlobalTableValue(const QString & moduleName, const QString & name, const QList<int> & indeces) const = 0;
};

}
Q_DECLARE_METATYPE(Shared::RunInterface::StopReason)
Q_DECLARE_INTERFACE(Shared::RunInterface, "kumir2.run")

#endif
