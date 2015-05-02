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
	DateTime::init();
	KeyboardController::init();
	Mouse::Init();
	Output8(kPic0Imr, 0xf8); /* PITとPIC1とキーボードを許可(11111000) */
	Output8(kPic1Imr, 0xef); /* マウスを許可(11101111) */
	Sti();

	/* system information */
	SysinfoInit();

	/* Web Browser */
	new Task("Web Browsing Task", 1, 2, []() {
		Browser::View("index.htm");
		TaskController::getNowTask()->sleep(); // 永遠にスリープ
	});

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
