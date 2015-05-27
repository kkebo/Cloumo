#include <SmartPointer.h>
#include <MinMax.h>
#include "../headers.h"
#include "HTMLTokenizer.h"
#include "HTMLTreeConstructor.h"

Sheet::Sheet(const Size &size, bool _nonRect, void (*click)(const Point &)) :
	_frame(size),
	nonRect(_nonRect),
	buf(new unsigned int[size.getArea()]),
	onClick(click) {}

Sheet::~Sheet() {
	if (height >= 0) upDown(-1);
	delete[] buf;
}

// シートの高さを変更
void Sheet::upDown(int height) {
	int old = this->height;

	if (height > SheetCtl::top + 1) height = SheetCtl::top + 1;
	if (height < -1) height = -1;
	this->_height = height;

	if (old > height) { // 前より低くなった
		if (height >= 0) { // 表示
			for (int h = old; h > height; --h) {
				SheetCtl::sheets[h] = SheetCtl::sheets[h - 1];
				SheetCtl::sheets[h]->_height = h;
			}
			SheetCtl::sheets[height] = this;
			SheetCtl::refreshMap(frame, height + 1);
			SheetCtl::refreshSub(frame, SheetCtl::top);
		} else { // 非表示
			if (SheetCtl::top > old) {
				for (int h = old; h < SheetCtl::top; ++h) {
					SheetCtl::sheets[h] = SheetCtl::sheets[h + 1];
					SheetCtl::sheets[h]->_height = h;
				}
			}
			--SheetCtl::_top;
			SheetCtl::refreshMap(frame, 0);
			SheetCtl::refreshSub(frame, SheetCtl::top);
		}
	} else if (old < height) { // 以前より高くなった
		if (old >= 0) {	// より高く
			for (int h = old; h < height; ++h) {
				SheetCtl::sheets[h] = SheetCtl::sheets[h + 1];
				SheetCtl::sheets[h]->_height = h;
			}
			SheetCtl::sheets[height] = this;
		} else { // 非表示から表示へ
			// 管理配列の限界が来たら無視
			if (SheetCtl::top + 1 >= kMaxSheets) return;
			
			for (int h = SheetCtl::top; h >= height; --h) {
				SheetCtl::sheets[h + 1] = SheetCtl::sheets[h];
				SheetCtl::sheets[h + 1]->_height = h + 1;
			}
			SheetCtl::sheets[height] = this;
			++SheetCtl::_top;
		}
		SheetCtl::refreshMap(frame, height);
		SheetCtl::refreshSub(frame, SheetCtl::top);
	}
}

// シートのリフレッシュ
void Sheet::refresh(Rectangle range) const {
	if (height >= 0) {	// 非表示シートはリフレッシュしない
		range.slide(frame.offset);
		SheetCtl::refreshMap(range, height);
		SheetCtl::refreshSub(range, height);
	}
}

// シートを移動
void Sheet::moveTo(const Point &pos) {
	Rectangle oldFrame(frame);
	_frame.offset = pos;
	if (height >= 0) {	// 非表示シートはリフレッシュしない
		SheetCtl::refreshMap(oldFrame, 0);
		SheetCtl::refreshMap(frame, height);
		SheetCtl::refreshSub(oldFrame, height - 1);
		SheetCtl::refreshSub(frame, height);
	}
}

// 単色直線を描画
void Sheet::drawLine(const Line &line, unsigned int color) {
	int x, y, dx, dy, len;

	// 縦横の直線高速化
	if (line.start.y == line.end.y) {
		for (x = line.start.x; x <= line.end.x; ++x) {
			buf[line.start.y * frame.size.width + x] = color;
		}
		return;
	} else if (line.start.x == line.end.x) {
		for (y = line.start.y; y <= line.end.y; ++y) {
			buf[y * frame.size.width + line.start.x] = color;
		}
		return;
	}

	dx = line.end.x - line.start.x > 0 ? line.end.x - line.start.x : line.start.x - line.end.x;
	dy = line.end.y - line.start.y > 0 ? line.end.y - line.start.y : line.start.y - line.end.y;
	x = line.start.x << 10;
	y = line.start.y << 10;
	if (dx >= dy) {
		len = dx + 1;
		dx = line.end.x - line.start.x < 0 ? -1024 : 1024;
		dy = line.end.y - line.start.y >= 0 ? ((line.end.y - line.start.y + 1) << 10) / len
		                                    : ((line.end.y - line.start.y - 1) << 10) / len;
	} else {
		len = dy + 1;
		dy = line.end.y - line.start.y < 0 ? -1024 : 1024;
		dx = line.end.x - line.start.x >= 0 ? ((line.end.x - line.start.x + 1) << 10) / len
		                                    : ((line.end.x - line.start.x - 1) << 10) / len;
	}

	for (int i = 0; i < len; ++i) {
		buf[(y >> 10) * frame.size.width + (x >> 10)] = color;
		x += dx;
		y += dy;
	}
}

// グラデーション直線を描画
void Sheet::gradLine(const Line &line, unsigned int col0, unsigned int col1, GradientDirection direction) {
	int x, y, dx, dy, len;

	dx = line.end.x - line.start.x > 0 ? line.end.x - line.start.x : line.start.x - line.end.x;
	dy = line.end.y - line.start.y > 0 ? line.end.y - line.start.y : line.start.y - line.end.y;
	x = line.start.x << 10;
	y = line.start.y << 10;
	if (dx >= dy) {
		len = dx + 1;
		dx = line.end.x - line.start.x < 0 ? -1024 : 1024;
		dy = line.end.y - line.start.y >= 0 ? ((line.end.y - line.start.y + 1) << 10) / len
		                                    : ((line.end.y - line.start.y - 1) << 10) / len;
	} else {
		len = dy + 1;
		dy = line.end.y - line.start.y < 0 ? -1024 : 1024;
		dx = line.end.x - line.start.x >= 0 ? ((line.end.x - line.start.x + 1) << 10) / len
		                                    : ((line.end.x - line.start.x - 1) << 10) / len;
	}

	if (direction == GradientDirection::LeftToRight) { // 横
		if (line.start.x == line.end.x) { // 高速
			for (int i = 0; i < len; ++i) {
				buf[(y >> 10) * frame.size.width + (x >> 10)] = col0;
				x += dx;
				y += dy;
			}
		} else {
			for (int i = 0; i < len; ++i) {
				buf[(y >> 10) * frame.size.width + (x >> 10)] = GetGrad(line.start.x, line.end.x, x >> 10, col0, col1);
				x += dx;
				y += dy;
			}
		}
	} else if (direction == GradientDirection::TopToBottom) { // 縦
		if (line.start.y == line.end.y) { // 高速
			for (int i = 0; i < len; ++i) {
				buf[(y >> 10) * frame.size.width + (x >> 10)] = col0;
				x += dx;
				y += dy;
			}
		} else {
			for (int i = 0; i < len; ++i) {
				buf[(y >> 10) * frame.size.width + (x >> 10)] = GetGrad(line.start.y, line.end.y, y >> 10, col0, col1);
				x += dx;
				y += dy;
			}
		}
	}
}

