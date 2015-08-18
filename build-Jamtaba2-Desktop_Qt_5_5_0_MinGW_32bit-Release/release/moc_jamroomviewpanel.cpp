/****************************************************************************
** Meta object code from reading C++ file 'jamroomviewpanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/gui/jamroomviewpanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jamroomviewpanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_JamRoomViewPanel_t {
    QByteArrayData data[9];
    char stringdata0[159];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_JamRoomViewPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_JamRoomViewPanel_t qt_meta_stringdata_JamRoomViewPanel = {
    {
QT_MOC_LITERAL(0, 0, 16), // "JamRoomViewPanel"
QT_MOC_LITERAL(1, 17, 24), // "startingListeningTheRoom"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 15), // "Login::RoomInfo"
QT_MOC_LITERAL(4, 59, 8), // "roomInfo"
QT_MOC_LITERAL(5, 68, 25), // "finishingListeningTheRoom"
QT_MOC_LITERAL(6, 94, 17), // "enteringInTheRoom"
QT_MOC_LITERAL(7, 112, 23), // "on_buttonListen_clicked"
QT_MOC_LITERAL(8, 136, 22) // "on_buttonEnter_clicked"

    },
    "JamRoomViewPanel\0startingListeningTheRoom\0"
    "\0Login::RoomInfo\0roomInfo\0"
    "finishingListeningTheRoom\0enteringInTheRoom\0"
    "on_buttonListen_clicked\0on_buttonEnter_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_JamRoomViewPanel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       5,    1,   42,    2, 0x06 /* Public */,
       6,    1,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   48,    2, 0x08 /* Private */,
       8,    0,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void JamRoomViewPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        JamRoomViewPanel *_t = static_cast<JamRoomViewPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startingListeningTheRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 1: _t->finishingListeningTheRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 2: _t->enteringInTheRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 3: _t->on_buttonListen_clicked(); break;
        case 4: _t->on_buttonEnter_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (JamRoomViewPanel::*_t)(Login::RoomInfo );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JamRoomViewPanel::startingListeningTheRoom)) {
                *result = 0;
            }
        }
        {
            typedef void (JamRoomViewPanel::*_t)(Login::RoomInfo );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JamRoomViewPanel::finishingListeningTheRoom)) {
                *result = 1;
            }
        }
        {
            typedef void (JamRoomViewPanel::*_t)(Login::RoomInfo );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&JamRoomViewPanel::enteringInTheRoom)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject JamRoomViewPanel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_JamRoomViewPanel.data,
      qt_meta_data_JamRoomViewPanel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *JamRoomViewPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *JamRoomViewPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_JamRoomViewPanel.stringdata0))
        return static_cast<void*>(const_cast< JamRoomViewPanel*>(this));
    return QWidget::qt_metacast(_clname);
}

int JamRoomViewPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void JamRoomViewPanel::startingListeningTheRoom(Login::RoomInfo _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void JamRoomViewPanel::finishingListeningTheRoom(Login::RoomInfo _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void JamRoomViewPanel::enteringInTheRoom(Login::RoomInfo _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
