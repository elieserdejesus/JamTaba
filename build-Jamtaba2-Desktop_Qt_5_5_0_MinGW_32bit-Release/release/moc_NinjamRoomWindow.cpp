/****************************************************************************
** Meta object code from reading C++ file 'NinjamRoomWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/gui/NinjamRoomWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NinjamRoomWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NinjamRoomWindow_t {
    QByteArrayData data[31];
    char stringdata0[448];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NinjamRoomWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NinjamRoomWindow_t qt_meta_stringdata_NinjamRoomWindow = {
    {
QT_MOC_LITERAL(0, 0, 16), // "NinjamRoomWindow"
QT_MOC_LITERAL(1, 17, 21), // "ninjamBpiComboChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 21), // "ninjamBpmComboChanged"
QT_MOC_LITERAL(4, 62, 25), // "ninjamAccentsComboChanged"
QT_MOC_LITERAL(5, 88, 16), // "onPanSliderMoved"
QT_MOC_LITERAL(6, 105, 5), // "value"
QT_MOC_LITERAL(7, 111, 12), // "onFaderMoved"
QT_MOC_LITERAL(8, 124, 13), // "onMuteClicked"
QT_MOC_LITERAL(9, 138, 13), // "onSoloClicked"
QT_MOC_LITERAL(10, 152, 13), // "on_bpiChanged"
QT_MOC_LITERAL(11, 166, 3), // "bpi"
QT_MOC_LITERAL(12, 170, 13), // "on_bpmChanged"
QT_MOC_LITERAL(13, 184, 3), // "bpm"
QT_MOC_LITERAL(14, 188, 22), // "on_intervalBeatChanged"
QT_MOC_LITERAL(15, 211, 4), // "beat"
QT_MOC_LITERAL(16, 216, 15), // "on_channelAdded"
QT_MOC_LITERAL(17, 232, 12), // "Ninjam::User"
QT_MOC_LITERAL(18, 245, 4), // "user"
QT_MOC_LITERAL(19, 250, 19), // "Ninjam::UserChannel"
QT_MOC_LITERAL(20, 270, 7), // "channel"
QT_MOC_LITERAL(21, 278, 9), // "channelID"
QT_MOC_LITERAL(22, 288, 17), // "on_channelRemoved"
QT_MOC_LITERAL(23, 306, 21), // "on_channelNameChanged"
QT_MOC_LITERAL(24, 328, 21), // "on_channelXmitChanged"
QT_MOC_LITERAL(25, 350, 11), // "transmiting"
QT_MOC_LITERAL(26, 362, 22), // "on_chatMessageReceived"
QT_MOC_LITERAL(27, 385, 7), // "message"
QT_MOC_LITERAL(28, 393, 25), // "userSendingNewChatMessage"
QT_MOC_LITERAL(29, 419, 3), // "msg"
QT_MOC_LITERAL(30, 423, 24) // "on_licenceButton_clicked"

    },
    "NinjamRoomWindow\0ninjamBpiComboChanged\0"
    "\0ninjamBpmComboChanged\0ninjamAccentsComboChanged\0"
    "onPanSliderMoved\0value\0onFaderMoved\0"
    "onMuteClicked\0onSoloClicked\0on_bpiChanged\0"
    "bpi\0on_bpmChanged\0bpm\0on_intervalBeatChanged\0"
    "beat\0on_channelAdded\0Ninjam::User\0"
    "user\0Ninjam::UserChannel\0channel\0"
    "channelID\0on_channelRemoved\0"
    "on_channelNameChanged\0on_channelXmitChanged\0"
    "transmiting\0on_chatMessageReceived\0"
    "message\0userSendingNewChatMessage\0msg\0"
    "on_licenceButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NinjamRoomWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   99,    2, 0x08 /* Private */,
       3,    1,  102,    2, 0x08 /* Private */,
       4,    1,  105,    2, 0x08 /* Private */,
       5,    1,  108,    2, 0x08 /* Private */,
       7,    1,  111,    2, 0x08 /* Private */,
       8,    0,  114,    2, 0x08 /* Private */,
       9,    0,  115,    2, 0x08 /* Private */,
      10,    1,  116,    2, 0x08 /* Private */,
      12,    1,  119,    2, 0x08 /* Private */,
      14,    1,  122,    2, 0x08 /* Private */,
      16,    3,  125,    2, 0x08 /* Private */,
      22,    3,  132,    2, 0x08 /* Private */,
      23,    3,  139,    2, 0x08 /* Private */,
      24,    2,  146,    2, 0x08 /* Private */,
      26,    2,  151,    2, 0x08 /* Private */,
      28,    1,  156,    2, 0x08 /* Private */,
      30,    0,  159,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, 0x80000000 | 17, 0x80000000 | 19, QMetaType::Long,   18,   20,   21,
    QMetaType::Void, 0x80000000 | 17, 0x80000000 | 19, QMetaType::Long,   18,   20,   21,
    QMetaType::Void, 0x80000000 | 17, 0x80000000 | 19, QMetaType::Long,   18,   20,   21,
    QMetaType::Void, QMetaType::Long, QMetaType::Bool,   21,   25,
    QMetaType::Void, 0x80000000 | 17, QMetaType::QString,    2,   27,
    QMetaType::Void, QMetaType::QString,   29,
    QMetaType::Void,

       0        // eod
};

void NinjamRoomWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NinjamRoomWindow *_t = static_cast<NinjamRoomWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ninjamBpiComboChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->ninjamBpmComboChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->ninjamAccentsComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onPanSliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onFaderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->onMuteClicked(); break;
        case 6: _t->onSoloClicked(); break;
        case 7: _t->on_bpiChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_bpmChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->on_intervalBeatChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->on_channelAdded((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 11: _t->on_channelRemoved((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 12: _t->on_channelNameChanged((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 13: _t->on_channelXmitChanged((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 14: _t->on_chatMessageReceived((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 15: _t->userSendingNewChatMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: _t->on_licenceButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject NinjamRoomWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NinjamRoomWindow.data,
      qt_meta_data_NinjamRoomWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NinjamRoomWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NinjamRoomWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NinjamRoomWindow.stringdata0))
        return static_cast<void*>(const_cast< NinjamRoomWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int NinjamRoomWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
