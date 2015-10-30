/****************************************************************************
** Meta object code from reading C++ file 'Service.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/ninjam/Service.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Service.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Ninjam__Service_t {
    QByteArrayData data[40];
    char stringdata0[592];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Ninjam__Service_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Ninjam__Service_t qt_meta_stringdata_Ninjam__Service = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Ninjam::Service"
QT_MOC_LITERAL(1, 16, 18), // "userChannelCreated"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 12), // "Ninjam::User"
QT_MOC_LITERAL(4, 49, 4), // "user"
QT_MOC_LITERAL(5, 54, 19), // "Ninjam::UserChannel"
QT_MOC_LITERAL(6, 74, 7), // "channel"
QT_MOC_LITERAL(7, 82, 18), // "userChannelRemoved"
QT_MOC_LITERAL(8, 101, 18), // "userChannelUpdated"
QT_MOC_LITERAL(9, 120, 24), // "userCountMessageReceived"
QT_MOC_LITERAL(10, 145, 5), // "users"
QT_MOC_LITERAL(11, 151, 8), // "maxUsers"
QT_MOC_LITERAL(12, 160, 16), // "serverBpiChanged"
QT_MOC_LITERAL(13, 177, 10), // "currentBpi"
QT_MOC_LITERAL(14, 188, 7), // "lastBpi"
QT_MOC_LITERAL(15, 196, 16), // "serverBpmChanged"
QT_MOC_LITERAL(16, 213, 10), // "currentBpm"
QT_MOC_LITERAL(17, 224, 22), // "audioIntervalCompleted"
QT_MOC_LITERAL(18, 247, 12), // "channelIndex"
QT_MOC_LITERAL(19, 260, 16), // "encodedAudioData"
QT_MOC_LITERAL(20, 277, 24), // "audioIntervalDownloading"
QT_MOC_LITERAL(21, 302, 15), // "bytesDownloaded"
QT_MOC_LITERAL(22, 318, 22), // "disconnectedFromServer"
QT_MOC_LITERAL(23, 341, 14), // "Ninjam::Server"
QT_MOC_LITERAL(24, 356, 6), // "server"
QT_MOC_LITERAL(25, 363, 17), // "connectedInServer"
QT_MOC_LITERAL(26, 381, 19), // "chatMessageReceived"
QT_MOC_LITERAL(27, 401, 6), // "sender"
QT_MOC_LITERAL(28, 408, 7), // "message"
QT_MOC_LITERAL(29, 416, 22), // "privateMessageReceived"
QT_MOC_LITERAL(30, 439, 17), // "userEnterInTheJam"
QT_MOC_LITERAL(31, 457, 7), // "newUser"
QT_MOC_LITERAL(32, 465, 15), // "userLeaveTheJam"
QT_MOC_LITERAL(33, 481, 5), // "error"
QT_MOC_LITERAL(34, 487, 3), // "msg"
QT_MOC_LITERAL(35, 491, 14), // "socketReadSlot"
QT_MOC_LITERAL(36, 506, 15), // "socketErrorSlot"
QT_MOC_LITERAL(37, 522, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(38, 551, 20), // "socketDisconnectSlot"
QT_MOC_LITERAL(39, 572, 19) // "socketConnectedSlot"

    },
    "Ninjam::Service\0userChannelCreated\0\0"
    "Ninjam::User\0user\0Ninjam::UserChannel\0"
    "channel\0userChannelRemoved\0"
    "userChannelUpdated\0userCountMessageReceived\0"
    "users\0maxUsers\0serverBpiChanged\0"
    "currentBpi\0lastBpi\0serverBpmChanged\0"
    "currentBpm\0audioIntervalCompleted\0"
    "channelIndex\0encodedAudioData\0"
    "audioIntervalDownloading\0bytesDownloaded\0"
    "disconnectedFromServer\0Ninjam::Server\0"
    "server\0connectedInServer\0chatMessageReceived\0"
    "sender\0message\0privateMessageReceived\0"
    "userEnterInTheJam\0newUser\0userLeaveTheJam\0"
    "error\0msg\0socketReadSlot\0socketErrorSlot\0"
    "QAbstractSocket::SocketError\0"
    "socketDisconnectSlot\0socketConnectedSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Ninjam__Service[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      15,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,  109,    2, 0x06 /* Public */,
       7,    2,  114,    2, 0x06 /* Public */,
       8,    2,  119,    2, 0x06 /* Public */,
       9,    2,  124,    2, 0x06 /* Public */,
      12,    2,  129,    2, 0x06 /* Public */,
      15,    1,  134,    2, 0x06 /* Public */,
      17,    3,  137,    2, 0x06 /* Public */,
      20,    3,  144,    2, 0x06 /* Public */,
      22,    1,  151,    2, 0x06 /* Public */,
      25,    1,  154,    2, 0x06 /* Public */,
      26,    2,  157,    2, 0x06 /* Public */,
      29,    2,  162,    2, 0x06 /* Public */,
      30,    1,  167,    2, 0x06 /* Public */,
      32,    1,  170,    2, 0x06 /* Public */,
      33,    1,  173,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      35,    0,  176,    2, 0x08 /* Private */,
      36,    1,  177,    2, 0x08 /* Private */,
      38,    0,  180,    2, 0x08 /* Private */,
      39,    0,  181,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   10,   11,
    QMetaType::Void, QMetaType::Short, QMetaType::Short,   13,   14,
    QMetaType::Void, QMetaType::Short,   16,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::QByteArray,    4,   18,   19,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,    2,   18,   21,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,   27,   28,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,   27,   28,
    QMetaType::Void, 0x80000000 | 3,   31,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,   34,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 37,   33,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Ninjam::Service::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Service *_t = static_cast<Service *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->userChannelCreated((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2]))); break;
        case 1: _t->userChannelRemoved((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2]))); break;
        case 2: _t->userChannelUpdated((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2]))); break;
        case 3: _t->userCountMessageReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->serverBpiChanged((*reinterpret_cast< short(*)>(_a[1])),(*reinterpret_cast< short(*)>(_a[2]))); break;
        case 5: _t->serverBpmChanged((*reinterpret_cast< short(*)>(_a[1]))); break;
        case 6: _t->audioIntervalCompleted((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QByteArray(*)>(_a[3]))); break;
        case 7: _t->audioIntervalDownloading((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 8: _t->disconnectedFromServer((*reinterpret_cast< const Ninjam::Server(*)>(_a[1]))); break;
        case 9: _t->connectedInServer((*reinterpret_cast< const Ninjam::Server(*)>(_a[1]))); break;
        case 10: _t->chatMessageReceived((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: _t->privateMessageReceived((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 12: _t->userEnterInTheJam((*reinterpret_cast< Ninjam::User(*)>(_a[1]))); break;
        case 13: _t->userLeaveTheJam((*reinterpret_cast< Ninjam::User(*)>(_a[1]))); break;
        case 14: _t->error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 15: _t->socketReadSlot(); break;
        case 16: _t->socketErrorSlot((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 17: _t->socketDisconnectSlot(); break;
        case 18: _t->socketConnectedSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Service::*_t)(Ninjam::User , Ninjam::UserChannel );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::userChannelCreated)) {
                *result = 0;
            }
        }
        {
            typedef void (Service::*_t)(Ninjam::User , Ninjam::UserChannel );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::userChannelRemoved)) {
                *result = 1;
            }
        }
        {
            typedef void (Service::*_t)(Ninjam::User , Ninjam::UserChannel );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::userChannelUpdated)) {
                *result = 2;
            }
        }
        {
            typedef void (Service::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::userCountMessageReceived)) {
                *result = 3;
            }
        }
        {
            typedef void (Service::*_t)(short , short );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::serverBpiChanged)) {
                *result = 4;
            }
        }
        {
            typedef void (Service::*_t)(short );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::serverBpmChanged)) {
                *result = 5;
            }
        }
        {
            typedef void (Service::*_t)(Ninjam::User , int , QByteArray );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::audioIntervalCompleted)) {
                *result = 6;
            }
        }
        {
            typedef void (Service::*_t)(Ninjam::User , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::audioIntervalDownloading)) {
                *result = 7;
            }
        }
        {
            typedef void (Service::*_t)(const Ninjam::Server & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::disconnectedFromServer)) {
                *result = 8;
            }
        }
        {
            typedef void (Service::*_t)(const Ninjam::Server & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::connectedInServer)) {
                *result = 9;
            }
        }
        {
            typedef void (Service::*_t)(Ninjam::User , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::chatMessageReceived)) {
                *result = 10;
            }
        }
        {
            typedef void (Service::*_t)(Ninjam::User , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::privateMessageReceived)) {
                *result = 11;
            }
        }
        {
            typedef void (Service::*_t)(Ninjam::User );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::userEnterInTheJam)) {
                *result = 12;
            }
        }
        {
            typedef void (Service::*_t)(Ninjam::User );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::userLeaveTheJam)) {
                *result = 13;
            }
        }
        {
            typedef void (Service::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Service::error)) {
                *result = 14;
            }
        }
    }
}

