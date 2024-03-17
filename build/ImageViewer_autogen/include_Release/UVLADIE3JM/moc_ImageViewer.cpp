/****************************************************************************
** Meta object code from reading C++ file 'ImageViewer.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/ImageViewer.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSImageViewerENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSImageViewerENDCLASS = QtMocHelpers::stringData(
    "ImageViewer",
    "on_actionOpen_triggered",
    "",
    "on_actionSave_as_triggered",
    "on_actionClear_triggered",
    "on_actionExit_triggered",
    "on_pushButtonSetColor_clicked",
    "on_pushButtonSetColorA_clicked",
    "on_pushButtonSetColorB_clicked",
    "on_pushButtonSetColorC_clicked",
    "on_pushButtonClear_clicked",
    "on_pushButtonRotate_clicked",
    "on_pushButtonScale_clicked",
    "on_pushButtonMirror_clicked",
    "on_pushButtonShear_clicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSImageViewerENDCLASS_t {
    uint offsetsAndSizes[30];
    char stringdata0[12];
    char stringdata1[24];
    char stringdata2[1];
    char stringdata3[27];
    char stringdata4[25];
    char stringdata5[24];
    char stringdata6[30];
    char stringdata7[31];
    char stringdata8[31];
    char stringdata9[31];
    char stringdata10[27];
    char stringdata11[28];
    char stringdata12[27];
    char stringdata13[28];
    char stringdata14[27];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSImageViewerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSImageViewerENDCLASS_t qt_meta_stringdata_CLASSImageViewerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 11),  // "ImageViewer"
        QT_MOC_LITERAL(12, 23),  // "on_actionOpen_triggered"
        QT_MOC_LITERAL(36, 0),  // ""
        QT_MOC_LITERAL(37, 26),  // "on_actionSave_as_triggered"
        QT_MOC_LITERAL(64, 24),  // "on_actionClear_triggered"
        QT_MOC_LITERAL(89, 23),  // "on_actionExit_triggered"
        QT_MOC_LITERAL(113, 29),  // "on_pushButtonSetColor_clicked"
        QT_MOC_LITERAL(143, 30),  // "on_pushButtonSetColorA_clicked"
        QT_MOC_LITERAL(174, 30),  // "on_pushButtonSetColorB_clicked"
        QT_MOC_LITERAL(205, 30),  // "on_pushButtonSetColorC_clicked"
        QT_MOC_LITERAL(236, 26),  // "on_pushButtonClear_clicked"
        QT_MOC_LITERAL(263, 27),  // "on_pushButtonRotate_clicked"
        QT_MOC_LITERAL(291, 26),  // "on_pushButtonScale_clicked"
        QT_MOC_LITERAL(318, 27),  // "on_pushButtonMirror_clicked"
        QT_MOC_LITERAL(346, 26)   // "on_pushButtonShear_clicked"
    },
    "ImageViewer",
    "on_actionOpen_triggered",
    "",
    "on_actionSave_as_triggered",
    "on_actionClear_triggered",
    "on_actionExit_triggered",
    "on_pushButtonSetColor_clicked",
    "on_pushButtonSetColorA_clicked",
    "on_pushButtonSetColorB_clicked",
    "on_pushButtonSetColorC_clicked",
    "on_pushButtonClear_clicked",
    "on_pushButtonRotate_clicked",
    "on_pushButtonScale_clicked",
    "on_pushButtonMirror_clicked",
    "on_pushButtonShear_clicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSImageViewerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x08,    1 /* Private */,
       3,    0,   93,    2, 0x08,    2 /* Private */,
       4,    0,   94,    2, 0x08,    3 /* Private */,
       5,    0,   95,    2, 0x08,    4 /* Private */,
       6,    0,   96,    2, 0x08,    5 /* Private */,
       7,    0,   97,    2, 0x08,    6 /* Private */,
       8,    0,   98,    2, 0x08,    7 /* Private */,
       9,    0,   99,    2, 0x08,    8 /* Private */,
      10,    0,  100,    2, 0x08,    9 /* Private */,
      11,    0,  101,    2, 0x08,   10 /* Private */,
      12,    0,  102,    2, 0x08,   11 /* Private */,
      13,    0,  103,    2, 0x08,   12 /* Private */,
      14,    0,  104,    2, 0x08,   13 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ImageViewer::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSImageViewerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSImageViewerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSImageViewerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ImageViewer, std::true_type>,
        // method 'on_actionOpen_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSave_as_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionClear_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionExit_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonSetColor_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonSetColorA_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonSetColorB_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonSetColorC_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonClear_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonRotate_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonScale_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonMirror_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonShear_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ImageViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ImageViewer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_actionOpen_triggered(); break;
        case 1: _t->on_actionSave_as_triggered(); break;
        case 2: _t->on_actionClear_triggered(); break;
        case 3: _t->on_actionExit_triggered(); break;
        case 4: _t->on_pushButtonSetColor_clicked(); break;
        case 5: _t->on_pushButtonSetColorA_clicked(); break;
        case 6: _t->on_pushButtonSetColorB_clicked(); break;
        case 7: _t->on_pushButtonSetColorC_clicked(); break;
        case 8: _t->on_pushButtonClear_clicked(); break;
        case 9: _t->on_pushButtonRotate_clicked(); break;
        case 10: _t->on_pushButtonScale_clicked(); break;
        case 11: _t->on_pushButtonMirror_clicked(); break;
        case 12: _t->on_pushButtonShear_clicked(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *ImageViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImageViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSImageViewerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ImageViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
