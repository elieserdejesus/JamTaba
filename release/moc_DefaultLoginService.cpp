/****************************************************************************
** Meta object code from reading C++ file 'DefaultLoginService.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/jamtaba/network/loginserver/DefaultLoginService.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DefaultLoginService.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DefaultLoginService_t {
    QByteArrayData data[13];
    char stringdata[205];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DefaultLoginService_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DefaultLoginService_t qt_meta_stringdata_DefaultLoginService = {
    {
QT_MOC_LITERAL(0, 0, 19), // "DefaultLoginService"
QT_MOC_LITERAL(1, 20, 13), // "connectedSlot"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 16), // "disconnectedSlot"
QT_MOC_LITERAL(4, 52, 9), // "errorSlot"
QT_MOC_LITERAL(5, 62, 27), // "QNetworkReply::NetworkError"
QT_MOC_LITERAL(6, 90, 13), // "sslErrorsSlot"
QT_MOC_LITERAL(7, 104, 16), // "QList<QSslError>"
QT_MOC_LITERAL(8, 121, 24), // "connectNetworkReplySlots"
QT_MOC_LITERAL(9, 146, 14), // "QNetworkReply*"
QT_MOC_LITERAL(10, 161, 5), // "reply"
QT_MOC_LITERAL(11, 167, 29), // "LoginServiceCommands::COMMAND"
QT_MOC_LITERAL(12, 197, 7) // "command"

    },
    "DefaultLoginService\0connectedSlot\0\0"
    "disconnectedSlot\0errorSlot\0"
    "QNetworkReply::NetworkError\0sslErrorsSlot\0"
    "QList<QSslError>\0connectNetworkReplySlots\0"
    "QNetworkReply*\0reply\0LoginServiceCommands::COMMAND\0"
    "command"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DefaultLoginService[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x09 /* Protected */,
       3,    0,   40,    2, 0x09 /* Protected */,
       4,    1,   41,    2, 0x09 /* Protected */,
       6,    1,   44,    2, 0x09 /* Protected */,
       8,    2,   47,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11,   10,   12,

       0        // eod
};

void DefaultLoginService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DefaultLoginService *_t = static_cast<DefaultLoginService *>(_o);
        switch (_id) {
        case 0: _t->connectedSlot(); break;
        case 1: _t->disconnectedSlot(); break;
        case 2: _t->errorSlot((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 3: _t->sslErrorsSlot((*reinterpret_cast< QList<QSslError>(*)>(_a[1]))); break;
        case 4: _t->connectNetworkReplySlots((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< LoginServiceCommands::COMMAND(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply::NetworkError >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    }
}

const QMetaObject DefaultLoginService::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DefaultLoginService.data,
      qt_meta_data_DefaultLoginService,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DefaultLoginService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DefaultLoginService::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DefaultLoginService.stringdata))
        return static_cast<void*>(const_cast< DefaultLoginService*>(this));
    if (!strcmp(_clname, "LoginService"))
        return static_cast< LoginService*>(const_cast< DefaultLoginService*>(this));
    return QObject::qt_metacast(_clname);
}

int DefaultLoginService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
