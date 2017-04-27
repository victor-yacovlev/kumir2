/****************************************************************************
** Meta object code from reading C++ file 'pult.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/vodoley/pult.h"
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
struct qt_meta_stringdata_OvenTimer_t {
    QByteArrayData data[5];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OvenTimer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OvenTimer_t qt_meta_stringdata_OvenTimer = {
    {
QT_MOC_LITERAL(0, 0, 9), // "OvenTimer"
QT_MOC_LITERAL(1, 10, 9), // "angChange"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 5), // "value"
QT_MOC_LITERAL(4, 27, 8) // "setValue"

    },
    "OvenTimer\0angChange\0\0value\0setValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OvenTimer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   27,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void OvenTimer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OvenTimer *_t = static_cast<OvenTimer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->angChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (OvenTimer::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OvenTimer::angChange)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject OvenTimer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_OvenTimer.data,
      qt_meta_data_OvenTimer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *OvenTimer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OvenTimer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_OvenTimer.stringdata0))
        return static_cast<void*>(const_cast< OvenTimer*>(this));
    return QWidget::qt_metacast(_clname);
}

int OvenTimer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void OvenTimer::angChange(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_VodoleyPult_t {
    QByteArrayData data[42];
    char stringdata0[343];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VodoleyPult_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VodoleyPult_t qt_meta_stringdata_VodoleyPult = {
    {
QT_MOC_LITERAL(0, 0, 11), // "VodoleyPult"
QT_MOC_LITERAL(1, 12, 4), // "goUp"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 6), // "goDown"
QT_MOC_LITERAL(4, 25, 6), // "goLeft"
QT_MOC_LITERAL(5, 32, 7), // "goRight"
QT_MOC_LITERAL(6, 40, 11), // "hasLeftWall"
QT_MOC_LITERAL(7, 52, 12), // "hasRightWall"
QT_MOC_LITERAL(8, 65, 8), // "noUpWall"
QT_MOC_LITERAL(9, 74, 10), // "noDownWall"
QT_MOC_LITERAL(10, 85, 10), // "noLeftWall"
QT_MOC_LITERAL(11, 96, 11), // "noRightWall"
QT_MOC_LITERAL(12, 108, 5), // "Color"
QT_MOC_LITERAL(13, 114, 5), // "Clean"
QT_MOC_LITERAL(14, 120, 3), // "Rad"
QT_MOC_LITERAL(15, 124, 4), // "Temp"
QT_MOC_LITERAL(16, 129, 6), // "logToK"
QT_MOC_LITERAL(17, 136, 7), // "PultCmd"
QT_MOC_LITERAL(18, 144, 4), // "text"
QT_MOC_LITERAL(19, 149, 8), // "sendText"
QT_MOC_LITERAL(20, 158, 6), // "noLink"
QT_MOC_LITERAL(21, 165, 6), // "LinkOK"
QT_MOC_LITERAL(22, 172, 9), // "newClient"
QT_MOC_LITERAL(23, 182, 16), // "clientDisconnect"
QT_MOC_LITERAL(24, 199, 2), // "Up"
QT_MOC_LITERAL(25, 202, 5), // "AOutS"
QT_MOC_LITERAL(26, 208, 5), // "BOutS"
QT_MOC_LITERAL(27, 214, 5), // "COutS"
QT_MOC_LITERAL(28, 220, 5), // "TempS"
QT_MOC_LITERAL(29, 226, 5), // "AtoBS"
QT_MOC_LITERAL(30, 232, 5), // "AtoCS"
QT_MOC_LITERAL(31, 238, 5), // "CtoBS"
QT_MOC_LITERAL(32, 244, 5), // "CtoAS"
QT_MOC_LITERAL(33, 250, 12), // "resetVodoley"
QT_MOC_LITERAL(34, 263, 11), // "showMessage"
QT_MOC_LITERAL(35, 275, 7), // "message"
QT_MOC_LITERAL(36, 283, 10), // "logToKumir"
QT_MOC_LITERAL(37, 294, 6), // "FillAs"
QT_MOC_LITERAL(38, 301, 6), // "FillBs"
QT_MOC_LITERAL(39, 308, 6), // "FillCs"
QT_MOC_LITERAL(40, 315, 12), // "lockCButtons"
QT_MOC_LITERAL(41, 328, 14) // "UnLockCButtons"

    },
    "VodoleyPult\0goUp\0\0goDown\0goLeft\0goRight\0"
    "hasLeftWall\0hasRightWall\0noUpWall\0"
    "noDownWall\0noLeftWall\0noRightWall\0"
    "Color\0Clean\0Rad\0Temp\0logToK\0PultCmd\0"
    "text\0sendText\0noLink\0LinkOK\0newClient\0"
    "clientDisconnect\0Up\0AOutS\0BOutS\0COutS\0"
    "TempS\0AtoBS\0AtoCS\0CtoBS\0CtoAS\0"
    "resetVodoley\0showMessage\0message\0"
    "logToKumir\0FillAs\0FillBs\0FillCs\0"
    "lockCButtons\0UnLockCButtons"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VodoleyPult[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      38,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      17,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  204,    2, 0x06 /* Public */,
       3,    0,  205,    2, 0x06 /* Public */,
       4,    0,  206,    2, 0x06 /* Public */,
       5,    0,  207,    2, 0x06 /* Public */,
       6,    0,  208,    2, 0x06 /* Public */,
       7,    0,  209,    2, 0x06 /* Public */,
       8,    0,  210,    2, 0x06 /* Public */,
       9,    0,  211,    2, 0x06 /* Public */,
      10,    0,  212,    2, 0x06 /* Public */,
      11,    0,  213,    2, 0x06 /* Public */,
      12,    0,  214,    2, 0x06 /* Public */,
      13,    0,  215,    2, 0x06 /* Public */,
      14,    0,  216,    2, 0x06 /* Public */,
      15,    0,  217,    2, 0x06 /* Public */,
      16,    0,  218,    2, 0x06 /* Public */,
      17,    1,  219,    2, 0x06 /* Public */,
      19,    1,  222,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      20,    0,  225,    2, 0x0a /* Public */,
      21,    0,  226,    2, 0x0a /* Public */,
      22,    1,  227,    2, 0x0a /* Public */,
      23,    0,  230,    2, 0x0a /* Public */,
      24,    0,  231,    2, 0x0a /* Public */,
      25,    0,  232,    2, 0x0a /* Public */,
      26,    0,  233,    2, 0x0a /* Public */,
      27,    0,  234,    2, 0x0a /* Public */,
      28,    0,  235,    2, 0x0a /* Public */,
      29,    0,  236,    2, 0x0a /* Public */,
      30,    0,  237,    2, 0x0a /* Public */,
      31,    0,  238,    2, 0x0a /* Public */,
      32,    0,  239,    2, 0x0a /* Public */,
      33,    0,  240,    2, 0x0a /* Public */,
      34,    1,  241,    2, 0x0a /* Public */,
      36,    0,  244,    2, 0x0a /* Public */,
      37,    0,  245,    2, 0x0a /* Public */,
      38,    0,  246,    2, 0x0a /* Public */,
      39,    0,  247,    2, 0x0a /* Public */,
      40,    0,  248,    2, 0x0a /* Public */,
      41,    0,  249,    2, 0x0a /* Public */,

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
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void, QMetaType::QString,   18,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
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
    QMetaType::Void, QMetaType::QString,   35,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void VodoleyPult::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VodoleyPult *_t = static_cast<VodoleyPult *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->goUp(); break;
        case 1: _t->goDown(); break;
        case 2: _t->goLeft(); break;
        case 3: _t->goRight(); break;
        case 4: _t->hasLeftWall(); break;
        case 5: _t->hasRightWall(); break;
        case 6: _t->noUpWall(); break;
        case 7: _t->noDownWall(); break;
        case 8: _t->noLeftWall(); break;
        case 9: _t->noRightWall(); break;
        case 10: _t->Color(); break;
        case 11: _t->Clean(); break;
        case 12: _t->Rad(); break;
        case 13: _t->Temp(); break;
        case 14: _t->logToK(); break;
        case 15: _t->PultCmd((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: _t->sendText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 17: _t->noLink(); break;
        case 18: _t->LinkOK(); break;
        case 19: _t->newClient((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: _t->clientDisconnect(); break;
        case 21: _t->Up(); break;
        case 22: _t->AOutS(); break;
        case 23: _t->BOutS(); break;
        case 24: _t->COutS(); break;
        case 25: _t->TempS(); break;
        case 26: _t->AtoBS(); break;
        case 27: _t->AtoCS(); break;
        case 28: _t->CtoBS(); break;
        case 29: _t->CtoAS(); break;
        case 30: _t->resetVodoley(); break;
        case 31: _t->showMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 32: _t->logToKumir(); break;
        case 33: _t->FillAs(); break;
        case 34: _t->FillBs(); break;
        case 35: _t->FillCs(); break;
        case 36: _t->lockCButtons(); break;
        case 37: _t->UnLockCButtons(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::goUp)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::goDown)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::goLeft)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::goRight)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::hasLeftWall)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::hasRightWall)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::noUpWall)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::noDownWall)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::noLeftWall)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::noRightWall)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::Color)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::Clean)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::Rad)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::Temp)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::logToK)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::PultCmd)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (VodoleyPult::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VodoleyPult::sendText)) {
                *result = 16;
                return;
            }
        }
    }
}

