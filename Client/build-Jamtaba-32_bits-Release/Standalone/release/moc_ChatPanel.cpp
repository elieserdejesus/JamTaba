/****************************************************************************
** Meta object code from reading C++ file 'ChatPanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/gui/ChatPanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChatPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ChatPanel_t {
    QByteArrayData data[14];
    char stringdata0[223];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ChatPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ChatPanel_t qt_meta_stringdata_ChatPanel = {
    {
QT_MOC_LITERAL(0, 0, 9), // "ChatPanel"
QT_MOC_LITERAL(1, 10, 21), // "userSendingNewMessage"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 3), // "msg"
QT_MOC_LITERAL(4, 37, 29), // "userConfirmingVoteToBpiChange"
QT_MOC_LITERAL(5, 67, 6), // "newBpi"
QT_MOC_LITERAL(6, 74, 29), // "userConfirmingVoteToBpmChange"
QT_MOC_LITERAL(7, 104, 6), // "newBpm"
QT_MOC_LITERAL(8, 111, 26), // "on_chatTextEditionFinished"
QT_MOC_LITERAL(9, 138, 32), // "on_verticalScrollBarRangeChanged"
QT_MOC_LITERAL(10, 171, 3), // "min"
QT_MOC_LITERAL(11, 175, 3), // "max"
QT_MOC_LITERAL(12, 179, 22), // "on_buttonClear_clicked"
QT_MOC_LITERAL(13, 202, 20) // "on_voteButtonClicked"

    },
    "ChatPanel\0userSendingNewMessage\0\0msg\0"
    "userConfirmingVoteToBpiChange\0newBpi\0"
    "userConfirmingVoteToBpmChange\0newBpm\0"
    "on_chatTextEditionFinished\0"
    "on_verticalScrollBarRangeChanged\0min\0"
    "max\0on_buttonClear_clicked\0"
    "on_voteButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ChatPanel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       6,    1,   55,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   58,    2, 0x08 /* Private */,
       9,    2,   59,    2, 0x08 /* Private */,
      12,    0,   64,    2, 0x08 /* Private */,
      13,    0,   65,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   10,   11,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ChatPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ChatPanel *_t = static_cast<ChatPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->userSendingNewMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->userConfirmingVoteToBpiChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->userConfirmingVoteToBpmChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_chatTextEditionFinished(); break;
        case 4: _t->on_verticalScrollBarRangeChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->on_buttonClear_clicked(); break;
        case 6: _t->on_voteButtonClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ChatPanel::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ChatPanel::userSendingNewMessage)) {
                *result = 0;
            }
        }
        {
            typedef void (ChatPanel::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ChatPanel::userConfirmingVoteToBpiChange)) {
                *result = 1;
            }
        }
        {
            typedef void (ChatPanel::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ChatPanel::userConfirmingVoteToBpmChange)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject ChatPanel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ChatPanel.data,
      qt_meta_data_ChatPanel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ChatPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChatPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ChatPanel.stringdata0))
        return static_cast<void*>(const_cast< ChatPanel*>(this));
    return QWidget::qt_metacast(_clname);
}

int ChatPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ChatPanel::userSendingNewMessage(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ChatPanel::userConfirmingVoteToBpiChange(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ChatPanel::userConfirmingVoteToBpmChange(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
