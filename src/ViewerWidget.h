#pragma once
#include <QtWidgets>
#include "halfEdge.h"
#include "light.h";

enum types { line, circle, polygon, curved, rectangle };
struct object {
	object(QVector<QPoint> obj, QColor color, int layer, int type)
		:points(obj), color(color), layer(layer), type(type) {};
	object(QVector<QPoint> obj, QColor color, int layer, int type, bool fill, QColor TC[3], int fillType)
		:points(obj), color(color), layer(layer), type(type), fill(fill), fillType(fillType) {
		triangleColors[0] = TC[0]; triangleColors[1] = TC[1]; triangleColors[2] = TC[2];
	};
	object(QVector<QPoint> obj, QColor color, int layer, int type, int curvedType)
		:points(obj), color(color), layer(layer), type(type), curvedType(curvedType) {};

	QVector<QPoint> points;
	QColor color;
	QColor triangleColors[3];
	int layer, type, curvedType, fillType;
	bool fill;
};

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
	QVector<object> list;

#pragma endregion
#pragma region 3D

	object3D obj;
	QVector3D v, u, n;
	QVector<QVector<double>> Z;

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
	void pushBackObject(QVector<QPoint> obj, QColor color, int layer, int type) {
		list.push_back(object(obj, color, layer, type));
	}
	void pushBackObject(QVector<QPoint> obj, QColor color, int layer, int type, bool fill, QColor TC[3], int fillType) {
		list.push_back(object(obj, color, layer, type, fill, TC, fillType));
	}
	void pushBackObject(QVector<QPoint> obj, QColor color, int layer, int type, int curvedType) {
		list.push_back(object(obj, color, layer, type, curvedType));
	}
	void removeObject(int layer) { 
		list.removeAt(layer); 
		for (object& o : list) {
			if (o.layer > layer) {
				o.layer--;
			}
		}
	}
	void clearList() { list.clear(); }
	QVector<QPoint> getObjectPoints(int layer) { return list[layer].points; }
	object getObject(int layer) { return list[layer]; }
	int objectSize(int layer) { return list[layer].points.size(); }
	QPoint getObjectPoint(int layer, int index) { return list[layer].points.at(index); }
	void changeObjectPoint(int layer, int index, QPoint point) { list[layer].points.replace(index, point); }
	void changeObject(int layer, QVector<QPoint> obj) { list[layer].points = obj; }
	void changeObject(int layer, QVector<QPoint> obj, QColor TC[3], bool fill, int fillType) {
		list[layer].points = obj; list[layer].fill = fill; list[layer].fillType = fillType; 
		list[layer].triangleColors[0] = TC[0]; list[layer].triangleColors[1] = TC[1]; list[layer].triangleColors[2] = TC[2];
	}
	void changeObjectColor(int layer, QColor color) { list[layer].color = color; }

	void setMoveActive(bool state) { moveActive = state; }
	bool getMoveActive() { return moveActive; }
	void setMoving(bool state) { moving = state; }
	bool getMoving() { return moving; }
	void setOrigin(QPoint point) { origin = point; }
	QPoint getOrigin() { return origin; }

	int getClosestPointIndex(int layer, QPoint P);

	//My functions
	void DDA(QPoint start, QPoint end, QColor color);
	void Bresenham(QPoint start, QPoint end, QColor color);
	void drawCircle(QPoint center, QPoint end, QColor color);
	void drawCircle(QPoint center, int r, QColor color);
	void drawPolygon(QVector<QPoint> points, QColor color, bool fill, QColor triangleColor[3] = {0}, int algtype = 0, int triangleFillType = 0);
	void drawList(int algtype = 0);

	void drawPolygonWireframe(QVector<QPoint> points, QColor color);

	void rotateObject(int layer, int degrees, int type, QColor color, QColor triangleColor[3], int algtype = 0, int triangleFillType = 0);
	void scaleObject(int layer, double multiplier, QColor color, QColor triangleColor[3], int type, int algtype = 0, int triangleFillType = 0);
	void scaleObject(int layer, double multiplierX, double multiplierY, QColor color, QColor triangleColor[3], int type, int algtype = 0, int triangleFillType = 0);
	void mirrorObject(int layer, int type, QColor color, QColor triangleColor[3], int algtype = 0, int triangleFillType = 0);
	void shearObjectDx(int layer, int type, QColor color, QColor triangleColor[3], double dx, int algtype = 0, int triangleFillType = 0);
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

	void drawCurve(int layer, QVector<QPoint> points, QColor color, int type = 0, int show = 0);
	void drawHermitCubic(int layer, QVector<QPoint> points, QColor color, int show);
	void drawCasteljauAlg(QVector<QPoint> points, QColor color, int show);
	void drawCoonsCubic(QVector<QPoint> points, QColor color, int show);
#pragma endregion
#pragma region 3D
	
	bool saveCubeToVTK(QString filename, int length);
	bool saveUVSphereToVTK(QString filename, int radius ,int rings, int segments);

	bool loadObject(QString filename);
	bool saveObject(QString filename, bool wireframe);

	void projectObject(lighting primary, int lightingMethod, int cameraDistance, int lightSharpness, double zenith = 0, double azimuth = 0, int projectType = 0, int distance = 0, bool wireframe = false);
	void zBuffer(lighting primary, int lightingMethod, int cameraDistance, int lightSharpness, QVector<QVector<QColor>>& F, QVector<QPoint> T, QVector<QVector3D> p);
	void zFill(lighting primary, int lightingMethod, int cameraDistance, int lightSharpness, double x1, double x2, double m1, double m2, double ymin, double ymax, QVector<QPoint>& T, QVector<QVector3D>& p);

#pragma endregion

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};