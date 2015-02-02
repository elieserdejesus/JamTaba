/****************************************************************************
** Meta object code from reading C++ file 'audioiodialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/jamtaba/gui/audioiodialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audioiodialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Gui__AudioIODialog_t {
    QByteArrayData data[15];
    char stringdata[249];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Gui__AudioIODialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Gui__AudioIODialog_t qt_meta_stringdata_Gui__AudioIODialog = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Gui::AudioIODialog"
QT_MOC_LITERAL(1, 19, 24), // "audioIOPropertiesChanged"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 14), // "selectedDevice"
QT_MOC_LITERAL(4, 60, 7), // "firstIn"
QT_MOC_LITERAL(5, 68, 6), // "lastIn"
QT_MOC_LITERAL(6, 75, 8), // "firstOut"
QT_MOC_LITERAL(7, 84, 7), // "lastOut"
QT_MOC_LITERAL(8, 92, 10), // "sampleRate"
QT_MOC_LITERAL(9, 103, 10), // "bufferSize"
QT_MOC_LITERAL(10, 114, 38), // "on_comboFirstInput_currentInd..."
QT_MOC_LITERAL(11, 153, 39), // "on_comboFirstOutput_currentIn..."
QT_MOC_LITERAL(12, 193, 28), // "on_comboAsioDriver_activated"
QT_MOC_LITERAL(13, 222, 5), // "index"
QT_MOC_LITERAL(14, 228, 20) // "on_okButton_released"

    },
    "Gui::AudioIODialog\0audioIOPropertiesChanged\0"
    "\0selectedDevice\0firstIn\0lastIn\0firstOut\0"
    "lastOut\0sampleRate\0bufferSize\0"
    "on_comboFirstInput_currentIndexChanged\0"
    "on_comboFirstOutput_currentIndexChanged\0"
    "on_comboAsioDriver_activated\0index\0"
    "on_okButton_released"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Gui__AudioIODialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    7,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   54,    2, 0x08 /* Private */,
      11,    1,   57,    2, 0x08 /* Private */,
      12,    1,   60,    2, 0x08 /* Private */,
      14,    0,   63,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,    6,    7,    8,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void,

       0        // eod
};

void Gui::AudioIODialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioIODialog *_t = static_cast<AudioIODialog *>(_o);
        switch (_id) {
        case 0: _t->audioIOPropertiesChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 1: _t->on_comboFirstInput_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_comboFirstOutput_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_comboAsioDriver_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_okButton_released(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AudioIODialog::*_t)(int , int , int , int , int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AudioIODialog::audioIOPropertiesChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Gui::AudioIODialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Gui__AudioIODialog.data,
      qt_meta_data_Gui__AudioIODialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Gui::AudioIODialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Gui::AudioIODialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Gui__AudioIODialog.stringdata))
        return static_cast<void*>(const_cast< AudioIODialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Gui::AudioIODialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
void Gui::AudioIODialog::audioIOPropertiesChanged(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
