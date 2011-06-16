#include "kumirprogram.h"

namespace CoreGUI {

using Shared::GeneratorResult;
using Shared::GeneratorType;

KumirProgram::KumirProgram(QObject *parent)
    : QObject(parent)
    , e_state(Idle)
    , m_ast(0)
    , m_process(0)
    , m_terminal(0)
    , plugin_cppGenerator(0)
    , plugin_bytcodeGenerator(0)
    , a_fastRun(0)
    , a_regularRun(0)
    , a_stepRun(0)
    , a_stepIn(0)
    , a_stepOut(0)
    , a_stop(0)
    , gr_actions(0)
    , m_connector(0)
{
    m_process = new QProcess(this);
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(handleProcessFinished(int,QProcess::ExitStatus)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(handleProcessError(QProcess::ProcessError)));


    m_connector = new Connector();
    const QString key = QString("kumir-%1").arg(QCoreApplication::applicationPid());
    m_connector->listenFor(key);


    a_fastRun = new QAction(tr("Fast run"), this);
    a_fastRun->setIcon(QIcon::fromTheme("media-seek-forward"));
    connect(a_fastRun, SIGNAL(triggered()), this, SLOT(fastRun()));

    a_regularRun = new QAction(tr("Regular run"), this);
    a_regularRun->setIcon(QIcon::fromTheme("media-playback-start"));
    connect(a_regularRun, SIGNAL(triggered()), this, SLOT(regularRun()));

    a_stepRun = new QAction(tr("Step run"), this);
    a_stepRun->setIcon(QIcon::fromTheme("media-skip-forward"));
    connect(a_stepRun, SIGNAL(triggered()), this, SLOT(stepRun()));

    a_stepIn = new QAction(tr("Step in"), this);
    a_stepIn->setIcon(QIcon::fromTheme("debug-step-into"));
    connect(a_stepIn, SIGNAL(triggered()), this, SLOT(stepIn()));

    a_stepOut = new QAction(tr("Step out"), this);
    a_stepOut->setIcon(QIcon::fromTheme("debug-step-out"));
    connect(a_stepOut, SIGNAL(triggered()), this, SLOT(stepOut()));

    a_stop = new QAction(tr("Stop"), this);
    a_stop->setIcon(QIcon::fromTheme("media-playback-stop"));
    connect(a_stop, SIGNAL(triggered()), this, SLOT(stop()));

    a_stepIn->setEnabled(false);
    a_stepOut->setEnabled(false);
    a_stop->setEnabled(false);

    QAction * s1 = new QAction(this);
    s1->setSeparator(true);

    QAction * s2 = new QAction(this);
    s2->setSeparator(true);

    gr_actions = new QActionGroup(this);
    gr_actions->addAction(a_fastRun);
    gr_actions->addAction(a_regularRun);
    gr_actions->addAction(s1);
    gr_actions->addAction(a_stepRun);
    gr_actions->addAction(a_stepIn);
    gr_actions->addAction(a_stepOut);
    gr_actions->addAction(s2);
    gr_actions->addAction(a_stop);

}

void KumirProgram::setTerminal(Terminal *t)
{
    m_terminal = t;
    connect(m_connector, SIGNAL(inputFormatReceived(QString)),
            m_terminal, SLOT(input(QString)));
    connect(m_connector, SIGNAL(outputTextReceived(QString)),
            m_terminal, SLOT(output(QString)));
    connect(m_connector, SIGNAL(errorMessageReceived(QString)),
            m_terminal, SLOT(error(QString)));
}

void KumirProgram::fastRun()
{
    if (e_state!=Idle) {
        return;
    }
    QString suffix;
#ifdef Q_OS_WIN32
    suffix = ".exe";
#else
    suffix = ".bin";
#endif
    const QString exeFileName = s_sourceFileName+suffix;
    bool mustRegenerate = true;
    if (QFile::exists(exeFileName)) {
        QFileInfo fi(exeFileName);
        QDateTime astTime = m_ast->lastModified;
        QDateTime fileTime = fi.lastModified();
        mustRegenerate = !astTime.isValid() || fileTime < astTime;
    }
    bool ok = !mustRegenerate;
    if (mustRegenerate) {

        QFile exeFile(exeFileName);
        if (exeFile.exists()) {
            if (!exeFile.remove()) {
                qDebug() << "Can't remove existing file " << exeFileName;
            }
        }
        if (exeFile.open(QIODevice::WriteOnly)) {
            GeneratorResult res = plugin_cppGenerator->generateExecuable(m_ast, &exeFile);
            if (res.type==Shared::GenError) {
                qDebug() << "Error generating execuable";
            }
            else {
                ok = true;
            }
            exeFile.close();
            QFile::Permissions perms = exeFile.permissions();
            perms |= QFile::ExeOwner;
            perms |= QFile::ExeUser;
            perms |= QFile::ExeGroup;
            perms |= QFile::ExeOther;
            exeFile.setPermissions(perms);
        }
        else {
            qDebug() << "Can't open file for writing: " << exeFileName;
        }
    }
    if (ok) {
        const QString cmd = QString("%1 --key=kumir-%2")
                .arg(exeFileName)
                .arg(QCoreApplication::applicationPid());
        m_terminal->start(exeFileName);
        m_process->start(cmd);
        m_process->waitForStarted();
        e_state = FastRun;
    }
}

void KumirProgram::handleProcessError(QProcess::ProcessError error)
{
    qDebug() << "Process error " << error;
    e_state = Idle;
    m_terminal->error(tr("Unknown error"));
}

void KumirProgram::handleProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    qDebug() << "Process finished with code " << exitCode << " and status " << status;
    e_state = Idle;
    m_terminal->finish();
}

void KumirProgram::regularRun()
{

}

void KumirProgram::stepRun()
{

}

void KumirProgram::stepIn()
{

}

void KumirProgram::stepOut()
{

}

void KumirProgram::stop()
{

}

} // namespace CoreGui
