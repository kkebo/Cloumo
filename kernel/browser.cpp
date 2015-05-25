#include "../headers.h"
#include <string.h>

Browser::Browser(const char *url) {
	// ファイル読み込み
	File htmlFile(url);
	if (htmlFile.open()) {
		source.reset(htmlFile.read());
		size = htmlFile.size;
		
		// 内部シート作成
		sheet.reset(new Sheet(Size(SheetCtl::scrnx_ - SheetCtl::back_->bxsize - 1, SheetCtl::scrny_ * 3), false));
		sheet->fillRect(Rectangle(sheet->bxsize - 1, sheet->bysize - 1), 0xffffff);
		
		// マウスに登録
		Mouse::browserTask = TaskSwitcher::getNowTask();
	}
}

// HTMLファイルを表示
void Browser::Render() {
	int x = 15, y = 15, /*titlex = 50, */j;
	Encoding encode = Encoding::SJIS;
	char s[11];
	bool uline = false, bold = false, title = false, line = false, pre = false, list = false;
	unsigned int fcolor = 0;
	//char text[240 * 1024];
	//int tcount = 0;

	for (int i = 0; i < size; ++i) {
		if (source[i] == '<') { // タグ
			++i;
			// 要素名を小文字でsに代入
			for (j = 0; source[i] != ' ' && source[i] != '>' && j < 10 && i < size; ++i, ++j) {
				if ('A' <= source[i] && source[i] <= 'Z') source[i] += 0x20;
				s[j] = source[i];
			}
			s[j] = 0;

			if (!strcmp(s, "meta")) {
				if (source[i] == ' ') ++i;
				if (!strncmpi((char*)source + i, "charset=", 8)) {
					i += 8;
					if (source[i] == '"') ++i;
					if (!strncmpi((char*)source + i, "shift_jis", 9)) {
						encode = Encoding::SJIS;
					} else if (!strncmpi((char*)source + i, "utf-8", 5)) {
						encode = Encoding::UTF8;
					} else if (!strncmpi((char*)source + i, "euc-jp", 6)) {
						encode = Encoding::EUCJP;
					}
				} else if (!strncmpi((char*)source + i, "http-equiv=\"content-type\" ", 26)) {
					i += 26;
					if (!strncmpi((char*)source + i, "content=\"", 9)) {
						i += 9;
						while (source[i] != '"' && source[i] != '>' && i + 8 < size) {
							if (!strncmp((char*)source + i, "charset=", 8)) {
								i += 8;
								if (!strncmpi((char*)source + i, "shift_jis", 9)) {
									encode = Encoding::SJIS;
									break;
								} else if (!strncmpi((char*)source + i, "utf-8", 5)) {
									encode = Encoding::UTF8;
									break;
								} else if (!strncmpi((char*)source + i, "euc-jp", 6)) {
									encode = Encoding::EUCJP;
									break;
								}
							}
							++i;
						}
					}
				}
			} else if (!strcmp(s, "title")) {
				title = true;
			} else if (!strcmp(s, "/title")) {
				title = false;
			} else if (!strcmp(s, "br")) {
				y += 18;
				x = 15;
			} else if (!strcmp(s, "hr")) {
				if (x > 15) {
					y += 18;
					x = 15;
				}
				if (y < sheet->bysize - 1 - 16 - 4) {
					SheetCtl::drawLine(sheet, Rgb(0, 0, 0), x, y + 8, sheet->bxsize - 1 - 4, y + 8);
				}
				y += 18;
			} else if (!strcmp(s, "p")) {
				y += 18;
				x = 15;
			} else if (!strcmp(s, "/p") && x > 15) {
				y += 18;
				x = 15;
			} else if (!strcmp(s, "h1")) {
				if (x > 15) {
					y += 18;
					x = 15;
				}
				y += 18;
				x = 15;
				bold = true;
			} else if (!strcmp(s, "/h1")) {
				y += 32;
				x = 15;
				bold = false;
			} else if (!strcmp(s, "h2")) {
				if (x > 15) {
					y += 18;
					x = 15;
				}
				y += 18;
				x = 15;
				bold = true;
			} else if (!strcmp(s, "/h2")) {
				y += 32;
				x = 15;
				bold = false;
			} else if (!strcmp(s, "h3")) {
				if (x > 15) {
					y += 18;
					x = 15;
				}
				y += 18;
				x = 15;
				bold = true;
			} else if (!strcmp(s, "/h3")) {
				y += 32;
				x = 15;
				bold = false;
			} else if (!strcmp(s, "h4")) {
				if (x > 15) {
					y += 18;
					x = 15;
				}
				y += 18;
				x = 15;
				bold = true;
			} else if (!strcmp(s, "/h4")) {
				y += 32;
				x = 15;
				bold = false;
			} else if (!strcmp(s, "h5")) {
				if (x > 15) {
					y += 18;
					x = 15;
				}
				y += 18;
				x = 15;
				bold = true;
			} else if (!strcmp(s, "/h5")) {
				y += 32;
				x = 15;
				bold = false;
			} else if (!strcmp(s, "h6")) {
				if (x > 15) {
					y += 18;
					x = 15;
				}
				y += 18;
				x = 15;
				bold = true;
			} else if (!strcmp(s, "/h6")) {
				y += 32;
				x = 15;
				bold = false;
			} else if (!strcmp(s, "u")) {
				uline = true;
			} else if (!strcmp(s, "/u")) {
				uline = false;
			} else if (!strcmp(s, "b")) {
				bold = true;
			} else if (!strcmp(s, "/b")) {
				bold = false;
			} else if (!strcmp(s, "strong")) {
				bold = true;
			} else if (!strcmp(s, "/strong")) {
				bold = false;
			} else if (!strcmp(s, "div") && x > 15) {
				y += 18;
				x = 15;
			} else if (!strcmp(s, "/div") && x > 15) {
				y += 18;
				x = 15;
			} else if (!strcmp(s, "a")) {
				++i;
				if (!memcmp(source + i, "href=\"", 6) || !memcmp(source + i, "HREF=\"", 6)) {
					uline = true;
					fcolor = Rgb(0, 0, 255);
				}
			} else if (!strcmp(s, "/a")) {
				uline = false;
				fcolor = Rgb(0, 0, 0);
			} else if (!strcmp(s, "button")) {
				for (; source[i] != '>' && i < size; ++i) {}
				++i;
				for (; source[i] != '>' && i < size; ++i) {}
				continue;
			} else if (!strcmp(s, "s")) {
				line = true;
			} else if (!strcmp(s, "/s")) {
				line = false;
			} else if (!strcmp(s, "pre")) {
				pre = true;
			} else if (!strcmp(s, "/pre")) {
				pre = false;
			} else if (!strcmp(s, "ul")) {
				list = true;
			} else if (!strcmp(s, "/ul")) {
				list = false;
			} else if (!strcmp(s, "li") && list) {
				x = 15 + 8;
				y += 18;
				//SheetCtl::fillenn(sheet, 0, x + 4, y + 4, x + 12, y + 12);
				x += 16;
			} else if (!strcmp(s, "/li")) {
			} else if (!strcmp(s, "img")) {
				SheetCtl::drawRect(sheet, 0, x, y, x + 40, y + 16);
				x = 15;
				y += 18;
			}
			for (; source[i] != '>' && i < size; ++i) {}
			continue;
		} else if (source[i] == '&') { // 特殊記号
			++i;
			if (!strncmpi((char*)source + i, "lt", 2)) {	// 不等号より小
				SheetCtl::drawString(sheet, x, y, fcolor, "<");
				x += 8;
				i += 2;
			} else if (!strncmpi((char*)source + i, "gt", 2)) {	// 不等号より大
				SheetCtl::drawString(sheet, x, y, fcolor, ">");
				x += 8;
				i += 2;
			} else if (!strncmpi((char*)source + i, "amp", 3)) {	// アンパサンド
				SheetCtl::drawString(sheet, x, y, fcolor, "&");
				x += 8;
				i += 3;
			} else if (!strncmpi((char*)source + i, "copy", 4)) {	// 著作権記号
				SheetCtl::drawString(sheet, x + 4, y, fcolor, "C");
				//SheetCtl::drawenn(sheet, fcolor, x, y, x + 16, y + 16);
				x += 16;
				i += 4;
			} else if (!strncmpi((char*)source + i, "yen", 3)) {	// 円記号
				SheetCtl::drawString(sheet, x, y, fcolor, "\\");
				x += 8;
				i += 3;
			} else if (!strncmpi((char*)source + i, "reg", 3)) {	// 登録商標記号
				SheetCtl::drawString(sheet, x + 4, y, fcolor, "R");
				//SheetCtl::drawenn(sheet, fcolor, x, y, x + 16, y + 16);
				x += 16;
				i += 3;
			} else if (!strncmpi((char*)source + i, "quot", 4)) {	// 引用符
				SheetCtl::drawString(sheet, x, y, fcolor, "\"");
				x += 8;
				i += 4;
			} else {
				--i;
				continue;
			}
			if (source[i] == ';') ++i;
			continue;
		} else if (source[i] == 0x0a) {
			if (pre) {
				x = 15;
				y += 18;
			} else if (x > 15) {
				for (; source[i] == 0x0a || source[i] == 0x0d || source[i] == ' ' && i < size; ++i) {}
				x += 8;
				--i;
			}
			continue;
		} else if (source[i] == '\t' || (source[i] == ' ' && !title && !pre && x == 15) || source[i] == 0x0d) {
			continue;
		}

		// ウィンドウからx座標がはみ出ていたら無視
		if (x >= sheet->bxsize - 8 - 15)  continue;
		// ウィンドウからy座標がはみ出ていたら終了
		if (y >= sheet->bysize - 16 - 15) break;

		if (0xe2 <= source[i] && source[i] <= 0xef && encode == Encoding::UTF8) { // UTF-8 3バイト全角文字
			s[0] = source[i];
			s[1] = source[i + 1];
			s[2] = source[i + 2];
			s[3] = 0;
			if (title) {
				//SheetCtl::drawString(sheet, titlex - sheet.x0, 5 - sheet.y0, Rgb(0, 0, 0), s);
				//titlex += 16;
			} else {
				SheetCtl::drawString(sheet, x, y, fcolor, s);
				//text[tcount .. tcount + 2] = s[0 .. 2];
				//tcount += 3;
				x += 16;
			}
			i += 2;
			if (bold) SheetCtl::drawString(sheet, x - 15, y, fcolor, s);
			if (uline) SheetCtl::drawLine(sheet, fcolor, x - 16, y + 15, x, y + 15);
			if (line) SheetCtl::drawLine(sheet, fcolor, x - 16, y + 7, x, y + 7);
		} else if ((0xc2 <= source[i] && source[i] <= 0xd1 && encode == Encoding::UTF8)
				   || ((0x81 <= source[i] && source[i] <= 0x9f) || (0xe0 <= source[i] && source[i] <= 0xfc) && encode == Encoding::SJIS)
				   || (0x81 <= source[i] && source[i] <= 0xfe && encode == Encoding::EUCJP)) { // 2バイト全角文字
			s[0] = source[i];
			s[1] = source[i + 1];
			s[2] = 0;
			if (title) {
				//SheetCtl::drawString(sheet, titlex - sheet.x0, 5 - sheet.y0, Rgb(0, 0, 0), s, encode);
				//titlex += 16;
			} else {
				SheetCtl::drawString(sheet, x, y, fcolor, s, encode);
				//text[tcount .. tcount + 1] = s[0 .. 1];
				//tcount += 2;
				x += 16;
			}
			++i;
			if (bold) SheetCtl::drawString(sheet, x - 15, y, fcolor, s, encode);
			if (uline) SheetCtl::drawLine(sheet, fcolor, x - 16, y + 15, x, y + 15);
			if (line) SheetCtl::drawLine(sheet, fcolor, x - 16, y + 7, x, y + 7);
		} else { // 半角文字
			s[0] = source[i];
			s[1] = 0;
			if (title) {
				//SheetCtl::drawString(sheet, titlex - sheet.x0, 5 - sheet.y0, Rgb(0, 0, 0), s);
				//titlex += 8;
			} else {
				SheetCtl::drawString(sheet, x, y, fcolor, s);
				//text[tcount] = s[0];
				//++tcount;
				x += 8;
			}
			if (bold) SheetCtl::drawString(sheet, x - 7, y, fcolor, s);
			if (uline) SheetCtl::drawLine(sheet, fcolor, x - 8, y + 15, x, y + 15);
			if (line) SheetCtl::drawLine(sheet, fcolor, x - 8, y + 7, x, y + 7);
		}
	}
	
	//text[tcount] = 0;
	//SheetCtl::drawString(SheetCtl::back, 0, 0, Rgb(255, 255, 255), text);
	
	// sheet の内容の一部を window_[0] に表示
	dy = 0;
	Mapping();
}

void Browser::Scroll(int data) {
	if (data > 0) {
		dy += 16;
		if (dy > sheet->bysize - SheetCtl::window_[0]->bysize) dy = sheet->bysize - SheetCtl::window_[0]->bysize;
		Mapping();
	} else if (data < 0) {
		dy -= 16;
		if (dy < 0) dy = 0;
		Mapping();
	}
}

void Browser::Mapping() {
	for (int y = 0; y < SheetCtl::window_[0]->bysize - 2; ++y) {
		for (int x = 0; x < SheetCtl::window_[0]->bxsize - 2; ++x) {
			if (SheetCtl::window_[0]->buf[(y + 1) * SheetCtl::window_[0]->bxsize + x + 1] != sheet->buf[(y + dy) * sheet->bxsize + x])
				SheetCtl::window_[0]->buf[(y + 1) * SheetCtl::window_[0]->bxsize + x + 1] = sheet->buf[(y + dy) * sheet->bxsize + x];
		}
	}
}
