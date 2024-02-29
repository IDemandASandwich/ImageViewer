/********************************************************************************
** Form generated from reading UI file 'ImageViewer.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEVIEWER_H
#define UI_IMAGEVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImageViewerClass
{
public:
    QAction *actionOpen;
    QAction *actionSave_as;
    QAction *actionClear;
    QAction *actionExit;
    QAction *actionResize;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuImage;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButtonSetColor;
    QPushButton *pushButtonClear;
    QGroupBox *groupBoxDraw;
    QGridLayout *gridLayout;
    QComboBox *comboBoxLineAlg;
    QPushButton *pushButtonScale;
    QPushButton *pushButtonRotate;
    QToolButton *toolButtonDrawCircle;
    QToolButton *toolButtonDrawLine;
    QToolButton *toolButtonDrawPolygon;
    QSpinBox *spinBoxRotate;
    QDoubleSpinBox *doubleSpinBoxScale;
    QSpacerItem *verticalSpacer;

    void setupUi(QMainWindow *ImageViewerClass)
    {
        if (ImageViewerClass->objectName().isEmpty())
            ImageViewerClass->setObjectName("ImageViewerClass");
        ImageViewerClass->resize(671, 600);
        actionOpen = new QAction(ImageViewerClass);
        actionOpen->setObjectName("actionOpen");
        actionSave_as = new QAction(ImageViewerClass);
        actionSave_as->setObjectName("actionSave_as");
        actionClear = new QAction(ImageViewerClass);
        actionClear->setObjectName("actionClear");
        actionExit = new QAction(ImageViewerClass);
        actionExit->setObjectName("actionExit");
        actionResize = new QAction(ImageViewerClass);
        actionResize->setObjectName("actionResize");
        centralWidget = new QWidget(ImageViewerClass);
        centralWidget->setObjectName("centralWidget");
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        scrollArea = new QScrollArea(centralWidget);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 423, 525));
        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(scrollArea);

        ImageViewerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ImageViewerClass);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 671, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName("menuFile");
        menuImage = new QMenu(menuBar);
        menuImage->setObjectName("menuImage");
        ImageViewerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ImageViewerClass);
        mainToolBar->setObjectName("mainToolBar");
        ImageViewerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ImageViewerClass);
        statusBar->setObjectName("statusBar");
        ImageViewerClass->setStatusBar(statusBar);
        dockWidget = new QDockWidget(ImageViewerClass);
        dockWidget->setObjectName("dockWidget");
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName("dockWidgetContents");
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName("groupBox");
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName("verticalLayout_2");
        pushButtonSetColor = new QPushButton(groupBox);
        pushButtonSetColor->setObjectName("pushButtonSetColor");

        verticalLayout_2->addWidget(pushButtonSetColor);


        verticalLayout->addWidget(groupBox);

        pushButtonClear = new QPushButton(dockWidgetContents);
        pushButtonClear->setObjectName("pushButtonClear");

        verticalLayout->addWidget(pushButtonClear);

        groupBoxDraw = new QGroupBox(dockWidgetContents);
        groupBoxDraw->setObjectName("groupBoxDraw");
        gridLayout = new QGridLayout(groupBoxDraw);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName("gridLayout");
        comboBoxLineAlg = new QComboBox(groupBoxDraw);
        comboBoxLineAlg->addItem(QString());
        comboBoxLineAlg->addItem(QString());
        comboBoxLineAlg->setObjectName("comboBoxLineAlg");

        gridLayout->addWidget(comboBoxLineAlg, 0, 0, 1, 3);

        pushButtonScale = new QPushButton(groupBoxDraw);
        pushButtonScale->setObjectName("pushButtonScale");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButtonScale->sizePolicy().hasHeightForWidth());
        pushButtonScale->setSizePolicy(sizePolicy);

        gridLayout->addWidget(pushButtonScale, 3, 1, 1, 1);

        pushButtonRotate = new QPushButton(groupBoxDraw);
        pushButtonRotate->setObjectName("pushButtonRotate");
        QSizePolicy sizePolicy1(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButtonRotate->sizePolicy().hasHeightForWidth());
        pushButtonRotate->setSizePolicy(sizePolicy1);
        pushButtonRotate->setMaximumSize(QSize(100, 16777215));
        pushButtonRotate->setFlat(false);

        gridLayout->addWidget(pushButtonRotate, 2, 1, 1, 1);

        toolButtonDrawCircle = new QToolButton(groupBoxDraw);
        toolButtonDrawCircle->setObjectName("toolButtonDrawCircle");
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(toolButtonDrawCircle->sizePolicy().hasHeightForWidth());
        toolButtonDrawCircle->setSizePolicy(sizePolicy2);
        toolButtonDrawCircle->setCheckable(true);

        gridLayout->addWidget(toolButtonDrawCircle, 1, 1, 1, 1);

        toolButtonDrawLine = new QToolButton(groupBoxDraw);
        toolButtonDrawLine->setObjectName("toolButtonDrawLine");
        sizePolicy2.setHeightForWidth(toolButtonDrawLine->sizePolicy().hasHeightForWidth());
        toolButtonDrawLine->setSizePolicy(sizePolicy2);
        toolButtonDrawLine->setCheckable(true);
        toolButtonDrawLine->setAutoRepeatDelay(0);

        gridLayout->addWidget(toolButtonDrawLine, 1, 0, 1, 1);

        toolButtonDrawPolygon = new QToolButton(groupBoxDraw);
        toolButtonDrawPolygon->setObjectName("toolButtonDrawPolygon");
        toolButtonDrawPolygon->setCheckable(true);

        gridLayout->addWidget(toolButtonDrawPolygon, 1, 2, 1, 1);

        spinBoxRotate = new QSpinBox(groupBoxDraw);
        spinBoxRotate->setObjectName("spinBoxRotate");
        spinBoxRotate->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBoxRotate->setMinimum(-360);
        spinBoxRotate->setMaximum(360);

        gridLayout->addWidget(spinBoxRotate, 2, 0, 1, 1);

        doubleSpinBoxScale = new QDoubleSpinBox(groupBoxDraw);
        doubleSpinBoxScale->setObjectName("doubleSpinBoxScale");
        doubleSpinBoxScale->setMinimum(0.500000000000000);
        doubleSpinBoxScale->setMaximum(2.000000000000000);
        doubleSpinBoxScale->setSingleStep(0.250000000000000);

        gridLayout->addWidget(doubleSpinBoxScale, 3, 0, 1, 1);


        verticalLayout->addWidget(groupBoxDraw);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        dockWidget->setWidget(dockWidgetContents);
        ImageViewerClass->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuImage->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave_as);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuImage->addAction(actionClear);

        retranslateUi(ImageViewerClass);

        pushButtonRotate->setDefault(false);


        QMetaObject::connectSlotsByName(ImageViewerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ImageViewerClass)
    {
        ImageViewerClass->setWindowTitle(QCoreApplication::translate("ImageViewerClass", "ImageViewer", nullptr));
        actionOpen->setText(QCoreApplication::translate("ImageViewerClass", "Open", nullptr));
#if QT_CONFIG(shortcut)
        actionOpen->setShortcut(QCoreApplication::translate("ImageViewerClass", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_as->setText(QCoreApplication::translate("ImageViewerClass", "Save as", nullptr));
#if QT_CONFIG(shortcut)
        actionSave_as->setShortcut(QCoreApplication::translate("ImageViewerClass", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionClear->setText(QCoreApplication::translate("ImageViewerClass", "Clear", nullptr));
        actionExit->setText(QCoreApplication::translate("ImageViewerClass", "Exit", nullptr));
#if QT_CONFIG(shortcut)
        actionExit->setShortcut(QCoreApplication::translate("ImageViewerClass", "Alt+F4", nullptr));
#endif // QT_CONFIG(shortcut)
        actionResize->setText(QCoreApplication::translate("ImageViewerClass", "Resize", nullptr));
        menuFile->setTitle(QCoreApplication::translate("ImageViewerClass", "File", nullptr));
        menuImage->setTitle(QCoreApplication::translate("ImageViewerClass", "Image", nullptr));
        dockWidget->setWindowTitle(QCoreApplication::translate("ImageViewerClass", "Tools", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ImageViewerClass", "Color", nullptr));
        pushButtonSetColor->setText(QString());
        pushButtonClear->setText(QCoreApplication::translate("ImageViewerClass", "Clear", nullptr));
        groupBoxDraw->setTitle(QCoreApplication::translate("ImageViewerClass", "Draw", nullptr));
        comboBoxLineAlg->setItemText(0, QCoreApplication::translate("ImageViewerClass", "DDA", nullptr));
        comboBoxLineAlg->setItemText(1, QCoreApplication::translate("ImageViewerClass", "Bresenham", nullptr));

        pushButtonScale->setText(QCoreApplication::translate("ImageViewerClass", "Scale", nullptr));
        pushButtonRotate->setText(QCoreApplication::translate("ImageViewerClass", "Rotate", nullptr));
        toolButtonDrawCircle->setText(QCoreApplication::translate("ImageViewerClass", "Circle", nullptr));
        toolButtonDrawLine->setText(QCoreApplication::translate("ImageViewerClass", "Line", nullptr));
        toolButtonDrawPolygon->setText(QCoreApplication::translate("ImageViewerClass", "Polygon", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ImageViewerClass: public Ui_ImageViewerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEVIEWER_H
