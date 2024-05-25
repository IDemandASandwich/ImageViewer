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
					w->drawLine(w->getDrawLineBegin(), e->pos(), globalColor, ui->comboBoxLineAlg->currentIndex());
					w->setDrawLineActivated(false);
					w->setMoveActive(true);
					w->pushBackObject({w->getDrawLineBegin(), e->pos()}, globalColor, 0, types::line, ui->checkBoxFill->isChecked());
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
					w->drawCircle(w->getDrawLineBegin(), e->pos(), globalColor);
					w->setDrawLineActivated(false);
					w->setMoveActive(true);
					w->pushBackObject({ w->getDrawLineBegin(), e->pos() }, globalColor, 0, types::circle, false);
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
					w->drawPolygon(p, globalColor, ui->checkBoxFill->isChecked());
					w->setDrawLineActivated(false);
					w->setMoveActive(true);
					w->pushBackObject( p, globalColor , 0, types::rectangle, ui->checkBoxFill->isChecked());
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
				w->drawPolygon(temp, globalColor, fill, triangleColor, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
				type = types::polygon;
				updateList("Polygon");
			}
			if (ui->toolButtonDrawCurved->isChecked()) {
				w->drawCurve(temp, globalColor, ui->comboBoxCurvedType->currentIndex(), ui->comboBoxShowAddons->currentIndex());
				type = types::curved;
				updateList("Curved line");
			}

			w->pushBackObject(temp, globalColor, 0, type, fill);
			w->setMoveActive(true);
			temp.clear();
		}
	}
	else {
		if (e->button() == Qt::LeftButton) {
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
			QVector<QPoint> temp = w->getObject(currentLayer);

			for (int i = 0; i < temp.size(); i++) {
				temp[i] += move;
			}

			newObj = temp;
		}
		else if (ui->toolButtonDrawCurved->isChecked()) {
			QVector<QPoint> temp = w->getObject(currentLayer);
			temp[currentPointIndex] += move;

			if ((ui->comboBoxCurvedType->currentIndex() == 0) && (currentPointIndex % 2 == 0)) {
				temp[currentPointIndex + 1] += move;
			}

			newObj = temp;
		}
		else{
			newObj = { w->getObject(currentLayer).at(0) + move, w->getObject(currentLayer).at(1) + move };
		}

		if (ui->toolButtonDrawPolygon->isChecked() || ui->toolButtonDrawRectangle->isChecked()) {
			bool fill = (ui->checkBoxFill->isChecked() && newObj.size() >= 3) ? true : false;
			w->changeObject(currentLayer, newObj, triangleColor, fill, ui->comboBoxFillType->currentIndex());
		}
		else {
			w->changeObject(currentLayer, newObj);
		}

		w->clear();
		w->drawList();
	}
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);

	if (w->getMoving()) {
		QPoint move = e->pos() - w->getOrigin();

		if (ui->toolButtonDrawLine->isChecked()) {
			w->clear();
			w->drawLine(w->getObject(currentLayer).at(0) + move, w->getObject(currentLayer).at(1) + move, globalColor, ui->comboBoxLineAlg->currentIndex());
		}
		else if (ui->toolButtonDrawCircle->isChecked()) {
			w->clear();
			w->drawCircle(w->getObject(currentLayer).at(0) + move, w->getObject(currentLayer).at(1) + move, globalColor);
		}
		else if (ui->toolButtonDrawPolygon->isChecked()) {
			QVector<QPoint> temp = w->getObject(currentLayer);

			for (int i = 0; i < temp.size(); i++) {
				temp[i] += move;
			}
			bool fill = (ui->checkBoxFill->isChecked() && temp.size() >= 3) ? true : false;

			w->clear();
			w->drawPolygon(temp, globalColor, fill, triangleColor, ui->comboBoxLineAlg->currentIndex(), ui->comboBoxFillType->currentIndex());
		}
		else if (ui->toolButtonDrawCurved->isChecked()) {
			QVector<QPoint> temp = w->getObject(currentLayer);
			temp[currentPointIndex] += move;

			if ((ui->comboBoxCurvedType->currentIndex() == 0) && (currentPointIndex%2 == 0)) {
				temp[currentPointIndex + 1] += move;
			}

			w->clear();
			w->drawCurve(temp, globalColor, ui->comboBoxCurvedType->currentIndex(), ui->comboBoxShowAddons->currentIndex());
		}
		else if (ui->toolButtonDrawRectangle->isChecked()) {
			QVector<QPoint> temp = w->getObject(currentLayer);

			for (int i = 0; i < temp.size(); i++) {
				temp[i] += move;
			}
			bool fill = (ui->checkBoxFill->isChecked() && temp.size() >= 3) ? true : false;

			w->clear();
			w->drawPolygon(temp, globalColor, fill);
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
	vW->clearObjectPoints(currentLayer);
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
	vW->clearObjectPoints(currentLayer);
	vW->setMoveActive(false);
	vW->clearList();

	ui->listWidgetLayers->clear();
	currentLayer = 0;
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
			vW->drawCurve(vW->getObject(currentLayer), globalColor, ui->comboBoxCurvedType->currentIndex(), ui->comboBoxShowAddons->currentIndex());
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

void ImageViewer::on_listWidgetLayers_currentChanged() {
	currentLayer = ui->listWidgetLayers->currentRow();
}

void ImageViewer::updateList(QString item) {
	QListWidget* l = ui->listWidgetLayers;

	currentLayer = 0;
	l->insertItem(0, item);
	l->setCurrentRow(currentLayer);
}