const QMetaObject VodoleyPult::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_VodoleyPult.data,
      qt_meta_data_VodoleyPult,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *VodoleyPult::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VodoleyPult::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_VodoleyPult.stringdata0))
        return static_cast<void*>(const_cast< VodoleyPult*>(this));
    if (!strcmp(_clname, "Ui::VodoleyPult"))
        return static_cast< Ui::VodoleyPult*>(const_cast< VodoleyPult*>(this));
    return QWidget::qt_metacast(_clname);
}

int VodoleyPult::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 38)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 38;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 38)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 38;
    }
    return _id;
}

// SIGNAL 0
void VodoleyPult::goUp()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void VodoleyPult::goDown()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void VodoleyPult::goLeft()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void VodoleyPult::goRight()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void VodoleyPult::hasLeftWall()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void VodoleyPult::hasRightWall()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void VodoleyPult::noUpWall()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void VodoleyPult::noDownWall()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void VodoleyPult::noLeftWall()
{
    QMetaObject::activate(this, &staticMetaObject, 8, Q_NULLPTR);
}

// SIGNAL 9
void VodoleyPult::noRightWall()
{
    QMetaObject::activate(this, &staticMetaObject, 9, Q_NULLPTR);
}

// SIGNAL 10
void VodoleyPult::Color()
{
    QMetaObject::activate(this, &staticMetaObject, 10, Q_NULLPTR);
}

// SIGNAL 11
void VodoleyPult::Clean()
{
    QMetaObject::activate(this, &staticMetaObject, 11, Q_NULLPTR);
}

// SIGNAL 12
void VodoleyPult::Rad()
{
    QMetaObject::activate(this, &staticMetaObject, 12, Q_NULLPTR);
}

// SIGNAL 13
void VodoleyPult::Temp()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void VodoleyPult::logToK()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}

// SIGNAL 15
void VodoleyPult::PultCmd(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void VodoleyPult::sendText(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}
QT_END_MOC_NAMESPACE
