#include "../headers.h"
#include <SmartPointer.h>
#include <MinMax.h>

Sheet::Sheet(const Vector &size, bool inv, void (*click)()) : buf(new unsigned int[size.getArea()]), frame(Point(), size), trans(inv), onClick(click) {}

Sheet::~Sheet() {
	if (height >= 0) upDown(-1);
	flags = false;
	delete[] buf;
}

void *Sheet::operator new(size_t size) {
	for (int i = 0; i < kMaxSheets; ++i) {
		if (!SheetCtl::sheets0[i].flags) {
			Sheet *sht = &SheetCtl::sheets0[i];
			sht->flags = true;
			sht->height = -1;
			return sht;
		}
	}
	return nullptr;
}

// シートの高さを変更
void Sheet::upDown(int height) {
	int old = this->height;

	if (height > SheetCtl::top + 1) height = SheetCtl::top + 1;
	if (height < -1) height = -1;
	this->height = height;

	if (old > height) {	// 前より低くなった
		if (height >= 0) {	// 表示
			for (int h = old; h > height; --h) {
				SheetCtl::sheets[h] = SheetCtl::sheets[h - 1];
				SheetCtl::sheets[h]->height = h;
			}
			SheetCtl::sheets[height] = this;
			SheetCtl::refreshMap(frame, height + 1);
			SheetCtl::refreshSub(frame, SheetCtl::top);
		} else {	// 非表示
			if (SheetCtl::top > old) {
				for (int h = old; h < SheetCtl::top; ++h) {
					SheetCtl::sheets[h] = SheetCtl::sheets[h + 1];
					SheetCtl::sheets[h]->height = h;
				}
			}
			--SheetCtl::top;
			SheetCtl::refreshMap(frame, 0);
			SheetCtl::refreshSub(frame, SheetCtl::top);
		}
	} else if (old < height) {	// 以前より高くなった
		if (old >= 0) {	// より高く
			for (int h = old; h < height; ++h) {
				SheetCtl::sheets[h] = SheetCtl::sheets[h + 1];
				SheetCtl::sheets[h]->height = h;
			}
			SheetCtl::sheets[height] = this;
		} else {	// 非表示から表示へ
			for (int h = SheetCtl::top; h >= height; --h) {
				SheetCtl::sheets[h + 1] = SheetCtl::sheets[h];
				SheetCtl::sheets[h + 1]->height = h + 1;
			}
			SheetCtl::sheets[height] = this;
			++SheetCtl::top;
		}
		SheetCtl::refreshMap(frame, height);
		SheetCtl::refreshSub(frame, SheetCtl::top);
	}
}

// シートのリフレッシュ
void Sheet::refresh(const Rect &range) {
	if (height >= 0) {	// 非表示シートはリフレッシュしない
		SheetCtl::refreshMap(range.slideAndClone(frame.offset), height);
		SheetCtl::refreshSub(range.slideAndClone(frame.offset), height);
	}
}

// シートを移動
void Sheet::slide(const Point &cod) {
	Rect oldFrame(frame);
	frame.offset = cod;
	if (height >= 0) {	// 非表示シートはリフレッシュしない
		SheetCtl::refreshMap(oldFrame, 0);
		SheetCtl::refreshMap(frame, height);
		SheetCtl::refreshSub(oldFrame, height - 1);
		SheetCtl::refreshSub(frame, height);
	}
}

Task *SheetCtl::refreshTask = nullptr;
int SheetCtl::top        = -1;
int SheetCtl::tbox_cpos_  = 2;
unsigned int SheetCtl::tbox_col_ = 0;
Timer *SheetCtl::tbox_timer_ = nullptr;
string *SheetCtl::tbox_str_ = nullptr;
unsigned char *SheetCtl::vram = nullptr;
int SheetCtl::scrnx      = 0;
int SheetCtl::scrny      = 0;
unsigned char *SheetCtl::map = nullptr;
Sheet *SheetCtl::back    = nullptr;
Sheet *SheetCtl::contextMenu = nullptr;
Sheet **SheetCtl::window = nullptr;
int SheetCtl::numOfTab    = 0;
int SheetCtl::activeTab   = 0;
Sheet *SheetCtl::sheets0 = nullptr;
Sheet **SheetCtl::sheets = nullptr;
int SheetCtl::color      = 0;
unsigned char *SheetCtl::adrfont = nullptr;

