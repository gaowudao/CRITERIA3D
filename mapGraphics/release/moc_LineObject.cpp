/****************************************************************************
** Meta object code from reading C++ file 'LineObject.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../LineObject.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LineObject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LineObject_t {
    QByteArrayData data[10];
    char stringdata0[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LineObject_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LineObject_t qt_meta_stringdata_LineObject = {
    {
QT_MOC_LITERAL(0, 0, 10), // "LineObject"
QT_MOC_LITERAL(1, 11, 12), // "setEndPointA"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 8), // "Position"
QT_MOC_LITERAL(4, 34, 3), // "pos"
QT_MOC_LITERAL(5, 38, 12), // "setEndPointB"
QT_MOC_LITERAL(6, 51, 12), // "setEndPoints"
QT_MOC_LITERAL(7, 64, 1), // "a"
QT_MOC_LITERAL(8, 66, 1), // "b"
QT_MOC_LITERAL(9, 68, 27) // "updatePositionFromEndPoints"

    },
    "LineObject\0setEndPointA\0\0Position\0pos\0"
    "setEndPointB\0setEndPoints\0a\0b\0"
    "updatePositionFromEndPoints"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LineObject[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       5,    1,   37,    2, 0x0a /* Public */,
       6,    2,   40,    2, 0x0a /* Public */,
       9,    0,   45,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    7,    8,
    QMetaType::Void,

       0        // eod
};

void LineObject::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LineObject *_t = static_cast<LineObject *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setEndPointA((*reinterpret_cast< const Position(*)>(_a[1]))); break;
        case 1: _t->setEndPointB((*reinterpret_cast< const Position(*)>(_a[1]))); break;
        case 2: _t->setEndPoints((*reinterpret_cast< const Position(*)>(_a[1])),(*reinterpret_cast< const Position(*)>(_a[2]))); break;
        case 3: _t->updatePositionFromEndPoints(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LineObject::staticMetaObject = { {
    &MapGraphicsObject::staticMetaObject,
    qt_meta_stringdata_LineObject.data,
    qt_meta_data_LineObject,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LineObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LineObject::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LineObject.stringdata0))
        return static_cast<void*>(this);
    return MapGraphicsObject::qt_metacast(_clname);
}

int LineObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MapGraphicsObject::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
