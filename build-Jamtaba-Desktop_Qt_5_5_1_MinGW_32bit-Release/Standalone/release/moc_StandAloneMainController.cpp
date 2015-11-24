/****************************************************************************
** Meta object code from reading C++ file 'StandAloneMainController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/StandAloneMainController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StandAloneMainController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Controller__StandaloneMainController_t {
    QByteArrayData data[22];
    char stringdata0[367];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__StandaloneMainController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__StandaloneMainController_t qt_meta_stringdata_Controller__StandaloneMainController = {
    {
QT_MOC_LITERAL(0, 0, 36), // "Controller::StandaloneMainCon..."
QT_MOC_LITERAL(1, 37, 19), // "on_ninjamBpmChanged"
QT_MOC_LITERAL(2, 57, 0), // ""
QT_MOC_LITERAL(3, 58, 6), // "newBpm"
QT_MOC_LITERAL(4, 65, 26), // "on_connectedInNinjamServer"
QT_MOC_LITERAL(5, 92, 14), // "Ninjam::Server"
QT_MOC_LITERAL(6, 107, 6), // "server"
QT_MOC_LITERAL(7, 114, 31), // "on_audioDriverSampleRateChanged"
QT_MOC_LITERAL(8, 146, 13), // "newSampleRate"
QT_MOC_LITERAL(9, 160, 21), // "on_audioDriverStarted"
QT_MOC_LITERAL(10, 182, 21), // "on_audioDriverStopped"
QT_MOC_LITERAL(11, 204, 20), // "on_newNinjamInterval"
QT_MOC_LITERAL(12, 225, 24), // "on_ninjamStartProcessing"
QT_MOC_LITERAL(13, 250, 16), // "intervalPosition"
QT_MOC_LITERAL(14, 267, 19), // "on_VSTPluginFounded"
QT_MOC_LITERAL(15, 287, 4), // "name"
QT_MOC_LITERAL(16, 292, 5), // "group"
QT_MOC_LITERAL(17, 298, 4), // "path"
QT_MOC_LITERAL(18, 303, 33), // "on_vstPluginRequestedWindowRe..."
QT_MOC_LITERAL(19, 337, 10), // "pluginName"
QT_MOC_LITERAL(20, 348, 8), // "newWidht"
QT_MOC_LITERAL(21, 357, 9) // "newHeight"

    },
    "Controller::StandaloneMainController\0"
    "on_ninjamBpmChanged\0\0newBpm\0"
    "on_connectedInNinjamServer\0Ninjam::Server\0"
    "server\0on_audioDriverSampleRateChanged\0"
    "newSampleRate\0on_audioDriverStarted\0"
    "on_audioDriverStopped\0on_newNinjamInterval\0"
    "on_ninjamStartProcessing\0intervalPosition\0"
    "on_VSTPluginFounded\0name\0group\0path\0"
    "on_vstPluginRequestedWindowResize\0"
    "pluginName\0newWidht\0newHeight"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller__StandaloneMainController[] = {

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
       1,    1,   59,    2, 0x09 /* Protected */,
       4,    1,   62,    2, 0x09 /* Protected */,
       7,    1,   65,    2, 0x09 /* Protected */,
       9,    0,   68,    2, 0x09 /* Protected */,
      10,    0,   69,    2, 0x09 /* Protected */,
      11,    0,   70,    2, 0x09 /* Protected */,
      12,    1,   71,    2, 0x09 /* Protected */,
      14,    3,   74,    2, 0x09 /* Protected */,
      18,    3,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   15,   16,   17,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,   19,   20,   21,

       0        // eod
};

void Controller::StandaloneMainController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StandaloneMainController *_t = static_cast<StandaloneMainController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_ninjamBpmChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_connectedInNinjamServer((*reinterpret_cast< Ninjam::Server(*)>(_a[1]))); break;
        case 2: _t->on_audioDriverSampleRateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_audioDriverStarted(); break;
        case 4: _t->on_audioDriverStopped(); break;
        case 5: _t->on_newNinjamInterval(); break;
        case 6: _t->on_ninjamStartProcessing((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_VSTPluginFounded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 8: _t->on_vstPluginRequestedWindowResize((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject Controller::StandaloneMainController::staticMetaObject = {
    { &MainController::staticMetaObject, qt_meta_stringdata_Controller__StandaloneMainController.data,
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
    return MainController::qt_metacast(_clname);
}

int Controller::StandaloneMainController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MainController::qt_metacall(_c, _id, _a);
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
