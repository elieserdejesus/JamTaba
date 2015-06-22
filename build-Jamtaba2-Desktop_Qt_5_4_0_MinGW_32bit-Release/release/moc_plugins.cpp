/****************************************************************************
** Meta object code from reading C++ file 'plugins.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Client/src/jamtaba/audio/core/plugins.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plugins.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Audio__PluginDescriptor_t {
    QByteArrayData data[1];
    char stringdata[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Audio__PluginDescriptor_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Audio__PluginDescriptor_t qt_meta_stringdata_Audio__PluginDescriptor = {
    {
QT_MOC_LITERAL(0, 0, 23) // "Audio::PluginDescriptor"

    },
    "Audio::PluginDescriptor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Audio__PluginDescriptor[] = {

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

void Audio::PluginDescriptor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Audio::PluginDescriptor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Audio__PluginDescriptor.data,
      qt_meta_data_Audio__PluginDescriptor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Audio::PluginDescriptor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Audio::PluginDescriptor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Audio__PluginDescriptor.stringdata))
        return static_cast<void*>(const_cast< PluginDescriptor*>(this));
    return QObject::qt_metacast(_clname);
}

int Audio::PluginDescriptor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Audio__PluginWindow_t {
    QByteArrayData data[1];
    char stringdata[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Audio__PluginWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Audio__PluginWindow_t qt_meta_stringdata_Audio__PluginWindow = {
    {
QT_MOC_LITERAL(0, 0, 19) // "Audio::PluginWindow"

    },
    "Audio::PluginWindow"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Audio__PluginWindow[] = {

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

void Audio::PluginWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Audio::PluginWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Audio__PluginWindow.data,
      qt_meta_data_Audio__PluginWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Audio::PluginWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Audio::PluginWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Audio__PluginWindow.stringdata))
        return static_cast<void*>(const_cast< PluginWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int Audio::PluginWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
