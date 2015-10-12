/****************************************************************************
** Meta object code from reading C++ file 'StandAloneMainController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/StandAloneMainController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StandAloneMainController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Controller__StandaloneSignalsHandler_t {
    QByteArrayData data[18];
    char stringdata0[303];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__StandaloneSignalsHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__StandaloneSignalsHandler_t qt_meta_stringdata_Controller__StandaloneSignalsHandler = {
    {
QT_MOC_LITERAL(0, 0, 36), // "Controller::StandaloneSignals..."
QT_MOC_LITERAL(1, 37, 19), // "on_ninjamBpmChanged"
QT_MOC_LITERAL(2, 57, 0), // ""
QT_MOC_LITERAL(3, 58, 6), // "newBpm"
QT_MOC_LITERAL(4, 65, 20), // "on_newNinjamInterval"
QT_MOC_LITERAL(5, 86, 24), // "on_ninjamStartProcessing"
QT_MOC_LITERAL(6, 111, 16), // "intervalPosition"
QT_MOC_LITERAL(7, 128, 26), // "on_connectedInNinjamServer"
QT_MOC_LITERAL(8, 155, 14), // "Ninjam::Server"
QT_MOC_LITERAL(9, 170, 6), // "server"
QT_MOC_LITERAL(10, 177, 31), // "on_audioDriverSampleRateChanged"
QT_MOC_LITERAL(11, 209, 13), // "newSampleRate"
QT_MOC_LITERAL(12, 223, 21), // "on_audioDriverStarted"
QT_MOC_LITERAL(13, 245, 21), // "on_audioDriverStopped"
QT_MOC_LITERAL(14, 267, 19), // "on_VSTPluginFounded"
QT_MOC_LITERAL(15, 287, 4), // "name"
QT_MOC_LITERAL(16, 292, 5), // "group"
QT_MOC_LITERAL(17, 298, 4) // "path"

    },
    "Controller::StandaloneSignalsHandler\0"
    "on_ninjamBpmChanged\0\0newBpm\0"
    "on_newNinjamInterval\0on_ninjamStartProcessing\0"
    "intervalPosition\0on_connectedInNinjamServer\0"
    "Ninjam::Server\0server\0"
    "on_audioDriverSampleRateChanged\0"
    "newSampleRate\0on_audioDriverStarted\0"
    "on_audioDriverStopped\0on_VSTPluginFounded\0"
    "name\0group\0path"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller__StandaloneSignalsHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x09 /* Protected */,
       4,    0,   57,    2, 0x09 /* Protected */,
       5,    1,   58,    2, 0x09 /* Protected */,
       7,    1,   61,    2, 0x09 /* Protected */,
      10,    1,   64,    2, 0x09 /* Protected */,
      12,    0,   67,    2, 0x09 /* Protected */,
      13,    0,   68,    2, 0x09 /* Protected */,
      14,    3,   69,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   15,   16,   17,

       0        // eod
};

void Controller::StandaloneSignalsHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StandaloneSignalsHandler *_t = static_cast<StandaloneSignalsHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_ninjamBpmChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_newNinjamInterval(); break;
        case 2: _t->on_ninjamStartProcessing((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_connectedInNinjamServer((*reinterpret_cast< Ninjam::Server(*)>(_a[1]))); break;
        case 4: _t->on_audioDriverSampleRateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_audioDriverStarted(); break;
        case 6: _t->on_audioDriverStopped(); break;
        case 7: _t->on_VSTPluginFounded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject Controller::StandaloneSignalsHandler::staticMetaObject = {
    { &MainControllerSignalsHandler::staticMetaObject, qt_meta_stringdata_Controller__StandaloneSignalsHandler.data,
      qt_meta_data_Controller__StandaloneSignalsHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Controller::StandaloneSignalsHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::StandaloneSignalsHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Controller__StandaloneSignalsHandler.stringdata0))
        return static_cast<void*>(const_cast< StandaloneSignalsHandler*>(this));
    return MainControllerSignalsHandler::qt_metacast(_clname);
}

int Controller::StandaloneSignalsHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MainControllerSignalsHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
struct qt_meta_stringdata_Controller__StandaloneMainController_t {
    QByteArrayData data[1];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__StandaloneMainController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__StandaloneMainController_t qt_meta_stringdata_Controller__StandaloneMainController = {
    {
QT_MOC_LITERAL(0, 0, 36) // "Controller::StandaloneMainCon..."

    },
    "Controller::StandaloneMainController"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller__StandaloneMainController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void Controller::StandaloneMainController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Controller::StandaloneMainController::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_Controller__StandaloneMainController.data,
      qt_meta_data_Controller__StandaloneMainController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Controller::StandaloneMainController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::StandaloneMainController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Controller__StandaloneMainController.stringdata0))
        return static_cast<void*>(const_cast< StandaloneMainController*>(this));
    if (!strcmp(_clname, "MainController"))
        return static_cast< MainController*>(const_cast< StandaloneMainController*>(this));
    return QApplication::qt_metacast(_clname);
}

int Controller::StandaloneMainController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
