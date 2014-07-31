#ifndef PYUTILS_H
#define PYUTILS_H

#include <QtCore>
extern "C" {
#include <Python.h>
}

namespace Python3Language {

extern void printPythonTraceback();

extern QString PyUnicodeToQString(PyObject * unicode);
extern PyObject* QStringToPyUnicode(const QString & qstring);

extern QVariantList PyListToQVariantList(PyObject * list);
extern PyObject* QVariantListToPyList(const QVariantList & list,
                                        bool toTuple);

extern QMap<QString,QVariant> PyObjectToPropertyMap(PyObject * object);
extern QVariant PyObjectToQVariant(PyObject * object);
extern PyObject* QVariantToPyObject(const QVariant & value);

extern QVariant callPythonFunction(
        PyThreadState * interpreter,
        PyObject * callable,
        const QVariantList & args
        );


extern QVariant callPythonFunction(PyThreadState * interpreter, PyObject * callable);

extern QVariant callPythonFunction(
        PyThreadState * interpreter,
        PyObject * callable,
        const QVariant & arg1
        );

extern QVariant callPythonFunction(
        PyThreadState * interpreter,
        PyObject * callable,
        const QVariant & arg1,
        const QVariant & arg2
        );

extern PyObject* compileModule(
        const QString &fileName,
        const QString &source,
        int * errorLineNumber = 0,
        QString * errorText = 0
        );

extern PyObject* createModuleFromSource(
        PyThreadState * interpreter,
        const QString & moduleName,
        const QString & moduleSource
        );

extern void appendToSysPath(const QString & path);

}

#endif // PYUTILS_H
