/****************************************************************************
** Meta object code from reading C++ file 'draw_turtle_field.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/widgets/draw_turtle_field.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'draw_turtle_field.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DrawNavigator_t {
    QByteArrayData data[10];
    char stringdata0[99];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DrawNavigator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DrawNavigator_t qt_meta_stringdata_DrawNavigator = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DrawNavigator"
QT_MOC_LITERAL(1, 14, 9), // "redrawNet"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 13), // "autoNetChange"
QT_MOC_LITERAL(4, 39, 5), // "value"
QT_MOC_LITERAL(5, 45, 13), // "netStepChange"
QT_MOC_LITERAL(6, 59, 12), // "XvalueChange"
QT_MOC_LITERAL(7, 72, 12), // "YvalueChange"
QT_MOC_LITERAL(8, 85, 7), // "autoNet"
QT_MOC_LITERAL(9, 93, 5) // "state"

    },
    "DrawNavigator\0redrawNet\0\0autoNetChange\0"
    "value\0netStepChange\0XvalueChange\0"
    "YvalueChange\0autoNet\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DrawNavigator[] = {

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
       3,    1,   45,    2, 0x06 /* Public */,
       5,    1,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   51,    2, 0x0a /* Public */,
       7,    1,   54,    2, 0x0a /* Public */,
       8,    1,   57,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Double,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    4,
    QMetaType::Void, QMetaType::Double,    4,
    QMetaType::Void, QMetaType::Int,    9,

       0        // eod
};

void DrawNavigator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DrawNavigator *_t = static_cast<DrawNavigator *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->redrawNet(); break;
        case 1: _t->autoNetChange((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->netStepChange((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->XvalueChange((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->YvalueChange((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->autoNet((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DrawNavigator::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DrawNavigator::redrawNet)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DrawNavigator::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DrawNavigator::autoNetChange)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (DrawNavigator::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DrawNavigator::netStepChange)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject DrawNavigator::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_DrawNavigator.data,
      qt_meta_data_DrawNavigator,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DrawNavigator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DrawNavigator::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DrawNavigator.stringdata0))
        return static_cast<void*>(const_cast< DrawNavigator*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int DrawNavigator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
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
void DrawNavigator::redrawNet()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void DrawNavigator::autoNetChange(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DrawNavigator::netStepChange(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
