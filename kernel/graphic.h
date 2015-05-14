/*
 * シート
 */

#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include <pistring.h>

const int kMaxSheets = 256;
const int kMaxTabs = 20;
const int kSheetUse = 1;
const int kTransColor = 255 << 24;

#define kBackgroundColor Rgb(0, 84, 255)
#define kActiveTabColor  Rgb(255, 255, 255)
#define kActiveTextColor 0
#define kPassiveTabColor Rgb(127, 169, 255)
#define kPassiveTextColor Rgb(0, 42, 127)

enum class Encoding { SJIS, UTF8, EUCJP };

struct Sheet {
	unsigned int *buf;
	int bxsize;
	int bysize;
	int vx0;
	int vy0;
	int height;
	int flags;
	bool trans;
};

class SheetCtl {
private:
	static unsigned char *vram_;
	static unsigned char *map_;
	static Sheet *sheets0_;
	static int adrfont_;

public:
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
	static Sheet *alloc(int, int, bool);
	static void upDown(Sheet *, int);
	static void refresh(Sheet &, int, int, int, int);
	static void refreshMap(int, int, int, int, int);
	static void refreshSub(int, int, int, int, int);
	static void slide(Sheet *, int, int);
	static void freeSheet(Sheet *);
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

unsigned int Rgb(unsigned char, unsigned char, unsigned char, unsigned char = 0);
unsigned int MixRgb(unsigned int, unsigned int);
unsigned int GetGrad(int, int, int, unsigned int, unsigned int);

#endif
