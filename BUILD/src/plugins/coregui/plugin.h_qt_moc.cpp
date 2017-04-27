/****************************************************************************
** Meta object code from reading C++ file 'plugin.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/coregui/plugin.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CoreGUI__Plugin_t {
    QByteArrayData data[31];
    char stringdata0[456];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoreGUI__Plugin_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoreGUI__Plugin_t qt_meta_stringdata_CoreGUI__Plugin = {
    {
QT_MOC_LITERAL(0, 0, 15), // "CoreGUI::Plugin"
QT_MOC_LITERAL(1, 16, 30), // "externalProcessCommandReceived"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 7), // "command"
QT_MOC_LITERAL(4, 56, 16), // "setProgramSource"
QT_MOC_LITERAL(5, 73, 17), // "ProgramSourceText"
QT_MOC_LITERAL(6, 91, 6), // "source"
QT_MOC_LITERAL(7, 98, 13), // "programSource"
QT_MOC_LITERAL(8, 112, 12), // "startTesting"
QT_MOC_LITERAL(9, 125, 25), // "showWorkspaceChooseDialog"
QT_MOC_LITERAL(10, 151, 6), // "wsName"
QT_MOC_LITERAL(11, 158, 24), // "applicationVersionString"
QT_MOC_LITERAL(12, 183, 17), // "showCoursesWindow"
QT_MOC_LITERAL(13, 201, 2), // "id"
QT_MOC_LITERAL(14, 204, 11), // "coursesList"
QT_MOC_LITERAL(15, 216, 9), // "fullPaths"
QT_MOC_LITERAL(16, 226, 14), // "showHelpWindow"
QT_MOC_LITERAL(17, 241, 5), // "index"
QT_MOC_LITERAL(18, 247, 32), // "showHelpWindowFromQuickReference"
QT_MOC_LITERAL(19, 280, 9), // "topicType"
QT_MOC_LITERAL(20, 290, 4), // "name"
QT_MOC_LITERAL(21, 295, 8), // "helpList"
QT_MOC_LITERAL(22, 304, 23), // "overridenEditorFontSize"
QT_MOC_LITERAL(23, 328, 24), // "prepareKumirProgramToRun"
QT_MOC_LITERAL(24, 353, 28), // "handleExternalProcessCommand"
QT_MOC_LITERAL(25, 382, 14), // "updateSettings"
QT_MOC_LITERAL(26, 397, 4), // "keys"
QT_MOC_LITERAL(27, 402, 17), // "updateAppFontSize"
QT_MOC_LITERAL(28, 420, 9), // "pointSize"
QT_MOC_LITERAL(29, 430, 15), // "showActorWindow"
QT_MOC_LITERAL(30, 446, 9) // "asciiName"

    },
    "CoreGUI::Plugin\0externalProcessCommandReceived\0"
    "\0command\0setProgramSource\0ProgramSourceText\0"
    "source\0programSource\0startTesting\0"
    "showWorkspaceChooseDialog\0wsName\0"
    "applicationVersionString\0showCoursesWindow\0"
    "id\0coursesList\0fullPaths\0showHelpWindow\0"
    "index\0showHelpWindowFromQuickReference\0"
    "topicType\0name\0helpList\0overridenEditorFontSize\0"
    "prepareKumirProgramToRun\0"
    "handleExternalProcessCommand\0"
    "updateSettings\0keys\0updateAppFontSize\0"
    "pointSize\0showActorWindow\0asciiName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoreGUI__Plugin[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  104,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,  107,    2, 0x0a /* Public */,
       7,    0,  110,    2, 0x0a /* Public */,
       8,    0,  111,    2, 0x0a /* Public */,
       9,    0,  112,    2, 0x0a /* Public */,
      10,    0,  113,    2, 0x0a /* Public */,
      11,    0,  114,    2, 0x0a /* Public */,
      12,    1,  115,    2, 0x0a /* Public */,
      14,    1,  118,    2, 0x0a /* Public */,
      16,    1,  121,    2, 0x0a /* Public */,
      18,    2,  124,    2, 0x0a /* Public */,
      21,    0,  129,    2, 0x0a /* Public */,
      22,    0,  130,    2, 0x0a /* Public */,
      23,    0,  131,    2, 0x09 /* Protected */,
      24,    1,  132,    2, 0x09 /* Protected */,
      25,    1,  135,    2, 0x09 /* Protected */,
      27,    1,  138,    2, 0x09 /* Protected */,
      29,    1,  141,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    0x80000000 | 5,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::QStringList, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   19,   20,
    QMetaType::QStringList,
    QMetaType::Int,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QStringList,   26,
    QMetaType::Void, QMetaType::Int,   28,
    QMetaType::Void, QMetaType::QByteArray,   30,

       0        // eod
};

