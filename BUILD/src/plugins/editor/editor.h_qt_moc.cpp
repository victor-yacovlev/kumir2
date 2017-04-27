/****************************************************************************
** Meta object code from reading C++ file 'editor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/editor/editor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor__EditorInstance_t {
    QByteArrayData data[54];
    char stringdata0[710];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor__EditorInstance_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor__EditorInstance_t qt_meta_stringdata_Editor__EditorInstance = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Editor::EditorInstance"
QT_MOC_LITERAL(1, 23, 18), // "urlsDragAndDropped"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 11), // "QList<QUrl>"
QT_MOC_LITERAL(4, 55, 20), // "documentCleanChanged"
QT_MOC_LITERAL(5, 76, 1), // "v"
QT_MOC_LITERAL(6, 78, 21), // "cursorPositionChanged"
QT_MOC_LITERAL(7, 100, 3), // "row"
QT_MOC_LITERAL(8, 104, 6), // "column"
QT_MOC_LITERAL(9, 111, 21), // "keyboardLayoutChanged"
QT_MOC_LITERAL(10, 133, 17), // "QLocale::Language"
QT_MOC_LITERAL(11, 151, 4), // "lang"
QT_MOC_LITERAL(12, 156, 8), // "capslock"
QT_MOC_LITERAL(13, 165, 5), // "shift"
QT_MOC_LITERAL(14, 171, 3), // "alt"
QT_MOC_LITERAL(15, 175, 7), // "message"
QT_MOC_LITERAL(16, 183, 23), // "requestHelpForAlgorithm"
QT_MOC_LITERAL(17, 207, 7), // "package"
QT_MOC_LITERAL(18, 215, 8), // "function"
QT_MOC_LITERAL(19, 224, 18), // "recordMacroChanged"
QT_MOC_LITERAL(20, 243, 2), // "on"
QT_MOC_LITERAL(21, 246, 26), // "breakpointCnagedOrInserted"
QT_MOC_LITERAL(22, 273, 7), // "enabled"
QT_MOC_LITERAL(23, 281, 6), // "lineNo"
QT_MOC_LITERAL(24, 288, 11), // "ignoreCount"
QT_MOC_LITERAL(25, 300, 9), // "condition"
QT_MOC_LITERAL(26, 310, 17), // "breakpointRemoved"
QT_MOC_LITERAL(27, 328, 24), // "forceCompleteCompilation"
QT_MOC_LITERAL(28, 353, 4), // "undo"
QT_MOC_LITERAL(29, 358, 4), // "redo"
QT_MOC_LITERAL(30, 363, 17), // "changeGlobalState"
QT_MOC_LITERAL(31, 381, 5), // "prevv"
QT_MOC_LITERAL(32, 387, 8), // "currentt"
QT_MOC_LITERAL(33, 396, 14), // "updateSettings"
QT_MOC_LITERAL(34, 411, 4), // "keys"
QT_MOC_LITERAL(35, 416, 16), // "updateInsertMenu"
QT_MOC_LITERAL(36, 433, 15), // "tryEscKeyAction"
QT_MOC_LITERAL(37, 449, 4), // "text"
QT_MOC_LITERAL(38, 454, 16), // "toggleBreakpoint"
QT_MOC_LITERAL(39, 471, 14), // "updatePosition"
QT_MOC_LITERAL(40, 486, 3), // "col"
QT_MOC_LITERAL(41, 490, 33), // "handleCompleteCompilationRequ..."
QT_MOC_LITERAL(42, 524, 11), // "visibleText"
QT_MOC_LITERAL(43, 536, 10), // "hiddenText"
QT_MOC_LITERAL(44, 547, 14), // "hiddenBaseLine"
QT_MOC_LITERAL(45, 562, 9), // "playMacro"
QT_MOC_LITERAL(46, 572, 22), // "handleAutoScrollChange"
QT_MOC_LITERAL(47, 595, 1), // "a"
QT_MOC_LITERAL(48, 597, 23), // "handleAutoScrollChangeX"
QT_MOC_LITERAL(49, 621, 20), // "disableInsertActions"
QT_MOC_LITERAL(50, 642, 19), // "enableInsertActions"
QT_MOC_LITERAL(51, 662, 17), // "toggleRecordMacro"
QT_MOC_LITERAL(52, 680, 10), // "editMacros"
QT_MOC_LITERAL(53, 691, 18) // "updateFromAnalizer"

    },
    "Editor::EditorInstance\0urlsDragAndDropped\0"
    "\0QList<QUrl>\0documentCleanChanged\0v\0"
    "cursorPositionChanged\0row\0column\0"
    "keyboardLayoutChanged\0QLocale::Language\0"
    "lang\0capslock\0shift\0alt\0message\0"
    "requestHelpForAlgorithm\0package\0"
    "function\0recordMacroChanged\0on\0"
    "breakpointCnagedOrInserted\0enabled\0"
    "lineNo\0ignoreCount\0condition\0"
    "breakpointRemoved\0forceCompleteCompilation\0"
    "undo\0redo\0changeGlobalState\0prevv\0"
    "currentt\0updateSettings\0keys\0"
    "updateInsertMenu\0tryEscKeyAction\0text\0"
    "toggleBreakpoint\0updatePosition\0col\0"
    "handleCompleteCompilationRequiest\0"
    "visibleText\0hiddenText\0hiddenBaseLine\0"
    "playMacro\0handleAutoScrollChange\0a\0"
    "handleAutoScrollChangeX\0disableInsertActions\0"
    "enableInsertActions\0toggleRecordMacro\0"
    "editMacros\0updateFromAnalizer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor__EditorInstance[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  149,    2, 0x06 /* Public */,
       4,    1,  152,    2, 0x06 /* Public */,
       6,    2,  155,    2, 0x06 /* Public */,
       9,    4,  160,    2, 0x06 /* Public */,
      15,    1,  169,    2, 0x06 /* Public */,
      16,    2,  172,    2, 0x06 /* Public */,
      19,    1,  177,    2, 0x06 /* Public */,
      21,    4,  180,    2, 0x06 /* Public */,
      26,    1,  189,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      27,    0,  192,    2, 0x0a /* Public */,
      28,    0,  193,    2, 0x0a /* Public */,
      29,    0,  194,    2, 0x0a /* Public */,
      30,    2,  195,    2, 0x0a /* Public */,
      33,    1,  200,    2, 0x0a /* Public */,
      35,    0,  203,    2, 0x0a /* Public */,
      36,    1,  204,    2, 0x0a /* Public */,
      38,    0,  207,    2, 0x0a /* Public */,
      39,    2,  208,    2, 0x08 /* Private */,
      41,    3,  213,    2, 0x08 /* Private */,
      45,    0,  220,    2, 0x08 /* Private */,
      46,    1,  221,    2, 0x08 /* Private */,
      48,    1,  224,    2, 0x08 /* Private */,
      49,    0,  227,    2, 0x08 /* Private */,
      50,    0,  228,    2, 0x08 /* Private */,
      51,    1,  229,    2, 0x08 /* Private */,
      52,    0,  232,    2, 0x08 /* Private */,
      53,    0,  233,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,    7,    8,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,   11,   12,   13,   14,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   17,   18,
    QMetaType::Void, QMetaType::Bool,   20,
    QMetaType::Void, QMetaType::Bool, QMetaType::UInt, QMetaType::UInt, QMetaType::QString,   22,   23,   24,   25,
    QMetaType::Void, QMetaType::UInt,   23,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,   31,   32,
    QMetaType::Void, QMetaType::QStringList,   34,
    QMetaType::Void,
    QMetaType::Bool, QMetaType::QString,   37,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    7,   40,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QStringList, QMetaType::Int,   42,   43,   44,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Char,   47,
    QMetaType::Void, QMetaType::Char,   47,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   20,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Editor::EditorInstance::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorInstance *_t = static_cast<EditorInstance *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->urlsDragAndDropped((*reinterpret_cast< const QList<QUrl>(*)>(_a[1]))); break;
        case 1: _t->documentCleanChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->cursorPositionChanged((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 3: _t->keyboardLayoutChanged((*reinterpret_cast< QLocale::Language(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 4: _t->message((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->requestHelpForAlgorithm((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->recordMacroChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->breakpointCnagedOrInserted((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< quint32(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 8: _t->breakpointRemoved((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        case 9: _t->forceCompleteCompilation(); break;
        case 10: _t->undo(); break;
        case 11: _t->redo(); break;
        case 12: _t->changeGlobalState((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2]))); break;
        case 13: _t->updateSettings((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 14: _t->updateInsertMenu(); break;
        case 15: { bool _r = _t->tryEscKeyAction((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 16: _t->toggleBreakpoint(); break;
        case 17: _t->updatePosition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 18: _t->handleCompleteCompilationRequiest((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 19: _t->playMacro(); break;
        case 20: _t->handleAutoScrollChange((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 21: _t->handleAutoScrollChangeX((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 22: _t->disableInsertActions(); break;
        case 23: _t->enableInsertActions(); break;
        case 24: _t->toggleRecordMacro((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 25: _t->editMacros(); break;
        case 26: _t->updateFromAnalizer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QUrl> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (EditorInstance::*_t)(const QList<QUrl> & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorInstance::urlsDragAndDropped)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (EditorInstance::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorInstance::documentCleanChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (EditorInstance::*_t)(uint , uint );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorInstance::cursorPositionChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (EditorInstance::*_t)(QLocale::Language , bool , bool , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorInstance::keyboardLayoutChanged)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (EditorInstance::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorInstance::message)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (EditorInstance::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorInstance::requestHelpForAlgorithm)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (EditorInstance::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorInstance::recordMacroChanged)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (EditorInstance::*_t)(bool , quint32 , quint32 , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorInstance::breakpointCnagedOrInserted)) {
                *result = 7;
                return;
            }
        }
        {
            typedef void (EditorInstance::*_t)(quint32 );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorInstance::breakpointRemoved)) {
                *result = 8;
                return;
            }
        }
    }
}

const QMetaObject Editor::EditorInstance::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Editor__EditorInstance.data,
      qt_meta_data_Editor__EditorInstance,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Editor::EditorInstance::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::EditorInstance::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__EditorInstance.stringdata0))
        return static_cast<void*>(const_cast< EditorInstance*>(this));
    if (!strcmp(_clname, "Shared::Editor::InstanceInterface"))
        return static_cast< Shared::Editor::InstanceInterface*>(const_cast< EditorInstance*>(this));
    if (!strcmp(_clname, "kumir2.Editor.InstanceInterface"))
        return static_cast< Shared::Editor::InstanceInterface*>(const_cast< EditorInstance*>(this));
    return QWidget::qt_metacast(_clname);
}

int Editor::EditorInstance::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void Editor::EditorInstance::urlsDragAndDropped(const QList<QUrl> & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Editor::EditorInstance::documentCleanChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Editor::EditorInstance::cursorPositionChanged(uint _t1, uint _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Editor::EditorInstance::keyboardLayoutChanged(QLocale::Language _t1, bool _t2, bool _t3, bool _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Editor::EditorInstance::message(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Editor::EditorInstance::requestHelpForAlgorithm(const QString & _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Editor::EditorInstance::recordMacroChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Editor::EditorInstance::breakpointCnagedOrInserted(bool _t1, quint32 _t2, quint32 _t3, const QString & _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Editor::EditorInstance::breakpointRemoved(quint32 _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_END_MOC_NAMESPACE
