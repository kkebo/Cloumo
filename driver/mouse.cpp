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
TaskQueue *Mouse::queue_ = nullptr;
MouseDecode Mouse::mdec_;
Task *Mouse::browserTask = nullptr;

void Mouse::Main() {
	Task *task = TaskController::getNowTask();
	unsigned char code;
	int dx, dy;
	
	// メンバ初期化
	mdec_.x_ = SheetCtl::scrnx_ / 2;
	mdec_.y_ = SheetCtl::scrny_ / 2;
	mdec_.phase_ = 0;
	mdec_.scroll_ = 0;
	queue_ = task->queue_;
	
	// マウスポインタ描画
	Mouse::sheet_ = SheetCtl::alloc(16, 16, true);
	for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 16; x++) {
			switch (Mouse::cursor_[x][y]) {
				case 'O':
					Mouse::sheet_->buf[y * 16 + x] = Rgb(255, 255, 255, 100);
					break;
				case '@':
					Mouse::sheet_->buf[y * 16 + x] = Rgb(12, 69, 255, 100);
					break;
				case 'G':
					Mouse::sheet_->buf[y * 16 + x] = Rgb(27, 81, 255, 100);
					break;
				case 'J':
					Mouse::sheet_->buf[y * 16 + x] = Rgb(58, 104, 255, 100);
					break;
				case 'C':
					Mouse::sheet_->buf[y * 16 + x] = Rgb(73, 116, 255, 100);
					break;
				case 'U':
					Mouse::sheet_->buf[y * 16 + x] = Rgb(0, 182, 200, 100);
					break;
				default:
					Mouse::sheet_->buf[y * 16 + x] = kTransColor;
					break;
			}
		}
	}
	Mouse::sheet_->vx0 = mdec_.x_ - 8;
	Mouse::sheet_->vy0 = mdec_.y_ - 8;
	SheetCtl::upDown(Mouse::sheet_, SheetCtl::top_ + 1);
	
	// マウス初期化 by uchan
	int i = 0;
	int errors = 0; // エラーの回数
	bool send = false; // マウスへ送信したらtrue
	for (;;) {
		/*
		 * まずマウスへ制御信号送信
		 */
		if (send == false) {
			switch (i) {
				case 0:
					KeyboardController::wait();
					Output8(kPortKeyCmd, kKeyCmdSendToMouse);
					KeyboardController::wait();
					Output8(kPortKeyData, 0xf3);
					send = true;
					break;
				case 1:
					KeyboardController::wait();
					Output8(kPortKeyCmd, kKeyCmdSendToMouse);
					KeyboardController::wait();
					Output8(kPortKeyData, 200);
					send = true;
					break;
				case 2:
					KeyboardController::wait();
					Output8(kPortKeyCmd, kKeyCmdSendToMouse);
					KeyboardController::wait();
					Output8(kPortKeyData, 0xf3);
					send = true;
					break;
				case 3:
					KeyboardController::wait();
					Output8(kPortKeyCmd, kKeyCmdSendToMouse);
					KeyboardController::wait();
					Output8(kPortKeyData, 100);
					send = true;
					break;
				case 4:
					KeyboardController::wait();
					Output8(kPortKeyCmd, kKeyCmdSendToMouse);
					KeyboardController::wait();
					Output8(kPortKeyData, 0xf3);
					send = true;
					break;
				case 5:
					KeyboardController::wait();
					Output8(kPortKeyCmd, kKeyCmdSendToMouse);
					KeyboardController::wait();
					Output8(kPortKeyData, 80);
					send = true;
					break;
				case 6:
					KeyboardController::wait();
					Output8(kPortKeyCmd, kKeyCmdSendToMouse);
					KeyboardController::wait();
					Output8(kPortKeyData, 0xf2);
					send = true;
					break;
			}
		}
		Cli();
		if (queue_->isempty()) {
			task->sleep();
			Sti();
		} else {
			int data = queue_->pop();
			Sti();
			
			if (0 <= i && i <= 6 && data == 0xfa) {
				// 正常に ACK が来た
			} else if (i == 7 && data == 0) {
				// ホイール無し
				scroll_ = false;
			} else if (i == 7 && data == 3) {
				// ホイール有り
				scroll_ = true;
			}
			
			if (i == 7 || errors > 10) {
				break;
			}
			
			if (data != 0xfe) {
				i++;
				send = false;
			} else {
				// 再送要求
				errors++;
			}
		}
	}
	
	KeyboardController::wait();
	Output8(kPortKeyCmd, kKeyCmdSendToMouse);
	KeyboardController::wait();
	Output8(kPortKeyData, kMouseCmdEnable);

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
							for (int i = 0; i < SheetCtl::numOfTab; i++) {
								if (i != SheetCtl::activeTab && 35 + 23 * i <= mdec_.y_ && mdec_.y_ < 33 + 16 + 8 + 23 * i) {
									// 選択したタブ
									SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * i, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * i, kPassiveTabColor, kActiveTabColor);
									SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * i, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * i, kPassiveTextColor, kActiveTextColor);
									SheetCtl::refresh(*SheetCtl::back_, 2, 35 + 23 * i, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * i);
									// アクティブだったタブ
									SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTabColor, kPassiveTabColor);
									SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTextColor, kPassiveTextColor);
									SheetCtl::refresh(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab);
									
									SheetCtl::upDown(SheetCtl::window_[SheetCtl::activeTab], -1);
									SheetCtl::upDown(SheetCtl::window_[i], 1);
									
									SheetCtl::activeTab = i;
								}
							}
						}
					} else if (mdec_.btn_ & 0x02 && SheetCtl::context_menu_->height < 0) {	// On right click
						SheetCtl::slide(SheetCtl::context_menu_, mdec_.x_ - SheetCtl::context_menu_->bxsize / 2, mdec_.y_ - SheetCtl::context_menu_->bysize / 2);
						SheetCtl::upDown(SheetCtl::context_menu_, SheetCtl::top_);
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
					if (browserTask && (mdec_.scroll_ == 1 || mdec_.scroll_ == -1)) {
						browserTask->queue_->push(mdec_.scroll_);
					}
					
					break;
			}
		}
	}
}
