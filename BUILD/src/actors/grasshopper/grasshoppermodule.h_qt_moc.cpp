/****************************************************************************
** Meta object code from reading C++ file 'grasshoppermodule.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/grasshopper/grasshoppermodule.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'grasshoppermodule.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorGrasshopper__GrasshopperModule_t {
    QByteArrayData data[21];
    char stringdata0[289];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorGrasshopper__GrasshopperModule_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorGrasshopper__GrasshopperModule_t qt_meta_stringdata_ActorGrasshopper__GrasshopperModule = {
    {
QT_MOC_LITERAL(0, 0, 35), // "ActorGrasshopper::Grasshopper..."
QT_MOC_LITERAL(1, 36, 17), // "changeGlobalState"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 28), // "ExtensionSystem::GlobalState"
QT_MOC_LITERAL(4, 84, 3), // "old"
QT_MOC_LITERAL(5, 88, 7), // "current"
QT_MOC_LITERAL(6, 96, 13), // "loadActorData"
QT_MOC_LITERAL(7, 110, 10), // "QIODevice*"
QT_MOC_LITERAL(8, 121, 6), // "source"
QT_MOC_LITERAL(9, 128, 14), // "reloadSettings"
QT_MOC_LITERAL(10, 143, 28), // "ExtensionSystem::SettingsPtr"
QT_MOC_LITERAL(11, 172, 8), // "settings"
QT_MOC_LITERAL(12, 181, 4), // "keys"
QT_MOC_LITERAL(13, 186, 5), // "reset"
QT_MOC_LITERAL(14, 192, 19), // "terminateEvaluation"
QT_MOC_LITERAL(15, 212, 19), // "setAnimationEnabled"
QT_MOC_LITERAL(16, 232, 7), // "enabled"
QT_MOC_LITERAL(17, 240, 12), // "runGoForward"
QT_MOC_LITERAL(18, 253, 8), // "distance"
QT_MOC_LITERAL(19, 262, 13), // "runGoBackward"
QT_MOC_LITERAL(20, 276, 12) // "runDoRepaint"

    },
    "ActorGrasshopper::GrasshopperModule\0"
    "changeGlobalState\0\0ExtensionSystem::GlobalState\0"
    "old\0current\0loadActorData\0QIODevice*\0"
    "source\0reloadSettings\0"
    "ExtensionSystem::SettingsPtr\0settings\0"
    "keys\0reset\0terminateEvaluation\0"
    "setAnimationEnabled\0enabled\0runGoForward\0"
    "distance\0runGoBackward\0runDoRepaint"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorGrasshopper__GrasshopperModule[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x0a /* Public */,
       6,    1,   64,    2, 0x0a /* Public */,
       9,    2,   67,    2, 0x0a /* Public */,
      13,    0,   72,    2, 0x0a /* Public */,
      14,    0,   73,    2, 0x0a /* Public */,
      15,    1,   74,    2, 0x0a /* Public */,
      17,    1,   77,    2, 0x0a /* Public */,
      19,    1,   80,    2, 0x0a /* Public */,
      20,    0,   83,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 10, QMetaType::QStringList,   11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void,

       0        // eod
};

void ActorGrasshopper::GrasshopperModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GrasshopperModule *_t = static_cast<GrasshopperModule *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeGlobalState((*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[1])),(*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[2]))); break;
        case 1: _t->loadActorData((*reinterpret_cast< QIODevice*(*)>(_a[1]))); break;
        case 2: _t->reloadSettings((*reinterpret_cast< ExtensionSystem::SettingsPtr(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 3: _t->reset(); break;
        case 4: _t->terminateEvaluation(); break;
        case 5: _t->setAnimationEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->runGoForward((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 7: _t->runGoBackward((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 8: _t->runDoRepaint(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QIODevice* >(); break;
            }
            break;
        }
    }
}

const QMetaObject ActorGrasshopper::GrasshopperModule::staticMetaObject = {
    { &GrasshopperModuleBase::staticMetaObject, qt_meta_stringdata_ActorGrasshopper__GrasshopperModule.data,
      qt_meta_data_ActorGrasshopper__GrasshopperModule,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorGrasshopper::GrasshopperModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorGrasshopper::GrasshopperModule::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorGrasshopper__GrasshopperModule.stringdata0))
        return static_cast<void*>(const_cast< GrasshopperModule*>(this));
    return GrasshopperModuleBase::qt_metacast(_clname);
}

int ActorGrasshopper::GrasshopperModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GrasshopperModuleBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