// 枠のみ長方形を描画
void Sheet::drawRect(const Rectangle &rect, unsigned int color) {
	int endy = rect.getEndPoint().y - 1;
	for (int x = 0; x < rect.size.width; ++x) {
		// 上の辺を描画
		buf[rect.offset.y * frame.size.width + x + rect.offset.x] = color;
		// 下の辺を描画
		buf[endy * frame.size.width + x + rect.offset.x] = color;
	}
	int endx = rect.getEndPoint().x - 1;
	for (int y = 1; y < rect.size.height - 1; ++y) {
		// 左の辺を描画
		buf[(y + rect.offset.y) * frame.size.width + rect.offset.x] = color;
		// 右の辺を描画
		buf[(y + rect.offset.y) * frame.size.width + endx] = color;
	}
}

// 塗りつぶし長方形を描画
void Sheet::fillRect(const Rectangle &rect, unsigned int color) {
	for (int y = 0; y < rect.size.height; ++y) {
		int by = y + rect.offset.y;
		for (int x = 0; x < rect.size.width; ++x) {
			buf[by * frame.size.width + x + rect.offset.x] = color;
		}
	}
}

// グラデーション長方形を描画
void Sheet::gradRect(const Rectangle &rect, unsigned int col0, unsigned int col1, GradientDirection direction) {
	if (direction == GradientDirection::LeftToRight) { // 横
		for (int x = 0; x < rect.size.width; ++x) {
			unsigned int gradColor = GetGrad(0, rect.size.width - 1, x, col0, col1);
			for (int y = 0; y < rect.size.height; ++y) {
				buf[(y + rect.offset.y) * frame.size.width + x + rect.offset.x] = gradColor;
			}
		}
	} else if (direction == GradientDirection::TopToBottom) { //縦
		for (int y = 0; y < rect.size.height; ++y) {
			unsigned int gradColor = GetGrad(0, rect.size.height - 1, y, col0, col1);
			for (int x = 0; x < rect.size.width; ++x) {
				buf[(y + rect.offset.y) * frame.size.width + x + rect.offset.x] = gradColor;
			}
		}
	}
}

