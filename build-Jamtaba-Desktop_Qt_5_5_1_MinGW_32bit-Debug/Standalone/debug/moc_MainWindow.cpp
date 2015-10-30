/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/gui/MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[51];
    char stringdata0[863];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 18), // "on_tabCloseRequest"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 5), // "index"
QT_MOC_LITERAL(4, 37, 13), // "on_tabChanged"
QT_MOC_LITERAL(5, 51, 19), // "on_newThemeSelected"
QT_MOC_LITERAL(6, 71, 8), // "QAction*"
QT_MOC_LITERAL(7, 80, 21), // "on_preferencesClicked"
QT_MOC_LITERAL(8, 102, 6), // "action"
QT_MOC_LITERAL(9, 109, 23), // "on_IOPreferencesChanged"
QT_MOC_LITERAL(10, 133, 11), // "QList<bool>"
QT_MOC_LITERAL(11, 145, 11), // "audioDevice"
QT_MOC_LITERAL(12, 157, 7), // "firstIn"
QT_MOC_LITERAL(13, 165, 6), // "lastIn"
QT_MOC_LITERAL(14, 172, 8), // "firstOut"
QT_MOC_LITERAL(15, 181, 7), // "lastOut"
QT_MOC_LITERAL(16, 189, 10), // "sampleRate"
QT_MOC_LITERAL(17, 200, 10), // "bufferSize"
QT_MOC_LITERAL(18, 211, 35), // "on_ninjamCommunityMenuItemTri..."
QT_MOC_LITERAL(19, 247, 38), // "on_ninjamOfficialSiteMenuItem..."
QT_MOC_LITERAL(20, 286, 33), // "on_privateServerMenuItemTrigg..."
QT_MOC_LITERAL(21, 320, 29), // "on_reportBugMenuItemTriggered"
QT_MOC_LITERAL(22, 350, 34), // "on_privateServerConnectionAcc..."
QT_MOC_LITERAL(23, 385, 6), // "server"
QT_MOC_LITERAL(24, 392, 10), // "serverPort"
QT_MOC_LITERAL(25, 403, 8), // "password"
QT_MOC_LITERAL(26, 412, 21), // "on_roomsListAvailable"
QT_MOC_LITERAL(27, 434, 22), // "QList<Login::RoomInfo>"
QT_MOC_LITERAL(28, 457, 11), // "publicRooms"
QT_MOC_LITERAL(29, 469, 33), // "on_newVersionAvailableForDown..."
QT_MOC_LITERAL(30, 503, 36), // "on_incompatibilityWithServerD..."
QT_MOC_LITERAL(31, 540, 26), // "on_errorConnectingToServer"
QT_MOC_LITERAL(32, 567, 8), // "errorMsg"
QT_MOC_LITERAL(33, 576, 21), // "on_startingRoomStream"
QT_MOC_LITERAL(34, 598, 15), // "Login::RoomInfo"
QT_MOC_LITERAL(35, 614, 8), // "roomInfo"
QT_MOC_LITERAL(36, 623, 21), // "on_stoppingRoomStream"
QT_MOC_LITERAL(37, 645, 17), // "on_enteringInRoom"
QT_MOC_LITERAL(38, 663, 20), // "onScanPluginsStarted"
QT_MOC_LITERAL(39, 684, 21), // "onScanPluginsFinished"
QT_MOC_LITERAL(40, 706, 15), // "onPluginFounded"
QT_MOC_LITERAL(41, 722, 4), // "name"
QT_MOC_LITERAL(42, 727, 5), // "group"
QT_MOC_LITERAL(43, 733, 4), // "path"
QT_MOC_LITERAL(44, 738, 10), // "pluginPath"
QT_MOC_LITERAL(45, 749, 37), // "on_localControlsCollapseButto..."
QT_MOC_LITERAL(46, 787, 21), // "on_channelNameChanged"
QT_MOC_LITERAL(47, 809, 20), // "on_xmitButtonClicked"
QT_MOC_LITERAL(48, 830, 7), // "checked"
QT_MOC_LITERAL(49, 838, 20), // "on_RoomStreamerError"
QT_MOC_LITERAL(50, 859, 3) // "msg"

    },
    "MainWindow\0on_tabCloseRequest\0\0index\0"
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
    "on_enteringInRoom\0onScanPluginsStarted\0"
    "onScanPluginsFinished\0onPluginFounded\0"
    "name\0group\0path\0pluginPath\0"
    "on_localControlsCollapseButtonClicked\0"
    "on_channelNameChanged\0on_xmitButtonClicked\0"
    "checked\0on_RoomStreamerError\0msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  144,    2, 0x09 /* Protected */,
       4,    1,  147,    2, 0x09 /* Protected */,
       5,    1,  150,    2, 0x09 /* Protected */,
       7,    1,  153,    2, 0x09 /* Protected */,
       9,    8,  156,    2, 0x09 /* Protected */,
      18,    0,  173,    2, 0x09 /* Protected */,
      19,    0,  174,    2, 0x09 /* Protected */,
      20,    0,  175,    2, 0x09 /* Protected */,
      21,    0,  176,    2, 0x09 /* Protected */,
      22,    3,  177,    2, 0x09 /* Protected */,
      26,    1,  184,    2, 0x09 /* Protected */,
      29,    0,  187,    2, 0x09 /* Protected */,
      30,    0,  188,    2, 0x09 /* Protected */,
      31,    1,  189,    2, 0x09 /* Protected */,
      33,    1,  192,    2, 0x09 /* Protected */,
      36,    1,  195,    2, 0x09 /* Protected */,
      37,    2,  198,    2, 0x09 /* Protected */,
      37,    1,  203,    2, 0x29 /* Protected | MethodCloned */,
      38,    0,  206,    2, 0x09 /* Protected */,
      39,    0,  207,    2, 0x09 /* Protected */,
      40,    3,  208,    2, 0x09 /* Protected */,
      38,    1,  215,    2, 0x09 /* Protected */,
      45,    0,  218,    2, 0x09 /* Protected */,
      46,    0,  219,    2, 0x09 /* Protected */,
      47,    1,  220,    2, 0x09 /* Protected */,
      49,    1,  223,    2, 0x09 /* Protected */,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   41,   42,   43,
    QMetaType::Void, QMetaType::QString,   44,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::QString,   50,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
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
        case 18: _t->onScanPluginsStarted(); break;
        case 19: _t->onScanPluginsFinished(); break;
        case 20: _t->onPluginFounded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 21: _t->onScanPluginsStarted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 22: _t->on_localControlsCollapseButtonClicked(); break;
        case 23: _t->on_channelNameChanged(); break;
        case 24: _t->on_xmitButtonClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 25: _t->on_RoomStreamerError((*reinterpret_cast< QString(*)>(_a[1]))); break;
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

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
