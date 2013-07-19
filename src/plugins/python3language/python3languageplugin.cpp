#include "python3languageplugin.h"
#include "Python.h"
#include <iostream>

namespace Python3Language {

Python3LanguagePlugin::Python3LanguagePlugin()
    : ExtensionSystem::KPlugin()
    , analizerModule_(NULL)
    , runInteractionWaiter_(nullptr)
    , runWorker_(nullptr)
    , mainPyThreadState_(NULL)

{

}

static Python3LanguagePlugin * PLUGIN = nullptr;
static RunWorker * WORKER = nullptr;
static RunInteractionWaiter * WAITER = nullptr;

static ::PyObject* _kumir_debug(PyObject *, PyObject *args)
{
    ::PyObject * msg = PyTuple_GetItem(args, 0);
    wchar_t * wcs = PyUnicode_AsWideCharString(msg, NULL);
    QString qs = QString::fromWCharArray(wcs);
    qDebug() << qs;
    PyMem_Free(wcs);
    Py_INCREF(Py_None);
    return Py_None;
}

static ::PyObject* _kumir_write_to_stdout(PyObject *, PyObject *args)
{
    ::PyObject * msg = PyTuple_GetItem(args, 0);
    QString qmsg;
    if (PyUnicode_Check(msg)) {
        wchar_t * buffer = PyUnicode_AsWideCharString(msg, 0);
        qmsg = QString::fromWCharArray(buffer);
        PyMem_Free(buffer);
    }
    else if (PyBytes_Check(msg)) {
        char * buffer = PyBytes_AsString(msg);
        qmsg = QString::fromLocal8Bit(buffer);
        PyMem_Free(buffer);
    }
    if (PLUGIN) {
        PLUGIN->handlePythonOutput(qmsg);
    }
    else {
        qDebug() << qmsg;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static ::PyObject* _kumir_write_to_stderr(PyObject *, PyObject *args)
{
    ::PyObject * msg = PyTuple_GetItem(args, 0);
    QString qmsg;
    if (PyUnicode_Check(msg)) {
        wchar_t * buffer = PyUnicode_AsWideCharString(msg, 0);
        qmsg = QString::fromWCharArray(buffer);
        PyMem_Free(buffer);
    }
    else if (PyBytes_Check(msg)) {
        char * buffer = PyBytes_AsString(msg);
        qmsg = QString::fromLocal8Bit(buffer);
        PyMem_Free(buffer);
    }
    if (PLUGIN) {
        PLUGIN->handlePythonError(qmsg);
    }
    else {
        qDebug() << qmsg;
    }
    Py_INCREF(Py_None);
    return Py_None;
}


static ::PyObject* _kumir_user_return(PyObject*, PyObject * args)
{
    Py_INCREF(Py_None);
    return Py_None;
}

static ::PyObject* _kumir_user_call(PyObject*, PyObject * args)
{
    Py_INCREF(Py_None);
    return Py_None;
}

static ::PyObject* _kumir_user_line(PyObject*, PyObject * args)
{
    ::PyObject* frame = PyTuple_GetItem(args, 0);
    ::PyObject* f_lineno = PyObject_GetAttrString(frame, "f_lineno");
    bool sameFile = true;

    if (WORKER && WORKER->mode!=RM_Blind) {
        static const QString DummyFileName = QString::fromAscii("<string>");
        ::PyObject* f_code = PyObject_GetAttrString(frame, "f_code");
        ::PyObject* co_filename = PyObject_GetAttrString(f_code, "co_filename");
        wchar_t * buffer = PyUnicode_AsWideCharString(co_filename, 0);
        const QString frameFileName = QString::fromWCharArray(buffer);
        PyMem_Free(buffer);
        sameFile = frameFileName==DummyFileName || frameFileName==WORKER->fileName;
    }

    if (WORKER && sameFile) {
        bool emitSteps = false;
        unsigned long int counter = 0u;
        WORKER->stepsCounterMutex->lock();
        if (!WORKER->justStarted) {
            WORKER->stepsCounted ++;
            emitSteps = WORKER->mode!=RM_Blind || WORKER->stepsCounted % 1000 == 0;
            counter = WORKER->stepsCounted;
        }
        WORKER->justStarted = false;
        WORKER->stepsCounterMutex->unlock();
        if (emitSteps && PLUGIN) {
            PLUGIN->handlePythonStepsCounterChanged(counter);
        }
    }

    if (WORKER && WORKER->mode!=RM_Blind && sameFile) {
        WORKER->previousFrameGlobals = WORKER->currentFrameGlobals;
        WORKER->previousFrameLocals = WORKER->currentFrameLocals;
        WORKER->previousLineNo = WORKER->currentLineNo;
        WORKER->currentLineNo = PyLong_AsLong(f_lineno) - 1;
        WORKER->currentFrameLocals = PyObject_GetAttrString(frame, "f_locals");
        WORKER->currentFrameGlobals = PyObject_GetAttrString(frame, "f_globals");
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        if (WORKER->mode!=RM_Continuous)
            PLUGIN->handlePythonLineChanged(PLUGIN->runWorker_->currentLineNo);
        if (WORKER->previousLineNo != -1 &&
                WORKER->previousLineNo < WORKER->lvalueAtoms.size() &&
                WORKER->previousFrameLocals && WORKER->previousFrameGlobals)
        {
            const QStringList & atoms = WORKER->lvalueAtoms[WORKER->previousLineNo];
            QStringList assignments;
            for (int i=0; i<atoms.size(); i++) {
                const QString & atom = atoms[i];
                const char * atom_name = atom.toUtf8().data();
                QString qvalue;
                ::PyObject * value = NULL;
                value = PyDict_GetItemString(WORKER->previousFrameLocals, atom_name);
                if (PyErr_Occurred()) {
                    PyErr_Print();
                }
                if (!value) {
                    value = PyDict_GetItemString(WORKER->previousFrameGlobals, atom_name);
                    if (PyErr_Occurred()) {
                        PyErr_Print();
                    }
                }
                if (value) {
                    ::PyObject * repr = PyObject_Repr(value);
                    if (PyErr_Occurred()) {
                        PyErr_Print();
                    }
                    if (repr) {
                        wchar_t* buffer = PyUnicode_AsWideCharString(repr, 0);
                        qvalue = QString::fromWCharArray(buffer);
                        PyMem_Free(buffer);
//                        Py_DECREF(repr);
                    }
//                    Py_DECREF(value);
                }
                assignments << QString("%1 = %2").arg(atom).arg(qvalue);
            }
            if (assignments.size() > 0) {
                const QString marginLine = assignments.join(", ");
                PLUGIN->handlePythonMarginText(WORKER->previousLineNo,
                                               marginLine);
            }
        }
    }

    ::PyObject* result = NULL;  // continue evaluation or not
    if (WORKER && WAITER) {
        if (sameFile)
            WAITER->tryToPause();
        if (WORKER->isTerminate()) {
            WORKER->stepsCounterMutex->lock();
            WORKER->stepsCounted --;
            WORKER->stepsCounterMutex->unlock();
            result = PyLong_FromLong(0);
        }
        else {
            result = PyLong_FromLong(1);
        }
    }
    return result;
}

static ::PyObject* _kumir_user_exception(PyObject*, PyObject * args)
{
    ::PyObject* frame = PyTuple_GetItem(args, 0);
    ::PyObject* f_lineno = PyObject_GetAttrString(frame, "f_lineno");
    ::PyObject* exc_info = PyTuple_GetItem(args, 1);
    ::PyObject* err = PyTuple_GetItem(exc_info, 1);
    wchar_t* buffer = PyUnicode_AsWideCharString(err, 0);
    if (PLUGIN && WORKER) {
        WORKER->currentLineNo = PyLong_AsLong(f_lineno) - 1;
        WORKER->error = QString::fromWCharArray(buffer);
        PLUGIN->handlePythonLineChanged(PLUGIN->runWorker_->currentLineNo);
    }
    PyMem_Free(buffer);
    Py_INCREF(Py_None);
    return Py_None;
}


static ::PyMethodDef _kumirMethods[] = {
    { "debug", _kumir_debug, METH_VARARGS, ""},
    { "write_to_stdout", _kumir_write_to_stdout, METH_VARARGS, ""},
    { "write_to_stderr", _kumir_write_to_stderr, METH_VARARGS, ""},
    { "user_return", _kumir_user_return, METH_VARARGS, ""},
    { "user_call", _kumir_user_call, METH_VARARGS, ""},
    { "user_line", _kumir_user_line, METH_VARARGS, ""},
    { "user_exception", _kumir_user_exception, METH_VARARGS, ""},
    { NULL, NULL, 0, NULL }
};

static ::PyModuleDef _kumirModule = {
    PyModuleDef_HEAD_INIT, "_kumir", NULL, -1, _kumirMethods,
    NULL, NULL, NULL, NULL
};

static ::PyObject* PyInit__kumir(void)
{
    return PyModule_Create(&_kumirModule);
}

void Python3LanguagePlugin::prepareAnalizer()
{
    mainPyThreadState_ = Py_NewInterpreter();
    static const QString pyLibPath = qApp->property("sharePath").toString()+"/python3language";
    static const QByteArray analizer_py_Path = pyLibPath.toLocal8Bit();
    char * analizer_py = (char*)calloc(analizer_py_Path.size(), sizeof(char));
    strcpy(analizer_py, analizer_py_Path.constData());
    ::PyObject * sysPath = PySys_GetObject("path");
    ::PyObject * kumirPath = PyUnicode_FromString(analizer_py);
    PyList_Insert(sysPath, 0, kumirPath);
    PyImport_AppendInittab("_kumir", &PyInit__kumir);
    analizerModule_ = PyImport_ImportModule("analizer");
    if (!analizerModule_) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    if (analizerState_.size() > 0) {
        const std::wstring ws = analizerState_.toStdWString();
        ::PyObject * py_dump = PyUnicode_FromWideChar(ws.data(), ws.length());
        ::PyObject * py_restore_state = PyObject_GetAttrString(analizerModule_, "restore_state");
        ::PyObject * args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, py_dump);
        ::PyObject * res = PyObject_CallObject(py_restore_state, args);
        if (!res || PyErr_Occurred()) {
            PyErr_Print();
        }

    }
}

void Python3LanguagePlugin::saveAnalizer()
{
    ::PyObject * py_save_state = PyObject_GetAttrString(analizerModule_, "save_state");
    ::PyObject * args = PyTuple_New(0);
    ::PyObject * py_dump = PyObject_CallObject(py_save_state, args);
    if (!py_dump || PyErr_Occurred()) {
        PyErr_Print();
    }
    else {
        wchar_t * buffer = PyUnicode_AsWideCharString(py_dump, 0);
        analizerState_ = QString::fromWCharArray(buffer);
        PyMem_Free(buffer);
    }
    Py_EndInterpreter(mainPyThreadState_);
}

QString Python3LanguagePlugin::initialize(const QStringList &)
{    
    runInteractionWaiter_ = new RunInteractionWaiter(this);
    runWorker_ = new RunWorker(this);
    connect(runWorker_, SIGNAL(finished()), this, SLOT(prepareAnalizer()));

    static const QString pyLibPath = qApp->property("sharePath").toString()+"/python3language";
    static const QByteArray analizer_py_Path = pyLibPath.toLocal8Bit();
    char * analizer_py = (char*)calloc(analizer_py_Path.size(), sizeof(char));
    strcpy(analizer_py, analizer_py_Path.constData());
    PyImport_AppendInittab("_kumir", &PyInit__kumir);
    Py_Initialize();    
    PyEval_InitThreads();
//    ::PyObject * sysPath = PySys_GetObject("path");
//    ::PyObject * kumirPath = PyUnicode_FromString(analizer_py);
//    PyList_Insert(sysPath, 0, kumirPath);
//    analizerModule_ = PyImport_ImportModule("analizer");
//    if (!analizerModule_) {
//        ::PyObject * ptype, *pvalue, *ptraceback;
//        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
//        PyObject_Print(pvalue, stderr,0);
//        PyObject_Print(ptraceback, stderr,0);
//        return "Can't import python module 'analizer' using path " + pyLibPath;
//    }
    prepareAnalizer();

    PLUGIN = this;
    WORKER = runWorker_;
    WAITER = runInteractionWaiter_;
    return "";
}

int Python3LanguagePlugin::newDocument()
{    
    ::PyObject* py_new_document = PyObject_GetAttrString(analizerModule_,
                                                    "new_document");
    if (!py_new_document) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    ::PyObject* py_id = PyObject_CallFunction(py_new_document, 0);
    if (!py_id) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    int result = PyLong_AsLong(py_id);
    Py_DecRef(py_id);
    return result;
}

void Python3LanguagePlugin::setSourceText(int documentId, const QString &text)
{    
    ::PyObject * py_set_source_text = PyObject_GetAttrString(analizerModule_, "set_source_text");
    ::PyObject * args = PyTuple_New(2);
    std::wstring ws = text.toStdWString();
    ::PyObject * id = PyLong_FromLong(documentId);
    ::PyObject * txt = PyUnicode_FromWideChar(ws.c_str(), ws.length());
    PyTuple_SetItem(args, 0, id);
    PyTuple_SetItem(args, 1, txt);
    ::PyObject *pRes = PyObject_CallObject(py_set_source_text, args);
    Py_DECREF(args);
    if (!pRes) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
}


void Python3LanguagePlugin::setHiddenTextBaseLine(int documentId, int baseLine)
{

}

QList<Error> Python3LanguagePlugin::errors(int documentId) const
{
    QList<Error> result;
    ::PyObject * py_errors = PyObject_GetAttrString(analizerModule_, "errors");
    ::PyObject * args = PyTuple_New(1);
    ::PyObject * id = PyLong_FromLong(documentId);
    PyTuple_SetItem(args, 0, id);
    ::PyObject *pRes = PyObject_CallObject(py_errors, args);
    Py_DECREF(args);
    if (!pRes) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return result;
    }
    int sz = PyList_Size(pRes);
    for (int i=0; i<sz; i++) {
        ::PyObject * item = PyList_GetItem(pRes, i);
        ::PyObject * lineNo = PyTuple_GetItem(item, 0);
        ::PyObject * offset = PyTuple_GetItem(item, 1);
        ::PyObject * length = PyTuple_GetItem(item, 2);
        ::PyObject * text = PyTuple_GetItem(item, 3);
        Error error;
        error.line = PyLong_AsLong(lineNo);
        error.start = PyLong_AsLong(offset);
        error.len = PyLong_AsLong(length);
        wchar_t * buffer = PyUnicode_AsWideCharString(text, 0);
        error.code = QString::fromWCharArray(buffer);
        if (error.code.length() > 0)
            error.code[0] = error.code[0].toUpper();
        PyMem_Free(buffer);
        result.push_back(error);
    }
    return result;
}

QList<LineProp> Python3LanguagePlugin::lineProperties(int documentId) const
{
    QList<LineProp> result;
    ::PyObject * py_line_prop = PyObject_GetAttrString(analizerModule_, "line_properties");
    ::PyObject * args = PyTuple_New(1);
    ::PyObject * id = PyLong_FromLong(documentId);
    PyTuple_SetItem(args, 0, id);
    ::PyObject *pRes = PyObject_CallObject(py_line_prop, args);
    Py_DECREF(args);
    if (!pRes) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return result;
    }
    int propsCount = PyList_Size(pRes);
    for (int i=0; i<propsCount; i++) {
        ::PyObject * item = PyList_GetItem(pRes, i);
        int propLength = PyList_Size(item);
        LineProp lp(propLength);
        for (int j=0; j<propLength; j++) {
            ::PyObject * propElem = PyList_GetItem(item, j);
            unsigned long val = PyLong_AsUnsignedLong(propElem);
            lp[j] = (Shared::LexemType)val;
        }
        result.push_back(lp);
    }
    return result;
}

