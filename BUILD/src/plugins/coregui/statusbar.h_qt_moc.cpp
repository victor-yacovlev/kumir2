/****************************************************************************
** Meta object code from reading C++ file 'statusbar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/coregui/statusbar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'statusbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CoreGUI__StatusBar_t {
    QByteArrayData data[27];
    char stringdata0[325];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoreGUI__StatusBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoreGUI__StatusBar_t qt_meta_stringdata_CoreGUI__StatusBar = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CoreGUI::StatusBar"
QT_MOC_LITERAL(1, 19, 8), // "setState"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 28), // "ExtensionSystem::GlobalState"
QT_MOC_LITERAL(4, 58, 5), // "state"
QT_MOC_LITERAL(5, 64, 10), // "setMessage"
QT_MOC_LITERAL(6, 75, 7), // "message"
QT_MOC_LITERAL(7, 83, 11), // "MessageRole"
QT_MOC_LITERAL(8, 95, 4), // "role"
QT_MOC_LITERAL(9, 100, 12), // "unsetMessage"
QT_MOC_LITERAL(10, 113, 19), // "setStepsDoneCounter"
QT_MOC_LITERAL(11, 133, 5), // "value"
QT_MOC_LITERAL(12, 139, 16), // "setErrorsCounter"
QT_MOC_LITERAL(13, 156, 23), // "setEditorCursorPosition"
QT_MOC_LITERAL(14, 180, 3), // "row"
QT_MOC_LITERAL(15, 184, 6), // "column"
QT_MOC_LITERAL(16, 191, 23), // "setEditorKeyboardLayout"
QT_MOC_LITERAL(17, 215, 17), // "QLocale::Language"
QT_MOC_LITERAL(18, 233, 4), // "lang"
QT_MOC_LITERAL(19, 238, 8), // "capslock"
QT_MOC_LITERAL(20, 247, 5), // "shift"
QT_MOC_LITERAL(21, 253, 3), // "alt"
QT_MOC_LITERAL(22, 257, 18), // "setRecordIndicator"
QT_MOC_LITERAL(23, 276, 2), // "on"
QT_MOC_LITERAL(24, 279, 15), // "setDocumentType"
QT_MOC_LITERAL(25, 295, 24), // "MainWindow::DocumentType"
QT_MOC_LITERAL(26, 320, 4) // "type"

    },
    "CoreGUI::StatusBar\0setState\0\0"
    "ExtensionSystem::GlobalState\0state\0"
    "setMessage\0message\0MessageRole\0role\0"
    "unsetMessage\0setStepsDoneCounter\0value\0"
    "setErrorsCounter\0setEditorCursorPosition\0"
    "row\0column\0setEditorKeyboardLayout\0"
    "QLocale::Language\0lang\0capslock\0shift\0"
    "alt\0setRecordIndicator\0on\0setDocumentType\0"
    "MainWindow::DocumentType\0type"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoreGUI__StatusBar[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x0a /* Public */,
       5,    2,   67,    2, 0x0a /* Public */,
       5,    1,   72,    2, 0x2a /* Public | MethodCloned */,
       9,    0,   75,    2, 0x0a /* Public */,
      10,    1,   76,    2, 0x0a /* Public */,
      12,    1,   79,    2, 0x0a /* Public */,
      13,    2,   82,    2, 0x0a /* Public */,
      16,    4,   87,    2, 0x0a /* Public */,
      22,    1,   96,    2, 0x0a /* Public */,
      24,    1,   99,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 7,    6,    8,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt,   11,
    QMetaType::Void, QMetaType::UInt,   11,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,   14,   15,
    QMetaType::Void, 0x80000000 | 17, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,   18,   19,   20,   21,
    QMetaType::Void, QMetaType::Bool,   23,
    QMetaType::Void, 0x80000000 | 25,   26,

       0        // eod
};

void CoreGUI::StatusBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StatusBar *_t = static_cast<StatusBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setState((*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[1]))); break;
        case 1: _t->setMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< MessageRole(*)>(_a[2]))); break;
        case 2: _t->setMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->unsetMessage(); break;
        case 4: _t->setStepsDoneCounter((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 5: _t->setErrorsCounter((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 6: _t->setEditorCursorPosition((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 7: _t->setEditorKeyboardLayout((*reinterpret_cast< QLocale::Language(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 8: _t->setRecordIndicator((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setDocumentType((*reinterpret_cast< MainWindow::DocumentType(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CoreGUI::StatusBar::staticMetaObject = {
    { &QStatusBar::staticMetaObject, qt_meta_stringdata_CoreGUI__StatusBar.data,
      qt_meta_data_CoreGUI__StatusBar,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CoreGUI::StatusBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreGUI::StatusBar::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CoreGUI__StatusBar.stringdata0))
        return static_cast<void*>(const_cast< StatusBar*>(this));
    return QStatusBar::qt_metacast(_clname);
}

int CoreGUI::StatusBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStatusBar::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
