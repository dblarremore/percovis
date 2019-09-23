/****************************************************************************
** Meta object code from reading C++ file 'lorenz.h'
**
** Created: Thu Jul 14 17:02:44 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "lorenz.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lorenz.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Lorenz[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,    8,    7,    7, 0x05,
      33,   29,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,   47,    7,    7, 0x0a,
      64,   62,    7,    7, 0x0a,
      79,   77,    7,    7, 0x0a,
      95,   92,    7,    7, 0x0a,
     112,  109,    7,    7, 0x0a,
     129,  126,    7,    7, 0x0a,
     146,  143,    7,    7, 0x0a,
     164,  160,    7,    7, 0x0a,
     179,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Lorenz[] = {
    "Lorenz\0\0text\0angles(QString)\0dim\0"
    "dimen(double)\0S\0setS(double)\0R\0"
    "setR(double)\0B\0setB(double)\0X0\0"
    "setX0(double)\0Y0\0setY0(double)\0Z0\0"
    "setZ0(double)\0DT\0setDT(double)\0DIM\0"
    "setDIM(double)\0reset()\0"
};

const QMetaObject Lorenz::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_Lorenz,
      qt_meta_data_Lorenz, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Lorenz::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Lorenz::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Lorenz::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Lorenz))
        return static_cast<void*>(const_cast< Lorenz*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int Lorenz::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: angles((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: dimen((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: setS((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: setR((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: setB((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: setX0((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: setY0((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: setZ0((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: setDT((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: setDIM((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: reset(); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Lorenz::angles(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Lorenz::dimen(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
