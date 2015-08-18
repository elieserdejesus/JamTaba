/****************************************************************************
** Meta object code from reading C++ file 'guis.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/jamtaba/gui/plugins/guis.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guis.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PluginGui_t {
    QByteArrayData data[1];
    char stringdata0[10];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PluginGui_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PluginGui_t qt_meta_stringdata_PluginGui = {
    {
QT_MOC_LITERAL(0, 0, 9) // "PluginGui"

    },
    "PluginGui"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PluginGui[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void PluginGui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject PluginGui::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PluginGui.data,
      qt_meta_data_PluginGui,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PluginGui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PluginGui::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PluginGui.stringdata0))
        return static_cast<void*>(const_cast< PluginGui*>(this));
    return QWidget::qt_metacast(_clname);
}

int PluginGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_DelayGui_t {
    QByteArrayData data[5];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DelayGui_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DelayGui_t qt_meta_stringdata_DelayGui = {
    {
QT_MOC_LITERAL(0, 0, 8), // "DelayGui"
QT_MOC_LITERAL(1, 9, 22), // "on_sliderDelayReleased"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 25), // "on_sliderFeedbackReleased"
QT_MOC_LITERAL(4, 59, 22) // "on_sliderLevelReleased"

    },
    "DelayGui\0on_sliderDelayReleased\0\0"
    "on_sliderFeedbackReleased\0"
    "on_sliderLevelReleased"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DelayGui[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x08 /* Private */,
       3,    0,   30,    2, 0x08 /* Private */,
       4,    0,   31,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DelayGui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DelayGui *_t = static_cast<DelayGui *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_sliderDelayReleased(); break;
        case 1: _t->on_sliderFeedbackReleased(); break;
        case 2: _t->on_sliderLevelReleased(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject DelayGui::staticMetaObject = {
    { &PluginGui::staticMetaObject, qt_meta_stringdata_DelayGui.data,
      qt_meta_data_DelayGui,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DelayGui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DelayGui::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DelayGui.stringdata0))
        return static_cast<void*>(const_cast< DelayGui*>(this));
    return PluginGui::qt_metacast(_clname);
}

int DelayGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PluginGui::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
