#ifndef KUMIRCODERUN_PLUGIN_H
#define KUMIRCODERUN_PLUGIN_H

#include <QtCore>
#include "extensionsystem/kplugin.h"
#include "interfaces/runinterface.h"

namespace KumirCodeRun {

class KumirRunPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::RunInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::RunInterface)
public:
    explicit KumirRunPlugin();
    ~KumirRunPlugin();


    bool canStepOut() const;

    bool loadProgram(const QString & fileName, const QByteArray & source);
    bool hasMoreInstructions() const;
    bool hasTestingEntryPoint() const;
    QString error() const;

    int currentLineNo() const ;
    QPair<quint32,quint32> currentColumn() const;
    inline QDateTime loadedProgramVersion() const { return loadedVersion_; }
    unsigned long int stepsCounted() const;
    QAbstractItemModel * debuggerVariablesViewModel() const;

    inline bool isGuiRequired() { return false; }
    QVariant valueStackTopItem() const;

    QMap<QString,QVariant> getScalarLocalValues(int frameNo) const;
    QMap<QString,QVariant> getScalarGlobalValues(const QString & moduleName) const;
    QVariantList getLocalTableValues(
            int frameNo,
            int maxCount,
            const QString & name,
            const QList< QPair<int,int> > & ranges,
            bool & complete
            ) const;
    QVariantList getGlobalTableValues(
            const QString & moduleName,
            int maxCount,
            const QString & name,
            const QList< QPair<int,int> > & ranges,
            bool & complete
            ) const;
    QVariant getLocalTableValue(int frameNo, const QString & name, const QList<int> & indeces) const;
    QVariant getGlobalTableValue(const QString & moduleName, const QString & name, const QList<int> & indeces) const;
    bool isTestingRun() const;

public slots:
    void runBlind();
    void runContinuous();
    void runStepOver();
    void runStepInto();
    void runToEnd();
    void runTesting();
    void terminate();




protected:
    void start();
    void timerEvent(QTimerEvent *);
    QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters() const;
    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine & runtimeArguments);
    void prepareCommonRun();
    void prepareConsoleRun();
    void prepareGuiRun();
    QDateTime loadedVersion_;
    bool done_;
    bool onlyOneTryToInput_;

protected slots:
    void handleThreadFinished();
    void handleLineChanged(int lineNo, quint32 colStart, quint32 colEnd);


signals:
    void updateStepsCounter(quint64);
    void errorOutputRequest(const QString &);
    void stopped(int reason);
    void finishInput(const QVariantList & data);
    void inputRequest(const QString & format);
    void outputRequest(const QString & output);
    void lineChanged(int lineNo, quint32 colStart, quint32 colEnd);
    void marginText(int lineNo, const QString & text);
    void replaceMarginText(int lineNo, const QString & text, bool redFgColor);
    void clearMargin(int fromLine, int toLine);
    void resetModule(const QString &actorPluginName);

private:
    class Run * pRun_;
    struct CommonFunctors * common_;
    struct ConsoleFunctors * console_;
    struct GuiFunctors * gui_;

};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_PLUGIN_H
