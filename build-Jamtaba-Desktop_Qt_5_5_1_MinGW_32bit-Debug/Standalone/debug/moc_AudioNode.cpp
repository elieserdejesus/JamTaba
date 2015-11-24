/****************************************************************************
** Meta object code from reading C++ file 'AudioNode.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/audio/core/AudioNode.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AudioNode.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Audio__AudioNode_t {
    QByteArrayData data[10];
    char stringdata0[102];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Audio__AudioNode_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Audio__AudioNode_t qt_meta_stringdata_Audio__AudioNode = {
    {
QT_MOC_LITERAL(0, 0, 16), // "Audio::AudioNode"
QT_MOC_LITERAL(1, 17, 11), // "gainChanged"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 7), // "newGain"
QT_MOC_LITERAL(4, 38, 10), // "panChanged"
QT_MOC_LITERAL(5, 49, 6), // "newPan"
QT_MOC_LITERAL(6, 56, 11), // "muteChanged"
QT_MOC_LITERAL(7, 68, 10), // "muteStatus"
QT_MOC_LITERAL(8, 79, 11), // "soloChanged"
QT_MOC_LITERAL(9, 91, 10) // "soloStatus"

    },
    "Audio::AudioNode\0gainChanged\0\0newGain\0"
    "panChanged\0newPan\0muteChanged\0muteStatus\0"
    "soloChanged\0soloStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Audio__AudioNode[] = {

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
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       6,    1,   40,    2, 0x06 /* Public */,
       8,    1,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void, QMetaType::Float,    5,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    9,

       0        // eod
};

void Audio::AudioNode::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioNode *_t = static_cast<AudioNode *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->gainChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 1: _t->panChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 2: _t->muteChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->soloChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AudioNode::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioNode::gainChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (AudioNode::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioNode::panChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (AudioNode::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioNode::muteChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (AudioNode::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioNode::soloChanged)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject Audio::AudioNode::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Audio__AudioNode.data,
      qt_meta_data_Audio__AudioNode,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Audio::AudioNode::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Audio::AudioNode::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Audio__AudioNode.stringdata0))
        return static_cast<void*>(const_cast< AudioNode*>(this));
    return QObject::qt_metacast(_clname);
}

int Audio::AudioNode::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Audio::AudioNode::gainChanged(float _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Audio::AudioNode::panChanged(float _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Audio::AudioNode::muteChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Audio::AudioNode::soloChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
