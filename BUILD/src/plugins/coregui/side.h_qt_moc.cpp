/****************************************************************************
** Meta object code from reading C++ file 'side.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/coregui/side.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'side.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CoreGUI__Side_t {
    QByteArrayData data[8];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoreGUI__Side_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoreGUI__Side_t qt_meta_stringdata_CoreGUI__Side = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CoreGUI::Side"
QT_MOC_LITERAL(1, 14, 16), // "visiblityRequest"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 22), // "handleVisiblityRequest"
QT_MOC_LITERAL(4, 55, 7), // "visible"
QT_MOC_LITERAL(5, 63, 4), // "size"
QT_MOC_LITERAL(6, 68, 15), // "forceResizeItem"
QT_MOC_LITERAL(7, 84, 2) // "sz"

    },
    "CoreGUI::Side\0visiblityRequest\0\0"
    "handleVisiblityRequest\0visible\0size\0"
    "forceResizeItem\0sz"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoreGUI__Side[] = {

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
       1,    0,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    2,   30,    2, 0x08 /* Private */,
       6,    1,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QSize,    4,    5,
    QMetaType::Void, QMetaType::QSize,    7,

       0        // eod
};

void CoreGUI::Side::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Side *_t = static_cast<Side *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->visiblityRequest(); break;
        case 1: _t->handleVisiblityRequest((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QSize(*)>(_a[2]))); break;
        case 2: _t->forceResizeItem((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Side::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Side::visiblityRequest)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject CoreGUI::Side::staticMetaObject = {
    { &QSplitter::staticMetaObject, qt_meta_stringdata_CoreGUI__Side.data,
      qt_meta_data_CoreGUI__Side,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CoreGUI::Side::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreGUI::Side::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CoreGUI__Side.stringdata0))
        return static_cast<void*>(const_cast< Side*>(this));
    return QSplitter::qt_metacast(_clname);
}

int CoreGUI::Side::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSplitter::qt_metacall(_c, _id, _a);
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
void CoreGUI::Side::visiblityRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
