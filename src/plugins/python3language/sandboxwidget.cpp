#include "sandboxwidget.h"
#include "sandboxwidget_frame.h"
#include "pyutils.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QTextBrowser>
#include <QTextCharFormat>
#include <QFocusEvent>
#include <QTextFrame>
#include <QTextTable>
#include <QEvent>
#include <QKeyEvent>

#include <algorithm>
#include <functional>

#include "extensionsystem/pluginmanager.h"
#include "extensionsystem/kplugin.h"
#include "interfaces/editorinterface.h"

namespace Python3Language {

SandboxWidget * SandboxWidget::self = 0;

SandboxWidget::SandboxWidget(const QString & pythonPath, QWidget *parent)
    : QWidget(parent)
    , commandNumber_(0)
    , contents_(new QTextEdit(this))
    , py_(0)
    , mainModule_(0)
    , pythonPath_(pythonPath)
{
    QVBoxLayout * mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(contents_);
    contents_->installEventFilter(this);


    if (editorSettings()) {
        const QString fontName = editorSettings()->value("Font/Family", "monospace").toString();
        const int fontSize = editorSettings()->value("Font/Size", 12).toInt();
        QFont font = contents_->font();
        font.setFamily(fontName);
        font.setPointSize(fontSize);
        contents_->setFont(font);
    }
    reset();
}

void SandboxWidget::reset()
{
    if (py_) {
        PyEval_AcquireThread(py_);
        Py_EndInterpreter(py_);
        PyEval_ReleaseLock();
    }

#ifdef Q_OS_WIN32
    if (_Py_atomic_load_relaxed(&_PyThreadState_Current))
#else
    if (PyThreadState_GET())
#endif
        PyEval_AcquireLock();
    else
        PyGILState_Ensure();
    py_ = Py_NewInterpreter();
#ifdef Q_OS_WIN32
    prepareBundledSysPath();
#else
    appendToSysPath(pythonPath_);
#endif
    PyObject* py_sandbox_wrapper = PyImport_ImportModule("sandbox_wrapper");
    if (!py_sandbox_wrapper) { printPythonTraceback(); return; }
    mainModule_ = PyImport_AddModule("__main__");
    PyEval_ReleaseThread(py_);

    commandNumber_ = 0;    
    addDefaultInputItem();
}

void SandboxWidget::addDefaultInputItem()
{
    addInputItem(tr("In [%1]> "));
}

void SandboxWidget::addInputItem(const QString &promptText)
{
    contents_->setTextCursor(contents_->document()->rootFrame()->lastCursorPosition());
    QTextTableFormat inputFormat;
    inputFormat.setBorder(0.5);
    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    inputFormat.setBorderBrush(QColor(Qt::blue));
    inputFormat.setCellPadding(0);
    inputFormat.setCellSpacing(0);
    inputFormat.setMargin(8);
    QVector<QTextLength> constraints;
    constraints.resize(2);
    constraints[0] = QTextLength(QTextLength::FixedLength, 150.0);
    constraints[1] = QTextLength(QTextLength::PercentageLength, 100.0);
    inputFormat.setColumnWidthConstraints(constraints);
    QTextTable *frame = contents_->textCursor().insertTable(1, 2, inputFormat);
    const QString text = promptText.arg(++commandNumber_);    
    frame->setProperty(Sandbox::PropFrameType, Sandbox::FrameInput);
    frame->setProperty(Sandbox::PropEditableFrame, true);
    QTextCharFormat headerFormat = frame->cellAt(0, 0).format();
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setForeground(QColor(Qt::blue));
    frame->cellAt(0, 0).setFormat(headerFormat);
    frame->cellAt(0, 0).firstCursorPosition().insertText(text);
    contents_->setTextCursor(frame->cellAt(0, 1).firstCursorPosition());
}

void SandboxWidget::addTextOutputItem(const QString &outputText, const Sandbox::FrameType frameType)
{
    QString frameHeading = tr("Out [%1]: ");
    QColor frameColor = Qt::green;
    if (Sandbox::FrameResult == frameType) {
        frameHeading = tr("Result [%1]: ");
        frameColor = Qt::cyan;
    }
    else if (Sandbox::FrameError == frameType) {
        frameHeading = tr("Error [%1]: ");
        frameColor = Qt::red;
    }
    contents_->setTextCursor(contents_->document()->rootFrame()->lastCursorPosition());
    QTextTableFormat inputFormat;
    inputFormat.setBorder(0.5);
    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    inputFormat.setBorderBrush(frameColor);
    inputFormat.setCellPadding(0);
    inputFormat.setCellSpacing(0);
    inputFormat.setMargin(8);
    QVector<QTextLength> constraints;
    constraints.resize(2);
    constraints[0] = QTextLength(QTextLength::FixedLength, 150.0);
    constraints[1] = QTextLength(QTextLength::PercentageLength, 100.0);
    inputFormat.setColumnWidthConstraints(constraints);
    QTextTable *frame = contents_->textCursor().insertTable(1, 2, inputFormat);
    const QString text = frameHeading.arg(commandNumber_);
    frame->setProperty(Sandbox::PropFrameType, frameType);
    frame->setProperty(Sandbox::PropEditableFrame, false);
    QTextCharFormat headerFormat = frame->cellAt(0, 0).format();
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setForeground(frameColor);
    frame->cellAt(0, 0).setFormat(headerFormat);
    frame->cellAt(0, 0).firstCursorPosition().insertText(text);
    frame->cellAt(0, 1).firstCursorPosition().insertText(outputText);
    contents_->setTextCursor(frame->cellAt(0, 1).firstCursorPosition());
}

void SandboxWidget::addGraphicsOutputItem(const QImage &image)
{

}

void SandboxWidget::addHtmlItem(const QString &html)
{

}

void SandboxWidget::processCurrentInputFrame()
{
    QTextFrame * currentFrame = contents_->textCursor().currentFrame();
    currentFrame->setProperty(Sandbox::PropEditableFrame, false);

    const QTextTable * frameTable = qobject_cast<const QTextTable*>(currentFrame);
    Q_ASSERT(frameTable);

    const QTextTableCell cell = frameTable->cellAt(0, 1);
    QString text;
    for (QTextFrame::iterator it=cell.begin(); it!=cell.end(); ++it) {
        text += it.currentBlock().text();
    }

    text = text.trimmed();
    if (text.length() > 0) {
        lastCommandOutput_.clear();
        lastCommandError_.clear();
        PyEval_AcquireThread(py_);
        PyObject *mainModule = PyImport_AddModule("__main__");
        PyObject *globals = PyModule_GetDict(mainModule);
        PyObject *result = PyRun_String(text.toUtf8().constData(), Py_file_input, globals, globals);
        if (!result) {
            PyErr_Print();
        }
        if (lastCommandOutput_.length() > 0) {
            addTextOutputItem(lastCommandOutput_, Sandbox::FrameOutput);
        }
        if (lastCommandError_.length() > 0) {
            addTextOutputItem(lastCommandError_, Sandbox::FrameError);
        }
        if (result) {
            const QByteArray resultTypeName(result->ob_type->tp_name);
            if ("NoneType" != resultTypeName) {
                addTextOutputItem(PyObjectToQString(result), Sandbox::FrameResult);
            }
        }
        Py_XDECREF(result);
        PyEval_ReleaseThread(py_);
    }

    addDefaultInputItem();
}

SandboxWidget *SandboxWidget::instance(const QString & pythonPath, QWidget *parent)
{
    if (!self) {
        self = new SandboxWidget(pythonPath, parent);
    }
    return self;
}

PyObject *SandboxWidget::__init__()
{
    static PyMethodDef methods[] = {
        { "write_output", write_output, METH_VARARGS, "Output regular text to sandbox frame" },
        { "write_error", write_error, METH_VARARGS, "Output error (red) text to sandbox frame" },
        { "read_input", read_input, METH_VARARGS, "Read text line from sandbox frame" },
        { 0, 0, 0, 0 }
    };

    static PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "_kumir_sandbox", 0, -1, methods,
        0, 0, 0, 0
    };

