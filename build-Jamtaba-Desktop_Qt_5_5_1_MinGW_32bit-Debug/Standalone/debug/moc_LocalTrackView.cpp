/****************************************************************************
** Meta object code from reading C++ file 'LocalTrackView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/gui/LocalTrackView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LocalTrackView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LocalTrackView_t {
    QByteArrayData data[8];
    char stringdata0[158];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LocalTrackView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LocalTrackView_t qt_meta_stringdata_LocalTrackView = {
    {
QT_MOC_LITERAL(0, 0, 14), // "LocalTrackView"
QT_MOC_LITERAL(1, 15, 30), // "on_inputSelectionButtonClicked"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 24), // "on_monoInputMenuSelected"
QT_MOC_LITERAL(4, 72, 8), // "QAction*"
QT_MOC_LITERAL(5, 81, 26), // "on_stereoInputMenuSelected"
QT_MOC_LITERAL(6, 108, 26), // "on_MidiInputDeviceSelected"
QT_MOC_LITERAL(7, 135, 22) // "on_noInputMenuSelected"

    },
    "LocalTrackView\0on_inputSelectionButtonClicked\0"
    "\0on_monoInputMenuSelected\0QAction*\0"
    "on_stereoInputMenuSelected\0"
    "on_MidiInputDeviceSelected\0"
    "on_noInputMenuSelected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LocalTrackView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    1,   40,    2, 0x08 /* Private */,
       5,    1,   43,    2, 0x08 /* Private */,
       6,    1,   46,    2, 0x08 /* Private */,
       7,    0,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,

       0        // eod
};

void LocalTrackView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LocalTrackView *_t = static_cast<LocalTrackView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_inputSelectionButtonClicked(); break;
        case 1: _t->on_monoInputMenuSelected((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: _t->on_stereoInputMenuSelected((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: _t->on_MidiInputDeviceSelected((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 4: _t->on_noInputMenuSelected(); break;
        default: ;
        }
    }
}

const QMetaObject LocalTrackView::staticMetaObject = {
    { &BaseTrackView::staticMetaObject, qt_meta_stringdata_LocalTrackView.data,
      qt_meta_data_LocalTrackView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LocalTrackView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LocalTrackView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LocalTrackView.stringdata0))
        return static_cast<void*>(const_cast< LocalTrackView*>(this));
    return BaseTrackView::qt_metacast(_clname);
}

int LocalTrackView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseTrackView::qt_metacall(_c, _id, _a);
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
    return _id;
}
QT_END_MOC_NAMESPACE
