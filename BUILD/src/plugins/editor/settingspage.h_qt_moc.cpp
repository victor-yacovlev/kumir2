/****************************************************************************
** Meta object code from reading C++ file 'settingspage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/plugins/editor/settingspage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settingspage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor__SettingsPage_t {
    QByteArrayData data[21];
    char stringdata0[290];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor__SettingsPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor__SettingsPage_t qt_meta_stringdata_Editor__SettingsPage = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Editor::SettingsPage"
QT_MOC_LITERAL(1, 21, 15), // "settingsChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 4), // "keys"
QT_MOC_LITERAL(4, 43, 6), // "accept"
QT_MOC_LITERAL(5, 50, 4), // "init"
QT_MOC_LITERAL(6, 55, 15), // "resetToDefaults"
QT_MOC_LITERAL(7, 71, 15), // "showColorDialog"
QT_MOC_LITERAL(8, 87, 14), // "setButtonColor"
QT_MOC_LITERAL(9, 102, 12), // "QToolButton*"
QT_MOC_LITERAL(10, 115, 1), // "b"
QT_MOC_LITERAL(11, 117, 5), // "color"
QT_MOC_LITERAL(12, 123, 11), // "buttonColor"
QT_MOC_LITERAL(13, 135, 18), // "const QToolButton*"
QT_MOC_LITERAL(14, 154, 17), // "updateFontPreview"
QT_MOC_LITERAL(15, 172, 27), // "validateProgramTemplateFile"
QT_MOC_LITERAL(16, 200, 32), // "browseInitialProgramTemplateFile"
QT_MOC_LITERAL(17, 233, 22), // "FreeCursorMovementType"
QT_MOC_LITERAL(18, 256, 16), // "CommentsAndTexts"
QT_MOC_LITERAL(19, 273, 6), // "Always"
QT_MOC_LITERAL(20, 280, 9) // "TextsOnly"

    },
    "Editor::SettingsPage\0settingsChanged\0"
    "\0keys\0accept\0init\0resetToDefaults\0"
    "showColorDialog\0setButtonColor\0"
    "QToolButton*\0b\0color\0buttonColor\0"
    "const QToolButton*\0updateFontPreview\0"
    "validateProgramTemplateFile\0"
    "browseInitialProgramTemplateFile\0"
    "FreeCursorMovementType\0CommentsAndTexts\0"
    "Always\0TextsOnly"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor__SettingsPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       1,   82, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   67,    2, 0x0a /* Public */,
       5,    0,   68,    2, 0x0a /* Public */,
       6,    0,   69,    2, 0x0a /* Public */,
       7,    0,   70,    2, 0x08 /* Private */,
       8,    2,   71,    2, 0x08 /* Private */,
      12,    1,   76,    2, 0x08 /* Private */,
      14,    0,   79,    2, 0x08 /* Private */,
      15,    0,   80,    2, 0x08 /* Private */,
      16,    0,   81,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9, QMetaType::QColor,   10,   11,
    QMetaType::QColor, 0x80000000 | 13,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // enums: name, flags, count, data
      17, 0x0,    3,   86,

 // enum data: key, value
      18, uint(Editor::SettingsPage::CommentsAndTexts),
      19, uint(Editor::SettingsPage::Always),
      20, uint(Editor::SettingsPage::TextsOnly),

       0        // eod
};

void Editor::SettingsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SettingsPage *_t = static_cast<SettingsPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->settingsChanged((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 1: _t->accept(); break;
        case 2: _t->init(); break;
        case 3: _t->resetToDefaults(); break;
        case 4: _t->showColorDialog(); break;
        case 5: _t->setButtonColor((*reinterpret_cast< QToolButton*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 6: { QColor _r = _t->buttonColor((*reinterpret_cast< const QToolButton*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QColor*>(_a[0]) = _r; }  break;
        case 7: _t->updateFontPreview(); break;
        case 8: _t->validateProgramTemplateFile(); break;
        case 9: _t->browseInitialProgramTemplateFile(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SettingsPage::*_t)(const QStringList & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SettingsPage::settingsChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Editor::SettingsPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Editor__SettingsPage.data,
      qt_meta_data_Editor__SettingsPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Editor::SettingsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::SettingsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__SettingsPage.stringdata0))
        return static_cast<void*>(const_cast< SettingsPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int Editor::SettingsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Editor::SettingsPage::settingsChanged(const QStringList & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
