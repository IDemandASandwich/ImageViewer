#include "ImageViewer.h"


ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	vW = new ViewerWidget(QSize(500, 500));
	ui->scrollArea->setWidget(vW);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(true);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);
	globalColor = Qt::black;
	QString style_sheet = QString("background-color: #%1;").arg(globalColor.rgba(), 0, 16);
	ui->pushButtonSetColor->setStyleSheet(style_sheet);

	triangleColor[0] = Qt::red;
	triangleColor[1] = Qt::green;
	triangleColor[2] = Qt::blue;
	ui->pushButtonSetColorA->setStyleSheet(QString("background-color: #%1;").arg(triangleColor[0].rgba(), 0, 16));
	ui->pushButtonSetColorB->setStyleSheet(QString("background-color: #%1;").arg(triangleColor[1].rgba(), 0, 16));
	ui->pushButtonSetColorC->setStyleSheet(QString("background-color: #%1;").arg(triangleColor[2].rgba(), 0, 16));


	initializeButtonGroup();
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (w->getMoveActive() == false)
	{
		if (e->button() == Qt::LeftButton)
		{
			if(ui->toolButtonDrawLine->isChecked()){
				if (w->getDrawLineActivated()) {
					w->drawLine(w->getDrawLineBegin(), e->pos(), globalColor, ui->comboBoxLineAlg->currentIndex());
					w->setDrawLineActivated(false);
					w->setMoveActive(true);
				}
				else {
					w->setDrawLineBegin(e->pos());
					w->setDrawLineActivated(true);
					w->update();
				}
			}
			else if (ui->toolButtonDrawCircle->isChecked()) {
				if (w->getDrawLineActivated()) {
					w->drawCircle(w->getDrawLineBegin(), e->pos(), globalColor);
					w->setDrawLineActivated(false);
					w->setMoveActive(true);
				}
				else {
					w->setDrawLineBegin(e->pos());
					w->setDrawLineActivated(true);
					w->update();
				}
			}
			else if (ui->toolButtonDrawPolygon->isChecked()) {
				w->update();
			}
			else if (ui->toolButtonDrawCurved->isChecked()) {	//duplicate code left for possible future expansion
				w->update();
			}

			w->addObjectPoint(e->pos());
			w->setPixel(e->pos().x(), e->pos().y(), Qt::red);
		}
		else if (e->button() == Qt::RightButton) {
			if(ui->toolButtonDrawPolygon->isChecked()){
				w->drawPolygon(w->getObject(), globalColor, triangleColor, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
			}
			if (ui->toolButtonDrawCurved->isChecked()) {
				w->drawCurve(w->getObject(), globalColor, ui->comboBoxCurvedType->currentIndex(), ui->comboBoxShowAddons->currentIndex());
			}

			w->setMoveActive(true);
		}
	}
	else {
		if (e->button() == Qt::LeftButton) {
			currentPointIndex = w->getClosestPointIndex(e->pos());
			w->setMoving(true);
			w->setOrigin(e->pos());
		}
	}

	ui->groupBoxFill->setVisible(ui->toolButtonDrawPolygon->isChecked());
	ui->groupBoxEdit->setVisible(ui->toolButtonDrawCurved->isChecked() == false);
	ui->groupBoxDraw->setVisible(false);
}
void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	
	if (e->button() == Qt::LeftButton && w->getMoving()) {
		w->setMoving(false);

		QPoint move = e->pos() - w->getOrigin();
		
		if (ui->toolButtonDrawCurved->isChecked()) {
			QPoint p1 = w->getObjectPoint(currentPointIndex);

			if (currentPointIndex % 2 == 0 && ui->comboBoxCurvedType->currentIndex() == 0) {
				QPoint p2 = w->getObjectPoint(currentPointIndex + 1);
				w->changeObjectPoint(currentPointIndex + 1, p2 + move);
			}

			w->changeObjectPoint(currentPointIndex, p1 + move);
		}
		else {
			for (int i = 0; i < w->objectSize(); i++) {
				QPoint p = w->getObjectPoint(i);
				w->changeObjectPoint(i, p + move);
			}
		}
	}
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	if (w->getMoving()) {
		QPoint move = e->pos() - w->getOrigin();

		if (ui->toolButtonDrawLine->isChecked()) {
			w->clear();
			w->drawLine(w->getObject().at(0) + move, w->getObject().at(1) + move, globalColor, ui->comboBoxLineAlg->currentIndex());
		}
		else if (ui->toolButtonDrawCircle->isChecked()) {
			w->clear();
			w->drawCircle(w->getObject().at(0) + move, w->getObject().at(1) + move, globalColor);
		}
		else if (ui->toolButtonDrawPolygon->isChecked()) {
			QVector<QPoint> temp = w->getObject();

			for (int i = 0; i < temp.size(); i++) {
				temp[i] += move;
			}

			w->clear();
			w->drawPolygon(temp, globalColor, triangleColor, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
		}
		else if (ui->toolButtonDrawCurved->isChecked()) {
			QVector<QPoint> temp = w->getObject();
			temp[currentPointIndex] += move;

			if ((ui->comboBoxCurvedType->currentIndex() == 0) && (currentPointIndex%2 == 0)) {
				temp[currentPointIndex + 1] += move;
			}

			w->clear();
			w->drawCurve(temp, globalColor, ui->comboBoxCurvedType->currentIndex(), ui->comboBoxShowAddons->currentIndex());
		}
	}
}
void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
	enum types { line, circle, polygon };

	if (vW->getMoveActive()) {
		double multiplier = 0;
		int type = 0;

		if (ui->toolButtonDrawLine->isChecked())
			type = line;
		else if (ui->toolButtonDrawCircle->isChecked())
			type = circle;
		else if (ui->toolButtonDrawPolygon->isChecked())
			type = polygon;

		if (wheelEvent->angleDelta().y() > 0)
			multiplier = 1.25;
		else
			multiplier = 0.75;

		vW->scaleObject(multiplier, globalColor, triangleColor, type, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
	}
		
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
bool ImageViewer::openImage(QString filename)
{
	QImage loadedImg(filename);
	if (!loadedImg.isNull()) {
		return vW->setImage(loadedImg);
	}
	return false;
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();

	QImage* img = vW->getImage();
	return img->save(filename, extension.toStdString().c_str());
}

//Slots
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveImage(fileName)) {
			msgBox.setText("Unable to save image.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered()
{
	vW->clear();
	vW->clearObjectPoints();
	vW->setMoveActive(false);
}
void ImageViewer::on_actionExit_triggered()
{
	this->close();
}

void ImageViewer::on_pushButtonSetColor_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColor->setStyleSheet(style_sheet);
		globalColor = newColor;
	}
}
void ImageViewer::on_pushButtonSetColorA_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColorA->setStyleSheet(style_sheet);
		triangleColor[0] = newColor;
	}
}
void ImageViewer::on_pushButtonSetColorB_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColorB->setStyleSheet(style_sheet);
		triangleColor[1] = newColor;
	}
}
void ImageViewer::on_pushButtonSetColorC_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColorC->setStyleSheet(style_sheet);
		triangleColor[2] = newColor;
	}
}

