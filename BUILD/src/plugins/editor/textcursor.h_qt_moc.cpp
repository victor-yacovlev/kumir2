/****************************************************************************
** Meta object code from reading C++ file 'textcursor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/editor/textcursor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'textcursor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor__TextCursor_t {
    QByteArrayData data[21];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor__TextCursor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor__TextCursor_t qt_meta_stringdata_Editor__TextCursor = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Editor::TextCursor"
QT_MOC_LITERAL(1, 19, 15), // "positionChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 3), // "row"
QT_MOC_LITERAL(4, 40, 3), // "col"
QT_MOC_LITERAL(5, 44, 13), // "updateRequest"
QT_MOC_LITERAL(6, 58, 8), // "fromLine"
QT_MOC_LITERAL(7, 67, 6), // "toLine"
QT_MOC_LITERAL(8, 74, 13), // "undoAvailable"
QT_MOC_LITERAL(9, 88, 13), // "redoAvailable"
QT_MOC_LITERAL(10, 102, 20), // "signalizeNotEditable"
QT_MOC_LITERAL(11, 123, 13), // "toggleComment"
QT_MOC_LITERAL(12, 137, 10), // "toggleLock"
QT_MOC_LITERAL(13, 148, 4), // "undo"
QT_MOC_LITERAL(14, 153, 4), // "redo"
QT_MOC_LITERAL(15, 158, 17), // "handleUndoChanged"
QT_MOC_LITERAL(16, 176, 1), // "v"
QT_MOC_LITERAL(17, 178, 17), // "handleRedoChanged"
QT_MOC_LITERAL(18, 196, 16), // "startRecordMacro"
QT_MOC_LITERAL(19, 213, 14), // "endRecordMacro"
QT_MOC_LITERAL(20, 228, 21) // "QSharedPointer<Macro>"

    },
    "Editor::TextCursor\0positionChanged\0\0"
    "row\0col\0updateRequest\0fromLine\0toLine\0"
    "undoAvailable\0redoAvailable\0"
    "signalizeNotEditable\0toggleComment\0"
    "toggleLock\0undo\0redo\0handleUndoChanged\0"
    "v\0handleRedoChanged\0startRecordMacro\0"
    "endRecordMacro\0QSharedPointer<Macro>"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor__TextCursor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   84,    2, 0x06 /* Public */,
       5,    0,   89,    2, 0x06 /* Public */,
       5,    2,   90,    2, 0x06 /* Public */,
       8,    1,   95,    2, 0x06 /* Public */,
       9,    1,   98,    2, 0x06 /* Public */,
      10,    0,  101,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,  102,    2, 0x0a /* Public */,
      12,    0,  103,    2, 0x0a /* Public */,
      13,    0,  104,    2, 0x0a /* Public */,
      14,    0,  105,    2, 0x0a /* Public */,
      15,    1,  106,    2, 0x0a /* Public */,
      17,    1,  109,    2, 0x0a /* Public */,
      18,    0,  112,    2, 0x0a /* Public */,
      19,    0,  113,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    6,    7,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void,
    0x80000000 | 20,

       0        // eod
};

void Editor::TextCursor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TextCursor *_t = static_cast<TextCursor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->positionChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->updateRequest(); break;
        case 2: _t->updateRequest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->undoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->redoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->signalizeNotEditable(); break;
        case 6: _t->toggleComment(); break;
        case 7: _t->toggleLock(); break;
        case 8: _t->undo(); break;
        case 9: _t->redo(); break;
        case 10: _t->handleUndoChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->handleRedoChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->startRecordMacro(); break;
        case 13: { QSharedPointer<Macro> _r = _t->endRecordMacro();
            if (_a[0]) *reinterpret_cast< QSharedPointer<Macro>*>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TextCursor::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TextCursor::positionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (TextCursor::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TextCursor::updateRequest)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (TextCursor::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TextCursor::updateRequest)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (TextCursor::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TextCursor::undoAvailable)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (TextCursor::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TextCursor::redoAvailable)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (TextCursor::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TextCursor::signalizeNotEditable)) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject Editor::TextCursor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Editor__TextCursor.data,
      qt_meta_data_Editor__TextCursor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Editor::TextCursor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::TextCursor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__TextCursor.stringdata0))
        return static_cast<void*>(const_cast< TextCursor*>(this));
    return QObject::qt_metacast(_clname);
}

int Editor::TextCursor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void Editor::TextCursor::positionChanged(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Editor::TextCursor::updateRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Editor::TextCursor::updateRequest(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Editor::TextCursor::undoAvailable(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Editor::TextCursor::redoAvailable(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Editor::TextCursor::signalizeNotEditable()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
