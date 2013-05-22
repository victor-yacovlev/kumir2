/****************************************************************************
** Meta object code from reading C++ file 'kumfiledialog.h'
**
** Created: Thu Dec 1 12:58:59 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "kumfiledialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kumfiledialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_KumFileDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   15,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_KumFileDialog[] = {
    "KumFileDialog\0\0filter\0checkFilter(QString)\0"
};

const QMetaObject KumFileDialog::staticMetaObject = {
    { &QFileDialog::staticMetaObject, qt_meta_stringdata_KumFileDialog,
      qt_meta_data_KumFileDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KumFileDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KumFileDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KumFileDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KumFileDialog))
        return static_cast<void*>(const_cast< KumFileDialog*>(this));
    return QFileDialog::qt_metacast(_clname);
}

int KumFileDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFileDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: checkFilter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
