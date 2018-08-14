/****************************************************************************
** Meta object code from reading C++ file 'tester.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../hw/tester.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tester.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Tester_t {
    QByteArrayData data[8];
    char stringdata0[41];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Tester_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Tester_t qt_meta_stringdata_Tester = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Tester"
QT_MOC_LITERAL(1, 7, 4), // "Open"
QT_MOC_LITERAL(2, 12, 0), // ""
QT_MOC_LITERAL(3, 13, 4), // "mode"
QT_MOC_LITERAL(4, 18, 5), // "Close"
QT_MOC_LITERAL(5, 24, 5), // "Write"
QT_MOC_LITERAL(6, 30, 4), // "data"
QT_MOC_LITERAL(7, 35, 5) // "Ready"

    },
    "Tester\0Open\0\0mode\0Close\0Write\0data\0"
    "Ready"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Tester[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    0,   37,    2, 0x06 /* Public */,
       5,    1,   38,    2, 0x06 /* Public */,
       7,    0,   41,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    6,
    QMetaType::Void,

       0        // eod
};

void Tester::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Tester *_t = static_cast<Tester *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Open((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->Close(); break;
        case 2: _t->Write((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 3: _t->Ready(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (Tester::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Tester::Open)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Tester::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Tester::Close)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Tester::*_t)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Tester::Write)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Tester::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Tester::Ready)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject Tester::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Tester.data,
      qt_meta_data_Tester,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Tester::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Tester::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Tester.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "CommonInterfaces"))
        return static_cast< CommonInterfaces*>(this);
    if (!strcmp(_clname, "CallBack"))
        return static_cast< CallBack*>(this);
    return QObject::qt_metacast(_clname);
}

int Tester::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void Tester::Open(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Tester::Close()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Tester::Write(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Tester::Ready()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
struct qt_meta_stringdata_TesterPort_t {
    QByteArrayData data[1];
    char stringdata0[11];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TesterPort_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TesterPort_t qt_meta_stringdata_TesterPort = {
    {
QT_MOC_LITERAL(0, 0, 10) // "TesterPort"

    },
    "TesterPort"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TesterPort[] = {

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

void TesterPort::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject TesterPort::staticMetaObject = {
    { &QSerialPort::staticMetaObject, qt_meta_stringdata_TesterPort.data,
      qt_meta_data_TesterPort,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *TesterPort::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TesterPort::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TesterPort.stringdata0))
        return static_cast<void*>(this);
    return QSerialPort::qt_metacast(_clname);
}

int TesterPort::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSerialPort::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