// 枠のみ円を描画
void Sheet::drawCircle(const Circle &cir, unsigned int color) {
	int x = cir.radius;
	int y = 0;
	int F = -2 * cir.radius + 3;
	/*if (直径 % 2 == 1) {
		while (x >= y) {
			buf[(yo + y) * frame.size.width + xo + x] = c;
			buf[(yo + y) * frame.size.width + xo - x] = c;
			buf[(yo - y) * frame.size.width + xo + x] = c;
			buf[(yo - y) * frame.size.width + xo - x] = c;
			buf[(yo + x) * frame.size.width + xo + y] = c;
			buf[(yo + x) * frame.size.width + xo - y] = c;
			buf[(yo - x) * frame.size.width + xo + y] = c;
			buf[(yo - x) * frame.size.width + xo - y] = c;
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	} else {*/
		while (x >= y) {
			buf[(cir.center.y + y) * frame.size.width + cir.center.x + x - 1] = color;
			buf[(cir.center.y + y) * frame.size.width + cir.center.x - x] = color;
			buf[(cir.center.y - y) * frame.size.width + cir.center.x + x - 1] = color;
			buf[(cir.center.y - y) * frame.size.width + cir.center.x - x] = color;
			buf[(cir.center.y + x - 1) * frame.size.width + cir.center.x + y] = color;
			buf[(cir.center.y + x - 1) * frame.size.width + cir.center.x - y] = color;
			buf[(cir.center.y - x) * frame.size.width + cir.center.x + y] = color;
			buf[(cir.center.y - x) * frame.size.width + cir.center.x - y] = color;
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	//}
}

// 塗りつぶし円を描画
void Sheet::fillCircle(const Circle &cir, unsigned int color) {
	int x = cir.radius;
	int y = 0;
	int F = -2 * cir.radius + 3;
	/*if (d % 2 == 1) {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; ++xx) {
				sht->buf[(yo + y) * sht->frame.size.width + xx] = c;
				sht->buf[(yo - y) * sht->frame.size.width + xx] = c;
			}
			for (int xx = xo - y; xx < xo + y; ++xx) {
				sht->buf[(yo + x) * sht->frame.size.width + xx] = c;
				sht->buf[(yo - x) * sht->frame.size.width + xx] = c;
			}
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	} else {*/
		while (x >= y) {
			for (int xx = cir.center.x - x; xx < cir.center.x + x; ++xx) {
				buf[(cir.center.y + y) * frame.size.width + xx] = color;
				buf[(cir.center.y - y) * frame.size.width + xx] = color;
			}
			for (int xx = cir.center.x - y; xx < cir.center.x + y; ++xx) {
				buf[(cir.center.y + x - 1) * frame.size.width + xx] = color;
				buf[(cir.center.y - x) * frame.size.width + xx] = color;
			}
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	//}
}

// TopToBottom グラデーションで塗りつぶされた円を描画
void Sheet::gradCircle(const Circle &cir, unsigned int col0, unsigned int col1) {
	int x = cir.radius;
	int y = 0;
	int F = -2 * cir.radius + 3;
	/*if (d % 2 == 1) {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; ++xx) {
				sht->buf[(yo + y) * sht->frame.size.width + xx] = GetGrad(y0, y0 + d, yo + y, c0, c1);
				sht->buf[(yo - y) * sht->frame.size.width + xx] = GetGrad(y0, y0 + d, yo - y, c0, c1);
			}
			for (int xx = xo - y; xx < xo + y; ++xx) {
				sht->buf[(yo + x) * sht->frame.size.width + xx] = GetGrad(y0, y0 + d, yo + x, c0, c1);
				sht->buf[(yo - x) * sht->frame.size.width + xx] = GetGrad(y0, y0 + d, yo - x, c0, c1);
			}
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	} else {*/
		while (x >= y) {
			for (int xx = cir.center.x - x; xx < cir.center.x + x; ++xx) {
				buf[(cir.center.y + y) * frame.size.width + xx] = GetGrad(-cir.radius, cir.radius, y, col0, col1);
				buf[(cir.center.y - y) * frame.size.width + xx] = GetGrad(-cir.radius, cir.radius, -y, col0, col1);
			}
			for (int xx = cir.center.x - y; xx < cir.center.x + y; ++xx) {
				buf[(cir.center.y + x - 1) * frame.size.width + xx] = GetGrad(-cir.radius, cir.radius, x - 1, col0, col1);
				buf[(cir.center.y - x) * frame.size.width + xx] = GetGrad(-cir.radius, cir.radius, -y, col0, col1);
			}
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	//}
}

// 単色文字を描画
void Sheet::drawChar(unsigned char *font, const Point &pos, unsigned int color) {
	for (int i = 0; i < 16; ++i) {
		unsigned int *p = buf + (pos.y + i) * frame.size.width + pos.x;
		unsigned char d = font[i];
		if (d & 0x80) { p[0] = color; }
		if (d & 0x40) { p[1] = color; }
		if (d & 0x20) { p[2] = color; }
		if (d & 0x10) { p[3] = color; }
		if (d & 0x08) { p[4] = color; }
		if (d & 0x04) { p[5] = color; }
		if (d & 0x02) { p[6] = color; }
		if (d & 0x01) { p[7] = color; }
	}
}

// 単色文字列を描画
void Sheet::drawString(const string &str, Point pos, unsigned int color, Encoding encode) {
	using uchar = unsigned char;
	unsigned char *fontdat = SheetCtl::font->read();
	unsigned char *font;
	int k, t;
	unsigned short langbyte1 = 0;
	unsigned int u8code;
	for (string::const_iterator s = str.begin(); s != str.end(); ++s) {
		if (!langbyte1) {
			if (encode == Encoding::SJIS && ((0x81 <= (uchar)*s && (uchar)*s <= 0x9f) || (0xe0 <= (uchar)*s && (uchar)*s <= 0xfc))) {	// Shift_JIS
				langbyte1 = (uchar)*s;
			} else if (encode == Encoding::UTF8) {	// UTF-8
				string::const_iterator next = s;
				++next;
				if (((0xe2 <= (uchar)*s && (uchar)*s <= 0xef) || (0xc2 <= (uchar)*s && (uchar)*s <= 0xd1))
					&& 0x80 <= (uchar)*next && (uchar)*next <= 0xbf) {
					langbyte1 = (((uchar)*s << 8) | (uchar)*next);
					++s;
					++next;
				} else {
					drawChar(fontdat + (uchar)*s * 16, pos, color);
				}
				if (langbyte1 == 0xefbd) {	// ｡~ｿ
					++s;
					drawChar(fontdat + (uchar)*s * 16, pos, color);
					langbyte1 = 0;
				} else if (langbyte1 == 0xefbe) {	// ﾀ~ﾟ
					++s;
					drawChar(fontdat + ((uchar)*s + 0x40) * 16, pos, color);
					langbyte1 = 0;
				} else if (langbyte1 == 0xe280 && (uchar)*next == 0xbe) {	// 波ダッシュ(~)
					++s;
					drawChar(fontdat + 0x7e * 16, pos, color);
					langbyte1 = 0;
				} else if (langbyte1 == 0xc2a5) {	// 円マーク(\)
					drawChar(fontdat + 0x5c * 16, pos, color);
					langbyte1 = 0;
				}
			} else if (encode == Encoding::EUCJP && 0x81 <= (uchar)*s && (uchar)*s <= 0xfe) {	// EUC-JP
				langbyte1 = (uchar)*s;
			} else {	// 半角1バイト文字
				drawChar(fontdat + (uchar)*s * 16, pos, color);
			}
		} else {
			if (encode == Encoding::SJIS) {
				k = (0x81 <= langbyte1 && langbyte1 <= 0x9f) ? (langbyte1 - 0x81) * 2
				                                             : (langbyte1 - 0xe0) * 2 + 62;
				if (0x40 <= (uchar)*s && (uchar)*s <= 0x7e) {
					t = (uchar)*s - 0x40;
				} else if (0x80 <= (uchar)*s && (uchar)*s <= 0x9e) {
					t = (uchar)*s - 0x80 + 63;
				} else {
					t = (uchar)*s - 0x9f;
					++k;
				}
				font = fontdat + 256 * 16 + (k * 94 + t) * 32;
			} else if (encode == Encoding::UTF8) {
				if (langbyte1 >> 12 != 0xc && langbyte1 >> 12 != 0xd) {
					u8code = ((langbyte1 << 8) | (uchar)*s);
				} else {
					u8code = langbyte1;
					--s;
				}
				font = fontdat + 256 * 16 + Utf8ToKT(u8code) * 32;
			} else/* if (encode == Encoding::EUCJP)*/ {
				font = fontdat + 256 * 16 + ((langbyte1 - 0xa1) * 94 + (uchar)*s - 0xa1) * 32;
			}
			langbyte1 = 0;
			drawChar(font, Point(pos.x - 8, pos.y), color);
			drawChar(font + 16, pos, color);
		}
		pos.x += 8;
	}
}

void Sheet::borderRadius(bool ltop, bool rtop, bool lbottom, bool rbottom) {
	int x = frame.size.width, y = frame.size.height;
	// 左上
	if (ltop) {
		drawLine(Line(0, 0, 2, 0), kTransColor); // □□□
		drawLine(Line(0, 1, 1, 1), kTransColor); // □□■
		drawLine(Line(0, 2, 0, 2), kTransColor); // □■■
	}
	// 右上
	if (rtop) {
		drawLine(Line(x - 3, 0, x - 1, 0), kTransColor); // □□□
		drawLine(Line(x - 2, 1, x - 1, 1), kTransColor); // ■□□
		drawLine(Line(x - 1, 2, x - 1, 2), kTransColor); // ■■□
	}
	// 左下
	if (lbottom) {
		drawLine(Line(0, y - 3, 0, y - 3), kTransColor); // □■■
		drawLine(Line(0, y - 2, 1, y - 2), kTransColor); // □□■
		drawLine(Line(0, y - 1, 2, y - 1), kTransColor); // □□□
	}
	// 右下
	if (rbottom) {
		drawLine(Line(x - 1, y - 3, x - 1, y - 3), kTransColor); // ■■□
		drawLine(Line(x - 2, y - 2, x - 1, y - 2), kTransColor); // ■□□
		drawLine(Line(x - 3, y - 1, x - 1, y - 1), kTransColor); // □□□
	}
}

// 画像を描画
void Sheet::drawPicture(const char *fileName, const Point &pos, long transColor, int ratio) {
	int info[4];
	unsigned int color;
	int i;
	DLL_STRPICENV env;
	File imagefile(fileName);

	if (imagefile.open()) {
		unsigned char *filebuf = imagefile.read();
		unsigned int fsize = imagefile.size;

		if (!_info_JPEG(&env, info, fsize, filebuf) && !_info_BMP(&env, info, fsize, filebuf)) {
			return;
		}

		unique_ptr<RGB> picbuf(new RGB[info[2] * info[3]]);
		if (picbuf) {
			if (info[0] == 1) {
				i = _decode0_BMP(&env, fsize, filebuf, 4, (unsigned char*)picbuf.get(), 0);
			} else {
				i = _decode0_JPEG(&env, fsize, filebuf, 4, (unsigned char*)picbuf.get(), 0);
			}
			if (!i && info[2] <= SheetCtl::resolution.width && info[3] <= SheetCtl::resolution.height) {
				for (int yy = 0; yy < info[3]; ++yy) {
					for (int xx = 0; xx < info[2]; ++xx) {
						color = Rgb(picbuf[yy * info[2] + xx].r, picbuf[yy * info[2] + xx].g, picbuf[yy * info[2] + xx].b);
						if ((long)color != transColor && buf[(yy + pos.y) * ratio * frame.size.width + (xx + pos.x) * ratio] != color) {
							buf[(yy + pos.y) * ratio * frame.size.width + (xx + pos.x) * ratio] = color;
						}
					}
				}
			}
		}
	}
}

// 指定色を変更
void Sheet::changeColor(const Rectangle &range, unsigned int col0, unsigned int col1) {
	for (int y = 0; y < range.size.height; ++y) {
		for (int x = 0; x < range.size.width; ++x) {
			if (buf[(y + range.offset.y) * frame.size.width + x + range.offset.x] == col0) {
				buf[(y + range.offset.y) * frame.size.width + x + range.offset.x] = col1;
			}
		}
	}
}

int SheetCtl::_top = -1;
const int &SheetCtl::top = SheetCtl::_top;
int SheetCtl::caretPosition = 2;
unsigned int SheetCtl::caretColor = 0;
Timer *SheetCtl::caretTimer;
string *SheetCtl::tboxString;
SheetCtl::VRAM SheetCtl::vram;
Size SheetCtl::_resolution(0, 0);
const Size &SheetCtl::resolution = SheetCtl::_resolution;
unsigned char *SheetCtl::map;
TaskQueue *SheetCtl::queue;
Sheet *SheetCtl::back;
Sheet *SheetCtl::contextMenu;
Sheet *SheetCtl::window[kMaxTabs];
int SheetCtl::numOfTab = 1;
int SheetCtl::activeTab = 0;
Sheet *SheetCtl::sheets[kMaxSheets];
int SheetCtl::color;
File *SheetCtl::font;
Point SheetCtl::mouseCursorPos(-1, 0);
Sheet *SheetCtl::mouseCursorSheet;
const char *SheetCtl::mouseCursor[] = {
	"*****OOOOOO*****",
	"***OO@@@@@@OO***",
	"**O@@@GCCG@@@O**",
	"*O@@JUUUUUUJ@@O*",
	"*O@JUUUUUUUUJ@O*",
	"O@@UUUUUUUUUU@@O",
	"O@GUUUUUUUUUUG@O",
	"O@CUUUUUUUUUUC@O",
	"O@CUUUUUUUUUUC@O",
	"O@GUUUUUUUUUUG@O",
	"O@@UUUUUUUUUU@@O",
	"*O@JUUUUUUUUJ@O*",
	"*O@@JUUUUUUJ@@O*",
	"**O@@@GCCG@@@O**",
	"***OO@@@@@@OO***",
	"*****OOOOOO*****"
};

// シートコントロールを初期化
void SheetCtl::init() {
	/* データメンバ初期化 */
	BootInfo *binfo = (BootInfo *)ADDRESS_BOOTINFO;
	vram.p16    = reinterpret_cast<unsigned short *>(binfo->vram);
	_resolution = Size(binfo->scrnx, binfo->scrny);
	color       = binfo->vmode;
	map         = new unsigned char[resolution.getArea()];
	tboxString  = new string();

	/* フォント読み込み */
	font = new File("japanese.fnt");
	font->open();

	/* サイドバー */
	back = new Sheet(Size(150, resolution.height), false, [](const Point &pos) {
		if (43 <= pos.x && pos.x <= 74 && back->frame.size.height - 20 - 45 <= pos.y && pos.y <= back->frame.size.height - 45) {
			// JP 選択
			KeyboardController::switchToJIS();
			back->changeColor(Rectangle(43, back->frame.size.height - 20 - 45, 32, 20), 0xfffffe, 0);
			back->changeColor(Rectangle(43, back->frame.size.height - 20 - 45, 32, 20), kBackgroundColor, 0xffffff);
			back->changeColor(Rectangle(74, back->frame.size.height - 20 - 45, 33, 20), 0, 0xfffffe);
			back->changeColor(Rectangle(74, back->frame.size.height - 20 - 45, 33, 20), 0xffffff, kBackgroundColor);
			back->refresh(Rectangle(43, back->frame.size.height - 20 - 45, 64, 20));
		} else if (75 <= pos.x && pos.x <= 107 && back->frame.size.height - 20 - 45 <= pos.y && pos.y <= back->frame.size.height - 45) {
			// US 選択
			KeyboardController::switchToUS();
			back->changeColor(Rectangle(74, back->frame.size.height - 20 - 45, 33, 20), 0xfffffe, 0);
			back->changeColor(Rectangle(74, back->frame.size.height - 20 - 45, 33, 20), kBackgroundColor, 0xffffff);
			back->changeColor(Rectangle(43, back->frame.size.height - 20 - 45, 32, 20), 0, 0xfffffe);
			back->changeColor(Rectangle(43, back->frame.size.height - 20 - 45, 32, 20), 0xffffff, kBackgroundColor);
			back->refresh(Rectangle(43, back->frame.size.height - 20 - 45, 64, 20));
		}
		
		for (int i = 0; i < SheetCtl::numOfTab; ++i) {
			if (i != SheetCtl::activeTab && 35 + 23 * i <= pos.y && pos.y < 33 + 16 + 8 + 23 * i) {
				// 選択したタブ
				Rectangle selectedTabRange(2, 35 + 23 * i, SheetCtl::back->frame.size.width - 2, 22);
				SheetCtl::back->changeColor(selectedTabRange, kPassiveTabColor, kActiveTabColor);
				SheetCtl::back->changeColor(selectedTabRange, kPassiveTextColor, kActiveTextColor);
				SheetCtl::back->refresh(selectedTabRange);
				// アクティブだったタブ
				Rectangle prevTabRange(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.size.width - 2, 22);
				SheetCtl::back->changeColor(prevTabRange, kActiveTabColor, kPassiveTabColor);
				SheetCtl::back->changeColor(prevTabRange, kActiveTextColor, kPassiveTextColor);
				SheetCtl::back->refresh(prevTabRange);
				
				SheetCtl::window[SheetCtl::activeTab]->upDown(-1);
				SheetCtl::window[i]->upDown(1);
				
				SheetCtl::activeTab = i;
			}
		}
	});
	// 背景色
	back->fillRect(back->frame, kBackgroundColor);
	// 戻る・進むボタン枠
	back->drawPicture("b_f.bmp", Point(4, 4), 0xff00ff);
	// 更新ボタン枠
	back->drawPicture("btn_r.bmp", Point(59, 4), 0xff00ff);
	// タブ
	back->drawString("system info", Point(6, 39), kActiveTextColor);
	back->changeColor(Rectangle(2, 35, back->frame.size.width - 2, 22), kBackgroundColor, kActiveTabColor);
	// キーマップスイッチ
	back->drawRect(Rectangle(42, back->frame.size.height - 20 - 46, 66, 22), 0xffffff);
	//back->drawLine(Line(50 + 24, back->frame.size.height - 20 - 45, 50 + 24, back->frame.size.height - 20 - 25), 0xffffff);
	back->fillRect(Rectangle(43, back->frame.size.height - 20 - 45, 32, 20), 0xffffff);
	back->drawString("JP", Point(50, back->frame.size.height - 20 - 43), 0);
	back->drawString("US", Point(50 + 32, back->frame.size.height - 20 - 43), 0xfffffe);
	// 検索窓
	back->fillRect(Rectangle(2, back->frame.size.height - 20 - 22, back->frame.size.width - 2 - 2, 22), 0xffffff);
	// 表示設定
	back->upDown(0);

	// tabs
	window[0] = new Sheet(Size(resolution.width - back->frame.size.width, resolution.height), false);
	window[0]->fillRect(window[0]->frame, 0xffffff);
	window[0]->drawRect(window[0]->frame, 0);
	window[0]->moveTo(Point(back->frame.size.width, 0));

	// system info タブを全面へ
	window[0]->upDown(1);

	// マウスポインタ描画
	mouseCursorPos = Point(-1, 0);
	mouseCursorSheet = new Sheet(Size(16, 16), true);
	for (int y = 0; y < 16; ++y) {
		for (int x = 0; x < 16; ++x) {
			switch (mouseCursor[x][y]) {
				case 'O':
					mouseCursorSheet->buf[y * 16 + x] = Rgb(255, 255, 255, 100);
					break;
				case '@':
					mouseCursorSheet->buf[y * 16 + x] = Rgb(12, 69, 255, 100);
					break;
				case 'G':
					mouseCursorSheet->buf[y * 16 + x] = Rgb(27, 81, 255, 100);
					break;
				case 'J':
					mouseCursorSheet->buf[y * 16 + x] = Rgb(58, 104, 255, 100);
					break;
				case 'C':
					mouseCursorSheet->buf[y * 16 + x] = Rgb(73, 116, 255, 100);
					break;
				case 'U':
					mouseCursorSheet->buf[y * 16 + x] = Rgb(0, 182, 200, 100);
					break;
				default:
					mouseCursorSheet->buf[y * 16 + x] = kTransColor;
					break;
			}
		}
	}
	mouseCursorSheet->moveTo(Point(resolution.width / 2, resolution.height / 2));
	mouseCursorSheet->upDown(top + 1);

	/* 右クリックメニュー */
	contextMenu = new Sheet(Size(150, 150), true);
	contextMenu->fillRect(contextMenu->frame, kTransColor);
	//contextMenu->gradCircle(Circle(Point(75, 75), 75), Rgb(200, 230, 255, 50), Rgb(100, 150, 255));
	contextMenu->fillCircle(Circle(Point(75, 75), 75), 0x19e0e0e0);
	//contextMenu->drawCircle(Circle(Point(75, 75), 75), Rgb(0, 0, 255, 50));
	contextMenu->fillCircle(Circle(Point(75, 75), 35), kTransColor);
	//contextMenu->drawCircle(Circle(Point(75, 75), 35), Rgb(0, 0, 255, 50));
	contextMenu->drawPicture("copy.bmp", Point(contextMenu->frame.size.width / 2 - 16, 3), 0xff00ff);
	contextMenu->drawPicture("source.bmp", Point(contextMenu->frame.size.width / 2 + 38, contextMenu->frame.size.height / 2 - 16), 0xff00ff);
	contextMenu->drawPicture("search.bmp", Point(contextMenu->frame.size.width / 2 - 16, contextMenu->frame.size.height - 32 - 3), 0xff00ff);
	contextMenu->drawPicture("refresh.bmp", Point(contextMenu->frame.size.width / 2 - 38 - 32, contextMenu->frame.size.height / 2 - 16), 0xff00ff);

	// GUI タスクを起動
	Task *guiTask = new Task("GUI Task", 1, 2, 256, [] {
		Task &task = *TaskSwitcher::getNowTask();
		
		// キャレットの表示とタイマー設定
		SheetCtl::back->drawLine(
			Line(
				SheetCtl::caretPosition + 2,
				SheetCtl::back->frame.size.height - 20 - 22 + 2,
				SheetCtl::caretPosition + 2,
				SheetCtl::back->frame.size.height - 20 - 22 + 2 + 18
			), SheetCtl::caretColor);
		SheetCtl::back->refresh(Rectangle(SheetCtl::caretPosition + 2, SheetCtl::back->frame.size.height - 20 - 22 + 2, 1, 18));
		SheetCtl::caretColor = 0xffffff;
		SheetCtl::caretTimer = new Timer(queue, 0x80);
		SheetCtl::caretTimer->set(50);
		
		for (;;) {
			Cli();
			if (task.queue->isempty()) {
				task.sleep();
				Sti();
			} else {
				int data = task.queue->pop();
				Sti();
				if (data < 0x80) {
					// from Keyboard Driver
					switch (data) {
						case 0x08: // BS
							if (SheetCtl::caretPosition > 2) {
								SheetCtl::caretPosition -= 8;
								Rectangle clearRange(SheetCtl::caretPosition + 2, SheetCtl::back->frame.size.height - 20 - 22 + 2, 9, 18);
								SheetCtl::back->fillRect(clearRange, 0xffffff);
								SheetCtl::caretColor = 0;
								SheetCtl::back->drawLine(Line(clearRange.offset, clearRange.offset + Point(0, 18)), SheetCtl::caretColor);
								SheetCtl::back->refresh(clearRange);
								SheetCtl::caretTimer->cancel();
								SheetCtl::caretTimer->set(50);
								SheetCtl::tboxString->erase(SheetCtl::tboxString->length() - 1, SheetCtl::tboxString->length());
							}
							break;
						
						case 0x09: // TAB
							if (KeyboardController::alt && SheetCtl::numOfTab >= 2) {
								// タブ切り替え
								int newActive = SheetCtl::activeTab + 1;
								if (newActive >= SheetCtl::numOfTab) newActive = 0;
								
								// 次のタブ
								Rectangle nextTabRange(2, 35 + 23 * newActive, SheetCtl::back->frame.size.width - 2, 22);
								SheetCtl::back->changeColor(nextTabRange, kPassiveTabColor, kActiveTabColor);
								SheetCtl::back->changeColor(nextTabRange, kPassiveTextColor, kActiveTextColor);
								SheetCtl::back->refresh(nextTabRange);
								// アクティブだったタブ
								Rectangle prevTabRange(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.size.width - 2, 22);
								SheetCtl::back->changeColor(prevTabRange, kActiveTabColor, kPassiveTabColor);
								SheetCtl::back->changeColor(prevTabRange, kActiveTextColor, kPassiveTextColor);
								SheetCtl::back->refresh(prevTabRange);
								
								SheetCtl::window[SheetCtl::activeTab]->upDown(-1);
								SheetCtl::window[newActive]->upDown(1);
								
								SheetCtl::activeTab = newActive;
							}
							break;
						
						case 0x0a: { // LF
							if (SheetCtl::tboxString->length() == 0) break;
							
							string filename = *SheetCtl::tboxString;
							
							// file:/// の削除
							if (filename.compare(0, 8, "file:///") == 0) {
								filename.erase(0, 8);
							}
							
							unique_ptr<File> htmlFile(new File(filename));
							if (htmlFile->open()) {
								// ファイルが存在した
								filename = "file:///" + filename;
								// タブ表示
								Rectangle newTabRange(2, 35 + 23 * SheetCtl::numOfTab, SheetCtl::back->frame.size.width - 2, 22);
								SheetCtl::back->drawString(filename, Point(6, 39 + 23 * SheetCtl::numOfTab), kActiveTextColor);
								SheetCtl::back->changeColor(newTabRange, kBackgroundColor, kActiveTabColor);
								SheetCtl::back->refresh(newTabRange);
								// アクティブだったタブ
								Rectangle prevTabRange(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.size.width - 2, 22);
								SheetCtl::back->changeColor(prevTabRange, kActiveTabColor, kPassiveTabColor);
								SheetCtl::back->changeColor(prevTabRange, kActiveTextColor, kPassiveTextColor);
								SheetCtl::back->refresh(prevTabRange);
								// ページ表示
								SheetCtl::window[SheetCtl::numOfTab] = new Sheet(Size(SheetCtl::resolution.width - SheetCtl::back->frame.size.width, SheetCtl::resolution.height), false);
								SheetCtl::window[SheetCtl::numOfTab]->fillRect(SheetCtl::window[SheetCtl::numOfTab]->frame, 0xffffff);
								SheetCtl::window[SheetCtl::numOfTab]->drawRect(SheetCtl::window[SheetCtl::numOfTab]->frame, 0);
								SheetCtl::window[SheetCtl::numOfTab]->moveTo(Point(SheetCtl::back->frame.size.width, 0));
								// レンダリング
								string source(reinterpret_cast<char *>(htmlFile->read().get()), htmlFile->size);
								HTML::Tokenizer tokenizer;
								Queue<shared_ptr<HTML::Token>> &tokens = tokenizer.tokenize(source.c_str());
								for (int i = 0; !tokens.isempty() && 1 + i * 16 + 16 < SheetCtl::back->frame.size.height - 1; ++i) {
									string str;
									shared_ptr<HTML::Token> token(tokens.pop());
									switch (token->type) {
										case HTML::Token::Type::Character:
											str = "Character Token";
											break;
											
										case HTML::Token::Type::StartTag:
											str = "StartTag Token";
											break;
											
										case HTML::Token::Type::EndTag:
											str = "EndTag Token";
											break;
											
										case HTML::Token::Type::DOCTYPE:
											str = "DOCTYPE Token";
											break;
											
										case HTML::Token::Type::Comment:
											str = "Comment Token";
											break;
											
										case HTML::Token::Type::EndOfFile:
											str = "EndOfFile Token";
											break;
									}
									str += " (data='" + token->data + "')";
									SheetCtl::window[SheetCtl::numOfTab]->drawString(str, Point(1, 1 + i * 16), 0);
								}
								SheetCtl::window[SheetCtl::activeTab]->upDown(-1);
								SheetCtl::window[SheetCtl::numOfTab]->upDown(1);
								SheetCtl::activeTab = SheetCtl::numOfTab;
								++SheetCtl::numOfTab;
							} else {
								// 一致するファイルなし
								filename = "file:///" + filename;
								// タブ表示
								Rectangle newTabRange(2, 35 + 23 * SheetCtl::numOfTab, SheetCtl::back->frame.size.width - 2, 22);
								SheetCtl::back->drawString(filename, Point(6, 39 + 23 * SheetCtl::numOfTab), kActiveTextColor);
								SheetCtl::back->changeColor(newTabRange, kBackgroundColor, kActiveTabColor);
								SheetCtl::back->refresh(newTabRange);
								// アクティブだったタブ
								Rectangle prevTabRange(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.size.width - 2, 22);
								SheetCtl::back->changeColor(prevTabRange, kActiveTabColor, kPassiveTabColor);
								SheetCtl::back->changeColor(prevTabRange, kActiveTextColor, kPassiveTextColor);
								SheetCtl::back->refresh(prevTabRange);
								// ページ表示
								SheetCtl::window[SheetCtl::numOfTab] = new Sheet(Size(SheetCtl::resolution.width - SheetCtl::back->frame.size.width, SheetCtl::resolution.height), false);
								SheetCtl::window[SheetCtl::numOfTab]->fillRect(SheetCtl::window[SheetCtl::numOfTab]->frame, 0xffffff);
								SheetCtl::window[SheetCtl::numOfTab]->drawRect(SheetCtl::window[SheetCtl::numOfTab]->frame, 0);
								SheetCtl::window[SheetCtl::numOfTab]->moveTo(Point(SheetCtl::back->frame.size.width, 0));
								// レンダリング
								SheetCtl::window[SheetCtl::numOfTab]->drawString("File not found", Point(1, 1), 0);
								SheetCtl::window[SheetCtl::numOfTab]->drawString("Can't find the file at '" + filename + "'", Point(1, 1 + 16), 0);
								SheetCtl::window[SheetCtl::activeTab]->upDown(-1);
								SheetCtl::window[SheetCtl::numOfTab]->upDown(1);
								SheetCtl::activeTab = SheetCtl::numOfTab;
								++SheetCtl::numOfTab;
							}
							
							*SheetCtl::tboxString = "";
							SheetCtl::caretPosition = 2;
							Rectangle clearRange(2, SheetCtl::back->frame.size.height - 20 - 22, SheetCtl::back->frame.size.width - 2 - 2, 22);
							SheetCtl::back->fillRect(clearRange, 0xffffff);
							SheetCtl::back->refresh(clearRange);
							break;
						}
						
						default: {
							char s[2];
							s[0] = static_cast<char>(data);
							s[1] = 0;
							SheetCtl::back->drawLine(
								Line(
									SheetCtl::caretPosition + 2,
									SheetCtl::back->frame.size.height - 20 - 22 + 2,
									SheetCtl::caretPosition + 2,
									SheetCtl::back->frame.size.height - 20 - 22 + 2 + 18
								), 0xffffff);
							SheetCtl::back->drawString(s, Point(SheetCtl::caretPosition + 2, SheetCtl::back->frame.size.height - 20 - 22 + 3), 0);
							SheetCtl::caretPosition += 8;
							SheetCtl::caretColor = 0;
							SheetCtl::back->drawLine(
								Line(
									SheetCtl::caretPosition + 2,
									SheetCtl::back->frame.size.height - 20 - 22 + 2,
									SheetCtl::caretPosition + 2,
									SheetCtl::back->frame.size.height - 20 - 22 + 2 + 18
								), SheetCtl::caretColor);
							SheetCtl::back->refresh(Rectangle(SheetCtl::caretPosition - 8 + 2, SheetCtl::back->frame.size.height - 20 - 22 + 2, 9, 18));
							SheetCtl::caretTimer->cancel();
							SheetCtl::caretTimer->set(50);
							*SheetCtl::tboxString += s[0];
							break;
						}
					}
				} else if (data == 0x80) {
					// キャレットカーソル用タイマーのタイムアウト
					SheetCtl::back->drawLine(
						Line(
							SheetCtl::caretPosition + 2,
							SheetCtl::back->frame.size.height - 20 - 22 + 2,
							SheetCtl::caretPosition + 2,
							SheetCtl::back->frame.size.height - 20 - 22 + 2 + 18
						), SheetCtl::caretColor);
					SheetCtl::back->refresh(Rectangle(SheetCtl::caretPosition + 2, SheetCtl::back->frame.size.height - 20 - 22 + 2, 1, 18));
					SheetCtl::caretColor ^= 0xffffff;
					SheetCtl::caretTimer->set(50);
				} else if (data < 260) {
					// from Mouse Driver
					switch (data) {
						case 256: // move
							mouseCursorSheet->moveTo(mouseCursorPos + Point(-8, -8));
							break;
						
						case 257: // left click
							// Close the context menu
							if (contextMenu->height > 0) {
								contextMenu->upDown(-1);
							}
							
							// 各シートの onClick イベントを発動
							for (int i = SheetCtl::top - 1; i >= 0; --i) {
								Sheet &sht = *sheets[i];
								if (sht.onClick && sht.frame.contains(mouseCursorPos)) {
									sht.onClick(mouseCursorPos);
									break;
								}
							}
							break;
						
						case 258: // right click
							if (contextMenu->height < 0) {
								// Open the context menu
								contextMenu->moveTo(Point(mouseCursorPos.x - contextMenu->frame.size.width / 2, mouseCursorPos.y - contextMenu->frame.size.height / 2));
								contextMenu->upDown(top);
							}
							break;
						
						case 259: // scroll up
							break;
						
						case 260: // scroll down
							break;
					}
				}
			}
		}
	});
	queue = guiTask->queue;
}

// 指定範囲の変更をmapに適用
void SheetCtl::refreshMap(const Rectangle &range, int h0) {
	int bx0, by0, bx1, by1, sid4;
	int vx0 = max(0, range.offset.x), vy0 = max(0, range.offset.y);
	int vx1 = min(resolution.width, range.getEndPoint().x), vy1 = min(resolution.height, range.getEndPoint().y);
	for (int sid = h0; sid <= top; ++sid) {
		const Sheet &sht = *sheets[sid];
		bx0 = max(0, vx0 - sht.frame.offset.x);
		by0 = max(0, vy0 - sht.frame.offset.y);
		bx1 = min(sht.frame.size.width, vx1 - sht.frame.offset.x);
		by1 = min(sht.frame.size.height, vy1 - sht.frame.offset.y);
		if (!sht.nonRect) {
			if (!(sht.frame.offset.x & 3) && !(bx0 & 3) && !(bx1 & 3)) {
				/* 透明色なし専用の高速版（4バイト型） */
				bx1 = (bx1 - bx0) / 4;
				sid4 = sid | sid << 8 | sid << 16 | sid << 24;
				for (int by = by0; by < by1; ++by) {
					for (int bx = 0; bx < bx1; ++bx) {
						((int*) &map[(sht.frame.offset.y + by) * resolution.width + sht.frame.offset.x + bx0])[bx] = sid4;
					}
				}
			} else {
				/* 透明色なし専用の高速版（1バイト型） */
				for (int by = by0; by < by1; ++by) {
					for (int bx = bx0; bx < bx1; ++bx) {
						map[(sht.frame.offset.y + by) * resolution.width + sht.frame.offset.x + bx] = sid;
					}
				}
			}
		} else {
			/* 透明色ありの一般版（1バイト型） */
			for (int by = by0; by < by1; ++by) {
				for (int bx = bx0; bx < bx1; ++bx) {
					if ((unsigned char) (sht.buf[by * sht.frame.size.width + bx] >> 24) != 255) {
						map[(sht.frame.offset.y + by) * resolution.width + sht.frame.offset.x + bx] = sid;
					}
				}
			}
		}
	}
}

// 指定範囲の変更をvramに適用
void SheetCtl::refreshSub(const Rectangle &range, int h1) {
	int bx0, by0, bx1, by1;
	unsigned int rgb;

	int vx0 = max(0, range.offset.x), vy0 = max(0, range.offset.y);
	int vx1 = min(resolution.width, range.getEndPoint().x), vy1 = min(resolution.height, range.getEndPoint().y);
	unique_ptr<unsigned int> backrgb(new unsigned int[(vx1 - vx0) * (vy1 - vy0)]);

	for (int sid = 0; sid <= h1; ++sid) {
		const Sheet &sht = *sheets[sid];
		/* vx0～vy1を使って、bx0～by1を逆算する */
		bx0 = max(0, vx0 - sht.frame.offset.x);
		by0 = max(0, vy0 - sht.frame.offset.y);
		bx1 = min(sht.frame.size.width, vx1 - sht.frame.offset.x);
		by1 = min(sht.frame.size.height, vy1 - sht.frame.offset.y);
		if (color == 32) {
			for (int by = by0; by < by1; ++by) {
				for (int bx = bx0; bx < bx1; ++bx) {
					rgb = sht.buf[by * sht.frame.size.width + bx];
					if (map[(sht.frame.offset.y + by) * resolution.width + sht.frame.offset.x + bx] == sid) {
						vram.p32[((sht.frame.offset.y + by) * resolution.width + (sht.frame.offset.x + bx))]
							= (sid <= 1) ? rgb
							           : MixRgb(rgb, backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)]);
					} else if ((unsigned char)(rgb >> 24) != 255) {
						backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)]
							= (sid <= 1) ? rgb
							           : MixRgb(rgb, backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)]);
					}
				}
			}
		} else if (color == 24) {
			for (int by = by0; by < by1; ++by) {
				for (int bx = bx0; bx < bx1; ++bx) {
					rgb = sht.buf[by * sht.frame.size.width + bx];
					if (map[(sht.frame.offset.y + by) * resolution.width + sht.frame.offset.x + bx] == sid) {
						if (sid > 1) {
							rgb = MixRgb(rgb, backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)]);
						}
						vram.p24[(sht.frame.offset.y + by) * resolution.width + (sht.frame.offset.x + bx)][0] = (unsigned char)rgb;
						vram.p24[(sht.frame.offset.y + by) * resolution.width + (sht.frame.offset.x + bx)][1] = (unsigned char)(rgb >> 8);
						vram.p24[(sht.frame.offset.y + by) * resolution.width + (sht.frame.offset.x + bx)][2] = (unsigned char)(rgb >> 16);
					} else if ((unsigned char)(rgb >> 24) != 255) {
						backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)]
						= (sid <= 1) ? rgb
						: MixRgb(rgb, backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)]);
					}
				}
			}
		} else if (color == 16) {
			for (int by = by0; by < by1; ++by) {
				for (int bx = bx0; bx < bx1; ++bx) {
					rgb = sht.buf[by * sht.frame.size.width + bx];
					if (map[(sht.frame.offset.y + by) * resolution.width + sht.frame.offset.x + bx] == sid) {
						vram.p16[(sht.frame.offset.y + by) * resolution.width + (sht.frame.offset.x + bx)]
						              = (sid <= 1) ?
						            		  ((((unsigned char) (rgb >> 16) << 8) & 0xf800)
								                 | (((unsigned char) (rgb >> 8) << 3) & 0x07e0)
								                 | ((unsigned char) rgb >> 3)) :
						                	  ((((((unsigned char) (backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)] >> 16) - (unsigned char) (rgb >> 16)) * (unsigned char) (rgb >> 24) / 255 + (unsigned char) (rgb >> 16)) << 8) & 0xf800)
								                 | (((((unsigned char) (backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)] >> 8) - (unsigned char) (rgb >> 8)) * (unsigned char) (rgb >> 24) / 255 + (unsigned char) (rgb >> 8)) << 3) & 0x07e0)
								                 | (((unsigned char) backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)] - (unsigned char) rgb) * (unsigned char) (rgb >> 24) / 255 + (unsigned char) rgb) >> 3);
					} else if ((unsigned char) (rgb >> 24) != 255) {
						backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)]
						        = (sid <= 1) ?
						        		rgb :
						        		MixRgb(rgb, backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)]);
					}
				}
			}
		}
	}
}
