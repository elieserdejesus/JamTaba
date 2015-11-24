/****************************************************************************
** Meta object code from reading C++ file 'vsthost.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/audio/vst/vsthost.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vsthost.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Vst__Host_t {
    QByteArrayData data[6];
    char stringdata0[70];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Vst__Host_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Vst__Host_t qt_meta_stringdata_Vst__Host = {
    {
QT_MOC_LITERAL(0, 0, 9), // "Vst::Host"
QT_MOC_LITERAL(1, 10, 28), // "pluginRequestingWindowResize"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 10), // "pluginName"
QT_MOC_LITERAL(4, 51, 8), // "newWidth"
QT_MOC_LITERAL(5, 60, 9) // "newHeight"

    },
    "Vst::Host\0pluginRequestingWindowResize\0"
    "\0pluginName\0newWidth\0newHeight"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Vst__Host[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,    3,    4,    5,

       0        // eod
};

void Vst::Host::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Host *_t = static_cast<Host *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pluginRequestingWindowResize((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Host::*_t)(QString , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Host::pluginRequestingWindowResize)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Vst::Host::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Vst__Host.data,
      qt_meta_data_Vst__Host,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Vst::Host::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Vst::Host::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Vst__Host.stringdata0))
        return static_cast<void*>(const_cast< Host*>(this));
    return QObject::qt_metacast(_clname);
}

int Vst::Host::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Vst::Host::pluginRequestingWindowResize(QString _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
