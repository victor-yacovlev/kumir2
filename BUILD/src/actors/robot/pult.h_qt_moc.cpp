/****************************************************************************
** Meta object code from reading C++ file 'pult.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/robot/pult.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pult.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RoboPult_t {
    QByteArrayData data[35];
    char stringdata0[274];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RoboPult_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RoboPult_t qt_meta_stringdata_RoboPult = {
    {
QT_MOC_LITERAL(0, 0, 8), // "RoboPult"
QT_MOC_LITERAL(1, 9, 4), // "goUp"
QT_MOC_LITERAL(2, 14, 0), // ""
QT_MOC_LITERAL(3, 15, 8), // "robReset"
QT_MOC_LITERAL(4, 24, 6), // "goDown"
QT_MOC_LITERAL(5, 31, 6), // "goLeft"
QT_MOC_LITERAL(6, 38, 7), // "goRight"
QT_MOC_LITERAL(7, 46, 9), // "hasUpWall"
QT_MOC_LITERAL(8, 56, 11), // "hasDownWall"
QT_MOC_LITERAL(9, 68, 11), // "hasLeftWall"
QT_MOC_LITERAL(10, 80, 12), // "hasRightWall"
QT_MOC_LITERAL(11, 93, 8), // "noUpWall"
QT_MOC_LITERAL(12, 102, 10), // "noDownWall"
QT_MOC_LITERAL(13, 113, 10), // "noLeftWall"
QT_MOC_LITERAL(14, 124, 11), // "noRightWall"
QT_MOC_LITERAL(15, 136, 5), // "Color"
QT_MOC_LITERAL(16, 142, 5), // "Clean"
QT_MOC_LITERAL(17, 148, 7), // "Colored"
QT_MOC_LITERAL(18, 156, 3), // "Rad"
QT_MOC_LITERAL(19, 160, 4), // "Temp"
QT_MOC_LITERAL(20, 165, 7), // "PultCmd"
QT_MOC_LITERAL(21, 173, 4), // "text"
QT_MOC_LITERAL(22, 178, 6), // "noLink"
QT_MOC_LITERAL(23, 185, 6), // "LinkOK"
QT_MOC_LITERAL(24, 192, 2), // "Up"
QT_MOC_LITERAL(25, 195, 4), // "Down"
QT_MOC_LITERAL(26, 200, 4), // "Left"
QT_MOC_LITERAL(27, 205, 5), // "Right"
QT_MOC_LITERAL(28, 211, 7), // "SwStena"
QT_MOC_LITERAL(29, 219, 10), // "SwSvobodno"
QT_MOC_LITERAL(30, 230, 5), // "TempS"
QT_MOC_LITERAL(31, 236, 4), // "RadS"
QT_MOC_LITERAL(32, 241, 10), // "switchButt"
QT_MOC_LITERAL(33, 252, 12), // "CenterButton"
QT_MOC_LITERAL(34, 265, 8) // "clearLog"

    },
    "RoboPult\0goUp\0\0robReset\0goDown\0goLeft\0"
    "goRight\0hasUpWall\0hasDownWall\0hasLeftWall\0"
    "hasRightWall\0noUpWall\0noDownWall\0"
    "noLeftWall\0noRightWall\0Color\0Clean\0"
    "Colored\0Rad\0Temp\0PultCmd\0text\0noLink\0"
    "LinkOK\0Up\0Down\0Left\0Right\0SwStena\0"
    "SwSvobodno\0TempS\0RadS\0switchButt\0"
    "CenterButton\0clearLog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RoboPult[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      19,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  174,    2, 0x06 /* Public */,
       3,    0,  175,    2, 0x06 /* Public */,
       4,    0,  176,    2, 0x06 /* Public */,
       5,    0,  177,    2, 0x06 /* Public */,
       6,    0,  178,    2, 0x06 /* Public */,
       7,    0,  179,    2, 0x06 /* Public */,
       8,    0,  180,    2, 0x06 /* Public */,
       9,    0,  181,    2, 0x06 /* Public */,
      10,    0,  182,    2, 0x06 /* Public */,
      11,    0,  183,    2, 0x06 /* Public */,
      12,    0,  184,    2, 0x06 /* Public */,
      13,    0,  185,    2, 0x06 /* Public */,
      14,    0,  186,    2, 0x06 /* Public */,
      15,    0,  187,    2, 0x06 /* Public */,
      16,    0,  188,    2, 0x06 /* Public */,
      17,    0,  189,    2, 0x06 /* Public */,
      18,    0,  190,    2, 0x06 /* Public */,
      19,    0,  191,    2, 0x06 /* Public */,
      20,    1,  192,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      22,    0,  195,    2, 0x0a /* Public */,
      23,    0,  196,    2, 0x0a /* Public */,
      24,    0,  197,    2, 0x0a /* Public */,
      25,    0,  198,    2, 0x0a /* Public */,
      26,    0,  199,    2, 0x0a /* Public */,
      27,    0,  200,    2, 0x0a /* Public */,
      28,    0,  201,    2, 0x0a /* Public */,
      29,    0,  202,    2, 0x0a /* Public */,
      30,    0,  203,    2, 0x0a /* Public */,
      31,    0,  204,    2, 0x0a /* Public */,
      32,    0,  205,    2, 0x0a /* Public */,
      33,    0,  206,    2, 0x0a /* Public */,
      34,    0,  207,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   21,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RoboPult::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RoboPult *_t = static_cast<RoboPult *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->goUp(); break;
        case 1: _t->robReset(); break;
        case 2: _t->goDown(); break;
        case 3: _t->goLeft(); break;
        case 4: _t->goRight(); break;
        case 5: _t->hasUpWall(); break;
        case 6: _t->hasDownWall(); break;
        case 7: _t->hasLeftWall(); break;
        case 8: _t->hasRightWall(); break;
        case 9: _t->noUpWall(); break;
        case 10: _t->noDownWall(); break;
        case 11: _t->noLeftWall(); break;
        case 12: _t->noRightWall(); break;
        case 13: _t->Color(); break;
        case 14: _t->Clean(); break;
        case 15: _t->Colored(); break;
        case 16: _t->Rad(); break;
        case 17: _t->Temp(); break;
        case 18: _t->PultCmd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 19: _t->noLink(); break;
        case 20: _t->LinkOK(); break;
        case 21: _t->Up(); break;
        case 22: _t->Down(); break;
        case 23: _t->Left(); break;
        case 24: _t->Right(); break;
        case 25: _t->SwStena(); break;
        case 26: _t->SwSvobodno(); break;
        case 27: _t->TempS(); break;
        case 28: _t->RadS(); break;
        case 29: _t->switchButt(); break;
        case 30: _t->CenterButton(); break;
        case 31: _t->clearLog(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::goUp)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::robReset)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::goDown)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::goLeft)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::goRight)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::hasUpWall)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::hasDownWall)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::hasLeftWall)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::hasRightWall)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::noUpWall)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::noDownWall)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::noLeftWall)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::noRightWall)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::Color)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::Clean)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::Colored)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::Rad)) {
                *result = 16;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::Temp)) {
                *result = 17;
                return;
            }
        }
        {
            typedef void (RoboPult::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboPult::PultCmd)) {
                *result = 18;
                return;
            }
        }
    }
}

