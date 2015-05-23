/*
 * シート
 */

#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include <pistring.h>

const int kMaxSheets = 256;
const int kMaxTabs = 20;
constexpr const int kTransColor = 255 << 24;

enum class GradientDirection { LeftToRight, TopToBottom };
enum class Encoding { SJIS, UTF8, EUCJP };

struct Point {
	int x, y;
	
	Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
	Point operator +(const Point &p) const {
		return Point(x + p.x, y + p.y);
	}
	Point operator -(const Point &p) const {
		return Point(x - p.x, y - p.y);
	}
	Point operator /(int n) const {
		return Point(x / n, y / n);
	}
	Point &operator +=(const Point &p) {
		x += p.x;
		y += p.y;
		return *this;
	}
	Point &operator -=(const Point &p) {
		x -= p.x;
		y -= p.y;
		return *this;
	}
};

struct Vector : public Point {
	using Point::Point;
	
	Vector operator +(const Vector &p) const {
		return Vector(x + p.x, y + p.y);
	}
	Vector operator -(const Vector &p) const {
		return Vector(x - p.x, y - p.y);
	}
	Vector operator /(int n) const {
		return Vector(x / n, y / n);
	}
	Vector &operator +=(const Vector &p) {
		x += p.x;
		y += p.y;
		return *this;
	}
	Vector &operator -=(const Vector &p) {
		x -= p.x;
		y -= p.y;
		return *this;
	}
	int getArea() const {
		return x * y;
	}
};

struct Line {
	Point offset;
	Vector vector;
	
	Line(const Point &o, const Vector &v) : offset(o), vector(v) {}
	Line(const Vector &v) : offset(0, 0), vector(v) {}
	Line(int x0 = 0, int y0 = 0, int x1 = 0, int y1 = 0) : Line(Point(x0, y0), Vector(x1, y1)) {}
	Point getEndPoint() const {
		return Point(offset.x + vector.x, offset.y + vector.y);
	}
	void slide(const Point &p) {
		offset.x += p.x;
		offset.y += p.y;
	}
	Line slideAndClone(const Point &p) {
		return Line(offset + p, vector);
	}
};

struct Rectangle : public Line {
	using Line::Line;

	bool contains(const Point &p) const {
		return offset.x <= p.x && p.x <= offset.x + vector.x
			&& offset.y <= p.y && p.y <= offset.y + vector.y;
	}
	Rectangle slideAndClone(const Point &p) {
		return Rectangle(offset + p, vector);
	}
};

struct Circle {
	Point center;
	int radius;
	
	Circle(const Point &p, int rad) : center(p), radius(rad) {}
	Circle(int rad) : Circle(Point(0, 0), rad) {}
};

class Sheet {
private:
	Sheet() : flags(false) {}

public:
	unsigned int *buf;
	Rectangle frame;
	int height;
	bool flags; // 使用中かどうか
	bool trans;
	void (*onClick)(const Point &pos);
	
	friend class SheetCtl;
	Sheet(const Vector &size, bool inv, void (*click)() = nullptr);
	virtual ~Sheet();
	static void *operator new(size_t);
	static void operator delete(void *) {}
	void upDown(int height);
	void refresh(const Rectangle &range) const;
	void slide(const Point &cod);
	
	// 描画関数
	void drawLine(const Line &line, unsigned int color);
	void gradLine(const Line &line, unsigned int col0, unsigned int col1, GradientDirection direction);
	void drawRect(const Rectangle &rect, unsigned int color);
	void fillRect(const Rectangle &rect, unsigned int color);
	void gradRect(const Rectangle &rect, unsigned int col0, unsigned int col1, GradientDirection direction);
	void drawCircle(const Circle &cir, unsigned int color);
	void fillCircle(const Circle &cir, unsigned int color);
	void gradCircle(const Circle &cir, unsigned int col0, unsigned int col1);
	void drawChar(unsigned char *font, const Point &pos, unsigned int color);
	void drawString(const string &str, Point pos, unsigned int color, Encoding encode = Encoding::UTF8);
	void borderRadius(bool ltop, bool rtop, bool lbottom, bool rbottom);
	void drawPicture(const char *fileName, const Point &pos, long transColor = -1, int ratio = 1);
	void changeColor(const Rectangle &range, unsigned int col0, unsigned int col1);
};

class Task;
class File;

class SheetCtl {
private:
	static unsigned char *vram;
	static unsigned char *map;
	static File *font;

public:
	static Task *refreshTask;
	static Sheet sheets0[];
	static Sheet *sheets[];
	static int scrnx;
	static int scrny;
	static int color;
	static int top;
	static Sheet *back;
	static Sheet *window[];
	static int numOfTab;
	static int activeTab;
	static Sheet *contextMenu;
	
	// for Text Boxes
	static int caretPosition;
	static unsigned int caretColor;
	static Timer *caretTimer;
	static string *tboxString;

public:
	friend class Sheet;
	static void init();
	static void refreshMap(const Rectangle &range, int);
	static void refreshSub(const Rectangle &range, int);
};

// 赤緑青をあわせてunsigned intで出力
constexpr unsigned int Rgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0) {
	return ((a << 24) | (r << 16) | (g << 8) | b);
}

// 透明化処理
constexpr unsigned int MixRgb(unsigned int rgb1, unsigned int rgb2) {
	return (((unsigned char) (rgb1 >> 24) << 24)
	        | (((unsigned char) (rgb2 >> 16) - (unsigned char) (rgb1 >> 16)) * (unsigned char) (rgb1 >> 24) / 255 + (unsigned char) (rgb1 >> 16)) << 16
	        | (((unsigned char) (rgb2 >> 8) - (unsigned char) (rgb1 >> 8)) * (unsigned char) (rgb1 >> 24) / 255 + (unsigned char) (rgb1 >> 8)) << 8
	        | (((unsigned char) rgb2 - (unsigned char) rgb1) * (unsigned char) (rgb1 >> 24) / 255 + (unsigned char) rgb1));
}

// グラデーション色を出力
constexpr unsigned int GetGrad(int p0, int p1, int p, unsigned int c0, unsigned int c1) {
	return (((unsigned char) (c0 >> 24) << 24)
	        | (((unsigned char) (c0 >> 16) + ((unsigned char) (c1 >> 16) - (unsigned char) (c0 >> 16)) * (p - p0) / (p1 - p0)) << 16)
	        | (((unsigned char) (c0 >> 8) + ((unsigned char) (c1 >> 8) - (unsigned char) (c0 >> 8)) * (p - p0) / (p1 - p0)) << 8)
	        | ((unsigned char) c0 + ((unsigned char) c1 - (unsigned char) c0) * (p - p0) / (p1 - p0)));
}

constexpr const auto kBackgroundColor = Rgb(0, 84, 255);
const auto kActiveTabColor  = 0xffffff;
const auto kActiveTextColor = 0;
constexpr const auto kPassiveTabColor = Rgb(127, 169, 255);
constexpr const auto kPassiveTextColor = Rgb(0, 42, 127);

#endif
