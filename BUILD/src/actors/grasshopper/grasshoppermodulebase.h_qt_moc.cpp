/****************************************************************************
** Meta object code from reading C++ file 'grasshoppermodulebase.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "grasshoppermodulebase.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'grasshoppermodulebase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorGrasshopper__GrasshopperModuleBase_t {
    QByteArrayData data[22];
    char stringdata0[327];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorGrasshopper__GrasshopperModuleBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorGrasshopper__GrasshopperModuleBase_t qt_meta_stringdata_ActorGrasshopper__GrasshopperModuleBase = {
    {
QT_MOC_LITERAL(0, 0, 39), // "ActorGrasshopper::Grasshopper..."
QT_MOC_LITERAL(1, 40, 33), // "notifyOnTemplateParametersCha..."
QT_MOC_LITERAL(2, 74, 0), // ""
QT_MOC_LITERAL(3, 75, 5), // "reset"
QT_MOC_LITERAL(4, 81, 14), // "reloadSettings"
QT_MOC_LITERAL(5, 96, 28), // "ExtensionSystem::SettingsPtr"
QT_MOC_LITERAL(6, 125, 8), // "settings"
QT_MOC_LITERAL(7, 134, 4), // "keys"
QT_MOC_LITERAL(8, 139, 17), // "changeGlobalState"
QT_MOC_LITERAL(9, 157, 28), // "ExtensionSystem::GlobalState"
QT_MOC_LITERAL(10, 186, 3), // "old"
QT_MOC_LITERAL(11, 190, 7), // "current"
QT_MOC_LITERAL(12, 198, 19), // "terminateEvaluation"
QT_MOC_LITERAL(13, 218, 19), // "setAnimationEnabled"
QT_MOC_LITERAL(14, 238, 7), // "enabled"
QT_MOC_LITERAL(15, 246, 12), // "runGoForward"
QT_MOC_LITERAL(16, 259, 8), // "distance"
QT_MOC_LITERAL(17, 268, 13), // "runGoBackward"
QT_MOC_LITERAL(18, 282, 12), // "runDoRepaint"
QT_MOC_LITERAL(19, 295, 13), // "loadActorData"
QT_MOC_LITERAL(20, 309, 10), // "QIODevice*"
QT_MOC_LITERAL(21, 320, 6) // "source"

    },
    "ActorGrasshopper::GrasshopperModuleBase\0"
    "notifyOnTemplateParametersChanged\0\0"
    "reset\0reloadSettings\0ExtensionSystem::SettingsPtr\0"
    "settings\0keys\0changeGlobalState\0"
    "ExtensionSystem::GlobalState\0old\0"
    "current\0terminateEvaluation\0"
    "setAnimationEnabled\0enabled\0runGoForward\0"
    "distance\0runGoBackward\0runDoRepaint\0"
    "loadActorData\0QIODevice*\0source"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorGrasshopper__GrasshopperModuleBase[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   65,    2, 0x0a /* Public */,
       4,    2,   66,    2, 0x0a /* Public */,
       8,    2,   71,    2, 0x0a /* Public */,
      12,    0,   76,    2, 0x0a /* Public */,
      13,    1,   77,    2, 0x0a /* Public */,
      15,    1,   80,    2, 0x0a /* Public */,
      17,    1,   83,    2, 0x0a /* Public */,
      18,    0,   86,    2, 0x0a /* Public */,
      19,    1,   87,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, QMetaType::QStringList,    6,    7,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 9,   10,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 20,   21,

       0        // eod
};

void ActorGrasshopper::GrasshopperModuleBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GrasshopperModuleBase *_t = static_cast<GrasshopperModuleBase *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->notifyOnTemplateParametersChanged(); break;
        case 1: _t->reset(); break;
        case 2: _t->reloadSettings((*reinterpret_cast< ExtensionSystem::SettingsPtr(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 3: _t->changeGlobalState((*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[1])),(*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[2]))); break;
        case 4: _t->terminateEvaluation(); break;
        case 5: _t->setAnimationEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->runGoForward((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 7: _t->runGoBackward((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 8: _t->runDoRepaint(); break;
        case 9: _t->loadActorData((*reinterpret_cast< QIODevice*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GrasshopperModuleBase::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GrasshopperModuleBase::notifyOnTemplateParametersChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ActorGrasshopper::GrasshopperModuleBase::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ActorGrasshopper__GrasshopperModuleBase.data,
      qt_meta_data_ActorGrasshopper__GrasshopperModuleBase,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorGrasshopper::GrasshopperModuleBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorGrasshopper::GrasshopperModuleBase::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorGrasshopper__GrasshopperModuleBase.stringdata0))
        return static_cast<void*>(const_cast< GrasshopperModuleBase*>(this));
    return QObject::qt_metacast(_clname);
}

int ActorGrasshopper::GrasshopperModuleBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void ActorGrasshopper::GrasshopperModuleBase::notifyOnTemplateParametersChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
