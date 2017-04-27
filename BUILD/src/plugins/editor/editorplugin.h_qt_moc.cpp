/****************************************************************************
** Meta object code from reading C++ file 'editorplugin.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/editor/editorplugin.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editorplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor__EditorPlugin_t {
    QByteArrayData data[6];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor__EditorPlugin_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor__EditorPlugin_t qt_meta_stringdata_Editor__EditorPlugin = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Editor::EditorPlugin"
QT_MOC_LITERAL(1, 21, 21), // "settingsUpdateRequest"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 4), // "keys"
QT_MOC_LITERAL(4, 49, 24), // "globalStateUpdateRequest"
QT_MOC_LITERAL(5, 74, 23) // "updateInsertMenuRequest"

    },
    "Editor::EditorPlugin\0settingsUpdateRequest\0"
    "\0keys\0globalStateUpdateRequest\0"
    "updateInsertMenuRequest"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor__EditorPlugin[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    2,   32,    2, 0x06 /* Public */,
       5,    0,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    3,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,    2,    2,
    QMetaType::Void,

       0        // eod
};

void Editor::EditorPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorPlugin *_t = static_cast<EditorPlugin *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->settingsUpdateRequest((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 1: _t->globalStateUpdateRequest((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2]))); break;
        case 2: _t->updateInsertMenuRequest(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (EditorPlugin::*_t)(const QStringList & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorPlugin::settingsUpdateRequest)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (EditorPlugin::*_t)(quint32 , quint32 );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorPlugin::globalStateUpdateRequest)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (EditorPlugin::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorPlugin::updateInsertMenuRequest)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject Editor::EditorPlugin::staticMetaObject = {
    { &ExtensionSystem::KPlugin::staticMetaObject, qt_meta_stringdata_Editor__EditorPlugin.data,
      qt_meta_data_Editor__EditorPlugin,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Editor::EditorPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::EditorPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__EditorPlugin.stringdata0))
        return static_cast<void*>(const_cast< EditorPlugin*>(this));
    if (!strcmp(_clname, "Shared::EditorInterface"))
        return static_cast< Shared::EditorInterface*>(const_cast< EditorPlugin*>(this));
    if (!strcmp(_clname, "kumir2.editor"))
        return static_cast< Shared::EditorInterface*>(const_cast< EditorPlugin*>(this));
    return ExtensionSystem::KPlugin::qt_metacast(_clname);
}

int Editor::EditorPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ExtensionSystem::KPlugin::qt_metacall(_c, _id, _a);
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
void Editor::EditorPlugin::settingsUpdateRequest(const QStringList & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Editor::EditorPlugin::globalStateUpdateRequest(quint32 _t1, quint32 _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Editor::EditorPlugin::updateInsertMenuRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

QT_PLUGIN_METADATA_SECTION const uint qt_section_alignment_dummy = 42;

#ifdef QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0x94, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x0d, 0x00, 'k',  'u',  'm',  'i',  'r',  '2', 
    '.',  'E',  'd',  'i',  't',  'o',  'r',  0x00,
    0x1b, 0x07, 0x00, 0x00, 0x09, 0x00, 'c',  'l', 
    'a',  's',  's',  'N',  'a',  'm',  'e',  0x00,
    0x0c, 0x00, 'E',  'd',  'i',  't',  'o',  'r', 
    'P',  'l',  'u',  'g',  'i',  'n',  0x00, 0x00,
    'Z',  0xc0, 0xa0, 0x00, 0x07, 0x00, 'v',  'e', 
    'r',  's',  'i',  'o',  'n',  0x00, 0x00, 0x00,
    0x11, 0x00, 0x00, 0x00, 0x05, 0x00, 'd',  'e', 
    'b',  'u',  'g',  0x00, 0x95, 0x0e, 0x00, 0x00,
    0x08, 0x00, 'M',  'e',  't',  'a',  'D',  'a', 
    't',  'a',  0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 'd',  0x00, 0x00, 0x00,
    '(',  0x00, 0x00, 0x00, 'X',  0x00, 0x00, 0x00,
    'H',  0x00, 0x00, 0x00
};

#else // QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0x94, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x0d, 0x00, 'k',  'u',  'm',  'i',  'r',  '2', 
    '.',  'E',  'd',  'i',  't',  'o',  'r',  0x00,
    0x15, 0x07, 0x00, 0x00, 0x08, 0x00, 'M',  'e', 
    't',  'a',  'D',  'a',  't',  'a',  0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x9b, 0x0a, 0x00, 0x00,
    0x09, 0x00, 'c',  'l',  'a',  's',  's',  'N', 
    'a',  'm',  'e',  0x00, 0x0c, 0x00, 'E',  'd', 
    'i',  't',  'o',  'r',  'P',  'l',  'u',  'g', 
    'i',  'n',  0x00, 0x00, '1',  0x00, 0x00, 0x00,
    0x05, 0x00, 'd',  'e',  'b',  'u',  'g',  0x00,
    'Z',  0xc0, 0xa0, 0x00, 0x07, 0x00, 'v',  'e', 
    'r',  's',  'i',  'o',  'n',  0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, '(',  0x00, 0x00, 0x00,
    'D',  0x00, 0x00, 0x00, 'd',  0x00, 0x00, 0x00,
    'p',  0x00, 0x00, 0x00
};
#endif // QT_NO_DEBUG

using namespace Editor;
QT_MOC_EXPORT_PLUGIN(Editor::EditorPlugin, EditorPlugin)

QT_END_MOC_NAMESPACE