QList<QPoint> Python3LanguagePlugin::lineRanks(int documentId) const
{
    return QList<QPoint>();
}

QStringList Python3LanguagePlugin::imports(int documentId) const
{
    return QStringList();
}

LineProp Python3LanguagePlugin::lineProp(int documentId, int lineNo, const QString &text) const
{
    LineProp lp(text.size());
    lp.fill(LxTypeEmpty);
    if (text.length()==0)
        return lp;
    ::PyObject * py_line_prop = PyObject_GetAttrString(analizerModule_, "line_prop");
    ::PyObject * args = PyTuple_New(3);
    std::wstring ws = text.toStdWString();
    ::PyObject * txt = PyUnicode_FromWideChar(ws.c_str(), ws.length());
    ::PyObject * id = PyLong_FromLong(documentId);
    ::PyObject * line_no = PyLong_FromLong(lineNo);
    PyTuple_SetItem(args, 0, id);
    PyTuple_SetItem(args, 1, line_no);
    PyTuple_SetItem(args, 2, txt);
    ::PyObject *pRes = PyObject_CallObject(py_line_prop, args);
    Py_DECREF(args);
    if (!pRes) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
        return lp;
    }
    int sz = PyList_Size(pRes);
    for (int i=0; i<sz; i++) {
        ::PyObject * item = PyList_GetItem(pRes, i);
        unsigned long val = PyLong_AsUnsignedLong(item);
        lp[i] = (Shared::LexemType)val;
    }
    return lp;
}

