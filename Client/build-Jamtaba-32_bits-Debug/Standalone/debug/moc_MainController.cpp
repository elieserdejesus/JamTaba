/****************************************************************************
** Meta object code from reading C++ file 'MainController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/MainController.h"
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
struct qt_meta_stringdata_Controller__MainControllerSignalsHandler_t {
    QByteArrayData data[28];
    char stringdata0[499];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__MainControllerSignalsHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__MainControllerSignalsHandler_t qt_meta_stringdata_Controller__MainControllerSignalsHandler = {
    {
QT_MOC_LITERAL(0, 0, 40), // "Controller::MainControllerSig..."
QT_MOC_LITERAL(1, 41, 30), // "on_disconnectedFromLoginServer"
QT_MOC_LITERAL(2, 72, 0), // ""
QT_MOC_LITERAL(3, 73, 26), // "on_connectedInNinjamServer"
QT_MOC_LITERAL(4, 100, 14), // "Ninjam::Server"
QT_MOC_LITERAL(5, 115, 6), // "server"
QT_MOC_LITERAL(6, 122, 31), // "on_disconnectedFromNinjamServer"
QT_MOC_LITERAL(7, 154, 22), // "on_errorInNinjamServer"
QT_MOC_LITERAL(8, 177, 5), // "error"
QT_MOC_LITERAL(9, 183, 36), // "on_ninjamEncodedAudioAvailabl..."
QT_MOC_LITERAL(10, 220, 12), // "channelIndex"
QT_MOC_LITERAL(11, 233, 11), // "isFirstPart"
QT_MOC_LITERAL(12, 245, 10), // "isLastPart"
QT_MOC_LITERAL(13, 256, 19), // "on_ninjamBpiChanged"
QT_MOC_LITERAL(14, 276, 6), // "newBpi"
QT_MOC_LITERAL(15, 283, 19), // "on_ninjamBpmChanged"
QT_MOC_LITERAL(16, 303, 6), // "newBpm"
QT_MOC_LITERAL(17, 310, 20), // "on_newNinjamInterval"
QT_MOC_LITERAL(18, 331, 24), // "on_ninjamStartProcessing"
QT_MOC_LITERAL(19, 356, 16), // "intervalPosition"
QT_MOC_LITERAL(20, 373, 31), // "on_audioDriverSampleRateChanged"
QT_MOC_LITERAL(21, 405, 13), // "newSampleRate"
QT_MOC_LITERAL(22, 419, 21), // "on_audioDriverStarted"
QT_MOC_LITERAL(23, 441, 21), // "on_audioDriverStopped"
QT_MOC_LITERAL(24, 463, 19), // "on_VSTPluginFounded"
QT_MOC_LITERAL(25, 483, 4), // "name"
QT_MOC_LITERAL(26, 488, 5), // "group"
QT_MOC_LITERAL(27, 494, 4) // "path"

    },
    "Controller::MainControllerSignalsHandler\0"
    "on_disconnectedFromLoginServer\0\0"
    "on_connectedInNinjamServer\0Ninjam::Server\0"
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

static const uint qt_meta_data_Controller__MainControllerSignalsHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x0a /* Public */,
       3,    1,   80,    2, 0x0a /* Public */,
       6,    1,   83,    2, 0x0a /* Public */,
       7,    1,   86,    2, 0x0a /* Public */,
       9,    4,   89,    2, 0x0a /* Public */,
      13,    1,   98,    2, 0x0a /* Public */,
      15,    1,  101,    2, 0x0a /* Public */,
      17,    0,  104,    2, 0x0a /* Public */,
      18,    1,  105,    2, 0x0a /* Public */,
      20,    1,  108,    2, 0x0a /* Public */,
      22,    0,  111,    2, 0x0a /* Public */,
      23,    0,  112,    2, 0x0a /* Public */,
      24,    3,  113,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::UChar, QMetaType::Bool, QMetaType::Bool,    2,   10,   11,   12,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   25,   26,   27,

       0        // eod
};

void Controller::MainControllerSignalsHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainControllerSignalsHandler *_t = static_cast<MainControllerSignalsHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_disconnectedFromLoginServer(); break;
        case 1: _t->on_connectedInNinjamServer((*reinterpret_cast< Ninjam::Server(*)>(_a[1]))); break;
        case 2: _t->on_disconnectedFromNinjamServer((*reinterpret_cast< const Ninjam::Server(*)>(_a[1]))); break;
        case 3: _t->on_errorInNinjamServer((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->on_ninjamEncodedAudioAvailableToSend((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 5: _t->on_ninjamBpiChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_ninjamBpmChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_newNinjamInterval(); break;
        case 8: _t->on_ninjamStartProcessing((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->on_audioDriverSampleRateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->on_audioDriverStarted(); break;
        case 11: _t->on_audioDriverStopped(); break;
        case 12: _t->on_VSTPluginFounded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject Controller::MainControllerSignalsHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Controller__MainControllerSignalsHandler.data,
      qt_meta_data_Controller__MainControllerSignalsHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Controller::MainControllerSignalsHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::MainControllerSignalsHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Controller__MainControllerSignalsHandler.stringdata0))
        return static_cast<void*>(const_cast< MainControllerSignalsHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int Controller::MainControllerSignalsHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