    return PyModule_Create(&module);
}

PyObject *SandboxWidget::write_output(PyObject *, PyObject *args)
{
    PyObject * msg = PyTuple_GetItem(args, 0);
    QString message = PyUnicodeToQString(msg);
    self->lastCommandOutput_ += message;
    Py_RETURN_NONE;
}

PyObject *SandboxWidget::write_error(PyObject *, PyObject *args)
{
    PyObject * msg = PyTuple_GetItem(args, 0);
    QString message = PyUnicodeToQString(msg);
    self->lastCommandError_ += message;
    Py_RETURN_NONE;
}

PyObject *SandboxWidget::read_input(PyObject *, PyObject *args)
{
    PyObject* result = 0;
    result = QStringToPyUnicode("");
    Py_INCREF(result);
    return result;
}

bool SandboxWidget::eventFilter(QObject *obj, QEvent *evt)
{
    if (contents_ == obj && QEvent::KeyPress == evt->type()) {
        QKeyEvent *event = static_cast<QKeyEvent*>(evt);
        return filterKeyPressEvent(event);
    }
    return false;
}

bool SandboxWidget::filterKeyPressEvent(QKeyEvent *event)
{
    bool destructive = false;
    if (event->matches(QKeySequence::Delete)) {
        destructive = true;
    }
    else if (event->matches(QKeySequence::DeleteEndOfLine)) {
        destructive = true;
    }
    else if (event->matches(QKeySequence::DeleteEndOfWord)) {
        destructive = true;
    }
    else if (event->matches(QKeySequence::DeleteStartOfWord)) {
        destructive = true;
    }
    else if (event->matches(QKeySequence::InsertLineSeparator)) {
        destructive = true;
    }
    else if (event->matches(QKeySequence::InsertParagraphSeparator)) {
        destructive = true;
    }
    else if (event->matches(QKeySequence::Cut)) {
        destructive = true;
    }
    else if (event->matches(QKeySequence::Paste)) {
        destructive = true;
    }
    else if (event->matches(QKeySequence::Replace)) {
        destructive = true;
    }
    else if (!event->modifiers().testFlag(Qt::AltModifier)
             && !event->modifiers().testFlag(Qt::ControlModifier))
    {
        if (Qt::Key_Backspace == event->key()) {
            destructive = true;
        }
        else if (event->text().length() > 0) {
            destructive = true;
        }
    }

    const QTextFrame * currentFrame = contents_->textCursor().currentFrame();
    const bool editable = currentFrame->property(Sandbox::PropEditableFrame).toBool();
    const Sandbox::FrameType frameType = static_cast<Sandbox::FrameType>(
                currentFrame->property(Sandbox::PropFrameType).toInt()
                );

    const bool selfProcess =
            Sandbox::FrameInput == frameType &&
            event->matches(QKeySequence::InsertParagraphSeparator);

    if (selfProcess) {
        processCurrentInputFrame();
    }


    return selfProcess || (destructive && !editable);
}

void SandboxWidget::focusInEvent(QFocusEvent *event)
{
    event->accept();
    contents_->setFocus();
}

ExtensionSystem::SettingsPtr SandboxWidget::editorSettings()
{
    using namespace ExtensionSystem;
    using namespace Shared;

    KPlugin* editorPlugin = PluginManager::instance()->findKPlugin<EditorInterface>();
    if (editorPlugin) {
        return editorPlugin->mySettings();
    }
    else {
        return SettingsPtr();
    }
}

} // namespace Python3Language
