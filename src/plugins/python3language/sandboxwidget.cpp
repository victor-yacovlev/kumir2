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
#include <QScrollBar>
#include <QLocale>

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
    , editor_(new QTextEdit(this))
    , py_(0)
    , pythonPath_(pythonPath)
    , resetFlag_(0)
{
    QVBoxLayout * mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(editor_);
    editor_->installEventFilter(this);


    if (editorSettings()) {
        const QString fontName = editorSettings()->value("Font/Family", "monospace").toString();
        const int fontSize = editorSettings()->value("Font/Size", 12).toInt();
        QFont font = editor_->font();
        font.setFamily(fontName);
        font.setPointSize(fontSize);
        editor_->setFont(font);
    }

    reset();
}

void SandboxWidget::reset()
{
    editor_->clear();
    if (py_) {
        PyEval_AcquireThread(py_);
        Py_EndInterpreter(py_);
        PyEval_ReleaseLock();
    }

//#ifdef Q_OS_WIN32
//    if (_Py_atomic_load_relaxed(&_PyThreadState_Current))
//#else
    if (PyThreadState_GET())
//#endif
        PyEval_AcquireLock();
    else
        PyGILState_Ensure();
    py_ = Py_NewInterpreter();
#ifdef Q_OS_WIN32
    prepareBundledSysPath();
#else
    appendToSysPath(pythonPath_);
#endif
    createSysArgv(QStringList() << "");
    mainModule_ = PyImport_AddModule("__main__");
    PyObject* py_sandbox_wrapper = PyImport_ImportModule("sandbox_wrapper");
    if (!py_sandbox_wrapper) { printPythonTraceback(); return; }

    PyObject* sandbox_dict = PyModule_GetDict(py_sandbox_wrapper);
    PyObject* sandbox_keys = PyDict_Keys(sandbox_dict);
    PyObject* main_dict = PyModule_GetDict(mainModule_);
    Py_ssize_t itemsCount = PyObject_Length(sandbox_keys);
    for (Py_ssize_t i=0; i<itemsCount; ++i) {
        PyObject* key = PyList_GetItem(sandbox_keys, i);
        PyObject* value = PyDict_GetItem(sandbox_dict, key);
        if (PyCallable_Check(value)) {
            PyDict_SetItem(main_dict, key, value);
        }
    }
    PyEval_ReleaseThread(py_);

    commandNumber_ = 0;    
    resetFlag_ = false;

    createWelcomeFrame();

    addDefaultInputItem();
}

void SandboxWidget::addDefaultInputItem()
{
    addInputItem(tr("In [%1]> "));
}

void SandboxWidget::addInputItem(const QString &promptText)
{
    QColor headerColor = darkScheme() ? QColor("deepskyblue") : QColor(Qt::blue);
    editor_->setTextCursor(editor_->document()->rootFrame()->lastCursorPosition());
    QTextTableFormat inputFormat;
    inputFormat.setBorder(1);
    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    inputFormat.setBorderBrush(headerColor);
    inputFormat.setCellPadding(8);
    inputFormat.setCellSpacing(0);
    inputFormat.setMargin(8);
    QVector<QTextLength> constraints;
    constraints.resize(2);
    constraints[0] = QTextLength(QTextLength::FixedLength, 150.0);
    constraints[1] = QTextLength(QTextLength::PercentageLength, 100.0);
    inputFormat.setColumnWidthConstraints(constraints);
    QTextTable *frame = editor_->textCursor().insertTable(1, 2, inputFormat);
    const QString text = promptText.arg(++commandNumber_);    
    frame->setProperty(Sandbox::PropFrameType, Sandbox::FrameInput);
    frame->setProperty(Sandbox::PropEditableFrame, true);
    QTextCharFormat headerFormat = frame->cellAt(0, 0).format();
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setForeground(headerColor);
    frame->cellAt(0, 0).setFormat(headerFormat);
    frame->cellAt(0, 0).firstCursorPosition().insertText(text);
    editor_->setTextCursor(frame->cellAt(0, 1).firstCursorPosition());
    editor_->verticalScrollBar()->setValue(editor_->verticalScrollBar()->maximum());
}

