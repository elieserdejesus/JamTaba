/****************************************************************************
** Meta object code from reading C++ file 'FxPanelItem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/gui/FxPanelItem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FxPanelItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FxPanelItem_t {
    QByteArrayData data[11];
    char stringdata0[129];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FxPanelItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FxPanelItem_t qt_meta_stringdata_FxPanelItem = {
    {
QT_MOC_LITERAL(0, 0, 11), // "FxPanelItem"
QT_MOC_LITERAL(1, 12, 14), // "on_contextMenu"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 1), // "p"
QT_MOC_LITERAL(4, 30, 16), // "on_buttonClicked"
QT_MOC_LITERAL(5, 47, 22), // "on_actionMenuTriggered"
QT_MOC_LITERAL(6, 70, 8), // "QAction*"
QT_MOC_LITERAL(7, 79, 1), // "a"
QT_MOC_LITERAL(8, 81, 24), // "on_fxMenuActionTriggered"
QT_MOC_LITERAL(9, 106, 8), // "bypassed"
QT_MOC_LITERAL(10, 115, 13) // "containPlugin"

    },
    "FxPanelItem\0on_contextMenu\0\0p\0"
    "on_buttonClicked\0on_actionMenuTriggered\0"
    "QAction*\0a\0on_fxMenuActionTriggered\0"
    "bypassed\0containPlugin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FxPanelItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       2,   44, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08 /* Private */,
       4,    0,   37,    2, 0x08 /* Private */,
       5,    1,   38,    2, 0x08 /* Private */,
       8,    1,   41,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,

 // properties: name, type, flags
       9, QMetaType::Bool, 0x00095001,
      10, QMetaType::Bool, 0x00095001,

       0        // eod
};

void FxPanelItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FxPanelItem *_t = static_cast<FxPanelItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_contextMenu((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 1: _t->on_buttonClicked(); break;
        case 2: _t->on_actionMenuTriggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: _t->on_fxMenuActionTriggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        FxPanelItem *_t = static_cast<FxPanelItem *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->pluginIsBypassed(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->containPlugin(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject FxPanelItem::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FxPanelItem.data,
      qt_meta_data_FxPanelItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FxPanelItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FxPanelItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FxPanelItem.stringdata0))
        return static_cast<void*>(const_cast< FxPanelItem*>(this));
    return QWidget::qt_metacast(_clname);
}

int FxPanelItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
