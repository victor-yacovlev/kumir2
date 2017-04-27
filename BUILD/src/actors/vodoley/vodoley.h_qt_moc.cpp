/****************************************************************************
** Meta object code from reading C++ file 'vodoley.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/vodoley/vodoley.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vodoley.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NewDialog_t {
    QByteArrayData data[1];
    char stringdata0[10];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NewDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NewDialog_t qt_meta_stringdata_NewDialog = {
    {
QT_MOC_LITERAL(0, 0, 9) // "NewDialog"

    },
    "NewDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NewDialog[] = {

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

void NewDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject NewDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_NewDialog.data,
      qt_meta_data_NewDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NewDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NewDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NewDialog.stringdata0))
        return static_cast<void*>(const_cast< NewDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int NewDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Vodoley_t {
    QByteArrayData data[20];
    char stringdata0[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Vodoley_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Vodoley_t qt_meta_stringdata_Vodoley = {
    {
QT_MOC_LITERAL(0, 0, 7), // "Vodoley"
QT_MOC_LITERAL(1, 8, 5), // "Otkaz"
QT_MOC_LITERAL(2, 14, 0), // ""
QT_MOC_LITERAL(3, 15, 3), // "msg"
QT_MOC_LITERAL(4, 19, 2), // "Ok"
QT_MOC_LITERAL(5, 22, 5), // "CNull"
QT_MOC_LITERAL(6, 28, 8), // "CNotNull"
QT_MOC_LITERAL(7, 37, 10), // "FileLoaded"
QT_MOC_LITERAL(8, 48, 5), // "fname"
QT_MOC_LITERAL(9, 54, 5), // "reset"
QT_MOC_LITERAL(10, 60, 4), // "newZ"
QT_MOC_LITERAL(11, 65, 5), // "loadZ"
QT_MOC_LITERAL(12, 71, 5), // "saveZ"
QT_MOC_LITERAL(13, 77, 5), // "FillA"
QT_MOC_LITERAL(14, 83, 5), // "FillB"
QT_MOC_LITERAL(15, 89, 5), // "FillC"
QT_MOC_LITERAL(16, 95, 10), // "MoveFromTo"
QT_MOC_LITERAL(17, 106, 4), // "from"
QT_MOC_LITERAL(18, 111, 2), // "to"
QT_MOC_LITERAL(19, 114, 6) // "redraw"

    },
    "Vodoley\0Otkaz\0\0msg\0Ok\0CNull\0CNotNull\0"
    "FileLoaded\0fname\0reset\0newZ\0loadZ\0"
    "saveZ\0FillA\0FillB\0FillC\0MoveFromTo\0"
    "from\0to\0redraw"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Vodoley[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,
       4,    0,   87,    2, 0x06 /* Public */,
       5,    0,   88,    2, 0x06 /* Public */,
       6,    0,   89,    2, 0x06 /* Public */,
       7,    1,   90,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   93,    2, 0x0a /* Public */,
      10,    0,   94,    2, 0x0a /* Public */,
      11,    0,   95,    2, 0x0a /* Public */,
      12,    0,   96,    2, 0x0a /* Public */,
      13,    0,   97,    2, 0x0a /* Public */,
      14,    0,   98,    2, 0x0a /* Public */,
      15,    0,   99,    2, 0x0a /* Public */,
      16,    2,  100,    2, 0x0a /* Public */,
      19,    0,  105,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,   17,   18,
    QMetaType::Void,

       0        // eod
};

void Vodoley::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Vodoley *_t = static_cast<Vodoley *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Otkaz((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->Ok(); break;
        case 2: _t->CNull(); break;
        case 3: _t->CNotNull(); break;
        case 4: _t->FileLoaded((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->reset(); break;
        case 6: _t->newZ(); break;
        case 7: _t->loadZ(); break;
        case 8: _t->saveZ(); break;
        case 9: _t->FillA(); break;
        case 10: _t->FillB(); break;
        case 11: _t->FillC(); break;
        case 12: _t->MoveFromTo((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 13: _t->redraw(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Vodoley::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Vodoley::Otkaz)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Vodoley::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Vodoley::Ok)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Vodoley::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Vodoley::CNull)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Vodoley::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Vodoley::CNotNull)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (Vodoley::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Vodoley::FileLoaded)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject Vodoley::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Vodoley.data,
      qt_meta_data_Vodoley,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Vodoley::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Vodoley::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Vodoley.stringdata0))
        return static_cast<void*>(const_cast< Vodoley*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Vodoley::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Vodoley::Otkaz(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Vodoley::Ok()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Vodoley::CNull()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void Vodoley::CNotNull()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void Vodoley::FileLoaded(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
