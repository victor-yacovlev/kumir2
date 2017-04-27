/****************************************************************************
** Meta object code from reading C++ file 'clipboard.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/editor/clipboard.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clipboard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor__Clipboard_t {
    QByteArrayData data[14];
    char stringdata0[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor__Clipboard_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor__Clipboard_t qt_meta_stringdata_Editor__Clipboard = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Editor::Clipboard"
QT_MOC_LITERAL(1, 18, 25), // "bufferEntriesCountChanged"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 1), // "c"
QT_MOC_LITERAL(4, 47, 4), // "push"
QT_MOC_LITERAL(5, 52, 13), // "ClipboardData"
QT_MOC_LITERAL(6, 66, 4), // "data"
QT_MOC_LITERAL(7, 71, 6), // "select"
QT_MOC_LITERAL(8, 78, 5), // "index"
QT_MOC_LITERAL(9, 84, 10), // "hasContent"
QT_MOC_LITERAL(10, 95, 7), // "content"
QT_MOC_LITERAL(11, 103, 5), // "clear"
QT_MOC_LITERAL(12, 109, 12), // "entriesCount"
QT_MOC_LITERAL(13, 122, 15) // "checkForChanged"

    },
    "Editor::Clipboard\0bufferEntriesCountChanged\0"
    "\0c\0push\0ClipboardData\0data\0select\0"
    "index\0hasContent\0content\0clear\0"
    "entriesCount\0checkForChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor__Clipboard[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   57,    2, 0x0a /* Public */,
       7,    1,   60,    2, 0x0a /* Public */,
       9,    0,   63,    2, 0x0a /* Public */,
      10,    0,   64,    2, 0x0a /* Public */,
      11,    0,   65,    2, 0x0a /* Public */,
      12,    0,   66,    2, 0x0a /* Public */,
      13,    0,   67,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Bool,
    0x80000000 | 5,
    QMetaType::Void,
    QMetaType::Int,
    QMetaType::Void,

       0        // eod
};

void Editor::Clipboard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Clipboard *_t = static_cast<Clipboard *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->bufferEntriesCountChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->push((*reinterpret_cast< const ClipboardData(*)>(_a[1]))); break;
        case 2: _t->select((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: { bool _r = _t->hasContent();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: { ClipboardData _r = _t->content();
            if (_a[0]) *reinterpret_cast< ClipboardData*>(_a[0]) = _r; }  break;
        case 5: _t->clear(); break;
        case 6: { int _r = _t->entriesCount();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: _t->checkForChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Clipboard::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Clipboard::bufferEntriesCountChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Editor::Clipboard::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Editor__Clipboard.data,
      qt_meta_data_Editor__Clipboard,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Editor::Clipboard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::Clipboard::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__Clipboard.stringdata0))
        return static_cast<void*>(const_cast< Clipboard*>(this));
    return QObject::qt_metacast(_clname);
}

int Editor::Clipboard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Editor::Clipboard::bufferEntriesCountChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
