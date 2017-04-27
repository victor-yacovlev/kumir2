/****************************************************************************
** Meta object code from reading C++ file 'painterwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/actors/painter/painterwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'painterwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ActorPainter__PainterWindow_t {
    QByteArrayData data[21];
    char stringdata0[238];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ActorPainter__PainterWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ActorPainter__PainterWindow_t qt_meta_stringdata_ActorPainter__PainterWindow = {
    {
QT_MOC_LITERAL(0, 0, 27), // "ActorPainter::PainterWindow"
QT_MOC_LITERAL(1, 28, 8), // "newImage"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 9), // "loadImage"
QT_MOC_LITERAL(4, 48, 11), // "saveImageAs"
QT_MOC_LITERAL(5, 60, 26), // "handleColorTextModeChanged"
QT_MOC_LITERAL(6, 87, 17), // "handleCursorMoved"
QT_MOC_LITERAL(7, 105, 1), // "x"
QT_MOC_LITERAL(8, 107, 1), // "y"
QT_MOC_LITERAL(9, 109, 5), // "color"
QT_MOC_LITERAL(10, 115, 15), // "saveImageToFile"
QT_MOC_LITERAL(11, 131, 8), // "fileName"
QT_MOC_LITERAL(12, 140, 16), // "handleFullScreen"
QT_MOC_LITERAL(13, 157, 11), // "handleScale"
QT_MOC_LITERAL(14, 169, 5), // "reset"
QT_MOC_LITERAL(15, 175, 10), // "changeZoom"
QT_MOC_LITERAL(16, 186, 6), // "factor"
QT_MOC_LITERAL(17, 193, 7), // "setZoom"
QT_MOC_LITERAL(18, 201, 5), // "scale"
QT_MOC_LITERAL(19, 207, 17), // "updateWindowTitle"
QT_MOC_LITERAL(20, 225, 12) // "documentName"

    },
    "ActorPainter::PainterWindow\0newImage\0"
    "\0loadImage\0saveImageAs\0"
    "handleColorTextModeChanged\0handleCursorMoved\0"
    "x\0y\0color\0saveImageToFile\0fileName\0"
    "handleFullScreen\0handleScale\0reset\0"
    "changeZoom\0factor\0setZoom\0scale\0"
    "updateWindowTitle\0documentName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ActorPainter__PainterWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x0a /* Public */,
       3,    0,   75,    2, 0x0a /* Public */,
       4,    0,   76,    2, 0x0a /* Public */,
       5,    0,   77,    2, 0x08 /* Private */,
       6,    3,   78,    2, 0x08 /* Private */,
      10,    1,   85,    2, 0x08 /* Private */,
      12,    0,   88,    2, 0x08 /* Private */,
      13,    0,   89,    2, 0x08 /* Private */,
      14,    0,   90,    2, 0x08 /* Private */,
      15,    1,   91,    2, 0x08 /* Private */,
      17,    1,   94,    2, 0x08 /* Private */,
      19,    2,   97,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QColor,    7,    8,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void, QMetaType::QReal,   18,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   11,   20,

       0        // eod
};

void ActorPainter::PainterWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PainterWindow *_t = static_cast<PainterWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newImage(); break;
        case 1: _t->loadImage(); break;
        case 2: _t->saveImageAs(); break;
        case 3: _t->handleColorTextModeChanged(); break;
        case 4: _t->handleCursorMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QColor(*)>(_a[3]))); break;
        case 5: _t->saveImageToFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->handleFullScreen(); break;
        case 7: _t->handleScale(); break;
        case 8: _t->reset(); break;
        case 9: _t->changeZoom((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->setZoom((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 11: _t->updateWindowTitle((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject ActorPainter::PainterWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ActorPainter__PainterWindow.data,
      qt_meta_data_ActorPainter__PainterWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ActorPainter::PainterWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ActorPainter::PainterWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ActorPainter__PainterWindow.stringdata0))
        return static_cast<void*>(const_cast< PainterWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int ActorPainter::PainterWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
