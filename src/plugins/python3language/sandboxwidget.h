#ifndef PYTHON3LANGUAGE_SANDBOXWIDGET_H
#define PYTHON3LANGUAGE_SANDBOXWIDGET_H

#include "sandboxwidget_frame.h"

#include <QtGlobal>
#include <QWidget>
#include <QTextEdit>
#include <QString>
#include <QImage>
#include <QMutex>

extern "C" {
#include <Python.h>
}

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
    void addTextOutputItem(const QString & outputText, const Sandbox::FrameType frameType);
    void addGraphicsOutputItem(const QImage & image);
    void addHtmlItem(const QString & html);

    void processCurrentInputFrame();

private:

    explicit SandboxWidget(const QString &pythonPath, QWidget *parent);

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
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_SANDBOXWIDGET_H
