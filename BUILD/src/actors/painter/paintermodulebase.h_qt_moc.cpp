/****************************************************************************
** Meta object code from reading C++ file 'paintermodulebase.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "paintermodulebase.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'paintermodulebase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorPainter__PainterModuleBase_t {
    QByteArrayData data[89];
    char stringdata0[810];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorPainter__PainterModuleBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorPainter__PainterModuleBase_t qt_meta_stringdata_ActorPainter__PainterModuleBase = {
    {
QT_MOC_LITERAL(0, 0, 31), // "ActorPainter::PainterModuleBase"
QT_MOC_LITERAL(1, 32, 33), // "notifyOnTemplateParametersCha..."
QT_MOC_LITERAL(2, 66, 0), // ""
QT_MOC_LITERAL(3, 67, 5), // "reset"
QT_MOC_LITERAL(4, 73, 14), // "reloadSettings"
QT_MOC_LITERAL(5, 88, 28), // "ExtensionSystem::SettingsPtr"
QT_MOC_LITERAL(6, 117, 8), // "settings"
QT_MOC_LITERAL(7, 126, 4), // "keys"
QT_MOC_LITERAL(8, 131, 17), // "changeGlobalState"
QT_MOC_LITERAL(9, 149, 28), // "ExtensionSystem::GlobalState"
QT_MOC_LITERAL(10, 178, 3), // "old"
QT_MOC_LITERAL(11, 182, 7), // "current"
QT_MOC_LITERAL(12, 190, 19), // "terminateEvaluation"
QT_MOC_LITERAL(13, 210, 19), // "setAnimationEnabled"
QT_MOC_LITERAL(14, 230, 7), // "enabled"
QT_MOC_LITERAL(15, 238, 7), // "runCMYK"
QT_MOC_LITERAL(16, 246, 5), // "Color"
QT_MOC_LITERAL(17, 252, 1), // "c"
QT_MOC_LITERAL(18, 254, 1), // "m"
QT_MOC_LITERAL(19, 256, 1), // "y"
QT_MOC_LITERAL(20, 258, 1), // "k"
QT_MOC_LITERAL(21, 260, 8), // "runCMYKA"
QT_MOC_LITERAL(22, 269, 1), // "a"
QT_MOC_LITERAL(23, 271, 6), // "runHSL"
QT_MOC_LITERAL(24, 278, 1), // "h"
QT_MOC_LITERAL(25, 280, 1), // "s"
QT_MOC_LITERAL(26, 282, 1), // "l"
QT_MOC_LITERAL(27, 284, 7), // "runHSLA"
QT_MOC_LITERAL(28, 292, 6), // "runHSV"
QT_MOC_LITERAL(29, 299, 1), // "v"
QT_MOC_LITERAL(30, 301, 7), // "runHSVA"
QT_MOC_LITERAL(31, 309, 6), // "runRGB"
QT_MOC_LITERAL(32, 316, 1), // "r"
QT_MOC_LITERAL(33, 318, 1), // "g"
QT_MOC_LITERAL(34, 320, 1), // "b"
QT_MOC_LITERAL(35, 322, 7), // "runRGBA"
QT_MOC_LITERAL(36, 330, 9), // "runMoveTo"
QT_MOC_LITERAL(37, 340, 1), // "x"
QT_MOC_LITERAL(38, 342, 13), // "runPageHeight"
QT_MOC_LITERAL(39, 356, 11), // "runLoadPage"
QT_MOC_LITERAL(40, 368, 8), // "fileName"
QT_MOC_LITERAL(41, 377, 7), // "runFill"
QT_MOC_LITERAL(42, 385, 11), // "runSetBrush"
QT_MOC_LITERAL(43, 397, 6), // "colour"
QT_MOC_LITERAL(44, 404, 7), // "runLine"
QT_MOC_LITERAL(45, 412, 2), // "x0"
QT_MOC_LITERAL(46, 415, 2), // "y0"
QT_MOC_LITERAL(47, 418, 9), // "runLineTo"
QT_MOC_LITERAL(48, 428, 10), // "runPolygon"
QT_MOC_LITERAL(49, 439, 1), // "N"
QT_MOC_LITERAL(50, 441, 12), // "QVector<int>"
QT_MOC_LITERAL(51, 454, 2), // "XX"
QT_MOC_LITERAL(52, 457, 2), // "YY"
QT_MOC_LITERAL(53, 460, 8), // "runWrite"
QT_MOC_LITERAL(54, 469, 4), // "text"
QT_MOC_LITERAL(55, 474, 10), // "runNewPage"
QT_MOC_LITERAL(56, 485, 5), // "width"
QT_MOC_LITERAL(57, 491, 6), // "height"
QT_MOC_LITERAL(58, 498, 10), // "background"
QT_MOC_LITERAL(59, 509, 9), // "runCircle"
QT_MOC_LITERAL(60, 519, 9), // "runSetPen"
QT_MOC_LITERAL(61, 529, 6), // "color_"
QT_MOC_LITERAL(62, 536, 8), // "runPoint"
QT_MOC_LITERAL(63, 545, 10), // "runDensity"
QT_MOC_LITERAL(64, 556, 5), // "value"
QT_MOC_LITERAL(65, 562, 12), // "runRectangle"
QT_MOC_LITERAL(66, 575, 14), // "runSplitToCMYK"
QT_MOC_LITERAL(67, 590, 4), // "int&"
QT_MOC_LITERAL(68, 595, 13), // "runSplitToHSL"
QT_MOC_LITERAL(69, 609, 13), // "runSplitToHSV"
QT_MOC_LITERAL(70, 623, 13), // "runSplitToRGB"
QT_MOC_LITERAL(71, 637, 11), // "runSavePage"
QT_MOC_LITERAL(72, 649, 13), // "runUnsetBrush"
QT_MOC_LITERAL(73, 663, 14), // "runPointSample"
QT_MOC_LITERAL(74, 678, 10), // "runCenterX"
QT_MOC_LITERAL(75, 689, 10), // "runCenterY"
QT_MOC_LITERAL(76, 700, 12), // "runPageWidth"
QT_MOC_LITERAL(77, 713, 12), // "runTextWidth"
QT_MOC_LITERAL(78, 726, 10), // "runSetFont"
QT_MOC_LITERAL(79, 737, 6), // "family"
QT_MOC_LITERAL(80, 744, 4), // "size"
QT_MOC_LITERAL(81, 749, 4), // "bold"
QT_MOC_LITERAL(82, 754, 6), // "italic"
QT_MOC_LITERAL(83, 761, 10), // "runEllipse"
QT_MOC_LITERAL(84, 772, 2), // "r1"
QT_MOC_LITERAL(85, 775, 2), // "r2"
QT_MOC_LITERAL(86, 778, 13), // "loadActorData"
QT_MOC_LITERAL(87, 792, 10), // "QIODevice*"
QT_MOC_LITERAL(88, 803, 6) // "source"

    },
    "ActorPainter::PainterModuleBase\0"
    "notifyOnTemplateParametersChanged\0\0"
    "reset\0reloadSettings\0ExtensionSystem::SettingsPtr\0"
    "settings\0keys\0changeGlobalState\0"
    "ExtensionSystem::GlobalState\0old\0"
    "current\0terminateEvaluation\0"
    "setAnimationEnabled\0enabled\0runCMYK\0"
    "Color\0c\0m\0y\0k\0runCMYKA\0a\0runHSL\0h\0s\0"
    "l\0runHSLA\0runHSV\0v\0runHSVA\0runRGB\0r\0"
    "g\0b\0runRGBA\0runMoveTo\0x\0runPageHeight\0"
    "runLoadPage\0fileName\0runFill\0runSetBrush\0"
    "colour\0runLine\0x0\0y0\0runLineTo\0"
    "runPolygon\0N\0QVector<int>\0XX\0YY\0"
    "runWrite\0text\0runNewPage\0width\0height\0"
    "background\0runCircle\0runSetPen\0color_\0"
    "runPoint\0runDensity\0value\0runRectangle\0"
    "runSplitToCMYK\0int&\0runSplitToHSL\0"
    "runSplitToHSV\0runSplitToRGB\0runSavePage\0"
    "runUnsetBrush\0runPointSample\0runCenterX\0"
    "runCenterY\0runPageWidth\0runTextWidth\0"
    "runSetFont\0family\0size\0bold\0italic\0"
    "runEllipse\0r1\0r2\0loadActorData\0"
    "QIODevice*\0source"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorPainter__PainterModuleBase[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      43,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  229,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,  230,    2, 0x0a /* Public */,
       4,    2,  231,    2, 0x0a /* Public */,
       8,    2,  236,    2, 0x0a /* Public */,
      12,    0,  241,    2, 0x0a /* Public */,
      13,    1,  242,    2, 0x0a /* Public */,
      15,    4,  245,    2, 0x0a /* Public */,
      21,    5,  254,    2, 0x0a /* Public */,
      23,    3,  265,    2, 0x0a /* Public */,
      27,    4,  272,    2, 0x0a /* Public */,
      28,    3,  281,    2, 0x0a /* Public */,
      30,    4,  288,    2, 0x0a /* Public */,
      31,    3,  297,    2, 0x0a /* Public */,
      35,    4,  304,    2, 0x0a /* Public */,
      36,    2,  313,    2, 0x0a /* Public */,
      38,    0,  318,    2, 0x0a /* Public */,
      39,    1,  319,    2, 0x0a /* Public */,
      41,    2,  322,    2, 0x0a /* Public */,
      42,    1,  327,    2, 0x0a /* Public */,
      44,    4,  330,    2, 0x0a /* Public */,
      47,    2,  339,    2, 0x0a /* Public */,
      48,    3,  344,    2, 0x0a /* Public */,
      53,    3,  351,    2, 0x0a /* Public */,
      55,    3,  358,    2, 0x0a /* Public */,
      59,    3,  365,    2, 0x0a /* Public */,
      60,    2,  372,    2, 0x0a /* Public */,
      62,    3,  377,    2, 0x0a /* Public */,
      63,    1,  384,    2, 0x0a /* Public */,
      65,    4,  387,    2, 0x0a /* Public */,
      66,    5,  396,    2, 0x0a /* Public */,
      68,    4,  407,    2, 0x0a /* Public */,
      69,    4,  416,    2, 0x0a /* Public */,
      70,    4,  425,    2, 0x0a /* Public */,
      71,    1,  434,    2, 0x0a /* Public */,
      72,    0,  437,    2, 0x0a /* Public */,
      73,    2,  438,    2, 0x0a /* Public */,
      74,    0,  443,    2, 0x0a /* Public */,
      75,    0,  444,    2, 0x0a /* Public */,
      76,    0,  445,    2, 0x0a /* Public */,
      77,    1,  446,    2, 0x0a /* Public */,
      78,    4,  449,    2, 0x0a /* Public */,
      83,    4,  458,    2, 0x0a /* Public */,
      86,    1,  467,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, QMetaType::QStringList,    6,    7,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 9,   10,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   14,
    0x80000000 | 16, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   17,   18,   19,   20,
    0x80000000 | 16, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   17,   18,   19,   20,   22,
    0x80000000 | 16, QMetaType::Int, QMetaType::Int, QMetaType::Int,   24,   25,   26,
    0x80000000 | 16, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   24,   25,   26,   22,
    0x80000000 | 16, QMetaType::Int, QMetaType::Int, QMetaType::Int,   24,   25,   29,
    0x80000000 | 16, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   24,   25,   29,   22,
    0x80000000 | 16, QMetaType::Int, QMetaType::Int, QMetaType::Int,   32,   33,   34,
    0x80000000 | 16, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   32,   33,   34,   22,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   37,   19,
    QMetaType::Int,
    QMetaType::Void, QMetaType::QString,   40,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   37,   19,
    QMetaType::Void, 0x80000000 | 16,   43,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   45,   46,   37,   19,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   37,   19,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 50, 0x80000000 | 50,   49,   51,   52,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString,   37,   19,   54,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 16,   56,   57,   58,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   37,   19,   32,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 16,   56,   61,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 16,   37,   19,   61,
    QMetaType::Void, QMetaType::Int,   64,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   45,   46,   37,   19,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 67, 0x80000000 | 67, 0x80000000 | 67, 0x80000000 | 67,   61,   17,   18,   19,   20,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 67, 0x80000000 | 67, 0x80000000 | 67,   61,   24,   25,   26,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 67, 0x80000000 | 67, 0x80000000 | 67,   61,   24,   25,   29,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 67, 0x80000000 | 67, 0x80000000 | 67,   61,   32,   33,   34,
    QMetaType::Void, QMetaType::QString,   40,
    QMetaType::Void,
    0x80000000 | 16, QMetaType::Int, QMetaType::Int,   37,   19,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int, QMetaType::QString,   54,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,   79,   80,   81,   82,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   37,   19,   84,   85,
    QMetaType::Void, 0x80000000 | 87,   88,

       0        // eod
};

