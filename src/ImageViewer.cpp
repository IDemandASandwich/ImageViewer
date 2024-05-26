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
	initializeButtonGroup3D();

	ui->listWidgetLayers->setDefaultDropAction(Qt::MoveAction);
	currentLayer = 0;
}

void ImageViewer::dropEvent(QDropEvent* event) {
	QListWidget* source = qobject_cast<QListWidget*>(event->source());
	if (source == ui->listWidgetLayers) {
		event->setDropAction(Qt::MoveAction);
		ImageViewer::dropEvent(event);
	}
	else {
		ImageViewer::dropEvent(event);
	}
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
	if (ui->radioButtonDraw->isChecked())
	{
		if (e->button() == Qt::LeftButton)
		{
			if(ui->toolButtonDrawLine->isChecked()){
				if (w->getDrawLineActivated()) {
					//w->drawLine(currentLayer, w->getDrawLineBegin(), e->pos(), globalColor, ui->comboBoxLineAlg->currentIndex());
					w->setDrawLineActivated(false);
					w->pushBackObject({w->getDrawLineBegin(), e->pos()}, globalColor, ui->listWidgetLayers->count(), types::line);
					updateList("Line");
				}
				else {
					w->setDrawLineBegin(e->pos());
					w->setDrawLineActivated(true);
					w->update();
				}
			}
			else if (ui->toolButtonDrawCircle->isChecked()) {
				if (w->getDrawLineActivated()) {
					//w->drawCircle(currentLayer, w->getDrawLineBegin(), e->pos(), globalColor);
					w->setDrawLineActivated(false);
					w->pushBackObject({ w->getDrawLineBegin(), e->pos() }, globalColor, ui->listWidgetLayers->count(), types::circle);
					updateList("Circle");
				}
				else {
					w->setDrawLineBegin(e->pos());
					w->setDrawLineActivated(true);
					w->update();
				}
			}
			else if (ui->toolButtonDrawPolygon->isChecked()) {
				w->update();
				temp.append(e->pos());
			}
			else if (ui->toolButtonDrawCurved->isChecked()) {	//duplicate code left for possible future expansion
				w->update();
				temp.append(e->pos());
			}
			else if (ui->toolButtonDrawRectangle->isChecked()) {
				if (w->getDrawLineActivated()) {
					QPoint p2(w->getDrawLineBegin().x(), e->pos().y());
					QPoint p3(e->pos().x(), w->getDrawLineBegin().y());
					QVector<QPoint> p = { w->getDrawLineBegin(), p2, e->pos(), p3 };
					QColor C[3] = { Qt::black,Qt::black, Qt::black };
					w->pushBackObject(p, globalColor, ui->listWidgetLayers->count(), types::rectangle, ui->checkBoxFill->isChecked(), C, 0);
					//w->drawPolygon(currentLayer, p, globalColor, ui->checkBoxFill->isChecked());
					w->setDrawLineActivated(false);
					updateList("Rectangle");
				}
				else {
					w->setDrawLineBegin(e->pos());
					w->setDrawLineActivated(true);
					w->update();
				}
			}

			w->setPixel(e->pos().x(), e->pos().y(), Qt::red);
		}
		else if (e->button() == Qt::RightButton) {
			bool fill = false;
			int type;
			if(ui->toolButtonDrawPolygon->isChecked()){
				fill = (ui->checkBoxFill->isChecked() && temp.size() >= 3) ? true : false;
				//w->drawPolygon(currentLayer, temp, globalColor, fill, triangleColor, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
				type = types::polygon;
				w->pushBackObject(temp, globalColor, ui->listWidgetLayers->count(), type, fill, triangleColor, ui->comboBoxFillType->currentIndex());
				updateList("Polygon");
			}
			if (ui->toolButtonDrawCurved->isChecked()) {
				type = types::curved;
				w->pushBackObject(temp, globalColor, ui->listWidgetLayers->count(), type, ui->comboBoxCurvedType->currentIndex());
				//w->drawCurve(currentLayer, temp, globalColor, ui->comboBoxCurvedType->currentIndex(), ui->comboBoxShowAddons->currentIndex());
				updateList("Curved line");
			}

			temp.clear();
		}

		vW->drawList();
	}
	else {
		if (e->button() == Qt::LeftButton && ui->listWidgetLayers->count() > 0) {
			currentPointIndex = w->getClosestPointIndex(currentLayer, e->pos());
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
		QVector<QPoint> newObj;
		
		if (ui->toolButtonDrawPolygon->isChecked() || ui->toolButtonDrawRectangle->isChecked()) {
			QVector<QPoint> temp = w->getObjectPoints(currentLayer);

			for (int i = 0; i < temp.size(); i++) {
				temp[i] += move;
			}

			newObj = temp;
		}
		else if (ui->toolButtonDrawCurved->isChecked()) {
			QVector<QPoint> temp = w->getObjectPoints(currentLayer);
			temp[currentPointIndex] += move;

			if ((ui->comboBoxCurvedType->currentIndex() == 0) && (currentPointIndex % 2 == 0)) {
				temp[currentPointIndex + 1] += move;
			}

			newObj = temp;
		}
		else{
			newObj = { w->getObjectPoints(currentLayer).at(0) + move, w->getObjectPoints(currentLayer).at(1) + move };
		}

		if (ui->toolButtonDrawPolygon->isChecked() || ui->toolButtonDrawRectangle->isChecked()) {
			bool fill = (ui->checkBoxFill->isChecked() && newObj.size() >= 3) ? true : false;
			w->changeObject(currentLayer, newObj, triangleColor, fill, ui->comboBoxFillType->currentIndex());
		}
		else {
			w->changeObject(currentLayer, newObj);
		}

		w->drawList();
	}
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	if (w->getMoving()) {
		QPoint move = e->pos() - w->getOrigin();
		object temp = w->getObject(currentLayer);

		if (ui->toolButtonDrawCurved->isChecked()) {
			temp.points[currentPointIndex] += move;

			if ((ui->comboBoxCurvedType->currentIndex() == 0) && (currentPointIndex%2 == 0)) {
				temp.points[currentPointIndex + 1] += move;
			}
		}
		else {
			for (int i = 0; i < temp.points.size(); i++) {
				temp.points[i] += move;
			}
			bool fill = (ui->checkBoxFill->isChecked() && temp.points.size() >= 3) ? true : false;
		}

		w->drawObject(temp);
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

		vW->scaleObject(currentLayer, multiplier, globalColor, triangleColor, type, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
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

bool ImageViewer::saveState(QString filename) {
	QVector<object> l = vW->getList();
	if (l.isEmpty())
		return false;

	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}

	QTextStream out(&file);

	for (object o : l) {
		out << o.points.size() << " ";
		for (QPoint p : o.points) {
			out << p.x() << " " << p.y() << " ";
		}
		out << o.color.rgba() << " ";
		out << o.triangleColors[0].rgba() << " " << o.triangleColors[1].rgba() << " " << o.triangleColors[2].rgba() << " ";
		out << o.layer << " ";
		out << o.type << " ";
		out << o.curvedType << " ";
		out << o.fillType << " ";
		out << o.fill << "\n";
	}

	file.close();

	return true;
}
bool ImageViewer::loadState(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}

	vW->clearList();
	ui->listWidgetLayers->clear();

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();
		QStringList l = line.split(" ");

		int numPoints = l[0].toInt() * 2;
		QStringList XY = l.mid(1, numPoints);
		QVector<QPoint> points;
		for (int i = 0; i < XY.size() - 1; i+=2) {
			points.append(QPoint(XY[i].toInt(), XY[i + 1].toInt()));
		}

		QColor color = QColor::fromRgba(l[numPoints + 1].toUInt());
		QColor TC[3] = {QColor::fromRgba(l[numPoints + 2].toUInt()),QColor::fromRgba(l[numPoints + 3].toUInt()),QColor::fromRgba(l[numPoints + 4].toUInt())};
		int layer = l[numPoints + 5].toInt();
		int type = l[numPoints + 6].toInt();
		int curvedType = l[numPoints + 7].toInt();
		int fillType = l[numPoints + 8].toInt();
		bool fill = l[numPoints + 9].toInt();

		vW->pushBackObject(points, color, layer, type, fill, TC, fillType, curvedType);

		switch (type) {
		case types::line:
			updateList("Line");
			break;
		case types::circle:
			updateList("Circle");
			break;
		case types::polygon:
			updateList("Polygon");
			break;
		case types::curved:
			updateList("Curved line");
			break;
		case types::rectangle:
			updateList("Rectangle");
			break;
		}
	}

	file.close();

	vW->clear();
	vW->drawList();

	return true;
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
	vW->removeObject(currentLayer);

	if (currentLayer >= 0 && currentLayer < ui->listWidgetLayers->count()) {
		delete ui->listWidgetLayers->takeItem(currentLayer);
	}
	currentLayer = 0;

	vW->clear();
	vW->drawList();
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

		if(ui->radioButtonMove->isChecked())
			vW->changeObjectColor(currentLayer, globalColor);
	}
}
void ImageViewer::on_pushButtonSetColorA_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColorA->setStyleSheet(style_sheet);

		if (ui->radioButtonMove->isChecked())
			triangleColor[0] = newColor;
	}
}
void ImageViewer::on_pushButtonSetColorB_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColorB->setStyleSheet(style_sheet);

		if (ui->radioButtonMove->isChecked())
			triangleColor[1] = newColor;
	}
}
void ImageViewer::on_pushButtonSetColorC_clicked()
{
	QColor newColor = QColorDialog::getColor(globalColor, this);
	if (newColor.isValid()) {
		QString style_sheet = QString("background-color: #%1;").arg(newColor.rgba(), 0, 16);
		ui->pushButtonSetColorC->setStyleSheet(style_sheet);
		
		if (ui->radioButtonMove->isChecked())
			triangleColor[2] = newColor;
	}
}

