#include   "ViewerWidget.h"

ViewerWidget::ViewerWidget(QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

#pragma region 2D

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete painter;
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	setDataPtr();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img == nullptr) {
		return true;
	}

	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}
bool ViewerWidget::changeSize(int width, int height)
{
	QSize newSize(width, height);

	if (newSize != QSize(0, 0)) {
		if (img != nullptr) {
			delete painter;
			delete img;
		}

		img = new QImage(newSize, QImage::Format_ARGB32);
		if (!img) {
			return false;
		}
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
		update();
	}

	return true;
}

void ViewerWidget::setPixel(int x, int y, uchar r, uchar g, uchar b, uchar a)
{
	r = r > 255 ? 255 : (r < 0 ? 0 : r);
	g = g > 255 ? 255 : (g < 0 ? 0 : g);
	b = b > 255 ? 255 : (b < 0 ? 0 : b);
	a = a > 255 ? 255 : (a < 0 ? 0 : a);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = b;
	data[startbyte + 1] = g;
	data[startbyte + 2] = r;
	data[startbyte + 3] = a;
}
void ViewerWidget::setPixel(int x, int y, double valR, double valG, double valB, double valA)
{
	valR = valR > 1 ? 1 : (valR < 0 ? 0 : valR);
	valG = valG > 1 ? 1 : (valG < 0 ? 0 : valG);
	valB = valB > 1 ? 1 : (valB < 0 ? 0 : valB);
	valA = valA > 1 ? 1 : (valA < 0 ? 0 : valA);

	size_t startbyte = y * img->bytesPerLine() + x * 4;
	data[startbyte] = static_cast<uchar>(255 * valB);
	data[startbyte + 1] = static_cast<uchar>(255 * valG);
	data[startbyte + 2] = static_cast<uchar>(255 * valR);
	data[startbyte + 3] = static_cast<uchar>(255 * valA);
}
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (color.isValid()) {
		size_t startbyte = y * img->bytesPerLine() + x * 4;

		data[startbyte] = color.blue();
		data[startbyte + 1] = color.green();
		data[startbyte + 2] = color.red();
		data[startbyte + 3] = color.alpha();
	}
}

//Draw functions
void ViewerWidget::drawLine(QPoint start, QPoint end, QColor color, int algType, bool crop, bool showpoints)
{
	if (crop) {
		QVector<QPoint> cropped = cropCB(start, end);

		if (cropped.isEmpty() == false) {
			if (cropped.at(0) == QPoint(-1, -1)) {
				return;
			}
			else {
				start = cropped.at(0);
				end = cropped.at(1);
			}
		}
	}

	if (showpoints) {
		showPoints(QVector<QPoint>({ start, end }));
	}

	switch (algType) {
	case 0:
		DDA(start, end, color);
		break;
	case 1:
		Bresenham(start, end, color);
		break;
	}
}
void ViewerWidget::drawCircle(QPoint center, QPoint end, QColor color) {
	int x1 = center.x();
	int y1 = center.y();
	int x2 = end.x();
	int y2 = end.y();

	int r = static_cast<int>(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)));

	int x = 0;
	int y = r;
	int p = 1 - r;
	int dvaX = 3;
	int dvaY = 2 * r - 2;

	while (x <= y) {
		if (isInside(x1 + x, y1 + y))	setPixel(x1 + x, y1 + y, color);
		if (isInside(x1 + x, y1 - y))	setPixel(x1 + x, y1 - y, color);
		if (isInside(x1 - x, y1 + y))	setPixel(x1 - x, y1 + y, color);
		if (isInside(x1 - x, y1 - y))	setPixel(x1 - x, y1 - y, color);
		if (isInside(x1 + y, y1 + x))	setPixel(x1 + y, y1 + x, color);
		if (isInside(x1 + y, y1 - x))	setPixel(x1 + y, y1 - x, color);
		if (isInside(x1 - y, y1 + x))	setPixel(x1 - y, y1 + x, color);
		if (isInside(x1 - y, y1 - x))	setPixel(x1 - y, y1 - x, color);

		if (p > 0) {
			p -= dvaY;
			y--;
			dvaY -= 2;
		}
		p += dvaX;
		dvaX += 2;
		x++;
	}

	update();
}
void ViewerWidget::drawCircle(QPoint center, int r, QColor color) {
	drawCircle(center, QPoint(center.x() + r, center.y()), color);
}
void ViewerWidget::drawPolygon(QVector<QPoint> points, QColor color, QColor triangleColor[3], int algtype, int triangleFillType)
{
	QVector<QPoint> cropped = cropSH(points);

	if (cropped.isEmpty() == false) {
		if (cropped.at(0) == QPoint(-1, -1)) {
			return;
		}
		else {
			points = cropped;
		}
	}

	if (points.size() > 3) {
		scanLine(points, color);
	}
	else if (points.size() == 3) {
		fillTriangle(points, color, triangleFillType, triangleColor);
	}
	
	for (int i = 0; i < points.size(); i++) {
		drawLine(points[i], points[(i + 1) % points.size()], color, algtype, false, false);
	}
}
void ViewerWidget::drawPolygonWireframe(QVector<QPoint> points, QColor color)
{
	QVector<QPoint> cropped = cropSH(points);

	if (cropped.isEmpty() == false) {
		if (cropped.at(0) == QPoint(-1, -1)) {
			return;
		}
		else {
			points = cropped;
		}
	}

	for (int i = 0; i < points.size(); i++) {
		drawLine(points[i], points[(i + 1) % points.size()], color, 0, false, false);
	}
}
void ViewerWidget::drawType(QColor color, QColor triangleColor[3], int type, int algtype, int triangleFillType) {
	enum types { line, circle, polygon };

	clear();

	switch (type) {
	case line:
		drawLine(object.at(0), object.at(1), color, algtype);
		break;
	case circle:
		drawCircle(object.at(0), object.at(1), color);
		break;
	case polygon:
		drawPolygon(object, color, triangleColor, algtype, triangleFillType);
		break;
	default:
		break;
	}
}

