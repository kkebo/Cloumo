#include "headers.h"

int SheetCtl::top_        = -1;
int SheetCtl::tbox_cpos_  = 2;
unsigned short* SheetCtl::vram_ = 0;
int SheetCtl::scrnx_      = 0;
int SheetCtl::scrny_      = 0;
unsigned char* SheetCtl::map_ = 0;
Sheet* SheetCtl::back_    = 0;
Sheet* SheetCtl::context_menu_ = 0;
Sheet** SheetCtl::window_ = 0;
Sheet* SheetCtl::sheets0_ = 0;
Sheet** SheetCtl::sheets_ = 0;
int SheetCtl::color_      = 0;
int SheetCtl::adrfont_    = 0;

// シートコントロールを初期化
void SheetCtl::init() {
	/* オブジェクト初期化 */
	BootInfo *binfo   = (BootInfo *)ADDRESS_BOOTINFO;
	vram_    = (unsigned short *)binfo->vram;
	scrnx_   = binfo->scrnx;
	scrny_   = binfo->scrny;
	color_   = binfo->vmode;
	map_     = new unsigned char[scrnx_ * scrny_];
	sheets_  = new Sheet*[kMaxSheets];
	sheets0_ = new Sheet[kMaxSheets];
	window_  = new Sheet*[kMaxTabs];
	for (int i = 0; i < kMaxSheets; i++) {
		sheets0_[i].flags = 0; /* 未使用マーク */
	}

	/* フォント読み込み */
	File* fontfile = FAT12::open("japanese.fnt");
	adrfont_ = (int)fontfile->read();

	/* サイドバー */
	back_ = alloc(150, scrny_, false);
	// 背景色
	fillRect(back_, Rgb(0, 84, 255), 0, 0, back_->bxsize, back_->bysize);
	// 戻る・進むボタン枠
	drawPicture(back_, 4, 4, "b_f.bmp", Rgb(255, 0, 255));
	// 更新ボタン枠
	drawPicture(back_, 59, 4, "btn_r.bmp", Rgb(255, 0, 255));
	// タブ
	drawString(back_, 6, 39, 0, "index.htm");
	colorChange(back_, 2, 35, back_->bxsize, 33 + 16 + 8, Rgb(0, 84, 255), Rgb(255, 255, 255));
	// 検索窓
	fillRect(back_, Rgb(255, 255, 255), 2, back_->bysize - 20 - 22, back_->bxsize - 2, back_->bysize - 20);
	// 表示設定
	upDown(back_, 0);

	/* ホームページ */
	window_[0] = alloc(scrnx_ - back_->bxsize, scrny_, false);
	drawRect(window_[0], 0, 0, 0, window_[0]->bxsize, window_[0]->bysize);
	fillRect(window_[0], Rgb(255, 255, 255), 1, 1, window_[0]->bxsize - 1, window_[0]->bysize - 1);
	slide(window_[0], back_->bxsize, 0);
	upDown(window_[0], 1);

	/* 右クリックメニュー */
	context_menu_ = alloc(150, 150, true);
	fillRect(context_menu_, kTransColor, 0, 0, context_menu_->bxsize, context_menu_->bysize);
	gradCircle(context_menu_, Rgb(200, 230, 255, 50), Rgb(100, 150, 255), 0, 0, 150);
	drawCircle(context_menu_, Rgb(0, 0, 255, 50), 0, 0, 150);
	fillCircle(context_menu_, kTransColor, 40, 40, 70);
	drawCircle(context_menu_, Rgb(0, 0, 255, 50), 40, 40, 70);
	drawPicture(context_menu_, context_menu_->bxsize / 2 - 16, 3, "copy.bmp", Rgb(255, 0, 255));
	drawPicture(context_menu_, context_menu_->bxsize / 2 + 38, context_menu_->bysize / 2 - 16, "source.bmp", Rgb(255, 0, 255));
	drawPicture(context_menu_, context_menu_->bxsize / 2 - 16, context_menu_->bysize - 32 - 3, "search.bmp", Rgb(255, 0, 255));
	drawPicture(context_menu_, context_menu_->bxsize / 2 - 38 - 32, context_menu_->bysize / 2 - 16, "refresh.bmp", Rgb(255, 0, 255));
	upDown(context_menu_, -1);
}

