/****************************************************************************
** Meta object code from reading C++ file 'NinjamController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/NinjamController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NinjamController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Controller__NinjamController_t {
    QByteArrayData data[49];
    char stringdata0[832];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__NinjamController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__NinjamController_t qt_meta_stringdata_Controller__NinjamController = {
    {
QT_MOC_LITERAL(0, 0, 28), // "Controller::NinjamController"
QT_MOC_LITERAL(1, 29, 17), // "currentBpiChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 6), // "newBpi"
QT_MOC_LITERAL(4, 55, 17), // "currentBpmChanged"
QT_MOC_LITERAL(5, 73, 6), // "newBpm"
QT_MOC_LITERAL(6, 80, 19), // "intervalBeatChanged"
QT_MOC_LITERAL(7, 100, 12), // "intervalBeat"
QT_MOC_LITERAL(8, 113, 19), // "startingNewInterval"
QT_MOC_LITERAL(9, 133, 15), // "startProcessing"
QT_MOC_LITERAL(10, 149, 16), // "intervalPosition"
QT_MOC_LITERAL(11, 166, 12), // "channelAdded"
QT_MOC_LITERAL(12, 179, 12), // "Ninjam::User"
QT_MOC_LITERAL(13, 192, 4), // "user"
QT_MOC_LITERAL(14, 197, 19), // "Ninjam::UserChannel"
QT_MOC_LITERAL(15, 217, 7), // "channel"
QT_MOC_LITERAL(16, 225, 9), // "channelID"
QT_MOC_LITERAL(17, 235, 14), // "channelRemoved"
QT_MOC_LITERAL(18, 250, 18), // "channelNameChanged"
QT_MOC_LITERAL(19, 269, 18), // "channelXmitChanged"
QT_MOC_LITERAL(20, 288, 11), // "transmiting"
QT_MOC_LITERAL(21, 300, 9), // "userLeave"
QT_MOC_LITERAL(22, 310, 8), // "userName"
QT_MOC_LITERAL(23, 319, 9), // "userEnter"
QT_MOC_LITERAL(24, 329, 15), // "chatMsgReceived"
QT_MOC_LITERAL(25, 345, 7), // "message"
QT_MOC_LITERAL(26, 353, 27), // "encodedAudioAvailableToSend"
QT_MOC_LITERAL(27, 381, 12), // "encodedAudio"
QT_MOC_LITERAL(28, 394, 12), // "channelIndex"
QT_MOC_LITERAL(29, 407, 11), // "isFirstPart"
QT_MOC_LITERAL(30, 419, 10), // "isLastPart"
QT_MOC_LITERAL(31, 430, 25), // "on_ninjamServerBpmChanged"
QT_MOC_LITERAL(32, 456, 25), // "on_ninjamServerBpiChanged"
QT_MOC_LITERAL(33, 482, 6), // "oldBpi"
QT_MOC_LITERAL(34, 489, 31), // "on_ninjamAudiointervalCompleted"
QT_MOC_LITERAL(35, 521, 16), // "encodedAudioData"
QT_MOC_LITERAL(36, 538, 33), // "on_ninjamAudiointervalDownloa..."
QT_MOC_LITERAL(37, 572, 15), // "downloadedBytes"
QT_MOC_LITERAL(38, 588, 27), // "on_ninjamUserChannelCreated"
QT_MOC_LITERAL(39, 616, 27), // "on_ninjamUserChannelRemoved"
QT_MOC_LITERAL(40, 644, 27), // "on_ninjamUserChannelUpdated"
QT_MOC_LITERAL(41, 672, 18), // "on_ninjamUserLeave"
QT_MOC_LITERAL(42, 691, 18), // "on_ninjamUserEnter"
QT_MOC_LITERAL(43, 710, 31), // "on_ninjamDisconnectedFromServer"
QT_MOC_LITERAL(44, 742, 14), // "Ninjam::Server"
QT_MOC_LITERAL(45, 757, 6), // "server"
QT_MOC_LITERAL(46, 764, 31), // "on_audioDriverSampleRateChanged"
QT_MOC_LITERAL(47, 796, 13), // "newSampleRate"
QT_MOC_LITERAL(48, 810, 21) // "on_audioDriverStopped"

    },
    "Controller::NinjamController\0"
    "currentBpiChanged\0\0newBpi\0currentBpmChanged\0"
    "newBpm\0intervalBeatChanged\0intervalBeat\0"
    "startingNewInterval\0startProcessing\0"
    "intervalPosition\0channelAdded\0"
    "Ninjam::User\0user\0Ninjam::UserChannel\0"
    "channel\0channelID\0channelRemoved\0"
    "channelNameChanged\0channelXmitChanged\0"
    "transmiting\0userLeave\0userName\0userEnter\0"
    "chatMsgReceived\0message\0"
    "encodedAudioAvailableToSend\0encodedAudio\0"
    "channelIndex\0isFirstPart\0isLastPart\0"
    "on_ninjamServerBpmChanged\0"
    "on_ninjamServerBpiChanged\0oldBpi\0"
    "on_ninjamAudiointervalCompleted\0"
    "encodedAudioData\0on_ninjamAudiointervalDownloading\0"
    "downloadedBytes\0on_ninjamUserChannelCreated\0"
    "on_ninjamUserChannelRemoved\0"
    "on_ninjamUserChannelUpdated\0"
    "on_ninjamUserLeave\0on_ninjamUserEnter\0"
    "on_ninjamDisconnectedFromServer\0"
    "Ninjam::Server\0server\0"
    "on_audioDriverSampleRateChanged\0"
    "newSampleRate\0on_audioDriverStopped"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller__NinjamController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  139,    2, 0x06 /* Public */,
       4,    1,  142,    2, 0x06 /* Public */,
       6,    1,  145,    2, 0x06 /* Public */,
       8,    0,  148,    2, 0x06 /* Public */,
       9,    1,  149,    2, 0x06 /* Public */,
      11,    3,  152,    2, 0x06 /* Public */,
      17,    3,  159,    2, 0x06 /* Public */,
      18,    3,  166,    2, 0x06 /* Public */,
      19,    2,  173,    2, 0x06 /* Public */,
      21,    1,  178,    2, 0x06 /* Public */,
      23,    1,  181,    2, 0x06 /* Public */,
      24,    2,  184,    2, 0x06 /* Public */,
      26,    4,  189,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      31,    1,  198,    2, 0x08 /* Private */,
      32,    2,  201,    2, 0x08 /* Private */,
      34,    3,  206,    2, 0x08 /* Private */,
      36,    3,  213,    2, 0x08 /* Private */,
      38,    2,  220,    2, 0x08 /* Private */,
      39,    2,  225,    2, 0x08 /* Private */,
      40,    2,  230,    2, 0x08 /* Private */,
      41,    1,  235,    2, 0x08 /* Private */,
      42,    1,  238,    2, 0x08 /* Private */,
      43,    1,  241,    2, 0x08 /* Private */,
      46,    1,  244,    2, 0x08 /* Private */,
      48,    0,  247,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14, QMetaType::Long,   13,   15,   16,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14, QMetaType::Long,   13,   15,   16,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14, QMetaType::Long,   13,   15,   16,
    QMetaType::Void, QMetaType::Long, QMetaType::Bool,   16,   20,
    QMetaType::Void, QMetaType::QString,   22,
    QMetaType::Void, QMetaType::QString,   22,
    QMetaType::Void, 0x80000000 | 12, QMetaType::QString,   13,   25,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::UChar, QMetaType::Bool, QMetaType::Bool,   27,   28,   29,   30,

 // slots: parameters
    QMetaType::Void, QMetaType::Short,    5,
    QMetaType::Void, QMetaType::Short, QMetaType::Short,   33,    3,
    QMetaType::Void, 0x80000000 | 12, QMetaType::Int, QMetaType::QByteArray,   13,   28,   35,
    QMetaType::Void, 0x80000000 | 12, QMetaType::Int, QMetaType::Int,   13,   28,   37,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14,   13,   15,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14,   13,   15,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14,   13,   15,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 44,   45,
    QMetaType::Void, QMetaType::Int,   47,
    QMetaType::Void,

       0        // eod
};

