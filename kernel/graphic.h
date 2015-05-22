/*
 * シート
 */

#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include <pistring.h>

const int kMaxSheets = 256;
const int kMaxTabs = 20;
constexpr const int kTransColor = 255 << 24;

enum class Encoding { SJIS, UTF8, EUCJP };

struct Point {
	int x = 0, y = 0;
	
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
	
	int getArea() const {
		return x * y;
	}
};

struct Rect {
	Point offset;
	Vector vector;

	Rect(const Point &o, const Vector &v) : offset(o), vector(v) {}
	Rect(int x0 = 0, int y0 = 0, int x1 = 0, int y1 = 0) : Rect(Point(x0, y0), Vector(x1, y1)) {}
	bool contains(const Point &p) const {
		return offset.x <= p.x && p.x <= offset.x + vector.x
			&& offset.y <= p.y && p.y <= offset.y + vector.y;
	}
	Point getEndPoint() const {
		return Point(offset.x + vector.x, offset.y + vector.y);
	}
	void slide(const Point &p) {
		offset.x += p.x;
		offset.y += p.y;
	}
	Rect slideAndClone(const Point &p) {
		return Rect(offset + p, vector);
	}
};

class Sheet {
public:
	unsigned int *buf;
	Rect frame;
	int height;
	bool flags; // 使用中
	bool trans;
	void (*onClick)(const Point &cod);
	
	Sheet() {};
	Sheet(const Vector &size, bool inv, void (*click)() = nullptr);
	virtual ~Sheet();
	static void *operator new(size_t size);
	static void operator delete(void *p) {}
	void upDown(int height);
	void refresh(const Rect &range);
	void slide(const Point &cod);
};

class Task;

class SheetCtl {
private:
	static unsigned char *vram;
	static unsigned char *map;
	static unsigned char *adrfont;

public:
	static Task *refreshTask;
	static Sheet *sheets0;
	static Sheet **sheets;
	static int scrnx;
	static int scrny;
	static int color;
	static int top;
	static Sheet *back;
	static Sheet **window;
	static int numOfTab;
	static int activeTab;
	static Sheet *contextMenu;
	
	// for Text Boxes
	static int tbox_cpos_;
	static unsigned int tbox_col_;
	static Timer *tbox_timer_;
	static string *tbox_str_;

public:
	static void init();
	static void refreshMap(const Rect &range, int);
	static void refreshSub(const Rect &range, int);
	
	static void drawLine(Sheet *, unsigned int, int, int, int, int);
	static void gradLine(Sheet *, unsigned int, unsigned int, int, int, int, int, int);
	static void drawRect(Sheet *, unsigned int, int, int, int, int);
	static void fillRect(Sheet *, unsigned int, int, int, int, int);
	static void gradRect(Sheet *, unsigned int, unsigned int, int, int, int, int, int);
	static void drawCircle(Sheet *, unsigned int, int, int, int);
	static void fillCircle(Sheet *, unsigned int, int, int, int);
	static void gradCircle(Sheet *, unsigned int, unsigned int, int, int, int);
	static void drawChar(Sheet *, int, int, unsigned int, unsigned char *);
	static void drawString(Sheet *, int, int, unsigned int, const char *, Encoding = Encoding::UTF8);
	static void borderRadius(Sheet *, bool, bool, bool, bool);
	static void drawPicture(Sheet *, int, int, const char *, long = -1, int = 1);
	static void colorChange(Sheet &, int, int, int, int, unsigned int, unsigned int);
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
constexpr const auto kActiveTabColor  = Rgb(255, 255, 255);
const auto kActiveTextColor = 0;
constexpr const auto kPassiveTabColor = Rgb(127, 169, 255);
constexpr const auto kPassiveTextColor = Rgb(0, 42, 127);

#endif
