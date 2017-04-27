/****************************************************************************
** Meta object code from reading C++ file 'robot25dwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/isometricrobot/robot25dwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'robot25dwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Robot25DWindow_t {
    QByteArrayData data[9];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Robot25DWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Robot25DWindow_t qt_meta_stringdata_Robot25DWindow = {
    {
QT_MOC_LITERAL(0, 0, 14), // "Robot25DWindow"
QT_MOC_LITERAL(1, 15, 13), // "resizeRequest"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 2), // "sz"
QT_MOC_LITERAL(4, 33, 16), // "handleLoadAction"
QT_MOC_LITERAL(5, 50, 16), // "handleNextAction"
QT_MOC_LITERAL(6, 67, 16), // "handlePrevAction"
QT_MOC_LITERAL(7, 84, 12), // "setTaskIndex"
QT_MOC_LITERAL(8, 97, 5) // "index"

    },
    "Robot25DWindow\0resizeRequest\0\0sz\0"
    "handleLoadAction\0handleNextAction\0"
    "handlePrevAction\0setTaskIndex\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Robot25DWindow[] = {

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
       4,    0,   42,    2, 0x08 /* Private */,
       5,    0,   43,    2, 0x08 /* Private */,
       6,    0,   44,    2, 0x08 /* Private */,
       7,    1,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QSize,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,

       0        // eod
};

void Robot25DWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Robot25DWindow *_t = static_cast<Robot25DWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resizeRequest((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 1: _t->handleLoadAction(); break;
        case 2: _t->handleNextAction(); break;
        case 3: _t->handlePrevAction(); break;
        case 4: _t->setTaskIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Robot25DWindow::*_t)(const QSize & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Robot25DWindow::resizeRequest)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Robot25DWindow::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_Robot25DWindow.data,
      qt_meta_data_Robot25DWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Robot25DWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Robot25DWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Robot25DWindow.stringdata0))
        return static_cast<void*>(const_cast< Robot25DWindow*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int Robot25DWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Robot25DWindow::resizeRequest(const QSize & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
