#include "../headers.h"
#include <stdio.h>

void DateTimeMain() {
	Task *task = TaskSwitcher::getNowTask();
	Timer *timer;
	unsigned char now[7] = {};
	bool timechk = false;
	char s[9];
	
	// 現在時刻取得
	static const unsigned char adr[7] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09, 0x32 };
	static const unsigned char max[7] = { 0x60, 0x59, 0x23, 0x31, 0x12, 0x99, 0x99 };
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
	Sheet dateTimeSheet(Size(8 * 8, 16), true);
	dateTimeSheet.fillRect(dateTimeSheet.frame, kTransColor);
	if (now[2] >= 12) {
		sprintf(s, "%02d:%02d PM", now[2] - 12, now[1]);
	} else {
		sprintf(s, "%02d:%02d AM", now[2], now[1]);
	}
	dateTimeSheet.drawString(s, Point(0, 0), 0xffffff);
	dateTimeSheet.moveTo(Point(2, SheetCtl::resolution.height - 18));
	SheetCtl::back->appendChild(&dateTimeSheet);
	dateTimeSheet.upDown(1);
	
	for (;;) {
		Cli();
		if (task->queue->isempty()) {
			// 解像度が変更されていたら位置を修正
			if (dateTimeSheet.frame.offset.y != SheetCtl::resolution.height - 18) {
				dateTimeSheet.moveTo(Point(2, SheetCtl::resolution.height - 18));
			}
			if (timechk) {
				Sti();
				if (now[2] >= 12) {
					sprintf(s, "%02d:%02d PM", now[2] - 12, now[1]);
				} else {
					sprintf(s, "%02d:%02d AM", now[2], now[1]);
				}
				dateTimeSheet.fillRect(Rectangle(Point(0, 0), dateTimeSheet.frame.size), kTransColor);
				dateTimeSheet.drawString(s, Point(0, 0), 0xffffff);
				dateTimeSheet.refresh(Rectangle(Point(0, 0), dateTimeSheet.frame.size));
				timechk = false;
			} else {
				task->sleep();
				Sti();
			}
		} else {
			int data = task->queue->pop();
			Sti();
			if (data == timer->data) { // 番号が合っているか確認
				timer->set(100);
				++now[0]; // second
				if (now[0] >= 60) {
					now[0] -= 60;
					++now[1]; // minute
					if (now[1] >= 60) {
						now[1] -= 60;
						++now[2]; // hour
						if (now[2] >= 24) {
							now[2] -= 24;
							++now[3]; // day
							// それ以降は後から作る
						}
					}
					timechk = true;
				}
			}
		}
	}
}
