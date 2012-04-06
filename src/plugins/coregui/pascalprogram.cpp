#include "pascalprogram.h"

namespace CoreGUI {

PascalProgram::PascalProgram(QObject *parent) :
    QObject(parent)
{
    i_documentId = 0;
    w_mainWidget = 0;
    m_terminal = 0;
    m_terminalWindow = 0;
    plugin_editor = 0;
    m_process = new QProcess(this);
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(handleProcessFinished(int,QProcess::ExitStatus)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(handleProcessError(QProcess::ProcessError)));



    a_stop = new QAction(tr("Stop"), this);
    a_stop->setIcon(QIcon::fromTheme("media-playback-stop", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/media-playback-stop.png")));
    connect(a_stop, SIGNAL(triggered()), this, SLOT(stop()));
#ifndef Q_OS_MAC
    a_stop->setShortcut(QKeySequence("Esc"));
#else
    a_stop->setShortcut(QKeySequence("Esc"));
#endif
    a_stop->setToolTip(a_stop->text()+" <b>"+a_stop->shortcut().toString()+"</b>");


    QAction * s1 = new QAction(this);
    s1->setSeparator(true);

    a_blindRun = new QAction(tr("Blind run"), this);
    a_blindRun->setIcon(QIcon::fromTheme("media-seek-forward", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/media-seek-forward.png")));
    connect(a_blindRun, SIGNAL(triggered()), this, SLOT(blindRun()));
#ifndef Q_OS_MAC
    a_blindRun->setShortcut(QKeySequence("Shift+F9"));

#else
    a_blindRun->setShortcut(QKeySequence("Ctrl+R"));
#endif
    a_blindRun->setToolTip(a_blindRun->text()+" <b>"+a_blindRun->shortcut().toString()+"</b>");


    gr_actions = new QActionGroup(this);
    gr_actions->addAction(a_blindRun);
    gr_actions->addAction(s1);
    gr_actions->addAction(a_stop);


}

void PascalProgram::setTerminal(Term *t, QDockWidget * w)
{
    m_terminal = t;
    m_terminalWindow = w;

    connect(m_terminal, SIGNAL(inputFinished(QVariantList)),
            this, SLOT(handleInputDone(QVariantList)));
    connect(m_terminal, SIGNAL(showWindowRequest()),
            m_terminalWindow, SLOT(show()));
}

void PascalProgram::blindRun()
{

}

void PascalProgram::stop()
{
    if (m_process) {
        m_process->kill();
    }
}

void PascalProgram::switchGlobalState(GlobalState , GlobalState cur)
{
    if (cur==GS_Unlocked || cur==GS_Observe) {
        a_blindRun->setEnabled(true);
        a_stop->setEnabled(false);

    }
    if (cur==GS_Running || cur==GS_Input) {
        a_blindRun->setEnabled(false);
        a_stop->setEnabled(true);
    }
    if (cur==GS_Pause) {
        a_blindRun->setEnabled(true);
        a_stop->setEnabled(true);
    }
}

} // namespace CoreGUI