std::string Python3LanguagePlugin::rawSourceData(int documentId) const
{
    return "";
}

QString Python3LanguagePlugin::createImportStatementLine(const QString &importName) const
{
    return "import " + importName;
}

void Python3LanguagePlugin::dropDocument(int documentId)
{

}

bool Python3LanguagePlugin::loadProgram(const QString &fileName, const QByteArray & source, ProgramFormat )
{
    runWorker_->fileName = fileName;
    runWorker_->source = source;
    return true;
}

QDateTime Python3LanguagePlugin::loadedProgramVersion() const
{
    return QDateTime::currentDateTime();
}

void Python3LanguagePlugin::setSourceDirName(int documentId, const QString &dirPath)
{

}

bool Python3LanguagePlugin::canStepOut() const
{
    return false;
}

RunWorker::RunWorker(Python3LanguagePlugin *parent)
    : QThread(parent)
    , mode(RM_Continuous)
    , terminateFlag(false)
    , terminateMutex(new QMutex)
    , justStarted(true)
    , stepsCounted(0u)
    , stepsCounterMutex(new QMutex)
{

}

void RunWorker::setTerminate()
{
    QMutexLocker lock(terminateMutex);
    terminateFlag = true;
}

void RunWorker::clearTerminate()
{
    QMutexLocker lock(terminateMutex);
    terminateFlag = false;
}

