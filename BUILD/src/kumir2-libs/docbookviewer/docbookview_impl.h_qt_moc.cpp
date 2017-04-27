/****************************************************************************
** Meta object code from reading C++ file 'docbookview_impl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/docbookviewer/docbookview_impl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'docbookview_impl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DocBookViewer__DocBookViewImpl_t {
    QByteArrayData data[21];
    char stringdata0[284];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DocBookViewer__DocBookViewImpl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DocBookViewer__DocBookViewImpl_t qt_meta_stringdata_DocBookViewer__DocBookViewImpl = {
    {
QT_MOC_LITERAL(0, 0, 30), // "DocBookViewer::DocBookViewImpl"
QT_MOC_LITERAL(1, 31, 12), // "itemSelected"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 8), // "ModelPtr"
QT_MOC_LITERAL(4, 54, 5), // "model"
QT_MOC_LITERAL(5, 60, 10), // "showAnItem"
QT_MOC_LITERAL(6, 71, 15), // "showPrintDialog"
QT_MOC_LITERAL(7, 87, 13), // "hideSidePanel"
QT_MOC_LITERAL(8, 101, 13), // "showSidePanel"
QT_MOC_LITERAL(9, 115, 14), // "setInitialView"
QT_MOC_LITERAL(10, 130, 7), // "setSize"
QT_MOC_LITERAL(11, 138, 4), // "size"
QT_MOC_LITERAL(12, 143, 19), // "switchToCompactMode"
QT_MOC_LITERAL(13, 163, 5), // "force"
QT_MOC_LITERAL(14, 169, 23), // "switchToEnoughtSizeMode"
QT_MOC_LITERAL(15, 193, 9), // "booksList"
QT_MOC_LITERAL(16, 203, 17), // "activateBookIndex"
QT_MOC_LITERAL(17, 221, 5), // "index"
QT_MOC_LITERAL(18, 227, 25), // "updateToggleSideBarButton"
QT_MOC_LITERAL(19, 253, 23), // "handleShowSideBarButton"
QT_MOC_LITERAL(20, 277, 6) // "action"

    },
    "DocBookViewer::DocBookViewImpl\0"
    "itemSelected\0\0ModelPtr\0model\0showAnItem\0"
    "showPrintDialog\0hideSidePanel\0"
    "showSidePanel\0setInitialView\0setSize\0"
    "size\0switchToCompactMode\0force\0"
    "switchToEnoughtSizeMode\0booksList\0"
    "activateBookIndex\0index\0"
    "updateToggleSideBarButton\0"
    "handleShowSideBarButton\0action"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DocBookViewer__DocBookViewImpl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   97,    2, 0x0a /* Public */,
       6,    0,  100,    2, 0x0a /* Public */,
       7,    0,  101,    2, 0x0a /* Public */,
       8,    0,  102,    2, 0x0a /* Public */,
       9,    0,  103,    2, 0x0a /* Public */,
      10,    1,  104,    2, 0x0a /* Public */,
      12,    1,  107,    2, 0x0a /* Public */,
      12,    0,  110,    2, 0x2a /* Public | MethodCloned */,
      14,    1,  111,    2, 0x0a /* Public */,
      14,    0,  114,    2, 0x2a /* Public | MethodCloned */,
      15,    0,  115,    2, 0x0a /* Public */,
      16,    1,  116,    2, 0x0a /* Public */,
      16,    0,  119,    2, 0x2a /* Public | MethodCloned */,
      18,    0,  120,    2, 0x08 /* Private */,
      19,    1,  121,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QSize,   11,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::QStringList,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   20,

       0        // eod
};

void DocBookViewer::DocBookViewImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DocBookViewImpl *_t = static_cast<DocBookViewImpl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemSelected((*reinterpret_cast< ModelPtr(*)>(_a[1]))); break;
        case 1: _t->showAnItem((*reinterpret_cast< ModelPtr(*)>(_a[1]))); break;
        case 2: _t->showPrintDialog(); break;
        case 3: _t->hideSidePanel(); break;
        case 4: _t->showSidePanel(); break;
        case 5: _t->setInitialView(); break;
        case 6: _t->setSize((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 7: _t->switchToCompactMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->switchToCompactMode(); break;
        case 9: _t->switchToEnoughtSizeMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->switchToEnoughtSizeMode(); break;
        case 11: { QStringList _r = _t->booksList();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        case 12: _t->activateBookIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->activateBookIndex(); break;
        case 14: _t->updateToggleSideBarButton(); break;
        case 15: _t->handleShowSideBarButton((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DocBookViewImpl::*_t)(ModelPtr );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DocBookViewImpl::itemSelected)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject DocBookViewer::DocBookViewImpl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DocBookViewer__DocBookViewImpl.data,
      qt_meta_data_DocBookViewer__DocBookViewImpl,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DocBookViewer::DocBookViewImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocBookViewer::DocBookViewImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DocBookViewer__DocBookViewImpl.stringdata0))
        return static_cast<void*>(const_cast< DocBookViewImpl*>(this));
    return QObject::qt_metacast(_clname);
}

int DocBookViewer::DocBookViewImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void DocBookViewer::DocBookViewImpl::itemSelected(ModelPtr _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
