/****************************************************************************
** Meta object code from reading C++ file 'NinjamService.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/jamtaba/ninjam/NinjamService.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NinjamService.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NinjamService_t {
    QByteArrayData data[39];
    char stringdata[566];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NinjamService_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NinjamService_t qt_meta_stringdata_NinjamService = {
    {
QT_MOC_LITERAL(0, 0, 13), // "NinjamService"
QT_MOC_LITERAL(1, 14, 18), // "userChannelCreated"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 10), // "NinjamUser"
QT_MOC_LITERAL(4, 45, 4), // "user"
QT_MOC_LITERAL(5, 50, 17), // "NinjamUserChannel"
QT_MOC_LITERAL(6, 68, 7), // "channel"
QT_MOC_LITERAL(7, 76, 18), // "userChannelRemoved"
QT_MOC_LITERAL(8, 95, 18), // "userChannelUpdated"
QT_MOC_LITERAL(9, 114, 24), // "userCountMessageReceived"
QT_MOC_LITERAL(10, 139, 5), // "users"
QT_MOC_LITERAL(11, 145, 8), // "maxUsers"
QT_MOC_LITERAL(12, 154, 16), // "serverBpiChanged"
QT_MOC_LITERAL(13, 171, 10), // "currentBpi"
QT_MOC_LITERAL(14, 182, 7), // "lastBpi"
QT_MOC_LITERAL(15, 190, 16), // "serverBpmChanged"
QT_MOC_LITERAL(16, 207, 10), // "currentBpm"
QT_MOC_LITERAL(17, 218, 26), // "audioIntervalPartAvailable"
QT_MOC_LITERAL(18, 245, 12), // "channelIndex"
QT_MOC_LITERAL(19, 258, 16), // "encodedAudioData"
QT_MOC_LITERAL(20, 275, 18), // "lastPartOfInterval"
QT_MOC_LITERAL(21, 294, 22), // "disconnectedFromServer"
QT_MOC_LITERAL(22, 317, 19), // "normalDisconnection"
QT_MOC_LITERAL(23, 337, 17), // "connectedInServer"
QT_MOC_LITERAL(24, 355, 12), // "NinjamServer"
QT_MOC_LITERAL(25, 368, 6), // "server"
QT_MOC_LITERAL(26, 375, 19), // "chatMessageReceived"
QT_MOC_LITERAL(27, 395, 6), // "sender"
QT_MOC_LITERAL(28, 402, 7), // "message"
QT_MOC_LITERAL(29, 410, 22), // "privateMessageReceived"
QT_MOC_LITERAL(30, 433, 17), // "userEnterInTheJam"
QT_MOC_LITERAL(31, 451, 7), // "newUser"
QT_MOC_LITERAL(32, 459, 15), // "userLeaveTheJam"
QT_MOC_LITERAL(33, 475, 5), // "error"
QT_MOC_LITERAL(34, 481, 3), // "msg"
QT_MOC_LITERAL(35, 485, 14), // "socketReadSlot"
QT_MOC_LITERAL(36, 500, 15), // "socketErrorSlot"
QT_MOC_LITERAL(37, 516, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(38, 545, 20) // "socketDisconnectSlot"

    },
    "NinjamService\0userChannelCreated\0\0"
    "NinjamUser\0user\0NinjamUserChannel\0"
    "channel\0userChannelRemoved\0"
    "userChannelUpdated\0userCountMessageReceived\0"
    "users\0maxUsers\0serverBpiChanged\0"
    "currentBpi\0lastBpi\0serverBpmChanged\0"
    "currentBpm\0audioIntervalPartAvailable\0"
    "channelIndex\0encodedAudioData\0"
    "lastPartOfInterval\0disconnectedFromServer\0"
    "normalDisconnection\0connectedInServer\0"
    "NinjamServer\0server\0chatMessageReceived\0"
    "sender\0message\0privateMessageReceived\0"
    "userEnterInTheJam\0newUser\0userLeaveTheJam\0"
    "error\0msg\0socketReadSlot\0socketErrorSlot\0"
    "QAbstractSocket::SocketError\0"
    "socketDisconnectSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NinjamService[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   99,    2, 0x06 /* Public */,
       7,    2,  104,    2, 0x06 /* Public */,
       8,    2,  109,    2, 0x06 /* Public */,
       9,    2,  114,    2, 0x06 /* Public */,
      12,    2,  119,    2, 0x06 /* Public */,
      15,    1,  124,    2, 0x06 /* Public */,
      17,    4,  127,    2, 0x06 /* Public */,
      21,    1,  136,    2, 0x06 /* Public */,
      23,    1,  139,    2, 0x06 /* Public */,
      26,    2,  142,    2, 0x06 /* Public */,
      29,    2,  147,    2, 0x06 /* Public */,
      30,    1,  152,    2, 0x06 /* Public */,
      32,    1,  155,    2, 0x06 /* Public */,
      33,    1,  158,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      35,    0,  161,    2, 0x08 /* Private */,
      36,    1,  162,    2, 0x08 /* Private */,
      38,    0,  165,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   10,   11,
    QMetaType::Void, QMetaType::Short, QMetaType::Short,   13,   14,
    QMetaType::Void, QMetaType::Short,   16,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::QByteArray, QMetaType::Bool,    4,   18,   19,   20,
    QMetaType::Void, QMetaType::Bool,   22,
    QMetaType::Void, 0x80000000 | 24,   25,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,   27,   28,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,   27,   28,
    QMetaType::Void, 0x80000000 | 3,   31,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,   34,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 37,   33,
    QMetaType::Void,

       0        // eod
};

