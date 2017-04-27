/****************************************************************************
** Meta object code from reading C++ file 'defaultstartpage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/coregui/defaultstartpage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'defaultstartpage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CoreGUI__DefaultStartPage_t {
    QByteArrayData data[7];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CoreGUI__DefaultStartPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CoreGUI__DefaultStartPage_t qt_meta_stringdata_CoreGUI__DefaultStartPage = {
    {
QT_MOC_LITERAL(0, 0, 25), // "CoreGUI::DefaultStartPage"
QT_MOC_LITERAL(1, 26, 24), // "handleShowCoursesPressed"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 21), // "handleShowHelpPressed"
QT_MOC_LITERAL(4, 74, 23), // "handleRecentFileClicked"
QT_MOC_LITERAL(5, 98, 25), // "handleRecentCourseClicked"
QT_MOC_LITERAL(6, 124, 21) // "handleHelpItemClicked"

    },
    "CoreGUI::DefaultStartPage\0"
    "handleShowCoursesPressed\0\0"
    "handleShowHelpPressed\0handleRecentFileClicked\0"
    "handleRecentCourseClicked\0"
    "handleHelpItemClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CoreGUI__DefaultStartPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x09 /* Protected */,
       3,    0,   40,    2, 0x09 /* Protected */,
       4,    0,   41,    2, 0x09 /* Protected */,
       5,    0,   42,    2, 0x09 /* Protected */,
       6,    0,   43,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CoreGUI::DefaultStartPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DefaultStartPage *_t = static_cast<DefaultStartPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->handleShowCoursesPressed(); break;
        case 1: _t->handleShowHelpPressed(); break;
        case 2: _t->handleRecentFileClicked(); break;
        case 3: _t->handleRecentCourseClicked(); break;
        case 4: _t->handleHelpItemClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CoreGUI::DefaultStartPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CoreGUI__DefaultStartPage.data,
      qt_meta_data_CoreGUI__DefaultStartPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CoreGUI::DefaultStartPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoreGUI::DefaultStartPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CoreGUI__DefaultStartPage.stringdata0))
        return static_cast<void*>(const_cast< DefaultStartPage*>(this));
    if (!strcmp(_clname, "Shared::StartpageWidgetInterface"))
        return static_cast< Shared::StartpageWidgetInterface*>(const_cast< DefaultStartPage*>(this));
    if (!strcmp(_clname, "kumir2.startpage"))
        return static_cast< Shared::StartpageWidgetInterface*>(const_cast< DefaultStartPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int CoreGUI::DefaultStartPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