// シートを確保
Sheet *SheetCtl::alloc(int xsize, int ysize, bool inv) {
	Sheet* sht;
	for (int i = 0; i < kMaxSheets; i++) {
		if (!sheets0_[i].flags) {
			sht = &sheets0_[i];
			sht->flags = kSheetUse;
			sht->height = -1;

			sht->buf = new unsigned int[xsize * ysize];
			sht->bxsize = xsize;
			sht->bysize = ysize;
			sht->trans = (inv) ? true : false;
			return sht;
		}
	}
	return 0;
}

// シートの高さを変更
void SheetCtl::upDown(Sheet* sht, int height) {
	int old = sht->height;

	if (height > top_ + 1) height = top_ + 1;
	if (height < -1) height = -1;
	sht->height = height;

	if (old > height) {	// 前より低くなった
		if (height >= 0) {	// 表示
			for (int h = old; h > height; h--) {
				sheets_[h] = sheets_[h - 1];
				sheets_[h]->height = h;
			}
			sheets_[height] = sht;
			refreshMap(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			refreshSub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, top_);
		} else {	// 非表示
			if (top_ > old) {
				for (int h = old; h < top_; h++) {
					sheets_[h] = sheets_[h + 1];
					sheets_[h]->height = h;
				}
			}
			top_--;
			refreshMap(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			refreshSub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, top_);
		}
	} else if (old < height) {	// 以前より高くなった
		if (old >= 0) {	// より高く
			for (int h = old; h < height; h++) {
				sheets_[h] = sheets_[h + 1];
				sheets_[h]->height = h;
			}
			sheets_[height] = sht;
		} else {	// 非表示から表示へ
			for (int h = top_; h >= height; h--) {
				sheets_[h + 1] = sheets_[h];
				sheets_[h + 1]->height = h + 1;
			}
			sheets_[height] = sht;
			top_++;
		}
		refreshMap(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		refreshSub(sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, top_); // 変更 height→top_
	}
}

// 指定シート内の指定範囲をリフレッシュ
void SheetCtl::refresh(Sheet *sht, int bx0, int by0, int bx1, int by1) {
	if (sht->height >= 0) {	// 非表示シートはリフレッシュしない
		refreshMap(sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height);
		refreshSub(sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height);
	}
}

// 指定範囲の変更をmapに適用
void SheetCtl::refreshMap(int vx0, int vy0, int vx1, int vy1, int h0) {
	int bx0, by0, bx1, by1, sid4;
	unsigned char sid;
	Sheet *sht;
	if (vx0 < 0) vx0 = 0;
	if (vy0 < 0) vy0 = 0;
	if (vx1 > scrnx_) vx1 = scrnx_;
	if (vy1 > scrny_) vy1 = scrny_;
	for (int h = h0; h <= top_; h++) {
		sht = sheets_[h];
		sid = sht - sheets0_; /* 番地を引き算してそれを下じき番号として利用 */
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) bx0 = 0;
		if (by0 < 0) by0 = 0;
		if (bx1 > sht->bxsize) bx1 = sht->bxsize;
		if (by1 > sht->bysize) by1 = sht->bysize;
		if (!sht->trans) {
			if (!(sht->vx0 & 3) && !(bx0 & 3) && !(bx1 & 3)) {
				/* 透明色なし専用の高速版（4バイト型） */
				bx1 = (bx1 - bx0) / 4;
				sid4 = sid | sid << 8 | sid << 16 | sid << 24;
				for (int by = by0; by < by1; by++) {
					for (int bx = 0; bx < bx1; bx++) {
						((int*) &map_[(sht->vy0 + by) * scrnx_ + sht->vx0 + bx0])[bx] = sid4;
					}
				}
			} else {
				/* 透明色なし専用の高速版（1バイト型） */
				for (int by = by0; by < by1; by++) {
					for (int bx = bx0; bx < bx1; bx++) {
						map_[(sht->vy0 + by) * scrnx_ + sht->vx0 + bx] = sid;
					}
				}
			}
		} else {
			/* 透明色ありの一般版（1バイト型） */
			for (int by = by0; by < by1; by++) {
				for (int bx = bx0; bx < bx1; bx++) {
					if ((unsigned char) (sht->buf[by * sht->bxsize + bx] >> 24) != 255) {
						map_[(sht->vy0 + by) * scrnx_ + sht->vx0 + bx] = sid;
					}
				}
			}
		}
	}
}

