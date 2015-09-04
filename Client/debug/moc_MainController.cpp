/****************************************************************************
** Meta object code from reading C++ file 'MainController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/jamtaba/MainController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Controller__MainController_t {
    QByteArrayData data[32];
    char stringdata0[531];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__MainController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__MainController_t qt_meta_stringdata_Controller__MainController = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Controller::MainController"
QT_MOC_LITERAL(1, 27, 13), // "enteredInRoom"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 15), // "Login::RoomInfo"
QT_MOC_LITERAL(4, 58, 4), // "room"
QT_MOC_LITERAL(5, 63, 14), // "exitedFromRoom"
QT_MOC_LITERAL(6, 78, 5), // "error"
QT_MOC_LITERAL(7, 84, 21), // "inputSelectionChanged"
QT_MOC_LITERAL(8, 106, 15), // "inputTrackIndex"
QT_MOC_LITERAL(9, 122, 30), // "on_disconnectedFromLoginServer"
QT_MOC_LITERAL(10, 153, 19), // "on_VSTPluginFounded"
QT_MOC_LITERAL(11, 173, 4), // "name"
QT_MOC_LITERAL(12, 178, 5), // "group"
QT_MOC_LITERAL(13, 184, 4), // "path"
QT_MOC_LITERAL(14, 189, 26), // "on_connectedInNinjamServer"
QT_MOC_LITERAL(15, 216, 14), // "Ninjam::Server"
QT_MOC_LITERAL(16, 231, 6), // "server"
QT_MOC_LITERAL(17, 238, 31), // "on_disconnectedFromNinjamServer"
QT_MOC_LITERAL(18, 270, 22), // "on_errorInNinjamServer"
QT_MOC_LITERAL(19, 293, 36), // "on_ninjamEncodedAudioAvailabl..."
QT_MOC_LITERAL(20, 330, 12), // "channelIndex"
QT_MOC_LITERAL(21, 343, 11), // "isFirstPart"
QT_MOC_LITERAL(22, 355, 10), // "isLastPart"
QT_MOC_LITERAL(23, 366, 19), // "on_ninjamBpiChanged"
QT_MOC_LITERAL(24, 386, 6), // "newBpi"
QT_MOC_LITERAL(25, 393, 19), // "on_ninjamBpmChanged"
QT_MOC_LITERAL(26, 413, 6), // "newBpm"
QT_MOC_LITERAL(27, 420, 20), // "on_newNinjamInterval"
QT_MOC_LITERAL(28, 441, 31), // "on_audioDriverSampleRateChanged"
QT_MOC_LITERAL(29, 473, 13), // "newSampleRate"
QT_MOC_LITERAL(30, 487, 21), // "on_audioDriverStopped"
QT_MOC_LITERAL(31, 509, 21) // "on_audioDriverStarted"

    },
    "Controller::MainController\0enteredInRoom\0"
    "\0Login::RoomInfo\0room\0exitedFromRoom\0"
    "error\0inputSelectionChanged\0inputTrackIndex\0"
    "on_disconnectedFromLoginServer\0"
    "on_VSTPluginFounded\0name\0group\0path\0"
    "on_connectedInNinjamServer\0Ninjam::Server\0"
    "server\0on_disconnectedFromNinjamServer\0"
    "on_errorInNinjamServer\0"
    "on_ninjamEncodedAudioAvailableToSend\0"
    "channelIndex\0isFirstPart\0isLastPart\0"
    "on_ninjamBpiChanged\0newBpi\0"
    "on_ninjamBpmChanged\0newBpm\0"
    "on_newNinjamInterval\0"
    "on_audioDriverSampleRateChanged\0"
    "newSampleRate\0on_audioDriverStopped\0"
    "on_audioDriverStarted"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller__MainController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x06 /* Public */,
       5,    1,   92,    2, 0x06 /* Public */,
       7,    1,   95,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   98,    2, 0x08 /* Private */,
      10,    3,   99,    2, 0x08 /* Private */,
      14,    1,  106,    2, 0x08 /* Private */,
      17,    1,  109,    2, 0x08 /* Private */,
      18,    1,  112,    2, 0x08 /* Private */,
      19,    4,  115,    2, 0x08 /* Private */,
      23,    1,  124,    2, 0x08 /* Private */,
      25,    1,  127,    2, 0x08 /* Private */,
      27,    0,  130,    2, 0x08 /* Private */,
      28,    1,  131,    2, 0x08 /* Private */,
      30,    0,  134,    2, 0x08 /* Private */,
      31,    0,  135,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Int,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   11,   12,   13,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::UChar, QMetaType::Bool, QMetaType::Bool,    2,   20,   21,   22,
    QMetaType::Void, QMetaType::Int,   24,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Controller::MainController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainController *_t = static_cast<MainController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->enteredInRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 1: _t->exitedFromRoom((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->inputSelectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_disconnectedFromLoginServer(); break;
        case 4: _t->on_VSTPluginFounded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 5: _t->on_connectedInNinjamServer((*reinterpret_cast< Ninjam::Server(*)>(_a[1]))); break;
        case 6: _t->on_disconnectedFromNinjamServer((*reinterpret_cast< const Ninjam::Server(*)>(_a[1]))); break;
        case 7: _t->on_errorInNinjamServer((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->on_ninjamEncodedAudioAvailableToSend((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 9: _t->on_ninjamBpiChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->on_ninjamBpmChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->on_newNinjamInterval(); break;
        case 12: _t->on_audioDriverSampleRateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->on_audioDriverStopped(); break;
        case 14: _t->on_audioDriverStarted(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainController::*_t)(Login::RoomInfo );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainController::enteredInRoom)) {
                *result = 0;
            }
        }
        {
            typedef void (MainController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainController::exitedFromRoom)) {
                *result = 1;
            }
        }
        {
            typedef void (MainController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainController::inputSelectionChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject Controller::MainController::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_Controller__MainController.data,
      qt_meta_data_Controller__MainController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Controller::MainController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::MainController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Controller__MainController.stringdata0))
        return static_cast<void*>(const_cast< MainController*>(this));
    if (!strcmp(_clname, "Audio::AudioDriverListener"))
        return static_cast< Audio::AudioDriverListener*>(const_cast< MainController*>(this));
    return QApplication::qt_metacast(_clname);
}

int Controller::MainController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void Controller::MainController::enteredInRoom(Login::RoomInfo _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Controller::MainController::exitedFromRoom(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Controller::MainController::inputSelectionChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