void ImageViewer::on_pushButtonClear_clicked()
{
	vW->clear();
	vW->clearObjectPoints();
	vW->setMoveActive(false);

	ui->groupBoxDraw->setVisible(true);
	ui->groupBoxEdit->setVisible(false);
	ui->groupBoxFill->setVisible(false);
}

void ImageViewer::initializeButtonGroup()
{
	QButtonGroup* buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(ui->toolButtonDrawLine);
	buttonGroup->addButton(ui->toolButtonDrawPolygon);
	buttonGroup->addButton(ui->toolButtonDrawCircle);
	buttonGroup->addButton(ui->toolButtonDrawCurved);
	buttonGroup->setExclusive(true);

	ui->groupBoxEdit->setVisible(false);
	ui->groupBoxFill->setVisible(false);

	ui->comboBoxLineAlg->setVisible(false);
	ui->comboBoxCurvedType->setVisible(false);
	ui->comboBoxShowAddons->setVisible(false);

	connect(ui->toolButtonDrawCircle, &QToolButton::clicked, [this]() {	
		ui->comboBoxLineAlg->setVisible(false); 
		ui->comboBoxCurvedType->setVisible(false);	
		ui->comboBoxShowAddons->setVisible(false);

	});
	connect(ui->toolButtonDrawLine, &QToolButton::clicked, [this]() {	
		ui->comboBoxLineAlg->setVisible(true);	
		ui->comboBoxCurvedType->setVisible(false); 
		ui->comboBoxShowAddons->setVisible(false);

	});
	connect(ui->toolButtonDrawPolygon, &QToolButton::clicked, [this]() {	
		ui->comboBoxLineAlg->setVisible(true);	
		ui->comboBoxCurvedType->setVisible(false);
		ui->comboBoxShowAddons->setVisible(false);

	});
	connect(ui->toolButtonDrawCurved, &QToolButton::clicked, [this]() {	
		ui->comboBoxLineAlg->setVisible(false);	
		ui->comboBoxCurvedType->setVisible(true); 
		ui->comboBoxShowAddons->setVisible(true);

	});

	connect(ui->comboBoxShowAddons, &QComboBox::currentIndexChanged, [this]() {
		vW->clear();
		vW->drawCurve(vW->getObject(), globalColor, ui->comboBoxCurvedType->currentIndex(), ui->comboBoxShowAddons->currentIndex());
	});
}

