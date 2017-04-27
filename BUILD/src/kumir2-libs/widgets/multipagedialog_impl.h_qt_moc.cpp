/****************************************************************************
** Meta object code from reading C++ file 'multipagedialog_impl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/widgets/multipagedialog_impl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'multipagedialog_impl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Widgets__MultiPageDialogImpl_t {
    QByteArrayData data[9];
    char stringdata0[118];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widgets__MultiPageDialogImpl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widgets__MultiPageDialogImpl_t qt_meta_stringdata_Widgets__MultiPageDialogImpl = {
    {
QT_MOC_LITERAL(0, 0, 28), // "Widgets::MultiPageDialogImpl"
QT_MOC_LITERAL(1, 29, 19), // "handleGroupSelected"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 67, 7), // "current"
QT_MOC_LITERAL(5, 75, 8), // "previous"
QT_MOC_LITERAL(6, 84, 14), // "acceptAllPages"
QT_MOC_LITERAL(7, 99, 13), // "resetAllPages"
QT_MOC_LITERAL(8, 113, 4) // "init"

    },
    "Widgets::MultiPageDialogImpl\0"
    "handleGroupSelected\0\0QTreeWidgetItem*\0"
    "current\0previous\0acceptAllPages\0"
    "resetAllPages\0init"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widgets__MultiPageDialogImpl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x08 /* Private */,
       6,    0,   39,    2, 0x08 /* Private */,
       7,    0,   40,    2, 0x08 /* Private */,
       8,    0,   41,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Widgets::MultiPageDialogImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MultiPageDialogImpl *_t = static_cast<MultiPageDialogImpl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->handleGroupSelected((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->acceptAllPages(); break;
        case 2: _t->resetAllPages(); break;
        case 3: _t->init(); break;
        default: ;
        }
    }
}

const QMetaObject Widgets::MultiPageDialogImpl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Widgets__MultiPageDialogImpl.data,
      qt_meta_data_Widgets__MultiPageDialogImpl,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widgets::MultiPageDialogImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widgets::MultiPageDialogImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widgets__MultiPageDialogImpl.stringdata0))
        return static_cast<void*>(const_cast< MultiPageDialogImpl*>(this));
    return QObject::qt_metacast(_clname);
}

int Widgets::MultiPageDialogImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
