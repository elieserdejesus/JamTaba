/****************************************************************************
** Meta object code from reading C++ file 'FxPanelItem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/gui/FxPanelItem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FxPanelItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FxPanelItem_t {
    QByteArrayData data[13];
    char stringdata[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FxPanelItem_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FxPanelItem_t qt_meta_stringdata_FxPanelItem = {
    {
QT_MOC_LITERAL(0, 0, 11), // "FxPanelItem"
QT_MOC_LITERAL(1, 12, 13), // "editingPlugin"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 14), // "Audio::Plugin*"
QT_MOC_LITERAL(4, 42, 6), // "plugin"
QT_MOC_LITERAL(5, 49, 13), // "pluginRemoved"
QT_MOC_LITERAL(6, 63, 14), // "on_contextMenu"
QT_MOC_LITERAL(7, 78, 1), // "p"
QT_MOC_LITERAL(8, 80, 16), // "on_buttonClicked"
QT_MOC_LITERAL(9, 97, 22), // "on_actionMenuTriggered"
QT_MOC_LITERAL(10, 120, 8), // "QAction*"
QT_MOC_LITERAL(11, 129, 1), // "a"
QT_MOC_LITERAL(12, 131, 14) // "pluginBypassed"

    },
    "FxPanelItem\0editingPlugin\0\0Audio::Plugin*\0"
    "plugin\0pluginRemoved\0on_contextMenu\0"
    "p\0on_buttonClicked\0on_actionMenuTriggered\0"
    "QAction*\0a\0pluginBypassed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FxPanelItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       1,   52, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   45,    2, 0x08 /* Private */,
       8,    0,   48,    2, 0x08 /* Private */,
       9,    1,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,

 // properties: name, type, flags
      12, QMetaType::Bool, 0x00095001,

       0        // eod
};

void FxPanelItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FxPanelItem *_t = static_cast<FxPanelItem *>(_o);
        switch (_id) {
        case 0: _t->editingPlugin((*reinterpret_cast< Audio::Plugin*(*)>(_a[1]))); break;
        case 1: _t->pluginRemoved((*reinterpret_cast< Audio::Plugin*(*)>(_a[1]))); break;
        case 2: _t->on_contextMenu((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 3: _t->on_buttonClicked(); break;
        case 4: _t->on_actionMenuTriggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FxPanelItem::*_t)(Audio::Plugin * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FxPanelItem::editingPlugin)) {
                *result = 0;
            }
        }
        {
            typedef void (FxPanelItem::*_t)(Audio::Plugin * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FxPanelItem::pluginRemoved)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject FxPanelItem::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_FxPanelItem.data,
      qt_meta_data_FxPanelItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FxPanelItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FxPanelItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FxPanelItem.stringdata))
        return static_cast<void*>(const_cast< FxPanelItem*>(this));
    return QLabel::qt_metacast(_clname);
}

int FxPanelItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = pluginIsBypassed(); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void FxPanelItem::editingPlugin(Audio::Plugin * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FxPanelItem::pluginRemoved(Audio::Plugin * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