void SandboxWidget::addTextOutputItem(const QString &outputText, const Sandbox::FrameType frameType)
{
    QString frameHeading = tr("Out [%1]: ");
    QColor frameColor = darkScheme() ? Qt::white : Qt::darkGreen;
    if (Sandbox::FrameError == frameType) {
        frameHeading = tr("Error [%1]: ");
        frameColor = darkScheme() ? QColor("orangered") : QColor(Qt::red);
    }
    editor_->setTextCursor(editor_->document()->rootFrame()->lastCursorPosition());
    QTextTableFormat inputFormat;
    inputFormat.setBorder(1);
    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    inputFormat.setBorderBrush(frameColor);
    inputFormat.setCellPadding(8);
    inputFormat.setCellSpacing(0);
    inputFormat.setMargin(8);
    QVector<QTextLength> constraints;
    constraints.resize(2);
    constraints[0] = QTextLength(QTextLength::FixedLength, 150.0);
    constraints[1] = QTextLength(QTextLength::PercentageLength, 100.0);
    inputFormat.setColumnWidthConstraints(constraints);
    QTextTable *frame = editor_->textCursor().insertTable(1, 2, inputFormat);
    const QString text = frameHeading.arg(commandNumber_);
    frame->setProperty(Sandbox::PropFrameType, frameType);
    frame->setProperty(Sandbox::PropEditableFrame, false);
    QTextCharFormat headerFormat = frame->cellAt(0, 0).format();
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setForeground(frameColor);
    frame->cellAt(0, 0).setFormat(headerFormat);
    frame->cellAt(0, 0).firstCursorPosition().insertText(text);
    frame->cellAt(0, 1).firstCursorPosition().insertText(outputText);
    editor_->setTextCursor(frame->cellAt(0, 1).firstCursorPosition());
    editor_->verticalScrollBar()->setValue(editor_->verticalScrollBar()->maximum());
}

void SandboxWidget::addGraphicsOutputItem(const QImage &image)
{

}

void SandboxWidget::addHtmlItem(const QString &html)
{
    QColor headerColor = darkScheme() ? QColor("darkgray") : QColor("lightgray");
    editor_->setTextCursor(editor_->document()->rootFrame()->lastCursorPosition());
    QTextFrameFormat inputFormat;
    inputFormat.setBorder(1);
    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    inputFormat.setBorderBrush(headerColor);
    inputFormat.setPadding(8);
    inputFormat.setMargin(8);
    QTextFrame *frame = editor_->textCursor().insertFrame(inputFormat);
    frame->setProperty(Sandbox::PropFrameType, Sandbox::FrameHtml);
    frame->setProperty(Sandbox::PropEditableFrame, false);
    frame->firstCursorPosition().insertHtml(html);
    editor_->setTextCursor(editor_->document()->rootFrame()->lastCursorPosition());
    editor_->verticalScrollBar()->setValue(editor_->verticalScrollBar()->maximum());
}