bool RunWorker::isTerminate()
{
    QMutexLocker lock(terminateMutex);
    return terminateFlag;
}

void RunWorker::run()
{
    clearTerminate();
    stepsCounterMutex->lock();
    justStarted = true;
    stepsCounted = 1u;
    stepsCounterMutex->unlock();
    ::PyThreadState * savedPyThreadState = Py_NewInterpreter();
    static const QString pyLibPath = qApp->property("sharePath").toString()+"/python3language";
    static const QByteArray run_py_Path = pyLibPath.toLocal8Bit();
    char * run_py = (char*)calloc(run_py_Path.size(), sizeof(char));
    strcpy(run_py, run_py_Path.constData());
    ::PyObject * sysPath = PySys_GetObject("path");
    ::PyObject * kumirPath = PyUnicode_FromString(run_py);
    PyList_Insert(sysPath, 0, kumirPath);
    PyImport_AppendInittab("_kumir", &PyInit__kumir);
    ::PyObject * runModule = PyImport_ImportModule("run");
    if (PyErr_Occurred()) {
        PyErr_Print();
        return;
    }

    previousLineNo = -1;
    previousFrameLocals = previousFrameGlobals = NULL;
    currentFrameLocals = currentFrameGlobals = NULL;

    std::wstring ws_file_name = fileName.toStdWString();
    std::wstring ws_source = source.toStdWString();
    ::PyObject * py_file_name =
            PyUnicode_FromWideChar(ws_file_name.c_str(), ws_file_name.length());
    ::PyObject * py_source = PyUnicode_FromWideChar(ws_source.data(), ws_source.size());

    ::PyObject * extract_lvalue_atoms = PyObject_GetAttrString(runModule, "extract_lvalue_atoms");
    ::PyObject * args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, py_source);
    ::PyObject * py_lvalue_atoms = PyObject_CallObject(extract_lvalue_atoms, args);
