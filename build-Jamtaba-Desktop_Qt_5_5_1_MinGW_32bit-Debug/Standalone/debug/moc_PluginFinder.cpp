/****************************************************************************
** Meta object code from reading C++ file 'PluginFinder.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/audio/vst/PluginFinder.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PluginFinder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Vst__PluginFinder_t {
    QByteArrayData data[9];
    char stringdata0[97];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Vst__PluginFinder_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Vst__PluginFinder_t qt_meta_stringdata_Vst__PluginFinder = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Vst::PluginFinder"
QT_MOC_LITERAL(1, 18, 11), // "scanStarted"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 12), // "scanFinished"
QT_MOC_LITERAL(4, 44, 17), // "pluginScanStarted"
QT_MOC_LITERAL(5, 62, 4), // "path"
QT_MOC_LITERAL(6, 67, 18), // "pluginScanFinished"
QT_MOC_LITERAL(7, 86, 4), // "name"
QT_MOC_LITERAL(8, 91, 5) // "group"

    },
    "Vst::PluginFinder\0scanStarted\0\0"
    "scanFinished\0pluginScanStarted\0path\0"
    "pluginScanFinished\0name\0group"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Vst__PluginFinder[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,
       4,    1,   36,    2, 0x06 /* Public */,
       6,    3,   39,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    7,    8,    5,

       0        // eod
};

void Vst::PluginFinder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PluginFinder *_t = static_cast<PluginFinder *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->scanStarted(); break;
        case 1: _t->scanFinished(); break;
        case 2: _t->pluginScanStarted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->pluginScanFinished((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PluginFinder::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PluginFinder::scanStarted)) {
                *result = 0;
            }
        }
        {
            typedef void (PluginFinder::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PluginFinder::scanFinished)) {
                *result = 1;
            }
        }
        {
            typedef void (PluginFinder::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PluginFinder::pluginScanStarted)) {
                *result = 2;
            }
        }
        {
            typedef void (PluginFinder::*_t)(QString , QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PluginFinder::pluginScanFinished)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject Vst::PluginFinder::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Vst__PluginFinder.data,
      qt_meta_data_Vst__PluginFinder,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Vst::PluginFinder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Vst::PluginFinder::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Vst__PluginFinder.stringdata0))
        return static_cast<void*>(const_cast< PluginFinder*>(this));
    return QObject::qt_metacast(_clname);
}

int Vst::PluginFinder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Vst::PluginFinder::scanStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Vst::PluginFinder::scanFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Vst::PluginFinder::pluginScanStarted(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Vst::PluginFinder::pluginScanFinished(QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
