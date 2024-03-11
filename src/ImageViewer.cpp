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
	globalColor = QColor("#55aa00");
	QString style_sheet = QString("background-color: #%1;").arg(globalColor.rgba(), 0, 16);
	ui->pushButtonSetColor->setStyleSheet(style_sheet);

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
		if (e->button() == Qt::LeftButton && ui->toolButtonDrawLine->isChecked())
		{
			if (w->getDrawLineActivated()) {
				w->addObjectPoint(e->pos());
				w->drawLine(w->getDrawLineBegin(), e->pos(), globalColor, ui->comboBoxLineAlg->currentIndex());
				w->setDrawLineActivated(false);
				
				enableButtons(false);
				w->setMoveActive(true);
			}
			else {
				w->addObjectPoint(e->pos());
				w->setDrawLineBegin(e->pos());
				w->setDrawLineActivated(true);
				w->setPixel(e->pos().x(), e->pos().y(), globalColor);
				w->update();
			}
		}
		else if (e->button() == Qt::LeftButton && ui->toolButtonDrawCircle->isChecked()) {
			if (w->getDrawLineActivated()) {
				w->addObjectPoint(e->pos());
				w->drawCircle(w->getDrawLineBegin(), e->pos(), globalColor);
				w->setDrawLineActivated(false);

				enableButtons(false);
				w->setMoveActive(true);
			}
			else {
				w->addObjectPoint(e->pos());
				w->setDrawLineBegin(e->pos());
				w->setDrawLineActivated(true);
				w->update();
			}
		}
		else if (e->button() == Qt::LeftButton && ui->toolButtonDrawPolygon->isChecked()) {
			w->addObjectPoint(e->pos());
			w->setDrawPolygonActivated(true);
			enableButtons(false);
			w->setPixel(e->pos().x(), e->pos().y(), globalColor);
			w->update();
		}
		else if (e->button() == Qt::RightButton && ui->toolButtonDrawPolygon->isChecked()) {
			if (w->getDrawPolygonActivated()) {
				w->drawPolygon(w->getObject(), globalColor, ui->comboBoxLineAlg->currentIndex());
				w->setMoveActive(true);
			}
		}
	}
	else {
		if (e->button() == Qt::LeftButton && w->getMoveActive()) {
			w->setMoving(true);
			w->setOrigin(e->pos());
		}
	}
}
void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	
	if (e->button() == Qt::LeftButton && w->getMoving()) {
		w->setMoving(false);

		QPoint move = e->pos() - w->getOrigin();
		
		for (int i = 0; i < w->objectSize(); i++) {
			QPoint point = w->getObjectPoint(i);
			w->changeObjectPoint(i, point + move);
		}
	}
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	if (w->getMoving()) {
		if (ui->toolButtonDrawLine->isChecked()) {
			QPoint move = e->pos() - w->getOrigin();
			w->clear();
			w->drawLine(w->getObject().at(0) + move, w->getObject().at(1) + move, globalColor, ui->comboBoxLineAlg->currentIndex());
		}
		else if (ui->toolButtonDrawCircle->isChecked()) {
			QPoint move = e->pos() - w->getOrigin();
			w->clear();
			w->drawCircle(w->getObject().at(0) + move, w->getObject().at(1) + move, globalColor);
		}
		else if (ui->toolButtonDrawPolygon->isChecked()) {
			QPoint move = e->pos() - w->getOrigin();
			QVector<QPoint> temp = w->getObject();

			for (int i = 0; i < temp.size(); i++) {
				temp[i] += move;
			}

			w->clear();
			w->drawPolygon(temp, globalColor, ui->comboBoxLineAlg->currentIndex());
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

		vW->scaleObject(multiplier, globalColor, type, ui->comboBoxLineAlg->currentIndex());
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
	enableButtons(true);
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

void ImageViewer::on_pushButtonClear_clicked()
{
	vW->clear();
	vW->clearObjectPoints();
	vW->setMoveActive(false);
	enableButtons(true);
}

void ImageViewer::initializeButtonGroup()
{
	QButtonGroup* buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(ui->toolButtonDrawLine);
	buttonGroup->addButton(ui->toolButtonDrawPolygon);
	buttonGroup->addButton(ui->toolButtonDrawCircle);
	buttonGroup->setExclusive(true);

	connect(ui->toolButtonDrawCircle, &QToolButton::clicked, [this]() {	ui->comboBoxLineAlg->setEnabled(false);	});
	connect(ui->toolButtonDrawLine, &QToolButton::clicked, [this]() {	ui->comboBoxLineAlg->setEnabled(true);	});
	connect(ui->toolButtonDrawPolygon, &QToolButton::clicked, [this]() {	ui->comboBoxLineAlg->setEnabled(true);	});
}

void ImageViewer::enableButtons(bool state) {
	ui->comboBoxLineAlg->setEnabled(state);
	ui->toolButtonDrawCircle->setEnabled(state);
	ui->toolButtonDrawLine->setEnabled(state);
	ui->toolButtonDrawPolygon->setEnabled(state);
}

void ImageViewer::on_pushButtonRotate_clicked() {
	enum types { line = 0, polygon = 2};
	int type = 0;

	if (ui->toolButtonDrawLine->isChecked())
		type = line;
	else if (ui->toolButtonDrawPolygon->isChecked())
		type = polygon;

	vW->rotateObject(ui->spinBoxRotate->value(), type, globalColor, ui->comboBoxLineAlg->currentIndex());
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

	vW->scaleObject(ui->doubleSpinBoxScaleX->value(), ui->doubleSpinBoxScaleY->value(),globalColor, type, ui->comboBoxLineAlg->currentIndex());
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

	vW->mirrorObject(type, globalColor, ui->comboBoxLineAlg->currentIndex());
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

	vW->shearObjectDx(type, globalColor, ui->doubleSpinBoxScaleX->value(), ui->comboBoxLineAlg->currentIndex());
}