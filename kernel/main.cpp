/*
 *　Main
 */

#include "../headers.h"

int main() {
	/* 初期化 */
	MemoryInit();
	DescriptorInit();
	PICInit();
	FAT12::init();
	TimerController::init();
	Task &mainTask = *TaskSwitcher::init();
	Cli();
	Output8(kPic0Imr, 0xf8); /* PITとPIC1とキーボードを許可(11111000) */
	Output8(kPic1Imr, 0xef); /* マウスを許可(11101111) */
	Sti();
	
	// タスクの起動
	SheetCtl::init();
	new Task("日付と時刻タスク", 2, 1, 128, &DateTimeMain);
	new Task("Japanese 106/109 Keyboard Driver", 2, 2, 128, &KeyboardController::Main);
	new Task("マウスドライバ", 1, 1, 128, &Mouse::Main);
	new Task("システム情報タスク", 2, 1, 128, &SysinfoMain);
	/*new Task((char *)kBrowserTaskName, 2, 2, 128, [] {
		Task *task = TaskSwitcher::getNowTask();
		Browser *browser = new Browser("index.htm");
		browser->Render();
		
		bool refreshRequired = false;
		
		for (;;) {
			Cli();
			if (task->queue_->isempty()) {
				if (refreshRequired) {
					SheetCtl::refresh(*SheetCtl::window_[0], 1, 1, SheetCtl::window_[0]->bxsize - 2, SheetCtl::window_[0]->bysize - 1);
					refreshRequired = false;
				}
				task->sleep();
				Sti();
			} else {
				int data = task->queue_->pop();
				Sti();
				browser->Scroll(data);
				refreshRequired = true;
			}
		}
	});*/

	/* 起動音 */
	/*Queue *btsoundQueue = new Queue(128);
	Timer *btsound = new Timer(btsoundQueue);
	Beep('D', 8, btsound, true);
	Beep('F', 8, btsound, true);
	Beep(0, 8, btsound, true);
	Beep('E', 8, btsound, true);
	Beep('G', 8, btsound, true);
	Beep(0, 8, btsound);
	delete btsound;*/

	// もはや用無し
	mainTask.run(MAX_TASKLEVELS - 1, 1);

	for (;;) {
		Hlt();
	}
}
