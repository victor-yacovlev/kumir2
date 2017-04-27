/****************************************************************************
** Meta object code from reading C++ file 'secondarywindow_generic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/widgets/secondarywindow_generic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'secondarywindow_generic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Widgets__SecondaryWindowGenericImplementation_t {
    QByteArrayData data[12];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widgets__SecondaryWindowGenericImplementation_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widgets__SecondaryWindowGenericImplementation_t qt_meta_stringdata_Widgets__SecondaryWindowGenericImplementation = {
    {
QT_MOC_LITERAL(0, 0, 45), // "Widgets::SecondaryWindowGener..."
QT_MOC_LITERAL(1, 46, 10), // "deactivate"
QT_MOC_LITERAL(2, 57, 0), // ""
QT_MOC_LITERAL(3, 58, 8), // "activate"
QT_MOC_LITERAL(4, 67, 2), // "ps"
QT_MOC_LITERAL(5, 70, 2), // "sz"
QT_MOC_LITERAL(6, 73, 19), // "handleResizeRequest"
QT_MOC_LITERAL(7, 93, 11), // "closeWindow"
QT_MOC_LITERAL(8, 105, 14), // "minimizeWindow"
QT_MOC_LITERAL(9, 120, 15), // "toggleStayOnTop"
QT_MOC_LITERAL(10, 136, 1), // "v"
QT_MOC_LITERAL(11, 138, 16) // "toggleDockWindow"

    },
    "Widgets::SecondaryWindowGenericImplementation\0"
    "deactivate\0\0activate\0ps\0sz\0"
    "handleResizeRequest\0closeWindow\0"
    "minimizeWindow\0toggleStayOnTop\0v\0"
    "toggleDockWindow"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widgets__SecondaryWindowGenericImplementation[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    2,   50,    2, 0x0a /* Public */,
       6,    1,   55,    2, 0x0a /* Public */,
       7,    0,   58,    2, 0x09 /* Protected */,
       8,    0,   59,    2, 0x09 /* Protected */,
       9,    1,   60,    2, 0x09 /* Protected */,
      11,    0,   63,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QSize,    4,    5,
    QMetaType::Void, QMetaType::QSize,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,

       0        // eod
};

void Widgets::SecondaryWindowGenericImplementation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SecondaryWindowGenericImplementation *_t = static_cast<SecondaryWindowGenericImplementation *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deactivate(); break;
        case 1: _t->activate((*reinterpret_cast< const QPoint(*)>(_a[1])),(*reinterpret_cast< const QSize(*)>(_a[2]))); break;
        case 2: _t->handleResizeRequest((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 3: _t->closeWindow(); break;
        case 4: _t->minimizeWindow(); break;
        case 5: _t->toggleStayOnTop((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->toggleDockWindow(); break;
        default: ;
        }
    }
}

const QMetaObject Widgets::SecondaryWindowGenericImplementation::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Widgets__SecondaryWindowGenericImplementation.data,
      qt_meta_data_Widgets__SecondaryWindowGenericImplementation,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widgets::SecondaryWindowGenericImplementation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widgets::SecondaryWindowGenericImplementation::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widgets__SecondaryWindowGenericImplementation.stringdata0))
        return static_cast<void*>(const_cast< SecondaryWindowGenericImplementation*>(this));
    if (!strcmp(_clname, "SecondaryWindowImplementationInterface"))
        return static_cast< SecondaryWindowImplementationInterface*>(const_cast< SecondaryWindowGenericImplementation*>(this));
    return QWidget::qt_metacast(_clname);
}

int Widgets::SecondaryWindowGenericImplementation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
