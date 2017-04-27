/****************************************************************************
** Meta object code from reading C++ file 'declarativesettingspage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/widgets/declarativesettingspage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'declarativesettingspage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Widgets__DeclarativeSettingsPage_t {
    QByteArrayData data[7];
    char stringdata0[83];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widgets__DeclarativeSettingsPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widgets__DeclarativeSettingsPage_t qt_meta_stringdata_Widgets__DeclarativeSettingsPage = {
    {
QT_MOC_LITERAL(0, 0, 32), // "Widgets::DeclarativeSettingsPage"
QT_MOC_LITERAL(1, 33, 15), // "settingsChanged"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 4), // "keys"
QT_MOC_LITERAL(4, 55, 4), // "init"
QT_MOC_LITERAL(5, 60, 6), // "accept"
QT_MOC_LITERAL(6, 67, 15) // "resetToDefaults"

    },
    "Widgets::DeclarativeSettingsPage\0"
    "settingsChanged\0\0keys\0init\0accept\0"
    "resetToDefaults"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widgets__DeclarativeSettingsPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   37,    2, 0x0a /* Public */,
       5,    0,   38,    2, 0x0a /* Public */,
       6,    0,   39,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Widgets::DeclarativeSettingsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DeclarativeSettingsPage *_t = static_cast<DeclarativeSettingsPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->settingsChanged((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 1: _t->init(); break;
        case 2: _t->accept(); break;
        case 3: _t->resetToDefaults(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DeclarativeSettingsPage::*_t)(const QStringList & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DeclarativeSettingsPage::settingsChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Widgets::DeclarativeSettingsPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Widgets__DeclarativeSettingsPage.data,
      qt_meta_data_Widgets__DeclarativeSettingsPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widgets::DeclarativeSettingsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widgets::DeclarativeSettingsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widgets__DeclarativeSettingsPage.stringdata0))
        return static_cast<void*>(const_cast< DeclarativeSettingsPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int Widgets::DeclarativeSettingsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Widgets::DeclarativeSettingsPage::settingsChanged(const QStringList & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
