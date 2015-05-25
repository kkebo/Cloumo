#include "../headers.h"

void PICInit() {
	// すべての割り込みを受け付けない
	Output8(kPic0Imr, 0xff);
	Output8(kPic1Imr, 0xff);

	// エッジトリガモード
	Output8(kPic0Icw1, 0x11);
	// IRQ0-7は、INT20-27で受ける
	Output8(kPic0Icw2, 0x20);
	// PIC1はIRQ2にて接続
	Output8(kPic0Icw3, 1 << 2);
	// ノンバッファモード
	Output8(kPic0Icw4, 0x01);

	// エッジトリガモード
	Output8(kPic1Icw1, 0x11);
	// IRQ0-7は、INT20-27で受ける
	Output8(kPic1Icw2, 0x28);
	// PIC1はIRQ2にて接続
	Output8(kPic1Icw3, 2);
	// ノンバッファモード
	Output8(kPic1Icw4, 0x01);

	// 11111011 PIC1以外はすべて禁止
	Output8(kPic0Imr, 0xfb);
	// 11111111 すべての割り込みを受け付けない
	Output8(kPic1Imr, 0xff);
}

// FPU
void IntHandler07(int *esp) {
	Task *now = TaskSwitcher::getNowTask();
	asm volatile("clts");
	if (TaskSwitcher::taskFPU != now) {
		if (TaskSwitcher::taskFPU) {
			int *p = (int *)TaskSwitcher::taskFPU->fpu;
			asm volatile("fnsave %0" : "=m"(p));
		}
		int *p = (int *)now->fpu;
		asm volatile("frstor %0" : "=m"(p));
		TaskSwitcher::taskFPU = now;
	}
}

// PIT割り込み
void IntHandler20(int *esp) {
	Timer *timer;
	bool ts = false;
	
	/* IRQ-00受付完了をPICに通知 */
	Output8(kPic0Ocw2, 0x60);
	
	// count をインクリメント
	++TimerController::count;
	
	// まだ直近のタイムアウトまで時間がある
	if (TimerController::next > TimerController::count) return;
	
	// タイムアウト処理
	timer = TimerController::t0;
	while (timer->timeout <= TimerController::count) {
		timer->running = false;
		if (timer != TaskSwitcher::timer) {
			// 一般タイマーのタイムアウト
			timer->queue->push(timer->data);
		} else {
			// タスクスイッチ用タイマーのタイムアウト
			ts = true;
		}
		timer = timer->next;
	}
	
	// 次のタイムアウトのための準備
	TimerController::t0 = timer;
	TimerController::next = timer->timeout;
	
	// タスクスイッチ
	if (ts) TaskSwitcher::switchTask();
	
	// オーバーフローする前にリセット (これじゃだめだった)
	/*if (TimerController::count >= 0xf0000000) {
		TimerController::reset();
	}*/
}

// PS/2キーボードからの割り込み
void IntHandler21(int *esp) {
	/* IRQ-01受付完了をPICに通知 */
	Output8(kPic0Ocw2, 0x61);

	KeyboardController::queue->push(Input8(kPortKeyData));
}

// PS/2マウスからの割り込み
void IntHandler2c(int *esp) {
	/* IRQ-12受付完了をPIC1に通知 */
	Output8(kPic1Ocw2, 0x64);
	/* IRQ-02受付完了をPIC0に通知 */
	Output8(kPic0Ocw2, 0x62);

	Mouse::queue->push(Input8(kPortKeyData));
}