void ImageViewer::on_pushButtonRotate_clicked() {
	enum types { line = 0, polygon = 2};
	int type = 0;

	if (ui->toolButtonDrawLine->isChecked())
		type = line;
	else if (ui->toolButtonDrawPolygon->isChecked())
		type = polygon;

	vW->rotateObject(ui->spinBoxRotate->value(), type, globalColor, triangleColor, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
}
void ImageViewer::on_pushButtonScale_clicked() {
	enum types { line, circle, polygon };
	int type = 0;

	if (ui->toolButtonDrawLine->isChecked())
		type = line;
	else if (ui->toolButtonDrawCircle->isChecked())
		type = circle;
	else if (ui->toolButtonDrawPolygon->isChecked())
		type = polygon;

	vW->scaleObject(ui->doubleSpinBoxScaleX->value(), ui->doubleSpinBoxScaleY->value(),globalColor, triangleColor, type, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
}
void ImageViewer::on_pushButtonMirror_clicked() {
	enum types { line, circle, polygon };
	int type = 0;

	if (ui->toolButtonDrawLine->isChecked())
		type = line;
	else if (ui->toolButtonDrawCircle->isChecked())
		type = circle;
	else if (ui->toolButtonDrawPolygon->isChecked())
		type = polygon;

	vW->mirrorObject(type, globalColor, triangleColor, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
}
void ImageViewer::on_pushButtonShear_clicked() {
	enum types { line, circle, polygon };
	int type = 0;

	if (ui->toolButtonDrawLine->isChecked())
		type = line;
	else if (ui->toolButtonDrawCircle->isChecked())
		type = circle;
	else if (ui->toolButtonDrawPolygon->isChecked())
		type = polygon;

	vW->shearObjectDx(type, globalColor, triangleColor, ui->doubleSpinBoxScaleX->value(), ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
}

void ImageViewer::on_pushButtonCube_clicked() {
	QVector<QVector3D> cube;

	int l = ui->spinBoxCubeSideLength->value();

	QVector3D v0(0 ,0 ,0 );
	QVector3D v1(l ,0 ,0 );
	QVector3D v2(l ,l ,0 );
	QVector3D v3(0 ,l ,0 );
	QVector3D v4(0 ,0 ,l );
	QVector3D v5(l ,0 ,l );
	QVector3D v6(l ,l ,l );
	QVector3D v7(0 ,l ,l );

	cube << v0 << v1 << v2 << v3 << v4 << v5 << v6 << v7;

	vW->createObjectCube2(cube);
}