/*
 *　Main
 */

#include "headers.h"
#include <stdio.h>

extern "C" void _main() {
	/* 初期化 */
	MemoryInit();
	DescriptorInit();
	PICInit();
	FAT12::init();
	SheetCtl::init();
	TimerController::init();
	Task* mainTask = TaskController::init();
	mainTask->run(1, 2);
	DateTime::init();
	KeyboardController::init();
	Mouse::Init();
	Output8(kPic0Imr, 0xf8); /* PITとPIC1とキーボードを許可(11111000) */
	Output8(kPic1Imr, 0xef); /* マウスを許可(11101111) */
	Sti();

	/* system info */
	char str[100];
	sprintf(str, "FREE %8dB/%dMB", MemoryTotal(), MemoryTest(0x00400000, 0xbfffffff) / 1024 / 1024);
	SheetCtl::drawString(SheetCtl::window_[1], 2, 2, 0, str);
	SheetCtl::drawString(SheetCtl::window_[1], 2, 18, 0, "level priority task name");
	for (int i = 0; i < 4; i++) {
		if (TaskController::tasks0_[i].flags_) {
			sprintf(str, "%5d %8d %s", TaskController::tasks0_[i].level_, TaskController::tasks0_[i].priority_, TaskController::tasks0_[i].name_);
			SheetCtl::drawString(SheetCtl::window_[1], 2, 34 + i * 16, 0, str);
		}
	}

	/* HTML */
	Browser::View("index.htm");

	/* 起動音 */
	Timer* btsound = TimerController::alloc();
	btsound->init(new Queue(128, mainTask));
	Beep('D', 8, btsound, true);
	Beep('F', 8, btsound, true);
	Beep(0, 8, btsound, true);
	Beep('E', 8, btsound, true);
	Beep('G', 8, btsound, true);
	Beep(0, 8, btsound);
	btsound->free();

	for (;;) {
		mainTask->sleep();
	}
}