// 指定範囲の変更をvramに適用
void SheetCtl::refreshSub(int vx0, int vy0, int vx1, int vy1, int h1) {
	int bx0, by0, bx1, by1;
	unsigned char sid;
	Sheet *sht;
	unsigned int rgb;
	unsigned int *backrgb;

	if (vx0 < 0) vx0 = 0;
	if (vy0 < 0) vy0 = 0;
	if (vx1 > scrnx_) vx1 = scrnx_;
	if (vy1 > scrny_) vy1 = scrny_;
	backrgb = new unsigned int[(vx1 - vx0) * (vy1 - vy0)];

	for (int h = 0; h <= h1; h++) {
		sht = sheets_[h];
		sid = sht - sheets0_;
		/* vx0～vy1を使って、bx0～by1を逆算する */
		bx0 = (vx0 - sht->vx0 < 0)? 0 : vx0 - sht->vx0;
		by0 = (vy0 - sht->vy0 < 0)? 0 : vy0 - sht->vy0;
		bx1 = (vx1 - sht->vx0 > sht->bxsize)? sht->bxsize : vx1 - sht->vx0;
		by1 = (vy1 - sht->vy0 > sht->bysize)? sht->bysize : vy1 - sht->vy0;
		if (color_ == 32) {
			for (int by = by0; by < by1; by++) {
				for (int bx = bx0; bx < bx1; bx++) {
					if (map_[(sht->vy0 + by) * scrnx_ + sht->vx0 + bx] == sid) {
						rgb = sht->buf[by * sht->bxsize + bx];
						((unsigned int *)vram_)[((sht->vy0 + by) * scrnx_ + (sht->vx0 + bx))]
							= (h <= 1) ? rgb 
							           : MixRgb(rgb, backrgb[(sht->vy0 + by - vy0) * (vx1 - vx0) + (sht->vx0 + bx - vx0)]);
					} else {
						rgb = sht->buf[by * sht->bxsize + bx];
						if ((unsigned char)(rgb >> 24) == 255) continue;
						backrgb[(sht->vy0 + by - vy0) * (vx1 - vx0) + (sht->vx0 + bx - vx0)]
							= (h <= 1) ? rgb
							           : MixRgb(rgb, backrgb[(sht->vy0 + by - vy0) * (vx1 - vx0) + (sht->vx0 + bx - vx0)]);
					}
				}
			}
		} else if (color_ == 16) {
			for (int by = by0; by < by1; by++) {
				for (int bx = bx0; bx < bx1; bx++) {
					if (map_[(sht->vy0 + by) * scrnx_ + sht->vx0 + bx] == sid) {
						rgb = sht->buf[by * sht->bxsize + bx];
						vram_[(sht->vy0 + by) * scrnx_ + (sht->vx0 + bx)]
						              = (h <= 1)?
						            		  ((((unsigned char) (rgb >> 16) << 8) & 0xf800)
								                 | (((unsigned char) (rgb >> 8) << 3) & 0x07e0)
								                 | ((unsigned char) rgb >> 3)) :
						                	  ((((((unsigned char) (backrgb[(sht->vy0 + by - vy0) * (vx1 - vx0) + (sht->vx0 + bx - vx0)] >> 16) - (unsigned char) (rgb >> 16)) * (unsigned char) (rgb >> 24) / 255 + (unsigned char) (rgb >> 16)) << 8) & 0xf800)
								                 | (((((unsigned char) (backrgb[(sht->vy0 + by - vy0) * (vx1 - vx0) + (sht->vx0 + bx - vx0)] >> 8) - (unsigned char) (rgb >> 8)) * (unsigned char) (rgb >> 24) / 255 + (unsigned char) (rgb >> 8)) << 3) & 0x07e0)
								                 | (((unsigned char) backrgb[(sht->vy0 + by - vy0) * (vx1 - vx0) + (sht->vx0 + bx - vx0)] - (unsigned char) rgb) * (unsigned char) (rgb >> 24) / 255 + (unsigned char) rgb) >> 3);
					} else {
						rgb = sht->buf[by * sht->bxsize + bx];
						if ((unsigned char) (rgb >> 24) == 255) continue;
						backrgb[(sht->vy0 + by - vy0) * (vx1 - vx0) + (sht->vx0 + bx - vx0)]
						        = (h <= 1)?
						        		rgb :
						        		MixRgb(rgb, backrgb[(sht->vy0 + by - vy0) * (vx1 - vx0) + (sht->vx0 + bx - vx0)]);
					}
				}
			}
		}
	}
	delete backrgb;
}

