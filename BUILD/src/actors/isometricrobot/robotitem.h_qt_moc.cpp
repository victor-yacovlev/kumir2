/****************************************************************************
** Meta object code from reading C++ file 'robotitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/isometricrobot/robotitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'robotitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Robot25D__RobotItem_t {
    QByteArrayData data[23];
    char stringdata0[298];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Robot25D__RobotItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Robot25D__RobotItem_t qt_meta_stringdata_Robot25D__RobotItem = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Robot25D::RobotItem"
QT_MOC_LITERAL(1, 20, 18), // "evaluationFinished"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 21), // "startAnimationRequest"
QT_MOC_LITERAL(4, 62, 5), // "reset"
QT_MOC_LITERAL(5, 68, 8), // "setSpeed"
QT_MOC_LITERAL(6, 77, 4), // "msec"
QT_MOC_LITERAL(7, 82, 11), // "setPosition"
QT_MOC_LITERAL(8, 94, 8), // "Point3Dr"
QT_MOC_LITERAL(9, 103, 5), // "point"
QT_MOC_LITERAL(10, 109, 10), // "setFrameNo"
QT_MOC_LITERAL(11, 120, 2), // "no"
QT_MOC_LITERAL(12, 123, 11), // "setAnimated"
QT_MOC_LITERAL(13, 135, 1), // "v"
QT_MOC_LITERAL(14, 137, 8), // "setPulse"
QT_MOC_LITERAL(15, 146, 23), // "handleAnimationFinished"
QT_MOC_LITERAL(16, 170, 21), // "handleModelRobotMoved"
QT_MOC_LITERAL(17, 192, 23), // "handleModelRobotCrashed"
QT_MOC_LITERAL(18, 216, 26), // "handleModelRobotTurnedLeft"
QT_MOC_LITERAL(19, 243, 27), // "handleModelRobotTurnedRight"
QT_MOC_LITERAL(20, 271, 22), // "handleModelCellPainted"
QT_MOC_LITERAL(21, 294, 1), // "x"
QT_MOC_LITERAL(22, 296, 1) // "y"

    },
    "Robot25D::RobotItem\0evaluationFinished\0"
    "\0startAnimationRequest\0reset\0setSpeed\0"
    "msec\0setPosition\0Point3Dr\0point\0"
    "setFrameNo\0no\0setAnimated\0v\0setPulse\0"
    "handleAnimationFinished\0handleModelRobotMoved\0"
    "handleModelRobotCrashed\0"
    "handleModelRobotTurnedLeft\0"
    "handleModelRobotTurnedRight\0"
    "handleModelCellPainted\0x\0y"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Robot25D__RobotItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,
       3,    0,   85,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   86,    2, 0x0a /* Public */,
       5,    1,   87,    2, 0x0a /* Public */,
       7,    1,   90,    2, 0x0a /* Public */,
      10,    1,   93,    2, 0x0a /* Public */,
      12,    1,   96,    2, 0x0a /* Public */,
      14,    1,   99,    2, 0x0a /* Public */,
      15,    0,  102,    2, 0x09 /* Protected */,
      16,    0,  103,    2, 0x09 /* Protected */,
      17,    0,  104,    2, 0x09 /* Protected */,
      18,    0,  105,    2, 0x09 /* Protected */,
      19,    0,  106,    2, 0x09 /* Protected */,
      20,    2,  107,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Short,   11,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::QReal,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   21,   22,

       0        // eod
};

void Robot25D::RobotItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RobotItem *_t = static_cast<RobotItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->evaluationFinished(); break;
        case 1: _t->startAnimationRequest(); break;
        case 2: _t->reset(); break;
        case 3: _t->setSpeed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setPosition((*reinterpret_cast< const Point3Dr(*)>(_a[1]))); break;
        case 5: _t->setFrameNo((*reinterpret_cast< qint16(*)>(_a[1]))); break;
        case 6: _t->setAnimated((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->setPulse((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 8: _t->handleAnimationFinished(); break;
        case 9: _t->handleModelRobotMoved(); break;
        case 10: _t->handleModelRobotCrashed(); break;
        case 11: _t->handleModelRobotTurnedLeft(); break;
        case 12: _t->handleModelRobotTurnedRight(); break;
        case 13: _t->handleModelCellPainted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RobotItem::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotItem::evaluationFinished)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RobotItem::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotItem::startAnimationRequest)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject Robot25D::RobotItem::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Robot25D__RobotItem.data,
      qt_meta_data_Robot25D__RobotItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Robot25D::RobotItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Robot25D::RobotItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Robot25D__RobotItem.stringdata0))
        return static_cast<void*>(const_cast< RobotItem*>(this));
    return QThread::qt_metacast(_clname);
}

int Robot25D::RobotItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Robot25D::RobotItem::evaluationFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Robot25D::RobotItem::startAnimationRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