//    Py_DECREF(py_file_name);
//    Py_DECREF(py_source);
//    Py_DECREF(args);
    if (PyErr_Occurred()) {
        PyErr_Print();
        return;
    }
    size_t lines_count = PyList_Size(py_lvalue_atoms);
    lvalueAtoms.clear();
    lvalueAtoms.resize(lines_count);
    for (size_t i=0; i<lines_count; i++) {
        ::PyObject * row = PyList_GetItem(py_lvalue_atoms, i);
        QStringList & qrow = lvalueAtoms[i];
        size_t atoms_count = PyList_Size(row);
        for (size_t j=0; j<atoms_count; j++) {
            ::PyObject * name = PyList_GetItem(row, j);
            wchar_t * buffer = PyUnicode_AsWideCharString(name, 0);
            qrow.append(QString::fromWCharArray(buffer));
            PyMem_Free(buffer);
//            Py_DECREF(name);
        }
//        Py_DECREF(row);
    }
//    Py_DECREF(py_lvalue_atoms);
    ::PyObject * main_loop = PyObject_GetAttrString(runModule, "main_loop");
    py_file_name = PyUnicode_FromWideChar(ws_file_name.c_str(), ws_file_name.length());
    py_source = PyUnicode_FromWideChar(ws_source.data(), ws_source.size());

    args = PyTuple_New(2);
    PyTuple_SetItem(args, 0, py_file_name);
    PyTuple_SetItem(args, 1, py_source);
    ::PyObject * py_result = PyObject_CallObject(main_loop, args);
