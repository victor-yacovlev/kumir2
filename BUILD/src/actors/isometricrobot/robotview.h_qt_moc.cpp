/****************************************************************************
** Meta object code from reading C++ file 'robotview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/isometricrobot/robotview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'robotview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Robot25D__RobotView_t {
    QByteArrayData data[11];
    char stringdata0[129];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Robot25D__RobotView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Robot25D__RobotView_t qt_meta_stringdata_Robot25D__RobotView = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Robot25D::RobotView"
QT_MOC_LITERAL(1, 20, 4), // "sync"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 13), // "setRobotSpeed"
QT_MOC_LITERAL(4, 40, 4), // "msec"
QT_MOC_LITERAL(5, 45, 23), // "handleModelFieldChanged"
QT_MOC_LITERAL(6, 69, 28), // "handleRobotEvaluationFinised"
QT_MOC_LITERAL(7, 98, 11), // "setAnimated"
QT_MOC_LITERAL(8, 110, 1), // "v"
QT_MOC_LITERAL(9, 112, 10), // "isAnimated"
QT_MOC_LITERAL(10, 123, 5) // "reset"

    },
    "Robot25D::RobotView\0sync\0\0setRobotSpeed\0"
    "msec\0handleModelFieldChanged\0"
    "handleRobotEvaluationFinised\0setAnimated\0"
    "v\0isAnimated\0reset"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Robot25D__RobotView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   50,    2, 0x0a /* Public */,
       5,    0,   53,    2, 0x09 /* Protected */,
       6,    0,   54,    2, 0x09 /* Protected */,
       7,    1,   55,    2, 0x0a /* Public */,
       9,    0,   58,    2, 0x0a /* Public */,
      10,    0,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Bool,
    QMetaType::Void,

       0        // eod
};

void Robot25D::RobotView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RobotView *_t = static_cast<RobotView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sync(); break;
        case 1: _t->setRobotSpeed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->handleModelFieldChanged(); break;
        case 3: _t->handleRobotEvaluationFinised(); break;
        case 4: _t->setAnimated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: { bool _r = _t->isAnimated();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: _t->reset(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RobotView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotView::sync)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Robot25D::RobotView::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Robot25D__RobotView.data,
      qt_meta_data_Robot25D__RobotView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Robot25D::RobotView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Robot25D::RobotView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Robot25D__RobotView.stringdata0))
        return static_cast<void*>(const_cast< RobotView*>(this));
    if (!strcmp(_clname, "QGraphicsRectItem"))
        return static_cast< QGraphicsRectItem*>(const_cast< RobotView*>(this));
    return QObject::qt_metacast(_clname);
}

int Robot25D::RobotView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Robot25D::RobotView::sync()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
