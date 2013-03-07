#ifndef RUN_INTERFACE
#define RUN_INTERFACE

#include <QtCore>
#include <QtGui>

namespace Shared {

enum ProgramFormat {
    FormatText,
    FormatBinary
};

class RunInterface {
public:
    enum StopReason { SR_Done, SR_UserInteraction, SR_InputRequest, SR_Error, SR_UserTerminated };
    virtual bool loadProgram(const QString &fileName, const QByteArray & source, ProgramFormat format) = 0;
    virtual QDateTime loadedProgramVersion() const = 0;

    virtual bool canStepOut() const = 0;
    virtual void runBlind() = 0;
    virtual void runContinuous() = 0;
    virtual void runStepOver() = 0;
    virtual void runStepInto() = 0;
    virtual void runStepOut() = 0;
    virtual void runTesting() = 0;
    virtual void terminate() = 0;
    virtual bool hasMoreInstructions() const = 0;
    virtual int currentLineNo() const = 0;
    virtual QString error() const = 0;

    virtual QMap<QString,QVariant> getScalarLocalValues(int frameNo) const = 0;
    virtual QMap<QString,QVariant> getScalarGlobalValues(const QString & moduleName) const = 0;
    virtual QVariantList getLocalTableValues(int frameNo, int maxCount, const QString & name,
                                             const QList< QPair<int,int> > & ranges, bool & complete) const = 0;
    virtual QVariantList getGlobalTableValues(const QString & moduleName, int maxCount, const QString & name,
                                             const QList< QPair<int,int> > & ranges, bool & complete) const = 0;
    virtual QVariant getLocalTableValue(int frameNo, const QString & name, const QList<int> & indeces) const = 0;
    virtual QVariant getGlobalTableValue(const QString & moduleName, const QString & name, const QList<int> & indeces) const = 0;


/*
 * NOTE: signals should not be declared as virtuals in interface, but
 *       must be implemented in derivative classes
 *
signals:
    void stopped(StopReason reason);
    void externalRequest(const QString &actorPluginName, const QString & algName, const QVariantList & args);
    void inputRequest(const QString & format);
    void outputRequest(const QString & output);
    void lineChanged(int lineNo);
    void marginText(int lineNo, const QString & text);
    void clearMargin(int fromLine, int toLine);
    void resetModule(const QString &actorPluginName);
*/
};

}
Q_DECLARE_METATYPE(Shared::RunInterface::StopReason)
Q_DECLARE_INTERFACE(Shared::RunInterface, "kumir2.run")

#endif
