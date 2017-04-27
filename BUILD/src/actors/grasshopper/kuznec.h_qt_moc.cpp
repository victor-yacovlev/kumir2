/****************************************************************************
** Meta object code from reading C++ file 'kuznec.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/grasshopper/kuznec.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kuznec.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_KumScene_t {
    QByteArrayData data[12];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumScene_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumScene_t qt_meta_stringdata_KumScene = {
    {
QT_MOC_LITERAL(0, 0, 8), // "KumScene"
QT_MOC_LITERAL(1, 9, 10), // "MousePress"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 1), // "x"
QT_MOC_LITERAL(4, 23, 1), // "y"
QT_MOC_LITERAL(5, 25, 4), // "Flag"
QT_MOC_LITERAL(6, 30, 6), // "xScene"
QT_MOC_LITERAL(7, 37, 6), // "yScene"
QT_MOC_LITERAL(8, 44, 12), // "MouseRelease"
QT_MOC_LITERAL(9, 57, 9), // "MouseMove"
QT_MOC_LITERAL(10, 67, 10), // "MouseWheel"
QT_MOC_LITERAL(11, 78, 5) // "Delta"

    },
    "KumScene\0MousePress\0\0x\0y\0Flag\0xScene\0"
    "yScene\0MouseRelease\0MouseMove\0MouseWheel\0"
    "Delta"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumScene[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    5,   34,    2, 0x06 /* Public */,
       8,    3,   45,    2, 0x06 /* Public */,
       9,    3,   52,    2, 0x06 /* Public */,
      10,    1,   59,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::Bool, QMetaType::QReal, QMetaType::QReal,    3,    4,    5,    6,    7,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void, QMetaType::Int,   11,

       0        // eod
};

void KumScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KumScene *_t = static_cast<KumScene *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->MousePress((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4])),(*reinterpret_cast< qreal(*)>(_a[5]))); break;
        case 1: _t->MouseRelease((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: _t->MouseMove((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: _t->MouseWheel((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (KumScene::*_t)(qreal , qreal , bool , qreal , qreal );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KumScene::MousePress)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (KumScene::*_t)(qreal , qreal , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KumScene::MouseRelease)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (KumScene::*_t)(int , int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KumScene::MouseMove)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (KumScene::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KumScene::MouseWheel)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject KumScene::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_KumScene.data,
      qt_meta_data_KumScene,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumScene::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumScene.stringdata0))
        return static_cast<void*>(const_cast< KumScene*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int KumScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void KumScene::MousePress(qreal _t1, qreal _t2, bool _t3, qreal _t4, qreal _t5)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KumScene::MouseRelease(qreal _t1, qreal _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KumScene::MouseMove(int _t1, int _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KumScene::MouseWheel(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_KumKuznec_t {
    QByteArrayData data[74];
    char stringdata0[704];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumKuznec_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumKuznec_t qt_meta_stringdata_KumKuznec = {
    {
QT_MOC_LITERAL(0, 0, 9), // "KumKuznec"
QT_MOC_LITERAL(1, 10, 4), // "sync"
QT_MOC_LITERAL(2, 15, 0), // ""
QT_MOC_LITERAL(3, 16, 8), // "setSteps"
QT_MOC_LITERAL(4, 25, 3), // "fwd"
QT_MOC_LITERAL(5, 29, 6), // "backwd"
QT_MOC_LITERAL(6, 36, 10), // "setFwdStep"
QT_MOC_LITERAL(7, 47, 11), // "setBackStep"
QT_MOC_LITERAL(8, 59, 3), // "bwd"
QT_MOC_LITERAL(9, 63, 11), // "stepForward"
QT_MOC_LITERAL(10, 75, 12), // "stepBackward"
QT_MOC_LITERAL(11, 88, 10), // "resizeSlot"
QT_MOC_LITERAL(12, 99, 6), // "QSize*"
QT_MOC_LITERAL(13, 106, 7), // "oldSize"
QT_MOC_LITERAL(14, 114, 7), // "newSize"
QT_MOC_LITERAL(15, 122, 12), // "lockControls"
QT_MOC_LITERAL(16, 135, 14), // "unlockControls"
QT_MOC_LITERAL(17, 150, 12), // "ClearPicture"
QT_MOC_LITERAL(18, 163, 6), // "ToDubl"
QT_MOC_LITERAL(19, 170, 8), // "FromDubl"
QT_MOC_LITERAL(20, 179, 8), // "ToWindow"
QT_MOC_LITERAL(21, 188, 8), // "MoveLeft"
QT_MOC_LITERAL(22, 197, 9), // "MoveRight"
QT_MOC_LITERAL(23, 207, 6), // "MoveUp"
QT_MOC_LITERAL(24, 214, 8), // "MoveDown"
QT_MOC_LITERAL(25, 223, 6), // "ZoomUp"
QT_MOC_LITERAL(26, 230, 8), // "ZoomDown"
QT_MOC_LITERAL(27, 239, 10), // "PrintToPDF"
QT_MOC_LITERAL(28, 250, 5), // "Close"
QT_MOC_LITERAL(29, 256, 8), // "ToCenter"
QT_MOC_LITERAL(30, 265, 10), // "FindKuznec"
QT_MOC_LITERAL(31, 276, 4), // "Info"
QT_MOC_LITERAL(32, 281, 10), // "MousePress"
QT_MOC_LITERAL(33, 292, 1), // "x"
QT_MOC_LITERAL(34, 294, 1), // "y"
QT_MOC_LITERAL(35, 296, 4), // "Flag"
QT_MOC_LITERAL(36, 301, 6), // "xScene"
QT_MOC_LITERAL(37, 308, 6), // "yScene"
QT_MOC_LITERAL(38, 315, 12), // "MouseRelease"
QT_MOC_LITERAL(39, 328, 9), // "MouseMove"
QT_MOC_LITERAL(40, 338, 10), // "MouseWheel"
QT_MOC_LITERAL(41, 349, 5), // "Delta"
QT_MOC_LITERAL(42, 355, 11), // "HorizWindow"
QT_MOC_LITERAL(43, 367, 10), // "VertWindow"
QT_MOC_LITERAL(44, 378, 12), // "StepXChanged"
QT_MOC_LITERAL(45, 391, 12), // "StepYChanged"
QT_MOC_LITERAL(46, 404, 14), // "NetCancelClick"
QT_MOC_LITERAL(47, 419, 10), // "NetOKClick"
QT_MOC_LITERAL(48, 430, 11), // "InfoOKClick"
QT_MOC_LITERAL(49, 442, 21), // "LoadFromFileActivated"
QT_MOC_LITERAL(50, 464, 19), // "SaveToFileActivated"
QT_MOC_LITERAL(51, 484, 9), // "CloseInst"
QT_MOC_LITERAL(52, 494, 10), // "SwitchMode"
QT_MOC_LITERAL(53, 505, 4), // "mode"
QT_MOC_LITERAL(54, 510, 14), // "showHideWindow"
QT_MOC_LITERAL(55, 525, 4), // "show"
QT_MOC_LITERAL(56, 530, 12), // "setWindowPos"
QT_MOC_LITERAL(57, 543, 12), // "getWindowPos"
QT_MOC_LITERAL(58, 556, 4), // "int*"
QT_MOC_LITERAL(59, 561, 17), // "getWindowGeometry"
QT_MOC_LITERAL(60, 579, 7), // "MoveFwd"
QT_MOC_LITERAL(61, 587, 6), // "canFwd"
QT_MOC_LITERAL(62, 594, 8), // "MoveBack"
QT_MOC_LITERAL(63, 603, 10), // "mainWindow"
QT_MOC_LITERAL(64, 614, 8), // "QWidget*"
QT_MOC_LITERAL(65, 623, 7), // "canBack"
QT_MOC_LITERAL(66, 631, 11), // "createFlags"
QT_MOC_LITERAL(67, 643, 11), // "redrawFlags"
QT_MOC_LITERAL(68, 655, 7), // "colored"
QT_MOC_LITERAL(69, 663, 12), // "ColorUncolor"
QT_MOC_LITERAL(70, 676, 7), // "addSled"
QT_MOC_LITERAL(71, 684, 5), // "start"
QT_MOC_LITERAL(72, 690, 3), // "fin"
QT_MOC_LITERAL(73, 694, 9) // "AutoClose"

    },
    "KumKuznec\0sync\0\0setSteps\0fwd\0backwd\0"
    "setFwdStep\0setBackStep\0bwd\0stepForward\0"
    "stepBackward\0resizeSlot\0QSize*\0oldSize\0"
    "newSize\0lockControls\0unlockControls\0"
    "ClearPicture\0ToDubl\0FromDubl\0ToWindow\0"
    "MoveLeft\0MoveRight\0MoveUp\0MoveDown\0"
    "ZoomUp\0ZoomDown\0PrintToPDF\0Close\0"
    "ToCenter\0FindKuznec\0Info\0MousePress\0"
    "x\0y\0Flag\0xScene\0yScene\0MouseRelease\0"
    "MouseMove\0MouseWheel\0Delta\0HorizWindow\0"
    "VertWindow\0StepXChanged\0StepYChanged\0"
    "NetCancelClick\0NetOKClick\0InfoOKClick\0"
    "LoadFromFileActivated\0SaveToFileActivated\0"
    "CloseInst\0SwitchMode\0mode\0showHideWindow\0"
    "show\0setWindowPos\0getWindowPos\0int*\0"
    "getWindowGeometry\0MoveFwd\0canFwd\0"
    "MoveBack\0mainWindow\0QWidget*\0canBack\0"
    "createFlags\0redrawFlags\0colored\0"
    "ColorUncolor\0addSled\0start\0fin\0AutoClose"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumKuznec[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      55,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  289,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    2,  290,    2, 0x0a /* Public */,
       6,    1,  295,    2, 0x0a /* Public */,
       7,    1,  298,    2, 0x0a /* Public */,
       9,    0,  301,    2, 0x0a /* Public */,
      10,    0,  302,    2, 0x0a /* Public */,
      11,    2,  303,    2, 0x0a /* Public */,
      15,    0,  308,    2, 0x0a /* Public */,
      16,    0,  309,    2, 0x0a /* Public */,
      17,    0,  310,    2, 0x0a /* Public */,
      18,    0,  311,    2, 0x0a /* Public */,
      19,    0,  312,    2, 0x0a /* Public */,
      20,    0,  313,    2, 0x0a /* Public */,
      21,    0,  314,    2, 0x0a /* Public */,
      22,    0,  315,    2, 0x0a /* Public */,
      23,    0,  316,    2, 0x0a /* Public */,
      24,    0,  317,    2, 0x0a /* Public */,
      25,    0,  318,    2, 0x0a /* Public */,
      26,    0,  319,    2, 0x0a /* Public */,
      27,    0,  320,    2, 0x0a /* Public */,
      28,    0,  321,    2, 0x0a /* Public */,
      29,    0,  322,    2, 0x0a /* Public */,
      30,    0,  323,    2, 0x0a /* Public */,
      31,    0,  324,    2, 0x0a /* Public */,
      32,    5,  325,    2, 0x0a /* Public */,
      38,    3,  336,    2, 0x0a /* Public */,
      39,    3,  343,    2, 0x0a /* Public */,
      40,    1,  350,    2, 0x0a /* Public */,
      42,    0,  353,    2, 0x0a /* Public */,
      43,    0,  354,    2, 0x0a /* Public */,
      44,    0,  355,    2, 0x0a /* Public */,
      45,    0,  356,    2, 0x0a /* Public */,
      46,    0,  357,    2, 0x0a /* Public */,
      47,    0,  358,    2, 0x0a /* Public */,
      48,    0,  359,    2, 0x0a /* Public */,
      49,    0,  360,    2, 0x0a /* Public */,
      50,    0,  361,    2, 0x0a /* Public */,
      51,    0,  362,    2, 0x0a /* Public */,
      52,    1,  363,    2, 0x0a /* Public */,
      54,    1,  366,    2, 0x0a /* Public */,
      54,    0,  369,    2, 0x2a /* Public | MethodCloned */,
      56,    2,  370,    2, 0x0a /* Public */,
      57,    2,  375,    2, 0x0a /* Public */,
      59,    0,  380,    2, 0x0a /* Public */,
      60,    0,  381,    2, 0x0a /* Public */,
      61,    0,  382,    2, 0x0a /* Public */,
      62,    0,  383,    2, 0x0a /* Public */,
      63,    0,  384,    2, 0x0a /* Public */,
      65,    0,  385,    2, 0x0a /* Public */,
      66,    0,  386,    2, 0x0a /* Public */,
      67,    0,  387,    2, 0x0a /* Public */,
      68,    0,  388,    2, 0x0a /* Public */,
      69,    0,  389,    2, 0x0a /* Public */,
      70,    2,  390,    2, 0x0a /* Public */,
      73,    0,  395,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    4,    5,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12,   13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::Bool, QMetaType::QReal, QMetaType::QReal,   33,   34,   35,   36,   37,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal, QMetaType::Bool,   33,   34,   35,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool,   33,   34,   35,
    QMetaType::Void, QMetaType::Int,   41,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   53,
    QMetaType::Void, QMetaType::Bool,   55,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   33,   34,
    QMetaType::Void, 0x80000000 | 58, 0x80000000 | 58,   33,   34,
    QMetaType::QRect,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    0x80000000 | 64,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Int,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   71,   72,
    QMetaType::Void,

       0        // eod
};

void KumKuznec::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KumKuznec *_t = static_cast<KumKuznec *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sync(); break;
        case 1: _t->setSteps((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->setFwdStep((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setBackStep((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: { int _r = _t->stepForward();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { int _r = _t->stepBackward();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: _t->resizeSlot((*reinterpret_cast< QSize*(*)>(_a[1])),(*reinterpret_cast< QSize*(*)>(_a[2]))); break;
        case 7: _t->lockControls(); break;
        case 8: _t->unlockControls(); break;
        case 9: _t->ClearPicture(); break;
        case 10: _t->ToDubl(); break;
        case 11: _t->FromDubl(); break;
        case 12: _t->ToWindow(); break;
        case 13: _t->MoveLeft(); break;
        case 14: _t->MoveRight(); break;
        case 15: _t->MoveUp(); break;
        case 16: _t->MoveDown(); break;
        case 17: _t->ZoomUp(); break;
        case 18: _t->ZoomDown(); break;
        case 19: { bool _r = _t->PrintToPDF();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 20: _t->Close(); break;
        case 21: _t->ToCenter(); break;
        case 22: _t->FindKuznec(); break;
        case 23: _t->Info(); break;
        case 24: _t->MousePress((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< qreal(*)>(_a[4])),(*reinterpret_cast< qreal(*)>(_a[5]))); break;
        case 25: _t->MouseRelease((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 26: _t->MouseMove((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 27: _t->MouseWheel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->HorizWindow(); break;
        case 29: _t->VertWindow(); break;
        case 30: _t->StepXChanged(); break;
        case 31: _t->StepYChanged(); break;
        case 32: _t->NetCancelClick(); break;
        case 33: _t->NetOKClick(); break;
        case 34: _t->InfoOKClick(); break;
        case 35: _t->LoadFromFileActivated(); break;
        case 36: _t->SaveToFileActivated(); break;
        case 37: _t->CloseInst(); break;
        case 38: _t->SwitchMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 39: _t->showHideWindow((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 40: _t->showHideWindow(); break;
        case 41: _t->setWindowPos((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 42: _t->getWindowPos((*reinterpret_cast< int*(*)>(_a[1])),(*reinterpret_cast< int*(*)>(_a[2]))); break;
        case 43: { QRect _r = _t->getWindowGeometry();
            if (_a[0]) *reinterpret_cast< QRect*>(_a[0]) = _r; }  break;
        case 44: _t->MoveFwd(); break;
        case 45: { bool _r = _t->canFwd();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 46: _t->MoveBack(); break;
        case 47: { QWidget* _r = _t->mainWindow();
            if (_a[0]) *reinterpret_cast< QWidget**>(_a[0]) = _r; }  break;
        case 48: { bool _r = _t->canBack();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 49: _t->createFlags(); break;
        case 50: _t->redrawFlags(); break;
        case 51: { int _r = _t->colored();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 52: _t->ColorUncolor(); break;
        case 53: _t->addSled((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 54: _t->AutoClose(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (KumKuznec::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KumKuznec::sync)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KumKuznec::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_KumKuznec.data,
      qt_meta_data_KumKuznec,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumKuznec::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumKuznec::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumKuznec.stringdata0))
        return static_cast<void*>(const_cast< KumKuznec*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int KumKuznec::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 55)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 55;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 55)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 55;
    }
    return _id;
}

// SIGNAL 0
void KumKuznec::sync()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
