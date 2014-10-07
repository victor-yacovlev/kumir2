#ifndef PYUTILS_H
#define PYUTILS_H

#include <QtCore>
extern "C" {
#include <Python.h>
}

namespace Python3Language {

extern void printPythonTraceback();
extern void printError(const QString & message);


extern QString PyUnicodeToQString(PyObject * unicode);
extern QString PyObjectToQString(PyObject * o);
extern PyObject* QStringToPyUnicode(const QString & qstring);

extern QVariantList PyListToQVariantList(PyObject * list);
extern PyObject* QVariantListToPyList(const QVariantList & list,
                                        bool toTuple);

extern QMap<QString,QVariant> PyDictToPropertyMap(PyObject * object);
extern QMap<QString,QVariant> PyObjectToPropertyMap(PyObject * object);

struct ValueRepresentation {
    QString name;
    QString repr;
    QList<ValueRepresentation> children;
};

extern ValueRepresentation PyObjectToValueRepresentation(const QString & name, PyObject * object);

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

extern void clearCreatedModules();

extern PyObject* findCreatedModule(const QString & name);

extern void appendToSysPath(const QString & path);

#ifdef Q_OS_WIN32
extern void prepareBundledSysPath();
#endif

}

Q_DECLARE_METATYPE(Python3Language::ValueRepresentation)

#endif // PYUTILS_H