void ViewerWidget::showPoints(QVector<QPoint> obj, QColor color) {
	for (const QPoint p : obj) {
		drawCircle(p, 3, color);
	}
}
int ViewerWidget::getClosestPointIndex(QPoint P) {
	int n = 0;
	double d1 = abs(object[0].y() - P.y()) + abs(object[0].x() - P.x());

	for (qsizetype i = 1; i < object.size(); i++) {
		double d2 = abs(object[i].y() - P.y()) + abs(object[i].x() - P.x());

		if (d2 < d1) {
			d1 = d2;
			n = i;
		}
	}

	return n;
}

void ViewerWidget::clear()
{
	img->fill(Qt::white);
	update();
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}

//My functions
void ViewerWidget::DDA(QPoint start, QPoint end, QColor color) {
	int x1 = start.x();
	int y1 = start.y();
	int x2 = end.x();
	int y2 = end.y();

	double m = static_cast<double>(y2 - y1) / static_cast<double>(x2 - x1);

	if (abs(m) <= 1) {
		if (x1 > x2) {
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		double y = y1;
		for (int x = x1; x <= x2; x++) {
			setPixel(x, static_cast<int>(y), color);
			y += m;
		}
	}
	else {
		if (y1 > y2) {
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		double x = x1;
		for (int y = y1; y <= y2; y++) {
			setPixel(static_cast<int>(x), y, color);
			x += 1 / m;
		}
	}

	update();
}
void ViewerWidget::Bresenham(QPoint start, QPoint end, QColor color) {
	int x1 = start.x();
	int y1 = start.y();
	int x2 = end.x();
	int y2 = end.y();

	double m = static_cast<double>(y2 - y1) / static_cast<double>(x2 - x1);

	if (abs(m) <= 1) {
		if (x1 > x2) {
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		int dx = x2 - x1;
		int dy = y2 - y1;
		int k1 = 2 * dy;
		int x = x1;
		int y = y1;

		if (m > 0 && m < 1) {
			int k2 = 2 * (dy - dx);
			int p = 2 * dy - dx;
			
			while (x <= x2) {
				setPixel(x, y, color);
				x++;
				if (p > 0) {
					y++;
					p += k2;
				}
				else {
					p += k1;
				}
			}
		}
		else {
			int k2 = 2 * (dy + dx);
			int p = 2 * dy + dx;

			while (x <= x2) {
				setPixel(x, y, color);
				x++;
				if (p < 0) {
					y--;
					p += k2;
				}
				else {
					p += k1;
				}
			}
		}
	}
	else {
		if (y1 > y2) {
			std::swap(x1, x2);
			std::swap(y1, y2);
		}

		int dx = x2 - x1;
		int dy = y2 - y1;
		int k1 = 2 * dx;
		int x = x1;
		int y = y1;

		if (m > 1) {
			int k2 = 2 * (dx - dy);
			int p = 2 * dx - dy;

			while (y <= y2) {
				setPixel(x, y, color);
				y++;
				if (p > 0) {
					x++;
					p += k2;
				}
				else {
					p += k1;
				}
			}
		}
		else {
			int k2 = 2 * (dx + dy);
			int p = 2 * dx + dy;

			while (y <= y2) {
				setPixel(x, y, color);
				y++;
				if (p < 0) {
					x--;
					p += k2;
				}
				else {
					p += k1;
				}
			}
		}
	}

	update();
}

void ViewerWidget::rotateObject(int degrees, int type, QColor color, QColor triangleColor[3], int algtype, int triangleFillType) {
	double theta = ((double)degrees / (double)180) * M_PI;
	double cx = object.at(0).x();
	double cy = object.at(0).y();

	for (qsizetype i = 1; i < object.size(); i++) {
		double x = cos(theta) * (object.at(i).x() - cx) + sin(theta) * (object.at(i).y() - cy) + cx;
		double y = -sin(theta) * (object.at(i).x() - cx) + cos(theta) * (object.at(i).y() - cy) + cy;

		QPoint p(x, y);
		object.replace(i, p);
	}

	drawType(color, triangleColor, type, algtype, triangleFillType);
}
void ViewerWidget::scaleObject(double multiplier, QColor color, QColor triangleColor[3], int type, int algtype, int triangleFillType) {
	double cx = object.at(0).x();
	double cy = object.at(0).y();

	for (qsizetype i = 1; i < object.size(); i++) {
		QPoint p = (object.at(i) - object.at(0)) * multiplier + object.at(0);
		object.replace(i, p);
	}

	drawType(color, triangleColor, type, algtype, triangleFillType);
}
void ViewerWidget::scaleObject(double multiplierX, double multiplierY, QColor color, QColor triangleColor[3], int type, int algtype, int triangleFillType) {
	double cx = object.at(0).x();
	double cy = object.at(0).y();

	for (qsizetype i = 1; i < object.size(); i++) {
		double x = (object.at(i).x() - cx) * multiplierX + cx;
		double y = (object.at(i).y() - cy) * multiplierY + cy;
		QPoint p(x, y);
		object.replace(i, p);
	}

	drawType(color, triangleColor, type, algtype, triangleFillType);
}
void ViewerWidget::mirrorObject(int type, QColor color, QColor triangleColor[3], int algtype, int triangleFillType) {
	QPoint n(object.at(1).y() - object.at(0).y(), object.at(0).x() - object.at(1).x());
	double a = object.at(1).y() - object.at(0).y();
	double b = object.at(0).x() - object.at(1).x();
	double c = -a * object.at(0).x() - b * object.at(0).y();

	for (qsizetype i = 1; i < object.size(); i++) {
		double x = object.at(i).x();
		double y = object.at(i).y();
		double d = (a * x + b * y + c) / (pow(a, 2) + pow(b, 2));

		object.replace(i, QPoint(x - 2 * a * d, y - 2 * b * d));
	}

	drawType(color, triangleColor, type, algtype, triangleFillType);
}
void ViewerWidget::shearObjectDx(int type, QColor color, QColor triangleColor[3], double dx, int algtype, int triangleFillType) {
	for (qsizetype i = 1; i < object.size(); i++) {
		object.replace(i, QPoint(object.at(i).x() + dx * object.at(i).y(), object.at(i).y()));
	}
	
	drawType(color, triangleColor, type, algtype, triangleFillType);
}

QVector<QPoint> ViewerWidget::cropCB(QPoint start, QPoint end) {
	if (isInside(start) && isInside(end)) { return QVector<QPoint>(); }
	else if (isInside(start) == false && isInside(end) == false) { return QVector<QPoint>({ QPoint(-1,-1) }); }

	double tl = 0, tu = 1;
	QVector2D d(end.x() - start.x(), end.y() - start.y());
	QList<QPoint> E = { QPoint(0,0), QPoint(0,img->height()), QPoint(img->width(),img->height()), QPoint(img->width(),0) };
	QPoint p0, p1;

	for (qsizetype i = 0; i < E.size(); i++) {
		QPoint Ei = E.at(i);
		QPoint En = E.at((i + 1) % E.size());
		QVector2D n(En.y() - Ei.y(), Ei.x() - En.x());
		QVector2D origin(img->width() / 2, img->height() / 2);
		QVector2D w(start.x() - Ei.x(), start.y() - Ei.y());
		
		double dn = QVector2D::dotProduct(d, n);
		double wn = QVector2D::dotProduct(w, n);

		if (dn != 0) {
			double t = -(wn / dn);
			if (dn > 0 && t <= 1) {
				tl = std::max(t, tl);
			}
			else if (dn < 0 && t >= 0) {
				tu = std::min(t, tu);
			}
		}
	}

	if (tl == 0 && tu == 1) { 
		return QVector<QPoint>();
	}
	else if (tl < tu) {
		p0 = start + (end - start) * tl;
		p1 = start + (end - start) * tu;
	}

	QVector<QPoint> cropped = { p0, p1 };

	return cropped;
}
QVector<QPoint> ViewerWidget::cropSH(QVector<QPoint> V) {
	int n = 0;
	for (auto v : V) { if (isInside(v)) { n++; } }
	if (n == V.size()) { return QVector<QPoint>(); }
	
	QVector<QPoint> W;
	QVector<QPoint> Vtemp = V;
	QPoint S;

	double x[4] = { 0., 0., static_cast<double>(-(img->width() - 1)), static_cast<double>(-(img->height() - 1)) };

	for (int i = 0; i < 4; i++) {
		if (Vtemp.isEmpty() == false)
			S = Vtemp.last();

		double xmin = x[i];

		for (int j = 0; j < Vtemp.size(); j++) {
			QPoint P = Vtemp.at(j);
			if (P.x() >= xmin) {
				if (S.x() >= xmin) {
					// Both points are inside or on the boundary
					W.append(P);
				}
				else {
					// P is inside, S is outside
					double t = (xmin - S.x()) / (P.x() - S.x());
					QPoint Pi(xmin, S.y() + t * (P.y() - S.y()));
					W.append(Pi);
					W.append(P);
				}
			}
			else {
				if (S.x() >= xmin) {
					// P is outside, S is inside or on the boundary
					double t = (xmin - S.x()) / (P.x() - S.x());
					QPoint Pi(xmin, S.y() + t * (P.y() - S.y()));
					W.append(Pi);
				}
			}
			S = P;
		}

		Vtemp = W;

		if(Vtemp.isEmpty() == false){
			for (QPoint& p : Vtemp) {
				p = QPoint(p.y(), -p.x());
			}
		}
		W.clear();
	}
	
	if (Vtemp.isEmpty()) {
		return QVector<QPoint>({ QPoint(-1,-1) });
	}

	return Vtemp;
}

void ViewerWidget::scanLine(QVector<QPoint> obj, QColor color) {
	struct edge{
		QPoint start;
		QPoint end;
		double m;
		edge():m(0) {};
		edge(QPoint s, QPoint e, double m = 0) :start(s), end(e), m(m) {};
	};
	struct info {
		int dy;
		double x;
		double w;
		info() { dy = 0; x = 0; w = 0; };
		info(int dy, double x, double w) :dy(dy), x(x), w(w) {};
	};
	QVector<edge> E;

	//setup edges 
	for (qsizetype i = 0; i < obj.size(); i++) {
		QPoint p1 = obj[i];
		QPoint p2 = obj[(i + 1) % obj.size()];
		double slope;

		if (p1.y() >= p2.y()) {
			std::swap(p1, p2);
		}

		if (p1.x() != p2.x()) {
			slope = static_cast<double>(p2.y() - p1.y()) / static_cast<double>(p2.x() - p1.x());
		}
		else {
			slope = -DBL_MAX;
		}

		if (slope != 0) {
			E.append(edge(p1, p2, slope));
		}
	}

	std::sort(E.begin(), E.end(), [](const edge& a, const edge& b) { return a.start.y() < b.start.y(); });
	//end setup edges

	int ymin = E.first().start.y();
	int ymax = E.last().end.y();

	QVector<QList<info>> TH;
	TH.resize(ymax - ymin);

	for (int i = 0; i < ymax - ymin; i++) {
		for (const edge& e : E) {
			if (i == (e.start.y() - ymin)) {
				TH[i].append(info(e.end.y() - e.start.y(), static_cast<double>(e.start.x()), 1. / e.m));
			}
		}
	}

	QList<info> ZAH;
	int y = ymin;

	for (qsizetype i = 0; i < TH.size(); i++) {
		if (TH.at(i).isEmpty() == false) {
			ZAH.append(TH.at(i));
			std::sort(ZAH.begin(), ZAH.end(), [](const info& a, const info& b) {return a.x < b.x; });
		}

		for (qsizetype j = 0; j < ZAH.size() - 1; j++) {
			if (ZAH.at(j).x != ZAH.at(j + 1).x) {
				drawLine(QPoint(static_cast<int>(round(ZAH.at(j).x)), y), QPoint(static_cast<int>(round(ZAH.at(j + 1).x)), y), color, 0);
			}
		}

		for (qsizetype j = ZAH.size() - 1; j >= 0; --j) {
			if (ZAH.at(j).dy == 0) {
				ZAH.removeAt(j);
			}
		}

		for (qsizetype j = 0; j < ZAH.size(); j++) {
			ZAH[j].dy -= 1;
			ZAH[j].x += ZAH[j].w;
		}

		y++;
	}
}
void ViewerWidget::fillTriangle(QVector<QPoint> obj, QColor color, int fillType, QColor triangleColor[3]) {
	QVector<QPoint> T = obj;

	std::sort(T.begin(), T.end(), [](const QPoint& a, const QPoint& b) {
		if (a.y() != b.y())
			return a.y() < b.y();
		else
			return a.x() < b.x();
	});

	if (T.at(0).y() == T.at(1).y()) {
		fillTriangleDown(T, color, fillType, triangleColor);
	}
	else if (T.at(1).y() == T.at(2).y()) {
		fillTriangleUp(T, color, fillType, triangleColor);
	}
	else {
		double m = static_cast<double>(T.at(2).y() - T.at(0).y()) / static_cast<double>(T.at(2).x() - T.at(0).x());

		QPoint P(((T.at(1).y() - T.at(0).y()) / m) + T.at(0).x(), T.at(1).y());

		fillTriangleDown(T, P, color, fillType, triangleColor);
		fillTriangleUp(T, P, color, fillType, triangleColor);
	}
}
void ViewerWidget::fillTriangleUp(QVector<QPoint> T, QColor color, int fillType, QColor triangleColor[3]) {

	double m1 = static_cast<double>(T.at(1).y() - T.at(0).y()) / static_cast<double>(T.at(1).x() - T.at(0).x());
	double m2 = static_cast<double>(T.at(2).y() - T.at(0).y()) / static_cast<double>(T.at(2).x() - T.at(0).x());

	int y = T.at(0).y();
	int ymax = T.at(1).y();
	double x1 = static_cast<double>(T.at(0).x());
	double x2 = x1;

	for (y; y < ymax; y++) {
		if (x1 != x2) {
			for (double x = ceil(x1 - 1); x < ceil(x2 + 1); x++) {
				if (fillType == 1) {
					color = nearestNeighbor(x, y, T, triangleColor);
				}
				else if (fillType == 2) {
					color = barycentric(x, y, T, triangleColor);
				}

				setPixel(static_cast<int>(x), y, color);
			}
		}
		x1 += 1./m1;
		x2 += 1./m2;
	}
}
void ViewerWidget::fillTriangleDown(QVector<QPoint> T, QColor color, int fillType, QColor triangleColor[3]) {
	double m1 = static_cast<double>(T.at(2).y() - T.at(0).y()) / static_cast<double>(T.at(2).x() - T.at(0).x());
	double m2 = static_cast<double>(T.at(2).y() - T.at(1).y()) / static_cast<double>(T.at(2).x() - T.at(1).x());

	int y = T.at(0).y();
	int ymax = T.at(2).y();
	double x1 = static_cast<double>(T.at(0).x());
	double x2 = static_cast<double>(T.at(1).x());

	for (y; y < ymax; y++) {
		if (x1 != x2) {
			for (double x = ceil(x1 - 1); x < ceil(x2 + 1); x++) {
				if (fillType == 1) {
					color = nearestNeighbor(x, y, T, triangleColor);
				}
				else if (fillType == 2) {
					color = barycentric(x, y, T, triangleColor);
				}

				setPixel(static_cast<int>(x), y, color);
			}
		}
		x1 += 1. / m1;
		x2 += 1. / m2;
	}
}
void ViewerWidget::fillTriangleUp(QVector<QPoint> T, QPoint P, QColor color, int fillType, QColor triangleColor[3]) {
	QVector<QPoint> Tt = T;
	if (Tt.at(1).x() < P.x()) {
		//T(T0,T1,T2) -> T(T0,T1,P)
		Tt[2] = P;
	}
	else {
		//T(T0,T1,T2) -> T(T0,P,T1)
		Tt[2] = Tt[1];
		Tt[1] = P;
	}

	double m1 = static_cast<double>(Tt.at(1).y() - Tt.at(0).y()) / static_cast<double>(Tt.at(1).x() - Tt.at(0).x());
	double m2 = static_cast<double>(Tt.at(2).y() - Tt.at(0).y()) / static_cast<double>(Tt.at(2).x() - Tt.at(0).x());

	int y = Tt.at(0).y();
	int ymax = Tt.at(1).y();
	double x1 = static_cast<double>(Tt.at(0).x());
	double x2 = x1;

	for (y; y < ymax; y++) {
		if (x1 != x2) {
			for (double x = ceil(x1 - 1); x < ceil(x2 + 1); x++) {
				if (fillType == 1) {
					color = nearestNeighbor(x, y, T, triangleColor);
				}
				else if (fillType == 2) {
					color = barycentric(x, y, T, triangleColor);
				}

				setPixel(static_cast<int>(x), y, color);
			}
		}
		x1 += 1. / m1;
		x2 += 1. / m2;
	}
}
void ViewerWidget::fillTriangleDown(QVector<QPoint> T, QPoint P, QColor color, int fillType, QColor triangleColor[3]) {
	QVector<QPoint> Tt = T;
	if (Tt.at(1).x() < P.x()) {
		//T(T0,T1,T2) -> T(T1,P,T2)
		Tt[0] = Tt[1];
		Tt[1] = P;
	}
	else {
		//T(T0,T1,T2) -> T(P,T1,T2)
		Tt[0] = P;
	}

	double m1 = static_cast<double>(Tt.at(2).y() - Tt.at(0).y()) / static_cast<double>(Tt.at(2).x() - Tt.at(0).x());
	double m2 = static_cast<double>(Tt.at(2).y() - Tt.at(1).y()) / static_cast<double>(Tt.at(2).x() - Tt.at(1).x());

	int y = Tt.at(0).y();
	int ymax = Tt.at(2).y();
	double x1 = static_cast<double>(Tt.at(0).x());
	double x2 = static_cast<double>(Tt.at(1).x());

	for (y; y < ymax; y++) {
		if (x1 != x2) {
			for (double x = ceil(x1 - 1); x < ceil(x2 + 1); x++) {
				if (fillType == 1) {
					color = nearestNeighbor(x, y, T, triangleColor);
				}
				else if (fillType == 2) {
					color = barycentric(x, y, T, triangleColor);
				}

				setPixel(static_cast<int>(x), y, color);
			}
		}
		x1 += 1. / m1;
		x2 += 1. / m2;
	}
}

QColor ViewerWidget::nearestNeighbor(int x, int y, QVector<QPoint> T, QColor triangleColor[3]) {
	double d0 = sqrt(pow(T[0].x() - x, 2) + pow(T[0].y() - y, 2));
	double d1 = sqrt(pow(T[1].x() - x, 2) + pow(T[1].y() - y, 2));
	double d2 = sqrt(pow(T[2].x() - x, 2) + pow(T[2].y() - y, 2));

	if (d0 < d1 && d0 < d2) {
		return triangleColor[0];
	}
	else if (d1 < d0 && d1 < d2) {
		return triangleColor[1];
	}
	else {
		return triangleColor[2];
	}
}
QColor ViewerWidget::barycentric(int x, int y, QVector<QPoint> T, QColor triangleColor[3]) {
	double A = static_cast<double>(abs((T[1].x() - T[0].x()) * (T[2].y() - T[0].y()) - (T[1].y() - T[0].y()) * (T[2].x() - T[0].x())));
	double A0 = static_cast<double>(abs((T[1].x() - x) * (T[2].y() - y) - (T[1].y() - y) * (T[2].x() - x)));
	double A1 = static_cast<double>(abs((T[0].x() - x) * (T[2].y() - y) - (T[0].y() - y) * (T[2].x() - x)));
	double lambda0 = A0 / A;
	double lambda1 = A1 / A;
	double lambda2 = 1 - lambda0 - lambda1;

	int r = lambda0 * triangleColor[0].red() + lambda1 * triangleColor[1].red() + lambda2 * triangleColor[2].red();
	int g = lambda0 * triangleColor[0].green() + lambda1 * triangleColor[1].green() + lambda2 * triangleColor[2].green();
	int b = lambda0 * triangleColor[0].blue() + lambda1 * triangleColor[1].blue() + lambda2 * triangleColor[2].blue();

	return QColor(r, g, b);
}

void ViewerWidget::drawCurve(QVector<QPoint> points, QColor color, int type, int show) {
	enum types { hermit, bezier, coons };
	
	switch (type){
	case hermit:
		drawHermitCubic(points, color, show);
		break;
	case bezier:
		drawCasteljauAlg(points, color, show);
		break;
	case coons:
		drawCoonsCubic(points, color, show);
		break;
	}
}
void ViewerWidget::drawHermitCubic(QVector<QPoint> points, QColor color, int show) {
	enum addons { lines_points, lines, point, none };
	double dt = 0.05;

	if (moveActive == false) {
		for (qsizetype i = points.size() - 1; i >= 0; i--) {	//point1, vectorEnd1, point2, vectorEnd2...
			QPoint temp(points[i].x(), points[i].y() - 100);
			object.insert(i + 1, temp);
		}

		points = object;
	}

	for (qsizetype i = 0; i < points.size(); i += 2) {
		bool show_points = (show == lines_points || show == point);
		QColor col = Qt::white;

		if (show == lines || show == lines_points)
			col = Qt::red;

		drawLine(points[i], points[i + 1], col, 0, true, show_points);
	}

	for (qsizetype i = 2; i < points.size(); i += 2) {
		QPoint Q0 = points[i - 2];
		double t = dt;

		while(t < 1.0){
			double F0 = 2 * pow(t, 3) - 3 * pow(t, 2) + 1;
			double F1 = -2 * pow(t, 3) + 3 * pow(t, 2);
			double F2 = pow(t, 3) - 2 * pow(t, 2) + t;
			double F3 = pow(t, 3) - pow(t, 2);

			QPoint P0 = points[i - 2];
			QPoint T0 = P0 - points[i - 1];
			QPoint P1 = points[i];
			QPoint T1 = P1 - points[i + 1];

			QPoint Q1(P0 * F0 + P1 * F1 + T0 * F2 + T1 * F3);
			drawLine(Q0, Q1, color);
			Q0 = Q1;
			t += dt;
		}

		drawLine(Q0, points[i], color);
	}
}
void ViewerWidget::drawCasteljauAlg(QVector<QPoint> points, QColor color, int show) {
	enum addons { lines_points, lines, point, none };

	qsizetype n = points.size();
	QVector<QVector<QPoint>> P(n);
	double dt = 0.05;
	double t = dt;
	QPoint Q0 = points[0];
	QPoint Q1;

	for (qsizetype i = 0; i < n; i++) {
		P[i].resize(n - i);
		P[0][i] = points[i];
	}
	
	while (t < 1) {
		for (qsizetype i = 1; i < n; i++) {
			for (qsizetype j = 0; j < n - i; j++) {
				P[i][j] = (1 - t) * P[i - 1][j] + t * P[i - 1][j + 1];
			}
		}
		
		Q1 = P[n - 1][0];
		drawLine(Q0, Q1, color);
		Q0 = Q1;
		t += dt;
	}

	if(show != none)
		showPoints(points);

	drawLine(Q0, points[n - 1], color);
}
void ViewerWidget::drawCoonsCubic(QVector<QPoint> points, QColor color, int show) {
	enum addons { lines_points, lines, point, none };
	double dt = 0.05;
	double t = 0;
	QPoint Q0, Q1;
	qsizetype n = points.size();

	auto B0 = [](double t) { return (-(1 / 6.) * pow(t, 3) + 0.5 * pow(t, 2) - 0.5 * t + 1 / 6.); };
	auto B1 = [](double t) { return(0.5 * pow(t, 3) - pow(t, 2) + 2 / 3.); };
	auto B2 = [](double t) { return(-0.5 * pow(t, 3) + 0.5 * pow(t, 2) + 0.5 * t + 1 / 6.); };
	auto B3 = [](double t) { return((1 / 6.) * pow(t, 3)); };

	for (qsizetype i = 3; i < n; i++) {
		t = 0;
		Q0 = points[i - 3] * B0(0) + points[i - 2] * B1(0) + points[i - 1] * B2(0) + points[i] * B3(0);

		while (t < 1) {
			t += dt;
			Q1 = points[i - 3] * B0(t) + points[i - 2] * B1(t) + points[i - 1] * B2(t) + points[i] * B3(t);
			drawLine(Q0, Q1, color);
			Q0 = Q1;
		}
	}

	if (show != none)
		showPoints(points);
}

#pragma endregion
#pragma region 3D

bool ViewerWidget::saveCubeToVTK(QString filename, int l) {
	QFile file(filename);

	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);

		double l2 = static_cast<double>(l) / 2.;

		out << "# vtk DataFile Version 3.0\nvtk output\nASCII\nDATASET POLYDATA\nPOINTS 8 float\n";
		out << Vertex(-l2, -l2, -l2) << "\n";
		out << Vertex(l2, -l2, -l2) << "\n";
		out << Vertex(l2, l2, -l2) << "\n";
		out << Vertex(-l2, l2, -l2) << "\n";
		out << Vertex(-l2, -l2, l2) << "\n";
		out << Vertex(l2, -l2, l2) << "\n";
		out << Vertex(l2, l2, l2) << "\n";
		out << Vertex(-l2, l2, l2) << "\n";
		

		out << "POLYGONS 12 48\n";
		out << "3 0 1 4\n";
		out << "3 1 5 4\n";

		out << "3 1 2 5\n";
		out << "3 2 6 5\n";

		out << "3 2 3 6\n";
		out << "3 3 7 6\n";

		out << "3 3 0 7\n";
		out << "3 0 4 7\n";

		out << "3 1 0 2\n";
		out << "3 0 3 2\n";

		out << "3 4 5 7\n";
		out << "3 5 6 7\n";

		file.close();
		return true;
	}
	else {
		qDebug() << "error opening file in createVTK_Cube!";
		return false;
	}
}
bool ViewerWidget::saveUVSphereToVTK(QString filename, int r, int rings, int segments) {
	QFile file(filename);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "error opening file in createVTK_Sphere!";
		return false;
	}

	QTextStream out(&file);

	double thetaInc = M_PI / static_cast<double>(rings + 1);
	double gammaInc = 2 * M_PI / static_cast<double>(segments);
	double theta = -(M_PI / 2.) + thetaInc;
	double gamma = gammaInc;

	//vertices
	int numPoints = rings * segments + 2;
	out << "# vtk DataFile Version 3.0\nvtk output\nASCII\nDATASET POLYDATA\nPOINTS " << numPoints << " float\n";
	out << "0 0 " << -r << "\n";
	for (int i = 1; i <= rings; i++) {
		double gamma = gammaInc; // Reset gamma for each ring
		for (int j = 0; j < segments; j++) {
			out << r * cos(theta) * cos(gamma) << " " << r * cos(theta) * sin(gamma) << " " << r * sin(theta) << "\n";
			gamma += gammaInc;
		}
		theta += thetaInc;
	}
	out << "0 0 " << r << "\n";

	//polygons
	//edge case where there is only one ring
	int n = 0;
	int faces = 0;
	if (rings == 1) {
		faces = segments * (rings + 1);
	}
	else {
		faces = 2*segments*rings;
	}
	out << "POLYGONS "<< faces << " " << 4*faces << "\n";
	//lower triangles
	for (int i = 1; i < segments; i++) {
		out << "3 0 " << i << " " << i + 1 << "\n";
	}
	out << "3 0 " << segments << " " << 1 << "\n";
	//middle part
	if (rings != 1) {
		for (int i = 0; i < rings-1; i++) {
			for (int j = 1 + i * segments; j < segments * (i + 1); j++) {
				//first is the bottom one, second is the top one
				out << "3 " << j << " " << j + segments << " " << (j + 1 + segments) << "\n";
				out << "3 " << j << " " << (j + 1 + segments) << " " << j + 1 << "\n";
			}
			
			//edges slicing the last surface
			out << "3 " << segments + segments * i << " " << 2 * segments + segments * i << " " << 1 + segments + segments * i << "\n";
			out << "3 " << segments + segments * i << " " << 1 + segments + segments * i << " " << 1 + segments * i << "\n";
		}
	}
	//upper triangles
	for (int i = numPoints - segments - 1; i < numPoints-2; i++) {
		out << "3 " << numPoints - 1 << " " << i + 1 << " " << i << "\n";
	}
	out << "3 " << numPoints - 1 << " " << numPoints - 1 - segments << " " << numPoints - 2 << "\n";

	file.close();
	return true;
}

