/****************************************************************************
** Meta object code from reading C++ file 'debuggerview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/coregui/debuggerview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'debuggerview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CoreGUI__DebuggerView_t {
    QByteArrayData data[9];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoreGUI__DebuggerView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoreGUI__DebuggerView_t qt_meta_stringdata_CoreGUI__DebuggerView = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CoreGUI::DebuggerView"
QT_MOC_LITERAL(1, 22, 18), // "setDebuggerEnabled"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 7), // "enabled"
QT_MOC_LITERAL(4, 50, 18), // "handleRowsInserted"
QT_MOC_LITERAL(5, 69, 5), // "index"
QT_MOC_LITERAL(6, 75, 5), // "start"
QT_MOC_LITERAL(7, 81, 3), // "end"
QT_MOC_LITERAL(8, 85, 17) // "handleRowsRemoved"

    },
    "CoreGUI::DebuggerView\0setDebuggerEnabled\0"
    "\0enabled\0handleRowsInserted\0index\0"
    "start\0end\0handleRowsRemoved"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoreGUI__DebuggerView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       4,    3,   32,    2, 0x08 /* Private */,
       8,    3,   39,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,    5,    6,    7,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,    5,    6,    7,

       0        // eod
};

void CoreGUI::DebuggerView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DebuggerView *_t = static_cast<DebuggerView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setDebuggerEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->handleRowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->handleRowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject CoreGUI::DebuggerView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_CoreGUI__DebuggerView.data,
      qt_meta_data_CoreGUI__DebuggerView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CoreGUI::DebuggerView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreGUI::DebuggerView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CoreGUI__DebuggerView.stringdata0))
        return static_cast<void*>(const_cast< DebuggerView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int CoreGUI::DebuggerView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