void ImageViewer::on_pushButtonClear_clicked()
{
	if (ui->listWidgetLayers->count() <= 0)
		return;

	vW->removeObject(currentLayer);
	if (currentLayer >= 0 && currentLayer < ui->listWidgetLayers->count()) {
		delete ui->listWidgetLayers->takeItem(currentLayer);

		if(currentLayer > 0)
			currentLayer--;
	}
	ui->listWidgetLayers->setCurrentRow(currentLayer);

	programmaticChange = true;
	ui->spinBoxLayer->setMaximum(ui->listWidgetLayers->count() - 1);
	programmaticChange = false;

	vW->clear();
	vW->drawList();
}

void ImageViewer::initializeButtonGroup()
{
	ui->groupBox_4->setVisible(ui->toolBox->currentIndex() == 1);

	QButtonGroup* buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(ui->toolButtonDrawLine);
	buttonGroup->addButton(ui->toolButtonDrawPolygon);
	buttonGroup->addButton(ui->toolButtonDrawCircle);
	buttonGroup->addButton(ui->toolButtonDrawCurved);
	buttonGroup->addButton(ui->toolButtonDrawRectangle);
	buttonGroup->setExclusive(true);

	connect(ui->comboBoxShowAddons, &QComboBox::currentIndexChanged, [this]() {
		if (ui->toolButtonDrawCurved->isChecked()) {
			vW->clear();
			vW->drawCurve(currentLayer, vW->getObjectPoints(currentLayer), globalColor, ui->comboBoxCurvedType->currentIndex(), ui->comboBoxShowAddons->currentIndex());
		}
	});
	connect(ui->toolBox, &QToolBox::currentChanged, [this](int index) {
		if (index == 0) {
			ui->groupBox_4->setVisible(false);
		}
		else {
			ui->groupBox_4->setVisible(true);
		}
	});
}

