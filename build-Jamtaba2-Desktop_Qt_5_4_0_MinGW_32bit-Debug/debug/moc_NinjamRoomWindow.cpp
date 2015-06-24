/****************************************************************************
** Meta object code from reading C++ file 'NinjamRoomWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/gui/NinjamRoomWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NinjamRoomWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NinjamRoomWindow_t {
    QByteArrayData data[20];
    char stringdata[267];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NinjamRoomWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NinjamRoomWindow_t qt_meta_stringdata_NinjamRoomWindow = {
    {
QT_MOC_LITERAL(0, 0, 16), // "NinjamRoomWindow"
QT_MOC_LITERAL(1, 17, 21), // "ninjamBpiComboChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 21), // "ninjamBpmComboChanged"
QT_MOC_LITERAL(4, 62, 25), // "ninjamAccentsComboChanged"
QT_MOC_LITERAL(5, 88, 10), // "bpiChanged"
QT_MOC_LITERAL(6, 99, 3), // "bpi"
QT_MOC_LITERAL(7, 103, 10), // "bpmChanged"
QT_MOC_LITERAL(8, 114, 3), // "bpm"
QT_MOC_LITERAL(9, 118, 19), // "intervalBeatChanged"
QT_MOC_LITERAL(10, 138, 4), // "beat"
QT_MOC_LITERAL(11, 143, 12), // "channelAdded"
QT_MOC_LITERAL(12, 156, 12), // "Ninjam::User"
QT_MOC_LITERAL(13, 169, 4), // "user"
QT_MOC_LITERAL(14, 174, 19), // "Ninjam::UserChannel"
QT_MOC_LITERAL(15, 194, 7), // "channel"
QT_MOC_LITERAL(16, 202, 9), // "channelID"
QT_MOC_LITERAL(17, 212, 14), // "channelRemoved"
QT_MOC_LITERAL(18, 227, 14), // "channelChanged"
QT_MOC_LITERAL(19, 242, 24) // "on_licenceButton_clicked"

    },
    "NinjamRoomWindow\0ninjamBpiComboChanged\0"
    "\0ninjamBpmComboChanged\0ninjamAccentsComboChanged\0"
    "bpiChanged\0bpi\0bpmChanged\0bpm\0"
    "intervalBeatChanged\0beat\0channelAdded\0"
    "Ninjam::User\0user\0Ninjam::UserChannel\0"
    "channel\0channelID\0channelRemoved\0"
    "channelChanged\0on_licenceButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NinjamRoomWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x08 /* Private */,
       3,    1,   67,    2, 0x08 /* Private */,
       4,    1,   70,    2, 0x08 /* Private */,
       5,    1,   73,    2, 0x08 /* Private */,
       7,    1,   76,    2, 0x08 /* Private */,
       9,    1,   79,    2, 0x08 /* Private */,
      11,    3,   82,    2, 0x08 /* Private */,
      17,    3,   89,    2, 0x08 /* Private */,
      18,    3,   96,    2, 0x08 /* Private */,
      19,    0,  103,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14, QMetaType::Long,   13,   15,   16,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14, QMetaType::Long,   13,   15,   16,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 14, QMetaType::Long,   13,   15,   16,
    QMetaType::Void,

       0        // eod
};

void NinjamRoomWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NinjamRoomWindow *_t = static_cast<NinjamRoomWindow *>(_o);
        switch (_id) {
        case 0: _t->ninjamBpiComboChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->ninjamBpmComboChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->ninjamAccentsComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->bpiChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->bpmChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->intervalBeatChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->channelAdded((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 7: _t->channelRemoved((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 8: _t->channelChanged((*reinterpret_cast< Ninjam::User(*)>(_a[1])),(*reinterpret_cast< Ninjam::UserChannel(*)>(_a[2])),(*reinterpret_cast< long(*)>(_a[3]))); break;
        case 9: _t->on_licenceButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject NinjamRoomWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NinjamRoomWindow.data,
      qt_meta_data_NinjamRoomWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NinjamRoomWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NinjamRoomWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NinjamRoomWindow.stringdata))
        return static_cast<void*>(const_cast< NinjamRoomWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int NinjamRoomWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
