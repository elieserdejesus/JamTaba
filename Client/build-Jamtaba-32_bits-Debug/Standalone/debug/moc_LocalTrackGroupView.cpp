/****************************************************************************
** Meta object code from reading C++ file 'LocalTrackGroupView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/gui/LocalTrackGroupView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LocalTrackGroupView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LocalTrackGroupView_t {
    QByteArrayData data[10];
    char stringdata0[201];
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
QT_MOC_LITERAL(3, 33, 20), // "on_toolButtonClicked"
QT_MOC_LITERAL(4, 54, 23), // "on_addSubChannelClicked"
QT_MOC_LITERAL(5, 78, 20), // "on_addChannelClicked"
QT_MOC_LITERAL(6, 99, 26), // "on_removeSubchannelHovered"
QT_MOC_LITERAL(7, 126, 23), // "on_removeChannelHovered"
QT_MOC_LITERAL(8, 150, 26), // "on_removeSubChannelClicked"
QT_MOC_LITERAL(9, 177, 23) // "on_removeChannelClicked"

    },
    "LocalTrackGroupView\0nameChanged\0\0"
    "on_toolButtonClicked\0on_addSubChannelClicked\0"
    "on_addChannelClicked\0on_removeSubchannelHovered\0"
    "on_removeChannelHovered\0"
    "on_removeSubChannelClicked\0"
    "on_removeChannelClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LocalTrackGroupView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   55,    2, 0x08 /* Private */,
       4,    0,   56,    2, 0x08 /* Private */,
       5,    0,   57,    2, 0x08 /* Private */,
       6,    0,   58,    2, 0x08 /* Private */,
       7,    0,   59,    2, 0x08 /* Private */,
       8,    0,   60,    2, 0x08 /* Private */,
       9,    0,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void LocalTrackGroupView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LocalTrackGroupView *_t = static_cast<LocalTrackGroupView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nameChanged(); break;
        case 1: _t->on_toolButtonClicked(); break;
        case 2: _t->on_addSubChannelClicked(); break;
        case 3: _t->on_addChannelClicked(); break;
        case 4: _t->on_removeSubchannelHovered(); break;
        case 5: _t->on_removeChannelHovered(); break;
        case 6: _t->on_removeSubChannelClicked(); break;
        case 7: _t->on_removeChannelClicked(); break;
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
    }
    Q_UNUSED(_a);
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
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void LocalTrackGroupView::nameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
