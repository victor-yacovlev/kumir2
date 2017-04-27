/****************************************************************************
** Meta object code from reading C++ file 'pult.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/turtle/pult.h"
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
struct qt_meta_stringdata_TurtlePult_t {
    QByteArrayData data[37];
    char stringdata0[295];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TurtlePult_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TurtlePult_t qt_meta_stringdata_TurtlePult = {
    {
QT_MOC_LITERAL(0, 0, 10), // "TurtlePult"
QT_MOC_LITERAL(1, 11, 4), // "goUp"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 4), // "step"
QT_MOC_LITERAL(4, 22, 6), // "goDown"
QT_MOC_LITERAL(5, 29, 6), // "goLeft"
QT_MOC_LITERAL(6, 36, 7), // "goRight"
QT_MOC_LITERAL(7, 44, 11), // "hasLeftWall"
QT_MOC_LITERAL(8, 56, 12), // "hasRightWall"
QT_MOC_LITERAL(9, 69, 8), // "noUpWall"
QT_MOC_LITERAL(10, 78, 10), // "noDownWall"
QT_MOC_LITERAL(11, 89, 10), // "noLeftWall"
QT_MOC_LITERAL(12, 100, 11), // "noRightWall"
QT_MOC_LITERAL(13, 112, 5), // "Color"
QT_MOC_LITERAL(14, 118, 5), // "Clean"
QT_MOC_LITERAL(15, 124, 3), // "Rad"
QT_MOC_LITERAL(16, 128, 4), // "Temp"
QT_MOC_LITERAL(17, 133, 6), // "logToK"
QT_MOC_LITERAL(18, 140, 7), // "PultCmd"
QT_MOC_LITERAL(19, 148, 4), // "text"
QT_MOC_LITERAL(20, 153, 8), // "sendText"
QT_MOC_LITERAL(21, 162, 6), // "noLink"
QT_MOC_LITERAL(22, 169, 6), // "LinkOK"
QT_MOC_LITERAL(23, 176, 9), // "newClient"
QT_MOC_LITERAL(24, 186, 16), // "clientDisconnect"
QT_MOC_LITERAL(25, 203, 2), // "Up"
QT_MOC_LITERAL(26, 206, 4), // "Down"
QT_MOC_LITERAL(27, 211, 4), // "Left"
QT_MOC_LITERAL(28, 216, 5), // "Right"
QT_MOC_LITERAL(29, 222, 7), // "SwStena"
QT_MOC_LITERAL(30, 230, 10), // "SwSvobodno"
QT_MOC_LITERAL(31, 241, 5), // "TempS"
QT_MOC_LITERAL(32, 247, 4), // "RadS"
QT_MOC_LITERAL(33, 252, 11), // "resetTurtle"
QT_MOC_LITERAL(34, 264, 11), // "showMessage"
QT_MOC_LITERAL(35, 276, 7), // "message"
QT_MOC_LITERAL(36, 284, 10) // "logToKumir"

    },
    "TurtlePult\0goUp\0\0step\0goDown\0goLeft\0"
    "goRight\0hasLeftWall\0hasRightWall\0"
    "noUpWall\0noDownWall\0noLeftWall\0"
    "noRightWall\0Color\0Clean\0Rad\0Temp\0"
    "logToK\0PultCmd\0text\0sendText\0noLink\0"
    "LinkOK\0newClient\0clientDisconnect\0Up\0"
    "Down\0Left\0Right\0SwStena\0SwSvobodno\0"
    "TempS\0RadS\0resetTurtle\0showMessage\0"
    "message\0logToKumir"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TurtlePult[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      17,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  174,    2, 0x06 /* Public */,
       4,    0,  177,    2, 0x06 /* Public */,
       5,    0,  178,    2, 0x06 /* Public */,
       6,    0,  179,    2, 0x06 /* Public */,
       7,    0,  180,    2, 0x06 /* Public */,
       8,    0,  181,    2, 0x06 /* Public */,
       9,    0,  182,    2, 0x06 /* Public */,
      10,    0,  183,    2, 0x06 /* Public */,
      11,    0,  184,    2, 0x06 /* Public */,
      12,    0,  185,    2, 0x06 /* Public */,
      13,    0,  186,    2, 0x06 /* Public */,
      14,    0,  187,    2, 0x06 /* Public */,
      15,    0,  188,    2, 0x06 /* Public */,
      16,    0,  189,    2, 0x06 /* Public */,
      17,    0,  190,    2, 0x06 /* Public */,
      18,    1,  191,    2, 0x06 /* Public */,
      20,    1,  194,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      21,    0,  197,    2, 0x0a /* Public */,
      22,    0,  198,    2, 0x0a /* Public */,
      23,    1,  199,    2, 0x0a /* Public */,
      24,    0,  202,    2, 0x0a /* Public */,
      25,    0,  203,    2, 0x0a /* Public */,
      26,    0,  204,    2, 0x0a /* Public */,
      27,    0,  205,    2, 0x0a /* Public */,
      28,    0,  206,    2, 0x0a /* Public */,
      29,    0,  207,    2, 0x0a /* Public */,
      30,    0,  208,    2, 0x0a /* Public */,
      31,    0,  209,    2, 0x0a /* Public */,
      32,    0,  210,    2, 0x0a /* Public */,
      33,    0,  211,    2, 0x0a /* Public */,
      34,    1,  212,    2, 0x0a /* Public */,
      36,    0,  215,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QReal,    3,
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
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::QString,    2,

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
    QMetaType::Void, QMetaType::QString,   35,
    QMetaType::Void,

       0        // eod
};

