/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/coregui/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CoreGUI__MakeNativeExecutableWorker_t {
    QByteArrayData data[3];
    char stringdata0[44];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoreGUI__MakeNativeExecutableWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoreGUI__MakeNativeExecutableWorker_t qt_meta_stringdata_CoreGUI__MakeNativeExecutableWorker = {
    {
QT_MOC_LITERAL(0, 0, 35), // "CoreGUI::MakeNativeExecutable..."
QT_MOC_LITERAL(1, 36, 6), // "cancel"
QT_MOC_LITERAL(2, 43, 0) // ""

    },
    "CoreGUI::MakeNativeExecutableWorker\0"
    "cancel\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoreGUI__MakeNativeExecutableWorker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CoreGUI::MakeNativeExecutableWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MakeNativeExecutableWorker *_t = static_cast<MakeNativeExecutableWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cancel(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CoreGUI::MakeNativeExecutableWorker::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_CoreGUI__MakeNativeExecutableWorker.data,
      qt_meta_data_CoreGUI__MakeNativeExecutableWorker,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CoreGUI::MakeNativeExecutableWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreGUI::MakeNativeExecutableWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CoreGUI__MakeNativeExecutableWorker.stringdata0))
        return static_cast<void*>(const_cast< MakeNativeExecutableWorker*>(this));
    return QThread::qt_metacast(_clname);
}

int CoreGUI::MakeNativeExecutableWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CoreGUI__MainWindow_t {
    QByteArrayData data[104];
    char stringdata0[1625];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoreGUI__MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoreGUI__MainWindow_t qt_meta_stringdata_CoreGUI__MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 19), // "CoreGUI::MainWindow"
