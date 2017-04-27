/****************************************************************************
** Meta object code from reading C++ file 'drawmodule.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/draw/drawmodule.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'drawmodule.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorDraw__DrawView_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorDraw__DrawView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorDraw__DrawView_t qt_meta_stringdata_ActorDraw__DrawView = {
    {
QT_MOC_LITERAL(0, 0, 19) // "ActorDraw::DrawView"

    },
    "ActorDraw::DrawView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorDraw__DrawView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void ActorDraw::DrawView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject ActorDraw::DrawView::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_ActorDraw__DrawView.data,
      qt_meta_data_ActorDraw__DrawView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorDraw::DrawView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorDraw::DrawView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorDraw__DrawView.stringdata0))
        return static_cast<void*>(const_cast< DrawView*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int ActorDraw::DrawView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_ActorDraw__DrawScene_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorDraw__DrawScene_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorDraw__DrawScene_t qt_meta_stringdata_ActorDraw__DrawScene = {
    {
QT_MOC_LITERAL(0, 0, 20) // "ActorDraw::DrawScene"

    },
    "ActorDraw::DrawScene"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorDraw__DrawScene[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void ActorDraw::DrawScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject ActorDraw::DrawScene::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_ActorDraw__DrawScene.data,
      qt_meta_data_ActorDraw__DrawScene,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorDraw::DrawScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorDraw::DrawScene::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorDraw__DrawScene.stringdata0))
        return static_cast<void*>(const_cast< DrawScene*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int ActorDraw::DrawScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_ActorDraw__DrawModule_t {
    QByteArrayData data[45];
    char stringdata0[463];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorDraw__DrawModule_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorDraw__DrawModule_t qt_meta_stringdata_ActorDraw__DrawModule = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ActorDraw::DrawModule"
QT_MOC_LITERAL(1, 22, 17), // "changeGlobalState"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 28), // "ExtensionSystem::GlobalState"
QT_MOC_LITERAL(4, 70, 3), // "old"
QT_MOC_LITERAL(5, 74, 7), // "current"
QT_MOC_LITERAL(6, 82, 13), // "loadActorData"
QT_MOC_LITERAL(7, 96, 10), // "QIODevice*"
QT_MOC_LITERAL(8, 107, 6), // "source"
QT_MOC_LITERAL(9, 114, 14), // "reloadSettings"
QT_MOC_LITERAL(10, 129, 28), // "ExtensionSystem::SettingsPtr"
QT_MOC_LITERAL(11, 158, 8), // "settings"
QT_MOC_LITERAL(12, 167, 4), // "keys"
QT_MOC_LITERAL(13, 172, 5), // "reset"
QT_MOC_LITERAL(14, 178, 19), // "setAnimationEnabled"
QT_MOC_LITERAL(15, 198, 7), // "enabled"
QT_MOC_LITERAL(16, 206, 11), // "runSetupPen"
QT_MOC_LITERAL(17, 218, 13), // "runReleasePen"
QT_MOC_LITERAL(18, 232, 14), // "runSetPenColor"
QT_MOC_LITERAL(19, 247, 5), // "Color"
QT_MOC_LITERAL(20, 253, 5), // "color"
QT_MOC_LITERAL(21, 259, 9), // "runMoveTo"
QT_MOC_LITERAL(22, 269, 1), // "x"
QT_MOC_LITERAL(23, 271, 1), // "y"
QT_MOC_LITERAL(24, 273, 9), // "runMoveBy"
QT_MOC_LITERAL(25, 283, 2), // "dX"
QT_MOC_LITERAL(26, 286, 2), // "dY"
QT_MOC_LITERAL(27, 289, 13), // "runAddCaption"
QT_MOC_LITERAL(28, 303, 5), // "width"
QT_MOC_LITERAL(29, 309, 4), // "text"
QT_MOC_LITERAL(30, 314, 17), // "runIsLineAtCircle"
QT_MOC_LITERAL(31, 332, 6), // "radius"
QT_MOC_LITERAL(32, 339, 12), // "zoomFullDraw"
QT_MOC_LITERAL(33, 352, 7), // "drawNet"
QT_MOC_LITERAL(34, 360, 13), // "autoNetChange"
QT_MOC_LITERAL(35, 374, 5), // "value"
QT_MOC_LITERAL(36, 380, 13), // "netStepChange"
QT_MOC_LITERAL(37, 394, 6), // "zoomIn"
QT_MOC_LITERAL(38, 401, 7), // "zoomOut"
QT_MOC_LITERAL(39, 409, 8), // "zoomNorm"
QT_MOC_LITERAL(40, 418, 13), // "showNavigator"
QT_MOC_LITERAL(41, 432, 5), // "state"
QT_MOC_LITERAL(42, 438, 8), // "openFile"
QT_MOC_LITERAL(43, 447, 8), // "saveFile"
QT_MOC_LITERAL(44, 456, 6) // "redraw"

    },
    "ActorDraw::DrawModule\0changeGlobalState\0"
    "\0ExtensionSystem::GlobalState\0old\0"
    "current\0loadActorData\0QIODevice*\0"
    "source\0reloadSettings\0"
    "ExtensionSystem::SettingsPtr\0settings\0"
    "keys\0reset\0setAnimationEnabled\0enabled\0"
    "runSetupPen\0runReleasePen\0runSetPenColor\0"
    "Color\0color\0runMoveTo\0x\0y\0runMoveBy\0"
    "dX\0dY\0runAddCaption\0width\0text\0"
    "runIsLineAtCircle\0radius\0zoomFullDraw\0"
    "drawNet\0autoNetChange\0value\0netStepChange\0"
    "zoomIn\0zoomOut\0zoomNorm\0showNavigator\0"
    "state\0openFile\0saveFile\0redraw"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorDraw__DrawModule[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,  129,    2, 0x0a /* Public */,
       6,    1,  134,    2, 0x0a /* Public */,
       9,    2,  137,    2, 0x0a /* Public */,
      13,    0,  142,    2, 0x0a /* Public */,
      14,    1,  143,    2, 0x0a /* Public */,
      16,    0,  146,    2, 0x0a /* Public */,
      17,    0,  147,    2, 0x0a /* Public */,
      18,    1,  148,    2, 0x0a /* Public */,
      21,    2,  151,    2, 0x0a /* Public */,
      24,    2,  156,    2, 0x0a /* Public */,
      27,    2,  161,    2, 0x0a /* Public */,
      30,    3,  166,    2, 0x0a /* Public */,
      32,    0,  173,    2, 0x0a /* Public */,
      33,    0,  174,    2, 0x0a /* Public */,
      34,    1,  175,    2, 0x0a /* Public */,
      36,    1,  178,    2, 0x0a /* Public */,
      37,    0,  181,    2, 0x0a /* Public */,
      38,    0,  182,    2, 0x0a /* Public */,
      39,    0,  183,    2, 0x0a /* Public */,
      40,    1,  184,    2, 0x0a /* Public */,
      42,    0,  187,    2, 0x0a /* Public */,
      43,    0,  188,    2, 0x0a /* Public */,
      44,    0,  189,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 10, QMetaType::QStringList,   11,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal,   22,   23,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal,   25,   26,
    QMetaType::Void, QMetaType::QReal, QMetaType::QString,   28,   29,
    QMetaType::Bool, QMetaType::QReal, QMetaType::QReal, QMetaType::QReal,   22,   23,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   35,
    QMetaType::Void, QMetaType::Double,   35,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ActorDraw::DrawModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DrawModule *_t = static_cast<DrawModule *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeGlobalState((*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[1])),(*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[2]))); break;
        case 1: _t->loadActorData((*reinterpret_cast< QIODevice*(*)>(_a[1]))); break;
        case 2: _t->reloadSettings((*reinterpret_cast< ExtensionSystem::SettingsPtr(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 3: _t->reset(); break;
        case 4: _t->setAnimationEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->runSetupPen(); break;
        case 6: _t->runReleasePen(); break;
        case 7: _t->runSetPenColor((*reinterpret_cast< const Color(*)>(_a[1]))); break;
        case 8: _t->runMoveTo((*reinterpret_cast< const qreal(*)>(_a[1])),(*reinterpret_cast< const qreal(*)>(_a[2]))); break;
        case 9: _t->runMoveBy((*reinterpret_cast< const qreal(*)>(_a[1])),(*reinterpret_cast< const qreal(*)>(_a[2]))); break;
        case 10: _t->runAddCaption((*reinterpret_cast< const qreal(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 11: { bool _r = _t->runIsLineAtCircle((*reinterpret_cast< const qreal(*)>(_a[1])),(*reinterpret_cast< const qreal(*)>(_a[2])),(*reinterpret_cast< const qreal(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: _t->zoomFullDraw(); break;
        case 13: _t->drawNet(); break;
        case 14: _t->autoNetChange((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->netStepChange((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->zoomIn(); break;
        case 17: _t->zoomOut(); break;
        case 18: _t->zoomNorm(); break;
        case 19: _t->showNavigator((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->openFile(); break;
        case 21: _t->saveFile(); break;
        case 22: _t->redraw(); break;
        default: ;
        }
    }
}

const QMetaObject ActorDraw::DrawModule::staticMetaObject = {
    { &DrawModuleBase::staticMetaObject, qt_meta_stringdata_ActorDraw__DrawModule.data,
      qt_meta_data_ActorDraw__DrawModule,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorDraw::DrawModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorDraw::DrawModule::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorDraw__DrawModule.stringdata0))
        return static_cast<void*>(const_cast< DrawModule*>(this));
    return DrawModuleBase::qt_metacast(_clname);
}

int ActorDraw::DrawModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawModuleBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
