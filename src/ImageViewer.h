#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_ImageViewer.h"
#include "ViewerWidget.h"

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);

protected:
	void dropEvent(QDropEvent* event) override;

private:
	Ui::ImageViewerClass* ui;
	ViewerWidget* vW;

	QSettings settings;
	QMessageBox msgBox;

#pragma region 2D

	enum types { line, circle, polygon, curved, rectangle };
	QColor globalColor;
	QColor triangleColor[3];	//Leads to passing unnecessary variables, too bad!
	int currentPointIndex;
	int currentLayer;
	QVector<QPoint> temp;

#pragma endregion

	//Event filters
	bool eventFilter(QObject* obj, QEvent* event);

	//ViewerWidget Events
	bool ViewerWidgetEventFilter(QObject* obj, QEvent* event);
	void ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event);
	void ViewerWidgetLeave(ViewerWidget* w, QEvent* event);
	void ViewerWidgetEnter(ViewerWidget* w, QEvent* event);
	void ViewerWidgetWheel(ViewerWidget* w, QEvent* event);

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	bool openImage(QString filename);
	bool saveImage(QString filename);

	//My functions
	void initializeButtonGroup();
	void updateList(QString item);

private slots:
	void on_actionOpen_triggered();
	void on_actionSave_as_triggered();
	void on_actionClear_triggered();
	void on_actionExit_triggered();

	//Tools slots
	void on_pushButtonSetColor_clicked();
	void on_pushButtonSetColorA_clicked();
	void on_pushButtonSetColorB_clicked();
	void on_pushButtonSetColorC_clicked();

	void on_pushButtonClear_clicked();
	void on_pushButtonRotate_clicked();
	void on_pushButtonScale_clicked();
	void on_pushButtonMirror_clicked();
	void on_pushButtonShear_clicked();

	//3D
	void drawObject3D();
	void initializeButtonGroup3D();

	void on_pushButtonCube_clicked();
	void on_pushButtonUVSphere_clicked();
	void on_pushButtonLoad_clicked();
	void on_pushButtonSave_clicked();

	void on_listWidgetLayers_itemClicked(QListWidgetItem* item);
};
