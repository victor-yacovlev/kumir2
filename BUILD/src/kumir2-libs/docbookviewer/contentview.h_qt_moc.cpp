/****************************************************************************
** Meta object code from reading C++ file 'contentview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/docbookviewer/contentview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'contentview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DocBookViewer__ContentView_t {
    QByteArrayData data[8];
    char stringdata0[97];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DocBookViewer__ContentView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DocBookViewer__ContentView_t qt_meta_stringdata_DocBookViewer__ContentView = {
    {
QT_MOC_LITERAL(0, 0, 26), // "DocBookViewer::ContentView"
QT_MOC_LITERAL(1, 27, 11), // "itemRequest"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 8), // "ModelPtr"
QT_MOC_LITERAL(4, 49, 5), // "model"
QT_MOC_LITERAL(5, 55, 18), // "clearLastAnchorUrl"
QT_MOC_LITERAL(6, 74, 18), // "handleInternalLink"
QT_MOC_LITERAL(7, 93, 3) // "url"

    },
    "DocBookViewer::ContentView\0itemRequest\0"
    "\0ModelPtr\0model\0clearLastAnchorUrl\0"
    "handleInternalLink\0url"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DocBookViewer__ContentView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   32,    2, 0x08 /* Private */,
       6,    1,   33,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl,    7,

       0        // eod
};

void DocBookViewer::ContentView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ContentView *_t = static_cast<ContentView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemRequest((*reinterpret_cast< ModelPtr(*)>(_a[1]))); break;
        case 1: _t->clearLastAnchorUrl(); break;
        case 2: _t->handleInternalLink((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ContentView::*_t)(ModelPtr );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ContentView::itemRequest)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject DocBookViewer::ContentView::staticMetaObject = {
    { &QTextBrowser::staticMetaObject, qt_meta_stringdata_DocBookViewer__ContentView.data,
      qt_meta_data_DocBookViewer__ContentView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DocBookViewer::ContentView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocBookViewer::ContentView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DocBookViewer__ContentView.stringdata0))
        return static_cast<void*>(const_cast< ContentView*>(this));
    if (!strcmp(_clname, "ContentRenderer"))
        return static_cast< ContentRenderer*>(const_cast< ContentView*>(this));
    return QTextBrowser::qt_metacast(_clname);
}

int DocBookViewer::ContentView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextBrowser::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DocBookViewer::ContentView::itemRequest(ModelPtr _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
