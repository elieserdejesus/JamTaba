/****************************************************************************
** Meta object code from reading C++ file 'RoomStreamerNode.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/audio/RoomStreamerNode.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RoomStreamerNode.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Audio__AbstractMp3Streamer_t {
    QByteArrayData data[1];
    char stringdata[27];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Audio__AbstractMp3Streamer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Audio__AbstractMp3Streamer_t qt_meta_stringdata_Audio__AbstractMp3Streamer = {
    {
QT_MOC_LITERAL(0, 0, 26) // "Audio::AbstractMp3Streamer"

    },
    "Audio::AbstractMp3Streamer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Audio__AbstractMp3Streamer[] = {

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

void Audio::AbstractMp3Streamer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Audio::AbstractMp3Streamer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Audio__AbstractMp3Streamer.data,
      qt_meta_data_Audio__AbstractMp3Streamer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Audio::AbstractMp3Streamer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Audio::AbstractMp3Streamer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Audio__AbstractMp3Streamer.stringdata))
        return static_cast<void*>(const_cast< AbstractMp3Streamer*>(this));
    if (!strcmp(_clname, "AudioNode"))
        return static_cast< AudioNode*>(const_cast< AbstractMp3Streamer*>(this));
    return QObject::qt_metacast(_clname);
}

int Audio::AbstractMp3Streamer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Audio__RoomStreamerNode_t {
    QByteArrayData data[5];
    char stringdata[76];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Audio__RoomStreamerNode_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Audio__RoomStreamerNode_t qt_meta_stringdata_Audio__RoomStreamerNode = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Audio::RoomStreamerNode"
QT_MOC_LITERAL(1, 24, 11), // "reply_error"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 27), // "QNetworkReply::NetworkError"
QT_MOC_LITERAL(4, 65, 10) // "reply_read"

    },
    "Audio::RoomStreamerNode\0reply_error\0"
    "\0QNetworkReply::NetworkError\0reply_read"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Audio__RoomStreamerNode[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x08 /* Private */,
       4,    0,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,

       0        // eod
};

void Audio::RoomStreamerNode::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RoomStreamerNode *_t = static_cast<RoomStreamerNode *>(_o);
        switch (_id) {
        case 0: _t->reply_error((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 1: _t->reply_read(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply::NetworkError >(); break;
            }
            break;
        }
    }
}

const QMetaObject Audio::RoomStreamerNode::staticMetaObject = {
    { &AbstractMp3Streamer::staticMetaObject, qt_meta_stringdata_Audio__RoomStreamerNode.data,
      qt_meta_data_Audio__RoomStreamerNode,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Audio::RoomStreamerNode::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Audio::RoomStreamerNode::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Audio__RoomStreamerNode.stringdata))
        return static_cast<void*>(const_cast< RoomStreamerNode*>(this));
    return AbstractMp3Streamer::qt_metacast(_clname);
}

int Audio::RoomStreamerNode::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractMp3Streamer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_Audio__AudioFileStreamerNode_t {
    QByteArrayData data[1];
    char stringdata[29];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Audio__AudioFileStreamerNode_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Audio__AudioFileStreamerNode_t qt_meta_stringdata_Audio__AudioFileStreamerNode = {
    {
QT_MOC_LITERAL(0, 0, 28) // "Audio::AudioFileStreamerNode"

    },
    "Audio::AudioFileStreamerNode"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Audio__AudioFileStreamerNode[] = {

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

void Audio::AudioFileStreamerNode::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Audio::AudioFileStreamerNode::staticMetaObject = {
    { &AbstractMp3Streamer::staticMetaObject, qt_meta_stringdata_Audio__AudioFileStreamerNode.data,
      qt_meta_data_Audio__AudioFileStreamerNode,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Audio::AudioFileStreamerNode::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Audio::AudioFileStreamerNode::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Audio__AudioFileStreamerNode.stringdata))
        return static_cast<void*>(const_cast< AudioFileStreamerNode*>(this));
    return AbstractMp3Streamer::qt_metacast(_clname);
}

int Audio::AudioFileStreamerNode::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractMp3Streamer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
