/****************************************************************************
** Meta object code from reading C++ file 'tabwidgetelement.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/coregui/tabwidgetelement.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tabwidgetelement.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CoreGUI__TabWidgetElement_t {
    QByteArrayData data[10];
    char stringdata0[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoreGUI__TabWidgetElement_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoreGUI__TabWidgetElement_t qt_meta_stringdata_CoreGUI__TabWidgetElement = {
    {
QT_MOC_LITERAL(0, 0, 25), // "CoreGUI::TabWidgetElement"
QT_MOC_LITERAL(1, 26, 12), // "titleChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 20), // "documentCleanChanged"
QT_MOC_LITERAL(4, 61, 1), // "v"
QT_MOC_LITERAL(5, 63, 26), // "explicitImportNamesRequest"
QT_MOC_LITERAL(6, 90, 23), // "setDocumentChangesClean"
QT_MOC_LITERAL(7, 114, 5), // "clean"
QT_MOC_LITERAL(8, 120, 25), // "updateCompilerImportsList"
QT_MOC_LITERAL(9, 146, 14) // "localizedNames"

    },
    "CoreGUI::TabWidgetElement\0titleChanged\0"
    "\0documentCleanChanged\0v\0"
    "explicitImportNamesRequest\0"
    "setDocumentChangesClean\0clean\0"
    "updateCompilerImportsList\0localizedNames"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoreGUI__TabWidgetElement[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       3,    1,   42,    2, 0x06 /* Public */,
       5,    0,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   46,    2, 0x09 /* Protected */,
       8,    1,   49,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::QStringList,    9,

       0        // eod
};

void CoreGUI::TabWidgetElement::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TabWidgetElement *_t = static_cast<TabWidgetElement *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->titleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->documentCleanChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->explicitImportNamesRequest(); break;
        case 3: _t->setDocumentChangesClean((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->updateCompilerImportsList((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TabWidgetElement::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabWidgetElement::titleChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (TabWidgetElement::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabWidgetElement::documentCleanChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (TabWidgetElement::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabWidgetElement::explicitImportNamesRequest)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject CoreGUI::TabWidgetElement::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CoreGUI__TabWidgetElement.data,
      qt_meta_data_CoreGUI__TabWidgetElement,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CoreGUI::TabWidgetElement::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreGUI::TabWidgetElement::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CoreGUI__TabWidgetElement.stringdata0))
        return static_cast<void*>(const_cast< TabWidgetElement*>(this));
    return QWidget::qt_metacast(_clname);
}

int CoreGUI::TabWidgetElement::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CoreGUI::TabWidgetElement::titleChanged(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CoreGUI::TabWidgetElement::documentCleanChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CoreGUI::TabWidgetElement::explicitImportNamesRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