const QMetaObject RoboPult::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RoboPult.data,
      qt_meta_data_RoboPult,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RoboPult::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RoboPult::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RoboPult.stringdata0))
        return static_cast<void*>(const_cast< RoboPult*>(this));
    if (!strcmp(_clname, "Ui::RoboPult"))
        return static_cast< Ui::RoboPult*>(const_cast< RoboPult*>(this));
    return QWidget::qt_metacast(_clname);
}

int RoboPult::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 32)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 32;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 32)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 32;
    }
    return _id;
}

// SIGNAL 0
void RoboPult::goUp()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void RoboPult::robReset()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void RoboPult::goDown()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void RoboPult::goLeft()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void RoboPult::goRight()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void RoboPult::hasUpWall()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void RoboPult::hasDownWall()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void RoboPult::hasLeftWall()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void RoboPult::hasRightWall()
{
    QMetaObject::activate(this, &staticMetaObject, 8, Q_NULLPTR);
}

// SIGNAL 9
void RoboPult::noUpWall()
{
    QMetaObject::activate(this, &staticMetaObject, 9, Q_NULLPTR);
}

// SIGNAL 10
void RoboPult::noDownWall()
{
    QMetaObject::activate(this, &staticMetaObject, 10, Q_NULLPTR);
}

// SIGNAL 11
void RoboPult::noLeftWall()
{
    QMetaObject::activate(this, &staticMetaObject, 11, Q_NULLPTR);
}

// SIGNAL 12
void RoboPult::noRightWall()
{
    QMetaObject::activate(this, &staticMetaObject, 12, Q_NULLPTR);
}

// SIGNAL 13
void RoboPult::Color()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void RoboPult::Clean()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}

// SIGNAL 15
void RoboPult::Colored()
{
    QMetaObject::activate(this, &staticMetaObject, 15, Q_NULLPTR);
}

// SIGNAL 16
void RoboPult::Rad()
{
    QMetaObject::activate(this, &staticMetaObject, 16, Q_NULLPTR);
}

// SIGNAL 17
void RoboPult::Temp()
{
    QMetaObject::activate(this, &staticMetaObject, 17, Q_NULLPTR);
}

// SIGNAL 18
void RoboPult::PultCmd(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}
QT_END_MOC_NAMESPACE
