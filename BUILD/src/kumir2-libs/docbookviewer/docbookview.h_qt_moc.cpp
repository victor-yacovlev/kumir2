/****************************************************************************
** Meta object code from reading C++ file 'docbookview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/kumir2-libs/docbookviewer/docbookview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'docbookview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DocBookViewer__DocBookView_t {
    QByteArrayData data[1];
    char stringdata0[27];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DocBookViewer__DocBookView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DocBookViewer__DocBookView_t qt_meta_stringdata_DocBookViewer__DocBookView = {
    {
QT_MOC_LITERAL(0, 0, 26) // "DocBookViewer::DocBookView"

    },
    "DocBookViewer::DocBookView"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DocBookViewer__DocBookView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void DocBookViewer::DocBookView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject DocBookViewer::DocBookView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DocBookViewer__DocBookView.data,
      qt_meta_data_DocBookViewer__DocBookView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DocBookViewer::DocBookView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocBookViewer::DocBookView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DocBookViewer__DocBookView.stringdata0))
        return static_cast<void*>(const_cast< DocBookView*>(this));
    return QWidget::qt_metacast(_clname);
}

int DocBookViewer::DocBookView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