void Controller::NinjamController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NinjamController *_t = static_cast<NinjamController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentBpiChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->currentBpmChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->intervalBeatChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->startingNewInterval(); break;
        case 4: _t->startProcessing((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->channelAdded((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 6: _t->channelRemoved((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 7: _t->channelNameChanged((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 8: _t->channelXmitChanged((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: _t->userLeave((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->userEnter((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: _t->chatMsgReceived((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 12: _t->encodedAudioAvailableToSend((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 13: _t->on_ninjamServerBpmChanged((*reinterpret_cast< short(*)>(_a[1]))); break;
        case 14: _t->on_ninjamServerBpiChanged((*reinterpret_cast< short(*)>(_a[1])),(*reinterpret_cast< short(*)>(_a[2]))); break;
        case 15: _t->on_ninjamAudiointervalCompleted((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QByteArray(*)>(_a[3]))); break;
        case 16: _t->on_ninjamAudiointervalDownloading((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 17: _t->on_ninjamUserChannelCreated((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2]))); break;
        case 18: _t->on_ninjamUserChannelRemoved((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2]))); break;
        case 19: _t->on_ninjamUserChannelUpdated((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2]))); break;
        case 20: _t->on_ninjamUserLeave((*reinterpret_cast< Ninjam::User(*)>(_a[1]))); break;
        case 21: _t->on_ninjamUserEnter((*reinterpret_cast< Ninjam::User(*)>(_a[1]))); break;
        case 22: _t->on_ninjamDisconnectedFromServer((*reinterpret_cast< Ninjam::Server(*)>(_a[1]))); break;
        case 23: _t->on_audioDriverSampleRateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->on_audioDriverStopped(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NinjamController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::currentBpiChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (NinjamController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::currentBpmChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (NinjamController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::intervalBeatChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (NinjamController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::startingNewInterval)) {
                *result = 3;
            }
        }
        {
            typedef void (NinjamController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::startProcessing)) {
                *result = 4;
            }
        }
        {
            typedef void (NinjamController::*_t)(Ninjam::User , Ninjam::UserChannel , long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::channelAdded)) {
                *result = 5;
            }
        }
        {
            typedef void (NinjamController::*_t)(Ninjam::User , Ninjam::UserChannel , long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::channelRemoved)) {
                *result = 6;
            }
        }
        {
            typedef void (NinjamController::*_t)(Ninjam::User , Ninjam::UserChannel , long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::channelNameChanged)) {
                *result = 7;
            }
        }
        {
            typedef void (NinjamController::*_t)(long , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::channelXmitChanged)) {
                *result = 8;
            }
        }
        {
            typedef void (NinjamController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::userLeave)) {
                *result = 9;
            }
        }
        {
            typedef void (NinjamController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::userEnter)) {
                *result = 10;
            }
        }
        {
            typedef void (NinjamController::*_t)(Ninjam::User , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::chatMsgReceived)) {
                *result = 11;
            }
        }
        {
            typedef void (NinjamController::*_t)(QByteArray , quint8 , bool , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamController::encodedAudioAvailableToSend)) {
                *result = 12;
            }
        }
    }
}

const QMetaObject Controller::NinjamController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Controller__NinjamController.data,
      qt_meta_data_Controller__NinjamController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Controller::NinjamController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::NinjamController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Controller__NinjamController.stringdata0))
        return static_cast<void*>(const_cast< NinjamController*>(this));
    return QObject::qt_metacast(_clname);
}

int Controller::NinjamController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void Controller::NinjamController::currentBpiChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Controller::NinjamController::currentBpmChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Controller::NinjamController::intervalBeatChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Controller::NinjamController::startingNewInterval()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void Controller::NinjamController::startProcessing(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Controller::NinjamController::channelAdded(Ninjam::User _t1, Ninjam::UserChannel _t2, long _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Controller::NinjamController::channelRemoved(Ninjam::User _t1, Ninjam::UserChannel _t2, long _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Controller::NinjamController::channelNameChanged(Ninjam::User _t1, Ninjam::UserChannel _t2, long _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Controller::NinjamController::channelXmitChanged(long _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Controller::NinjamController::userLeave(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Controller::NinjamController::userEnter(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Controller::NinjamController::chatMsgReceived(Ninjam::User _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Controller::NinjamController::encodedAudioAvailableToSend(QByteArray _t1, quint8 _t2, bool _t3, bool _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}
QT_END_MOC_NAMESPACE
