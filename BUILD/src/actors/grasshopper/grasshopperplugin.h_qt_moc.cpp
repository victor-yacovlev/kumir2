/****************************************************************************
** Meta object code from reading C++ file 'grasshopperplugin.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "grasshopperplugin.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'grasshopperplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorGrasshopper__GrasshopperPlugin_t {
    QByteArrayData data[13];
    char stringdata0[186];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorGrasshopper__GrasshopperPlugin_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorGrasshopper__GrasshopperPlugin_t qt_meta_stringdata_ActorGrasshopper__GrasshopperPlugin = {
    {
QT_MOC_LITERAL(0, 0, 35), // "ActorGrasshopper::Grasshopper..."
QT_MOC_LITERAL(1, 36, 4), // "sync"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 8), // "asyncRun"
QT_MOC_LITERAL(4, 51, 33), // "notifyOnTemplateParametersCha..."
QT_MOC_LITERAL(5, 85, 13), // "loadActorData"
QT_MOC_LITERAL(6, 99, 10), // "QIODevice*"
QT_MOC_LITERAL(7, 110, 6), // "source"
QT_MOC_LITERAL(8, 117, 13), // "asyncEvaluate"
QT_MOC_LITERAL(9, 131, 5), // "index"
QT_MOC_LITERAL(10, 137, 4), // "args"
QT_MOC_LITERAL(11, 142, 38), // "handleSettingsChangedCppImple..."
QT_MOC_LITERAL(12, 181, 4) // "keys"

    },
    "ActorGrasshopper::GrasshopperPlugin\0"
    "sync\0\0asyncRun\0notifyOnTemplateParametersChanged\0"
    "loadActorData\0QIODevice*\0source\0"
    "asyncEvaluate\0index\0args\0"
    "handleSettingsChangedCppImplementation\0"
    "keys"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorGrasshopper__GrasshopperPlugin[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    2,   45,    2, 0x06 /* Public */,
       4,    0,   50,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   51,    2, 0x0a /* Public */,
       8,    2,   54,    2, 0x08 /* Private */,
      11,    1,   59,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::QVariantList,    2,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::UInt, QMetaType::QVariantList,    9,   10,
    QMetaType::Void, QMetaType::QStringList,   12,

       0        // eod
};

void ActorGrasshopper::GrasshopperPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GrasshopperPlugin *_t = static_cast<GrasshopperPlugin *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sync(); break;
        case 1: _t->asyncRun((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< const QVariantList(*)>(_a[2]))); break;
        case 2: _t->notifyOnTemplateParametersChanged(); break;
        case 3: _t->loadActorData((*reinterpret_cast< QIODevice*(*)>(_a[1]))); break;
        case 4: _t->asyncEvaluate((*reinterpret_cast< quint32(*)>(_a[1])),(*reinterpret_cast< const QVariantList(*)>(_a[2]))); break;
        case 5: _t->handleSettingsChangedCppImplementation((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GrasshopperPlugin::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GrasshopperPlugin::sync)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (GrasshopperPlugin::*_t)(quint32 , const QVariantList & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GrasshopperPlugin::asyncRun)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (GrasshopperPlugin::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GrasshopperPlugin::notifyOnTemplateParametersChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject ActorGrasshopper::GrasshopperPlugin::staticMetaObject = {
    { &ExtensionSystem::KPlugin::staticMetaObject, qt_meta_stringdata_ActorGrasshopper__GrasshopperPlugin.data,
      qt_meta_data_ActorGrasshopper__GrasshopperPlugin,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorGrasshopper::GrasshopperPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorGrasshopper::GrasshopperPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorGrasshopper__GrasshopperPlugin.stringdata0))
        return static_cast<void*>(const_cast< GrasshopperPlugin*>(this));
    if (!strcmp(_clname, "Shared::ActorInterface"))
        return static_cast< Shared::ActorInterface*>(const_cast< GrasshopperPlugin*>(this));
    if (!strcmp(_clname, "kumir2.Actor"))
        return static_cast< Shared::ActorInterface*>(const_cast< GrasshopperPlugin*>(this));
    return ExtensionSystem::KPlugin::qt_metacast(_clname);
}

int ActorGrasshopper::GrasshopperPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ExtensionSystem::KPlugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ActorGrasshopper::GrasshopperPlugin::sync()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void ActorGrasshopper::GrasshopperPlugin::asyncRun(quint32 _t1, const QVariantList & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ActorGrasshopper::GrasshopperPlugin::notifyOnTemplateParametersChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

QT_PLUGIN_METADATA_SECTION const uint qt_section_alignment_dummy = 42;

#ifdef QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0xa4, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x90, 0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x17, 0x00, 'k',  'u',  'm',  'i',  'r',  '2', 
    '.',  'A',  'c',  't',  'o',  'r',  'G',  'r', 
    'a',  's',  's',  'h',  'o',  'p',  'p',  'e', 
    'r',  0x00, 0x00, 0x00, 0x9b, 0x08, 0x00, 0x00,
    0x09, 0x00, 'c',  'l',  'a',  's',  's',  'N', 
    'a',  'm',  'e',  0x00, 0x11, 0x00, 'G',  'r', 
    'a',  's',  's',  'h',  'o',  'p',  'p',  'e', 
    'r',  'P',  'l',  'u',  'g',  'i',  'n',  0x00,
    'Z',  0xc0, 0xa0, 0x00, 0x07, 0x00, 'v',  'e', 
    'r',  's',  'i',  'o',  'n',  0x00, 0x00, 0x00,
    0x11, 0x00, 0x00, 0x00, 0x05, 0x00, 'd',  'e', 
    'b',  'u',  'g',  0x00, 0x95, 0x10, 0x00, 0x00,
    0x08, 0x00, 'M',  'e',  't',  'a',  'D',  'a', 
    't',  'a',  0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 't',  0x00, 0x00, 0x00,
    '4',  0x00, 0x00, 0x00, 'h',  0x00, 0x00, 0x00,
    'X',  0x00, 0x00, 0x00
};

#else // QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0xa4, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x90, 0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x17, 0x00, 'k',  'u',  'm',  'i',  'r',  '2', 
    '.',  'A',  'c',  't',  'o',  'r',  'G',  'r', 
    'a',  's',  's',  'h',  'o',  'p',  'p',  'e', 
    'r',  0x00, 0x00, 0x00, 0x95, 0x08, 0x00, 0x00,
    0x08, 0x00, 'M',  'e',  't',  'a',  'D',  'a', 
    't',  'a',  0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1b, 0x0c, 0x00, 0x00, 0x09, 0x00, 'c',  'l', 
    'a',  's',  's',  'N',  'a',  'm',  'e',  0x00,
    0x11, 0x00, 'G',  'r',  'a',  's',  's',  'h', 
    'o',  'p',  'p',  'e',  'r',  'P',  'l',  'u', 
    'g',  'i',  'n',  0x00, '1',  0x00, 0x00, 0x00,
    0x05, 0x00, 'd',  'e',  'b',  'u',  'g',  0x00,
    'Z',  0xc0, 0xa0, 0x00, 0x07, 0x00, 'v',  'e', 
    'r',  's',  'i',  'o',  'n',  0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, '4',  0x00, 0x00, 0x00,
    'P',  0x00, 0x00, 0x00, 't',  0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00
};
#endif // QT_NO_DEBUG

using namespace ActorGrasshopper;
QT_MOC_EXPORT_PLUGIN(ActorGrasshopper::GrasshopperPlugin, GrasshopperPlugin)

QT_END_MOC_NAMESPACE
