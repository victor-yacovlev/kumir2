/****************************************************************************
** Meta object code from reading C++ file 'macrolisteditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/editor/macrolisteditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'macrolisteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor__MacroListEditor_t {
    QByteArrayData data[7];
    char stringdata0[90];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor__MacroListEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor__MacroListEditor_t qt_meta_stringdata_Editor__MacroListEditor = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Editor::MacroListEditor"
QT_MOC_LITERAL(1, 24, 17), // "handleItemChanged"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 60, 7), // "current"
QT_MOC_LITERAL(5, 68, 11), // "removeMacro"
QT_MOC_LITERAL(6, 80, 9) // "editMacro"

    },
    "Editor::MacroListEditor\0handleItemChanged\0"
    "\0QListWidgetItem*\0current\0removeMacro\0"
    "editMacro"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor__MacroListEditor[] = {

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
       1,    2,   29,    2, 0x08 /* Private */,
       5,    0,   34,    2, 0x08 /* Private */,
       6,    0,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Editor::MacroListEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MacroListEditor *_t = static_cast<MacroListEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->handleItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->removeMacro(); break;
        case 2: _t->editMacro(); break;
        default: ;
        }
    }
}

const QMetaObject Editor::MacroListEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Editor__MacroListEditor.data,
      qt_meta_data_Editor__MacroListEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Editor::MacroListEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::MacroListEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__MacroListEditor.stringdata0))
        return static_cast<void*>(const_cast< MacroListEditor*>(this));
    return QDialog::qt_metacast(_clname);
}

int Editor::MacroListEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
