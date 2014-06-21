#include "headers.h"

unsigned char Mouse::phase_ = 0;
unsigned char Mouse::buf_[] = { 0, 0, 0 };
const char *Mouse::cursor_[] = {
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
int Mouse::btn_ = 0;
int Mouse::new_mx_ = -1;
int Mouse::new_my_ = 0;
Sheet *Mouse::sheet_ = 0;
int Mouse::x_ = 0;
int Mouse::y_ = 0;
Queue *Mouse::queue_ = 0;

void Mouse::Init() {
	KeyboardController::wait();
	Output8(kPortKeyCmd, kKeyCmdSendToMouse);
	KeyboardController::wait();
	Output8(kPortKeyData, kMouseCmdEnable);

	Mouse::x_ = SheetCtl::scrnx_ / 2;
	Mouse::y_ = SheetCtl::scrny_ / 2;

	Mouse::sheet_ = SheetCtl::alloc(16, 16, true);
	for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 16; x++) {
			if (Mouse::cursor_[x][y] == 'O') {
				Mouse::sheet_->buf[y * 16 + x] = Rgb(255, 255, 255, 100);
			} else if (Mouse::cursor_[x][y] == '@') {
				Mouse::sheet_->buf[y * 16 + x] = Rgb(12, 69, 255, 100);
			} else if (Mouse::cursor_[x][y] == 'G') {
				Mouse::sheet_->buf[y * 16 + x] = Rgb(27, 81, 255, 100);
			} else if (Mouse::cursor_[x][y] == 'J') {
				Mouse::sheet_->buf[y * 16 + x] = Rgb(58, 104, 255, 100);
			} else if (Mouse::cursor_[x][y] == 'C') {
				Mouse::sheet_->buf[y * 16 + x] = Rgb(73, 116, 255, 100);
			} else if (Mouse::cursor_[x][y] == 'U') {
				Mouse::sheet_->buf[y * 16 + x] = Rgb(0, 182, 200, 100);
			} else {
				Mouse::sheet_->buf[y * 16 + x] = kTransColor;
			}
		}
	}
	Mouse::sheet_->vx0 = Mouse::x_ - 8;
	Mouse::sheet_->vy0 = Mouse::y_ - 8;
	SheetCtl::upDown(Mouse::sheet_, SheetCtl::top_ + 1);

	Task* task = TaskController::alloc();
	task->name_ = (char*)kMouseTaskName;
	task->tss_.esp = (int)malloc4k(64 * 1024) + 64 * 1024 - 12;
	task->tss_.eip = (int)&Mouse::Main;
	task->tss_.es = 1 * 8;
	task->tss_.cs = 2 * 8;
	task->tss_.ss = 1 * 8;
	task->tss_.ds = 1 * 8;
	task->tss_.fs = 1 * 8;
	task->tss_.gs = 1 * 8;
	task->run(2, 2);
	task->queue_ = Queue(128, task);
	Mouse::queue_ = &task->queue_;
}

void Mouse::Main() {
	Task *task = TaskController::getNowTask();
	unsigned char code;
    int dx, dy;

	for (;;) { // ÁÑ°Èôê„É´„Éº„É
		Cli();
		if (task->queue_.isempty()) {	// FIFO„Éê„ÉÉ„Éï„Ç°„ÅåÁ©∫„Å„Å£„ÅüÂ¥Âê
			if (Mouse::new_mx_ >= 0) {	// „Éû„Ç¶„Çπ„Éù„Ç§„É≥„Çø„ÇíÁßªÂã
				Sti();
				SheetCtl::slide(Mouse::sheet_, Mouse::new_mx_ - 8, Mouse::new_my_ - 8);
				Mouse::new_mx_ = -1;
			} else {	// „Çø„Çπ„ÇØ„ÅÆ„Çπ„É™„Éº„É
				task->sleep();
				Sti();
			}
		} else {	// FIFO„Éê„ÉÉ„Éï„Ç°„ÅÆÂá¶Áê
			code = task->queue_.pop();
			Sti();
			switch (phase_) {
			case 0:
				if (code == 0xfa) phase_++;
				break;
			case 1:
				if ((code & 0xc8) == 0x08) {
					buf_[0] = code;
					phase_++;
				}
				break;
			case 2:
				buf_[1] = code;
				phase_++;
				break;
			case 3:
				buf_[2] = code;
				phase_ = 1;
				btn_ = buf_[0] & 0x07;
				dx = buf_[1];
				dy = buf_[2];
				if (buf_[0] & 0x10) dx |= 0xffffff00;
				if (buf_[0] & 0x20) dy |= 0xffffff00;
				x_ += dx;
				y_ -= dy;
				if (x_ < 0) x_ = 0;
				if (y_ < 0) y_ = 0;
				if (x_ > SheetCtl::scrnx_ - 1) {
					x_ = SheetCtl::scrnx_ - 1;
				}
				if (y_ > SheetCtl::scrny_ - 1) {
					y_ = SheetCtl::scrny_ - 1;
				}
				new_mx_ = x_;
				new_my_ = y_;
				if (btn_ & 0x01) {	// Â∑¶„ÇØ„É™„É?ÇØ
					if (SheetCtl::context_menu_->height > 0) {
						SheetCtl::upDown(SheetCtl::context_menu_, -1);
					}
				} else if (btn_ & 0x02) {	// Âè≥„ÇØ„É™„É?ÇØ
					SheetCtl::slide(SheetCtl::context_menu_, x_ - SheetCtl::context_menu_->bxsize / 2, y_ - SheetCtl::context_menu_->bysize / 2);
					if (SheetCtl::context_menu_->height < 0) {
						SheetCtl::upDown(SheetCtl::context_menu_, SheetCtl::top_);
					}
				}
				break;
			}
		}
	}
}