const QMetaObject Ninjam::Service::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Ninjam__Service.data,
      qt_meta_data_Ninjam__Service,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Ninjam::Service::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Ninjam::Service::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Ninjam__Service.stringdata0))
        return static_cast<void*>(const_cast< Service*>(this));
    return QObject::qt_metacast(_clname);
}

int Ninjam::Service::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void Ninjam::Service::userChannelCreated(Ninjam::User _t1, Ninjam::UserChannel _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Ninjam::Service::userChannelRemoved(Ninjam::User _t1, Ninjam::UserChannel _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Ninjam::Service::userChannelUpdated(Ninjam::User _t1, Ninjam::UserChannel _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Ninjam::Service::userCountMessageReceived(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Ninjam::Service::serverBpiChanged(short _t1, short _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Ninjam::Service::serverBpmChanged(short _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Ninjam::Service::audioIntervalCompleted(Ninjam::User _t1, int _t2, QByteArray _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Ninjam::Service::audioIntervalDownloading(Ninjam::User _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Ninjam::Service::disconnectedFromServer(const Ninjam::Server & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Ninjam::Service::connectedInServer(const Ninjam::Server & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Ninjam::Service::chatMessageReceived(Ninjam::User _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Ninjam::Service::privateMessageReceived(Ninjam::User _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Ninjam::Service::userEnterInTheJam(Ninjam::User _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void Ninjam::Service::userLeaveTheJam(Ninjam::User _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void Ninjam::Service::error(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}
QT_END_MOC_NAMESPACE
