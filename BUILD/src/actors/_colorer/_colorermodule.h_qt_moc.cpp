/****************************************************************************
** Meta object code from reading C++ file '_colorermodule.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/_colorer/_colorermodule.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file '_colorermodule.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Actor_Colorer___ColorerModule_t {
    QByteArrayData data[24];
    char stringdata0[297];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Actor_Colorer___ColorerModule_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Actor_Colorer___ColorerModule_t qt_meta_stringdata_Actor_Colorer___ColorerModule = {
    {
QT_MOC_LITERAL(0, 0, 29), // "Actor_Colorer::_ColorerModule"
QT_MOC_LITERAL(1, 30, 17), // "changeGlobalState"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 28), // "ExtensionSystem::GlobalState"
QT_MOC_LITERAL(4, 78, 3), // "old"
QT_MOC_LITERAL(5, 82, 7), // "current"
QT_MOC_LITERAL(6, 90, 13), // "loadActorData"
QT_MOC_LITERAL(7, 104, 10), // "QIODevice*"
QT_MOC_LITERAL(8, 115, 6), // "source"
QT_MOC_LITERAL(9, 122, 14), // "reloadSettings"
QT_MOC_LITERAL(10, 137, 28), // "ExtensionSystem::SettingsPtr"
QT_MOC_LITERAL(11, 166, 8), // "settings"
QT_MOC_LITERAL(12, 175, 4), // "keys"
QT_MOC_LITERAL(13, 180, 5), // "reset"
QT_MOC_LITERAL(14, 186, 19), // "terminateEvaluation"
QT_MOC_LITERAL(15, 206, 16), // "runOperatorINPUT"
QT_MOC_LITERAL(16, 223, 5), // "Color"
QT_MOC_LITERAL(17, 229, 1), // "x"
QT_MOC_LITERAL(18, 231, 5), // "bool&"
QT_MOC_LITERAL(19, 237, 2), // "ok"
QT_MOC_LITERAL(20, 240, 17), // "runOperatorOUTPUT"
QT_MOC_LITERAL(21, 258, 16), // "runOperatorEQUAL"
QT_MOC_LITERAL(22, 275, 1), // "y"
QT_MOC_LITERAL(23, 277, 19) // "runOperatorNOTEQUAL"

    },
    "Actor_Colorer::_ColorerModule\0"
    "changeGlobalState\0\0ExtensionSystem::GlobalState\0"
    "old\0current\0loadActorData\0QIODevice*\0"
    "source\0reloadSettings\0"
    "ExtensionSystem::SettingsPtr\0settings\0"
    "keys\0reset\0terminateEvaluation\0"
    "runOperatorINPUT\0Color\0x\0bool&\0ok\0"
    "runOperatorOUTPUT\0runOperatorEQUAL\0y\0"
    "runOperatorNOTEQUAL"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Actor_Colorer___ColorerModule[] = {

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
      15,    2,   74,    2, 0x0a /* Public */,
      20,    1,   79,    2, 0x0a /* Public */,
      21,    2,   82,    2, 0x0a /* Public */,
      23,    2,   87,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 10, QMetaType::QStringList,   11,   12,
    QMetaType::Void,
    QMetaType::Void,
    0x80000000 | 16, QMetaType::QString, 0x80000000 | 18,   17,   19,
    QMetaType::QString, 0x80000000 | 16,   17,
    QMetaType::Bool, 0x80000000 | 16, 0x80000000 | 16,   17,   22,
    QMetaType::Bool, 0x80000000 | 16, 0x80000000 | 16,   17,   22,

       0        // eod
};

void Actor_Colorer::_ColorerModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        _ColorerModule *_t = static_cast<_ColorerModule *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeGlobalState((*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[1])),(*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[2]))); break;
        case 1: _t->loadActorData((*reinterpret_cast< QIODevice*(*)>(_a[1]))); break;
        case 2: _t->reloadSettings((*reinterpret_cast< ExtensionSystem::SettingsPtr(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 3: _t->reset(); break;
        case 4: _t->terminateEvaluation(); break;
        case 5: { Color _r = _t->runOperatorINPUT((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 6: { QString _r = _t->runOperatorOUTPUT((*reinterpret_cast< const Color(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 7: { bool _r = _t->runOperatorEQUAL((*reinterpret_cast< const Color(*)>(_a[1])),(*reinterpret_cast< const Color(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: { bool _r = _t->runOperatorNOTEQUAL((*reinterpret_cast< const Color(*)>(_a[1])),(*reinterpret_cast< const Color(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject Actor_Colorer::_ColorerModule::staticMetaObject = {
    { &_ColorerModuleBase::staticMetaObject, qt_meta_stringdata_Actor_Colorer___ColorerModule.data,
      qt_meta_data_Actor_Colorer___ColorerModule,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Actor_Colorer::_ColorerModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Actor_Colorer::_ColorerModule::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Actor_Colorer___ColorerModule.stringdata0))
        return static_cast<void*>(const_cast< _ColorerModule*>(this));
    return _ColorerModuleBase::qt_metacast(_clname);
}

int Actor_Colorer::_ColorerModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = _ColorerModuleBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
