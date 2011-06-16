#ifndef COREGUI_KUMIRPROGRAM_H
#define COREGUI_KUMIRPROGRAM_H

#include <QtCore>
#include <QtGui>
#include "interfaces/generatorinterface.h"
#include "stdlib/connector.h"
#include "terminal.h"

namespace CoreGUI {

using Shared::GeneratorInterface;
using Terminal::Terminal;
using StdLib::Connector;

class KumirProgram : public QObject
{
    Q_OBJECT
public:
    explicit KumirProgram(QObject *parent = 0);
    inline void setCppGenerator(GeneratorInterface * cpp) { plugin_cppGenerator = cpp; }
    inline void setBytecodeGenerator(GeneratorInterface * bc) { plugin_bytcodeGenerator = bc; }
    inline void setAST(const AST::Data * ast) { m_ast = ast; }
    inline QActionGroup * actions() { return gr_actions; }
    inline bool isRunning() const { return e_state!=Idle; }
    inline void setSourceFileName(const QString & s) { s_sourceFileName = s; }
    void setTerminal(Terminal * t);
signals:

public slots:
    void fastRun();
    void regularRun();
    void stepRun();
    void stepIn();
    void stepOut();
    void stop();
private slots:
    void handleProcessFinished(int exitCode, QProcess::ExitStatus);
    void handleProcessError(QProcess::ProcessError);
private:
    enum State { Idle, FastRun, RegularRun, StepRun } e_state;
    const AST::Data * m_ast;
    QProcess * m_process;
    Terminal * m_terminal;
    GeneratorInterface * plugin_cppGenerator;
    GeneratorInterface * plugin_bytcodeGenerator;
    QAction * a_fastRun;
    QAction * a_regularRun;
    QAction * a_stepRun;
    QAction * a_stepIn;
    QAction * a_stepOut;
    QAction * a_stop;
    QActionGroup * gr_actions;
    QString s_sourceFileName;
    Connector * m_connector;

};

} // namespace CoreGui

#endif // COREGUI_KUMIRPROGRAM_H
