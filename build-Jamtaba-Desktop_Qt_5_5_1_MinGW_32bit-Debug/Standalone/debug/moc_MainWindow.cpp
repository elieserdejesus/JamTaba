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
    QByteArrayData data[57];
    char stringdata0[1052];
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
QT_MOC_LITERAL(5, 51, 18), // "on_localTrackAdded"
QT_MOC_LITERAL(6, 70, 20), // "on_localTrackRemoved"
QT_MOC_LITERAL(7, 91, 21), // "on_preferencesClicked"
QT_MOC_LITERAL(8, 113, 8), // "QAction*"
QT_MOC_LITERAL(9, 122, 6), // "action"
QT_MOC_LITERAL(10, 129, 23), // "on_IOPreferencesChanged"
QT_MOC_LITERAL(11, 153, 11), // "QList<bool>"
QT_MOC_LITERAL(12, 165, 11), // "audioDevice"
QT_MOC_LITERAL(13, 177, 7), // "firstIn"
QT_MOC_LITERAL(14, 185, 6), // "lastIn"
QT_MOC_LITERAL(15, 192, 8), // "firstOut"
QT_MOC_LITERAL(16, 201, 7), // "lastOut"
QT_MOC_LITERAL(17, 209, 10), // "sampleRate"
QT_MOC_LITERAL(18, 220, 10), // "bufferSize"
QT_MOC_LITERAL(19, 231, 35), // "on_ninjamCommunityMenuItemTri..."
QT_MOC_LITERAL(20, 267, 38), // "on_ninjamOfficialSiteMenuItem..."
QT_MOC_LITERAL(21, 306, 33), // "on_privateServerMenuItemTrigg..."
QT_MOC_LITERAL(22, 340, 24), // "on_menuViewModeTriggered"
QT_MOC_LITERAL(23, 365, 25), // "ninjamTransmissionStarted"
QT_MOC_LITERAL(24, 391, 25), // "ninjamPreparingToTransmit"
QT_MOC_LITERAL(25, 417, 29), // "on_reportBugMenuItemTriggered"
QT_MOC_LITERAL(26, 447, 24), // "on_wikiMenuItemTriggered"
QT_MOC_LITERAL(27, 472, 31), // "on_UsersManualMenuItemTriggered"
QT_MOC_LITERAL(28, 504, 34), // "on_privateServerConnectionAcc..."
QT_MOC_LITERAL(29, 539, 6), // "server"
QT_MOC_LITERAL(30, 546, 10), // "serverPort"
QT_MOC_LITERAL(31, 557, 8), // "password"
QT_MOC_LITERAL(32, 566, 21), // "on_roomsListAvailable"
QT_MOC_LITERAL(33, 588, 22), // "QList<Login::RoomInfo>"
QT_MOC_LITERAL(34, 611, 11), // "publicRooms"
QT_MOC_LITERAL(35, 623, 33), // "on_newVersionAvailableForDown..."
QT_MOC_LITERAL(36, 657, 36), // "on_incompatibilityWithServerD..."
QT_MOC_LITERAL(37, 694, 26), // "on_errorConnectingToServer"
QT_MOC_LITERAL(38, 721, 8), // "errorMsg"
QT_MOC_LITERAL(39, 730, 21), // "on_startingRoomStream"
QT_MOC_LITERAL(40, 752, 15), // "Login::RoomInfo"
QT_MOC_LITERAL(41, 768, 8), // "roomInfo"
QT_MOC_LITERAL(42, 777, 21), // "on_stoppingRoomStream"
QT_MOC_LITERAL(43, 799, 17), // "on_enteringInRoom"
QT_MOC_LITERAL(44, 817, 20), // "onScanPluginsStarted"
QT_MOC_LITERAL(45, 838, 21), // "onScanPluginsFinished"
QT_MOC_LITERAL(46, 860, 15), // "onPluginFounded"
QT_MOC_LITERAL(47, 876, 4), // "name"
QT_MOC_LITERAL(48, 881, 5), // "group"
QT_MOC_LITERAL(49, 887, 4), // "path"
QT_MOC_LITERAL(50, 892, 10), // "pluginPath"
QT_MOC_LITERAL(51, 903, 37), // "on_localControlsCollapseButto..."
QT_MOC_LITERAL(52, 941, 21), // "on_channelNameChanged"
QT_MOC_LITERAL(53, 963, 20), // "on_RoomStreamerError"
QT_MOC_LITERAL(54, 984, 3), // "msg"
QT_MOC_LITERAL(55, 988, 33), // "on_actionFullscreenMode_trigg..."
QT_MOC_LITERAL(56, 1022, 29) // "on_pluginFinderDialogCanceled"

    },
    "MainWindow\0on_tabCloseRequest\0\0index\0"
    "on_tabChanged\0on_localTrackAdded\0"
    "on_localTrackRemoved\0on_preferencesClicked\0"
    "QAction*\0action\0on_IOPreferencesChanged\0"
    "QList<bool>\0audioDevice\0firstIn\0lastIn\0"
    "firstOut\0lastOut\0sampleRate\0bufferSize\0"
    "on_ninjamCommunityMenuItemTriggered\0"
    "on_ninjamOfficialSiteMenuItemTriggered\0"
    "on_privateServerMenuItemTriggered\0"
    "on_menuViewModeTriggered\0"
    "ninjamTransmissionStarted\0"
    "ninjamPreparingToTransmit\0"
    "on_reportBugMenuItemTriggered\0"
    "on_wikiMenuItemTriggered\0"
    "on_UsersManualMenuItemTriggered\0"
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
    "on_channelNameChanged\0on_RoomStreamerError\0"
    "msg\0on_actionFullscreenMode_triggered\0"
    "on_pluginFinderDialogCanceled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  179,    2, 0x09 /* Protected */,
       4,    1,  182,    2, 0x09 /* Protected */,
       5,    0,  185,    2, 0x09 /* Protected */,
       6,    0,  186,    2, 0x09 /* Protected */,
       7,    1,  187,    2, 0x09 /* Protected */,
      10,    8,  190,    2, 0x09 /* Protected */,
      19,    0,  207,    2, 0x09 /* Protected */,
      20,    0,  208,    2, 0x09 /* Protected */,
      21,    0,  209,    2, 0x09 /* Protected */,
      22,    1,  210,    2, 0x09 /* Protected */,
      23,    0,  213,    2, 0x09 /* Protected */,
      24,    0,  214,    2, 0x09 /* Protected */,
      25,    0,  215,    2, 0x09 /* Protected */,
      26,    0,  216,    2, 0x09 /* Protected */,
      27,    0,  217,    2, 0x09 /* Protected */,
      28,    3,  218,    2, 0x09 /* Protected */,
      32,    1,  225,    2, 0x09 /* Protected */,
      35,    0,  228,    2, 0x09 /* Protected */,
      36,    0,  229,    2, 0x09 /* Protected */,
      37,    1,  230,    2, 0x09 /* Protected */,
      39,    1,  233,    2, 0x09 /* Protected */,
      42,    1,  236,    2, 0x09 /* Protected */,
      43,    2,  239,    2, 0x09 /* Protected */,
      43,    1,  244,    2, 0x29 /* Protected | MethodCloned */,
      44,    0,  247,    2, 0x09 /* Protected */,
      45,    0,  248,    2, 0x09 /* Protected */,
      46,    3,  249,    2, 0x09 /* Protected */,
      44,    1,  256,    2, 0x09 /* Protected */,
      51,    0,  259,    2, 0x09 /* Protected */,
      52,    0,  260,    2, 0x09 /* Protected */,
      53,    1,  261,    2, 0x09 /* Protected */,
      55,    0,  264,    2, 0x08 /* Private */,
      56,    0,  265,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 11, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,   12,   13,   14,   15,   16,   17,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,   29,   30,   31,
    QMetaType::Void, 0x80000000 | 33,   34,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   38,
    QMetaType::Void, 0x80000000 | 40,   41,
    QMetaType::Void, 0x80000000 | 40,   41,
    QMetaType::Void, 0x80000000 | 40, QMetaType::QString,   41,   31,
    QMetaType::Void, 0x80000000 | 40,   41,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   47,   48,   49,
    QMetaType::Void, QMetaType::QString,   50,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   54,
    QMetaType::Void,
    QMetaType::Void,

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
        case 2: _t->on_localTrackAdded(); break;
        case 3: _t->on_localTrackRemoved(); break;
        case 4: _t->on_preferencesClicked((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 5: _t->on_IOPreferencesChanged((*reinterpret_cast< QList<bool>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8]))); break;
        case 6: _t->on_ninjamCommunityMenuItemTriggered(); break;
        case 7: _t->on_ninjamOfficialSiteMenuItemTriggered(); break;
        case 8: _t->on_privateServerMenuItemTriggered(); break;
        case 9: _t->on_menuViewModeTriggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 10: _t->ninjamTransmissionStarted(); break;
        case 11: _t->ninjamPreparingToTransmit(); break;
        case 12: _t->on_reportBugMenuItemTriggered(); break;
        case 13: _t->on_wikiMenuItemTriggered(); break;
        case 14: _t->on_UsersManualMenuItemTriggered(); break;
        case 15: _t->on_privateServerConnectionAccepted((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 16: _t->on_roomsListAvailable((*reinterpret_cast< QList<Login::RoomInfo>(*)>(_a[1]))); break;
        case 17: _t->on_newVersionAvailableForDownload(); break;
        case 18: _t->on_incompatibilityWithServerDetected(); break;
        case 19: _t->on_errorConnectingToServer((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: _t->on_startingRoomStream((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 21: _t->on_stoppingRoomStream((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 22: _t->on_enteringInRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 23: _t->on_enteringInRoom((*reinterpret_cast< Login::RoomInfo(*)>(_a[1]))); break;
        case 24: _t->onScanPluginsStarted(); break;
        case 25: _t->onScanPluginsFinished(); break;
        case 26: _t->onPluginFounded((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 27: _t->onScanPluginsStarted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 28: _t->on_localControlsCollapseButtonClicked(); break;
        case 29: _t->on_channelNameChanged(); break;
        case 30: _t->on_RoomStreamerError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 31: _t->on_actionFullscreenMode_triggered(); break;
        case 32: _t->on_pluginFinderDialogCanceled(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<bool> >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
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
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
