#include "../headers.h"
#include <stdio.h>

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
bool Mouse::scroll_ = false;
int Mouse::new_mx_ = -1;
int Mouse::new_my_ = 0;
Sheet *Mouse::sheet_ = nullptr;
Queue *Mouse::queue_ = nullptr;
MouseDecode Mouse::mdec_;
Task *Mouse::browserTask = nullptr;

void Mouse::Init() {
	// 初期マウスポインタ位置
	mdec_.x_ = SheetCtl::scrnx_ / 2;
	mdec_.y_ = SheetCtl::scrny_ / 2;

	// マウスポインタ描画
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
	Mouse::sheet_->vx0 = mdec_.x_ - 8;
	Mouse::sheet_->vy0 = mdec_.y_ - 8;
	SheetCtl::upDown(Mouse::sheet_, SheetCtl::top_ + 1);

	// マウスドライバタスク作成
	Task *task = new Task((char *)kMouseTaskName, 1, 1, &Mouse::Main, new Queue(128));
	queue_ = task->queue_;
	
	// マウス初期化 by uchan
	int i = 0;
	int errors = 0; // エラーの回数
	bool send = false; // マウスへ送信したらtrue
	for (;;) {
		/*
		 * まずマウスへ制御信号送信
		 */
		if (i == 0 && send == false) {
			KeyboardController::wait();
			Output8(kPortKeyCmd, kKeyCmdSendToMouse);
			KeyboardController::wait();
			Output8(kPortKeyData, 0xf3);
			send = true;
		} else if (i == 1 && send == false) {
			KeyboardController::wait();
			Output8(kPortKeyCmd, kKeyCmdSendToMouse);
			KeyboardController::wait();
			Output8(kPortKeyData, 200);
			send = true;
		} else if (i == 2 && send == false) {
			KeyboardController::wait();
			Output8(kPortKeyCmd, kKeyCmdSendToMouse);
			KeyboardController::wait();
			Output8(kPortKeyData, 0xf3);
			send = true;
		} else if (i == 3 && send == false) {
			KeyboardController::wait();
			Output8(kPortKeyCmd, kKeyCmdSendToMouse);
			KeyboardController::wait();
			Output8(kPortKeyData, 100);
			send = true;
		} else if (i == 4 && send == false) {
			KeyboardController::wait();
			Output8(kPortKeyCmd, kKeyCmdSendToMouse);
			KeyboardController::wait();
			Output8(kPortKeyData, 0xf3);
			send = true;
		} else if (i == 5 && send == false) {
			KeyboardController::wait();
			Output8(kPortKeyCmd, kKeyCmdSendToMouse);
			KeyboardController::wait();
			Output8(kPortKeyData, 80);
			send = true;
		} else if (i == 6 && send == false) {
			KeyboardController::wait();
			Output8(kPortKeyCmd, kKeyCmdSendToMouse);
			KeyboardController::wait();
			Output8(kPortKeyData, 0xf2);
			send = true;
		}
		Cli();
		if (queue_->isempty()) {
			task->sleep();
			Sti();
		} else {
			int data = queue_->pop();
			Sti();
				
			if (0 <= i && i <= 6 && data == 0xfa) {
				// エラー無し
			} else if (i == 7 && data == 0) {
				// ホイール無し
				scroll_ = false;
			} else if (i == 7 && data == 3) {
				// ホイール有り
				scroll_ = true;
			}
			
			if (i == 7 || errors > 10) {
				/*
				 * マウス有効化が終わったのでループを抜ける
				 */
				break;
			}
			
			if (data != 0xfe) {
				// 再送要求では無かった
				i++;
				send = false;
			} else {
				errors++;
			}
		}
	}
	
	KeyboardController::wait();
	Output8(kPortKeyCmd, kKeyCmdSendToMouse);
	KeyboardController::wait();
	Output8(kPortKeyData, kMouseCmdEnable);
	
	mdec_.phase_ = 0;
	mdec_.scroll_ = 0;
}

