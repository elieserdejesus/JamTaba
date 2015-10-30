/****************************************************************************
** Meta object code from reading C++ file 'MainController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/MainController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Controller__MainController_t {
    QByteArrayData data[27];
    char stringdata0[454];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__MainController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__MainController_t qt_meta_stringdata_Controller__MainController = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Controller::MainController"
QT_MOC_LITERAL(1, 27, 26), // "on_connectedInNinjamServer"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 14), // "Ninjam::Server"
QT_MOC_LITERAL(4, 70, 6), // "server"
QT_MOC_LITERAL(5, 77, 31), // "on_disconnectedFromNinjamServer"
QT_MOC_LITERAL(6, 109, 22), // "on_errorInNinjamServer"
QT_MOC_LITERAL(7, 132, 5), // "error"
QT_MOC_LITERAL(8, 138, 36), // "on_ninjamEncodedAudioAvailabl..."
QT_MOC_LITERAL(9, 175, 12), // "channelIndex"
QT_MOC_LITERAL(10, 188, 11), // "isFirstPart"
QT_MOC_LITERAL(11, 200, 10), // "isLastPart"
QT_MOC_LITERAL(12, 211, 19), // "on_ninjamBpiChanged"
QT_MOC_LITERAL(13, 231, 6), // "newBpi"
QT_MOC_LITERAL(14, 238, 19), // "on_ninjamBpmChanged"
QT_MOC_LITERAL(15, 258, 6), // "newBpm"
QT_MOC_LITERAL(16, 265, 20), // "on_newNinjamInterval"
QT_MOC_LITERAL(17, 286, 24), // "on_ninjamStartProcessing"
QT_MOC_LITERAL(18, 311, 16), // "intervalPosition"
QT_MOC_LITERAL(19, 328, 31), // "on_audioDriverSampleRateChanged"
QT_MOC_LITERAL(20, 360, 13), // "newSampleRate"
QT_MOC_LITERAL(21, 374, 21), // "on_audioDriverStarted"
QT_MOC_LITERAL(22, 396, 21), // "on_audioDriverStopped"
QT_MOC_LITERAL(23, 418, 19), // "on_VSTPluginFounded"
QT_MOC_LITERAL(24, 438, 4), // "name"
QT_MOC_LITERAL(25, 443, 5), // "group"
QT_MOC_LITERAL(26, 449, 4) // "path"

    },
    "Controller::MainController\0"
    "on_connectedInNinjamServer\0\0Ninjam::Server\0"
    "server\0on_disconnectedFromNinjamServer\0"
    "on_errorInNinjamServer\0error\0"
    "on_ninjamEncodedAudioAvailableToSend\0"
    "channelIndex\0isFirstPart\0isLastPart\0"
    "on_ninjamBpiChanged\0newBpi\0"
    "on_ninjamBpmChanged\0newBpm\0"
    "on_newNinjamInterval\0on_ninjamStartProcessing\0"
    "intervalPosition\0on_audioDriverSampleRateChanged\0"
    "newSampleRate\0on_audioDriverStarted\0"
    "on_audioDriverStopped\0on_VSTPluginFounded\0"
    "name\0group\0path"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller__MainController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x09 /* Protected */,
       5,    1,   77,    2, 0x09 /* Protected */,
       6,    1,   80,    2, 0x09 /* Protected */,
       8,    4,   83,    2, 0x09 /* Protected */,
      12,    1,   92,    2, 0x09 /* Protected */,
      14,    1,   95,    2, 0x09 /* Protected */,
      16,    0,   98,    2, 0x09 /* Protected */,
      17,    1,   99,    2, 0x09 /* Protected */,
      19,    1,  102,    2, 0x09 /* Protected */,
      21,    0,  105,    2, 0x09 /* Protected */,
      22,    0,  106,    2, 0x09 /* Protected */,
      23,    3,  107,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::UChar, QMetaType::Bool, QMetaType::Bool,    2,    9,   10,   11,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   24,   25,   26,

       0        // eod
};

void Controller::MainController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainController *_t = static_cast<MainController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_connectedInNinjamServer((*reinterpret_cast< Ninjam::Server(*)>(_a[1]))); break;
        case 1: _t->on_disconnectedFromNinjamServer((*reinterpret_cast< const Ninjam::Server(*)>(_a[1]))); break;
        case 2: _t->on_errorInNinjamServer((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->on_ninjamEncodedAudioAvailableToSend((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 4: _t->on_ninjamBpiChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_ninjamBpmChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_newNinjamInterval(); break;
        case 7: _t->on_ninjamStartProcessing((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_audioDriverSampleRateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->on_audioDriverStarted(); break;
        case 10: _t->on_audioDriverStopped(); break;
        case 11: _t->on_VSTPluginFounded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject Controller::MainController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Controller__MainController.data,
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
    return QObject::qt_metacast(_clname);
}

int Controller::MainController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