// シートを移動
void SheetCtl::slide(Sheet* sht, int vx0, int vy0) {
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) {	// 非表示シートはリフレッシュしない
		refreshMap(old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0);
		refreshMap(vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
		refreshSub(old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, sht->height - 1);
		refreshSub(vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
	}
}

// シートを解放
void SheetCtl::freeSheet(Sheet* sht) {
	if (sht->height >= 0) upDown(sht, -1);
	sht->flags = 0;
	delete sht->buf;
}

// 単色直線を描画
void SheetCtl::drawLine(Sheet* sht, unsigned int c, int x0, int y0, int x1, int y1) {
	int x, y, dx, dy, len;

	// 直線高速化
	if (y0 == y1) {
		for (x = x0; x <= x1; x++) {
			sht->buf[y0 * sht->bxsize + x] = c;
		}
		return;
	} else if (x0 == x1) {
		for (y = y0; y <= y1; y++) {
			sht->buf[y * sht->bxsize + x0] = c;
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

	for (int i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = c;
		x += dx;
		y += dy;
	}
}

// グラデーション直線を描画
void SheetCtl::gradLine(Sheet* sht, unsigned int c0, unsigned int c1, int x0, int y0, int x1, int y1, int d) {
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
			for (int i = 0; i < len; i++) {
				sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = c0;
				x += dx;
				y += dy;
			}
		} else {
			for (int i = 0; i < len; i++) {
				sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = GetGrad(x0, x1, x >> 10, c0, c1);
				x += dx;
				y += dy;
			}
		}
	} else if (d == 1) {	// 縦
		if (y0 == y1) {
			for (int i = 0; i < len; i++) {
				sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = c0;
				x += dx;
				y += dy;
			}
		} else {
			for (int i = 0; i < len; i++) {
				sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = GetGrad(y0, y1, y >> 10, c0, c1);
				x += dx;
				y += dy;
			}
		}
	}
}

// 枠のみ長方形を描画
void SheetCtl::drawRect(Sheet* sht, unsigned int c, int x0, int y0, int x1, int y1) {
	for (int x = x0; x < x1; x++) {
		sht->buf[y0 * sht->bxsize + x] = c;
		sht->buf[(y1 - 1) * sht->bxsize + x] = c;
	}
	for (int y = y0 + 1; y < y1 - 1; y++) {
		sht->buf[y * sht->bxsize + x0] = c;
		sht->buf[y * sht->bxsize + x1 - 1] = c;
	}
}

// 塗りつぶし長方形を描画
void SheetCtl::fillRect(Sheet* sht, unsigned int c, int x0, int y0, int x1, int y1) {
	for (int y = y0; y < y1; y++) {
		for (int x = x0; x < x1; x++) {
			sht->buf[y * sht->bxsize + x] = c;
		}
	}
}

// グラデーション長方形を描画
void SheetCtl::gradRect(Sheet* sht, unsigned int c0, unsigned int c1, int x0, int y0, int x1, int y1, int d) {
	if (d == 0) {	// 横
		for (int y = y0; y < y1; y++) {
			for (int x = x0; x < x1; x++) {
				sht->buf[y * sht->bxsize + x] = GetGrad(x0, x1 - 1, x, c0, c1);
			}
		}
	} else if (d == 1) {	//縦
		for (int y = y0; y < y1; y++) {
			for (int x = x0; x < x1; x++) {
				sht->buf[y * sht->bxsize + x] = GetGrad(y0, y1 - 1, y, c0, c1);
			}
		}
	}
}

// 枠のみ円を描画
void SheetCtl::drawCircle(Sheet* sht, unsigned int c, int x0, int y0, int d) {
	int x = d / 2;
	int y = 0;
	int F = -2 * (d / 2) + 3;
	int xo = x0 + d / 2;
	int yo = y0 + d / 2;
	if (d % 2 == 1) {
		while (x >= y) {
			sht->buf[(yo + y) * sht->bxsize + xo + x] = c;
			sht->buf[(yo + y) * sht->bxsize + xo - x] = c;
			sht->buf[(yo - y) * sht->bxsize + xo + x] = c;
			sht->buf[(yo - y) * sht->bxsize + xo - x] = c;
			sht->buf[(yo + x) * sht->bxsize + xo + y] = c;
			sht->buf[(yo + x) * sht->bxsize + xo - y] = c;
			sht->buf[(yo - x) * sht->bxsize + xo + y] = c;
			sht->buf[(yo - x) * sht->bxsize + xo - y] = c;
			if (F >= 0) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
		}
	} else {
		while (x >= y) {
			sht->buf[(yo + y) * sht->bxsize + xo + x - 1] = c;
			sht->buf[(yo + y) * sht->bxsize + xo - x] = c;
			sht->buf[(yo - y) * sht->bxsize + xo + x - 1] = c;
			sht->buf[(yo - y) * sht->bxsize + xo - x] = c;
			sht->buf[(yo + x - 1) * sht->bxsize + xo + y] = c;
			sht->buf[(yo + x - 1) * sht->bxsize + xo - y] = c;
			sht->buf[(yo - x) * sht->bxsize + xo + y] = c;
			sht->buf[(yo - x) * sht->bxsize + xo - y] = c;
			if (F >= 0) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
		}
	}
}

// 塗りつぶし円を描画
void SheetCtl::fillCircle(Sheet* sht, unsigned int c, int x0, int y0, int d) {
	int x = d / 2;
	int y = 0;
	int F = -2 * (d / 2) + 3;
	int xo = x0 + d / 2;
	int yo = y0 + d / 2;
	if (d % 2 == 1) {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; xx++) {
				sht->buf[(yo + y) * sht->bxsize + xx] = c;
				sht->buf[(yo - y) * sht->bxsize + xx] = c;
			}
			for (int xx = xo - y; xx < xo + y; xx++) {
				sht->buf[(yo + x) * sht->bxsize + xx] = c;
				sht->buf[(yo - x) * sht->bxsize + xx] = c;
			}
			if (F >= 0) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
		}
	} else {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; xx++) {
				sht->buf[(yo + y) * sht->bxsize + xx] = c;
				sht->buf[(yo - y) * sht->bxsize + xx] = c;
			}
			for (int xx = xo - y; xx < xo + y; xx++) {
				sht->buf[(yo + x - 1) * sht->bxsize + xx] = c;
				sht->buf[(yo - x) * sht->bxsize + xx] = c;
			}
			if (F >= 0) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
		}
	}
}

