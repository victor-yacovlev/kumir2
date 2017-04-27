/****************************************************************************
** Meta object code from reading C++ file 'pdautomata.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/kumiranalizer/pdautomata.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pdautomata.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_KumirAnalizer__PDAutomata_t {
    QByteArrayData data[51];
    char stringdata0[928];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KumirAnalizer__PDAutomata_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KumirAnalizer__PDAutomata_t qt_meta_stringdata_KumirAnalizer__PDAutomata = {
    {
QT_MOC_LITERAL(0, 0, 25), // "KumirAnalizer::PDAutomata"
QT_MOC_LITERAL(1, 26, 17), // "addDummyAlgHeader"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 15), // "setCurrentError"
QT_MOC_LITERAL(4, 61, 5), // "value"
QT_MOC_LITERAL(5, 67, 28), // "setCurrentErrorRaisePosition"
QT_MOC_LITERAL(6, 96, 25), // "Lexem::ErrorRaisePosition"
QT_MOC_LITERAL(7, 122, 3), // "pos"
QT_MOC_LITERAL(8, 126, 16), // "setOutOfAlgError"
QT_MOC_LITERAL(9, 143, 19), // "setModuleBeginError"
QT_MOC_LITERAL(10, 163, 20), // "setCurrentIndentRank"
QT_MOC_LITERAL(11, 184, 5), // "start"
QT_MOC_LITERAL(12, 190, 3), // "end"
QT_MOC_LITERAL(13, 194, 23), // "processCorrectEndOfLoop"
QT_MOC_LITERAL(14, 218, 29), // "processAlgEndInsteadOfLoopEnd"
QT_MOC_LITERAL(15, 248, 18), // "processCorrectCase"
QT_MOC_LITERAL(16, 267, 16), // "processCorrectIf"
QT_MOC_LITERAL(17, 284, 18), // "processCorrectThen"
QT_MOC_LITERAL(18, 303, 29), // "processCorrectThenIfNotExists"
QT_MOC_LITERAL(19, 333, 30), // "processCorrectThenIfNotExists2"
QT_MOC_LITERAL(20, 364, 16), // "processCorrectFi"
QT_MOC_LITERAL(21, 381, 18), // "processCorrectElse"
QT_MOC_LITERAL(22, 400, 20), // "processCorrectSwitch"
QT_MOC_LITERAL(23, 421, 25), // "processCorrectBeginOfLoop"
QT_MOC_LITERAL(24, 447, 23), // "processCorrectAlgHeader"
QT_MOC_LITERAL(25, 471, 22), // "processCorrectAlgBegin"
QT_MOC_LITERAL(26, 494, 21), // "processCorrectDocLine"
QT_MOC_LITERAL(27, 516, 29), // "processCorrectRestrictionLine"
QT_MOC_LITERAL(28, 546, 20), // "processCorrectAlgEnd"
QT_MOC_LITERAL(29, 567, 18), // "processCorrectLoad"
QT_MOC_LITERAL(30, 586, 16), // "appendSimpleLine"
QT_MOC_LITERAL(31, 603, 21), // "processAlgWithNoBegin"
QT_MOC_LITERAL(32, 625, 18), // "setGarbageAlgError"
QT_MOC_LITERAL(33, 644, 21), // "setGarbageIfThenError"
QT_MOC_LITERAL(34, 666, 25), // "setGarbageSwitchCaseError"
QT_MOC_LITERAL(35, 692, 24), // "setCorrespondingIfBroken"
QT_MOC_LITERAL(36, 717, 24), // "setExtraOpenKeywordError"
QT_MOC_LITERAL(37, 742, 2), // "kw"
QT_MOC_LITERAL(38, 745, 25), // "setExtraCloseKeywordError"
QT_MOC_LITERAL(39, 771, 7), // "suggest"
QT_MOC_LITERAL(40, 779, 4), // "text"
QT_MOC_LITERAL(41, 784, 24), // "moveCursorBackLinesCount"
QT_MOC_LITERAL(42, 809, 16), // "setTooManyErrors"
QT_MOC_LITERAL(43, 826, 21), // "finalizeIterativeRule"
QT_MOC_LITERAL(44, 848, 11), // "PDStackElem"
QT_MOC_LITERAL(45, 860, 9), // "stackElem"
QT_MOC_LITERAL(46, 870, 8), // "nextStep"
QT_MOC_LITERAL(47, 879, 8), // "saveData"
QT_MOC_LITERAL(48, 888, 11), // "restoreData"
QT_MOC_LITERAL(49, 900, 10), // "popHistory"
QT_MOC_LITERAL(50, 911, 16) // "clearDataHistory"

    },
    "KumirAnalizer::PDAutomata\0addDummyAlgHeader\0"
    "\0setCurrentError\0value\0"
    "setCurrentErrorRaisePosition\0"
    "Lexem::ErrorRaisePosition\0pos\0"
    "setOutOfAlgError\0setModuleBeginError\0"
    "setCurrentIndentRank\0start\0end\0"
    "processCorrectEndOfLoop\0"
    "processAlgEndInsteadOfLoopEnd\0"
    "processCorrectCase\0processCorrectIf\0"
    "processCorrectThen\0processCorrectThenIfNotExists\0"
    "processCorrectThenIfNotExists2\0"
    "processCorrectFi\0processCorrectElse\0"
    "processCorrectSwitch\0processCorrectBeginOfLoop\0"
    "processCorrectAlgHeader\0processCorrectAlgBegin\0"
    "processCorrectDocLine\0"
    "processCorrectRestrictionLine\0"
    "processCorrectAlgEnd\0processCorrectLoad\0"
    "appendSimpleLine\0processAlgWithNoBegin\0"
    "setGarbageAlgError\0setGarbageIfThenError\0"
    "setGarbageSwitchCaseError\0"
    "setCorrespondingIfBroken\0"
    "setExtraOpenKeywordError\0kw\0"
    "setExtraCloseKeywordError\0suggest\0"
    "text\0moveCursorBackLinesCount\0"
    "setTooManyErrors\0finalizeIterativeRule\0"
    "PDStackElem\0stackElem\0nextStep\0saveData\0"
    "restoreData\0popHistory\0clearDataHistory"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KumirAnalizer__PDAutomata[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      39,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  209,    2, 0x08 /* Private */,
       3,    1,  210,    2, 0x08 /* Private */,
       5,    1,  213,    2, 0x08 /* Private */,
       8,    0,  216,    2, 0x08 /* Private */,
       9,    1,  217,    2, 0x08 /* Private */,
      10,    2,  220,    2, 0x08 /* Private */,
      13,    0,  225,    2, 0x08 /* Private */,
      14,    0,  226,    2, 0x08 /* Private */,
      15,    0,  227,    2, 0x08 /* Private */,
      16,    0,  228,    2, 0x08 /* Private */,
      17,    0,  229,    2, 0x08 /* Private */,
      18,    0,  230,    2, 0x08 /* Private */,
      19,    0,  231,    2, 0x08 /* Private */,
      20,    0,  232,    2, 0x08 /* Private */,
      21,    0,  233,    2, 0x08 /* Private */,
      22,    0,  234,    2, 0x08 /* Private */,
      23,    0,  235,    2, 0x08 /* Private */,
      24,    0,  236,    2, 0x08 /* Private */,
      25,    0,  237,    2, 0x08 /* Private */,
      26,    0,  238,    2, 0x08 /* Private */,
      27,    0,  239,    2, 0x08 /* Private */,
      28,    0,  240,    2, 0x08 /* Private */,
      29,    0,  241,    2, 0x08 /* Private */,
      30,    0,  242,    2, 0x08 /* Private */,
      31,    0,  243,    2, 0x08 /* Private */,
      32,    0,  244,    2, 0x08 /* Private */,
      33,    0,  245,    2, 0x08 /* Private */,
      34,    0,  246,    2, 0x08 /* Private */,
      35,    0,  247,    2, 0x08 /* Private */,
      36,    1,  248,    2, 0x08 /* Private */,
      38,    1,  251,    2, 0x08 /* Private */,
      39,    2,  254,    2, 0x08 /* Private */,
      42,    0,  259,    2, 0x08 /* Private */,
      43,    1,  260,    2, 0x08 /* Private */,
      46,    0,  263,    2, 0x08 /* Private */,
      47,    0,  264,    2, 0x08 /* Private */,
      48,    0,  265,    2, 0x08 /* Private */,
      49,    0,  266,    2, 0x08 /* Private */,
      50,    0,  267,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   37,
    QMetaType::Void, QMetaType::QString,   37,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   40,   41,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 44,   45,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KumirAnalizer::PDAutomata::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PDAutomata *_t = static_cast<PDAutomata *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addDummyAlgHeader(); break;
        case 1: _t->setCurrentError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->setCurrentErrorRaisePosition((*reinterpret_cast< Lexem::ErrorRaisePosition(*)>(_a[1]))); break;
        case 3: _t->setOutOfAlgError(); break;
        case 4: _t->setModuleBeginError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->setCurrentIndentRank((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->processCorrectEndOfLoop(); break;
        case 7: _t->processAlgEndInsteadOfLoopEnd(); break;
        case 8: _t->processCorrectCase(); break;
        case 9: _t->processCorrectIf(); break;
        case 10: _t->processCorrectThen(); break;
        case 11: _t->processCorrectThenIfNotExists(); break;
        case 12: _t->processCorrectThenIfNotExists2(); break;
        case 13: _t->processCorrectFi(); break;
        case 14: _t->processCorrectElse(); break;
        case 15: _t->processCorrectSwitch(); break;
        case 16: _t->processCorrectBeginOfLoop(); break;
        case 17: _t->processCorrectAlgHeader(); break;
        case 18: _t->processCorrectAlgBegin(); break;
        case 19: _t->processCorrectDocLine(); break;
        case 20: _t->processCorrectRestrictionLine(); break;
        case 21: _t->processCorrectAlgEnd(); break;
        case 22: _t->processCorrectLoad(); break;
        case 23: _t->appendSimpleLine(); break;
        case 24: _t->processAlgWithNoBegin(); break;
        case 25: _t->setGarbageAlgError(); break;
        case 26: _t->setGarbageIfThenError(); break;
        case 27: _t->setGarbageSwitchCaseError(); break;
        case 28: _t->setCorrespondingIfBroken(); break;
        case 29: _t->setExtraOpenKeywordError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 30: _t->setExtraCloseKeywordError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 31: _t->suggest((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 32: _t->setTooManyErrors(); break;
        case 33: _t->finalizeIterativeRule((*reinterpret_cast< const PDStackElem(*)>(_a[1]))); break;
        case 34: _t->nextStep(); break;
        case 35: _t->saveData(); break;
        case 36: _t->restoreData(); break;
        case 37: _t->popHistory(); break;
        case 38: _t->clearDataHistory(); break;
        default: ;
        }
    }
}

const QMetaObject KumirAnalizer::PDAutomata::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KumirAnalizer__PDAutomata.data,
      qt_meta_data_KumirAnalizer__PDAutomata,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *KumirAnalizer::PDAutomata::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KumirAnalizer::PDAutomata::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_KumirAnalizer__PDAutomata.stringdata0))
        return static_cast<void*>(const_cast< PDAutomata*>(this));
    return QObject::qt_metacast(_clname);
}

int KumirAnalizer::PDAutomata::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 39)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 39;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 39)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 39;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
