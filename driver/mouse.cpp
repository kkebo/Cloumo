#include <MinMax.h>
#include "../headers.h"

const char *Mouse::cursor[] = {
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
bool Mouse::scroll = false;
Point Mouse::newPos(-1, 0);
Sheet *Mouse::sheet;
TaskQueue *Mouse::queue;
MouseDecode Mouse::mdec;
//Task *Mouse::browserTask = nullptr;

void Mouse::Main() {
	Task *task = TaskSwitcher::getNowTask();
	unsigned char code;
	int dx, dy;
	
	// メンバ初期化
	newPos = Point(-1, 0);
	mdec.pos = Point(SheetCtl::resolution.width / 2, SheetCtl::resolution.height / 2);
	mdec.phase = 0;
	mdec.scroll = 0;
	queue = task->queue;
	
	// マウスポインタ描画
	Mouse::sheet = new Sheet(Size(16, 16), true);
	for (int y = 0; y < 16; ++y) {
		for (int x = 0; x < 16; ++x) {
			switch (Mouse::cursor[x][y]) {
				case 'O':
					Mouse::sheet->buf[y * 16 + x] = Rgb(255, 255, 255, 100);
					break;
				case '@':
					Mouse::sheet->buf[y * 16 + x] = Rgb(12, 69, 255, 100);
					break;
				case 'G':
					Mouse::sheet->buf[y * 16 + x] = Rgb(27, 81, 255, 100);
					break;
				case 'J':
					Mouse::sheet->buf[y * 16 + x] = Rgb(58, 104, 255, 100);
					break;
				case 'C':
					Mouse::sheet->buf[y * 16 + x] = Rgb(73, 116, 255, 100);
					break;
				case 'U':
					Mouse::sheet->buf[y * 16 + x] = Rgb(0, 182, 200, 100);
					break;
				default:
					Mouse::sheet->buf[y * 16 + x] = kTransColor;
					break;
			}
		}
	}
	Mouse::sheet->moveTo(mdec.pos);
	Mouse::sheet->upDown(SheetCtl::top + 1);
	
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
			if (newPos.x >= 0) {
				Sti();
				sheet->moveTo(newPos + Point(-8, -8));
				newPos.x = -1;
			} else {
				task->sleep();
				Sti();
			}
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
					newPos = mdec.pos;
					
					if (mdec.btn & 0x01) { // On left click
						// Close the context menu
						if (SheetCtl::contextMenu->height > 0) {
							SheetCtl::contextMenu->upDown(-1);
						}
						
						// 各シートの onClick イベントを発動
						for (int i = SheetCtl::top - 1; i >= 0; --i) {
							Sheet &sht = *SheetCtl::sheets[i];
							if (sht.onClick && sht.frame.contains(mdec.pos)) {
								sht.onClick(mdec.pos);
								break;
							}
						}
					} else if (mdec.btn & 0x02 && SheetCtl::contextMenu->height < 0) { // On right click
						// Open the context menu
						SheetCtl::contextMenu->moveTo(Point(mdec.pos.x - SheetCtl::contextMenu->frame.size.width / 2, mdec.pos.y - SheetCtl::contextMenu->frame.size.height / 2));
						SheetCtl::contextMenu->upDown(SheetCtl::top);
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
					
					// とりあえず表示
					/*char str[20];
					sprintf(str, "%d", mdec.scroll);
					SheetCtl::fillRect(SheetCtl::back_, Rgb(255, 255, 255), 2, 300, SheetCtl::back_->bxsize - 3, 316);
					SheetCtl::drawString(SheetCtl::back_, 2, 300, 0, str);
					SheetCtl::refresh(SheetCtl::back_, 2, 300, SheetCtl::back_->bxsize - 3, 316);*/
					
					// スクロール
					//if (browserTask && (mdec.scroll == 1 || mdec.scroll == -1)) {
					//	browserTask->queue_->push(mdec.scroll);
					//}
					
					break;
			}
		}
	}
}
