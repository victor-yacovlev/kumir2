/****************************************************************************
** Meta object code from reading C++ file 'guirun.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/kumircoderun/guirun.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guirun.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_KumirCodeRun__Gui__InputFunctor_t {
    QByteArrayData data[6];
    char stringdata0[76];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumirCodeRun__Gui__InputFunctor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumirCodeRun__Gui__InputFunctor_t qt_meta_stringdata_KumirCodeRun__Gui__InputFunctor = {
    {
QT_MOC_LITERAL(0, 0, 31), // "KumirCodeRun::Gui::InputFunctor"
QT_MOC_LITERAL(1, 32, 12), // "requestInput"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 6), // "format"
QT_MOC_LITERAL(4, 53, 15), // "handleInputDone"
QT_MOC_LITERAL(5, 69, 6) // "values"

    },
    "KumirCodeRun::Gui::InputFunctor\0"
    "requestInput\0\0format\0handleInputDone\0"
    "values"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumirCodeRun__Gui__InputFunctor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   27,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QVariantList,    5,

       0        // eod
};

void KumirCodeRun::Gui::InputFunctor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        InputFunctor *_t = static_cast<InputFunctor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestInput((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->handleInputDone((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (InputFunctor::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&InputFunctor::requestInput)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KumirCodeRun::Gui::InputFunctor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KumirCodeRun__Gui__InputFunctor.data,
      qt_meta_data_KumirCodeRun__Gui__InputFunctor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumirCodeRun::Gui::InputFunctor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumirCodeRun::Gui::InputFunctor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumirCodeRun__Gui__InputFunctor.stringdata0))
        return static_cast<void*>(const_cast< InputFunctor*>(this));
    if (!strcmp(_clname, "VM::InputFunctor"))
        return static_cast< VM::InputFunctor*>(const_cast< InputFunctor*>(this));
    if (!strcmp(_clname, "Kumir::AbstractInputBuffer"))
        return static_cast< Kumir::AbstractInputBuffer*>(const_cast< InputFunctor*>(this));
    return QObject::qt_metacast(_clname);
}

int KumirCodeRun::Gui::InputFunctor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void KumirCodeRun::Gui::InputFunctor::requestInput(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KumirCodeRun__Gui__OutputFunctor_t {
    QByteArrayData data[4];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumirCodeRun__Gui__OutputFunctor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumirCodeRun__Gui__OutputFunctor_t qt_meta_stringdata_KumirCodeRun__Gui__OutputFunctor = {
    {
QT_MOC_LITERAL(0, 0, 32), // "KumirCodeRun::Gui::OutputFunctor"
QT_MOC_LITERAL(1, 33, 13), // "requestOutput"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 4) // "data"

    },
    "KumirCodeRun::Gui::OutputFunctor\0"
    "requestOutput\0\0data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumirCodeRun__Gui__OutputFunctor[] = {

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
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void KumirCodeRun::Gui::OutputFunctor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OutputFunctor *_t = static_cast<OutputFunctor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestOutput((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (OutputFunctor::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OutputFunctor::requestOutput)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KumirCodeRun::Gui::OutputFunctor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KumirCodeRun__Gui__OutputFunctor.data,
      qt_meta_data_KumirCodeRun__Gui__OutputFunctor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumirCodeRun::Gui::OutputFunctor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumirCodeRun::Gui::OutputFunctor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumirCodeRun__Gui__OutputFunctor.stringdata0))
        return static_cast<void*>(const_cast< OutputFunctor*>(this));
    if (!strcmp(_clname, "VM::OutputFunctor"))
        return static_cast< VM::OutputFunctor*>(const_cast< OutputFunctor*>(this));
    if (!strcmp(_clname, "Kumir::AbstractOutputBuffer"))
        return static_cast< Kumir::AbstractOutputBuffer*>(const_cast< OutputFunctor*>(this));
    return QObject::qt_metacast(_clname);
}

int KumirCodeRun::Gui::OutputFunctor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void KumirCodeRun::Gui::OutputFunctor::requestOutput(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KumirCodeRun__Gui__GetMainArgumentFunctor_t {
    QByteArrayData data[8];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumirCodeRun__Gui__GetMainArgumentFunctor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumirCodeRun__Gui__GetMainArgumentFunctor_t qt_meta_stringdata_KumirCodeRun__Gui__GetMainArgumentFunctor = {
    {
QT_MOC_LITERAL(0, 0, 41), // "KumirCodeRun::Gui::GetMainArg..."
QT_MOC_LITERAL(1, 42, 12), // "requestInput"
QT_MOC_LITERAL(2, 55, 0), // ""
QT_MOC_LITERAL(3, 56, 6), // "format"
QT_MOC_LITERAL(4, 63, 13), // "requestOutput"
QT_MOC_LITERAL(5, 77, 4), // "data"
QT_MOC_LITERAL(6, 82, 15), // "handleInputDone"
QT_MOC_LITERAL(7, 98, 6) // "values"

    },
    "KumirCodeRun::Gui::GetMainArgumentFunctor\0"
    "requestInput\0\0format\0requestOutput\0"
    "data\0handleInputDone\0values"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumirCodeRun__Gui__GetMainArgumentFunctor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QVariantList,    7,

       0        // eod
};

void KumirCodeRun::Gui::GetMainArgumentFunctor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GetMainArgumentFunctor *_t = static_cast<GetMainArgumentFunctor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestInput((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->requestOutput((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->handleInputDone((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GetMainArgumentFunctor::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GetMainArgumentFunctor::requestInput)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (GetMainArgumentFunctor::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GetMainArgumentFunctor::requestOutput)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject KumirCodeRun::Gui::GetMainArgumentFunctor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KumirCodeRun__Gui__GetMainArgumentFunctor.data,
      qt_meta_data_KumirCodeRun__Gui__GetMainArgumentFunctor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumirCodeRun::Gui::GetMainArgumentFunctor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumirCodeRun::Gui::GetMainArgumentFunctor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumirCodeRun__Gui__GetMainArgumentFunctor.stringdata0))
        return static_cast<void*>(const_cast< GetMainArgumentFunctor*>(this));
    if (!strcmp(_clname, "VM::GetMainArgumentFunctor"))
        return static_cast< VM::GetMainArgumentFunctor*>(const_cast< GetMainArgumentFunctor*>(this));
    return QObject::qt_metacast(_clname);
}

int KumirCodeRun::Gui::GetMainArgumentFunctor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void KumirCodeRun::Gui::GetMainArgumentFunctor::requestInput(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KumirCodeRun::Gui::GetMainArgumentFunctor::requestOutput(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_KumirCodeRun__Gui__ReturnMainValueFunctor_t {
    QByteArrayData data[4];
    char stringdata0[62];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumirCodeRun__Gui__ReturnMainValueFunctor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumirCodeRun__Gui__ReturnMainValueFunctor_t qt_meta_stringdata_KumirCodeRun__Gui__ReturnMainValueFunctor = {
    {
QT_MOC_LITERAL(0, 0, 41), // "KumirCodeRun::Gui::ReturnMain..."
QT_MOC_LITERAL(1, 42, 13), // "requestOutput"
QT_MOC_LITERAL(2, 56, 0), // ""
QT_MOC_LITERAL(3, 57, 4) // "data"

    },
    "KumirCodeRun::Gui::ReturnMainValueFunctor\0"
    "requestOutput\0\0data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumirCodeRun__Gui__ReturnMainValueFunctor[] = {

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
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void KumirCodeRun::Gui::ReturnMainValueFunctor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ReturnMainValueFunctor *_t = static_cast<ReturnMainValueFunctor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestOutput((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ReturnMainValueFunctor::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ReturnMainValueFunctor::requestOutput)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KumirCodeRun::Gui::ReturnMainValueFunctor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KumirCodeRun__Gui__ReturnMainValueFunctor.data,
      qt_meta_data_KumirCodeRun__Gui__ReturnMainValueFunctor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumirCodeRun::Gui::ReturnMainValueFunctor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumirCodeRun::Gui::ReturnMainValueFunctor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumirCodeRun__Gui__ReturnMainValueFunctor.stringdata0))
        return static_cast<void*>(const_cast< ReturnMainValueFunctor*>(this));
    if (!strcmp(_clname, "VM::ReturnMainValueFunctor"))
        return static_cast< VM::ReturnMainValueFunctor*>(const_cast< ReturnMainValueFunctor*>(this));
    return QObject::qt_metacast(_clname);
}

int KumirCodeRun::Gui::ReturnMainValueFunctor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void KumirCodeRun::Gui::ReturnMainValueFunctor::requestOutput(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KumirCodeRun__Gui__PauseFunctor_t {
    QByteArrayData data[3];
    char stringdata0[46];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumirCodeRun__Gui__PauseFunctor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumirCodeRun__Gui__PauseFunctor_t qt_meta_stringdata_KumirCodeRun__Gui__PauseFunctor = {
    {
QT_MOC_LITERAL(0, 0, 31), // "KumirCodeRun::Gui::PauseFunctor"
QT_MOC_LITERAL(1, 32, 12), // "requestPause"
QT_MOC_LITERAL(2, 45, 0) // ""

    },
    "KumirCodeRun::Gui::PauseFunctor\0"
    "requestPause\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumirCodeRun__Gui__PauseFunctor[] = {

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
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void KumirCodeRun::Gui::PauseFunctor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PauseFunctor *_t = static_cast<PauseFunctor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestPause(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PauseFunctor::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PauseFunctor::requestPause)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject KumirCodeRun::Gui::PauseFunctor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KumirCodeRun__Gui__PauseFunctor.data,
      qt_meta_data_KumirCodeRun__Gui__PauseFunctor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumirCodeRun::Gui::PauseFunctor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumirCodeRun::Gui::PauseFunctor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumirCodeRun__Gui__PauseFunctor.stringdata0))
        return static_cast<void*>(const_cast< PauseFunctor*>(this));
    if (!strcmp(_clname, "VM::PauseFunctor"))
        return static_cast< VM::PauseFunctor*>(const_cast< PauseFunctor*>(this));
    return QObject::qt_metacast(_clname);
}

int KumirCodeRun::Gui::PauseFunctor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void KumirCodeRun::Gui::PauseFunctor::requestPause()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_KumirCodeRun__Gui__DelayFunctor_t {
    QByteArrayData data[1];
    char stringdata0[32];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumirCodeRun__Gui__DelayFunctor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumirCodeRun__Gui__DelayFunctor_t qt_meta_stringdata_KumirCodeRun__Gui__DelayFunctor = {
    {
QT_MOC_LITERAL(0, 0, 31) // "KumirCodeRun::Gui::DelayFunctor"

    },
    "KumirCodeRun::Gui::DelayFunctor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumirCodeRun__Gui__DelayFunctor[] = {

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

void KumirCodeRun::Gui::DelayFunctor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject KumirCodeRun::Gui::DelayFunctor::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_KumirCodeRun__Gui__DelayFunctor.data,
      qt_meta_data_KumirCodeRun__Gui__DelayFunctor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumirCodeRun::Gui::DelayFunctor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumirCodeRun::Gui::DelayFunctor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumirCodeRun__Gui__DelayFunctor.stringdata0))
        return static_cast<void*>(const_cast< DelayFunctor*>(this));
    if (!strcmp(_clname, "VM::DelayFunctor"))
        return static_cast< VM::DelayFunctor*>(const_cast< DelayFunctor*>(this));
    return QThread::qt_metacast(_clname);
}

int KumirCodeRun::Gui::DelayFunctor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_KumirCodeRun__Gui__ExternalModuleResetFunctor_t {
    QByteArrayData data[4];
    char stringdata0[78];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumirCodeRun__Gui__ExternalModuleResetFunctor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumirCodeRun__Gui__ExternalModuleResetFunctor_t qt_meta_stringdata_KumirCodeRun__Gui__ExternalModuleResetFunctor = {
    {
QT_MOC_LITERAL(0, 0, 45), // "KumirCodeRun::Gui::ExternalMo..."
QT_MOC_LITERAL(1, 46, 15), // "showActorWindow"
QT_MOC_LITERAL(2, 62, 0), // ""
QT_MOC_LITERAL(3, 63, 14) // "actorAsciiName"

    },
    "KumirCodeRun::Gui::ExternalModuleResetFunctor\0"
    "showActorWindow\0\0actorAsciiName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumirCodeRun__Gui__ExternalModuleResetFunctor[] = {

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
    QMetaType::Void, QMetaType::QByteArray,    3,

       0        // eod
};

void KumirCodeRun::Gui::ExternalModuleResetFunctor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ExternalModuleResetFunctor *_t = static_cast<ExternalModuleResetFunctor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showActorWindow((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ExternalModuleResetFunctor::*_t)(const QByteArray & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ExternalModuleResetFunctor::showActorWindow)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject KumirCodeRun::Gui::ExternalModuleResetFunctor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KumirCodeRun__Gui__ExternalModuleResetFunctor.data,
      qt_meta_data_KumirCodeRun__Gui__ExternalModuleResetFunctor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumirCodeRun::Gui::ExternalModuleResetFunctor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumirCodeRun::Gui::ExternalModuleResetFunctor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumirCodeRun__Gui__ExternalModuleResetFunctor.stringdata0))
        return static_cast<void*>(const_cast< ExternalModuleResetFunctor*>(this));
    if (!strcmp(_clname, "VM::ExternalModuleResetFunctor"))
        return static_cast< VM::ExternalModuleResetFunctor*>(const_cast< ExternalModuleResetFunctor*>(this));
    return QObject::qt_metacast(_clname);
}

int KumirCodeRun::Gui::ExternalModuleResetFunctor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void KumirCodeRun::Gui::ExternalModuleResetFunctor::showActorWindow(const QByteArray & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
