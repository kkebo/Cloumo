#include <MinMax.h>
#include "../headers.h"

bool Mouse::scroll = false;
TaskQueue *Mouse::queue;
MouseDecode Mouse::mdec;

void Mouse::Main() {
	Task *task = TaskSwitcher::getNowTask();
	unsigned char code;
	int dx, dy;
	
	// メンバ初期化
	mdec.pos = Point(SheetCtl::resolution.width / 2, SheetCtl::resolution.height / 2);
	mdec.phase = 0;
	mdec.scroll = 0;
	queue = task->queue;
	
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
		if (queue->isempty()) {
			task->sleep();
			Sti();
		} else {
			int data = queue->pop();
			Sti();
			
			if (0 <= i && i <= 6 && data == 0xfa) {
				// 正常に ACK が来た
			} else if (i == 7 && data == 0) {
				// ホイール無し
				scroll = false;
			} else if (i == 7 && data == 3) {
				// ホイール有り
				scroll = true;
			}
			
			if (i == 7 || errors > 10) {
				break;
			}
			
			if (data != 0xfe) {
				++i;
				send = false;
			} else {
				// 再送要求
				++errors;
			}
		}
	}
	
	KeyboardController::wait();
	Output8(kPortKeyCmd, kKeyCmdSendToMouse);
	KeyboardController::wait();
	Output8(kPortKeyData, kMouseCmdEnable);

	for (;;) {
		Cli();
		if (queue->isempty()) {
			task->sleep();
			Sti();
		} else {
			code = queue->pop();
			Sti();
			switch (mdec.phase) {
				case 0:
					if (code == 0xfa) ++mdec.phase;
					break;
				case 1:
					if ((code & 0xc8) == 0x08) {
						mdec.buf[0] = code;
						++mdec.phase;
					}
					break;
				case 2:
					mdec.buf[1] = code;
					++mdec.phase;
					break;
				case 3:
					mdec.buf[2] = code;
					
					if (scroll) {
						++mdec.phase;
					} else {
						mdec.phase = 1;
					}
					
					mdec.btn = mdec.buf[0] & 0x07;
					dx = mdec.buf[1];
					dy = mdec.buf[2];
					
					if (mdec.buf[0] & 0x10) dx |= 0xffffff00;
					if (mdec.buf[0] & 0x20) dy |= 0xffffff00;
					mdec.pos = Point(
						min(SheetCtl::resolution.width - 1, max(0, mdec.pos.x + dx)),
						min(SheetCtl::resolution.height - 1, max(0, mdec.pos.y - dy))
					);
					
					SheetCtl::mouseCursorPos = mdec.pos;
					SheetCtl::queue->push(256);
					
					if (mdec.btn & 0x01) { // On left click
						SheetCtl::queue->push(257);
					}
					if (mdec.btn & 0x02) { // On right click
						SheetCtl::queue->push(258);
					}
					break;
				case 4:
					mdec.buf[3] = code;
					mdec.phase = 1;
					
					// mdec.buf[3]は、下位4ビットだけが有効な値である
					// とりあえず解析せずに値をしまう。
					mdec.scroll = mdec.buf[3] & 0x0f;
					if (mdec.scroll & 0x08) {
						// マイナスの値だった
						mdec.scroll |= 0xfffffff0;
					}
					
					if (mdec.scroll == -1) {
						SheetCtl::queue->push(259);
					} else if (mdec.scroll == 1) {
						SheetCtl::queue->push(260);
					}
					
					break;
			}
		}
	}
}
