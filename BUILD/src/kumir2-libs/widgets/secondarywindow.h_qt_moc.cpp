/****************************************************************************
** Meta object code from reading C++ file 'secondarywindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/widgets/secondarywindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'secondarywindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Widgets__SecondaryWindow_t {
    QByteArrayData data[9];
    char stringdata0[116];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widgets__SecondaryWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widgets__SecondaryWindow_t qt_meta_stringdata_Widgets__SecondaryWindow = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Widgets::SecondaryWindow"
QT_MOC_LITERAL(1, 25, 8), // "activate"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 14), // "updateSettings"
QT_MOC_LITERAL(4, 50, 28), // "ExtensionSystem::SettingsPtr"
QT_MOC_LITERAL(5, 79, 8), // "settings"
QT_MOC_LITERAL(6, 88, 4), // "keys"
QT_MOC_LITERAL(7, 93, 9), // "saveState"
QT_MOC_LITERAL(8, 103, 12) // "restoreState"

    },
    "Widgets::SecondaryWindow\0activate\0\0"
    "updateSettings\0ExtensionSystem::SettingsPtr\0"
    "settings\0keys\0saveState\0restoreState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widgets__SecondaryWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a /* Public */,
       3,    2,   35,    2, 0x0a /* Public */,
       7,    0,   40,    2, 0x0a /* Public */,
       8,    0,   41,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, QMetaType::QStringList,    5,    6,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Widgets::SecondaryWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SecondaryWindow *_t = static_cast<SecondaryWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->activate(); break;
        case 1: _t->updateSettings((*reinterpret_cast< ExtensionSystem::SettingsPtr(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 2: _t->saveState(); break;
        case 3: _t->restoreState(); break;
        default: ;
        }
    }
}

const QMetaObject Widgets::SecondaryWindow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Widgets__SecondaryWindow.data,
      qt_meta_data_Widgets__SecondaryWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widgets::SecondaryWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widgets::SecondaryWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widgets__SecondaryWindow.stringdata0))
        return static_cast<void*>(const_cast< SecondaryWindow*>(this));
    return QObject::qt_metacast(_clname);
}

int Widgets::SecondaryWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
