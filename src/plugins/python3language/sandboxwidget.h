#ifndef PYTHON3LANGUAGE_SANDBOXWIDGET_H
#define PYTHON3LANGUAGE_SANDBOXWIDGET_H

extern "C" {
#include <Python.h>
}

#include "sandboxwidget_frame.h"
#include "pyinterpreterprocess.h"

#include <QtGlobal>
#include <QWidget>
#include <QTextEdit>
#include <QString>
#include <QImage>
#include <QMutex>


#include "extensionsystem/settings.h"

namespace Python3Language {

class SandboxWidget : public QWidget
{
    Q_OBJECT
public:    
    static SandboxWidget* instance(const QString &pythonPath, QWidget *parent = 0);
    static PyObject* __init__();

Q_SIGNALS:

public Q_SLOTS:
    void createWelcomeFrame();
    void reset();
    void addDefaultInputItem();
    void addInputItem(const QString & promptText);
    void addPyInputItem(const QString & promptText);
    void addTextOutputItem(const QString & outputText, const Sandbox::FrameType frameType);
    void addGraphicsOutputItem(const QImage & image);
    void addHtmlItem(const QString & html);

    void processCurrentInputFrame();

private Q_SLOTS:
    void handleEvalFinished(bool moreLinesRequired);
    void handleStdOut(const QString &message);
    void handleStdErr(const QString &message);
    void handleProcessRespawned(int exitCode, QProcess::ExitStatus exitStatus);

private:


    explicit SandboxWidget(const QString &pythonPath, QWidget *parent);
    void createInterpreterProcess();

    static SandboxWidget * self;

    static PyObject* write_output(PyObject *, PyObject * args);
    static PyObject* write_error(PyObject *, PyObject * args);
    static PyObject* read_input(PyObject *, PyObject * args);
    static PyObject* _reset(PyObject *, PyObject *);

    bool eventFilter(QObject *obj, QEvent *evt);
    bool filterKeyPressEvent(QKeyEvent *event);
    void performEditOperation(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    ExtensionSystem::SettingsPtr editorSettings();
    bool darkScheme() const;
    quint32 commandNumber_;
    QTextEdit *editor_;
    PyThreadState *py_;
    PyObject *mainModule_;

    QString lastCommandOutput_;
    QString lastCommandError_;
    QString pythonPath_;

    bool resetFlag_;
    QMutex resetMutex_;

    PyInterpreterProcess* _pyInterpreterProcess;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_SANDBOXWIDGET_H
