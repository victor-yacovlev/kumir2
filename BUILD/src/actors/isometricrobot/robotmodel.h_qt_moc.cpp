/****************************************************************************
** Meta object code from reading C++ file 'robotmodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/isometricrobot/robotmodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'robotmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Robot25D__RobotModel_t {
    QByteArrayData data[10];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Robot25D__RobotModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Robot25D__RobotModel_t qt_meta_stringdata_Robot25D__RobotModel = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Robot25D::RobotModel"
QT_MOC_LITERAL(1, 21, 12), // "robotCrashed"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 10), // "robotMoved"
QT_MOC_LITERAL(4, 46, 15), // "robotTurnedLeft"
QT_MOC_LITERAL(5, 62, 16), // "robotTurnedRight"
QT_MOC_LITERAL(6, 79, 11), // "cellPainted"
QT_MOC_LITERAL(7, 91, 1), // "x"
QT_MOC_LITERAL(8, 93, 1), // "y"
QT_MOC_LITERAL(9, 95, 12) // "fieldChanged"

    },
    "Robot25D::RobotModel\0robotCrashed\0\0"
    "robotMoved\0robotTurnedLeft\0robotTurnedRight\0"
    "cellPainted\0x\0y\0fieldChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Robot25D__RobotModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,
       4,    0,   46,    2, 0x06 /* Public */,
       5,    0,   47,    2, 0x06 /* Public */,
       6,    2,   48,    2, 0x06 /* Public */,
       9,    0,   53,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    7,    8,
    QMetaType::Void,

       0        // eod
};

void Robot25D::RobotModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RobotModel *_t = static_cast<RobotModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->robotCrashed(); break;
        case 1: _t->robotMoved(); break;
        case 2: _t->robotTurnedLeft(); break;
        case 3: _t->robotTurnedRight(); break;
        case 4: _t->cellPainted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->fieldChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RobotModel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotModel::robotCrashed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RobotModel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotModel::robotMoved)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RobotModel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotModel::robotTurnedLeft)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (RobotModel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotModel::robotTurnedRight)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (RobotModel::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotModel::cellPainted)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (RobotModel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotModel::fieldChanged)) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject Robot25D::RobotModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Robot25D__RobotModel.data,
      qt_meta_data_Robot25D__RobotModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Robot25D::RobotModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Robot25D::RobotModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Robot25D__RobotModel.stringdata0))
        return static_cast<void*>(const_cast< RobotModel*>(this));
    return QObject::qt_metacast(_clname);
}

int Robot25D::RobotModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Robot25D::RobotModel::robotCrashed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Robot25D::RobotModel::robotMoved()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Robot25D::RobotModel::robotTurnedLeft()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void Robot25D::RobotModel::robotTurnedRight()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void Robot25D::RobotModel::cellPainted(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Robot25D::RobotModel::fieldChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