void NinjamService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NinjamService *_t = static_cast<NinjamService *>(_o);
        switch (_id) {
        case 0: _t->userChannelCreated((*reinterpret_cast< const NinjamUser(*)>(_a[1])),(*reinterpret_cast< const NinjamUserChannel(*)>(_a[2]))); break;
        case 1: _t->userChannelRemoved((*reinterpret_cast< const NinjamUser(*)>(_a[1])),(*reinterpret_cast< const NinjamUserChannel(*)>(_a[2]))); break;
        case 2: _t->userChannelUpdated((*reinterpret_cast< const NinjamUser(*)>(_a[1])),(*reinterpret_cast< const NinjamUserChannel(*)>(_a[2]))); break;
        case 3: _t->userCountMessageReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->serverBpiChanged((*reinterpret_cast< short(*)>(_a[1])),(*reinterpret_cast< short(*)>(_a[2]))); break;
        case 5: _t->serverBpmChanged((*reinterpret_cast< short(*)>(_a[1]))); break;
        case 6: _t->audioIntervalPartAvailable((*reinterpret_cast< const NinjamUser(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QByteArray(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 7: _t->disconnectedFromServer((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->connectedInServer((*reinterpret_cast< const NinjamServer(*)>(_a[1]))); break;
        case 9: _t->chatMessageReceived((*reinterpret_cast< const NinjamUser(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 10: _t->privateMessageReceived((*reinterpret_cast< const NinjamUser(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: _t->userEnterInTheJam((*reinterpret_cast< const NinjamUser(*)>(_a[1]))); break;
        case 12: _t->userLeaveTheJam((*reinterpret_cast< const NinjamUser(*)>(_a[1]))); break;
        case 13: _t->error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->socketReadSlot(); break;
        case 15: _t->socketErrorSlot((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 16: _t->socketDisconnectSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 15:
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
            typedef void (NinjamService::*_t)(const NinjamUser & , const NinjamUserChannel & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::userChannelCreated)) {
                *result = 0;
            }
        }
        {
            typedef void (NinjamService::*_t)(const NinjamUser & , const NinjamUserChannel & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::userChannelRemoved)) {
                *result = 1;
            }
        }
        {
            typedef void (NinjamService::*_t)(const NinjamUser & , const NinjamUserChannel & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::userChannelUpdated)) {
                *result = 2;
            }
        }
        {
            typedef void (NinjamService::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::userCountMessageReceived)) {
                *result = 3;
            }
        }
        {
            typedef void (NinjamService::*_t)(short , short );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::serverBpiChanged)) {
                *result = 4;
            }
        }
        {
            typedef void (NinjamService::*_t)(short );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::serverBpmChanged)) {
                *result = 5;
            }
        }
        {
            typedef void (NinjamService::*_t)(const NinjamUser & , int , QByteArray , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::audioIntervalPartAvailable)) {
                *result = 6;
            }
        }
        {
            typedef void (NinjamService::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::disconnectedFromServer)) {
                *result = 7;
            }
        }
        {
            typedef void (NinjamService::*_t)(const NinjamServer & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::connectedInServer)) {
                *result = 8;
            }
        }
        {
            typedef void (NinjamService::*_t)(const NinjamUser & , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::chatMessageReceived)) {
                *result = 9;
            }
        }
        {
            typedef void (NinjamService::*_t)(const NinjamUser & , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::privateMessageReceived)) {
                *result = 10;
            }
        }
        {
            typedef void (NinjamService::*_t)(const NinjamUser & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::userEnterInTheJam)) {
                *result = 11;
            }
        }
        {
            typedef void (NinjamService::*_t)(const NinjamUser & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::userLeaveTheJam)) {
                *result = 12;
            }
        }
        {
            typedef void (NinjamService::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamService::error)) {
                *result = 13;
            }
        }
    }
}

const QMetaObject NinjamService::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NinjamService.data,
      qt_meta_data_NinjamService,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NinjamService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NinjamService::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NinjamService.stringdata))
        return static_cast<void*>(const_cast< NinjamService*>(this));
    return QObject::qt_metacast(_clname);
}

int NinjamService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void NinjamService::userChannelCreated(const NinjamUser & _t1, const NinjamUserChannel & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NinjamService::userChannelRemoved(const NinjamUser & _t1, const NinjamUserChannel & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NinjamService::userChannelUpdated(const NinjamUser & _t1, const NinjamUserChannel & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NinjamService::userCountMessageReceived(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NinjamService::serverBpiChanged(short _t1, short _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void NinjamService::serverBpmChanged(short _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void NinjamService::audioIntervalPartAvailable(const NinjamUser & _t1, int _t2, QByteArray _t3, bool _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void NinjamService::disconnectedFromServer(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void NinjamService::connectedInServer(const NinjamServer & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void NinjamService::chatMessageReceived(const NinjamUser & _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void NinjamService::privateMessageReceived(const NinjamUser & _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void NinjamService::userEnterInTheJam(const NinjamUser & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void NinjamService::userLeaveTheJam(const NinjamUser & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void NinjamService::error(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}
QT_END_MOC_NAMESPACE
