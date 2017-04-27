/****************************************************************************
** Meta object code from reading C++ file 'terminal_plane.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/coregui/terminal_plane.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'terminal_plane.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Terminal__Plane_t {
    QByteArrayData data[16];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Terminal__Plane_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Terminal__Plane_t qt_meta_stringdata_Terminal__Plane = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Terminal::Plane"
QT_MOC_LITERAL(1, 16, 16), // "inputTextChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 3), // "txt"
QT_MOC_LITERAL(4, 38, 26), // "inputCursorPositionChanged"
QT_MOC_LITERAL(5, 65, 3), // "pos"
QT_MOC_LITERAL(6, 69, 18), // "inputFinishRequest"
QT_MOC_LITERAL(7, 88, 18), // "requestAutoScrollX"
QT_MOC_LITERAL(8, 107, 13), // "directionSign"
QT_MOC_LITERAL(9, 121, 18), // "requestAutoScrollY"
QT_MOC_LITERAL(10, 140, 16), // "updateScrollBars"
QT_MOC_LITERAL(11, 157, 9), // "selectAll"
QT_MOC_LITERAL(12, 167, 15), // "copyToClipboard"
QT_MOC_LITERAL(13, 183, 18), // "pasteFromClipboard"
QT_MOC_LITERAL(14, 202, 23), // "handleAutoscrollXChange"
QT_MOC_LITERAL(15, 226, 23) // "handleAutoscrollYChange"

    },
    "Terminal::Plane\0inputTextChanged\0\0txt\0"
    "inputCursorPositionChanged\0pos\0"
    "inputFinishRequest\0requestAutoScrollX\0"
    "directionSign\0requestAutoScrollY\0"
    "updateScrollBars\0selectAll\0copyToClipboard\0"
    "pasteFromClipboard\0handleAutoscrollXChange\0"
    "handleAutoscrollYChange"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Terminal__Plane[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       4,    1,   72,    2, 0x06 /* Public */,
       6,    0,   75,    2, 0x06 /* Public */,
       7,    1,   76,    2, 0x06 /* Public */,
       9,    1,   79,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   82,    2, 0x0a /* Public */,
      11,    0,   83,    2, 0x08 /* Private */,
      12,    0,   84,    2, 0x08 /* Private */,
      13,    0,   85,    2, 0x08 /* Private */,
      14,    1,   86,    2, 0x08 /* Private */,
      15,    1,   89,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::UShort,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Char,    8,
    QMetaType::Void, QMetaType::Char,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Char,    8,
    QMetaType::Void, QMetaType::Char,    8,

       0        // eod
};

void Terminal::Plane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Plane *_t = static_cast<Plane *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->inputTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->inputCursorPositionChanged((*reinterpret_cast< quint16(*)>(_a[1]))); break;
        case 2: _t->inputFinishRequest(); break;
        case 3: _t->requestAutoScrollX((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 4: _t->requestAutoScrollY((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 5: _t->updateScrollBars(); break;
        case 6: _t->selectAll(); break;
        case 7: _t->copyToClipboard(); break;
        case 8: _t->pasteFromClipboard(); break;
        case 9: _t->handleAutoscrollXChange((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 10: _t->handleAutoscrollYChange((*reinterpret_cast< char(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Plane::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Plane::inputTextChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Plane::*_t)(quint16 );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Plane::inputCursorPositionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Plane::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Plane::inputFinishRequest)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Plane::*_t)(char );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Plane::requestAutoScrollX)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (Plane::*_t)(char );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Plane::requestAutoScrollY)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject Terminal::Plane::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Terminal__Plane.data,
      qt_meta_data_Terminal__Plane,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Terminal::Plane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Terminal::Plane::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Terminal__Plane.stringdata0))
        return static_cast<void*>(const_cast< Plane*>(this));
    return QWidget::qt_metacast(_clname);
}

int Terminal::Plane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Terminal::Plane::inputTextChanged(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Terminal::Plane::inputCursorPositionChanged(quint16 _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Terminal::Plane::inputFinishRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void Terminal::Plane::requestAutoScrollX(char _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Terminal::Plane::requestAutoScrollY(char _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
