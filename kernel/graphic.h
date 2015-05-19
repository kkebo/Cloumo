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

class Sheet {
public:
	unsigned int *buf;
	int bxsize;
	int bysize;
	int vx0;
	int vy0;
	int height;
	bool flags; // 使用中
	bool trans;
	
	Sheet() = default;
	Sheet(int x, int y, bool inv);
	virtual ~Sheet();
	static void *operator new(size_t size);
	static void operator delete(void *p) {}
	void upDown(int height);
	void refresh(int bx0, int by0, int bx1, int by1);
	void slide(int vx0, int vy0);
};

class Task;

class SheetCtl {
private:
	static unsigned char *vram_;
	static unsigned char *map_;
	static unsigned char *adrfont_;

public:
	static Task *refreshTask;
	static Sheet *sheets0_;
	static Sheet **sheets_;
	static int scrnx_;
	static int scrny_;
	static int color_;
	static int top_;
	static Sheet *back_;
	static Sheet **window_;
	static int numOfTab;
	static int activeTab;
	static Sheet *context_menu_;
	static int tbox_cpos_;
	static unsigned int tbox_col_;
	static Timer *tbox_timer_;
	static string *tbox_str_;

public:
	static void init();
	static void refreshMap(int, int, int, int, int);
	static void refreshSub(int, int, int, int, int);
	
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