bool ViewerWidget::loadObject(QString filename) {
	QVector<Vertex>& vertices = obj.vertices;
	QVector<H_edge>& edges = obj.edges;
	QVector<Face>& faces = obj.faces;
	Z = QVector<QVector<double>>(width(), QVector<double>(height(), -DBL_MAX));

	if (!vertices.isEmpty()) { vertices.clear(); }
	if (!edges.isEmpty()) { edges.clear(); }
	if (!faces.isEmpty()) { faces.clear(); }

	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Error loading cube! - Couldn't find file " << filename;
		return false;
	}

	QTextStream in(&file);

	// Read and process header
	QString line = in.readLine();
	if (!line.startsWith("# vtk")) {
		qDebug() << "Invalid VTK file format";
		return false;
	}

	// Skip header
	while (!line.startsWith("POINTS")) {
		line = in.readLine();
	}
	
	QStringList parts = line.split(" ");
	int n = parts[1].toInt();
	for (int i = 0; i < n; i++) {
		line = in.readLine();
		parts = line.split(" ");
		if (parts.size() != 3) {
			qDebug() << "Invalid point data";
			return false;
		}
		double x = parts[0].toDouble();
		double y = parts[1].toDouble();
		double z = parts[2].toDouble();
		vertices.append(Vertex(x, y, z));
	}
	// Skip polygons count
	line = in.readLine();
	parts = line.split(" ");
	n = parts[2].toInt() - parts[1].toInt();
	edges.resize(n);
	n = parts[1].toInt();
	faces.resize(n);

	// Read polygons
	qsizetype m = 0;
	for (int i = 0; i < n; i++) {
		line = in.readLine().trimmed();
		if (line.isEmpty()) {
			continue;
		}
		parts = line.split(" ");
		parts.removeFirst();
		if (parts.isEmpty()) {
			qDebug() << "Invalid polygon data.";
			return false;
		}
		int v0 = parts[0].toInt();
		int v1 = parts[1].toInt();
		int v2 = parts[2].toInt();

		//create edges
		edges[m].set(&vertices[v0], &faces[i], &edges[m + 1], &edges[m + 2]);
		edges[m + 1].set(&vertices[v1], &faces[i], &edges[m + 2], &edges[m]);
		edges[m + 2].set(&vertices[v2], &faces[i], &edges[m], &edges[m + 1]);

		faces[i].edge = &edges[m];
		vertices[v0].edge = &edges[m];
		vertices[v1].edge = &edges[m + 1];
		vertices[v2].edge = &edges[m + 2];
		m += 3;
	}
	//assign pairs
	for (H_edge& e1 : edges) {
		if (e1.pair == nullptr) {
			bool pairFound = false;
			for (H_edge& e2 : edges) {
				if (&e1 != &e2) {
					if (e1.vert_origin == e2.edge_next->vert_origin && e1.edge_next->vert_origin == e2.vert_origin) {
						e1.pair = &e2;
						e2.pair = &e1;
						pairFound = true;
						break;
					}
				}
			}
		}
	}
	file.close();

	return true;
}
bool ViewerWidget::saveObject(QString filename, bool wireframe) {
	QVector<Vertex>& vertices = obj.vertices;
	QVector<H_edge>& edges = obj.edges;
	QVector<Face>& faces = obj.faces;

	QFile file(filename);

	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);

		out << "# vtk DataFile Version 3.0\nvtk output\nASCII\nDATASET POLYDATA\nPOINTS " << vertices.size() << " float\n";
		for (Vertex& vertex : vertices) {
			out << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
		}

		if (wireframe) {
			out << "LINES " << edges.size() / 2 << " " << 3 * edges.size() / 2 << "\n";
			for (H_edge& edge : edges) {
				int v1 = vertices.indexOf(*edge.vert_origin);
				int v2 = vertices.indexOf(*edge.edge_next->vert_origin);

				if(v1 < v2)
					out << "2 " << v1 << " " << v2 << "\n";
			}
		}

		if (!wireframe) {
			out << "POLYGONS " << faces.size() << " " << 4 * faces.size() << "\n";
			for (Face& face : faces) {
				out << "3 " << vertices.indexOf(*face.edge->vert_origin) << " " << vertices.indexOf(*face.edge->edge_next->vert_origin) << " " << vertices.indexOf(*face.edge->edge_prev->vert_origin) << "\n";
			}
		}

		file.close();
		return true;
	}
	else {
		qDebug() << "error opening file in saveObject!";
		return false;
	}
}

