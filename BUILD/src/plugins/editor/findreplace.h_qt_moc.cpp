/****************************************************************************
** Meta object code from reading C++ file 'findreplace.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/editor/findreplace.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'findreplace.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor__FindReplace_t {
    QByteArrayData data[16];
    char stringdata0[208];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor__FindReplace_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor__FindReplace_t qt_meta_stringdata_Editor__FindReplace = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Editor::FindReplace"
QT_MOC_LITERAL(1, 20, 8), // "showFind"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 11), // "showReplace"
QT_MOC_LITERAL(4, 42, 23), // "handleMoreButtonChecked"
QT_MOC_LITERAL(5, 66, 1), // "v"
QT_MOC_LITERAL(6, 68, 12), // "updateLayout"
QT_MOC_LITERAL(7, 81, 11), // "replaceMode"
QT_MOC_LITERAL(8, 93, 11), // "doFindFirst"
QT_MOC_LITERAL(9, 105, 4), // "text"
QT_MOC_LITERAL(10, 110, 10), // "doFindNext"
QT_MOC_LITERAL(11, 121, 14), // "doFindPrevious"
QT_MOC_LITERAL(12, 136, 9), // "doReplace"
QT_MOC_LITERAL(13, 146, 12), // "doReplaceAll"
QT_MOC_LITERAL(14, 159, 28), // "handleSearchParameterChanged"
QT_MOC_LITERAL(15, 188, 19) // "handleReturnPressed"

    },
    "Editor::FindReplace\0showFind\0\0showReplace\0"
    "handleMoreButtonChecked\0v\0updateLayout\0"
    "replaceMode\0doFindFirst\0text\0doFindNext\0"
    "doFindPrevious\0doReplace\0doReplaceAll\0"
    "handleSearchParameterChanged\0"
    "handleReturnPressed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor__FindReplace[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    0,   70,    2, 0x0a /* Public */,
       4,    1,   71,    2, 0x08 /* Private */,
       6,    1,   74,    2, 0x08 /* Private */,
       8,    1,   77,    2, 0x08 /* Private */,
      10,    0,   80,    2, 0x08 /* Private */,
      11,    0,   81,    2, 0x08 /* Private */,
      12,    0,   82,    2, 0x08 /* Private */,
      13,    0,   83,    2, 0x08 /* Private */,
      14,    0,   84,    2, 0x08 /* Private */,
      15,    0,   85,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Editor::FindReplace::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FindReplace *_t = static_cast<FindReplace *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showFind(); break;
        case 1: _t->showReplace(); break;
        case 2: _t->handleMoreButtonChecked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->updateLayout((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->doFindFirst((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->doFindNext(); break;
        case 6: _t->doFindPrevious(); break;
        case 7: _t->doReplace(); break;
        case 8: _t->doReplaceAll(); break;
        case 9: _t->handleSearchParameterChanged(); break;
        case 10: _t->handleReturnPressed(); break;
        default: ;
        }
    }
}

const QMetaObject Editor::FindReplace::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Editor__FindReplace.data,
      qt_meta_data_Editor__FindReplace,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Editor::FindReplace::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::FindReplace::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__FindReplace.stringdata0))
        return static_cast<void*>(const_cast< FindReplace*>(this));
    return QWidget::qt_metacast(_clname);
}

int Editor::FindReplace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