void SandboxWidget::processCurrentInputFrame()
{
    QTextFrame * currentFrame = editor_->textCursor().currentFrame();
    currentFrame->setProperty(Sandbox::PropEditableFrame, false);

    const QTextTable * frameTable = qobject_cast<const QTextTable*>(currentFrame);
    Q_ASSERT(frameTable);

    const QTextTableCell cell = frameTable->cellAt(0, 1);
    QString text;
    for (QTextFrame::iterator it=cell.begin(); it!=cell.end(); ++it) {
        if (text.length() > 0)
            text += "\n";
        text += it.currentBlock().text();
    }

    text = text.trimmed();
    if (text.length() > 0) {
        lastCommandOutput_.clear();
        lastCommandError_.clear();
        PyEval_AcquireThread(py_);
        PyObject *globals = PyModule_GetDict(mainModule_);
        PyObject *result = PyRun_String(text.toUtf8().constData(), Py_single_input, globals, globals);
        if (!result) {
            PyErr_Print();
        }
        if (!resetFlag_ && lastCommandOutput_.length() > 0) {
            addTextOutputItem(lastCommandOutput_, Sandbox::FrameOutput);
        }
        if (lastCommandError_.length() > 0) {
            addTextOutputItem(lastCommandError_, Sandbox::FrameError);
        }
        Py_XDECREF(result);
        PyEval_ReleaseThread(py_);
    }
    if (resetFlag_) {
        reset();
    }
    else {
        addDefaultInputItem();
    }
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
        { "_reset", _reset, METH_VARARGS, "Reset interpreter" },
        { 0, 0, 0, 0 }
    };

    static PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "_kumir_sandbox", 0, -1, methods,
        0, 0, 0, 0
    };

    return PyModule_Create(&module);
}

void SandboxWidget::createWelcomeFrame()
{
    const QString currentLanguage = "ru"; // TODO: internationalize!
    const QString welcomeFileName = QDir(pythonPath_).absoluteFilePath(
                QString("sandbox_greeting_%1.html").arg(currentLanguage)
                );
    QFile greeting(welcomeFileName);
    if (greeting.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream stream(&greeting);
        stream.setCodec("UTF-8");
        stream.setAutoDetectUnicode(true);
        const QString htmlData = stream.readAll();
        greeting.close();
        addHtmlItem(htmlData);
    }
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

PyObject *SandboxWidget::_reset(PyObject *, PyObject *)
{
    self->resetMutex_.lock();
    self->resetFlag_ = true;
    self->resetMutex_.unlock();
    Py_RETURN_NONE;
}

bool SandboxWidget::eventFilter(QObject *obj, QEvent *evt)
{
    if (editor_ == obj && QEvent::KeyPress == evt->type()) {
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

    const QTextFrame * currentFrame = editor_->textCursor().currentFrame();
    const bool editable = currentFrame->property(Sandbox::PropEditableFrame).toBool();
    const Sandbox::FrameType frameType = static_cast<Sandbox::FrameType>(
                currentFrame->property(Sandbox::PropFrameType).toInt()
                );

    const bool selfProcess =
            Sandbox::FrameInput == frameType &&
                            event->matches(QKeySequence::InsertParagraphSeparator);
    const bool selfEdit =
            Sandbox::FrameInput == frameType &&
            (
                event->matches(QKeySequence::InsertLineSeparator) ||
                (0 == event->modifiers() && Qt::Key_Tab == event->key())
            );

    if (selfProcess) {
        processCurrentInputFrame();
    }

    if (selfEdit) {
        performEditOperation(event);
    }


    return selfProcess || selfEdit || (destructive && !editable);
}

void SandboxWidget::performEditOperation(QKeyEvent *event)
{
    if (event->matches(QKeySequence::InsertLineSeparator)) {
        QTextBlock block = editor_->textCursor().block();
        const QString text = block.text();
        QString textToInsert = "\n";
        for (int i=0; i<text.length(); ++i) {
            if (' ' == text[i]) {
                textToInsert.push_back(' ');
            }
            else {
                break;
            }
        }
        editor_->textCursor().insertText(textToInsert);
    }
    else if (Qt::Key_Tab == event->key()) {
        editor_->textCursor().insertText("    ");
    }
}

void SandboxWidget::focusInEvent(QFocusEvent *event)
{
    event->accept();
    editor_->setFocus();
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

bool SandboxWidget::darkScheme() const
{
    const QColor bgColor = palette().color(QPalette::Base);
    int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
    return darkness / 3 <= 127;
}

} // namespace Python3Language