// グラデーション円を描画
void SheetCtl::gradCircle(Sheet* sht, unsigned int c0, unsigned int c1, int x0, int y0, int d) {
	int x = d / 2;
	int y = 0;
	int F = -2 * (d / 2) + 3;
	int xo = x0 + d / 2;
	int yo = y0 + d / 2;
	if (d % 2 == 1) {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; xx++) {
				sht->buf[(yo + y) * sht->bxsize + xx] = GetGrad(y0, y0 + d, yo + y, c0, c1);
				sht->buf[(yo - y) * sht->bxsize + xx] = GetGrad(y0, y0 + d, yo - y, c0, c1);
			}
			for (int xx = xo - y; xx < xo + y; xx++) {
				sht->buf[(yo + x) * sht->bxsize + xx] = GetGrad(y0, y0 + d, yo + x, c0, c1);
				sht->buf[(yo - x) * sht->bxsize + xx] = GetGrad(y0, y0 + d, yo - x, c0, c1);
			}
			if (F >= 0) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
		}
	} else {
		while (x >= y) {
			for (int xx = xo - x; xx < xo + x; xx++) {
				sht->buf[(yo + y) * sht->bxsize + xx] = GetGrad(y0, y0 + d, yo + y, c0, c1);
				sht->buf[(yo - y) * sht->bxsize + xx] = GetGrad(y0, y0 + d, yo - y, c0, c1);
			}
			for (int xx = xo - y; xx < xo + y; xx++) {
				sht->buf[(yo + x - 1) * sht->bxsize + xx] = GetGrad(y0, y0 + d, yo + x - 1, c0, c1);
				sht->buf[(yo - x) * sht->bxsize + xx] = GetGrad(y0, y0 + d, yo - y, c0, c1);
			}
			if (F >= 0) {
				x--;
				F -= 4 * x;
			}
			y++;
			F += 4 * y + 2;
		}
	}
}

