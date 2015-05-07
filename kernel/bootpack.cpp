/*
 *　Main
 */

#include "../headers.h"

extern "C" void _main() {
	/* 初期化 */
	MemoryInit();
	DescriptorInit();
	PICInit();
	FAT12::init();
	SheetCtl::init();
	TimerController::init();
	Task *mainTask = TaskController::init();
	Cli();
	Output8(kPic0Imr, 0xf8); /* PITとPIC1とキーボードを許可(11111000) */
	Output8(kPic1Imr, 0xef); /* マウスを許可(11101111) */
	Sti();
	
	// タスクの起動
	new Task((char *)kTimeTaskName, 2, 1, &DateTime::Main, new Queue(128));
	new Task((char *)kKeyboardTaskName, 2, 2, &KeyboardController::Main, new Queue(128));
	new Task((char *)kMouseTaskName, 1, 1, &Mouse::Main, new Queue(128));
	new Task((char *)kSysInfoTaskName, 2, 1, &SysinfoMain, new Queue(128));
	new Task((char *)kBrowserTaskName, 2, 2, []() {
		Task *task = TaskController::getNowTask();
		Browser *browser = new Browser("index.htm");
		browser->Render();
		
		for (;;) {
			Cli();
			if (task->queue_->isempty()) {
				task->sleep();
				Sti();
			} else {
				int data = task->queue_->pop();
				Sti();
				browser->Scroll(data);
			}
		}
	}, new Queue(128));

	/* 起動音 */
	/*Timer* btsound = TimerController::alloc();
	btsound->init(new Queue(128, mainTask));
	Beep('D', 8, btsound, true);
	Beep('F', 8, btsound, true);
	Beep(0, 8, btsound, true);
	Beep('E', 8, btsound, true);
	Beep('G', 8, btsound, true);
	Beep(0, 8, btsound);
	btsound->free();*/

	// もはや用無し
	mainTask->run(MAX_TASKLEVELS - 1, 1);

	for (;;) {
		Hlt();
	}
}