void CoreGUI::Plugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Plugin *_t = static_cast<Plugin *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->externalProcessCommandReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->setProgramSource((*reinterpret_cast< const ProgramSourceText(*)>(_a[1]))); break;
        case 2: { ProgramSourceText _r = _t->programSource();
            if (_a[0]) *reinterpret_cast< ProgramSourceText*>(_a[0]) = _r; }  break;
        case 3: _t->startTesting(); break;
        case 4: { bool _r = _t->showWorkspaceChooseDialog();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: { QString _r = _t->wsName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 6: { QString _r = _t->applicationVersionString();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 7: _t->showCoursesWindow((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: { QStringList _r = _t->coursesList((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        case 9: _t->showHelpWindow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->showHelpWindowFromQuickReference((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 11: { QStringList _r = _t->helpList();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        case 12: { int _r = _t->overridenEditorFontSize();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 13: _t->prepareKumirProgramToRun(); break;
        case 14: _t->handleExternalProcessCommand((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->updateSettings((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 16: _t->updateAppFontSize((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 17: _t->showActorWindow((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Plugin::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Plugin::externalProcessCommandReceived)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject CoreGUI::Plugin::staticMetaObject = {
    { &ExtensionSystem::KPlugin::staticMetaObject, qt_meta_stringdata_CoreGUI__Plugin.data,
      qt_meta_data_CoreGUI__Plugin,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CoreGUI::Plugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreGUI::Plugin::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CoreGUI__Plugin.stringdata0))
        return static_cast<void*>(const_cast< Plugin*>(this));
    if (!strcmp(_clname, "Shared::GuiInterface"))
        return static_cast< Shared::GuiInterface*>(const_cast< Plugin*>(this));
    if (!strcmp(_clname, "kumir2.Gui"))
        return static_cast< Shared::GuiInterface*>(const_cast< Plugin*>(this));
    return ExtensionSystem::KPlugin::qt_metacast(_clname);
}

int CoreGUI::Plugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ExtensionSystem::KPlugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void CoreGUI::Plugin::externalProcessCommandReceived(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

QT_PLUGIN_METADATA_SECTION const uint qt_section_alignment_dummy = 42;

#ifdef QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0x8c, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    'x',  0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x0e, 0x00, 'k',  'u',  'm',  'i',  'r',  '2', 
    '.',  'C',  'o',  'r',  'e',  'G',  'U',  'I', 
    0x1b, 0x07, 0x00, 0x00, 0x09, 0x00, 'c',  'l', 
    'a',  's',  's',  'N',  'a',  'm',  'e',  0x00,
    0x06, 0x00, 'P',  'l',  'u',  'g',  'i',  'n', 
    'Z',  0xc0, 0xa0, 0x00, 0x07, 0x00, 'v',  'e', 
    'r',  's',  'i',  'o',  'n',  0x00, 0x00, 0x00,
    0x11, 0x00, 0x00, 0x00, 0x05, 0x00, 'd',  'e', 
    'b',  'u',  'g',  0x00, 0x95, 0x0d, 0x00, 0x00,
    0x08, 0x00, 'M',  'e',  't',  'a',  'D',  'a', 
    't',  'a',  0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, '\\', 0x00, 0x00, 0x00,
    '(',  0x00, 0x00, 0x00, 'P',  0x00, 0x00, 0x00,
    '@',  0x00, 0x00, 0x00
};

#else // QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0x8c, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    'x',  0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x0e, 0x00, 'k',  'u',  'm',  'i',  'r',  '2', 
    '.',  'C',  'o',  'r',  'e',  'G',  'U',  'I', 
    0x15, 0x07, 0x00, 0x00, 0x08, 0x00, 'M',  'e', 
    't',  'a',  'D',  'a',  't',  'a',  0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x9b, 0x0a, 0x00, 0x00,
    0x09, 0x00, 'c',  'l',  'a',  's',  's',  'N', 
    'a',  'm',  'e',  0x00, 0x06, 0x00, 'P',  'l', 
    'u',  'g',  'i',  'n',  '1',  0x00, 0x00, 0x00,
    0x05, 0x00, 'd',  'e',  'b',  'u',  'g',  0x00,
    'Z',  0xc0, 0xa0, 0x00, 0x07, 0x00, 'v',  'e', 
    'r',  's',  'i',  'o',  'n',  0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, '(',  0x00, 0x00, 0x00,
    'D',  0x00, 0x00, 0x00, '\\', 0x00, 0x00, 0x00,
    'h',  0x00, 0x00, 0x00
};
#endif // QT_NO_DEBUG

using namespace CoreGUI;
QT_MOC_EXPORT_PLUGIN(CoreGUI::Plugin, Plugin)

QT_END_MOC_NAMESPACE