// シートコントロールを初期化
void SheetCtl::init() {
	/* オブジェクト初期化 */
	BootInfo *binfo = (BootInfo *)ADDRESS_BOOTINFO;
	vram    = binfo->vram;
	scrnx   = binfo->scrnx;
	scrny   = binfo->scrny;
	color   = binfo->vmode;
	map     = new unsigned char[scrnx * scrny];
	sheets  = new Sheet*[kMaxSheets];
	sheets0 = ::new Sheet[kMaxSheets];
	window  = new Sheet*[kMaxTabs];
	tbox_str_ = new string();
	numOfTab = 1;
	for (int i = 0; i < kMaxSheets; ++i) {
		sheets0[i].flags = false; /* 未使用マーク */
	}

	/* フォント読み込み */
	adrfont = FAT12::open("japanese.fnt")->read();

	/* サイドバー */
	back = new Sheet(Vector(150, scrny), false, [](const Point &cod) {
		for (int i = 0; i < SheetCtl::numOfTab; ++i) {
			if (i != SheetCtl::activeTab && 35 + 23 * i <= cod.y && cod.y < 33 + 16 + 8 + 23 * i) {
				// 選択したタブ
				SheetCtl::colorChange(*SheetCtl::back, 2, 35 + 23 * i, SheetCtl::back->frame.vector.x, 33 + 16 + 8 + 23 * i, kPassiveTabColor, kActiveTabColor);
				SheetCtl::colorChange(*SheetCtl::back, 2, 35 + 23 * i, SheetCtl::back->frame.vector.x, 33 + 16 + 8 + 23 * i, kPassiveTextColor, kActiveTextColor);
				SheetCtl::back->refresh(Rect(2, 35 + 23 * i, SheetCtl::back->frame.vector.x - 2, 22));
				// アクティブだったタブ
				SheetCtl::colorChange(*SheetCtl::back, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.vector.x, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTabColor, kPassiveTabColor);
				SheetCtl::colorChange(*SheetCtl::back, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.vector.x, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTextColor, kPassiveTextColor);
				SheetCtl::back->refresh(Rect(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.vector.x - 2, 22));
				
				SheetCtl::window[SheetCtl::activeTab]->upDown(-1);
				SheetCtl::window[i]->upDown(1);
				
				SheetCtl::activeTab = i;
			}
		}
	});
	// 背景色
	fillRect(back, kBackgroundColor, 0, 0, back->frame.vector.x, back->frame.vector.y);
	// 戻る・進むボタン枠
	drawPicture(back, 4, 4, "b_f.bmp", Rgb(255, 0, 255));
	// 更新ボタン枠
	drawPicture(back, 59, 4, "btn_r.bmp", Rgb(255, 0, 255));
	// タブ
	drawString(back, 6, 39, kActiveTextColor, "system info");
	colorChange(*back, 2, 35, back->frame.vector.x, 33 + 16 + 8, kBackgroundColor, kActiveTabColor);
	// 検索窓
	fillRect(back, Rgb(255, 255, 255), 2, back->frame.vector.y - 20 - 22, back->frame.vector.x - 2, back->frame.vector.y - 20);
	// 表示設定
	back->upDown(0);

	// tabs
	window[0] = new Sheet(Vector(scrnx - back->frame.vector.x, scrny), false);
	drawRect(window[0], 0, 0, 0, window[0]->frame.vector.x, window[0]->frame.vector.y);
	fillRect(window[0], Rgb(255, 255, 255), 1, 1, window[0]->frame.vector.x - 1, window[0]->frame.vector.y - 1);
	window[0]->slide(Point(back->frame.vector.x, 0));

	// system info タブを全面へ
	window[0]->upDown(1);

	/* 右クリックメニュー */
	contextMenu = new Sheet(Vector(150, 150), true);
	fillRect(contextMenu, kTransColor, 0, 0, contextMenu->frame.vector.x, contextMenu->frame.vector.y);
	//gradCircle(contextMenu, Rgb(200, 230, 255, 50), Rgb(100, 150, 255), 0, 0, 150);
	fillCircle(contextMenu, 0x19e0e0e0, 0, 0, 150);
	//drawCircle(contextMenu, Rgb(0, 0, 255, 50), 0, 0, 150);
	fillCircle(contextMenu, kTransColor, 40, 40, 70);
	//drawCircle(contextMenu, Rgb(0, 0, 255, 50), 40, 40, 70);
	drawPicture(contextMenu, contextMenu->frame.vector.x / 2 - 16, 3, "copy.bmp", Rgb(255, 0, 255));
	drawPicture(contextMenu, contextMenu->frame.vector.x / 2 + 38, contextMenu->frame.vector.y / 2 - 16, "source.bmp", Rgb(255, 0, 255));
	drawPicture(contextMenu, contextMenu->frame.vector.x / 2 - 16, contextMenu->frame.vector.y - 32 - 3, "search.bmp", Rgb(255, 0, 255));
	drawPicture(contextMenu, contextMenu->frame.vector.x / 2 - 38 - 32, contextMenu->frame.vector.y / 2 - 16, "refresh.bmp", Rgb(255, 0, 255));
}

