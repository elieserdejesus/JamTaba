/****************************************************************************
** Meta object code from reading C++ file 'LoginService.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/loginserver/LoginService.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LoginService.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Login__LoginService_t {
    QByteArrayData data[20];
    char stringdata[294];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Login__LoginService_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Login__LoginService_t qt_meta_stringdata_Login__LoginService = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Login::LoginService"
QT_MOC_LITERAL(1, 20, 17), // "connectedInServer"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 30), // "QList<Login::AbstractJamRoom*>"
QT_MOC_LITERAL(4, 70, 5), // "rooms"
QT_MOC_LITERAL(5, 76, 22), // "disconnectedFromServer"
QT_MOC_LITERAL(6, 99, 11), // "roomChanged"
QT_MOC_LITERAL(7, 111, 15), // "AbstractJamRoom"
QT_MOC_LITERAL(8, 127, 4), // "room"
QT_MOC_LITERAL(9, 132, 13), // "connectedSlot"
QT_MOC_LITERAL(10, 146, 16), // "disconnectedSlot"
QT_MOC_LITERAL(11, 163, 9), // "errorSlot"
QT_MOC_LITERAL(12, 173, 27), // "QNetworkReply::NetworkError"
QT_MOC_LITERAL(13, 201, 13), // "sslErrorsSlot"
QT_MOC_LITERAL(14, 215, 16), // "QList<QSslError>"
QT_MOC_LITERAL(15, 232, 24), // "connectNetworkReplySlots"
QT_MOC_LITERAL(16, 257, 14), // "QNetworkReply*"
QT_MOC_LITERAL(17, 272, 5), // "reply"
QT_MOC_LITERAL(18, 278, 7), // "Command"
QT_MOC_LITERAL(19, 286, 7) // "command"

    },
    "Login::LoginService\0connectedInServer\0"
    "\0QList<Login::AbstractJamRoom*>\0rooms\0"
    "disconnectedFromServer\0roomChanged\0"
    "AbstractJamRoom\0room\0connectedSlot\0"
    "disconnectedSlot\0errorSlot\0"
    "QNetworkReply::NetworkError\0sslErrorsSlot\0"
    "QList<QSslError>\0connectNetworkReplySlots\0"
    "QNetworkReply*\0reply\0Command\0command"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Login__LoginService[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    0,   57,    2, 0x06 /* Public */,
       6,    1,   58,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   61,    2, 0x08 /* Private */,
      10,    0,   62,    2, 0x08 /* Private */,
      11,    1,   63,    2, 0x08 /* Private */,
      13,    1,   66,    2, 0x08 /* Private */,
      15,    2,   69,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,    2,
    QMetaType::Void, 0x80000000 | 14,    2,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 18,   17,   19,

       0        // eod
};

void Login::LoginService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LoginService *_t = static_cast<LoginService *>(_o);
        switch (_id) {
        case 0: _t->connectedInServer((*reinterpret_cast< QList<Login::AbstractJamRoom*>(*)>(_a[1]))); break;
        case 1: _t->disconnectedFromServer(); break;
        case 2: _t->roomChanged((*reinterpret_cast< const AbstractJamRoom(*)>(_a[1]))); break;
        case 3: _t->connectedSlot(); break;
        case 4: _t->disconnectedSlot(); break;
        case 5: _t->errorSlot((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 6: _t->sslErrorsSlot((*reinterpret_cast< QList<QSslError>(*)>(_a[1]))); break;
        case 7: _t->connectNetworkReplySlots((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< Command(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply::NetworkError >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LoginService::*_t)(QList<Login::AbstractJamRoom*> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LoginService::connectedInServer)) {
                *result = 0;
            }
        }
        {
            typedef void (LoginService::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LoginService::disconnectedFromServer)) {
                *result = 1;
            }
        }
        {
            typedef void (LoginService::*_t)(const AbstractJamRoom & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LoginService::roomChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject Login::LoginService::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Login__LoginService.data,
      qt_meta_data_Login__LoginService,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Login::LoginService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Login::LoginService::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Login__LoginService.stringdata))
        return static_cast<void*>(const_cast< LoginService*>(this));
    return QObject::qt_metacast(_clname);
}

int Login::LoginService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Login::LoginService::connectedInServer(QList<Login::AbstractJamRoom*> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Login::LoginService::disconnectedFromServer()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Login::LoginService::roomChanged(const AbstractJamRoom & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
