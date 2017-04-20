#ifndef PYTHON3LANGUAGE_SANDBOXWIDGET_H
#define PYTHON3LANGUAGE_SANDBOXWIDGET_H


#include "sandboxwidget_frame.h"
#include "pyinterpreterprocess.h"

#include <QtGlobal>
#include <QWidget>
#include <QTextEdit>
#include <QString>
#include <QImage>
#include <QMutex>


#include <kumir2-libs/extensionsystem/settings.h>

namespace Python3Language {

class SandboxWidget : public QWidget
{
    Q_OBJECT
public:    
    explicit SandboxWidget(const QString &pythonPath, QWidget *parent);

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
    void createInterpreterProcess();

    bool eventFilter(QObject *obj, QEvent *evt);
    bool filterKeyPressEvent(QKeyEvent *event);
    void performEditOperation(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    ExtensionSystem::SettingsPtr editorSettings();
    bool darkScheme() const;

    QTextEdit *_editor;
    QString _pythonPath;

    PyInterpreterProcess* _pyInterpreterProcess;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_SANDBOXWIDGET_H
