/****************************************************************************
** Meta object code from reading C++ file 'PreferencesDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/gui/PreferencesDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PreferencesDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PreferencesDialog_t {
    QByteArrayData data[24];
    char stringdata0[481];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PreferencesDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PreferencesDialog_t qt_meta_stringdata_PreferencesDialog = {
    {
QT_MOC_LITERAL(0, 0, 17), // "PreferencesDialog"
QT_MOC_LITERAL(1, 18, 20), // "ioPreferencesChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 11), // "QList<bool>"
QT_MOC_LITERAL(4, 52, 16), // "midiInputsStatus"
QT_MOC_LITERAL(5, 69, 19), // "selectedAudioDevice"
QT_MOC_LITERAL(6, 89, 7), // "firstIn"
QT_MOC_LITERAL(7, 97, 6), // "lastIn"
QT_MOC_LITERAL(8, 104, 8), // "firstOut"
QT_MOC_LITERAL(9, 113, 7), // "lastOut"
QT_MOC_LITERAL(10, 121, 10), // "sampleRate"
QT_MOC_LITERAL(11, 132, 10), // "bufferSize"
QT_MOC_LITERAL(12, 143, 38), // "on_comboFirstInput_currentInd..."
QT_MOC_LITERAL(13, 182, 39), // "on_comboFirstOutput_currentIn..."
QT_MOC_LITERAL(14, 222, 29), // "on_comboAudioDevice_activated"
QT_MOC_LITERAL(15, 252, 5), // "index"
QT_MOC_LITERAL(16, 258, 20), // "on_okButton_released"
QT_MOC_LITERAL(17, 279, 26), // "on_prefsTab_currentChanged"
QT_MOC_LITERAL(18, 306, 27), // "on_buttonAddVstPath_clicked"
QT_MOC_LITERAL(19, 334, 29), // "on_buttonRemoveVstPathClicked"
QT_MOC_LITERAL(20, 364, 30), // "on_buttonClearVstCache_clicked"
QT_MOC_LITERAL(21, 395, 25), // "on_buttonScanVSTs_clicked"
QT_MOC_LITERAL(22, 421, 30), // "on_browseRecPathButton_clicked"
QT_MOC_LITERAL(23, 452, 28) // "on_recordingCheckBox_clicked"

    },
    "PreferencesDialog\0ioPreferencesChanged\0"
    "\0QList<bool>\0midiInputsStatus\0"
    "selectedAudioDevice\0firstIn\0lastIn\0"
    "firstOut\0lastOut\0sampleRate\0bufferSize\0"
    "on_comboFirstInput_currentIndexChanged\0"
    "on_comboFirstOutput_currentIndexChanged\0"
    "on_comboAudioDevice_activated\0index\0"
    "on_okButton_released\0on_prefsTab_currentChanged\0"
    "on_buttonAddVstPath_clicked\0"
    "on_buttonRemoveVstPathClicked\0"
    "on_buttonClearVstCache_clicked\0"
    "on_buttonScanVSTs_clicked\0"
    "on_browseRecPathButton_clicked\0"
    "on_recordingCheckBox_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PreferencesDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    8,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   91,    2, 0x08 /* Private */,
      13,    1,   94,    2, 0x08 /* Private */,
      14,    1,   97,    2, 0x08 /* Private */,
      16,    0,  100,    2, 0x08 /* Private */,
      17,    1,  101,    2, 0x08 /* Private */,
      18,    0,  104,    2, 0x08 /* Private */,
      19,    0,  105,    2, 0x08 /* Private */,
      20,    0,  106,    2, 0x08 /* Private */,
      21,    0,  107,    2, 0x08 /* Private */,
      22,    0,  108,    2, 0x08 /* Private */,
      23,    0,  109,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    4,    5,    6,    7,    8,    9,   10,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PreferencesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PreferencesDialog *_t = static_cast<PreferencesDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ioPreferencesChanged((*reinterpret_cast< QList<bool>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8]))); break;
        case 1: _t->on_comboFirstInput_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_comboFirstOutput_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_comboAudioDevice_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_okButton_released(); break;
        case 5: _t->on_prefsTab_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_buttonAddVstPath_clicked(); break;
        case 7: _t->on_buttonRemoveVstPathClicked(); break;
        case 8: _t->on_buttonClearVstCache_clicked(); break;
        case 9: _t->on_buttonScanVSTs_clicked(); break;
        case 10: _t->on_browseRecPathButton_clicked(); break;
        case 11: _t->on_recordingCheckBox_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<bool> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PreferencesDialog::*_t)(QList<bool> , int , int , int , int , int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PreferencesDialog::ioPreferencesChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject PreferencesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PreferencesDialog.data,
      qt_meta_data_PreferencesDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PreferencesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PreferencesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PreferencesDialog.stringdata0))
        return static_cast<void*>(const_cast< PreferencesDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PreferencesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void PreferencesDialog::ioPreferencesChanged(QList<bool> _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7, int _t8)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