// 指定範囲の変更をmapに適用
void SheetCtl::refreshMap(const Rect &range, int h0) {
	int bx0, by0, bx1, by1, sid4;
	int vx0 = max(0, range.offset.x), vy0 = max(0, range.offset.y);
	int vx1 = min(scrnx, range.getEndPoint().x), vy1 = min(scrny, range.getEndPoint().y);
	for (int sid = h0; sid <= top; ++sid) {
		const Sheet &sht = *sheets[sid];
		bx0 = max(0, vx0 - sht.frame.offset.x);
		by0 = max(0, vy0 - sht.frame.offset.y);
		bx1 = min(sht.frame.vector.x, vx1 - sht.frame.offset.x);
		by1 = min(sht.frame.vector.y, vy1 - sht.frame.offset.y);
		if (!sht.trans) {
			if (!(sht.frame.offset.x & 3) && !(bx0 & 3) && !(bx1 & 3)) {
				/* 透明色なし専用の高速版（4バイト型） */
				bx1 = (bx1 - bx0) / 4;
				sid4 = sid | sid << 8 | sid << 16 | sid << 24;
				for (int by = by0; by < by1; ++by) {
					for (int bx = 0; bx < bx1; ++bx) {
						((int*) &map[(sht.frame.offset.y + by) * scrnx + sht.frame.offset.x + bx0])[bx] = sid4;
					}
				}
			} else {
				/* 透明色なし専用の高速版（1バイト型） */
				for (int by = by0; by < by1; ++by) {
					for (int bx = bx0; bx < bx1; ++bx) {
						map[(sht.frame.offset.y + by) * scrnx + sht.frame.offset.x + bx] = sid;
					}
				}
			}
		} else {
			/* 透明色ありの一般版（1バイト型） */
			for (int by = by0; by < by1; ++by) {
				for (int bx = bx0; bx < bx1; ++bx) {
					if ((unsigned char) (sht.buf[by * sht.frame.vector.x + bx] >> 24) != 255) {
						map[(sht.frame.offset.y + by) * scrnx + sht.frame.offset.x + bx] = sid;
					}
				}
			}
		}
	}
}

