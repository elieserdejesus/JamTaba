/****************************************************************************
** Meta object code from reading C++ file 'NinjamPanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/gui/NinjamPanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NinjamPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NinjamPanel_t {
    QByteArrayData data[14];
    char stringdata0[212];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NinjamPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NinjamPanel_t qt_meta_stringdata_NinjamPanel = {
    {
QT_MOC_LITERAL(0, 0, 11), // "NinjamPanel"
QT_MOC_LITERAL(1, 12, 17), // "bpiComboActivated"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 17), // "bpmComboActivated"
QT_MOC_LITERAL(4, 49, 19), // "accentsComboChanged"
QT_MOC_LITERAL(5, 69, 5), // "index"
QT_MOC_LITERAL(6, 75, 17), // "gainSliderChanged"
QT_MOC_LITERAL(7, 93, 5), // "value"
QT_MOC_LITERAL(8, 99, 16), // "panSliderChanged"
QT_MOC_LITERAL(9, 116, 17), // "muteButtonClicked"
QT_MOC_LITERAL(10, 134, 17), // "soloButtonClicked"
QT_MOC_LITERAL(11, 152, 21), // "hostSyncButtonClicked"
QT_MOC_LITERAL(12, 174, 19), // "comboAccentsChanged"
QT_MOC_LITERAL(13, 194, 17) // "comboShapeChanged"

    },
    "NinjamPanel\0bpiComboActivated\0\0"
    "bpmComboActivated\0accentsComboChanged\0"
    "index\0gainSliderChanged\0value\0"
    "panSliderChanged\0muteButtonClicked\0"
    "soloButtonClicked\0hostSyncButtonClicked\0"
    "comboAccentsChanged\0comboShapeChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NinjamPanel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       3,    1,   67,    2, 0x06 /* Public */,
       4,    1,   70,    2, 0x06 /* Public */,
       6,    1,   73,    2, 0x06 /* Public */,
       8,    1,   76,    2, 0x06 /* Public */,
       9,    0,   79,    2, 0x06 /* Public */,
      10,    0,   80,    2, 0x06 /* Public */,
      11,    0,   81,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   82,    2, 0x08 /* Private */,
      13,    1,   85,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void NinjamPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NinjamPanel *_t = static_cast<NinjamPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->bpiComboActivated((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->bpmComboActivated((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->accentsComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->gainSliderChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->panSliderChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->muteButtonClicked(); break;
        case 6: _t->soloButtonClicked(); break;
        case 7: _t->hostSyncButtonClicked(); break;
        case 8: _t->comboAccentsChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->comboShapeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NinjamPanel::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamPanel::bpiComboActivated)) {
                *result = 0;
            }
        }
        {
            typedef void (NinjamPanel::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamPanel::bpmComboActivated)) {
                *result = 1;
            }
        }
        {
            typedef void (NinjamPanel::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamPanel::accentsComboChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (NinjamPanel::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamPanel::gainSliderChanged)) {
                *result = 3;
            }
        }
        {
            typedef void (NinjamPanel::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamPanel::panSliderChanged)) {
                *result = 4;
            }
        }
        {
            typedef void (NinjamPanel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamPanel::muteButtonClicked)) {
                *result = 5;
            }
        }
        {
            typedef void (NinjamPanel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamPanel::soloButtonClicked)) {
                *result = 6;
            }
        }
        {
            typedef void (NinjamPanel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NinjamPanel::hostSyncButtonClicked)) {
                *result = 7;
            }
        }
    }
}

const QMetaObject NinjamPanel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NinjamPanel.data,
      qt_meta_data_NinjamPanel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NinjamPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NinjamPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NinjamPanel.stringdata0))
        return static_cast<void*>(const_cast< NinjamPanel*>(this));
    return QWidget::qt_metacast(_clname);
}

int NinjamPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void NinjamPanel::bpiComboActivated(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NinjamPanel::bpmComboActivated(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NinjamPanel::accentsComboChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NinjamPanel::gainSliderChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NinjamPanel::panSliderChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void NinjamPanel::muteButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void NinjamPanel::soloButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void NinjamPanel::hostSyncButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
