/****************************************************************************
** Meta object code from reading C++ file 'painterview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/painter/painterview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'painterview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorPainter__PainterView_t {
    QByteArrayData data[6];
    char stringdata0[48];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorPainter__PainterView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorPainter__PainterView_t qt_meta_stringdata_ActorPainter__PainterView = {
    {
QT_MOC_LITERAL(0, 0, 25), // "ActorPainter::PainterView"
QT_MOC_LITERAL(1, 26, 10), // "cursorOver"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 1), // "x"
QT_MOC_LITERAL(4, 40, 1), // "y"
QT_MOC_LITERAL(5, 42, 5) // "color"

    },
    "ActorPainter::PainterView\0cursorOver\0"
    "\0x\0y\0color"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorPainter__PainterView[] = {

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
       1,    3,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QColor,    3,    4,    5,

       0        // eod
};

void ActorPainter::PainterView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PainterView *_t = static_cast<PainterView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cursorOver((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QColor(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PainterView::*_t)(int , int , const QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PainterView::cursorOver)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ActorPainter::PainterView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ActorPainter__PainterView.data,
      qt_meta_data_ActorPainter__PainterView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorPainter::PainterView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorPainter::PainterView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorPainter__PainterView.stringdata0))
        return static_cast<void*>(const_cast< PainterView*>(this));
    return QWidget::qt_metacast(_clname);
}

int ActorPainter::PainterView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void ActorPainter::PainterView::cursorOver(int _t1, int _t2, const QColor & _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
