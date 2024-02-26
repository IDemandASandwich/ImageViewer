#pragma once
#include <QtWidgets>
class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QPainter* painter = nullptr;
	uchar* data = nullptr;

	bool drawLineActivated = false;
	QPoint drawLineBegin = QPoint(0, 0);

	bool drawPolygonActivated = false;
	bool moveActive = false;
	bool moving = false;

	//My vars
	QPoint origin;
	QVector<QPoint> object;

public:
	ViewerWidget(QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();
	bool changeSize(int width, int height);

	void setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a = 255);
	void setPixel(int x, int y, double valR, double valG, double valB, double valA = 1.);
	void setPixel(int x, int y, const QColor& color);
	bool isInside(int x, int y) { return (x >= 0 && y >= 0 && x < img->width() && y < img->height()) ? true : false; }

	//Draw functions
	void drawLine(QPoint start, QPoint end, QColor color, int algType = 0);
	void setDrawLineBegin(QPoint begin) { drawLineBegin = begin; }
	QPoint getDrawLineBegin() { return drawLineBegin; }
	void setDrawLineActivated(bool state) { drawLineActivated = state; }
	bool getDrawLineActivated() { return drawLineActivated; }

	void setDrawPolygonActivated(bool state) { drawPolygonActivated = state; }
	bool getDrawPolygonActivated() { return drawPolygonActivated; }

	//Get/Set functions
	uchar* getData() { return data; }
	void setDataPtr() { data = img->bits(); }
	void setPainter() { painter = new QPainter(img); }

	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	void addObjectPoint(QPoint point) { object.push_back(point); }
	void addObjectPoint(int x, int y) { object.push_back(QPoint(x, y)); }
	void debugObjectPoints(){ for (int i = 0; i < object.size(); i++) { qDebug() << object[i]; }  }
	void clearPoints() { object.clear(); }
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

	void clear();

	//My functions
	void DDA(QPoint start, QPoint end, QColor color);
	void Bresenham(QPoint start, QPoint end, QColor color);
	void drawCircle(QPoint center, QPoint end, QColor color);
	void drawPolygon(QVector<QPoint> points, QColor color, int algtype = 0);

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};