// 指定範囲の変更をvramに適用
void SheetCtl::refreshSub(const Rect &range, int h1) {
	int bx0, by0, bx1, by1;
	unsigned int rgb;

	int vx0 = max(0, range.offset.x), vy0 = max(0, range.offset.y);
	int vx1 = min(scrnx, range.getEndPoint().x), vy1 = min(scrny, range.getEndPoint().y);
	unique_ptr<unsigned int> backrgb(new unsigned int[(vx1 - vx0) * (vy1 - vy0)]);

	for (int sid = 0; sid <= h1; ++sid) {
		const Sheet &sht = *sheets[sid];
		/* vx0～vy1を使って、bx0～by1を逆算する */
		bx0 = max(0, vx0 - sht.frame.offset.x);
		by0 = max(0, vy0 - sht.frame.offset.y);
		bx1 = min(sht.frame.vector.x, vx1 - sht.frame.offset.x);
		by1 = min(sht.frame.vector.y, vy1 - sht.frame.offset.y);
		if (color == 32) {
			for (int by = by0; by < by1; ++by) {
				for (int bx = bx0; bx < bx1; ++bx) {
					rgb = sht.buf[by * sht.frame.vector.x + bx];
					if (map[(sht.frame.offset.y + by) * scrnx + sht.frame.offset.x + bx] == sid) {
						((unsigned int *)vram)[((sht.frame.offset.y + by) * scrnx + (sht.frame.offset.x + bx))]
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
					rgb = sht.buf[by * sht.frame.vector.x + bx];
					if (map[(sht.frame.offset.y + by) * scrnx + sht.frame.offset.x + bx] == sid) {
						unsigned char *vram24 = (unsigned char *)(vram + ((sht.frame.offset.y + by) * scrnx + (sht.frame.offset.x + bx)) * 3);
						if (sid > 1) {
							rgb = MixRgb(rgb, backrgb[(sht.frame.offset.y + by - vy0) * (vx1 - vx0) + (sht.frame.offset.x + bx - vx0)]);
						}
						vram24[0] = (unsigned char)rgb;
						vram24[1] = (unsigned char)(rgb >> 8);
						vram24[2] = (unsigned char)(rgb >> 16);
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
					rgb = sht.buf[by * sht.frame.vector.x + bx];
					if (map[(sht.frame.offset.y + by) * scrnx + sht.frame.offset.x + bx] == sid) {
						((unsigned short *)vram)[(sht.frame.offset.y + by) * scrnx + (sht.frame.offset.x + bx)]
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

// 単色直線を描画
void SheetCtl::drawLine(Sheet *sht, unsigned int c, int x0, int y0, int x1, int y1) {
	int x, y, dx, dy, len;

	// 直線高速化
	if (y0 == y1) {
		for (x = x0; x <= x1; ++x) {
			sht->buf[y0 * sht->frame.vector.x + x] = c;
		}
		return;
	} else if (x0 == x1) {
		for (y = y0; y <= y1; ++y) {
			sht->buf[y * sht->frame.vector.x + x0] = c;
		}
		return;
	}

	dx = (x1 - x0 < 0)? x0 - x1 : x1 - x0;
	dy = (y1 - y0 < 0)? y0 - y1 : y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx >= dy) {
		len = dx + 1;
		dx = (x0 > x1) ? -1024 : 1024;
		dy = (y0 <= y1) ? ((y1 - y0 + 1) << 10) / len
		                : ((y1 - y0 - 1) << 10) / len;
	} else {
		len = dy + 1;
		dy = (y0 > y1) ? -1024 : 1024;
		dx = (x0 <= x1) ? ((x1 - x0 + 1) << 10) / len
		                : ((x1 - x0 - 1) << 10) / len;
	}

	for (int i = 0; i < len; ++i) {
		sht->buf[(y >> 10) * sht->frame.vector.x + (x >> 10)] = c;
		x += dx;
		y += dy;
	}
}

// グラデーション直線を描画
void SheetCtl::gradLine(Sheet *sht, unsigned int c0, unsigned int c1, int x0, int y0, int x1, int y1, int d) {
	int x, y, dx, dy, len;

	dx = (x1 - x0 < 0)? x0 - x1 : x1 - x0;
	dy = (y1 - y0 < 0)? y0 - y1 : y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx >= dy) {
		len = dx + 1;
		dx = (x0 > x1) ? -1024 : 1024;
		dy = (y0 <= y1) ? ((y1 - y0 + 1) << 10) / len
		                : ((y1 - y0 - 1) << 10) / len;
	} else {
		len = dy + 1;
		dy = (y0 > y1) ? -1024 : 1024;
		dx = (x0 <= x1) ? ((x1 - x0 + 1) << 10) / len
		                : ((x1 - x0 - 1) << 10) / len;
	}

	if (d == 0) {	// 横
		if (x0 == x1) {
			for (int i = 0; i < len; ++i) {
				sht->buf[(y >> 10) * sht->frame.vector.x + (x >> 10)] = c0;
				x += dx;
				y += dy;
			}
		} else {
			for (int i = 0; i < len; ++i) {
				sht->buf[(y >> 10) * sht->frame.vector.x + (x >> 10)] = GetGrad(x0, x1, x >> 10, c0, c1);
				x += dx;
				y += dy;
			}
		}
	} else if (d == 1) {	// 縦
		if (y0 == y1) {
			for (int i = 0; i < len; ++i) {
				sht->buf[(y >> 10) * sht->frame.vector.x + (x >> 10)] = c0;
				x += dx;
				y += dy;
			}
		} else {
			for (int i = 0; i < len; ++i) {
				sht->buf[(y >> 10) * sht->frame.vector.x + (x >> 10)] = GetGrad(y0, y1, y >> 10, c0, c1);
				x += dx;
				y += dy;
			}
		}
	}
}

// 枠のみ長方形を描画
void SheetCtl::drawRect(Sheet *sht, unsigned int c, int x0, int y0, int x1, int y1) {
	for (int x = x0; x < x1; ++x) {
		sht->buf[y0 * sht->frame.vector.x + x] = c;
		sht->buf[(y1 - 1) * sht->frame.vector.x + x] = c;
	}
	for (int y = y0 + 1; y < y1 - 1; ++y) {
		sht->buf[y * sht->frame.vector.x + x0] = c;
		sht->buf[y * sht->frame.vector.x + x1 - 1] = c;
	}
}

// 塗りつぶし長方形を描画
void SheetCtl::fillRect(Sheet *sht, unsigned int c, int x0, int y0, int x1, int y1) {
	for (int y = y0; y < y1; ++y) {
		for (int x = x0; x < x1; ++x) {
			sht->buf[y * sht->frame.vector.x + x] = c;
		}
	}
}

// グラデーション長方形を描画
void SheetCtl::gradRect(Sheet *sht, unsigned int c0, unsigned int c1, int x0, int y0, int x1, int y1, int d) {
	if (d == 0) {	// 横
		for (int y = y0; y < y1; ++y) {
			for (int x = x0; x < x1; ++x) {
				sht->buf[y * sht->frame.vector.x + x] = GetGrad(x0, x1 - 1, x, c0, c1);
			}
		}
	} else if (d == 1) {	//縦
		for (int y = y0; y < y1; ++y) {
			for (int x = x0; x < x1; ++x) {
				sht->buf[y * sht->frame.vector.x + x] = GetGrad(y0, y1 - 1, y, c0, c1);
			}
		}
	}
}

// 枠のみ円を描画
void SheetCtl::drawCircle(Sheet *sht, unsigned int c, int x0, int y0, int d) {
	int x = d / 2;
	int y = 0;
	int F = -2 * (d / 2) + 3;
	int xo = x0 + d / 2;
	int yo = y0 + d / 2;
	if (d % 2 == 1) {
		while (x >= y) {
			sht->buf[(yo + y) * sht->frame.vector.x + xo + x] = c;
			sht->buf[(yo + y) * sht->frame.vector.x + xo - x] = c;
			sht->buf[(yo - y) * sht->frame.vector.x + xo + x] = c;
			sht->buf[(yo - y) * sht->frame.vector.x + xo - x] = c;
			sht->buf[(yo + x) * sht->frame.vector.x + xo + y] = c;
			sht->buf[(yo + x) * sht->frame.vector.x + xo - y] = c;
			sht->buf[(yo - x) * sht->frame.vector.x + xo + y] = c;
			sht->buf[(yo - x) * sht->frame.vector.x + xo - y] = c;
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	} else {
		while (x >= y) {
			sht->buf[(yo + y) * sht->frame.vector.x + xo + x - 1] = c;
			sht->buf[(yo + y) * sht->frame.vector.x + xo - x] = c;
			sht->buf[(yo - y) * sht->frame.vector.x + xo + x - 1] = c;
			sht->buf[(yo - y) * sht->frame.vector.x + xo - x] = c;
			sht->buf[(yo + x - 1) * sht->frame.vector.x + xo + y] = c;
			sht->buf[(yo + x - 1) * sht->frame.vector.x + xo - y] = c;
			sht->buf[(yo - x) * sht->frame.vector.x + xo + y] = c;
			sht->buf[(yo - x) * sht->frame.vector.x + xo - y] = c;
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	}
}

// 塗りつぶし円を描画
void SheetCtl::fillCircle(Sheet *sht, unsigned int c, int x0, int y0, int d) {
	int x = d / 2;
	int y = 0;
	int F = -2 * (d / 2) + 3;
	int xo = x0 + d / 2;
	int yo = y0 + d / 2;
	if (d % 2 == 1) {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; ++xx) {
				sht->buf[(yo + y) * sht->frame.vector.x + xx] = c;
				sht->buf[(yo - y) * sht->frame.vector.x + xx] = c;
			}
			for (int xx = xo - y; xx < xo + y; ++xx) {
				sht->buf[(yo + x) * sht->frame.vector.x + xx] = c;
				sht->buf[(yo - x) * sht->frame.vector.x + xx] = c;
			}
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	} else {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; ++xx) {
				sht->buf[(yo + y) * sht->frame.vector.x + xx] = c;
				sht->buf[(yo - y) * sht->frame.vector.x + xx] = c;
			}
			for (int xx = xo - y; xx < xo + y; ++xx) {
				sht->buf[(yo + x - 1) * sht->frame.vector.x + xx] = c;
				sht->buf[(yo - x) * sht->frame.vector.x + xx] = c;
			}
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	}
}

// グラデーション円を描画
void SheetCtl::gradCircle(Sheet *sht, unsigned int c0, unsigned int c1, int x0, int y0, int d) {
	int x = d / 2;
	int y = 0;
	int F = -2 * (d / 2) + 3;
	int xo = x0 + d / 2;
	int yo = y0 + d / 2;
	if (d % 2 == 1) {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; ++xx) {
				sht->buf[(yo + y) * sht->frame.vector.x + xx] = GetGrad(y0, y0 + d, yo + y, c0, c1);
				sht->buf[(yo - y) * sht->frame.vector.x + xx] = GetGrad(y0, y0 + d, yo - y, c0, c1);
			}
			for (int xx = xo - y; xx < xo + y; ++xx) {
				sht->buf[(yo + x) * sht->frame.vector.x + xx] = GetGrad(y0, y0 + d, yo + x, c0, c1);
				sht->buf[(yo - x) * sht->frame.vector.x + xx] = GetGrad(y0, y0 + d, yo - x, c0, c1);
			}
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	} else {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; ++xx) {
				sht->buf[(yo + y) * sht->frame.vector.x + xx] = GetGrad(y0, y0 + d, yo + y, c0, c1);
				sht->buf[(yo - y) * sht->frame.vector.x + xx] = GetGrad(y0, y0 + d, yo - y, c0, c1);
			}
			for (int xx = xo - y; xx < xo + y; ++xx) {
				sht->buf[(yo + x - 1) * sht->frame.vector.x + xx] = GetGrad(y0, y0 + d, yo + x - 1, c0, c1);
				sht->buf[(yo - x) * sht->frame.vector.x + xx] = GetGrad(y0, y0 + d, yo - y, c0, c1);
			}
			if (F >= 0) {
				--x;
				F -= 4 * x;
			}
			++y;
			F += 4 * y + 2;
		}
	}
}

// 単色文字を描画
void SheetCtl::drawChar(Sheet *sht, int x, int y, unsigned int c, unsigned char * font) {
	unsigned int *p;
	unsigned char d;
	for (int i = 0; i < 16; ++i) {
		p = sht->buf + (y + i) * sht->frame.vector.x + x;
		d = font[i];
		if (d & 0x80) { p[0] = c; }
		if (d & 0x40) { p[1] = c; }
		if (d & 0x20) { p[2] = c; }
		if (d & 0x10) { p[3] = c; }
		if (d & 0x08) { p[4] = c; }
		if (d & 0x04) { p[5] = c; }
		if (d & 0x02) { p[6] = c; }
		if (d & 0x01) { p[7] = c; }
	}
}

// 単色文字列を描画
void SheetCtl::drawString(Sheet *sht, int x, int y, unsigned int c, const char *str, Encoding encode) {
	unsigned char *fontdat = adrfont;
	unsigned char *font;
	unsigned char *s = (unsigned char *)str;
	int k, t;
	unsigned short langbyte1 = 0;
	unsigned int u8code;
	for (; *s; ++s) {
		if (!langbyte1) {
			if (encode == Encoding::SJIS && ((0x81 <= *s && *s <= 0x9f) || (0xe0 <= *s && *s <= 0xfc))) {	// Shift_JIS
				langbyte1 = *s;
			} else if (encode == Encoding::UTF8) {	// UTF-8
				if (((0xe2 <= *s && *s <= 0xef) || (0xc2 <= *s && *s <= 0xd1))
					&& 0x80 <= *(s + 1) && *(s + 1) <= 0xbf) {
					langbyte1 = ((*s << 8) | *(s + 1));
					++s;
				} else {
					drawChar(sht, x, y, c, fontdat + *s * 16);
				}
				if (langbyte1 == 0xefbd) {	// ｡~ｿ
					++s;
					drawChar(sht, x, y, c, fontdat + *s * 16);
					langbyte1 = 0;
				} else if (langbyte1 == 0xefbe) {	// ﾀ~ﾟ
					++s;
					drawChar(sht, x, y, c, fontdat + (*s + 0x40) * 16);
					langbyte1 = 0;
				} else if (langbyte1 == 0xe280 && *(s + 1) == 0xbe) {	// 波ダッシュ(~)
					++s;
					drawChar(sht, x, y, c, fontdat + 0x7e * 16);
					langbyte1 = 0;
				} else if (langbyte1 == 0xc2a5) {	// 円マーク(\)
					drawChar(sht, x, y, c, fontdat + 0x5c * 16);
					langbyte1 = 0;
				}
			} else if (encode == Encoding::EUCJP && 0x81 <= *s && *s <= 0xfe) {	// EUC-JP
				langbyte1 = *s;
			} else {	// 半角1バイト文字
				drawChar(sht, x, y, c, fontdat + *s * 16);
			}
		} else {
			if (encode == Encoding::SJIS) {
				k = (0x81 <= langbyte1 && langbyte1 <= 0x9f) ? (langbyte1 - 0x81) * 2
				                                             : (langbyte1 - 0xe0) * 2 + 62;
				if (0x40 <= *s && *s <= 0x7e) {
					t = *s - 0x40;
				} else if (0x80 <= *s && *s <= 0x9e) {
					t = *s - 0x80 + 63;
				} else {
					t = *s - 0x9f;
					++k;
				}
				font = fontdat + 256 * 16 + (k * 94 + t) * 32;
			} else if (encode == Encoding::UTF8) {
				if (langbyte1 >> 12 != 0xc && langbyte1 >> 12 != 0xd) {
					u8code = ((langbyte1 << 8) | *s);
				} else {
					u8code = langbyte1;
					--s;
				}
				font = fontdat + 256 * 16 + Utf8ToKT(u8code) * 32;
			} else/* if (encode == Encoding::EUCJP)*/ {
				font = fontdat + 256 * 16 + ((langbyte1 - 0xa1) * 94 + *s - 0xa1) * 32;
			}
			langbyte1 = 0;
			drawChar(sht, x - 8, y, c, font);
			drawChar(sht, x, y, c, font + 16);
		}
		x += 8;
	}
}

void SheetCtl::borderRadius(Sheet *sht, bool leftt, bool rightt, bool leftb, bool rightb) {
	int x = sht->frame.vector.x, y = sht->frame.vector.y;
	// 左上
	if (leftt) {
		drawLine(sht, kTransColor, 0, 0, 2, 0);				// □□□
		drawLine(sht, kTransColor, 0, 1, 1, 1);				// □□■
		drawLine(sht, kTransColor, 0, 2, 0, 2);				// □■■
	}
	// 右上
	if (rightt) {
		drawLine(sht, kTransColor, x - 3, 0, x - 1, 0);			// □□□
		drawLine(sht, kTransColor, x - 2, 1, x - 1, 1);			// ■□□
		drawLine(sht, kTransColor, x - 1, 2, x - 1, 2);			// ■■□
	}
	// 左下
	if (leftb) {
		drawLine(sht, kTransColor, 0, y - 3, 0, y - 3);		// □■■
		drawLine(sht, kTransColor, 0, y - 2, 1, y - 2);		// □□■
		drawLine(sht, kTransColor, 0, y - 1, 2, y - 1);		// □□□
	}
	// 右下
	if (rightb) {
		drawLine(sht, kTransColor, x - 1, y - 3, x - 1, y - 3);	// ■■□
		drawLine(sht, kTransColor, x - 2, y - 2, x - 1, y - 2);	// ■□□
		drawLine(sht, kTransColor, x - 3, y - 1, x - 1, y - 1);	// □□□
	}
}

// 画像を描画
void SheetCtl::drawPicture(Sheet *sht, int x, int y, const char *fname, long col_inv, int mag) {
	int info[4];
	unsigned int col;
	int i;
	DLL_STRPICENV env;
	unique_ptr<File> imagefile(FAT12::open(fname));

	if (imagefile) {
		unsigned char *filebuf = imagefile->read();
		unsigned int fsize = imagefile->size();

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
			if (!i && info[2] <= scrnx && info[3] <= scrny) {
				for (int yy = 0; yy < info[3]; ++yy) {
					for (int xx = 0; xx < info[2]; ++xx) {
						col = Rgb(picbuf[yy * info[2] + xx].r, picbuf[yy * info[2] + xx].g, picbuf[yy * info[2] + xx].b);
						if ((int)col != col_inv && sht->buf[(yy + y) * mag * sht->frame.vector.x + (xx + x) * mag] != col) {
							sht->buf[(yy + y) * mag * sht->frame.vector.x + (xx + x) * mag] = col;
						}
					}
				}
			}
		}
	}
}

// 指定色を変更
void SheetCtl::colorChange(Sheet &sht, int x0, int y0, int x1, int y1, unsigned int c0, unsigned int c1) {
	for (int y = 0; y < y1 - y0; ++y) {
		for (int x = 0; x < x1 - x0; ++x) {
			if (sht.buf[(y + y0) * sht.frame.vector.x + x + x0] == c0) {
				sht.buf[(y + y0) * sht.frame.vector.x + x + x0] = c1;
			}
		}
	}
}
