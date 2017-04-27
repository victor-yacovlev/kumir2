/****************************************************************************
** Meta object code from reading C++ file 'complexnumbersmodule.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/complexnumbers/complexnumbersmodule.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'complexnumbersmodule.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorComplexNumbers__ComplexNumbersModule_t {
    QByteArrayData data[20];
    char stringdata0[253];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorComplexNumbers__ComplexNumbersModule_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorComplexNumbers__ComplexNumbersModule_t qt_meta_stringdata_ActorComplexNumbers__ComplexNumbersModule = {
    {
QT_MOC_LITERAL(0, 0, 41), // "ActorComplexNumbers::ComplexN..."
QT_MOC_LITERAL(1, 42, 5), // "reset"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 19), // "terminateEvaluation"
QT_MOC_LITERAL(4, 69, 19), // "setAnimationEnabled"
QT_MOC_LITERAL(5, 89, 7), // "enabled"
QT_MOC_LITERAL(6, 97, 5), // "runRe"
QT_MOC_LITERAL(7, 103, 7), // "Complex"
QT_MOC_LITERAL(8, 111, 1), // "x"
QT_MOC_LITERAL(9, 113, 5), // "runIm"
QT_MOC_LITERAL(10, 119, 15), // "runOperatorPLUS"
QT_MOC_LITERAL(11, 135, 1), // "y"
QT_MOC_LITERAL(12, 137, 16), // "runOperatorMINUS"
QT_MOC_LITERAL(13, 154, 19), // "runOperatorASTERISK"
QT_MOC_LITERAL(14, 174, 16), // "runOperatorSLASH"
QT_MOC_LITERAL(15, 191, 17), // "runOperatorASSIGN"
QT_MOC_LITERAL(16, 209, 16), // "runOperatorINPUT"
QT_MOC_LITERAL(17, 226, 5), // "bool&"
QT_MOC_LITERAL(18, 232, 2), // "ok"
QT_MOC_LITERAL(19, 235, 17) // "runOperatorOUTPUT"

    },
    "ActorComplexNumbers::ComplexNumbersModule\0"
    "reset\0\0terminateEvaluation\0"
    "setAnimationEnabled\0enabled\0runRe\0"
    "Complex\0x\0runIm\0runOperatorPLUS\0y\0"
    "runOperatorMINUS\0runOperatorASTERISK\0"
    "runOperatorSLASH\0runOperatorASSIGN\0"
    "runOperatorINPUT\0bool&\0ok\0runOperatorOUTPUT"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorComplexNumbers__ComplexNumbersModule[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x0a /* Public */,
       3,    0,   80,    2, 0x0a /* Public */,
       4,    1,   81,    2, 0x0a /* Public */,
       6,    1,   84,    2, 0x0a /* Public */,
       9,    1,   87,    2, 0x0a /* Public */,
      10,    2,   90,    2, 0x0a /* Public */,
      12,    2,   95,    2, 0x0a /* Public */,
      13,    2,  100,    2, 0x0a /* Public */,
      14,    2,  105,    2, 0x0a /* Public */,
      15,    1,  110,    2, 0x0a /* Public */,
      15,    1,  113,    2, 0x0a /* Public */,
      16,    2,  116,    2, 0x0a /* Public */,
      19,    1,  121,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::QReal, 0x80000000 | 7,    8,
    QMetaType::QReal, 0x80000000 | 7,    8,
    0x80000000 | 7, 0x80000000 | 7, 0x80000000 | 7,    8,   11,
    0x80000000 | 7, 0x80000000 | 7, 0x80000000 | 7,    8,   11,
    0x80000000 | 7, 0x80000000 | 7, 0x80000000 | 7,    8,   11,
    0x80000000 | 7, 0x80000000 | 7, 0x80000000 | 7,    8,   11,
    0x80000000 | 7, QMetaType::Int,    8,
    0x80000000 | 7, QMetaType::QReal,    8,
    0x80000000 | 7, QMetaType::QString, 0x80000000 | 17,    8,   18,
    QMetaType::QString, 0x80000000 | 7,    8,

       0        // eod
};

void ActorComplexNumbers::ComplexNumbersModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ComplexNumbersModule *_t = static_cast<ComplexNumbersModule *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->reset(); break;
        case 1: _t->terminateEvaluation(); break;
        case 2: _t->setAnimationEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: { qreal _r = _t->runRe((*reinterpret_cast< const Complex(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 4: { qreal _r = _t->runIm((*reinterpret_cast< const Complex(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 5: { Complex _r = _t->runOperatorPLUS((*reinterpret_cast< const Complex(*)>(_a[1])),(*reinterpret_cast< const Complex(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Complex*>(_a[0]) = _r; }  break;
        case 6: { Complex _r = _t->runOperatorMINUS((*reinterpret_cast< const Complex(*)>(_a[1])),(*reinterpret_cast< const Complex(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Complex*>(_a[0]) = _r; }  break;
        case 7: { Complex _r = _t->runOperatorASTERISK((*reinterpret_cast< const Complex(*)>(_a[1])),(*reinterpret_cast< const Complex(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Complex*>(_a[0]) = _r; }  break;
        case 8: { Complex _r = _t->runOperatorSLASH((*reinterpret_cast< const Complex(*)>(_a[1])),(*reinterpret_cast< const Complex(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Complex*>(_a[0]) = _r; }  break;
        case 9: { Complex _r = _t->runOperatorASSIGN((*reinterpret_cast< const int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Complex*>(_a[0]) = _r; }  break;
        case 10: { Complex _r = _t->runOperatorASSIGN((*reinterpret_cast< const qreal(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Complex*>(_a[0]) = _r; }  break;
        case 11: { Complex _r = _t->runOperatorINPUT((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Complex*>(_a[0]) = _r; }  break;
        case 12: { QString _r = _t->runOperatorOUTPUT((*reinterpret_cast< const Complex(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject ActorComplexNumbers::ComplexNumbersModule::staticMetaObject = {
    { &ComplexNumbersModuleBase::staticMetaObject, qt_meta_stringdata_ActorComplexNumbers__ComplexNumbersModule.data,
      qt_meta_data_ActorComplexNumbers__ComplexNumbersModule,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorComplexNumbers::ComplexNumbersModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorComplexNumbers::ComplexNumbersModule::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorComplexNumbers__ComplexNumbersModule.stringdata0))
        return static_cast<void*>(const_cast< ComplexNumbersModule*>(this));
    return ComplexNumbersModuleBase::qt_metacast(_clname);
}

int ActorComplexNumbers::ComplexNumbersModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ComplexNumbersModuleBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
