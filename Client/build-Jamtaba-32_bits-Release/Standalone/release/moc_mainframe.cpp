/****************************************************************************
** Meta object code from reading C++ file 'mainframe.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/gui/mainframe.h"
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
    QByteArrayData data[56];
    char stringdata0[958];
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
QT_MOC_LITERAL(5, 50, 19), // "on_newThemeSelected"
QT_MOC_LITERAL(6, 70, 8), // "QAction*"
QT_MOC_LITERAL(7, 79, 21), // "on_preferencesClicked"
QT_MOC_LITERAL(8, 101, 6), // "action"
QT_MOC_LITERAL(9, 108, 23), // "on_IOPreferencesChanged"
QT_MOC_LITERAL(10, 132, 11), // "QList<bool>"
QT_MOC_LITERAL(11, 144, 11), // "audioDevice"
QT_MOC_LITERAL(12, 156, 7), // "firstIn"
QT_MOC_LITERAL(13, 164, 6), // "lastIn"
QT_MOC_LITERAL(14, 171, 8), // "firstOut"
QT_MOC_LITERAL(15, 180, 7), // "lastOut"
QT_MOC_LITERAL(16, 188, 10), // "sampleRate"
QT_MOC_LITERAL(17, 199, 10), // "bufferSize"
QT_MOC_LITERAL(18, 210, 35), // "on_ninjamCommunityMenuItemTri..."
QT_MOC_LITERAL(19, 246, 38), // "on_ninjamOfficialSiteMenuItem..."
QT_MOC_LITERAL(20, 285, 33), // "on_privateServerMenuItemTrigg..."
QT_MOC_LITERAL(21, 319, 29), // "on_reportBugMenuItemTriggered"
QT_MOC_LITERAL(22, 349, 34), // "on_privateServerConnectionAcc..."
QT_MOC_LITERAL(23, 384, 6), // "server"
QT_MOC_LITERAL(24, 391, 10), // "serverPort"
QT_MOC_LITERAL(25, 402, 8), // "password"
QT_MOC_LITERAL(26, 411, 21), // "on_roomsListAvailable"
QT_MOC_LITERAL(27, 433, 22), // "QList<Login::RoomInfo>"
QT_MOC_LITERAL(28, 456, 11), // "publicRooms"
QT_MOC_LITERAL(29, 468, 33), // "on_newVersionAvailableForDown..."
QT_MOC_LITERAL(30, 502, 36), // "on_incompatibilityWithServerD..."
QT_MOC_LITERAL(31, 539, 26), // "on_errorConnectingToServer"
QT_MOC_LITERAL(32, 566, 8), // "errorMsg"
QT_MOC_LITERAL(33, 575, 21), // "on_startingRoomStream"
QT_MOC_LITERAL(34, 597, 15), // "Login::RoomInfo"
QT_MOC_LITERAL(35, 613, 8), // "roomInfo"
QT_MOC_LITERAL(36, 622, 21), // "on_stoppingRoomStream"
QT_MOC_LITERAL(37, 644, 17), // "on_enteringInRoom"
QT_MOC_LITERAL(38, 662, 16), // "on_enteredInRoom"
QT_MOC_LITERAL(39, 679, 17), // "on_exitedFromRoom"
QT_MOC_LITERAL(40, 697, 19), // "normalDisconnection"
QT_MOC_LITERAL(41, 717, 20), // "onScanPluginsStarted"
QT_MOC_LITERAL(42, 738, 21), // "onScanPluginsFinished"
QT_MOC_LITERAL(43, 760, 15), // "onPluginFounded"
QT_MOC_LITERAL(44, 776, 4), // "name"
QT_MOC_LITERAL(45, 781, 5), // "group"
QT_MOC_LITERAL(46, 787, 4), // "path"
QT_MOC_LITERAL(47, 792, 10), // "pluginPath"
QT_MOC_LITERAL(48, 803, 24), // "on_inputSelectionChanged"
QT_MOC_LITERAL(49, 828, 15), // "inputTrackIndex"
QT_MOC_LITERAL(50, 844, 37), // "on_localControlsCollapseButto..."
QT_MOC_LITERAL(51, 882, 21), // "on_channelNameChanged"
QT_MOC_LITERAL(52, 904, 20), // "on_xmitButtonClicked"
QT_MOC_LITERAL(53, 925, 7), // "checked"
QT_MOC_LITERAL(54, 933, 20), // "on_RoomStreamerError"
QT_MOC_LITERAL(55, 954, 3) // "msg"

    },
    "MainFrame\0on_tabCloseRequest\0\0index\0"
    "on_tabChanged\0on_newThemeSelected\0"
    "QAction*\0on_preferencesClicked\0action\0"
    "on_IOPreferencesChanged\0QList<bool>\0"
    "audioDevice\0firstIn\0lastIn\0firstOut\0"
    "lastOut\0sampleRate\0bufferSize\0"
    "on_ninjamCommunityMenuItemTriggered\0"
    "on_ninjamOfficialSiteMenuItemTriggered\0"
    "on_privateServerMenuItemTriggered\0"
    "on_reportBugMenuItemTriggered\0"
    "on_privateServerConnectionAccepted\0"
    "server\0serverPort\0password\0"
    "on_roomsListAvailable\0QList<Login::RoomInfo>\0"
    "publicRooms\0on_newVersionAvailableForDownload\0"
    "on_incompatibilityWithServerDetected\0"
    "on_errorConnectingToServer\0errorMsg\0"
    "on_startingRoomStream\0Login::RoomInfo\0"
    "roomInfo\0on_stoppingRoomStream\0"
    "on_enteringInRoom\0on_enteredInRoom\0"
    "on_exitedFromRoom\0normalDisconnection\0"
    "onScanPluginsStarted\0onScanPluginsFinished\0"
    "onPluginFounded\0name\0group\0path\0"
    "pluginPath\0on_inputSelectionChanged\0"
    "inputTrackIndex\0on_localControlsCollapseButtonClicked\0"
    "on_channelNameChanged\0on_xmitButtonClicked\0"
    "checked\0on_RoomStreamerError\0msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainFrame[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  159,    2, 0x09 /* Protected */,
       4,    1,  162,    2, 0x09 /* Protected */,
       5,    1,  165,    2, 0x09 /* Protected */,
       7,    1,  168,    2, 0x09 /* Protected */,
       9,    8,  171,    2, 0x09 /* Protected */,
      18,    0,  188,    2, 0x09 /* Protected */,
      19,    0,  189,    2, 0x09 /* Protected */,
      20,    0,  190,    2, 0x09 /* Protected */,
      21,    0,  191,    2, 0x09 /* Protected */,
      22,    3,  192,    2, 0x09 /* Protected */,
      26,    1,  199,    2, 0x09 /* Protected */,
      29,    0,  202,    2, 0x09 /* Protected */,
      30,    0,  203,    2, 0x09 /* Protected */,
      31,    1,  204,    2, 0x09 /* Protected */,
      33,    1,  207,    2, 0x09 /* Protected */,
      36,    1,  210,    2, 0x09 /* Protected */,
      37,    2,  213,    2, 0x09 /* Protected */,
      37,    1,  218,    2, 0x29 /* Protected | MethodCloned */,
      38,    1,  221,    2, 0x09 /* Protected */,
      39,    1,  224,    2, 0x09 /* Protected */,
      41,    0,  227,    2, 0x09 /* Protected */,
      42,    0,  228,    2, 0x09 /* Protected */,
      43,    3,  229,    2, 0x09 /* Protected */,
      41,    1,  236,    2, 0x09 /* Protected */,
      48,    1,  239,    2, 0x09 /* Protected */,
      50,    0,  242,    2, 0x09 /* Protected */,
      51,    0,  243,    2, 0x09 /* Protected */,
      52,    1,  244,    2, 0x09 /* Protected */,
      54,    1,  247,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, 0x80000000 | 6,    8,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,   11,   12,   13,   14,   15,   16,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,   23,   24,   25,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   32,
    QMetaType::Void, 0x80000000 | 34,   35,
    QMetaType::Void, 0x80000000 | 34,   35,
    QMetaType::Void, 0x80000000 | 34, QMetaType::QString,   35,   25,
    QMetaType::Void, 0x80000000 | 34,   35,
    QMetaType::Void, 0x80000000 | 34,   35,
    QMetaType::Void, QMetaType::Bool,   40,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   44,   45,   46,
    QMetaType::Void, QMetaType::QString,   47,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   53,
    QMetaType::Void, QMetaType::QString,   55,

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
        case 2: _t->on_newThemeSelected((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 3: _t->on_preferencesClicked((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 4: _t->on_IOPreferencesChanged((*reinterpret_cast< QList<bool>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8]))); break;
        case 5: _t->on_ninjamCommunityMenuItemTriggered(); break;
        case 6: _t->on_ninjamOfficialSiteMenuItemTriggered(); break;
        case 7: _t->on_privateServerMenuItemTriggered(); break;
        case 8: _t->on_reportBugMenuItemTriggered(); break;
        case 9: _t->on_privateServerConnectionAccepted((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 10: _t->on_roomsListAvailable((*reinterpret_cast< QList<Login::RoomInfo>(*)>(_a[1]))); break;
        case 11: _t->on_newVersionAvailableForDownload(); break;
        case 12: _t->on_incompatibilityWithServerDetected(); break;
        case 13: _t->on_errorConnectingToServer((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->on_startingRoomStream((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 15: _t->on_stoppingRoomStream((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 16: _t->on_enteringInRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 17: _t->on_enteringInRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 18: _t->on_enteredInRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 19: _t->on_exitedFromRoom((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->onScanPluginsStarted(); break;
        case 21: _t->onScanPluginsFinished(); break;
        case 22: _t->onPluginFounded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 23: _t->onScanPluginsStarted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 24: _t->on_inputSelectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->on_localControlsCollapseButtonClicked(); break;
        case 26: _t->on_channelNameChanged(); break;
        case 27: _t->on_xmitButtonClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: _t->on_RoomStreamerError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 4:
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
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
