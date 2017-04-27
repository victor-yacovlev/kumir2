/****************************************************************************
** Meta object code from reading C++ file 'lexer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/kumiranalizer/lexer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lexer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_KumirAnalizer__Lexer_t {
    QByteArrayData data[12];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumirAnalizer__Lexer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumirAnalizer__Lexer_t qt_meta_stringdata_KumirAnalizer__Lexer = {
    {
QT_MOC_LITERAL(0, 0, 20), // "KumirAnalizer::Lexer"
QT_MOC_LITERAL(1, 21, 19), // "splitIntoStatements"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 5), // "lines"
QT_MOC_LITERAL(4, 48, 10), // "baseLineNo"
QT_MOC_LITERAL(5, 59, 24), // "QList<TextStatementPtr>&"
QT_MOC_LITERAL(6, 84, 10), // "statements"
QT_MOC_LITERAL(7, 95, 14), // "extraTypeNames"
QT_MOC_LITERAL(8, 110, 15), // "splitIntoLexems"
QT_MOC_LITERAL(9, 126, 4), // "text"
QT_MOC_LITERAL(10, 131, 16), // "QList<LexemPtr>&"
QT_MOC_LITERAL(11, 148, 6) // "lexems"

    },
    "KumirAnalizer::Lexer\0splitIntoStatements\0"
    "\0lines\0baseLineNo\0QList<TextStatementPtr>&\0"
    "statements\0extraTypeNames\0splitIntoLexems\0"
    "text\0QList<LexemPtr>&\0lexems"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumirAnalizer__Lexer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    4,   24,    2, 0x0a /* Public */,
       8,    3,   33,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Int, QMetaType::QStringList, QMetaType::Int, 0x80000000 | 5, QMetaType::QStringList,    3,    4,    6,    7,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 10, QMetaType::QStringList,    9,   11,    7,

       0        // eod
};

void KumirAnalizer::Lexer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Lexer *_t = static_cast<Lexer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { int _r = _t->splitIntoStatements((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QList<TextStatementPtr>(*)>(_a[3])),(*reinterpret_cast< const QStringList(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: _t->splitIntoLexems((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QList<LexemPtr>(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject KumirAnalizer::Lexer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KumirAnalizer__Lexer.data,
      qt_meta_data_KumirAnalizer__Lexer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumirAnalizer::Lexer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumirAnalizer::Lexer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumirAnalizer__Lexer.stringdata0))
        return static_cast<void*>(const_cast< Lexer*>(this));
    return QObject::qt_metacast(_clname);
}

int KumirAnalizer::Lexer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
