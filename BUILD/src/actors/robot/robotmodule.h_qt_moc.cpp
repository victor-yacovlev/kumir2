/****************************************************************************
** Meta object code from reading C++ file 'robotmodule.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/robot/robotmodule.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'robotmodule.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorRobot__SimpleRobot_t {
    QByteArrayData data[4];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorRobot__SimpleRobot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorRobot__SimpleRobot_t qt_meta_stringdata_ActorRobot__SimpleRobot = {
    {
QT_MOC_LITERAL(0, 0, 23), // "ActorRobot::SimpleRobot"
QT_MOC_LITERAL(1, 24, 5), // "moved"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 5) // "point"

    },
    "ActorRobot::SimpleRobot\0moved\0\0point"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorRobot__SimpleRobot[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QPointF,    3,

       0        // eod
};

void ActorRobot::SimpleRobot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SimpleRobot *_t = static_cast<SimpleRobot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->moved((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SimpleRobot::*_t)(QPointF );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SimpleRobot::moved)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ActorRobot::SimpleRobot::staticMetaObject = {
    { &QGraphicsObject::staticMetaObject, qt_meta_stringdata_ActorRobot__SimpleRobot.data,
      qt_meta_data_ActorRobot__SimpleRobot,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorRobot::SimpleRobot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorRobot::SimpleRobot::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorRobot__SimpleRobot.stringdata0))
        return static_cast<void*>(const_cast< SimpleRobot*>(this));
    return QGraphicsObject::qt_metacast(_clname);
}

int ActorRobot::SimpleRobot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ActorRobot::SimpleRobot::moved(QPointF _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ActorRobot__FieldItm_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorRobot__FieldItm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorRobot__FieldItm_t qt_meta_stringdata_ActorRobot__FieldItm = {
    {
QT_MOC_LITERAL(0, 0, 20) // "ActorRobot::FieldItm"

    },
    "ActorRobot::FieldItm"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorRobot__FieldItm[] = {

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

void ActorRobot::FieldItm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject ActorRobot::FieldItm::staticMetaObject = {
    { &QGraphicsWidget::staticMetaObject, qt_meta_stringdata_ActorRobot__FieldItm.data,
      qt_meta_data_ActorRobot__FieldItm,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorRobot::FieldItm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorRobot::FieldItm::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorRobot__FieldItm.stringdata0))
        return static_cast<void*>(const_cast< FieldItm*>(this));
    return QGraphicsWidget::qt_metacast(_clname);
}

int ActorRobot::FieldItm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_ActorRobot__RoboField_t {
    QByteArrayData data[14];
    char stringdata0[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorRobot__RoboField_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorRobot__RoboField_t qt_meta_stringdata_ActorRobot__RoboField = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ActorRobot::RoboField"
QT_MOC_LITERAL(1, 22, 10), // "MousePress"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 1), // "x"
QT_MOC_LITERAL(4, 36, 1), // "y"
QT_MOC_LITERAL(5, 38, 4), // "Flag"
QT_MOC_LITERAL(6, 43, 9), // "roboMoved"
QT_MOC_LITERAL(7, 53, 3), // "pos"
QT_MOC_LITERAL(8, 57, 12), // "cellDialogOk"
QT_MOC_LITERAL(9, 70, 8), // "timerTic"
QT_MOC_LITERAL(10, 79, 6), // "addRow"
QT_MOC_LITERAL(11, 86, 6), // "remRow"
QT_MOC_LITERAL(12, 93, 6), // "remCol"
QT_MOC_LITERAL(13, 100, 6) // "addCol"

    },
    "ActorRobot::RoboField\0MousePress\0\0x\0"
    "y\0Flag\0roboMoved\0pos\0cellDialogOk\0"
    "timerTic\0addRow\0remRow\0remCol\0addCol"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorRobot__RoboField[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   61,    2, 0x0a /* Public */,
       8,    0,   64,    2, 0x0a /* Public */,
       9,    0,   65,    2, 0x0a /* Public */,
      10,    0,   66,    2, 0x0a /* Public */,
      11,    0,   67,    2, 0x0a /* Public */,
      12,    0,   68,    2, 0x0a /* Public */,
      13,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::Bool,    3,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QPointF,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ActorRobot::RoboField::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RoboField *_t = static_cast<RoboField *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->MousePress((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->roboMoved((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 2: _t->cellDialogOk(); break;
        case 3: _t->timerTic(); break;
        case 4: _t->addRow(); break;
        case 5: _t->remRow(); break;
        case 6: _t->remCol(); break;
        case 7: _t->addCol(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RoboField::*_t)(qreal , qreal , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RoboField::MousePress)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ActorRobot::RoboField::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_ActorRobot__RoboField.data,
      qt_meta_data_ActorRobot__RoboField,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorRobot::RoboField::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorRobot::RoboField::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorRobot__RoboField.stringdata0))
        return static_cast<void*>(const_cast< RoboField*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int ActorRobot::RoboField::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ActorRobot::RoboField::MousePress(qreal _t1, qreal _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ActorRobot__RobotView_t {
    QByteArrayData data[11];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorRobot__RobotView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorRobot__RobotView_t qt_meta_stringdata_ActorRobot__RobotView = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ActorRobot::RobotView"
QT_MOC_LITERAL(1, 22, 13), // "resizeRequest"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 11), // "newGeometry"
QT_MOC_LITERAL(4, 49, 12), // "handleDocked"
QT_MOC_LITERAL(5, 62, 14), // "changeEditMode"
QT_MOC_LITERAL(6, 77, 5), // "state"
QT_MOC_LITERAL(7, 83, 7), // "setDock"
QT_MOC_LITERAL(8, 91, 10), // "reloadSett"
QT_MOC_LITERAL(9, 102, 28), // "ExtensionSystem::SettingsPtr"
QT_MOC_LITERAL(10, 131, 8) // "settings"

    },
    "ActorRobot::RobotView\0resizeRequest\0"
    "\0newGeometry\0handleDocked\0changeEditMode\0"
    "state\0setDock\0reloadSett\0"
    "ExtensionSystem::SettingsPtr\0settings"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorRobot__RobotView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   42,    2, 0x0a /* Public */,
       5,    1,   43,    2, 0x0a /* Public */,
       7,    1,   46,    2, 0x0a /* Public */,
       8,    1,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QSize,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

void ActorRobot::RobotView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RobotView *_t = static_cast<RobotView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resizeRequest((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 1: _t->handleDocked(); break;
        case 2: _t->changeEditMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->setDock((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->reloadSett((*reinterpret_cast< ExtensionSystem::SettingsPtr(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RobotView::*_t)(const QSize & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotView::resizeRequest)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ActorRobot::RobotView::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_ActorRobot__RobotView.data,
      qt_meta_data_ActorRobot__RobotView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorRobot::RobotView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorRobot::RobotView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorRobot__RobotView.stringdata0))
        return static_cast<void*>(const_cast< RobotView*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int ActorRobot::RobotView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ActorRobot::RobotView::resizeRequest(const QSize & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ActorRobot__RobotModule_t {
    QByteArrayData data[69];
    char stringdata0[906];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorRobot__RobotModule_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorRobot__RobotModule_t qt_meta_stringdata_ActorRobot__RobotModule = {
    {
QT_MOC_LITERAL(0, 0, 23), // "ActorRobot::RobotModule"
QT_MOC_LITERAL(1, 24, 13), // "sendToPultLog"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 5), // "reset"
QT_MOC_LITERAL(4, 45, 19), // "terminateEvaluation"
QT_MOC_LITERAL(5, 65, 13), // "loadActorData"
QT_MOC_LITERAL(6, 79, 10), // "QIODevice*"
QT_MOC_LITERAL(7, 90, 6), // "source"
QT_MOC_LITERAL(8, 97, 19), // "setAnimationEnabled"
QT_MOC_LITERAL(9, 117, 7), // "enabled"
QT_MOC_LITERAL(10, 125, 31), // "acceptableCommandLineParameters"
QT_MOC_LITERAL(11, 157, 44), // "QList<ExtensionSystem::Comman..."
QT_MOC_LITERAL(12, 202, 10), // "initialize"
QT_MOC_LITERAL(13, 213, 23), // "configurationParameters"
QT_MOC_LITERAL(14, 237, 28), // "ExtensionSystem::CommandLine"
QT_MOC_LITERAL(15, 266, 17), // "runtimeParameters"
QT_MOC_LITERAL(16, 284, 7), // "runGoUp"
QT_MOC_LITERAL(17, 292, 9), // "runGoDown"
QT_MOC_LITERAL(18, 302, 9), // "runGoLeft"
QT_MOC_LITERAL(19, 312, 10), // "runGoRight"
QT_MOC_LITERAL(20, 323, 10), // "runDoPaint"
QT_MOC_LITERAL(21, 334, 14), // "runIsWallAtTop"
QT_MOC_LITERAL(22, 349, 17), // "runIsWallAtBottom"
QT_MOC_LITERAL(23, 367, 15), // "runIsWallAtLeft"
QT_MOC_LITERAL(24, 383, 16), // "runIsWallAtRight"
QT_MOC_LITERAL(25, 400, 14), // "runIsFreeAtTop"
QT_MOC_LITERAL(26, 415, 17), // "runIsFreeAtBottom"
QT_MOC_LITERAL(27, 433, 15), // "runIsFreeAtLeft"
QT_MOC_LITERAL(28, 449, 16), // "runIsFreeAtRight"
QT_MOC_LITERAL(29, 466, 10), // "runIsColor"
QT_MOC_LITERAL(30, 477, 10), // "runIsClear"
QT_MOC_LITERAL(31, 488, 12), // "runRadiation"
QT_MOC_LITERAL(32, 501, 14), // "runTemperature"
QT_MOC_LITERAL(33, 516, 12), // "runFieldSize"
QT_MOC_LITERAL(34, 529, 4), // "int&"
QT_MOC_LITERAL(35, 534, 4), // "rows"
QT_MOC_LITERAL(36, 539, 4), // "cols"
QT_MOC_LITERAL(37, 544, 7), // "runMark"
QT_MOC_LITERAL(38, 552, 3), // "row"
QT_MOC_LITERAL(39, 556, 3), // "col"
QT_MOC_LITERAL(40, 560, 10), // "runColored"
QT_MOC_LITERAL(41, 571, 11), // "runRobotPos"
QT_MOC_LITERAL(42, 583, 11), // "runCellTemp"
QT_MOC_LITERAL(43, 595, 10), // "runCellRad"
QT_MOC_LITERAL(44, 606, 9), // "runUpChar"
QT_MOC_LITERAL(45, 616, 11), // "runDownChar"
QT_MOC_LITERAL(46, 628, 12), // "isSafeToQuit"
QT_MOC_LITERAL(47, 641, 14), // "reloadSettings"
QT_MOC_LITERAL(48, 656, 28), // "ExtensionSystem::SettingsPtr"
QT_MOC_LITERAL(49, 685, 8), // "settings"
QT_MOC_LITERAL(50, 694, 4), // "keys"
QT_MOC_LITERAL(51, 699, 17), // "changeGlobalState"
QT_MOC_LITERAL(52, 717, 28), // "ExtensionSystem::GlobalState"
QT_MOC_LITERAL(53, 746, 3), // "old"
QT_MOC_LITERAL(54, 750, 7), // "current"
QT_MOC_LITERAL(55, 758, 7), // "loadEnv"
QT_MOC_LITERAL(56, 766, 8), // "resetEnv"
QT_MOC_LITERAL(57, 775, 8), // "save2png"
QT_MOC_LITERAL(58, 784, 7), // "saveEnv"
QT_MOC_LITERAL(59, 792, 7), // "editEnv"
QT_MOC_LITERAL(60, 800, 6), // "newEnv"
QT_MOC_LITERAL(61, 807, 14), // "createNewField"
QT_MOC_LITERAL(62, 822, 17), // "createRescentMenu"
QT_MOC_LITERAL(63, 840, 15), // "updateLastFiles"
QT_MOC_LITERAL(64, 856, 7), // "newFile"
QT_MOC_LITERAL(65, 864, 10), // "openRecent"
QT_MOC_LITERAL(66, 875, 13), // "setWindowSize"
QT_MOC_LITERAL(67, 889, 12), // "copyFromPult"
QT_MOC_LITERAL(68, 902, 3) // "log"

    },
    "ActorRobot::RobotModule\0sendToPultLog\0"
    "\0reset\0terminateEvaluation\0loadActorData\0"
    "QIODevice*\0source\0setAnimationEnabled\0"
    "enabled\0acceptableCommandLineParameters\0"
    "QList<ExtensionSystem::CommandLineParameter>\0"
    "initialize\0configurationParameters\0"
    "ExtensionSystem::CommandLine\0"
    "runtimeParameters\0runGoUp\0runGoDown\0"
    "runGoLeft\0runGoRight\0runDoPaint\0"
    "runIsWallAtTop\0runIsWallAtBottom\0"
    "runIsWallAtLeft\0runIsWallAtRight\0"
    "runIsFreeAtTop\0runIsFreeAtBottom\0"
    "runIsFreeAtLeft\0runIsFreeAtRight\0"
    "runIsColor\0runIsClear\0runRadiation\0"
    "runTemperature\0runFieldSize\0int&\0rows\0"
    "cols\0runMark\0row\0col\0runColored\0"
    "runRobotPos\0runCellTemp\0runCellRad\0"
    "runUpChar\0runDownChar\0isSafeToQuit\0"
    "reloadSettings\0ExtensionSystem::SettingsPtr\0"
    "settings\0keys\0changeGlobalState\0"
    "ExtensionSystem::GlobalState\0old\0"
    "current\0loadEnv\0resetEnv\0save2png\0"
    "saveEnv\0editEnv\0newEnv\0createNewField\0"
    "createRescentMenu\0updateLastFiles\0"
    "newFile\0openRecent\0setWindowSize\0"
    "copyFromPult\0log"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorRobot__RobotModule[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      47,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  249,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,  252,    2, 0x0a /* Public */,
       4,    0,  253,    2, 0x0a /* Public */,
       5,    1,  254,    2, 0x0a /* Public */,
       8,    1,  257,    2, 0x0a /* Public */,
      10,    0,  260,    2, 0x0a /* Public */,
      12,    2,  261,    2, 0x0a /* Public */,
      16,    0,  266,    2, 0x0a /* Public */,
      17,    0,  267,    2, 0x0a /* Public */,
      18,    0,  268,    2, 0x0a /* Public */,
      19,    0,  269,    2, 0x0a /* Public */,
      20,    0,  270,    2, 0x0a /* Public */,
      21,    0,  271,    2, 0x0a /* Public */,
      22,    0,  272,    2, 0x0a /* Public */,
      23,    0,  273,    2, 0x0a /* Public */,
      24,    0,  274,    2, 0x0a /* Public */,
      25,    0,  275,    2, 0x0a /* Public */,
      26,    0,  276,    2, 0x0a /* Public */,
      27,    0,  277,    2, 0x0a /* Public */,
      28,    0,  278,    2, 0x0a /* Public */,
      29,    0,  279,    2, 0x0a /* Public */,
      30,    0,  280,    2, 0x0a /* Public */,
      31,    0,  281,    2, 0x0a /* Public */,
      32,    0,  282,    2, 0x0a /* Public */,
      33,    2,  283,    2, 0x0a /* Public */,
      37,    2,  288,    2, 0x0a /* Public */,
      40,    2,  293,    2, 0x0a /* Public */,
      41,    2,  298,    2, 0x0a /* Public */,
      42,    2,  303,    2, 0x0a /* Public */,
      43,    2,  308,    2, 0x0a /* Public */,
      44,    2,  313,    2, 0x0a /* Public */,
      45,    2,  318,    2, 0x0a /* Public */,
      46,    0,  323,    2, 0x0a /* Public */,
      47,    2,  324,    2, 0x0a /* Public */,
      51,    2,  329,    2, 0x0a /* Public */,
      55,    0,  334,    2, 0x0a /* Public */,
      56,    0,  335,    2, 0x0a /* Public */,
      57,    0,  336,    2, 0x0a /* Public */,
      58,    0,  337,    2, 0x0a /* Public */,
      59,    0,  338,    2, 0x0a /* Public */,
      60,    0,  339,    2, 0x0a /* Public */,
      61,    0,  340,    2, 0x0a /* Public */,
      62,    0,  341,    2, 0x0a /* Public */,
      63,    1,  342,    2, 0x0a /* Public */,
      65,    0,  345,    2, 0x0a /* Public */,
      66,    0,  346,    2, 0x0a /* Public */,
      67,    1,  347,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QVariant,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::Bool,    9,
    0x80000000 | 11,
    QMetaType::QString, QMetaType::QStringList, 0x80000000 | 14,   13,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::QReal,
    QMetaType::Int,
    QMetaType::Void, 0x80000000 | 34, 0x80000000 | 34,   35,   36,
    QMetaType::Bool, QMetaType::Int, QMetaType::Int,   38,   39,
    QMetaType::Bool, QMetaType::Int, QMetaType::Int,   38,   39,
    QMetaType::Void, 0x80000000 | 34, 0x80000000 | 34,   38,   39,
    QMetaType::Int, QMetaType::Int, QMetaType::Int,   38,   39,
    QMetaType::QReal, QMetaType::Int, QMetaType::Int,   38,   39,
    QMetaType::QChar, QMetaType::Int, QMetaType::Int,   38,   39,
    QMetaType::QChar, QMetaType::Int, QMetaType::Int,   38,   39,
    QMetaType::Bool,
    QMetaType::Void, 0x80000000 | 48, QMetaType::QStringList,   49,   50,
    QMetaType::Void, 0x80000000 | 52, 0x80000000 | 52,   53,   54,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   64,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   68,

       0        // eod
};

void ActorRobot::RobotModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RobotModule *_t = static_cast<RobotModule *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendToPultLog((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 1: _t->reset(); break;
        case 2: _t->terminateEvaluation(); break;
        case 3: _t->loadActorData((*reinterpret_cast< QIODevice*(*)>(_a[1]))); break;
        case 4: _t->setAnimationEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: { QList<ExtensionSystem::CommandLineParameter> _r = _t->acceptableCommandLineParameters();
            if (_a[0]) *reinterpret_cast< QList<ExtensionSystem::CommandLineParameter>*>(_a[0]) = _r; }  break;
        case 6: { QString _r = _t->initialize((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const ExtensionSystem::CommandLine(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 7: _t->runGoUp(); break;
        case 8: _t->runGoDown(); break;
        case 9: _t->runGoLeft(); break;
        case 10: _t->runGoRight(); break;
        case 11: _t->runDoPaint(); break;
        case 12: { bool _r = _t->runIsWallAtTop();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = _t->runIsWallAtBottom();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: { bool _r = _t->runIsWallAtLeft();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 15: { bool _r = _t->runIsWallAtRight();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 16: { bool _r = _t->runIsFreeAtTop();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 17: { bool _r = _t->runIsFreeAtBottom();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 18: { bool _r = _t->runIsFreeAtLeft();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 19: { bool _r = _t->runIsFreeAtRight();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 20: { bool _r = _t->runIsColor();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 21: { bool _r = _t->runIsClear();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 22: { qreal _r = _t->runRadiation();
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 23: { int _r = _t->runTemperature();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 24: _t->runFieldSize((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 25: { bool _r = _t->runMark((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 26: { bool _r = _t->runColored((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 27: _t->runRobotPos((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 28: { int _r = _t->runCellTemp((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 29: { qreal _r = _t->runCellRad((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 30: { QChar _r = _t->runUpChar((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QChar*>(_a[0]) = _r; }  break;
        case 31: { QChar _r = _t->runDownChar((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QChar*>(_a[0]) = _r; }  break;
        case 32: { bool _r = _t->isSafeToQuit();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 33: _t->reloadSettings((*reinterpret_cast< ExtensionSystem::SettingsPtr(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 34: _t->changeGlobalState((*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[1])),(*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[2]))); break;
        case 35: _t->loadEnv(); break;
        case 36: _t->resetEnv(); break;
        case 37: _t->save2png(); break;
        case 38: _t->saveEnv(); break;
        case 39: _t->editEnv(); break;
        case 40: _t->newEnv(); break;
        case 41: _t->createNewField(); break;
        case 42: _t->createRescentMenu(); break;
        case 43: _t->updateLastFiles((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 44: _t->openRecent(); break;
        case 45: _t->setWindowSize(); break;
        case 46: _t->copyFromPult((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QIODevice* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RobotModule::*_t)(const QVariant & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RobotModule::sendToPultLog)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ActorRobot::RobotModule::staticMetaObject = {
    { &RobotModuleBase::staticMetaObject, qt_meta_stringdata_ActorRobot__RobotModule.data,
      qt_meta_data_ActorRobot__RobotModule,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorRobot::RobotModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorRobot::RobotModule::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorRobot__RobotModule.stringdata0))
        return static_cast<void*>(const_cast< RobotModule*>(this));
    return RobotModuleBase::qt_metacast(_clname);
}

int ActorRobot::RobotModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RobotModuleBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    }
    return _id;
}

// SIGNAL 0
void ActorRobot::RobotModule::sendToPultLog(const QVariant & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ActorRobot__EditLine_t {
    QByteArrayData data[1];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorRobot__EditLine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorRobot__EditLine_t qt_meta_stringdata_ActorRobot__EditLine = {
    {
QT_MOC_LITERAL(0, 0, 20) // "ActorRobot::EditLine"

    },
    "ActorRobot::EditLine"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorRobot__EditLine[] = {

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

void ActorRobot::EditLine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject ActorRobot::EditLine::staticMetaObject = {
    { &QGraphicsObject::staticMetaObject, qt_meta_stringdata_ActorRobot__EditLine.data,
      qt_meta_data_ActorRobot__EditLine,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorRobot::EditLine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorRobot::EditLine::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorRobot__EditLine.stringdata0))
        return static_cast<void*>(const_cast< EditLine*>(this));
    return QGraphicsObject::qt_metacast(_clname);
}

int ActorRobot::EditLine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
