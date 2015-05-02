#include "../headers.h"
#include <stdio.h>

void showSysInfo() {
	char str[100];
	auto memTotal = MemoryTotal();
	
	SheetCtl::fillRect(SheetCtl::window_[1], Rgb(255, 255, 255), 2, 2, SheetCtl::window_[1]->bxsize - 1, 34 + 5 * 16 + 8);
	sprintf(str, "RAM: %d MB    FREE: %u MB (%u Byte)", MemoryTest(0x00400000, 0xbfffffff) / 1024 / 1024, memTotal / 1024 / 1024, memTotal);
	SheetCtl::drawString(SheetCtl::window_[1], 2, 2, 0, str);
	SheetCtl::drawString(SheetCtl::window_[1], 2, 2 + 16 * 2, 0, "level priority flag task name");
	int j = 0;
	for (int i = 0; i < MAX_TASKS; i++) {
		Task *task = &TaskController::tasks0_[i];
		if (task->flags_ != TaskFlag::Free) {
			sprintf(str, "%5d %8d %4s %s", task->level_, task->priority_, (task->flags_ == TaskFlag::Running) ? "run" : "slp", task->name_);
			SheetCtl::drawString(SheetCtl::window_[1], 2, 2 + 16 * 3 + i * 16, 0, str);
			j++;
		}
	}
	SheetCtl::refresh(SheetCtl::window_[1], 2, 2, SheetCtl::scrnx_ - 1, 2 + 16 * 3 + j * 16 + 8);
}

void SysinfoInit() {
	new Task((char *)kSysInfoTaskName, 2, 2, []() {
		Task *task = TaskController::getNowTask();
		
		Timer *timer = TimerController::alloc();
		timer->init(task->queue_);
		timer->set(100);
		
		for (;;) {
			Cli();
			if (task->queue_->isempty()) {
				task->sleep();
			} else {
				int data = task->queue_->pop();
				Sti();
				if (data == timer->data()) {
					showSysInfo();
				}
			}
		}
	}, new Queue(128));
	showSysInfo();
}
