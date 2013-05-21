/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Fri May 11 09:53:52 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindowTask[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,
      30,   15,   15,   15, 0x0a,
      43,   15,   15,   15, 0x0a,
      58,   15,   15,   15, 0x0a,
      71,   15,   15,   15, 0x0a,
      88,   15,   15,   15, 0x0a,
     109,  103,   15,   15, 0x0a,
     140,  131,   15,   15, 0x0a,
     158,   15,   15,   15, 0x0a,
     170,   15,   15,   15, 0x0a,
     182,   15,   15,   15, 0x0a,
     199,  194,   15,   15, 0x0a,
     212,   15,   15,   15, 0x0a,
     224,   15,   15,   15, 0x0a,
     236,  232,   15,   15, 0x0a,
     271,   15,   15,   15, 0x0a,
     281,   15,   15,   15, 0x0a,
     295,   15,   15,   15, 0x0a,
     308,   15,   15,   15, 0x0a,
     319,   15,   15,   15, 0x0a,
     332,   15,   15,   15, 0x0a,
     348,  343,   15,   15, 0x0a,
     373,   15,   15,   15, 0x0a,
     382,   15,   15,   15, 0x0a,
     393,   15,   15,   15, 0x0a,
     403,   15,   15,   15, 0x0a,
     417,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindowTask[] = {
    "MainWindowTask\0\0aboutToQuit()\0"
    "loadCourse()\0returnTested()\0saveCourse()\0"
    "saveCourseFile()\0saveBaseKurs()\0index\0"
    "showText(QModelIndex)\0fileName\0"
    "loadHtml(QString)\0startTask()\0checkTask()\0"
    "lockCheck()\0mark\0setMark(int)\0resetTask()\0"
    "Close()\0pos\0customContextMenuRequested(QPoint)\0"
    "addTask()\0addDeepTask()\0deleteTask()\0"
    "saveKurs()\0saveKursAs()\0editTask()\0"
    "flag\0setEditTaskEnabled(bool)\0moveUp()\0"
    "moveDown()\0newKurs()\0endRootEdit()\0"
    "cancelRootEdit()\0"
};

const QMetaObject MainWindowTask::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindowTask,
      qt_meta_data_MainWindowTask, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindowTask::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindowTask::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindowTask::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindowTask))
        return static_cast<void*>(const_cast< MainWindowTask*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindowTask::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: aboutToQuit(); break;
        case 1: loadCourse(); break;
        case 2: returnTested(); break;
        case 3: saveCourse(); break;
        case 4: saveCourseFile(); break;
        case 5: saveBaseKurs(); break;
        case 6: showText((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 7: loadHtml((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: startTask(); break;
        case 9: checkTask(); break;
        case 10: lockCheck(); break;
        case 11: setMark((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: resetTask(); break;
        case 13: Close(); break;
        case 14: customContextMenuRequested((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 15: addTask(); break;
        case 16: addDeepTask(); break;
        case 17: deleteTask(); break;
        case 18: saveKurs(); break;
        case 19: saveKursAs(); break;
        case 20: editTask(); break;
        case 21: setEditTaskEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: moveUp(); break;
        case 23: moveDown(); break;
        case 24: newKurs(); break;
        case 25: endRootEdit(); break;
        case 26: cancelRootEdit(); break;
        default: ;
        }
        _id -= 27;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
