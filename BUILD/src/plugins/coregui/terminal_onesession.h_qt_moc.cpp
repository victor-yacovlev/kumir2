/****************************************************************************
** Meta object code from reading C++ file 'terminal_onesession.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/coregui/terminal_onesession.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'terminal_onesession.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Terminal__OneSession_t {
    QByteArrayData data[19];
    char stringdata0[174];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Terminal__OneSession_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Terminal__OneSession_t qt_meta_stringdata_Terminal__OneSession = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Terminal::OneSession"
QT_MOC_LITERAL(1, 21, 13), // "updateRequest"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 7), // "message"
QT_MOC_LITERAL(4, 44, 3), // "txt"
QT_MOC_LITERAL(5, 48, 9), // "inputDone"
QT_MOC_LITERAL(6, 58, 6), // "output"
QT_MOC_LITERAL(7, 65, 4), // "text"
QT_MOC_LITERAL(8, 70, 8), // "CharSpec"
QT_MOC_LITERAL(9, 79, 2), // "cs"
QT_MOC_LITERAL(10, 82, 5), // "input"
QT_MOC_LITERAL(11, 88, 6), // "format"
QT_MOC_LITERAL(12, 95, 5), // "error"
QT_MOC_LITERAL(13, 101, 6), // "finish"
QT_MOC_LITERAL(14, 108, 9), // "terminate"
QT_MOC_LITERAL(15, 118, 14), // "tryFinishInput"
QT_MOC_LITERAL(16, 133, 20), // "changeCursorPosition"
QT_MOC_LITERAL(17, 154, 3), // "pos"
QT_MOC_LITERAL(18, 158, 15) // "changeInputText"

    },
    "Terminal::OneSession\0updateRequest\0\0"
    "message\0txt\0inputDone\0output\0text\0"
    "CharSpec\0cs\0input\0format\0error\0finish\0"
    "terminate\0tryFinishInput\0changeCursorPosition\0"
    "pos\0changeInputText"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Terminal__OneSession[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    1,   70,    2, 0x06 /* Public */,
       5,    1,   73,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   76,    2, 0x0a /* Public */,
      10,    1,   81,    2, 0x0a /* Public */,
      12,    1,   84,    2, 0x0a /* Public */,
      13,    0,   87,    2, 0x0a /* Public */,
      14,    0,   88,    2, 0x0a /* Public */,
      15,    0,   89,    2, 0x0a /* Public */,
      16,    1,   90,    2, 0x0a /* Public */,
      18,    1,   93,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QVariantList,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 8,    7,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UShort,   17,
    QMetaType::Void, QMetaType::QString,    7,

       0        // eod
};

void Terminal::OneSession::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OneSession *_t = static_cast<OneSession *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateRequest(); break;
        case 1: _t->message((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->inputDone((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 3: _t->output((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const CharSpec(*)>(_a[2]))); break;
        case 4: _t->input((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->error((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->finish(); break;
        case 7: _t->terminate(); break;
        case 8: _t->tryFinishInput(); break;
        case 9: _t->changeCursorPosition((*reinterpret_cast< quint16(*)>(_a[1]))); break;
        case 10: _t->changeInputText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (OneSession::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OneSession::updateRequest)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (OneSession::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OneSession::message)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (OneSession::*_t)(const QVariantList & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OneSession::inputDone)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject Terminal::OneSession::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Terminal__OneSession.data,
      qt_meta_data_Terminal__OneSession,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Terminal::OneSession::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Terminal::OneSession::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Terminal__OneSession.stringdata0))
        return static_cast<void*>(const_cast< OneSession*>(this));
    return QObject::qt_metacast(_clname);
}

int Terminal::OneSession::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Terminal::OneSession::updateRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Terminal::OneSession::message(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Terminal::OneSession::inputDone(const QVariantList & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