//    Py_DECREF(args);
    if (!py_result) {
        ::PyObject * ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyObject_Print(pvalue, stderr,0);
        PyObject_Print(ptraceback, stderr,0);
    }
    int result = ERROR;
    if (PyLong_Check(py_result)) {
        result = PyLong_AsLong(py_result);
    }
    Py_EndInterpreter(savedPyThreadState);
    if (result == DONE) {
        PLUGIN->handlePythonExecutionFinished();
    }
    else if (result == TERMINATED) {
        PLUGIN->handlePythonExecutionTerminated();
    }
    else if (result == ERROR) {
        PLUGIN->handlePythonExecutionError();
    }
}

void Python3LanguagePlugin::runBlind()
{    
    runWorker_->mode = RM_Blind;
    runInteractionWaiter_->mode = RM_Blind;
    if (runInteractionWaiter_->isWaiting()) {
        // Unlock started execution
        runInteractionWaiter_->continueExecution();
    }
    else {
        saveAnalizer();
        runWorker_->start();
    }
}

void Python3LanguagePlugin::runContinuous()
{
    runWorker_->mode = RM_Continuous;
    runInteractionWaiter_->mode = RM_Continuous;
    if (runInteractionWaiter_->isWaiting()) {
        // Unlock started execution
        runInteractionWaiter_->continueExecution();
    }
    else {
        saveAnalizer();
        runWorker_->start();
    }
}

void Python3LanguagePlugin::runStepOver()
{
    runInteractionWaiter_->mode = RM_StepOver;
    runWorker_->mode = RM_StepOver;
    if (runInteractionWaiter_->isWaiting()) {
        // Unlock started execution
        runInteractionWaiter_->continueExecution();
    }
    else {
        saveAnalizer();
        runWorker_->start();
    }
}

void Python3LanguagePlugin::runStepInto()
{
    runStepOver();
}

void Python3LanguagePlugin::runStepOut()
{
    runStepOver();
}

void Python3LanguagePlugin::runTesting()
{
    Q_EMIT stopped(RunInterface::SR_Error);
}

bool Python3LanguagePlugin::isTestingRun() const
{
    return false;
}

RunInteractionWaiter::RunInteractionWaiter(Python3LanguagePlugin *parent)
    : QThread(parent)
    , mode(RM_Continuous)
    , mutex(new QMutex)
    , flag(false)
{

}

