/****************************************************************************
** Meta object code from reading C++ file 'mainframe.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/jamtaba/gui/mainframe.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainframe.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainFrame_t {
    QByteArrayData data[45];
    char stringdata0[715];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainFrame_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainFrame_t qt_meta_stringdata_MainFrame = {
    {
QT_MOC_LITERAL(0, 0, 9), // "MainFrame"
QT_MOC_LITERAL(1, 10, 18), // "on_tabCloseRequest"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 5), // "index"
QT_MOC_LITERAL(4, 36, 13), // "on_tabChanged"
QT_MOC_LITERAL(5, 50, 21), // "on_preferencesClicked"
QT_MOC_LITERAL(6, 72, 8), // "QAction*"
QT_MOC_LITERAL(7, 81, 6), // "action"
QT_MOC_LITERAL(8, 88, 23), // "on_IOPreferencesChanged"
QT_MOC_LITERAL(9, 112, 11), // "QList<bool>"
QT_MOC_LITERAL(10, 124, 11), // "audioDevice"
QT_MOC_LITERAL(11, 136, 7), // "firstIn"
QT_MOC_LITERAL(12, 144, 6), // "lastIn"
QT_MOC_LITERAL(13, 151, 8), // "firstOut"
QT_MOC_LITERAL(14, 160, 7), // "lastOut"
QT_MOC_LITERAL(15, 168, 10), // "sampleRate"
QT_MOC_LITERAL(16, 179, 10), // "bufferSize"
QT_MOC_LITERAL(17, 190, 21), // "on_roomsListAvailable"
QT_MOC_LITERAL(18, 212, 22), // "QList<Login::RoomInfo>"
QT_MOC_LITERAL(19, 235, 11), // "publicRooms"
QT_MOC_LITERAL(20, 247, 33), // "on_newVersionAvailableForDown..."
QT_MOC_LITERAL(21, 281, 36), // "on_incompatibilityWithServerD..."
QT_MOC_LITERAL(22, 318, 26), // "on_errorConnectingToServer"
QT_MOC_LITERAL(23, 345, 21), // "on_startingRoomStream"
QT_MOC_LITERAL(24, 367, 15), // "Login::RoomInfo"
QT_MOC_LITERAL(25, 383, 8), // "roomInfo"
QT_MOC_LITERAL(26, 392, 21), // "on_stoppingRoomStream"
QT_MOC_LITERAL(27, 414, 17), // "on_enteringInRoom"
QT_MOC_LITERAL(28, 432, 16), // "on_enteredInRoom"
QT_MOC_LITERAL(29, 449, 17), // "on_exitedFromRoom"
QT_MOC_LITERAL(30, 467, 19), // "normalDisconnection"
QT_MOC_LITERAL(31, 487, 19), // "onPluginScanStarted"
QT_MOC_LITERAL(32, 507, 20), // "onPluginScanFinished"
QT_MOC_LITERAL(33, 528, 15), // "onPluginFounded"
QT_MOC_LITERAL(34, 544, 4), // "name"
QT_MOC_LITERAL(35, 549, 5), // "group"
QT_MOC_LITERAL(36, 555, 4), // "path"
QT_MOC_LITERAL(37, 560, 24), // "on_inputSelectionChanged"
QT_MOC_LITERAL(38, 585, 15), // "inputTrackIndex"
QT_MOC_LITERAL(39, 601, 37), // "on_localControlsCollapseButto..."
QT_MOC_LITERAL(40, 639, 21), // "on_channelNameChanged"
QT_MOC_LITERAL(41, 661, 20), // "on_xmitButtonClicked"
QT_MOC_LITERAL(42, 682, 7), // "checked"
QT_MOC_LITERAL(43, 690, 20), // "on_RoomStreamerError"
QT_MOC_LITERAL(44, 711, 3) // "msg"

    },
    "MainFrame\0on_tabCloseRequest\0\0index\0"
    "on_tabChanged\0on_preferencesClicked\0"
    "QAction*\0action\0on_IOPreferencesChanged\0"
    "QList<bool>\0audioDevice\0firstIn\0lastIn\0"
    "firstOut\0lastOut\0sampleRate\0bufferSize\0"
    "on_roomsListAvailable\0QList<Login::RoomInfo>\0"
    "publicRooms\0on_newVersionAvailableForDownload\0"
    "on_incompatibilityWithServerDetected\0"
    "on_errorConnectingToServer\0"
    "on_startingRoomStream\0Login::RoomInfo\0"
    "roomInfo\0on_stoppingRoomStream\0"
    "on_enteringInRoom\0on_enteredInRoom\0"
    "on_exitedFromRoom\0normalDisconnection\0"
    "onPluginScanStarted\0onPluginScanFinished\0"
    "onPluginFounded\0name\0group\0path\0"
    "on_inputSelectionChanged\0inputTrackIndex\0"
    "on_localControlsCollapseButtonClicked\0"
    "on_channelNameChanged\0on_xmitButtonClicked\0"
    "checked\0on_RoomStreamerError\0msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainFrame[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  119,    2, 0x08 /* Private */,
       4,    1,  122,    2, 0x08 /* Private */,
       5,    1,  125,    2, 0x08 /* Private */,
       8,    8,  128,    2, 0x08 /* Private */,
      17,    1,  145,    2, 0x08 /* Private */,
      20,    0,  148,    2, 0x08 /* Private */,
      21,    0,  149,    2, 0x08 /* Private */,
      22,    0,  150,    2, 0x08 /* Private */,
      23,    1,  151,    2, 0x08 /* Private */,
      26,    1,  154,    2, 0x08 /* Private */,
      27,    1,  157,    2, 0x08 /* Private */,
      28,    1,  160,    2, 0x08 /* Private */,
      29,    1,  163,    2, 0x08 /* Private */,
      31,    0,  166,    2, 0x08 /* Private */,
      32,    0,  167,    2, 0x08 /* Private */,
      33,    3,  168,    2, 0x08 /* Private */,
      37,    1,  175,    2, 0x08 /* Private */,
      39,    0,  178,    2, 0x08 /* Private */,
      40,    0,  179,    2, 0x08 /* Private */,
      41,    1,  180,    2, 0x08 /* Private */,
      43,    1,  183,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,   10,   11,   12,   13,   14,   15,   16,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 24,   25,
    QMetaType::Void, 0x80000000 | 24,   25,
    QMetaType::Void, 0x80000000 | 24,   25,
    QMetaType::Void, 0x80000000 | 24,   25,
    QMetaType::Void, QMetaType::Bool,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   34,   35,   36,
    QMetaType::Void, QMetaType::Int,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   42,
    QMetaType::Void, QMetaType::QString,   44,

       0        // eod
};

