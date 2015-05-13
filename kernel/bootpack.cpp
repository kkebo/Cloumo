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
	new Task((char *)kTimeTaskName, 2, 1, 128, &DateTime::Main);
	new Task((char *)kKeyboardTaskName, 2, 2, 128, &KeyboardController::Main);
	new Task((char *)kMouseTaskName, 1, 1, 128, &Mouse::Main);
	new Task((char *)kSysInfoTaskName, 2, 1, 128, &SysinfoMain);
	new Task((char *)kBrowserTaskName, 2, 2, 128, []() {
		Task *task = TaskController::getNowTask();
		//Browser *browser = new Browser("index.htm");
		//browser->Render();
		File *htmlFile;
		if (htmlFile = FAT12::open("index.htm")) {
			char *source = htmlFile->read();
			HTML::Tokenizer tokenizer;
			Queue<HTML::Token *> *tokens = tokenizer.tokenize(source);
			for (int i = 0; !tokens->isempty(); i++) {
				SheetCtl::drawString(SheetCtl::window_[0], 1, 1 + i * 16, 0, tokens->pop()->getData());
			}
			delete htmlFile;
		}
		
		//bool refreshRequired = false;
		
		for (;;) {
			Cli();
			if (task->queue_->isempty()) {
				/*if (refreshRequired) {
					SheetCtl::refresh(*SheetCtl::window_[0], 1, 1, SheetCtl::window_[0]->bxsize - 2, SheetCtl::window_[0]->bysize - 1);
					refreshRequired = false;
				}*/
				task->sleep();
				Sti();
			} else {
				//int data = task->queue_->pop();
				Sti();
				//browser->Scroll(data);
				//refreshRequired = true;
			}
		}
	});

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
	mainTask->run(MAX_TASKLEVELS - 1, 1);

	for (;;) {
		Hlt();
	}
}