void ViewerWidget::projectObject(lighting primary, int lightingMethod, int cameraDistance, int lightSharpness, double zenith, double azimuth, int projectType, int d, bool wireframe) {
	enum projecttype{ parallel, perspective };
	// Check if zenith or azimuth has changed
	static double lastZenith = -1, lastAzimuth = -1;
	if (zenith != lastZenith || azimuth != lastAzimuth) {
		n = QVector3D(sin(zenith) * sin(azimuth), sin(zenith) * cos(azimuth), cos(zenith));
		u = QVector3D(sin(zenith + M_PI_2) * sin(azimuth), sin(zenith + M_PI_2) * cos(azimuth), cos(zenith + M_PI_2));
		v = QVector3D::crossProduct(n, u);
		lastZenith = zenith;
		lastAzimuth = azimuth;
	}

	QVector<QVector<QColor>> F;//(width(), QVector<QColor>(height(), Qt::white));
	Z = QVector<QVector<double>>(width(), QVector<double>(height(), -DBL_MAX));

	int cX = width() / 2.;
	int cY = height() / 2.;

	clear();
	for (Face& f : obj.faces) {
		// Transform
		QVector3D p1o(f.edge->vert_origin->x, f.edge->vert_origin->y, f.edge->vert_origin->z);
		QVector3D p2o(f.edge->edge_next->vert_origin->x, f.edge->edge_next->vert_origin->y, f.edge->edge_next->vert_origin->z);
		QVector3D p3o(f.edge->edge_prev->vert_origin->x, f.edge->edge_prev->vert_origin->y, f.edge->edge_prev->vert_origin->z);

		QVector3D p1(QVector3D::dotProduct(p1o, v), QVector3D::dotProduct(p1o, u), QVector3D::dotProduct(p1o, n));
		QVector3D p2(QVector3D::dotProduct(p2o, v), QVector3D::dotProduct(p2o, u), QVector3D::dotProduct(p2o, n));
		QVector3D p3(QVector3D::dotProduct(p3o, v), QVector3D::dotProduct(p3o, u), QVector3D::dotProduct(p3o, n));

		// Projected coords
		QVector<QPoint> T;

		if (projectType == parallel) {
			T = { QPoint(p1.x()  + cX, p1.y() + cY), 
				QPoint(p2.x() + cX, p2.y() + cY), 
				QPoint(p3.x() + cX, p3.y() + cY) };
		}
		else {
			T = { QPoint(d * p1.x() / (d - p1.z()) + cX, d * p1.y() / (d - p1.z()) + cY),
				QPoint(d* p2.x() / (d - p2.z()) + cX, d* p2.y() / (d - p2.z()) + cY),
				QPoint(d* p3.x() / (d - p3.z()) + cX, d* p3.y() / (d - p3.z()) + cY) };
		}

		if (wireframe) {
			drawPolygonWireframe(T, Qt::black);
		}
		else {
			zBuffer(primary, lightingMethod, cameraDistance, lightSharpness, F, T, { p1, p2, p3 });
		}	
	}
	update();
}
void ViewerWidget::zBuffer(lighting primary, int lightingMethod, int cameraDistance, int lightSharpness, QVector<QVector<QColor>>& F, QVector<QPoint> T, QVector<QVector3D> p) {
	std::sort(T.begin(), T.end(), [](const QPoint& a, const QPoint& b) {
		if (a.y() != b.y())
			return a.y() < b.y();
		else
			return a.x() < b.x();
		});
	std::sort(p.begin(), p.end(), [](const QVector3D& a, const QVector3D& b) {
		if (a.y() != b.y())
			return a.y() < b.y();
		else if (a.x() != b.x())
			return a.x() < b.x();
		else
			return a.z() < b.z();

		});

	QPoint p0 = T.at(0);
	QPoint p1 = T.at(1);
	QPoint p2 = T.at(2);

	auto slope = [](QPoint& a, QPoint& b) {return static_cast<double>(a.y() - b.y()) / static_cast<double>(a.x() - b.x()); };

	if (p0.y() == p1.y()) {
		//down
		double m1 = slope(p2, p0);
		double m2 = slope(p2, p1);
		int ymin = p0.y();
		int ymax = p2.y();
		double x1 = static_cast<double>(p0.x());
		double x2 = static_cast<double>(p1.x());

		zFill(primary, lightingMethod, cameraDistance, lightSharpness, x1, x2, m1, m2, ymin, ymax, T, p);
	}
	else if (p1.y() == p2.y()) {
		//up
		double m1 = slope(p1, p0);
		double m2 = slope(p2, p0);
		int ymin = p0.y();
		int ymax = p1.y();
		double x1 = static_cast<double>(p0.x());
		double x2 = x1;

		zFill(primary, lightingMethod, cameraDistance, lightSharpness, x1, x2, m1, m2, ymin, ymax, T, p);
	}
	else {
		double m = slope(p2, p0);

		QPoint P(((p1.y() - p0.y()) / m) + p0.x(), p1.y());

		//down
		if (p1.x() < P.x()) {
			//T(T0,T1,T2) -> T(T1,P,T2)
			p0 = p1;
			p1 = P;
		}
		else {
			//T(T0,T1,T2) -> T(P,T1,T2)
			p0 = P;
		}

		double m1 = slope(p2, p0);
		double m2 = slope(p2, p1);
		int ymin = p0.y();
		int ymax = p2.y();
		double x1 = static_cast<double>(p0.x());
		double x2 = static_cast<double>(p1.x());

		zFill(primary, lightingMethod, cameraDistance, lightSharpness, x1, x2, m1, m2, ymin, ymax, T, p);

		//up
		p0 = T[0];
		p1 = T[1];
		p2 = T[2];
		if (p1.x() < P.x()) {
			//T(T0,T1,T2) -> T(T0,T1,P)
			p2 = P;
		}
		else {
			//T(T0,T1,T2) -> T(T0,P,T1)
			p2 = p1;
			p1 = P;
		}

		m1 = slope(p1, p0);
		m2 = slope(p2, p0);
		ymin = p0.y();
		ymax = p1.y();
		x1 = static_cast<double>(p0.x());
		x2 = x1;

		zFill(primary, lightingMethod, cameraDistance, lightSharpness, x1, x2, m1, m2, ymin, ymax, T, p);
	}
}
void ViewerWidget::zFill(lighting primary, int lightingMethod, int cameraDistance, int lightSharpness, double x1, double x2, double m1, double m2, double ymin, double ymax, QVector<QPoint>& T, QVector<QVector3D>& p) {
	int p0x = T[0].x(); int p0y = T[0].y();
	int p1x = T[1].x(); int p1y = T[1].y();
	int p2x = T[2].x(); int p2y = T[2].y();
	QVector3D camera(250, 250, cameraDistance);
	QVector3D source(primary.source.x, primary.source.y, primary.source.z);

	QVector3D p0(p0x, p0y, p.at(0).z());
	QVector3D p1(p1x, p1y, p.at(1).z());
	QVector3D p2(p2x, p2y, p.at(2).z());
	QColor c0, c1, c2, color;

	auto interpolateZ = [&](const double& x, const double& y) {
		double A = static_cast<double>(abs((p1x - p0x) * (p2y - p0y) - (p1y - p0y) * (p2x - p0x)));
		double lambda0 = static_cast<double>(abs((p1x - x) * (p2y - y) - (p1y - y) * (p2x - x))) / A;	// A0/A
		double lambda1 = static_cast<double>(abs((p0x - x) * (p2y - y) - (p0y - y) * (p2x - x))) / A;	// A1/A
		double lambda2 = 1 - lambda0 - lambda1;
		return (lambda0 * p[0].z() + lambda1 * p[1].z() + lambda2 * p[2].z());
		};
	auto interpolateColor = [&](const double& x, const double& y) {
		double A = static_cast<double>(abs((p1x - p0x) * (p2y - p0y) - (p1y - p0y) * (p2x - p0x)));
		double lambda0 = static_cast<double>(abs((p1x - x) * (p2y - y) - (p1y - y) * (p2x - x))) / A;	// A0/A
		double lambda1 = static_cast<double>(abs((p0x - x) * (p2y - y) - (p0y - y) * (p2x - x))) / A;	// A1/A
		double lambda2 = 1 - lambda0 - lambda1;

		int r = lambda0 * c0.red() + lambda1 * c1.red() + lambda2 * c2.red();
		int g = lambda0 * c0.green() + lambda1 * c1.green() + lambda2 * c2.green();
		int b = lambda0 * c0.blue() + lambda1 * c1.blue() + lambda2 * c2.blue();
		return QColor(r,g,b);
		};
	auto phong = [&](const QVector3D& p) {
		QVector3D V = (camera - p).normalized();
		QVector3D N = p.normalized();
		QVector3D L = (source - p).normalized();
		QVector3D R = 2 * QVector3D::dotProduct(L, N) * N - L;

		double dot = fabs(pow(QVector3D::dotProduct(V, R), lightSharpness));
		int rs = primary.source.r * primary.reflection.coeffR * dot;
		int gs = primary.source.g * primary.reflection.coeffG * dot;
		int bs = primary.source.b * primary.reflection.coeffB * dot;

		dot = QVector3D::dotProduct(L, N);
		int rd = primary.source.r * primary.diffusion.coeffR * dot;
		int gd = primary.source.g * primary.diffusion.coeffG * dot;
		int bd = primary.source.b * primary.diffusion.coeffB * dot;

		int ra = primary.ambient.r * primary.ambient.coeffR;
		int ga = primary.ambient.g * primary.ambient.coeffG;
		int ba = primary.ambient.b * primary.ambient.coeffB;

		int r = rs + rd + ra;
		int g = gs + gd + ga;
		int b = bs + bd + ba;

		r = std::min(r, 255);
		g = std::min(g, 255);
		b = std::min(b, 255);

		r = std::max(r, 0);
		g = std::max(g, 0);
		b = std::max(b, 0);

		return QColor(r, g, b);
		};

	if (lightingMethod == 0) {
		QVector3D center((p0 + p1 + p2) / 3.0);
		color = phong(center);
	}
	else{
		c0 = phong(p0);
		c1 = phong(p1);
		c2 = phong(p2);
	}
	
	for (int y = ymin; y < ymax; y++) {
		if (x1 != x2) {
			int start = floor(x1);
			int end = floor (x2 + 1);
			for (int x = start; x < end; x++) {
				double z = interpolateZ(x, y);

				if (lightingMethod == 1) {
					color = interpolateColor(x, y);
				}

				if (isInside(x, y)) {
					if (Z[x][y] < z) {
						Z[x][y] = z;
						setPixel(x, y, color);
					}
				}
			}
		}
		x1 += 1. / m1;
		x2 += 1. / m2;
	}
}

#pragma endregion