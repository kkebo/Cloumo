#include "../headers.h"
#include <stdio.h>

unsigned char DateTime::now[7] = { 0, 0, 0, 0, 0, 0, 0 };
Timer *DateTime::timer;

void DateTime::Main() {
	Task *task = TaskController::getNowTask();
	int i;
	bool timechk = false;
	char s[9];
	Rectangle rewriteRange(2, SheetCtl::back->frame.vector.y - 18, 8 * 8, 16);
	
	// 現在時刻取得
	static unsigned char adr[7] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09, 0x32 };
	static unsigned char max[7] = { 0x60, 0x59, 0x23, 0x31, 0x12, 0x99, 0x99 };
	for (;;) {
		char err = 0;
		for (int i = 0; i < 7; ++i) {
			Output8(0x70, adr[i]);
			now[i] = Input8(0x71);
		}
		for (int i = 0; i < 7; ++i) {
			Output8(0x70, adr[i]);
			if (now[i] != Input8(0x71) || (now[i] & 0x0f) > 9 || now[i] > max[i]) {
				err = 1;
			}
		}
		for (int i = 0; i < 7; ++i) {
			now[i] -= (now[i] / 0x10 * 0x6);
		}
		if (!err) break;
	}
	
	// タイマーセット
	timer = new Timer(task->queue);
	timer->set(100); // 1秒おき

	// バーに時刻を表示
	if (now[2] >= 12) {
		sprintf(s, "%02d:%02d PM", now[2] - 12, now[1]);
	} else {
		sprintf(s, "%02d:%02d AM", now[2], now[1]);
	}
	SheetCtl::back->drawString(s, rewriteRange.offset, 0xffffff);
	SheetCtl::back->refresh(rewriteRange);
	
	for (;;) {
		Cli();
		if (task->queue->isempty()) {
			if (timechk) {
				Sti();
				if (now[2] >= 12) {
					sprintf(s, "%02d:%02d PM", now[2] - 12, now[1]);
				} else {
					sprintf(s, "%02d:%02d AM", now[2], now[1]);
				}
				SheetCtl::back->fillRect(rewriteRange, Rgb(0, 84, 255));
				SheetCtl::back->drawString(s, rewriteRange.offset, 0xffffff);
				SheetCtl::back->refresh(rewriteRange);
				timechk = false;
			} else {
				task->sleep();
				Sti();
			}
		} else {
			i = task->queue->pop();
			Sti();
			if (i == timer->getData()) { // 番号が合っているか確認
				timer->set(100);
				++now[0];	// □□□□/□□/□□ □□:□□:■■
				if (now[0] >= 60) {
					now[0] -= 60;
					++now[1];	// □□□□/□□/□□ □□:■■:□□
					if (now[1] >= 60) {
						now[1] -= 60;
						++now[2];	// □□□□/□□/□□ ■■:□□:□□
						if (now[2] >= 24) ++now[3];	// □□□□/□□/■■ □□:□□:□□
					}
					timechk = true;
				}
			}
		}
	}
}
