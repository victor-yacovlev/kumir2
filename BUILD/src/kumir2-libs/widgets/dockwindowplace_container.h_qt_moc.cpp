/****************************************************************************
** Meta object code from reading C++ file 'dockwindowplace_container.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/widgets/dockwindowplace_container.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dockwindowplace_container.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Widgets__DockWindowPlaceContainer_t {
    QByteArrayData data[9];
    char stringdata0[102];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widgets__DockWindowPlaceContainer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widgets__DockWindowPlaceContainer_t qt_meta_stringdata_Widgets__DockWindowPlaceContainer = {
    {
QT_MOC_LITERAL(0, 0, 33), // "Widgets::DockWindowPlaceConta..."
QT_MOC_LITERAL(1, 34, 13), // "resizeRequest"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 4), // "size"
QT_MOC_LITERAL(4, 54, 8), // "activate"
QT_MOC_LITERAL(5, 63, 2), // "ps"
QT_MOC_LITERAL(6, 66, 2), // "sz"
QT_MOC_LITERAL(7, 69, 10), // "deactivate"
QT_MOC_LITERAL(8, 80, 21) // "resizePlaceToLastSize"

    },
    "Widgets::DockWindowPlaceContainer\0"
    "resizeRequest\0\0size\0activate\0ps\0sz\0"
    "deactivate\0resizePlaceToLastSize"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widgets__DockWindowPlaceContainer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,   37,    2, 0x0a /* Public */,
       7,    0,   42,    2, 0x0a /* Public */,
       8,    0,   43,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint, QMetaType::QSize,    5,    6,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Widgets::DockWindowPlaceContainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DockWindowPlaceContainer *_t = static_cast<DockWindowPlaceContainer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resizeRequest((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->activate((*reinterpret_cast< const QPoint(*)>(_a[1])),(*reinterpret_cast< const QSize(*)>(_a[2]))); break;
        case 2: _t->deactivate(); break;
        case 3: _t->resizePlaceToLastSize(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DockWindowPlaceContainer::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DockWindowPlaceContainer::resizeRequest)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Widgets::DockWindowPlaceContainer::staticMetaObject = {
    { &SecondaryWindowGenericImplementation::staticMetaObject, qt_meta_stringdata_Widgets__DockWindowPlaceContainer.data,
      qt_meta_data_Widgets__DockWindowPlaceContainer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widgets::DockWindowPlaceContainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widgets::DockWindowPlaceContainer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widgets__DockWindowPlaceContainer.stringdata0))
        return static_cast<void*>(const_cast< DockWindowPlaceContainer*>(this));
    return SecondaryWindowGenericImplementation::qt_metacast(_clname);
}

int Widgets::DockWindowPlaceContainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SecondaryWindowGenericImplementation::qt_metacall(_c, _id, _a);
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
void Widgets::DockWindowPlaceContainer::resizeRequest(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
