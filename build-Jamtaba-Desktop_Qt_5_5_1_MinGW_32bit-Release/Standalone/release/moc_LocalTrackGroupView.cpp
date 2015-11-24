/****************************************************************************
** Meta object code from reading C++ file 'LocalTrackGroupView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/gui/LocalTrackGroupView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LocalTrackGroupView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LocalTrackGroupView_t {
    QByteArrayData data[19];
    char stringdata0[343];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LocalTrackGroupView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LocalTrackGroupView_t qt_meta_stringdata_LocalTrackGroupView = {
    {
QT_MOC_LITERAL(0, 0, 19), // "LocalTrackGroupView"
QT_MOC_LITERAL(1, 20, 11), // "nameChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 12), // "trackRemoved"
QT_MOC_LITERAL(4, 46, 10), // "trackAdded"
QT_MOC_LITERAL(5, 57, 12), // "presetLoaded"
QT_MOC_LITERAL(6, 70, 11), // "presetSaved"
QT_MOC_LITERAL(7, 82, 20), // "on_toolButtonClicked"
QT_MOC_LITERAL(8, 103, 23), // "on_addSubChannelClicked"
QT_MOC_LITERAL(9, 127, 20), // "on_addChannelClicked"
QT_MOC_LITERAL(10, 148, 26), // "on_removeSubchannelHovered"
QT_MOC_LITERAL(11, 175, 23), // "on_removeChannelHovered"
QT_MOC_LITERAL(12, 199, 26), // "on_removeSubChannelClicked"
QT_MOC_LITERAL(13, 226, 23), // "on_removeChannelClicked"
QT_MOC_LITERAL(14, 250, 20), // "on_LoadPresetClicked"
QT_MOC_LITERAL(15, 271, 20), // "on_SavePresetClicked"
QT_MOC_LITERAL(16, 292, 21), // "on_ResetPresetClicked"
QT_MOC_LITERAL(17, 314, 20), // "on_xmitButtonClicked"
QT_MOC_LITERAL(18, 335, 7) // "checked"

    },
    "LocalTrackGroupView\0nameChanged\0\0"
    "trackRemoved\0trackAdded\0presetLoaded\0"
    "presetSaved\0on_toolButtonClicked\0"
    "on_addSubChannelClicked\0on_addChannelClicked\0"
    "on_removeSubchannelHovered\0"
    "on_removeChannelHovered\0"
    "on_removeSubChannelClicked\0"
    "on_removeChannelClicked\0on_LoadPresetClicked\0"
    "on_SavePresetClicked\0on_ResetPresetClicked\0"
    "on_xmitButtonClicked\0checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LocalTrackGroupView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,
       3,    0,   95,    2, 0x06 /* Public */,
       4,    0,   96,    2, 0x06 /* Public */,
       5,    0,   97,    2, 0x06 /* Public */,
       6,    0,   98,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   99,    2, 0x08 /* Private */,
       8,    0,  100,    2, 0x08 /* Private */,
       9,    0,  101,    2, 0x08 /* Private */,
      10,    0,  102,    2, 0x08 /* Private */,
      11,    0,  103,    2, 0x08 /* Private */,
      12,    0,  104,    2, 0x08 /* Private */,
      13,    0,  105,    2, 0x08 /* Private */,
      14,    0,  106,    2, 0x08 /* Private */,
      15,    0,  107,    2, 0x08 /* Private */,
      16,    0,  108,    2, 0x08 /* Private */,
      17,    1,  109,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   18,

       0        // eod
};

void LocalTrackGroupView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LocalTrackGroupView *_t = static_cast<LocalTrackGroupView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nameChanged(); break;
        case 1: _t->trackRemoved(); break;
        case 2: _t->trackAdded(); break;
        case 3: _t->presetLoaded(); break;
        case 4: _t->presetSaved(); break;
        case 5: _t->on_toolButtonClicked(); break;
        case 6: _t->on_addSubChannelClicked(); break;
        case 7: _t->on_addChannelClicked(); break;
        case 8: _t->on_removeSubchannelHovered(); break;
        case 9: _t->on_removeChannelHovered(); break;
        case 10: _t->on_removeSubChannelClicked(); break;
        case 11: _t->on_removeChannelClicked(); break;
        case 12: _t->on_LoadPresetClicked(); break;
        case 13: _t->on_SavePresetClicked(); break;
        case 14: _t->on_ResetPresetClicked(); break;
        case 15: _t->on_xmitButtonClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LocalTrackGroupView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LocalTrackGroupView::nameChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (LocalTrackGroupView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LocalTrackGroupView::trackRemoved)) {
                *result = 1;
            }
        }
        {
            typedef void (LocalTrackGroupView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LocalTrackGroupView::trackAdded)) {
                *result = 2;
            }
        }
        {
            typedef void (LocalTrackGroupView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LocalTrackGroupView::presetLoaded)) {
                *result = 3;
            }
        }
        {
            typedef void (LocalTrackGroupView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LocalTrackGroupView::presetSaved)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject LocalTrackGroupView::staticMetaObject = {
    { &TrackGroupView::staticMetaObject, qt_meta_stringdata_LocalTrackGroupView.data,
      qt_meta_data_LocalTrackGroupView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LocalTrackGroupView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LocalTrackGroupView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LocalTrackGroupView.stringdata0))
        return static_cast<void*>(const_cast< LocalTrackGroupView*>(this));
    return TrackGroupView::qt_metacast(_clname);
}

int LocalTrackGroupView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TrackGroupView::qt_metacall(_c, _id, _a);
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
void LocalTrackGroupView::nameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void LocalTrackGroupView::trackRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void LocalTrackGroupView::trackAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void LocalTrackGroupView::presetLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void LocalTrackGroupView::presetSaved()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