void Mouse::Main() {
	Task *task = TaskController::getNowTask();
	unsigned char code;
    int dx, dy;

	for (;;) {
		Cli();
		if (queue_->isempty()) {
			if (Mouse::new_mx_ >= 0) {
				Sti();
				SheetCtl::slide(Mouse::sheet_, Mouse::new_mx_ - 8, Mouse::new_my_ - 8);
				Mouse::new_mx_ = -1;
			} else {
				task->sleep();
				Sti();
			}
		} else {
			code = queue_->pop();
			Sti();
			switch (mdec_.phase_) {
				case 0:
					if (code == 0xfa) mdec_.phase_++;
					break;
				case 1:
					if ((code & 0xc8) == 0x08) {
						mdec_.buf_[0] = code;
						mdec_.phase_++;
					}
					break;
				case 2:
					mdec_.buf_[1] = code;
					mdec_.phase_++;
					break;
				case 3:
					mdec_.buf_[2] = code;
					
					if (scroll_) {
						mdec_.phase_++;
					} else {
						mdec_.phase_ = 1;
					}
					
					mdec_.btn_ = mdec_.buf_[0] & 0x07;
					dx = mdec_.buf_[1];
					dy = mdec_.buf_[2];
					
					if (mdec_.buf_[0] & 0x10) dx |= 0xffffff00;
					if (mdec_.buf_[0] & 0x20) dy |= 0xffffff00;
					mdec_.x_ += dx;
					mdec_.y_ -= dy;
					if (mdec_.x_ < 0) mdec_.x_ = 0;
					if (mdec_.y_ < 0) mdec_.y_ = 0;
					if (mdec_.x_ > SheetCtl::scrnx_ - 1) {
						mdec_.x_ = SheetCtl::scrnx_ - 1;
					}
					if (mdec_.y_ > SheetCtl::scrny_ - 1) {
						mdec_.y_ = SheetCtl::scrny_ - 1;
					}
					new_mx_ = mdec_.x_;
					new_my_ = mdec_.y_;
					
					if (mdec_.btn_ & 0x01) {	// On left click
						if (SheetCtl::context_menu_->height > 0) {
							SheetCtl::upDown(SheetCtl::context_menu_, -1);
						} else if (2 <= mdec_.x_ && mdec_.x_ < SheetCtl::back_->bxsize) {
							if (35 <= mdec_.y_ && mdec_.y_ < 33 + 16 + 8) {
								SheetCtl::colorChange(SheetCtl::back_, 2, 35, SheetCtl::back_->bxsize, 33 + 16 + 8, Rgb(127, 169, 255), Rgb(255, 255, 255));
								SheetCtl::colorChange(SheetCtl::back_, 2, 35, SheetCtl::back_->bxsize, 33 + 16 + 8, Rgb(0, 42, 127), 0);
								SheetCtl::colorChange(SheetCtl::back_, 2, 33 + 16 + 8 + 1, SheetCtl::back_->bxsize, 31 + 16 + 8 + 1 + 16 + 8, Rgb(255, 255, 255), Rgb(127, 169, 255));
								SheetCtl::colorChange(SheetCtl::back_, 2, 33 + 16 + 8 + 1, SheetCtl::back_->bxsize, 31 + 16 + 8 + 1 + 16 + 8, 0, Rgb(0, 42, 127));
								SheetCtl::refresh(SheetCtl::back_, 2, 35, SheetCtl::back_->bxsize, 31 + 16 + 8 + 1 + 16 + 8);
								SheetCtl::upDown(SheetCtl::window_[1], -1);
								SheetCtl::upDown(SheetCtl::window_[0], 1);
							} else if (33 + 16 + 8 + 1 <= mdec_.y_ && mdec_.y_ < 31 + 16 + 8 + 1 + 16 + 8) {
								SheetCtl::colorChange(SheetCtl::back_, 2, 35, SheetCtl::back_->bxsize, 33 + 16 + 8, Rgb(255, 255, 255), Rgb(127, 169, 255));
								SheetCtl::colorChange(SheetCtl::back_, 2, 35, SheetCtl::back_->bxsize, 33 + 16 + 8, 0, Rgb(0, 42, 127));
								SheetCtl::colorChange(SheetCtl::back_, 2, 33 + 16 + 8 + 1, SheetCtl::back_->bxsize, 31 + 16 + 8 + 1 + 16 + 8, Rgb(127, 169, 255), Rgb(255, 255, 255));
								SheetCtl::colorChange(SheetCtl::back_, 2, 33 + 16 + 8 + 1, SheetCtl::back_->bxsize, 31 + 16 + 8 + 1 + 16 + 8, Rgb(0, 42, 127), 0);
								SheetCtl::refresh(SheetCtl::back_, 2, 35, SheetCtl::back_->bxsize, 31 + 16 + 8 + 1 + 16 + 8);
								SheetCtl::upDown(SheetCtl::window_[0], -1);
								SheetCtl::upDown(SheetCtl::window_[1], 1);
							}
						}
					} else if (mdec_.btn_ & 0x02) {	// On right click
						SheetCtl::slide(SheetCtl::context_menu_, mdec_.x_ - SheetCtl::context_menu_->bxsize / 2, mdec_.y_ - SheetCtl::context_menu_->bysize / 2);
						if (SheetCtl::context_menu_->height < 0) {
							SheetCtl::upDown(SheetCtl::context_menu_, SheetCtl::top_);
						}
					}
					break;
				case 4:
					mdec_.buf_[3] = code;
					mdec_.phase_ = 1;
					
					// mdec_.buf_[3]は、下位4ビットだけが有効な値である
					// とりあえず解析せずに値をしまう。
					mdec_.scroll_ = mdec_.buf_[3] & 0x0f;
					if (mdec_.scroll_ & 0x08) {
						// マイナスの値だった
						mdec_.scroll_ |= 0xfffffff0;
					}
					
					// とりあえず表示
					/*char str[20];
					sprintf(str, "%d", mdec_.scroll_);
					SheetCtl::fillRect(SheetCtl::back_, Rgb(255, 255, 255), 2, 300, SheetCtl::back_->bxsize - 3, 316);
					SheetCtl::drawString(SheetCtl::back_, 2, 300, 0, str);
					SheetCtl::refresh(SheetCtl::back_, 2, 300, SheetCtl::back_->bxsize - 3, 316);*/
					
					// スクロール
					if (browserTask) {
						browserTask->queue_->push(mdec_.scroll_);
					}
					
					break;
			}
		}
	}
}
