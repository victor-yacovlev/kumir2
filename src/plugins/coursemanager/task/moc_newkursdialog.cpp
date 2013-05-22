/****************************************************************************
** Meta object code from reading C++ file 'newkursdialog.h'
**
** Created: Thu Apr 26 15:28:19 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "newkursdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'newkursdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_newKursDialog[] = {

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
      15,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_newKursDialog[] = {
    "newKursDialog\0\0showFileSelect()\0"
};

const QMetaObject newKursDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_newKursDialog,
      qt_meta_data_newKursDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &newKursDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *newKursDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *newKursDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_newKursDialog))
        return static_cast<void*>(const_cast< newKursDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int newKursDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showFileSelect(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