QT_MOC_LITERAL(1, 20, 22), // "switchToRowFirstLayout"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 25), // "switchToColumnFirstLayout"
QT_MOC_LITERAL(4, 70, 11), // "lockActions"
QT_MOC_LITERAL(5, 82, 13), // "unlockActions"
QT_MOC_LITERAL(6, 96, 26), // "ensureSeconrarySideVisible"
QT_MOC_LITERAL(7, 123, 17), // "setConsoleVisible"
QT_MOC_LITERAL(8, 141, 1), // "v"
QT_MOC_LITERAL(9, 143, 11), // "recentFiles"
QT_MOC_LITERAL(10, 155, 9), // "fullPaths"
QT_MOC_LITERAL(11, 165, 14), // "loadRecentFile"
QT_MOC_LITERAL(12, 180, 8), // "fullPath"
QT_MOC_LITERAL(13, 189, 11), // "loadFromUrl"
QT_MOC_LITERAL(14, 201, 1), // "s"
QT_MOC_LITERAL(15, 203, 17), // "TabWidgetElement*"
QT_MOC_LITERAL(16, 221, 3), // "url"
QT_MOC_LITERAL(17, 225, 16), // "addToRecentFiles"
QT_MOC_LITERAL(18, 242, 21), // "loadFromCourseManager"
QT_MOC_LITERAL(19, 264, 39), // "Shared::GuiInterface::Program..."
QT_MOC_LITERAL(20, 304, 4), // "data"
QT_MOC_LITERAL(21, 309, 26), // "courseManagerProgramSource"
QT_MOC_LITERAL(22, 336, 15), // "saveCurrentFile"
QT_MOC_LITERAL(23, 352, 17), // "saveCurrentFileAs"
QT_MOC_LITERAL(24, 370, 17), // "saveCurrentFileTo"
QT_MOC_LITERAL(25, 388, 8), // "fileName"
QT_MOC_LITERAL(26, 397, 14), // "restoreSession"
QT_MOC_LITERAL(27, 412, 11), // "saveSession"
QT_MOC_LITERAL(28, 424, 12), // "loadSettings"
QT_MOC_LITERAL(29, 437, 4), // "keys"
QT_MOC_LITERAL(30, 442, 12), // "saveSettings"
QT_MOC_LITERAL(31, 455, 10), // "newProgram"
QT_MOC_LITERAL(32, 466, 7), // "newText"
QT_MOC_LITERAL(33, 474, 4), // "text"
QT_MOC_LITERAL(34, 479, 8), // "fileOpen"
QT_MOC_LITERAL(35, 488, 15), // "closeCurrentTab"
QT_MOC_LITERAL(36, 504, 8), // "closeTab"
QT_MOC_LITERAL(37, 513, 5), // "index"
QT_MOC_LITERAL(38, 519, 15), // "showPreferences"
QT_MOC_LITERAL(39, 535, 11), // "showMessage"
QT_MOC_LITERAL(40, 547, 12), // "clearMessage"
QT_MOC_LITERAL(41, 560, 23), // "setFocusOnCentralWidget"
QT_MOC_LITERAL(42, 584, 20), // "changeFocusOnMenubar"
QT_MOC_LITERAL(43, 605, 15), // "switchWorkspace"
QT_MOC_LITERAL(44, 621, 9), // "showAbout"
QT_MOC_LITERAL(45, 631, 14), // "showUserManual"
QT_MOC_LITERAL(46, 646, 8), // "showHelp"
QT_MOC_LITERAL(47, 655, 14), // "updateSettings"
QT_MOC_LITERAL(48, 670, 28), // "ExtensionSystem::SettingsPtr"
QT_MOC_LITERAL(49, 699, 8), // "settings"
QT_MOC_LITERAL(50, 708, 20), // "updateStartPageTitle"
QT_MOC_LITERAL(51, 729, 5), // "title"
QT_MOC_LITERAL(52, 735, 41), // "const Shared::Browser::Instan..."
QT_MOC_LITERAL(53, 777, 6), // "sender"
QT_MOC_LITERAL(54, 784, 20), // "makeNativeExecutable"
QT_MOC_LITERAL(55, 805, 20), // "saveNativeExecutable"
QT_MOC_LITERAL(56, 826, 19), // "ensureBottomVisible"
QT_MOC_LITERAL(57, 846, 29), // "addPresentationModeItemToMenu"
QT_MOC_LITERAL(58, 876, 21), // "enterPresentationMode"
QT_MOC_LITERAL(59, 898, 21), // "leavePresentationMode"
QT_MOC_LITERAL(60, 920, 22), // "togglePresentationMode"
QT_MOC_LITERAL(61, 943, 18), // "isPresentationMode"
QT_MOC_LITERAL(62, 962, 24), // "setFirstTimeWindowLayout"
QT_MOC_LITERAL(63, 987, 31), // "setFirstTimeWindowLayout_stage2"
QT_MOC_LITERAL(64, 1019, 31), // "setFirstTimeWindowLayout_stage3"
QT_MOC_LITERAL(65, 1051, 22), // "prepareRecentFilesMenu"
QT_MOC_LITERAL(66, 1074, 14), // "prepareRunMenu"
QT_MOC_LITERAL(67, 1089, 15), // "prepareEditMenu"
QT_MOC_LITERAL(68, 1105, 17), // "prepareInsertMenu"
QT_MOC_LITERAL(69, 1123, 18), // "setupActionsForTab"
QT_MOC_LITERAL(70, 1142, 18), // "setupContentForTab"
QT_MOC_LITERAL(71, 1161, 20), // "setupStatusbarForTab"
QT_MOC_LITERAL(72, 1182, 14), // "setTitleForTab"
QT_MOC_LITERAL(73, 1197, 21), // "handleTabTitleChanged"
QT_MOC_LITERAL(74, 1219, 17), // "checkCounterValue"
QT_MOC_LITERAL(75, 1237, 17), // "showAlgorithmHelp"
QT_MOC_LITERAL(76, 1255, 7), // "package"
QT_MOC_LITERAL(77, 1263, 8), // "function"
QT_MOC_LITERAL(78, 1272, 11), // "addToRecent"
QT_MOC_LITERAL(79, 1284, 26), // "handleDocumentCleanChanged"
QT_MOC_LITERAL(80, 1311, 31), // "checkForConsoleHiddenBySplitter"
QT_MOC_LITERAL(81, 1343, 23), // "setupMenuBarContextMenu"
QT_MOC_LITERAL(82, 1367, 17), // "addMenuBeforeHelp"
QT_MOC_LITERAL(83, 1385, 6), // "QMenu*"
QT_MOC_LITERAL(84, 1392, 4), // "menu"
QT_MOC_LITERAL(85, 1397, 18), // "updateVisibleMenus"
QT_MOC_LITERAL(86, 1416, 32), // "handleBreakpointCnagedOrInserted"
QT_MOC_LITERAL(87, 1449, 7), // "enabled"
QT_MOC_LITERAL(88, 1457, 6), // "lineNo"
QT_MOC_LITERAL(89, 1464, 11), // "ignoreCount"
QT_MOC_LITERAL(90, 1476, 9), // "condition"
QT_MOC_LITERAL(91, 1486, 23), // "handleBreakpointRemoved"
QT_MOC_LITERAL(92, 1510, 12), // "DocumentType"
QT_MOC_LITERAL(93, 1523, 4), // "Text"
QT_MOC_LITERAL(94, 1528, 7), // "Program"
QT_MOC_LITERAL(95, 1536, 9), // "StartPage"
QT_MOC_LITERAL(96, 1546, 14), // "DockWindowType"
QT_MOC_LITERAL(97, 1561, 8), // "Terminal"
QT_MOC_LITERAL(98, 1570, 4), // "Help"
QT_MOC_LITERAL(99, 1575, 13), // "StandardActor"
QT_MOC_LITERAL(100, 1589, 10), // "WorldActor"
QT_MOC_LITERAL(101, 1600, 7), // "Control"
QT_MOC_LITERAL(102, 1608, 10), // "SubControl"
QT_MOC_LITERAL(103, 1619, 5) // "Other"

    },
    "CoreGUI::MainWindow\0switchToRowFirstLayout\0"
    "\0switchToColumnFirstLayout\0lockActions\0"
    "unlockActions\0ensureSeconrarySideVisible\0"
    "setConsoleVisible\0v\0recentFiles\0"
    "fullPaths\0loadRecentFile\0fullPath\0"
    "loadFromUrl\0s\0TabWidgetElement*\0url\0"
    "addToRecentFiles\0loadFromCourseManager\0"
    "Shared::GuiInterface::ProgramSourceText\0"
    "data\0courseManagerProgramSource\0"
    "saveCurrentFile\0saveCurrentFileAs\0"
    "saveCurrentFileTo\0fileName\0restoreSession\0"
    "saveSession\0loadSettings\0keys\0"
    "saveSettings\0newProgram\0newText\0text\0"
    "fileOpen\0closeCurrentTab\0closeTab\0"
    "index\0showPreferences\0showMessage\0"
    "clearMessage\0setFocusOnCentralWidget\0"
    "changeFocusOnMenubar\0switchWorkspace\0"
    "showAbout\0showUserManual\0showHelp\0"
    "updateSettings\0ExtensionSystem::SettingsPtr\0"
    "settings\0updateStartPageTitle\0title\0"
    "const Shared::Browser::InstanceInterface*\0"
    "sender\0makeNativeExecutable\0"
    "saveNativeExecutable\0ensureBottomVisible\0"
    "addPresentationModeItemToMenu\0"
    "enterPresentationMode\0leavePresentationMode\0"
    "togglePresentationMode\0isPresentationMode\0"
    "setFirstTimeWindowLayout\0"
    "setFirstTimeWindowLayout_stage2\0"
    "setFirstTimeWindowLayout_stage3\0"
    "prepareRecentFilesMenu\0prepareRunMenu\0"
    "prepareEditMenu\0prepareInsertMenu\0"
    "setupActionsForTab\0setupContentForTab\0"
    "setupStatusbarForTab\0setTitleForTab\0"
    "handleTabTitleChanged\0checkCounterValue\0"
    "showAlgorithmHelp\0package\0function\0"
    "addToRecent\0handleDocumentCleanChanged\0"
    "checkForConsoleHiddenBySplitter\0"
    "setupMenuBarContextMenu\0addMenuBeforeHelp\0"
    "QMenu*\0menu\0updateVisibleMenus\0"
    "handleBreakpointCnagedOrInserted\0"
    "enabled\0lineNo\0ignoreCount\0condition\0"
    "handleBreakpointRemoved\0DocumentType\0"
    "Text\0Program\0StartPage\0DockWindowType\0"
    "Terminal\0Help\0StandardActor\0WorldActor\0"
    "Control\0SubControl\0Other"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoreGUI__MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      69,   14, // methods
       0,    0, // properties
       2,  492, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  359,    2, 0x0a /* Public */,
       3,    0,  360,    2, 0x0a /* Public */,
       4,    0,  361,    2, 0x0a /* Public */,
       5,    0,  362,    2, 0x0a /* Public */,
       6,    0,  363,    2, 0x0a /* Public */,
       7,    1,  364,    2, 0x0a /* Public */,
       9,    1,  367,    2, 0x0a /* Public */,
      11,    1,  370,    2, 0x0a /* Public */,
      13,    1,  373,    2, 0x0a /* Public */,
      13,    2,  376,    2, 0x0a /* Public */,
      18,    1,  381,    2, 0x0a /* Public */,
      21,    0,  384,    2, 0x0a /* Public */,
      22,    0,  385,    2, 0x0a /* Public */,
      23,    0,  386,    2, 0x0a /* Public */,
      24,    1,  387,    2, 0x0a /* Public */,
      26,    0,  390,    2, 0x0a /* Public */,
      27,    0,  391,    2, 0x0a /* Public */,
      28,    1,  392,    2, 0x0a /* Public */,
      30,    0,  395,    2, 0x0a /* Public */,
      31,    0,  396,    2, 0x0a /* Public */,
      32,    0,  397,    2, 0x0a /* Public */,
      32,    2,  398,    2, 0x0a /* Public */,
      34,    0,  403,    2, 0x0a /* Public */,
      35,    0,  404,    2, 0x0a /* Public */,
      36,    1,  405,    2, 0x0a /* Public */,
      38,    0,  408,    2, 0x0a /* Public */,
      39,    1,  409,    2, 0x0a /* Public */,
      40,    0,  412,    2, 0x0a /* Public */,
      41,    0,  413,    2, 0x0a /* Public */,
      42,    0,  414,    2, 0x0a /* Public */,
      43,    0,  415,    2, 0x0a /* Public */,
      44,    0,  416,    2, 0x0a /* Public */,
      45,    0,  417,    2, 0x0a /* Public */,
      46,    0,  418,    2, 0x0a /* Public */,
      47,    2,  419,    2, 0x0a /* Public */,
      50,    2,  424,    2, 0x0a /* Public */,
      54,    0,  429,    2, 0x0a /* Public */,
      55,    0,  430,    2, 0x0a /* Public */,
      56,    0,  431,    2, 0x0a /* Public */,
      57,    0,  432,    2, 0x0a /* Public */,
      58,    0,  433,    2, 0x0a /* Public */,
      59,    0,  434,    2, 0x0a /* Public */,
      60,    0,  435,    2, 0x0a /* Public */,
      61,    0,  436,    2, 0x0a /* Public */,
      62,    0,  437,    2, 0x08 /* Private */,
      63,    0,  438,    2, 0x08 /* Private */,
      64,    0,  439,    2, 0x08 /* Private */,
      11,    0,  440,    2, 0x08 /* Private */,
      65,    0,  441,    2, 0x08 /* Private */,
      66,    0,  442,    2, 0x08 /* Private */,
      67,    0,  443,    2, 0x08 /* Private */,
      68,    0,  444,    2, 0x08 /* Private */,
      69,    0,  445,    2, 0x08 /* Private */,
      70,    0,  446,    2, 0x08 /* Private */,
      71,    0,  447,    2, 0x08 /* Private */,
      72,    1,  448,    2, 0x08 /* Private */,
      73,    0,  451,    2, 0x08 /* Private */,
      74,    0,  452,    2, 0x08 /* Private */,
      75,    2,  453,    2, 0x08 /* Private */,
      78,    1,  458,    2, 0x08 /* Private */,
      79,    1,  461,    2, 0x08 /* Private */,
      80,    2,  464,    2, 0x08 /* Private */,
      81,    0,  469,    2, 0x08 /* Private */,
      82,    1,  470,    2, 0x08 /* Private */,
      85,    0,  473,    2, 0x08 /* Private */,
      85,    1,  474,    2, 0x08 /* Private */,
      85,    1,  477,    2, 0x08 /* Private */,
      86,    4,  480,    2, 0x08 /* Private */,
      91,    1,  489,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::QStringList, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::QString,   14,
    0x80000000 | 15, QMetaType::QUrl, QMetaType::Bool,   16,   17,
    0x80000000 | 15, 0x80000000 | 19,   20,
    0x80000000 | 19,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::QString,   25,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,   29,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   25,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool, QMetaType::Int,   37,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 48, QMetaType::QStringList,   49,   29,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 52,   51,   53,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
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
    QMetaType::Void, QMetaType::Int,   37,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   76,   77,
    QMetaType::Void, QMetaType::QString,   25,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 83,   84,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   37,
    QMetaType::Void, 0x80000000 | 15,    2,
    QMetaType::Void, QMetaType::Bool, QMetaType::UInt, QMetaType::UInt, QMetaType::QString,   87,   88,   89,   90,
    QMetaType::Void, QMetaType::UInt,   88,

 // enums: name, flags, count, data
      92, 0x0,    3,  500,
      96, 0x0,    7,  506,

 // enum data: key, value
      93, uint(CoreGUI::MainWindow::Text),
      94, uint(CoreGUI::MainWindow::Program),
      95, uint(CoreGUI::MainWindow::StartPage),
      97, uint(CoreGUI::MainWindow::Terminal),
      98, uint(CoreGUI::MainWindow::Help),
      99, uint(CoreGUI::MainWindow::StandardActor),
     100, uint(CoreGUI::MainWindow::WorldActor),
     101, uint(CoreGUI::MainWindow::Control),
     102, uint(CoreGUI::MainWindow::SubControl),
     103, uint(CoreGUI::MainWindow::Other),

       0        // eod
};

