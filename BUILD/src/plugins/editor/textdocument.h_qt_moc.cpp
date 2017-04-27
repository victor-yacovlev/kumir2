/****************************************************************************
** Meta object code from reading C++ file 'textdocument.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/editor/textdocument.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'textdocument.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor__TextDocument_t {
    QByteArrayData data[6];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor__TextDocument_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor__TextDocument_t qt_meta_stringdata_Editor__TextDocument = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Editor::TextDocument"
QT_MOC_LITERAL(1, 21, 26), // "completeCompilationRequest"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 11), // "visibleText"
QT_MOC_LITERAL(4, 61, 10), // "hiddenText"
QT_MOC_LITERAL(5, 72, 14) // "hiddenBaseLine"

    },
    "Editor::TextDocument\0completeCompilationRequest\0"
    "\0visibleText\0hiddenText\0hiddenBaseLine"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor__TextDocument[] = {

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
    QMetaType::Void, QMetaType::QStringList, QMetaType::QStringList, QMetaType::Int,    3,    4,    5,

       0        // eod
};

void Editor::TextDocument::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TextDocument *_t = static_cast<TextDocument *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->completeCompilationRequest((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TextDocument::*_t)(const QStringList & , const QStringList & , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TextDocument::completeCompilationRequest)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Editor::TextDocument::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Editor__TextDocument.data,
      qt_meta_data_Editor__TextDocument,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Editor::TextDocument::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::TextDocument::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__TextDocument.stringdata0))
        return static_cast<void*>(const_cast< TextDocument*>(this));
    return QObject::qt_metacast(_clname);
}

int Editor::TextDocument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Editor::TextDocument::completeCompilationRequest(const QStringList & _t1, const QStringList & _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
