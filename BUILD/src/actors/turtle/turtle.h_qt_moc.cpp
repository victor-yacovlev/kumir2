/****************************************************************************
** Meta object code from reading C++ file 'turtle.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/turtle/turtle.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'turtle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_turtle_t {
    QByteArrayData data[15];
    char stringdata0[90];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_turtle_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_turtle_t qt_meta_stringdata_turtle = {
    {
QT_MOC_LITERAL(0, 0, 6), // "turtle"
QT_MOC_LITERAL(1, 7, 5), // "Otkaz"
QT_MOC_LITERAL(2, 13, 0), // ""
QT_MOC_LITERAL(3, 14, 3), // "msg"
QT_MOC_LITERAL(4, 18, 2), // "Ok"
QT_MOC_LITERAL(5, 21, 6), // "rotate"
QT_MOC_LITERAL(6, 28, 5), // "moveT"
QT_MOC_LITERAL(7, 34, 6), // "TailUp"
QT_MOC_LITERAL(8, 41, 8), // "TailDown"
QT_MOC_LITERAL(9, 50, 5), // "reset"
QT_MOC_LITERAL(10, 56, 8), // "DoRotate"
QT_MOC_LITERAL(11, 65, 3), // "deg"
QT_MOC_LITERAL(12, 69, 6), // "DoMove"
QT_MOC_LITERAL(13, 76, 5), // "range"
QT_MOC_LITERAL(14, 82, 7) // "Repaint"

    },
    "turtle\0Otkaz\0\0msg\0Ok\0rotate\0moveT\0"
    "TailUp\0TailDown\0reset\0DoRotate\0deg\0"
    "DoMove\0range\0Repaint"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_turtle[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       4,    0,   67,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   68,    2, 0x0a /* Public */,
       6,    0,   69,    2, 0x0a /* Public */,
       7,    0,   70,    2, 0x0a /* Public */,
       8,    0,   71,    2, 0x0a /* Public */,
       9,    0,   72,    2, 0x0a /* Public */,
      10,    1,   73,    2, 0x0a /* Public */,
      12,    1,   76,    2, 0x0a /* Public */,
      14,    0,   79,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   11,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void,

       0        // eod
};

void turtle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        turtle *_t = static_cast<turtle *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Otkaz((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->Ok(); break;
        case 2: _t->rotate(); break;
        case 3: { bool _r = _t->moveT();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: _t->TailUp(); break;
        case 5: _t->TailDown(); break;
        case 6: _t->reset(); break;
        case 7: _t->DoRotate((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->DoMove((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->Repaint(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (turtle::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&turtle::Otkaz)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (turtle::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&turtle::Ok)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject turtle::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_turtle.data,
      qt_meta_data_turtle,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *turtle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *turtle::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_turtle.stringdata0))
        return static_cast<void*>(const_cast< turtle*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int turtle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void turtle::Otkaz(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void turtle::Ok()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