void CoreGUI::MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->switchToRowFirstLayout(); break;
        case 1: _t->switchToColumnFirstLayout(); break;
        case 2: _t->lockActions(); break;
        case 3: _t->unlockActions(); break;
        case 4: _t->ensureSeconrarySideVisible(); break;
        case 5: _t->setConsoleVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: { QStringList _r = _t->recentFiles((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        case 7: _t->loadRecentFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->loadFromUrl((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: { TabWidgetElement* _r = _t->loadFromUrl((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< TabWidgetElement**>(_a[0]) = _r; }  break;
        case 10: { TabWidgetElement* _r = _t->loadFromCourseManager((*reinterpret_cast< const Shared::GuiInterface::ProgramSourceText(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< TabWidgetElement**>(_a[0]) = _r; }  break;
        case 11: { Shared::GuiInterface::ProgramSourceText _r = _t->courseManagerProgramSource();
            if (_a[0]) *reinterpret_cast< Shared::GuiInterface::ProgramSourceText*>(_a[0]) = _r; }  break;
        case 12: { bool _r = _t->saveCurrentFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = _t->saveCurrentFileAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: { bool _r = _t->saveCurrentFileTo((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 15: _t->restoreSession(); break;
        case 16: _t->saveSession(); break;
        case 17: _t->loadSettings((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 18: _t->saveSettings(); break;
        case 19: _t->newProgram(); break;
        case 20: _t->newText(); break;
        case 21: _t->newText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 22: _t->fileOpen(); break;
        case 23: _t->closeCurrentTab(); break;
        case 24: { bool _r = _t->closeTab((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 25: _t->showPreferences(); break;
        case 26: _t->showMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 27: _t->clearMessage(); break;
        case 28: _t->setFocusOnCentralWidget(); break;
        case 29: _t->changeFocusOnMenubar(); break;
        case 30: _t->switchWorkspace(); break;
        case 31: _t->showAbout(); break;
        case 32: _t->showUserManual(); break;
        case 33: _t->showHelp(); break;
        case 34: _t->updateSettings((*reinterpret_cast< ExtensionSystem::SettingsPtr(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 35: _t->updateStartPageTitle((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const Shared::Browser::InstanceInterface*(*)>(_a[2]))); break;
        case 36: _t->makeNativeExecutable(); break;
        case 37: _t->saveNativeExecutable(); break;
        case 38: _t->ensureBottomVisible(); break;
        case 39: _t->addPresentationModeItemToMenu(); break;
        case 40: _t->enterPresentationMode(); break;
        case 41: _t->leavePresentationMode(); break;
        case 42: _t->togglePresentationMode(); break;
        case 43: { bool _r = _t->isPresentationMode();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 44: _t->setFirstTimeWindowLayout(); break;
        case 45: _t->setFirstTimeWindowLayout_stage2(); break;
        case 46: _t->setFirstTimeWindowLayout_stage3(); break;
        case 47: _t->loadRecentFile(); break;
        case 48: _t->prepareRecentFilesMenu(); break;
        case 49: _t->prepareRunMenu(); break;
        case 50: _t->prepareEditMenu(); break;
        case 51: _t->prepareInsertMenu(); break;
        case 52: _t->setupActionsForTab(); break;
        case 53: _t->setupContentForTab(); break;
        case 54: _t->setupStatusbarForTab(); break;
        case 55: _t->setTitleForTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 56: _t->handleTabTitleChanged(); break;
        case 57: _t->checkCounterValue(); break;
        case 58: _t->showAlgorithmHelp((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 59: _t->addToRecent((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 60: _t->handleDocumentCleanChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 61: _t->checkForConsoleHiddenBySplitter((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 62: _t->setupMenuBarContextMenu(); break;
        case 63: _t->addMenuBeforeHelp((*reinterpret_cast< QMenu*(*)>(_a[1]))); break;
        case 64: _t->updateVisibleMenus(); break;
        case 65: _t->updateVisibleMenus((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 66: _t->updateVisibleMenus((*reinterpret_cast< TabWidgetElement*(*)>(_a[1]))); break;
        case 67: _t->handleBreakpointCnagedOrInserted((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< quint32(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 68: _t->handleBreakpointRemoved((*reinterpret_cast< quint32(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Shared::GuiInterface::ProgramSourceText >(); break;
            }
            break;
        case 63:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QMenu* >(); break;
            }
            break;
        }
    }
}

const QMetaObject CoreGUI::MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_CoreGUI__MainWindow.data,
      qt_meta_data_CoreGUI__MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CoreGUI::MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreGUI::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CoreGUI__MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int CoreGUI::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 69)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 69;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 69)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 69;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
