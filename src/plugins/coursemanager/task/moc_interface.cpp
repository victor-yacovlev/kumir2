/****************************************************************************
** Meta object code from reading C++ file 'interface.h'
**
** Created: Fri May 11 09:53:56 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "interface.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'interface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ControlInterface[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_ControlInterface[] = {
    "ControlInterface\0"
};

const QMetaObject ControlInterface::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ControlInterface,
      qt_meta_data_ControlInterface, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ControlInterface::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ControlInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ControlInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ControlInterface))
        return static_cast<void*>(const_cast< ControlInterface*>(this));
    if (!strcmp(_clname, "taskControlInterface"))
        return static_cast< taskControlInterface*>(const_cast< ControlInterface*>(this));
    if (!strcmp(_clname, "kumir.taskControlInterface/1.7.1"))
        return static_cast< taskControlInterface*>(const_cast< ControlInterface*>(this));
    return QObject::qt_metacast(_clname);
}

int ControlInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
