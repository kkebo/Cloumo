/*
 * シート
 */

#pragma once

#include <pistring.h>
#include <List.h>

const int kMaxSheets = 256;
const int kMaxTabs = 100;

enum class GradientDirection { LeftToRight, TopToBottom };
enum class Encoding { SJIS, UTF8, EUCJP };

struct Point {
	int x = 0, y = 0;
	
	Point() = default;
	Point(int x_, int y_) : x(x_), y(y_) {}
	Point(const Point &p) : x(p.x), y(p.y) {}
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

struct Size {
	int width, height;

	Size(int w, int h) : width(w), height(h) {}
	Size operator /(int n) const {
		return Size(width / n, height / n);
	}
	inline int getArea() const {
		return width * height;
	}
};

struct Line {
	Point start, end;
	
	Line(const Point &o, const Point &s) : start(o), end(s) {}
	Line(int x0, int y0, int x1, int y1) : start(x0, y0), end(x1, y1) {}
};

struct Rectangle {
	Point offset;
	Size size;

	Rectangle(const Point &o, const Size &s) : offset(o), size(s) {}
	Rectangle(const Size &s) : offset(0, 0), size(s) {}
	Rectangle(int x, int y, int width, int height) : offset(x, y), size(width, height) {}
	Rectangle(int width, int height) : offset(0, 0), size(width, height) {}
	bool contains(const Point &p) const {
		return offset.x <= p.x && p.x <= offset.x + size.width
		    && offset.y <= p.y && p.y <= offset.y + size.height;
	}
	Point getEndPoint() const {
		return Point(offset.x + size.width, offset.y + size.height);
	}
	inline int getArea() const {
		return size.width * size.height;
	}
	Rectangle &slide(const Point &p) {
		offset += p;
		return *this;
	}
};

struct Circle {
	Point center;
	int radius;
	
	Circle(const Point &p, int rad) : center(p), radius(rad) {}
	explicit Circle(int rad) : Circle(Point(0, 0), rad) {}
};

class Sheet {
private:
	Rectangle _frame = Rectangle(0, 0);
	//int _zIndex = -1;
	bool nonRect;
	List<Sheet *> _children;
	Sheet *_parent = nullptr;

public:
	unsigned int *buf;
	const List<Sheet *> &children = _children;
	Sheet *const &parent = _parent;
	const Rectangle &frame = _frame;
	//const int &zIndex = _zIndex;
	Tab *tab = nullptr;
	void (*onClick)(const Point &pos, Sheet &sht) = nullptr;
	void (*onClosed)() = nullptr;

	friend class SheetCtl;
	Sheet(const Size &size, bool _nonRect = false);
	virtual ~Sheet();
	void upDown(int zIndex);
	void refresh(Rectangle range) const;
	void moveTo(const Point &pos);
	void appendChild(Sheet *child, bool show = false);
	
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
class Tab;

class SheetCtl {
private:
	static union VRAM {
		unsigned short *p16;
		unsigned char (*p24)[3];
		unsigned int *p32;
	} vram;
	static const Sheet **map;
	static File *font;
	static Sheet *sheets[];
	static Size _resolution;
	static int color;
	static int _top;
	
	// for GUI Task
	static Tab *tabs[];
	static int numOfTab;
	static int activeTab;
	static const char *mouseCursor[];
	static Sheet *mouseCursorSheet;
	static Sheet *contextMenu;
	
	// for Text Boxes
	static int caretPosition;
	static unsigned int caretColor;
	static Timer *caretTimer;
	static string *tboxString;

	static void guiTaskMain();
	static void onClickBack(const Point &pos, Sheet &sht);
	static void refreshMap(const Rectangle &range);
	static void refreshSub(const Rectangle &range);

public:
	static Sheet *back;
	
	//static const int &top;
	static const Size &resolution;
	static const int &colorDepth;
	
	// for GUI Task
	static TaskQueue *queue;
	static Point mouseCursorPos;

	friend class Sheet;
	friend class Tab;
	static void init();
	static void reInit();
	static void blueScreen(const char *str);
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

const int kTransColor = 0xff000000;
const auto kBackgroundColor = 0x263238;//Rgb(0, 84, 255);
const auto kActiveTabColor  = 0xffffff;
const auto kActiveTextColor = 0;
const auto kPassiveTabColor = 0x96a2a8;//Rgb(127, 169, 255);
const auto kPassiveTextColor = 0x13191c;//Rgb(0, 42, 127);