void ImageViewer::on_pushButtonRotate_clicked() {
	enum types { line = 0, polygon = 2};
	int type = 0;

	if (ui->toolButtonDrawLine->isChecked())
		type = line;
	else if (ui->toolButtonDrawPolygon->isChecked())
		type = polygon;

	vW->rotateObject(currentLayer, ui->spinBoxRotate->value(), type, globalColor, triangleColor, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
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

	vW->mirrorObject(currentLayer, type, globalColor, triangleColor, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
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

	vW->shearObjectDx(currentLayer, type, globalColor, triangleColor, ui->doubleSpinBoxScaleX->value(), ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
}

// 3D
void ImageViewer::drawObject3D() {
	enum types { parallel, perspective };
	int distance = ui->spinBoxDistance->value();
	bool wireframe = ui->checkBoxWireframe->isChecked();
	double zenith = static_cast<double>(ui->verticalSliderZenith->value()) * M_PI / 180.;
	double azimuth = static_cast<double>(ui->horizontalSliderAzimuth->value()) * M_PI / 180.;

	int x = ui->spinBoxSourceX->value();
	int y = ui->spinBoxSourceY->value();
	int z = ui->spinBoxSourceZ->value();
	unsigned char r = static_cast<unsigned char>(ui->spinBoxSourceR->value());
	unsigned char g = static_cast<unsigned char>(ui->spinBoxSourceG->value());
	unsigned char b = static_cast<unsigned char>(ui->spinBoxSourceB->value());
	light source(r, g, b, x, y, z);

	r = static_cast<unsigned char>(ui->spinBoxAmbientR->value());
	g = static_cast<unsigned char>(ui->spinBoxAmbientG->value());
	b = static_cast<unsigned char>(ui->spinBoxAmbientB->value());
	double cr = ui->doubleSpinBoxAmbientCoeffR->value();
	double cg = ui->doubleSpinBoxAmbientCoeffG->value();
	double cb = ui->doubleSpinBoxAmbientCoeffB->value();
	light ambient(r, g, b, cr, cg, cb);

	cr = ui->doubleSpinBoxDiffusionCoeffR->value();
	cg = ui->doubleSpinBoxDiffusionCoeffG->value();
	cb = ui->doubleSpinBoxDiffusionCoeffB->value();
	light diffusion(cr, cg, cb);

	cr = ui->doubleSpinBoxReflectionCoeffR->value();
	cg = ui->doubleSpinBoxReflectionCoeffG->value();
	cb = ui->doubleSpinBoxReflectionCoeffB->value();
	light reflection(cr, cg, cb);

	lighting primary(source, ambient, diffusion, reflection);

	int lightingMethod = 0;
	ui->radioButtonConstant->isChecked() ? lightingMethod = 0 : lightingMethod = 1;

	int cameraDistance = ui->spinBoxCameraDistance->value();
	int lightSharpness = ui->spinBoxLightSharpness->value();

	int type = 0;
	if (ui->radioButtonParallel->isChecked())
		type = parallel;
	else
		type = perspective;

	vW->projectObject(primary, lightingMethod, cameraDistance, lightSharpness, zenith, azimuth, type, distance, wireframe);
}
void ImageViewer::initializeButtonGroup3D(){
	// can be optimised, this redraws the whole thing even if the changed setting doesn't affect it
	ui->groupBox_4->setVisible(ui->toolBox->currentIndex() == 1);
	
	connect(ui->checkBoxWireframe, &QCheckBox::stateChanged, this, &ImageViewer::drawObject3D);
	
	connect(ui->verticalSliderZenith, &QSlider::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->horizontalSliderAzimuth, &QSlider::valueChanged, this, &ImageViewer::drawObject3D);
	
	connect(ui->spinBoxDistance, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->spinBoxCameraDistance, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->spinBoxLightSharpness, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);

	connect(ui->spinBoxSourceX, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->spinBoxSourceY, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->spinBoxSourceZ, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->spinBoxSourceR, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->spinBoxSourceG, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->spinBoxSourceB, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);

	connect(ui->spinBoxAmbientR, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->spinBoxAmbientG, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->spinBoxAmbientB, &QSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->doubleSpinBoxAmbientCoeffR, &QDoubleSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->doubleSpinBoxAmbientCoeffG, &QDoubleSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->doubleSpinBoxAmbientCoeffB, &QDoubleSpinBox::valueChanged, this, &ImageViewer::drawObject3D);

	connect(ui->doubleSpinBoxDiffusionCoeffR, &QDoubleSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->doubleSpinBoxDiffusionCoeffG, &QDoubleSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->doubleSpinBoxDiffusionCoeffB, &QDoubleSpinBox::valueChanged, this, &ImageViewer::drawObject3D);

	connect(ui->doubleSpinBoxReflectionCoeffR, &QDoubleSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->doubleSpinBoxReflectionCoeffG, &QDoubleSpinBox::valueChanged, this, &ImageViewer::drawObject3D);
	connect(ui->doubleSpinBoxReflectionCoeffB, &QDoubleSpinBox::valueChanged, this, &ImageViewer::drawObject3D);

	connect(ui->radioButtonParallel, &QRadioButton::toggled, this, &ImageViewer::drawObject3D);
	connect(ui->radioButtonConstant, &QRadioButton::toggled, this, &ImageViewer::drawObject3D);
}

void ImageViewer::on_pushButtonCube_clicked() {
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.vtk)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);

	int l = ui->spinBoxCubeSideLength->value();

	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!vW->saveCubeToVTK(fileName, l)) {
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
void ImageViewer::on_pushButtonUVSphere_clicked() {
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.vtk)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);

	double radius = ui->spinBoxRadius->value();
	int rings = ui->spinBoxRings->value();
	int segments = ui->spinBoxSegments->value();

	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!vW->saveUVSphereToVTK(fileName, radius, rings, segments)) {
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

void ImageViewer::on_pushButtonLoad_clicked() {
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Object data (*.vtk)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load object", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!vW->loadObject(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
	else {
		drawObject3D();
	}
}
void ImageViewer::on_pushButtonSave_clicked() {
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.vtk)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	bool wireframe = ui->checkBoxWireframe->isChecked();

	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!vW->saveObject(fileName, wireframe)) {
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

void ImageViewer::on_pushButtonSaveState_clicked() {
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image viewer save state (*.ivss)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save state", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveState(fileName)) {
			msgBox.setText("Unable to save state.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}
void ImageViewer::on_pushButtonLoadState_clicked() {
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image viewer save state (*.ivss)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load state", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!loadState(fileName)) {
		msgBox.setText("Unable to load state.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}

void ImageViewer::on_listWidgetLayers_itemClicked(QListWidgetItem* item) {
	int currentRow = ui->listWidgetLayers->row(item);
	currentLayer = currentRow;
	object o = vW->getObject(currentLayer);
	globalColor = o.color;

	QString style_sheet = QString("background-color: #%1;").arg(o.color.rgba(), 0, 16);
	ui->pushButtonSetColor->setStyleSheet(style_sheet);

	ui->radioButtonMove->setChecked(true);
	programmaticChange = true;
	ui->spinBoxLayer->setValue(o.layer);
	programmaticChange = false;

	switch (o.type) {
	case(types::line):
		ui->toolButtonDrawLine->setChecked(true);
		break;
	case(types::circle):
		ui->toolButtonDrawCircle->setChecked(true);
		ui->comboBoxCurvedType->setCurrentIndex(o.curvedType);
		break;
	case(types::polygon):
		ui->toolButtonDrawPolygon->setChecked(true);
		ui->checkBoxFill->setChecked(o.fill);
		ui->comboBoxFillType->setCurrentIndex(o.fillType);

		if (o.points.size() == 3) {
			style_sheet = QString("background-color: #%1;").arg(o.triangleColors[0].rgba(), 0, 16);
			ui->pushButtonSetColorA->setStyleSheet(style_sheet);
			style_sheet = QString("background-color: #%1;").arg(o.triangleColors[1].rgba(), 0, 16);
			ui->pushButtonSetColorB->setStyleSheet(style_sheet);
			style_sheet = QString("background-color: #%1;").arg(o.triangleColors[2].rgba(), 0, 16);
			ui->pushButtonSetColorC->setStyleSheet(style_sheet);
		}

		break;
	case(types::curved):
		ui->toolButtonDrawCurved->setChecked(true);
		break;
	case(types::rectangle):
		ui->toolButtonDrawRectangle->setChecked(true);
		ui->checkBoxFill->setChecked(o.fill);
		ui->comboBoxFillType->setCurrentIndex(o.fillType);
		break;
	}
}

void ImageViewer::updateList(QString item) {
	currentLayer = ui->listWidgetLayers->count();
	ui->listWidgetLayers->addItem(item);
	ui->listWidgetLayers->setCurrentRow(currentLayer);
	ui->spinBoxLayer->setMaximum(ui->listWidgetLayers->count() - 1);

	programmaticChange = true;
	ui->spinBoxLayer->setValue(currentLayer);
	programmaticChange = false;
}

void ImageViewer::on_radioButtonMove_toggled(bool checked) {
	vW->setMoveActive(checked);
	ui->groupBoxDraw->setDisabled(checked);
}

void ImageViewer::on_spinBoxLayer_valueChanged(int value) {
	if (!programmaticChange) {
		int oldIndex = currentLayer;
		int newIndex = value;

		QListWidgetItem* item1 = ui->listWidgetLayers->item(oldIndex);
		QListWidgetItem* item2 = ui->listWidgetLayers->item(newIndex);
		QString temp = item1->text();

		item1->setText(item2->text());
		item2->setText(temp);

		vW->swapObjects(oldIndex, newIndex);

		currentLayer = newIndex;
		ui->listWidgetLayers->setCurrentRow(currentLayer);

		vW->drawList();
	}
}