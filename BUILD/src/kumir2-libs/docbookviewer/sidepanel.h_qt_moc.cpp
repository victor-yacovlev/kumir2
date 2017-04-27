/****************************************************************************
** Meta object code from reading C++ file 'sidepanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/docbookviewer/sidepanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sidepanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DocBookViewer__SidePanel_t {
    QByteArrayData data[15];
    char stringdata0[191];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DocBookViewer__SidePanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DocBookViewer__SidePanel_t qt_meta_stringdata_DocBookViewer__SidePanel = {
    {
QT_MOC_LITERAL(0, 0, 24), // "DocBookViewer::SidePanel"
QT_MOC_LITERAL(1, 25, 10), // "itemPicked"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 8), // "ModelPtr"
QT_MOC_LITERAL(4, 46, 5), // "model"
QT_MOC_LITERAL(5, 52, 10), // "selectItem"
QT_MOC_LITERAL(6, 63, 10), // "searchText"
QT_MOC_LITERAL(7, 74, 22), // "clearNavigationFilters"
QT_MOC_LITERAL(8, 97, 17), // "focusToSearchLine"
QT_MOC_LITERAL(9, 115, 18), // "hadleButtonPressed"
QT_MOC_LITERAL(10, 134, 20), // "selectTreeWidgetItem"
QT_MOC_LITERAL(11, 155, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(12, 172, 4), // "item"
QT_MOC_LITERAL(13, 177, 8), // "doFilter"
QT_MOC_LITERAL(14, 186, 4) // "text"

    },
    "DocBookViewer::SidePanel\0itemPicked\0"
    "\0ModelPtr\0model\0selectItem\0searchText\0"
    "clearNavigationFilters\0focusToSearchLine\0"
    "hadleButtonPressed\0selectTreeWidgetItem\0"
    "QTreeWidgetItem*\0item\0doFilter\0text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DocBookViewer__SidePanel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   57,    2, 0x0a /* Public */,
       5,    2,   60,    2, 0x0a /* Public */,
       7,    0,   65,    2, 0x0a /* Public */,
       8,    0,   66,    2, 0x0a /* Public */,
       9,    0,   67,    2, 0x08 /* Private */,
      10,    1,   68,    2, 0x08 /* Private */,
      13,    1,   71,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::QString,   14,

       0        // eod
};

void DocBookViewer::SidePanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SidePanel *_t = static_cast<SidePanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemPicked((*reinterpret_cast< ModelPtr(*)>(_a[1]))); break;
        case 1: _t->selectItem((*reinterpret_cast< ModelPtr(*)>(_a[1]))); break;
        case 2: _t->selectItem((*reinterpret_cast< ModelPtr(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->clearNavigationFilters(); break;
        case 4: _t->focusToSearchLine(); break;
        case 5: _t->hadleButtonPressed(); break;
        case 6: _t->selectTreeWidgetItem((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 7: _t->doFilter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SidePanel::*_t)(ModelPtr );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SidePanel::itemPicked)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject DocBookViewer::SidePanel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DocBookViewer__SidePanel.data,
      qt_meta_data_DocBookViewer__SidePanel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DocBookViewer::SidePanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocBookViewer::SidePanel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DocBookViewer__SidePanel.stringdata0))
        return static_cast<void*>(const_cast< SidePanel*>(this));
    return QWidget::qt_metacast(_clname);
}

int DocBookViewer::SidePanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void DocBookViewer::SidePanel::itemPicked(ModelPtr _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
