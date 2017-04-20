#include "sandboxwidget.h"
#include "sandboxwidget_frame.h"

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

#include <kumir2-libs/extensionsystem/pluginmanager.h>
#include <kumir2-libs/extensionsystem/kplugin.h>
#include <kumir2/editorinterface.h>

namespace Python3Language {

SandboxWidget::SandboxWidget(const QString & pythonPath, QWidget *parent)
    : QWidget(parent)
    , _editor(new QTextEdit(this))
    , _pythonPath(pythonPath)
    , _pyInterpreterProcess(0)
{
    QVBoxLayout * mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(_editor);
    _editor->installEventFilter(this);


    if (editorSettings()) {
        const QString fontName = editorSettings()->value("Font/Family", "monospace").toString();
        const int fontSize = editorSettings()->value("Font/Size", 12).toInt();
        QFont font = _editor->font();
        font.setFamily(fontName);
        font.setPointSize(fontSize);
        _editor->setFont(font);
    }

    reset();
}

void SandboxWidget::createInterpreterProcess()
{
    _pyInterpreterProcess = PyInterpreterProcess::create(this);
    connect(_pyInterpreterProcess, SIGNAL(stdoutReceived(QString)), this, SLOT(handleStdOut(QString)));
    connect(_pyInterpreterProcess, SIGNAL(stderrReceived(QString)), this, SLOT(handleStdErr(QString)));
    connect(_pyInterpreterProcess, SIGNAL(inputRequiestReceived(QString)), this, SLOT(addPyInputItem(QString)));
    connect(_pyInterpreterProcess, SIGNAL(resetReceived()), this, SLOT(reset()));
    connect(_pyInterpreterProcess, SIGNAL(processRespawned(int,QProcess::ExitStatus)),
            this, SLOT(handleProcessRespawned(int,QProcess::ExitStatus)));
}

void SandboxWidget::reset()
{    
//    if (py_) {
//        PyEval_AcquireThread(py_);
//        Py_EndInterpreter(py_);
//        PyEval_ReleaseLock();
//    }

////#ifdef Q_OS_WIN32
////    if (_Py_atomic_load_relaxed(&_PyThreadState_Current))
////#else
//    if (PyThreadState_GET())
////#endif
//        PyEval_AcquireLock();
//    else
//        PyGILState_Ensure();
//    py_ = Py_NewInterpreter();
//#ifdef Q_OS_WIN32
//    prepareBundledSysPath();
//#else
//    appendToSysPath(pythonPath_);
//#endif
//    createSysArgv(QStringList() << "");
//    mainModule_ = PyImport_AddModule("__main__");
//    PyObject* py_sandbox_wrapper = PyImport_ImportModule("sandbox_wrapper");
//    if (!py_sandbox_wrapper) { printPythonTraceback(); return; }

//    PyObject* sandbox_dict = PyModule_GetDict(py_sandbox_wrapper);
//    PyObject* sandbox_keys = PyDict_Keys(sandbox_dict);
//    PyObject* main_dict = PyModule_GetDict(mainModule_);
//    Py_ssize_t itemsCount = PyObject_Length(sandbox_keys);
//    for (Py_ssize_t i=0; i<itemsCount; ++i) {
//        PyObject* key = PyList_GetItem(sandbox_keys, i);
//        PyObject* value = PyDict_GetItem(sandbox_dict, key);
//        if (PyCallable_Check(value)) {
//            PyDict_SetItem(main_dict, key, value);
//        }
//    }
//    PyEval_ReleaseThread(py_);

//    commandNumber_ = 0;
//    resetFlag_ = false;

    if (_pyInterpreterProcess) {
        _pyInterpreterProcess->disconnect(this);
        _pyInterpreterProcess->sendExit();
        _pyInterpreterProcess->deleteLater();
        QCoreApplication::processEvents();
    }

    createInterpreterProcess();

    _editor->clear();
    createWelcomeFrame();
    addDefaultInputItem();
}

void SandboxWidget::addDefaultInputItem()
{
    addInputItem(tr(">>> "));
}

void SandboxWidget::addInputItem(const QString &promptText)
{
    const QTextFrame * currentFrame = _editor->textCursor().currentFrame();
    const bool editable = currentFrame->property(Sandbox::PropEditableFrame).toBool();
    const Sandbox::FrameType frameType = static_cast<Sandbox::FrameType>(
                currentFrame->property(Sandbox::PropFrameType).toInt()
                );
    if (frameType == Sandbox::FrameError || frameType == Sandbox::FrameOutput) {
        _editor->textCursor().deletePreviousChar();
    }


    QColor headerColor = darkScheme() ? QColor("deepskyblue") : QColor(Qt::blue);
    _editor->setTextCursor(_editor->document()->rootFrame()->lastCursorPosition());
    QTextTableFormat inputFormat;
    inputFormat.setBorder(1);
    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    inputFormat.setBorderBrush(headerColor);
    inputFormat.setCellPadding(4);
    inputFormat.setCellSpacing(0);
    inputFormat.setMargin(4);
    QVector<QTextLength> constraints;
    constraints.resize(2);
    constraints[0] = QTextLength(QTextLength::FixedLength, 150.0);
    constraints[1] = QTextLength(QTextLength::PercentageLength, 100.0);
    inputFormat.setColumnWidthConstraints(constraints);
    QTextTable *frame = _editor->textCursor().insertTable(1, 2, inputFormat);
    const QString text = promptText;
    frame->setProperty(Sandbox::PropFrameType, Sandbox::FrameInput);
    frame->setProperty(Sandbox::PropEditableFrame, true);
    QTextCharFormat headerFormat = frame->cellAt(0, 0).format();
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setForeground(headerColor);
    frame->cellAt(0, 0).setFormat(headerFormat);
    frame->cellAt(0, 0).firstCursorPosition().insertText(text);
    _editor->setTextCursor(frame->cellAt(0, 1).firstCursorPosition());
    _editor->verticalScrollBar()->setValue(_editor->verticalScrollBar()->maximum());
}

void SandboxWidget::addPyInputItem(const QString &promptText)
{
    QColor headerColor = darkScheme() ? QColor("deepskyblue") : QColor(Qt::magenta);
    _editor->setTextCursor(_editor->document()->rootFrame()->lastCursorPosition());
    QTextTableFormat inputFormat;
    inputFormat.setBorder(1);
    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    inputFormat.setBorderBrush(headerColor);
    inputFormat.setCellPadding(4);
    inputFormat.setCellSpacing(0);
    inputFormat.setMargin(4);
    QVector<QTextLength> constraints;
    constraints.resize(2);
    constraints[0] = QTextLength(QTextLength::FixedLength, 150.0);
    constraints[1] = QTextLength(QTextLength::PercentageLength, 100.0);
    inputFormat.setColumnWidthConstraints(constraints);
    QTextTable *frame = _editor->textCursor().insertTable(1, 2, inputFormat);
    const QString text = promptText.isEmpty() ? QString("Input > ") : promptText + " ";
    frame->setProperty(Sandbox::PropFrameType, Sandbox::FramePyInput);
    frame->setProperty(Sandbox::PropEditableFrame, true);
    QTextCharFormat headerFormat = frame->cellAt(0, 0).format();
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setForeground(headerColor);
    frame->cellAt(0, 0).setFormat(headerFormat);
    frame->cellAt(0, 0).firstCursorPosition().insertText(text);
    _editor->setTextCursor(frame->cellAt(0, 1).firstCursorPosition());
    _editor->verticalScrollBar()->setValue(_editor->verticalScrollBar()->maximum());
}

void SandboxWidget::addTextOutputItem(const QString &outputText, const Sandbox::FrameType frameType)
{
//    QString frameHeading = tr("Out [%1]: ");
//    QColor frameColor = darkScheme() ? Qt::white : Qt::darkGreen;
//    if (Sandbox::FrameError == frameType) {
//        frameHeading = tr("Error [%1]: ");
//        frameColor = darkScheme() ? QColor("orangered") : QColor(Qt::red);
//    }
//    editor_->setTextCursor(editor_->document()->rootFrame()->lastCursorPosition());
//    QTextTableFormat inputFormat;
//    inputFormat.setBorder(1);
//    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
//    inputFormat.setBorderBrush(frameColor);
//    inputFormat.setCellPadding(8);
//    inputFormat.setCellSpacing(0);
//    inputFormat.setMargin(8);
//    QVector<QTextLength> constraints;
//    constraints.resize(2);
//    constraints[0] = QTextLength(QTextLength::FixedLength, 150.0);
//    constraints[1] = QTextLength(QTextLength::PercentageLength, 100.0);
//    inputFormat.setColumnWidthConstraints(constraints);
//    QTextTable *frame = editor_->textCursor().insertTable(1, 2, inputFormat);
//    const QString text = frameHeading.arg(commandNumber_);
//    frame->setProperty(Sandbox::PropFrameType, frameType);
//    frame->setProperty(Sandbox::PropEditableFrame, false);
//    QTextCharFormat headerFormat = frame->cellAt(0, 0).format();
//    headerFormat.setFontWeight(QFont::Bold);
//    headerFormat.setForeground(frameColor);
//    frame->cellAt(0, 0).setFormat(headerFormat);
//    frame->cellAt(0, 0).firstCursorPosition().insertText(text);
//    frame->cellAt(0, 1).firstCursorPosition().insertText(outputText);
//    editor_->setTextCursor(frame->cellAt(0, 1).firstCursorPosition());
//    editor_->verticalScrollBar()->setValue(editor_->verticalScrollBar()->maximum());

    QColor frameColor = darkScheme() ? QColor("darkgray") : QColor("lightgray");
    if (Sandbox::FrameError == frameType) {
        frameColor = darkScheme() ? QColor("orangered") : QColor(Qt::red);
    }
    _editor->setTextCursor(_editor->document()->rootFrame()->lastCursorPosition());
    QTextFrameFormat inputFormat;
    inputFormat.setBorder(0);
    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    inputFormat.setBorderBrush(frameColor);
    inputFormat.setPadding(0);
    inputFormat.setMargin(4);
    QTextFrame *frame = _editor->textCursor().insertFrame(inputFormat);
    frame->setProperty(Sandbox::PropFrameType, frameType);
    frame->setProperty(Sandbox::PropEditableFrame, false);
    frame->firstCursorPosition().insertText(outputText);
    _editor->setTextCursor(frame->lastCursorPosition());
    _editor->verticalScrollBar()->setValue(_editor->verticalScrollBar()->maximum());
}

void SandboxWidget::addGraphicsOutputItem(const QImage &image)
{

}

void SandboxWidget::addHtmlItem(const QString &html)
{
    QColor headerColor = darkScheme() ? QColor("darkgray") : QColor("lightgray");
    _editor->setTextCursor(_editor->document()->rootFrame()->lastCursorPosition());
    QTextFrameFormat inputFormat;
    inputFormat.setBorder(0);
    inputFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    inputFormat.setBorderBrush(headerColor);
    inputFormat.setPadding(0);
    inputFormat.setMargin(4);
    QTextFrame *frame = _editor->textCursor().insertFrame(inputFormat);
    frame->setProperty(Sandbox::PropFrameType, Sandbox::FrameHtml);
    frame->setProperty(Sandbox::PropEditableFrame, false);
    frame->firstCursorPosition().insertHtml(html);
    _editor->setTextCursor(_editor->document()->rootFrame()->lastCursorPosition());
    _editor->verticalScrollBar()->setValue(_editor->verticalScrollBar()->maximum());
}

void SandboxWidget::processCurrentInputFrame()
{
    QTextFrame * currentFrame = _editor->textCursor().currentFrame();
    currentFrame->setProperty(Sandbox::PropEditableFrame, false);

    bool plainInput = currentFrame->property(Sandbox::PropFrameType).toInt() == Sandbox::FramePyInput;

    const QTextTable * frameTable = qobject_cast<const QTextTable*>(currentFrame);
    Q_ASSERT(frameTable);

    const QTextTableCell cell = frameTable->cellAt(0, 1);
    QStringList lines;
    QString text;
    for (QTextFrame::iterator it=cell.begin(); it!=cell.end(); ++it) {
        text = it.currentBlock().text();
        lines.append(text);
    }

    text = lines.join("\n");

    if (text.trimmed().length() > 0) {

        if (plainInput) {
            _pyInterpreterProcess->sendInput(text + "\n");
        }
        else {
            if (lines.size() > 1) {
                bool completeStatement = lines.last().trimmed().isEmpty();
                if (completeStatement)
                    text += "\n";
            }
            _pyInterpreterProcess->nonBlockingEval(text, this, "handleEvalFinished");
        }
    }
    else {
        currentFrame->setProperty(Sandbox::PropEditableFrame, true);
    }
}

void SandboxWidget::handleEvalFinished(bool moreLinesRequired)
{
    if (!moreLinesRequired) {
        addDefaultInputItem();
    }
    else {
        QTextFrame * currentFrame = _editor->textCursor().currentFrame();
        currentFrame->setProperty(Sandbox::PropEditableFrame, true);

        QTextBlock block = _editor->textCursor().block();
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
        if (!textToInsert.startsWith(' ') && text.trimmed().endsWith(":"))
            textToInsert = "\n    ";
        _editor->textCursor().insertText(textToInsert);
    }
}

void SandboxWidget::handleStdOut(const QString &message)
{
//    lastCommandOutput_ += message;
    const QTextFrame * currentFrame = _editor->textCursor().currentFrame();
    const bool editable = currentFrame->property(Sandbox::PropEditableFrame).toBool();
    const Sandbox::FrameType frameType = static_cast<Sandbox::FrameType>(
                currentFrame->property(Sandbox::PropFrameType).toInt()
                );
    if (frameType == Sandbox::FrameOutput) {
        _editor->textCursor().insertText(message);
    }
    else {
        addTextOutputItem(message, Sandbox::FrameOutput);
    }
}

void SandboxWidget::handleStdErr(const QString &message)
{
    const QTextFrame * currentFrame = _editor->textCursor().currentFrame();
    const bool editable = currentFrame->property(Sandbox::PropEditableFrame).toBool();
    const Sandbox::FrameType frameType = static_cast<Sandbox::FrameType>(
                currentFrame->property(Sandbox::PropFrameType).toInt()
                );
    if (frameType == Sandbox::FrameError) {
        _editor->textCursor().insertText(message);
    }
    else {
        addTextOutputItem(message, Sandbox::FrameError);
    }
}

void SandboxWidget::handleProcessRespawned(int exitCode, QProcess::ExitStatus exitStatus)
{
    const QString message =
            tr("Interpreter restarted after been exited with code %1")
            .arg(exitCode) + "\n";
    addTextOutputItem(message, Sandbox::FrameError);
    addDefaultInputItem();
}


void SandboxWidget::createWelcomeFrame()
{
    const QString currentLanguage = "ru"; // TODO: internationalize!
    const QString welcomeFileName = QDir(_pythonPath).absoluteFilePath(
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


bool SandboxWidget::eventFilter(QObject *obj, QEvent *evt)
{
    if (_editor == obj && QEvent::KeyPress == evt->type()) {
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

    const QTextFrame * currentFrame = _editor->textCursor().currentFrame();
    const bool editable = currentFrame->property(Sandbox::PropEditableFrame).toBool();
    const Sandbox::FrameType frameType = static_cast<Sandbox::FrameType>(
                currentFrame->property(Sandbox::PropFrameType).toInt()
                );

    const bool selfProcess =
            (Sandbox::FrameInput == frameType || Sandbox::FramePyInput == frameType) &&
                            event->matches(QKeySequence::InsertParagraphSeparator);
    const bool selfEdit =
            (Sandbox::FrameInput == frameType || Sandbox::FramePyInput == frameType) &&
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
        QTextBlock block = _editor->textCursor().block();
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
        _editor->textCursor().insertText(textToInsert);
    }
    else if (Qt::Key_Tab == event->key()) {
        _editor->textCursor().insertText("    ");
    }
}

void SandboxWidget::focusInEvent(QFocusEvent *event)
{
    event->accept();
    _editor->setFocus();
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
