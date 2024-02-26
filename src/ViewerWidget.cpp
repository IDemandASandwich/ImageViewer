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
void ViewerWidget::drawLine(QPoint start, QPoint end, QColor color, int algType)
{
	switch (algType) {
	case 0:
		DDA(start, end, color);
		break;
	case 1:
		Bresenham(start, end, color);
		break;
	}

	update();
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
		setPixel(x1 + x, y1 + y, color);
		setPixel(x1 + x, y1 - y, color);
		setPixel(x1 - x, y1 + y, color);
		setPixel(x1 - x, y1 - y, color);
		setPixel(x1 + y, y1 + x, color);
		setPixel(x1 + y, y1 - x, color);
		setPixel(x1 - y, y1 + x, color);
		setPixel(x1 - y, y1 - x, color);

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
}

void ViewerWidget::drawPolygon(QVector<QPoint> points, QColor color, int algtype) {
	if (points.size() < 3) {
		return;
	}

	for (int i = 0; i < points.size() - 1; i++) {
		drawLine(points[i], points[i + 1], color, algtype);
	}
	drawLine(points[points.size() - 1], points[0], color, algtype);
}
