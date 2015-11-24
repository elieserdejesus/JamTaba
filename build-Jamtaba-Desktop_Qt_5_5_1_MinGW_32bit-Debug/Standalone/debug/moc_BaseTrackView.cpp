/****************************************************************************
** Meta object code from reading C++ file 'BaseTrackView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/gui/BaseTrackView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BaseTrackView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_BaseTrackView_t {
    QByteArrayData data[16];
    char stringdata0[244];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BaseTrackView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BaseTrackView_t qt_meta_stringdata_BaseTrackView = {
    {
QT_MOC_LITERAL(0, 0, 13), // "BaseTrackView"
QT_MOC_LITERAL(1, 14, 13), // "onMuteClicked"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 13), // "onSoloClicked"
QT_MOC_LITERAL(4, 43, 12), // "onFaderMoved"
QT_MOC_LITERAL(5, 56, 5), // "value"
QT_MOC_LITERAL(6, 62, 16), // "onPanSliderMoved"
QT_MOC_LITERAL(7, 79, 20), // "onBoostButtonClicked"
QT_MOC_LITERAL(8, 100, 21), // "onAudioNodePanChanged"
QT_MOC_LITERAL(9, 122, 11), // "newPanValue"
QT_MOC_LITERAL(10, 134, 22), // "onAudioNodeGainChanged"
QT_MOC_LITERAL(11, 157, 12), // "newGainValue"
QT_MOC_LITERAL(12, 170, 22), // "onAudioNodeMuteChanged"
QT_MOC_LITERAL(13, 193, 13), // "newMuteStatus"
QT_MOC_LITERAL(14, 207, 22), // "onAudioNodeSoloChanged"
QT_MOC_LITERAL(15, 230, 13) // "newSoloStatus"

    },
    "BaseTrackView\0onMuteClicked\0\0onSoloClicked\0"
    "onFaderMoved\0value\0onPanSliderMoved\0"
    "onBoostButtonClicked\0onAudioNodePanChanged\0"
    "newPanValue\0onAudioNodeGainChanged\0"
    "newGainValue\0onAudioNodeMuteChanged\0"
    "newMuteStatus\0onAudioNodeSoloChanged\0"
    "newSoloStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BaseTrackView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    1,   61,    2, 0x08 /* Private */,
       6,    1,   64,    2, 0x08 /* Private */,
       7,    0,   67,    2, 0x08 /* Private */,
       8,    1,   68,    2, 0x08 /* Private */,
      10,    1,   71,    2, 0x08 /* Private */,
      12,    1,   74,    2, 0x08 /* Private */,
      14,    1,   77,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    9,
    QMetaType::Void, QMetaType::Float,   11,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Bool,   15,

       0        // eod
};

void BaseTrackView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BaseTrackView *_t = static_cast<BaseTrackView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onMuteClicked(); break;
        case 1: _t->onSoloClicked(); break;
        case 2: _t->onFaderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onPanSliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onBoostButtonClicked(); break;
        case 5: _t->onAudioNodePanChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 6: _t->onAudioNodeGainChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 7: _t->onAudioNodeMuteChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->onAudioNodeSoloChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject BaseTrackView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BaseTrackView.data,
      qt_meta_data_BaseTrackView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *BaseTrackView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BaseTrackView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_BaseTrackView.stringdata0))
        return static_cast<void*>(const_cast< BaseTrackView*>(this));
    return QWidget::qt_metacast(_clname);
}

int BaseTrackView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
