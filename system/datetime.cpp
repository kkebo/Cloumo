#include "headers.h"
#include <stdio.h>

unsigned char DateTime::t_[7] = { 0, 0, 0, 0, 0, 0, 0 };
Timer* DateTime::timer_ = 0;

// 初期設定
void DateTime::init() {
	static unsigned char adr[7] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09, 0x32 };
	static unsigned char max[7] = { 0x60, 0x59, 0x23, 0x31, 0x12, 0x99, 0x99 };
	for (;;) {
		char err = 0;
		for (int i = 0; i < 7; i++) {
			Output8(0x70, adr[i]);
			t_[i] = Input8(0x71);
		}
		for (int i = 0; i < 7; i++) {
			Output8(0x70, adr[i]);
			if (t_[i] != Input8(0x71) || (t_[i] & 0x0f) > 9 || t_[i] > max[i]) {
				err = 1;
			}
		}
		for (int i = 0; i < 7; i++) {
			t_[i] = t_[i] - (t_[i] / 0x10 * 0x6);
		}
		if (!err) break;
	}

	char s[20];
	Task* task = TaskController::alloc();
	task->name_ = (char*)kTimeTaskName;
	task->tss_.esp = (int)malloc4k(64 * 1024) + 64 * 1024 - 12;
	task->tss_.eip = (int)&mainLoop;
	task->tss_.es = 1 * 8;
	task->tss_.cs = 2 * 8;
	task->tss_.ss = 1 * 8;
	task->tss_.ds = 1 * 8;
	task->tss_.fs = 1 * 8;
	task->tss_.gs = 1 * 8;
	task->run(2, 2); /* level=2, priority=2 */
	task->queue_ = Queue(128, task);

	timer_ = TimerController::alloc();
	timer_->init(&task->queue_);
	timer_->set(100); // 1秒おき

	// バーに時刻を表示
	if (t_[2] >= 12) {
		sprintf(s, "PM %02d:%02d", t_[2] - 12, t_[1]);
	} else {
		sprintf(s, "AM %02d:%02d", t_[2], t_[1]);
	}
	SheetCtl::drawString(SheetCtl::back_, 2, SheetCtl::back_->bysize - 18, Rgb(255, 255, 255), s);
	SheetCtl::refresh(SheetCtl::back_, 2, SheetCtl::back_->bysize - 18, 2 + 8 * 8, SheetCtl::back_->bysize - 2);
}

void DateTime::mainLoop() {
	Task* task = TaskController::getNowTask();
	int i;
	bool timechk = false;
	char s[20];

	for (;;) {
		Cli();
		if (task->queue_.isempty()) {
			if (timechk) {
				Sti();
				if (t_[2] >= 12) {
					sprintf(s, "PM %02d:%02d", t_[2] - 12, t_[1]);
				} else {
					sprintf(s, "AM %02d:%02d", t_[2], t_[1]);
				}
				SheetCtl::fillRect(SheetCtl::back_, Rgb(0, 84, 255), 2, SheetCtl::back_->bysize - 18, 2 + 8 * 8, SheetCtl::back_->bysize - 2);
				SheetCtl::drawString(SheetCtl::back_, 2, SheetCtl::back_->bysize - 18, Rgb(255, 255, 255), s);
				SheetCtl::refresh(SheetCtl::back_, 2, SheetCtl::back_->bysize - 18, 2 + 8 * 8, SheetCtl::back_->bysize - 2);
				timechk = false;
			} else {
				task->sleep();
				Sti();
			}
		} else {
			i = task->queue_.pop();
			Sti();
			if (i == timer_->data()) { // 番号が合っているか確認
				timer_->set(100);
				t_[0]++;	// □□□□/□□/□□ □□:□□:■■
				if (t_[0] >= 60) {
					t_[0] -= 60;
					t_[1]++;	// □□□□/□□/□□ □□:■■:□□
					if (t_[1] >= 60) {
						t_[1] -= 60;
						t_[2]++;	// □□□□/□□/□□ ■■:□□:□□
						if (t_[2] >= 24) t_[3]++;	// □□□□/□□/■■ □□:□□:□□
					}
					timechk = true;
				}
			}
		}
	}
}
