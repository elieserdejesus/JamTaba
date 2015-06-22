/****************************************************************************
** Meta object code from reading C++ file 'MainController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/MainController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Controller__MainController_t {
    QByteArrayData data[12];
    char stringdata[195];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller__MainController_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller__MainController_t qt_meta_stringdata_Controller__MainController = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Controller::MainController"
QT_MOC_LITERAL(1, 27, 13), // "enteredInRoom"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 23), // "Login::AbstractJamRoom*"
QT_MOC_LITERAL(4, 66, 4), // "room"
QT_MOC_LITERAL(5, 71, 25), // "on_disconnectedFromServer"
QT_MOC_LITERAL(6, 97, 15), // "onPluginFounded"
QT_MOC_LITERAL(7, 113, 24), // "Audio::PluginDescriptor*"
QT_MOC_LITERAL(8, 138, 10), // "descriptor"
QT_MOC_LITERAL(9, 149, 23), // "connectedInNinjamServer"
QT_MOC_LITERAL(10, 173, 14), // "Ninjam::Server"
QT_MOC_LITERAL(11, 188, 6) // "server"

    },
    "Controller::MainController\0enteredInRoom\0"
    "\0Login::AbstractJamRoom*\0room\0"
    "on_disconnectedFromServer\0onPluginFounded\0"
    "Audio::PluginDescriptor*\0descriptor\0"
    "connectedInNinjamServer\0Ninjam::Server\0"
    "server"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller__MainController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   37,    2, 0x08 /* Private */,
       6,    1,   38,    2, 0x08 /* Private */,
       9,    1,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 10,   11,

       0        // eod
};

void Controller::MainController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainController *_t = static_cast<MainController *>(_o);
        switch (_id) {
        case 0: _t->enteredInRoom((*reinterpret_cast< Login::AbstractJamRoom*(*)>(_a[1]))); break;
        case 1: _t->on_disconnectedFromServer(); break;
        case 2: _t->onPluginFounded((*reinterpret_cast< Audio::PluginDescriptor*(*)>(_a[1]))); break;
        case 3: _t->connectedInNinjamServer((*reinterpret_cast< const Ninjam::Server(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Audio::PluginDescriptor* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainController::*_t)(Login::AbstractJamRoom * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainController::enteredInRoom)) {
                *result = 0;
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
    if (!strcmp(_clname, qt_meta_stringdata_Controller__MainController.stringdata))
        return static_cast<void*>(const_cast< MainController*>(this));
    return QApplication::qt_metacast(_clname);
}

int Controller::MainController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Controller::MainController::enteredInRoom(Login::AbstractJamRoom * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