void ActorPainter::PainterModuleBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PainterModuleBase *_t = static_cast<PainterModuleBase *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->notifyOnTemplateParametersChanged(); break;
        case 1: _t->reset(); break;
        case 2: _t->reloadSettings((*reinterpret_cast< ExtensionSystem::SettingsPtr(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 3: _t->changeGlobalState((*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[1])),(*reinterpret_cast< ExtensionSystem::GlobalState(*)>(_a[2]))); break;
        case 4: _t->terminateEvaluation(); break;
        case 5: _t->setAnimationEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: { Color _r = _t->runCMYK((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 7: { Color _r = _t->runCMYKA((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4])),(*reinterpret_cast< const int(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 8: { Color _r = _t->runHSL((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 9: { Color _r = _t->runHSLA((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 10: { Color _r = _t->runHSV((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 11: { Color _r = _t->runHSVA((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 12: { Color _r = _t->runRGB((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 13: { Color _r = _t->runRGBA((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 14: _t->runMoveTo((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 15: { int _r = _t->runPageHeight();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 16: _t->runLoadPage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->runFill((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 18: _t->runSetBrush((*reinterpret_cast< const Color(*)>(_a[1]))); break;
        case 19: _t->runLine((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4]))); break;
        case 20: _t->runLineTo((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 21: _t->runPolygon((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QVector<int>(*)>(_a[2])),(*reinterpret_cast< const QVector<int>(*)>(_a[3]))); break;
        case 22: _t->runWrite((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 23: _t->runNewPage((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const Color(*)>(_a[3]))); break;
        case 24: _t->runCircle((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 25: _t->runSetPen((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const Color(*)>(_a[2]))); break;
        case 26: _t->runPoint((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const Color(*)>(_a[3]))); break;
        case 27: _t->runDensity((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 28: _t->runRectangle((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4]))); break;
        case 29: _t->runSplitToCMYK((*reinterpret_cast< const Color(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 30: _t->runSplitToHSL((*reinterpret_cast< const Color(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 31: _t->runSplitToHSV((*reinterpret_cast< const Color(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 32: _t->runSplitToRGB((*reinterpret_cast< const Color(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 33: _t->runSavePage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 34: _t->runUnsetBrush(); break;
        case 35: { Color _r = _t->runPointSample((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Color*>(_a[0]) = _r; }  break;
        case 36: { int _r = _t->runCenterX();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 37: { int _r = _t->runCenterY();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 38: { int _r = _t->runPageWidth();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 39: { int _r = _t->runTextWidth((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 40: _t->runSetFont((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const bool(*)>(_a[3])),(*reinterpret_cast< const bool(*)>(_a[4]))); break;
        case 41: _t->runEllipse((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4]))); break;
        case 42: _t->loadActorData((*reinterpret_cast< QIODevice*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 21:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PainterModuleBase::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PainterModuleBase::notifyOnTemplateParametersChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject ActorPainter::PainterModuleBase::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ActorPainter__PainterModuleBase.data,
      qt_meta_data_ActorPainter__PainterModuleBase,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorPainter::PainterModuleBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorPainter::PainterModuleBase::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorPainter__PainterModuleBase.stringdata0))
        return static_cast<void*>(const_cast< PainterModuleBase*>(this));
    return QObject::qt_metacast(_clname);
}

int ActorPainter::PainterModuleBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    }
    return _id;
}

// SIGNAL 0
void ActorPainter::PainterModuleBase::notifyOnTemplateParametersChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
