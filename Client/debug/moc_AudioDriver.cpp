/****************************************************************************
** Meta object code from reading C++ file 'AudioDriver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/jamtaba/audio/core/AudioDriver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AudioDriver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Audio__AudioDriver_t {
    QByteArrayData data[6];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Audio__AudioDriver_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Audio__AudioDriver_t qt_meta_stringdata_Audio__AudioDriver = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Audio::AudioDriver"
QT_MOC_LITERAL(1, 19, 17), // "sampleRateChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 13), // "newSampleRate"
QT_MOC_LITERAL(4, 52, 7), // "stopped"
QT_MOC_LITERAL(5, 60, 7) // "started"

    },
    "Audio::AudioDriver\0sampleRateChanged\0"
    "\0newSampleRate\0stopped\0started"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Audio__AudioDriver[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    0,   32,    2, 0x06 /* Public */,
       5,    0,   33,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Audio::AudioDriver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioDriver *_t = static_cast<AudioDriver *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sampleRateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->stopped(); break;
        case 2: _t->started(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AudioDriver::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioDriver::sampleRateChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (AudioDriver::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioDriver::stopped)) {
                *result = 1;
            }
        }
        {
            typedef void (AudioDriver::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioDriver::started)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject Audio::AudioDriver::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Audio__AudioDriver.data,
      qt_meta_data_Audio__AudioDriver,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Audio::AudioDriver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Audio::AudioDriver::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Audio__AudioDriver.stringdata0))
        return static_cast<void*>(const_cast< AudioDriver*>(this));
    return QObject::qt_metacast(_clname);
}

int Audio::AudioDriver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Audio::AudioDriver::sampleRateChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Audio::AudioDriver::stopped()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Audio::AudioDriver::started()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