// 単色文字を描画
void SheetCtl::drawChar(Sheet* sht, int x, int y, unsigned int c, unsigned char * font) {
	unsigned int *p;
	unsigned char d;
	for (int i = 0; i < 16; i++) {
		p = sht->buf + (y + i) * sht->bxsize + x;
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
void SheetCtl::drawString(Sheet *sht, int x, int y, unsigned int c, const char *str, int encode) {
	unsigned char *fontdat = (unsigned char *)adrfont_;
	unsigned char *font;
	unsigned char *s = (unsigned char *)str;
	int k, t;
	unsigned short langbyte1 = 0;
	unsigned int u8code;
	for (; *s; s++) {
		if (!langbyte1) {
			if (encode == 0 && ((0x81 <= *s && *s <= 0x9f) || (0xe0 <= *s && *s <= 0xfc))) {	// Shift_JIS
				langbyte1 = *s;
			} else if (encode == 1) {	// UTF-8
				if (((0xe2 <= *s && *s <= 0xef) || (0xc2 <= *s && *s <= 0xd1))
					&& 0x80 <= *(s + 1) && *(s + 1) <= 0xbf) {
					langbyte1 = ((*s << 8) | *(s + 1));
					s++;
				} else {
					drawChar(sht, x, y, c, fontdat + *s * 16);
				}
				if (langbyte1 == 0xefbd) {	// ｡~ｿ
					s++;
					drawChar(sht, x, y, c, fontdat + *s * 16);
					langbyte1 = 0;
				} else if (langbyte1 == 0xefbe) {	// ﾀ~ﾟ
					s++;
					drawChar(sht, x, y, c, fontdat + (*s + 0x40) * 16);
					langbyte1 = 0;
				} else if (langbyte1 == 0xe280 && *(s + 1) == 0xbe) {	// 波ダッシュ(~)
					s++;
					drawChar(sht, x, y, c, fontdat + 0x7e * 16);
					langbyte1 = 0;
				} else if (langbyte1 == 0xc2a5) {	// 円マーク(\)
					drawChar(sht, x, y, c, fontdat + 0x5c * 16);
					langbyte1 = 0;
				}
			} else if (encode == 2 && 0x81 <= *s && *s <= 0xfe) {	// EUC-JP
				langbyte1 = *s;
			} else {	// 半角1バイト文字
				drawChar(sht, x, y, c, fontdat + *s * 16);
			}
		} else {
			if (encode == 0) {
				k = (0x81 <= langbyte1 && langbyte1 <= 0x9f) ? (langbyte1 - 0x81) * 2
				                                             : (langbyte1 - 0xe0) * 2 + 62;
				if (0x40 <= *s && *s <= 0x7e) {
					t = *s - 0x40;
				} else if (0x80 <= *s && *s <= 0x9e) {
					t = *s - 0x80 + 63;
				} else {
					t = *s - 0x9f;
					k++;
				}
				font = fontdat + 256 * 16 + (k * 94 + t) * 32;
			} else if (encode == 1) {
				if (langbyte1 >> 12 != 0xc && langbyte1 >> 12 != 0xd) {
					u8code = ((langbyte1 << 8) | *s);
				} else {
					u8code = langbyte1;
					s--;
				}
				font = fontdat + 256 * 16 + Utf8ToKT(u8code) * 32;
			} else/* if (encode == 2)*/ {
				font = fontdat + 256 * 16 + ((langbyte1 - 0xa1) * 94 + *s - 0xa1) * 32;
			}
			langbyte1 = 0;
			drawChar(sht, x - 8, y, c, font);
			drawChar(sht, x, y, c, font + 16);
		}
		x += 8;
	}
}

void SheetCtl::kadotori(Sheet* sht, bool leftt, bool rightt, bool leftb, bool rightb) {
	int x = sht->bxsize, y = sht->bysize;
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
void SheetCtl::drawPicture(Sheet* sht, int x, int y, const char *fname, long col_inv, int mag) {
	int info[4];
	unsigned int col;
	int i;
	DLL_STRPICENV env;
	File* imagefile = FAT12::open(fname);

	if (imagefile) {
		unsigned char* filebuf = imagefile->read();
		unsigned int fsize = imagefile->size();

		if (!info_JPEG(&env, info, fsize, filebuf) && !info_BMP(&env, info, fsize, filebuf)) {
			return;
		}

		RGB* picbuf = new RGB[info[2] * info[3]];
		if (picbuf) {
			if (info[0] == 1) {
				i = decode0_BMP(&env, fsize, filebuf, 4, (unsigned char*)picbuf, 0);
			} else {
				i = decode0_JPEG(&env, fsize, filebuf, 4, (unsigned char*)picbuf, 0);
			}
			if (!i && info[2] <= scrnx_ && info[3] <= scrny_) {
				for (int yy = 0; yy < info[3]; yy++) {
					for (int xx = 0; xx < info[2]; xx++) {
						col = Rgb(picbuf[yy * info[2] + xx].r, picbuf[yy * info[2] + xx].g, picbuf[yy * info[2] + xx].b);
						if ((int)col != col_inv && sht->buf[(yy + y) * mag * sht->bxsize + (xx + x) * mag] != col) {
							sht->buf[(yy + y) * mag * sht->bxsize + (xx + x) * mag] = col;
						}
					}
				}
			}
		}
		delete picbuf;
		delete imagefile;
	}
}

// 指定色を変更
void SheetCtl::colorChange(Sheet* sht, int x0, int y0, int x1, int y1, unsigned int c0, unsigned int c1) {
	for (int y = 0; y < y1 - y0; y++) {
		for (int x = 0; x < x1 - x0; x++) {
			if (sht->buf[(y + y0) * sht->bxsize + x + x0] == c0) {
				sht->buf[(y + y0) * sht->bxsize + x + x0] = c1;
			}
		}
	}
}

// 赤緑青をあわせてunsigned intで出力
unsigned int Rgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	return ((a << 24) | (r << 16) | (g << 8) | b);
}

// 透明化処理
unsigned int MixRgb(unsigned int rgb1, unsigned int rgb2) {
	return (((unsigned char) (rgb1 >> 24) << 24)
	        | (((unsigned char) (rgb2 >> 16) - (unsigned char) (rgb1 >> 16)) * (unsigned char) (rgb1 >> 24) / 255 + (unsigned char) (rgb1 >> 16)) << 16
	        | (((unsigned char) (rgb2 >> 8) - (unsigned char) (rgb1 >> 8)) * (unsigned char) (rgb1 >> 24) / 255 + (unsigned char) (rgb1 >> 8)) << 8
	        | (((unsigned char) rgb2 - (unsigned char) rgb1) * (unsigned char) (rgb1 >> 24) / 255 + (unsigned char) rgb1));
}

// グラデーション色を出力
unsigned int GetGrad(int p0, int p1, int p, unsigned int c0, unsigned int c1) {
	return (((unsigned char) (c0 >> 24) << 24)
	        | (((unsigned char) (c0 >> 16) + ((unsigned char) (c1 >> 16) - (unsigned char) (c0 >> 16)) * (p - p0) / (p1 - p0)) << 16)
	        | (((unsigned char) (c0 >> 8) + ((unsigned char) (c1 >> 8) - (unsigned char) (c0 >> 8)) * (p - p0) / (p1 - p0)) << 8)
	        | ((unsigned char) c0 + ((unsigned char) c1 - (unsigned char) c0) * (p - p0) / (p1 - p0)));
}
