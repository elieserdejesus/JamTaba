/****************************************************************************
** Meta object code from reading C++ file 'PreferencesDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/gui/PreferencesDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PreferencesDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PreferencesDialog_t {
    QByteArrayData data[21];
    char stringdata[399];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PreferencesDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PreferencesDialog_t qt_meta_stringdata_PreferencesDialog = {
    {
QT_MOC_LITERAL(0, 0, 17), // "PreferencesDialog"
QT_MOC_LITERAL(1, 18, 9), // "ioChanged"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 18), // "selectedMidiDevice"
QT_MOC_LITERAL(4, 48, 19), // "selectedAudioDevice"
QT_MOC_LITERAL(5, 68, 7), // "firstIn"
QT_MOC_LITERAL(6, 76, 6), // "lastIn"
QT_MOC_LITERAL(7, 83, 8), // "firstOut"
QT_MOC_LITERAL(8, 92, 7), // "lastOut"
QT_MOC_LITERAL(9, 100, 10), // "sampleRate"
QT_MOC_LITERAL(10, 111, 10), // "bufferSize"
QT_MOC_LITERAL(11, 122, 38), // "on_comboFirstInput_currentInd..."
QT_MOC_LITERAL(12, 161, 39), // "on_comboFirstOutput_currentIn..."
QT_MOC_LITERAL(13, 201, 28), // "on_comboAsioDriver_activated"
QT_MOC_LITERAL(14, 230, 5), // "index"
QT_MOC_LITERAL(15, 236, 20), // "on_okButton_released"
QT_MOC_LITERAL(16, 257, 26), // "on_prefsTab_currentChanged"
QT_MOC_LITERAL(17, 284, 27), // "on_buttonAddVstPath_clicked"
QT_MOC_LITERAL(18, 312, 29), // "on_buttonRemoveVstPathClicked"
QT_MOC_LITERAL(19, 342, 30), // "on_buttonClearVstCache_clicked"
QT_MOC_LITERAL(20, 373, 25) // "on_buttonScanVSTs_clicked"

    },
    "PreferencesDialog\0ioChanged\0\0"
    "selectedMidiDevice\0selectedAudioDevice\0"
    "firstIn\0lastIn\0firstOut\0lastOut\0"
    "sampleRate\0bufferSize\0"
    "on_comboFirstInput_currentIndexChanged\0"
    "on_comboFirstOutput_currentIndexChanged\0"
    "on_comboAsioDriver_activated\0index\0"
    "on_okButton_released\0on_prefsTab_currentChanged\0"
    "on_buttonAddVstPath_clicked\0"
    "on_buttonRemoveVstPathClicked\0"
    "on_buttonClearVstCache_clicked\0"
    "on_buttonScanVSTs_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PreferencesDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    8,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   81,    2, 0x08 /* Private */,
      12,    1,   84,    2, 0x08 /* Private */,
      13,    1,   87,    2, 0x08 /* Private */,
      15,    0,   90,    2, 0x08 /* Private */,
      16,    1,   91,    2, 0x08 /* Private */,
      17,    0,   94,    2, 0x08 /* Private */,
      18,    0,   95,    2, 0x08 /* Private */,
      19,    0,   96,    2, 0x08 /* Private */,
      20,    0,   97,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,    6,    7,    8,    9,   10,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   14,
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
        switch (_id) {
        case 0: _t->ioChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8]))); break;
        case 1: _t->on_comboFirstInput_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_comboFirstOutput_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_comboAsioDriver_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_okButton_released(); break;
        case 5: _t->on_prefsTab_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_buttonAddVstPath_clicked(); break;
        case 7: _t->on_buttonRemoveVstPathClicked(); break;
        case 8: _t->on_buttonClearVstCache_clicked(); break;
        case 9: _t->on_buttonScanVSTs_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PreferencesDialog::*_t)(int , int , int , int , int , int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PreferencesDialog::ioChanged)) {
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
    if (!strcmp(_clname, qt_meta_stringdata_PreferencesDialog.stringdata))
        return static_cast<void*>(const_cast< PreferencesDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PreferencesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void PreferencesDialog::ioChanged(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7, int _t8)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
