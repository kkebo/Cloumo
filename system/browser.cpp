#include "../headers.h"

int strcmpi(const char* string1, const char* string2) {
	return 0;
}

int strncmpi(const char* string1, const char* string2, size_t length) {
	// 仮(これは文字数が多いとかなり遅い)
	char string1Capital[length + 1];
	char string2Capital[length + 1];
	for (int i = 0; i < length; i++) {
		char c1 = string1[i];
		string1Capital[i] = ('A' <= c1 && c1 <= 'Z') ? c1 + 0x20 : c1;
	}
	string1Capital[length] = 0;
	for (int i = 0; i < length; i++) {
		char c2 = string2[i];
		string2Capital[i] = ('A' <= c2 && c2 <= 'Z') ? c2 + 0x20 : c2;
	}
	string2Capital[length] = 0;
	return strncmp(string1Capital, string2Capital, length);
}

// HTMLファイルを表示
void Browser::View(const char* url) {
	int x = 15, y = 15, /*titlex = 50, */j, encode = 0/* Shift_JIS */;
	char s[11];
	bool uline = false, bold = false, title = false, line = false, pre = false, list = false;
	unsigned int fcolor = 0;
	//char text[240 * 1024];
	//int tcount = 0;

	// ファイル読み込み
	File* htmlfile = FAT12::open(url);
	if (!htmlfile) return;
	unsigned char* source = htmlfile->read();
	unsigned int fsize = htmlfile->size();

	for (int i = 0; i < fsize; i++) {
		if (source[i] == '<') { // タグ
			i++;
			// 要素名を小文字でsに代入
			for (j = 0; source[i] != ' ' && source[i] != '>' && j < 10 && i < fsize; i++, j++) {
				if ('A' <= source[i] && source[i] <= 'Z') source[i] += 0x20;
				s[j] = source[i];
			}
			s[j] = 0;

			if (!strcmp(s, "meta")) {
				if (source[i] == ' ') i++;
				if (!strncmpi((char*)source + i, "charset=", 8)) {
					i += 8;
					if (source[i] == '"') i++;
					if (!strncmpi((char*)source + i, "shift_jis", 9)) {
						encode = 0;
					} else if (!strncmpi((char*)source + i, "utf-8", 5)) {
						encode = 1;
					} else if (!strncmpi((char*)source + i, "euc-jp", 6)) {
						encode = 2;
					}
				} else if (!strncmpi((char*)source + i, "http-equiv=\"content-type\" ", 26)) {
					i += 26;
					if (!strncmpi((char*)source + i, "content=\"", 9)) {
						i += 9;
						while (source[i] != '"' && source[i] != '>' && i + 8 < fsize) {
							if (!strncmp((char*)source + i, "charset=", 8)) {
								i += 8;
								if (!strncmpi((char*)source + i, "shift_jis", 9)) {
									encode = 0;
									break;
								} else if (!strncmpi((char*)source + i, "utf-8", 5)) {
									encode = 1;
									break;
								} else if (!strncmpi((char*)source + i, "euc-jp", 6)) {
									encode = 2;
									break;
								}
							}
							i++;
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
				if (y < SheetCtl::window_[0]->bysize - 1 - 16 - 4) {
					SheetCtl::drawLine(SheetCtl::window_[0], Rgb(0, 0, 0), x, y + 8, SheetCtl::window_[0]->bxsize - 1 - 4, y + 8);
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
				i++;
				if (!memcmp(source + i, "href=\"", 6) || !memcmp(source + i, "HREF=\"", 6)) {
					uline = true;
					fcolor = Rgb(0, 0, 255);
				}
			} else if (!strcmp(s, "/a")) {
				uline = false;
				fcolor = Rgb(0, 0, 0);
			} else if (!strcmp(s, "button")) {
				for (; source[i] != '>' && i < fsize; i++) {}
				i++;
				for (; source[i] != '>' && i < fsize; i++) {}
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
				//SheetCtl::fillenn(SheetCtl::window_[0], 0, x + 4, y + 4, x + 12, y + 12);
				x += 16;
			} else if (!strcmp(s, "/li")) {
			} else if (!strcmp(s, "img")) {
				SheetCtl::drawRect(SheetCtl::window_[0], 0, x, y, x + 40, y + 16);
				x = 15;
				y += 18;
			}
			for (; source[i] != '>' && i < fsize; i++) {}
			continue;
		} else if (source[i] == '&') { // 特殊記号
			i++;
			if (!strncmpi((char*)source + i, "lt", 2)) {	// 不等号より小
				SheetCtl::drawString(SheetCtl::window_[0], x, y, fcolor, "<");
				x += 8;
				i += 2;
			} else if (!strncmpi((char*)source + i, "gt", 2)) {	// 不等号より大
				SheetCtl::drawString(SheetCtl::window_[0], x, y, fcolor, ">");
				x += 8;
				i += 2;
			} else if (!strncmpi((char*)source + i, "amp", 3)) {	// アンパサンド
				SheetCtl::drawString(SheetCtl::window_[0], x, y, fcolor, "&");
				x += 8;
				i += 3;
			} else if (!strncmpi((char*)source + i, "copy", 4)) {	// 著作権記号
				SheetCtl::drawString(SheetCtl::window_[0], x + 4, y, fcolor, "C");
				//SheetCtl::drawenn(SheetCtl::window_[0], fcolor, x, y, x + 16, y + 16);
				x += 16;
				i += 4;
			} else if (!strncmpi((char*)source + i, "yen", 3)) {	// 円記号
				SheetCtl::drawString(SheetCtl::window_[0], x, y, fcolor, "\\");
				x += 8;
				i += 3;
			} else if (!strncmpi((char*)source + i, "reg", 3)) {	// 登録商標記号
				SheetCtl::drawString(SheetCtl::window_[0], x + 4, y, fcolor, "R");
				//SheetCtl::drawenn(SheetCtl::window_[0], fcolor, x, y, x + 16, y + 16);
				x += 16;
				i += 3;
			} else if (!strncmpi((char*)source + i, "quot", 4)) {	// 引用符
				SheetCtl::drawString(SheetCtl::window_[0], x, y, fcolor, "\"");
				x += 8;
				i += 4;
			} else {
				i--;
				continue;
			}
			if (source[i] == ';') i++;
			continue;
		} else if (source[i] == 0x0a) {
			if (pre) {
				x = 15;
				y += 18;
			} else if (x > 15) {
				for (; source[i] == 0x0a || source[i] == 0x0d || source[i] == ' ' && i < fsize; i++) {}
				x += 8;
				i--;
			}
			continue;
		} else if (source[i] == '\t' || (source[i] == ' ' && !title && !pre && x == 15) || source[i] == 0x0d) {
			continue;
		}

		// ウィンドウからx座標がはみ出ていたら無視
		if (x >= SheetCtl::window_[0]->bxsize - 8 - 15)  continue;
		// ウィンドウからy座標がはみ出ていたら終了
		if (y >= SheetCtl::window_[0]->bysize - 16 - 15) break;

		if (0xe2 <= source[i] && source[i] <= 0xef && encode == 1) { // UTF-8 3バイト全角文字
			s[0] = source[i];
			s[1] = source[i + 1];
			s[2] = source[i + 2];
			s[3] = 0;
			if (title) {
				//SheetCtl::drawString(SheetCtl::window_[0], titlex - SheetCtl::window_[0].x0, 5 - SheetCtl::window_[0].y0, Rgb(0, 0, 0), s);
				//titlex += 16;
			} else {
				SheetCtl::drawString(SheetCtl::window_[0], x, y, fcolor, s);
				//text[tcount .. tcount + 2] = s[0 .. 2];
				//tcount += 3;
				x += 16;
			}
			i += 2;
			if (bold) SheetCtl::drawString(SheetCtl::window_[0], x - 15, y, fcolor, s);
			if (uline) SheetCtl::drawLine(SheetCtl::window_[0], fcolor, x - 16, y + 15, x, y + 15);
			if (line) SheetCtl::drawLine(SheetCtl::window_[0], fcolor, x - 16, y + 7, x, y + 7);
		} else if ((0xc2 <= source[i] && source[i] <= 0xd1 && encode == 1)
			|| ((0x81 <= source[i] && source[i] <= 0x9f) || (0xe0 <= source[i] && source[i] <= 0xfc) && encode == 0)
			|| (0x81 <= source[i] && source[i] <= 0xfe && encode == 2)) { // 2バイト全角文字
			s[0] = source[i];
			s[1] = source[i + 1];
			s[2] = 0;
			if (title) {
				//SheetCtl::drawString(SheetCtl::window_[0], titlex - SheetCtl::window_[0].x0, 5 - SheetCtl::window_[0].y0, Rgb(0, 0, 0), s, encode);
				//titlex += 16;
			} else {
				SheetCtl::drawString(SheetCtl::window_[0], x, y, fcolor, s, encode);
				//text[tcount .. tcount + 1] = s[0 .. 1];
				//tcount += 2;
				x += 16;
			}
			i++;
			if (bold) SheetCtl::drawString(SheetCtl::window_[0], x - 15, y, fcolor, s, encode);
			if (uline) SheetCtl::drawLine(SheetCtl::window_[0], fcolor, x - 16, y + 15, x, y + 15);
			if (line) SheetCtl::drawLine(SheetCtl::window_[0], fcolor, x - 16, y + 7, x, y + 7);
		} else { // 半角文字
			s[0] = source[i];
			s[1] = 0;
			if (title) {
				//SheetCtl::drawString(SheetCtl::window_[0], titlex - SheetCtl::window_[0].x0, 5 - SheetCtl::window_[0].y0, Rgb(0, 0, 0), s);
				//titlex += 8;
			} else {
				SheetCtl::drawString(SheetCtl::window_[0], x, y, fcolor, s);
				//text[tcount] = s[0];
				//tcount++;
				x += 8;
			}
			if (bold) SheetCtl::drawString(SheetCtl::window_[0], x - 7, y, fcolor, s);
			if (uline) SheetCtl::drawLine(SheetCtl::window_[0], fcolor, x - 8, y + 15, x, y + 15);
			if (line) SheetCtl::drawLine(SheetCtl::window_[0], fcolor, x - 8, y + 7, x, y + 7);
		}
	}

	SheetCtl::refresh(SheetCtl::window_[0], 0, 0, SheetCtl::window_[0]->bxsize, SheetCtl::window_[0]->bysize);
	//text[tcount] = 0;
	//SheetCtl::drawString(SheetCtl::back, 0, 0, Rgb(255, 255, 255), text);

	delete htmlfile;
}
