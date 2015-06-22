/****************************************************************************
** Meta object code from reading C++ file 'NinjamJamRoomController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/NinjamJamRoomController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NinjamJamRoomController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Controller__NinjamJamRoomController_t {
    QByteArrayData data[26];
    char stringdata[417];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__NinjamJamRoomController_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__NinjamJamRoomController_t qt_meta_stringdata_Controller__NinjamJamRoomController = {
    {
QT_MOC_LITERAL(0, 0, 35), // "Controller::NinjamJamRoomCont..."
QT_MOC_LITERAL(1, 36, 17), // "currentBpiChanged"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 6), // "newBpi"
QT_MOC_LITERAL(4, 62, 17), // "currentBpmChanged"
QT_MOC_LITERAL(5, 80, 6), // "newBpm"
QT_MOC_LITERAL(6, 87, 19), // "intervalBeatChanged"
QT_MOC_LITERAL(7, 107, 12), // "intervalBeat"
QT_MOC_LITERAL(8, 120, 12), // "channelAdded"
QT_MOC_LITERAL(9, 133, 12), // "Ninjam::User"
QT_MOC_LITERAL(10, 146, 4), // "user"
QT_MOC_LITERAL(11, 151, 19), // "Ninjam::UserChannel"
QT_MOC_LITERAL(12, 171, 7), // "channel"
QT_MOC_LITERAL(13, 179, 9), // "channelID"
QT_MOC_LITERAL(14, 189, 14), // "channelRemoved"
QT_MOC_LITERAL(15, 204, 14), // "channelChanged"
QT_MOC_LITERAL(16, 219, 22), // "ninjamServerBpmChanged"
QT_MOC_LITERAL(17, 242, 22), // "ninjamServerBpiChanged"
QT_MOC_LITERAL(18, 265, 6), // "oldBpi"
QT_MOC_LITERAL(19, 272, 20), // "ninjamAudioAvailable"
QT_MOC_LITERAL(20, 293, 12), // "channelIndex"
QT_MOC_LITERAL(21, 306, 16), // "encodedAudioData"
QT_MOC_LITERAL(22, 323, 18), // "lastPartOfInterval"
QT_MOC_LITERAL(23, 342, 24), // "ninjamUserChannelCreated"
QT_MOC_LITERAL(24, 367, 24), // "ninjamUserChannelRemoved"
QT_MOC_LITERAL(25, 392, 24) // "ninjamUserChannelUpdated"

    },
    "Controller::NinjamJamRoomController\0"
    "currentBpiChanged\0\0newBpi\0currentBpmChanged\0"
    "newBpm\0intervalBeatChanged\0intervalBeat\0"
    "channelAdded\0Ninjam::User\0user\0"
    "Ninjam::UserChannel\0channel\0channelID\0"
    "channelRemoved\0channelChanged\0"
    "ninjamServerBpmChanged\0ninjamServerBpiChanged\0"
    "oldBpi\0ninjamAudioAvailable\0channelIndex\0"
    "encodedAudioData\0lastPartOfInterval\0"
    "ninjamUserChannelCreated\0"
    "ninjamUserChannelRemoved\0"
    "ninjamUserChannelUpdated"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller__NinjamJamRoomController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       4,    1,   77,    2, 0x06 /* Public */,
       6,    1,   80,    2, 0x06 /* Public */,
       8,    3,   83,    2, 0x06 /* Public */,
      14,    3,   90,    2, 0x06 /* Public */,
      15,    3,   97,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      16,    1,  104,    2, 0x08 /* Private */,
      17,    2,  107,    2, 0x08 /* Private */,
      19,    4,  112,    2, 0x08 /* Private */,
      23,    2,  121,    2, 0x08 /* Private */,
      24,    2,  126,    2, 0x08 /* Private */,
      25,    2,  131,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11, QMetaType::Long,   10,   12,   13,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11, QMetaType::Long,   10,   12,   13,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11, QMetaType::Long,   10,   12,   13,

 // slots: parameters
    QMetaType::Void, QMetaType::Short,    5,
    QMetaType::Void, QMetaType::Short, QMetaType::Short,   18,    3,
    QMetaType::Void, 0x80000000 | 9, QMetaType::Int, QMetaType::QByteArray, QMetaType::Bool,   10,   20,   21,   22,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11,   10,   12,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11,   10,   12,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11,   10,   12,

       0        // eod
};

void Controller::NinjamJamRoomController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NinjamJamRoomController *_t = static_cast<NinjamJamRoomController *>(_o);
        switch (_id) {
        case 0: _t->currentBpiChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->currentBpmChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->intervalBeatChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->channelAdded((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 4: _t->channelRemoved((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 5: _t->channelChanged((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 6: _t->ninjamServerBpmChanged((*reinterpret_cast< short(*)>(_a[1]))); break;
        case 7: _t->ninjamServerBpiChanged((*reinterpret_cast< short(*)>(_a[1])),(*reinterpret_cast< short(*)>(_a[2]))); break;
        case 8: _t->ninjamAudioAvailable((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QByteArray(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 9: _t->ninjamUserChannelCreated((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2]))); break;
        case 10: _t->ninjamUserChannelRemoved((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2]))); break;
        case 11: _t->ninjamUserChannelUpdated((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NinjamJamRoomController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamJamRoomController::currentBpiChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (NinjamJamRoomController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamJamRoomController::currentBpmChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (NinjamJamRoomController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamJamRoomController::intervalBeatChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (NinjamJamRoomController::*_t)(Ninjam::User , Ninjam::UserChannel , long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamJamRoomController::channelAdded)) {
                *result = 3;
            }
        }
        {
            typedef void (NinjamJamRoomController::*_t)(Ninjam::User , Ninjam::UserChannel , long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamJamRoomController::channelRemoved)) {
                *result = 4;
            }
        }
        {
            typedef void (NinjamJamRoomController::*_t)(Ninjam::User , Ninjam::UserChannel , long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamJamRoomController::channelChanged)) {
                *result = 5;
            }
        }
    }
}

const QMetaObject Controller::NinjamJamRoomController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Controller__NinjamJamRoomController.data,
      qt_meta_data_Controller__NinjamJamRoomController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Controller::NinjamJamRoomController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::NinjamJamRoomController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Controller__NinjamJamRoomController.stringdata))
        return static_cast<void*>(const_cast< NinjamJamRoomController*>(this));
    return QObject::qt_metacast(_clname);
}

int Controller::NinjamJamRoomController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Controller::NinjamJamRoomController::currentBpiChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Controller::NinjamJamRoomController::currentBpmChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Controller::NinjamJamRoomController::intervalBeatChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Controller::NinjamJamRoomController::channelAdded(Ninjam::User _t1, Ninjam::UserChannel _t2, long _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Controller::NinjamJamRoomController::channelRemoved(Ninjam::User _t1, Ninjam::UserChannel _t2, long _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Controller::NinjamJamRoomController::channelChanged(Ninjam::User _t1, Ninjam::UserChannel _t2, long _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
