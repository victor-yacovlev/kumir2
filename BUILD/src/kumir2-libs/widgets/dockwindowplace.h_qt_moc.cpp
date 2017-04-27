/****************************************************************************
** Meta object code from reading C++ file 'dockwindowplace.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/widgets/dockwindowplace.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dockwindowplace.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Widgets__DockWindowPlace_t {
    QByteArrayData data[10];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widgets__DockWindowPlace_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widgets__DockWindowPlace_t qt_meta_stringdata_Widgets__DockWindowPlace = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Widgets::DockWindowPlace"
QT_MOC_LITERAL(1, 25, 16), // "visiblityRequest"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 7), // "visible"
QT_MOC_LITERAL(4, 51, 4), // "size"
QT_MOC_LITERAL(5, 56, 13), // "resizeRequest"
QT_MOC_LITERAL(6, 70, 8), // "activate"
QT_MOC_LITERAL(7, 79, 8), // "prefSize"
QT_MOC_LITERAL(8, 88, 13), // "processResize"
QT_MOC_LITERAL(9, 102, 2) // "sz"

    },
    "Widgets::DockWindowPlace\0visiblityRequest\0"
    "\0visible\0size\0resizeRequest\0activate\0"
    "prefSize\0processResize\0sz"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widgets__DockWindowPlace[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   42,    2, 0x0a /* Public */,
       8,    1,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QSize,    3,    4,
    QMetaType::Void, QMetaType::QSize,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QSize,    7,
    QMetaType::Void, QMetaType::QSize,    9,

       0        // eod
};

void Widgets::DockWindowPlace::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DockWindowPlace *_t = static_cast<DockWindowPlace *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->visiblityRequest((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QSize(*)>(_a[2]))); break;
        case 1: _t->resizeRequest((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 2: _t->activate((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 3: _t->processResize((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DockWindowPlace::*_t)(bool , const QSize & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DockWindowPlace::visiblityRequest)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (DockWindowPlace::*_t)(const QSize & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DockWindowPlace::resizeRequest)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject Widgets::DockWindowPlace::staticMetaObject = {
    { &QTabWidget::staticMetaObject, qt_meta_stringdata_Widgets__DockWindowPlace.data,
      qt_meta_data_Widgets__DockWindowPlace,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widgets::DockWindowPlace::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widgets::DockWindowPlace::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widgets__DockWindowPlace.stringdata0))
        return static_cast<void*>(const_cast< DockWindowPlace*>(this));
    return QTabWidget::qt_metacast(_clname);
}

int Widgets::DockWindowPlace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
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
void Widgets::DockWindowPlace::visiblityRequest(bool _t1, const QSize & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Widgets::DockWindowPlace::resizeRequest(const QSize & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