void RunInteractionWaiter::tryToPause()
{
    if (mode!=RM_Continuous && mode!=RM_Blind) {
        mutex->lock();
        flag = true;
        mutex->unlock();
        PLUGIN->handlePythonExecutionPaused();
        bool flag_value = true;
        while (flag_value) {
            mutex->lock();
            flag_value = flag;
            mutex->unlock();
            if (flag_value) {
                msleep(50);
            }
            else {
                break;
            }
        }
    }
    else {
        // Works really fast, so it requires sleep to make possible
        // to termitate program using GUI
        usleep(500);
        QCoreApplication::processEvents();
    }
}

void RunInteractionWaiter::continueExecution()
{
    mutex->lock();
    flag = false;
    mutex->unlock();
}


bool RunInteractionWaiter::isWaiting() const
{
    mutex->lock();
    bool value = flag;
    mutex->unlock();
    return value;
}

void Python3LanguagePlugin::terminate()
{
    runWorker_->setTerminate();
    runInteractionWaiter_->continueExecution();
}

bool Python3LanguagePlugin::hasMoreInstructions() const
{
    return runWorker_->isRunning();
}

int Python3LanguagePlugin::currentLineNo() const
{
    return runWorker_->currentLineNo;
}

QString Python3LanguagePlugin::error() const
{
    return runWorker_->error;
}

QVariant Python3LanguagePlugin::valueStackTopItem() const
{
    return QVariant();
}

QMap<QString,QVariant> Python3LanguagePlugin::getScalarLocalValues(int frameNo) const
{
    return QMap<QString,QVariant>();
}

QMap<QString,QVariant> Python3LanguagePlugin::getScalarGlobalValues(const QString & moduleName) const
{
    return QMap<QString,QVariant>();
}

QVariantList Python3LanguagePlugin::getLocalTableValues(int frameNo, int maxCount, const QString & name,
                                 const QList< QPair<int,int> > & ranges, bool & complete) const
{
    return QVariantList();
}

QVariantList Python3LanguagePlugin::getGlobalTableValues(const QString & moduleName, int maxCount, const QString & name,
                                  const QList< QPair<int,int> > & ranges, bool & complete) const
{
    return QVariantList();
}

QVariant Python3LanguagePlugin::getLocalTableValue(int frameNo, const QString & name, const QList<int> & indeces) const
{
    return QVariant();
}

QVariant Python3LanguagePlugin::getGlobalTableValue(const QString & moduleName, const QString & name, const QList<int> & indeces) const
{
    return QVariant();
}

void Python3LanguagePlugin::handlePythonOutput(const QString &message)
{
    Q_EMIT outputRequest(message);
}

void Python3LanguagePlugin::handlePythonError(const QString &message)
{
    Q_EMIT errorOutputRequest(message);
}

void Python3LanguagePlugin::handlePythonLineChanged(int lineNo)
{
    Q_EMIT lineChanged(lineNo, 0, 0);
}

void Python3LanguagePlugin::handlePythonExecutionPaused()
{
    Q_EMIT stopped(Shared::RunInterface::SR_UserInteraction);
}

void Python3LanguagePlugin::handlePythonExecutionFinished()
{
    Q_EMIT stopped(Shared::RunInterface::SR_Done);
}

void Python3LanguagePlugin::handlePythonExecutionTerminated()
{
    Q_EMIT stopped(Shared::RunInterface::SR_UserTerminated);
}

void Python3LanguagePlugin::handlePythonExecutionError()
{
    Q_EMIT stopped(Shared::RunInterface::SR_Error);
}

void Python3LanguagePlugin::handlePythonMarginText(int lineNo, const QString &text)
{
    Q_EMIT clearMargin(lineNo, -1);
    Q_EMIT marginText(lineNo, text);
}

void Python3LanguagePlugin::handlePythonStepsCounterChanged(unsigned long int value)
{
    Q_EMIT updateStepsCounter(value);
}

bool Python3LanguagePlugin::hasTestingEntryPoint() const
{
    return false;
}

unsigned long int Python3LanguagePlugin::stepsCounted() const
{
    QMutexLocker lock(WORKER->stepsCounterMutex);
    return WORKER->stepsCounted;
}

QAbstractItemModel * Python3LanguagePlugin::debuggerVariablesViewModel() const
{
    return 0;
}

} // namespace

Q_EXPORT_PLUGIN(Python3Language::Python3LanguagePlugin)