void MainFrame::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainFrame *_t = static_cast<MainFrame *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_tabCloseRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_tabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_preferencesClicked((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: _t->on_IOPreferencesChanged((*reinterpret_cast< QList<bool>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8]))); break;
        case 4: _t->on_roomsListAvailable((*reinterpret_cast< QList<Login::RoomInfo>(*)>(_a[1]))); break;
        case 5: _t->on_newVersionAvailableForDownload(); break;
        case 6: _t->on_incompatibilityWithServerDetected(); break;
        case 7: _t->on_errorConnectingToServer(); break;
        case 8: _t->on_startingRoomStream((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 9: _t->on_stoppingRoomStream((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 10: _t->on_enteringInRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 11: _t->on_enteredInRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 12: _t->on_exitedFromRoom((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->onPluginScanStarted(); break;
        case 14: _t->onPluginScanFinished(); break;
        case 15: _t->onPluginFounded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 16: _t->on_inputSelectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->on_localControlsCollapseButtonClicked(); break;
        case 18: _t->on_channelNameChanged(); break;
        case 19: _t->on_xmitButtonClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->on_RoomStreamerError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<bool> >(); break;
            }
            break;
        }
    }
}

const QMetaObject MainFrame::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainFrame.data,
      qt_meta_data_MainFrame,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainFrame::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainFrame.stringdata0))
        return static_cast<void*>(const_cast< MainFrame*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