void TurtlePult::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TurtlePult *_t = static_cast<TurtlePult *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->goUp((*reinterpret_cast< qreal(*)>(_a[1]))); break;
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
        case 22: _t->Down(); break;
        case 23: _t->Left(); break;
        case 24: _t->Right(); break;
        case 25: _t->SwStena(); break;
        case 26: _t->SwSvobodno(); break;
        case 27: _t->TempS(); break;
        case 28: _t->RadS(); break;
        case 29: _t->resetTurtle(); break;
        case 30: _t->showMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 31: _t->logToKumir(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TurtlePult::*_t)(qreal );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::goUp)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::goDown)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::goLeft)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::goRight)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::hasLeftWall)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::hasRightWall)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::noUpWall)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::noDownWall)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::noLeftWall)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::noRightWall)) {
                *result = 9;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::Color)) {
                *result = 10;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::Clean)) {
                *result = 11;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::Rad)) {
                *result = 12;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::Temp)) {
                *result = 13;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::logToK)) {
                *result = 14;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::PultCmd)) {
                *result = 15;
                return;
            }
        }
        {
            typedef void (TurtlePult::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TurtlePult::sendText)) {
                *result = 16;
                return;
            }
        }
    }
}

const QMetaObject TurtlePult::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TurtlePult.data,
      qt_meta_data_TurtlePult,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TurtlePult::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TurtlePult::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TurtlePult.stringdata0))
        return static_cast<void*>(const_cast< TurtlePult*>(this));
    if (!strcmp(_clname, "Ui::TurtlePult"))
        return static_cast< Ui::TurtlePult*>(const_cast< TurtlePult*>(this));
    return QWidget::qt_metacast(_clname);
}

int TurtlePult::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void TurtlePult::goUp(qreal _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TurtlePult::goDown()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void TurtlePult::goLeft()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void TurtlePult::goRight()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void TurtlePult::hasLeftWall()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void TurtlePult::hasRightWall()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void TurtlePult::noUpWall()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void TurtlePult::noDownWall()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void TurtlePult::noLeftWall()
{
    QMetaObject::activate(this, &staticMetaObject, 8, Q_NULLPTR);
}

// SIGNAL 9
void TurtlePult::noRightWall()
{
    QMetaObject::activate(this, &staticMetaObject, 9, Q_NULLPTR);
}

// SIGNAL 10
void TurtlePult::Color()
{
    QMetaObject::activate(this, &staticMetaObject, 10, Q_NULLPTR);
}

// SIGNAL 11
void TurtlePult::Clean()
{
    QMetaObject::activate(this, &staticMetaObject, 11, Q_NULLPTR);
}

// SIGNAL 12
void TurtlePult::Rad()
{
    QMetaObject::activate(this, &staticMetaObject, 12, Q_NULLPTR);
}

// SIGNAL 13
void TurtlePult::Temp()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void TurtlePult::logToK()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}

// SIGNAL 15
void TurtlePult::PultCmd(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void TurtlePult::sendText(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}
QT_END_MOC_NAMESPACE
