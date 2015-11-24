/****************************************************************************
** Meta object code from reading C++ file 'ChordsPanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Client/src/gui/chords/ChordsPanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChordsPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ChordsPanel_t {
    QByteArrayData data[6];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ChordsPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ChordsPanel_t qt_meta_stringdata_ChordsPanel = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ChordsPanel"
QT_MOC_LITERAL(1, 12, 29), // "buttonSendChordsToChatClicked"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 28), // "on_buttonTransposeUp_clicked"
QT_MOC_LITERAL(4, 72, 30), // "on_buttonTransposeDown_clicked"
QT_MOC_LITERAL(5, 103, 29) // "on_buttonDiscardChordsClicked"

    },
    "ChordsPanel\0buttonSendChordsToChatClicked\0"
    "\0on_buttonTransposeUp_clicked\0"
    "on_buttonTransposeDown_clicked\0"
    "on_buttonDiscardChordsClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ChordsPanel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   35,    2, 0x08 /* Private */,
       4,    0,   36,    2, 0x08 /* Private */,
       5,    0,   37,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ChordsPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ChordsPanel *_t = static_cast<ChordsPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->buttonSendChordsToChatClicked(); break;
        case 1: _t->on_buttonTransposeUp_clicked(); break;
        case 2: _t->on_buttonTransposeDown_clicked(); break;
        case 3: _t->on_buttonDiscardChordsClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ChordsPanel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ChordsPanel::buttonSendChordsToChatClicked)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ChordsPanel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ChordsPanel.data,
      qt_meta_data_ChordsPanel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ChordsPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChordsPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ChordsPanel.stringdata0))
        return static_cast<void*>(const_cast< ChordsPanel*>(this));
    return QWidget::qt_metacast(_clname);
}

int ChordsPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ChordsPanel::buttonSendChordsToChatClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
