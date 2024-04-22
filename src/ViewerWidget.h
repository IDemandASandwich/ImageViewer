#pragma once
#include <QtWidgets>
#include "halfEdge.h"

class ViewerWidget :public QWidget {
	Q_OBJECT
private:
#pragma region 2D

	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QPainter* painter = nullptr;
	uchar* data = nullptr;

	bool drawLineActivated = false;
	QPoint drawLineBegin = QPoint(0, 0);

	bool moveActive = false;
	bool moving = false;
	bool showpoints = true;	//TODO: add on/off switch

	//My vars
	QPoint origin;
	QVector<QPoint> object;

	enum types { line, circle, polygon };

#pragma endregion
#pragma region 3D

	object3D obj;
	QVector<QVector<double>> depth;

#pragma endregion

public:
	ViewerWidget(QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

#pragma region 2D
	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();
	bool changeSize(int width, int height);

	void setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a = 255);
	void setPixel(int x, int y, double valR, double valG, double valB, double valA = 1.);
	void setPixel(int x, int y, const QColor& color);
	bool isInside(int x, int y) { return (x > 0 && y > 0 && x < img->width() && y < img->height()) ? true : false; }
	bool isInside(QPoint p) { return (p.x() > 0 && p.y() > 0 && p.x() < img->width() && p.y() < img->height()) ? true : false; }

	//Draw functions
	void drawLine(QPoint start, QPoint end, QColor color, int algType = 0, bool crop = true, bool showpoints = false);
	void setDrawLineBegin(QPoint begin) { drawLineBegin = begin; }
	QPoint getDrawLineBegin() { return drawLineBegin; }
	void setDrawLineActivated(bool state) { drawLineActivated = state; }
	bool getDrawLineActivated() { return drawLineActivated; }

	//Get/Set functions
	uchar* getData() { return data; }
	void setDataPtr() { data = img->bits(); }
	void setPainter() { painter = new QPainter(img); }

	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	void clear();

	//my get/set functions
	void addObjectPoint(QPoint point) { object.push_back(point); }
	void addObjectPoint(int x, int y) { object.push_back(QPoint(x, y)); }
	void debugObjectPoints(){ for (int i = 0; i < object.size(); i++) { qDebug() << object[i]; }  }
	void clearObjectPoints() { object.clear(); }
	QVector<QPoint> getObject() { return object; }
	int objectSize() { return object.size(); }
	QPoint getObjectPoint(int index) { return object.at(index); }
	void changeObjectPoint(int index, QPoint point) { object.replace(index, point); }

	void setMoveActive(bool state) { moveActive = state; }
	bool getMoveActive() { return moveActive; }
	void setMoving(bool state) { moving = state; }
	bool getMoving() { return moving; }
	void setOrigin(QPoint point) { origin = point; }
	QPoint getOrigin() { return origin; }

	int getClosestPointIndex(QPoint P);

	//My functions
	void DDA(QPoint start, QPoint end, QColor color);
	void Bresenham(QPoint start, QPoint end, QColor color);
	void drawCircle(QPoint center, QPoint end, QColor color);
	void drawCircle(QPoint center, int r, QColor color);
	void drawPolygon(QVector<QPoint> points, QColor color, QColor triangleColor[3] = {0}, int algtype = 0, int triangleFillType = 0);
	void drawPolygonWireframe(QVector<QPoint> points, QColor color);
	void drawType(QColor color, QColor triangleColor[3],int type, int algtype = 0, int triangleFillType = 0);

	void rotateObject(int degrees, int type, QColor color, QColor triangleColor[3], int algtype = 0, int triangleFillType = 0);
	void scaleObject(double multiplier, QColor color, QColor triangleColor[3], int type, int algtype = 0, int triangleFillType = 0);
	void scaleObject(double multiplierX, double multiplierY, QColor color, QColor triangleColor[3], int type, int algtype = 0, int triangleFillType = 0);
	void mirrorObject(int type, QColor color, QColor triangleColor[3], int algtype = 0, int triangleFillType = 0);
	void shearObjectDx(int type, QColor color, QColor triangleColor[3], double dx, int algtype = 0, int triangleFillType = 0);
	QVector<QPoint> cropCB(QPoint start, QPoint end);
	QVector<QPoint> cropSH(QVector<QPoint> V);

	void showPoints(QVector<QPoint> obj, QColor color = Qt::red);

	void scanLine(QVector<QPoint> obj,QColor color);
	void fillTriangle(QVector<QPoint> obj, QColor color, int fillType, QColor triangleColor[3]);
	void fillTriangleUp(QVector<QPoint> T, QColor color, int fillType, QColor triangleColor[3]);
	void fillTriangleDown(QVector<QPoint> T, QColor color, int fillType, QColor triangleColor[3]);
	void fillTriangleUp(QVector<QPoint> T, QPoint P,QColor color, int fillType, QColor triangleColor[3]);
	void fillTriangleDown(QVector<QPoint> T, QPoint P, QColor color, int fillType, QColor triangleColor[3]);

	QColor nearestNeighbor(int x, int y, QVector<QPoint> T, QColor triangleColor[3]);
	QColor barycentric(int x, int y, QVector<QPoint> T, QColor triangleColor[3]);

	void drawCurve(QVector<QPoint> points, QColor color, int type = 0, int show = 0);
	void drawHermitCubic(QVector<QPoint> points, QColor color, int show);
	void drawCasteljauAlg(QVector<QPoint> points, QColor color, int show);
	void drawCoonsCubic(QVector<QPoint> points, QColor color, int show);
#pragma endregion
#pragma region 3D
	
	bool saveCubeToVTK(QString filename, int length);
	bool saveUVSphereToVTK(QString filename, int radius ,int rings, int segments);

	bool loadObject(QString filename);
	bool saveObject(QString filename, bool wireframe);

	void projectObject(double zenith = 0, double azimuth = 0, int projectType = 0, int distance = 0, bool wireframe = false);
	void zBuffer(QVector<QVector<QColor>>& F, QVector<QVector<double>>& Z, QVector<QPoint> T, QVector<QVector3D> p, QColor faceColor);
	double interpolateZ(double x, double y, QVector<QPoint> T, QVector<QVector3D> p);
	void zFill(double x1, double x2, double m1, double m2, double ymin, double ymax, QVector<QVector<double>>& Z, QVector<QPoint>& T, QVector<QVector3D>& p, QColor& faceColor);

#pragma endregion

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};