/****************************************************************************
** Meta object code from reading C++ file 'remotecontrol.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/isometricrobot/remotecontrol.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'remotecontrol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorIsometricRobot__SvgRemoteControl_t {
    QByteArrayData data[7];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorIsometricRobot__SvgRemoteControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorIsometricRobot__SvgRemoteControl_t qt_meta_stringdata_ActorIsometricRobot__SvgRemoteControl = {
    {
QT_MOC_LITERAL(0, 0, 37), // "ActorIsometricRobot::SvgRemot..."
QT_MOC_LITERAL(1, 38, 13), // "buttonPressed"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 5), // "svgId"
QT_MOC_LITERAL(4, 59, 14), // "setLinkEnabled"
QT_MOC_LITERAL(5, 74, 2), // "on"
QT_MOC_LITERAL(6, 77, 22) // "handleSvgButtonPressed"

    },
    "ActorIsometricRobot::SvgRemoteControl\0"
    "buttonPressed\0\0svgId\0setLinkEnabled\0"
    "on\0handleSvgButtonPressed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorIsometricRobot__SvgRemoteControl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   32,    2, 0x0a /* Public */,
       6,    1,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void ActorIsometricRobot::SvgRemoteControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SvgRemoteControl *_t = static_cast<SvgRemoteControl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->buttonPressed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->setLinkEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->handleSvgButtonPressed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SvgRemoteControl::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SvgRemoteControl::buttonPressed)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ActorIsometricRobot::SvgRemoteControl::staticMetaObject = {
    { &QSvgWidget::staticMetaObject, qt_meta_stringdata_ActorIsometricRobot__SvgRemoteControl.data,
      qt_meta_data_ActorIsometricRobot__SvgRemoteControl,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorIsometricRobot::SvgRemoteControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorIsometricRobot::SvgRemoteControl::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorIsometricRobot__SvgRemoteControl.stringdata0))
        return static_cast<void*>(const_cast< SvgRemoteControl*>(this));
    return QSvgWidget::qt_metacast(_clname);
}

int ActorIsometricRobot::SvgRemoteControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSvgWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ActorIsometricRobot::SvgRemoteControl::buttonPressed(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
