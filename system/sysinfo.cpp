#include "../headers.h"
#include <stdio.h>

void SysinfoMain();

void SysinfoInit() {
	Task *task = TaskController::alloc();
	task->name_ = (char *)kSysInfoTaskName;
	task->tss_.esp = (int)malloc4k(64 * 1024) + 64 * 1024 - 12;
	task->tss_.eip = (int)&SysinfoMain;
	task->tss_.es = 1 * 8;
	task->tss_.cs = 2 * 8;
	task->tss_.ss = 1 * 8;
	task->tss_.ds = 1 * 8;
	task->tss_.fs = 1 * 8;
	task->tss_.gs = 1 * 8;
	task->run(2, 2);
}

void SysinfoMain() {
	char str[100];

	for (;;) {
		SheetCtl::fillRect(SheetCtl::window_[1], Rgb(255, 255, 255), 2, 2, SheetCtl::window_[1]->bxsize - 1, 34 + 5 * 16 + 8);
		auto memTotal = MemoryTotal();
		sprintf(str, "RAM: %d MB    FREE: %u MB (%u Byte)", MemoryTest(0x00400000, 0xbfffffff) / 1024 / 1024, memTotal / 1024 / 1024, memTotal);
		SheetCtl::drawString(SheetCtl::window_[1], 2, 2, 0, str);
		SheetCtl::drawString(SheetCtl::window_[1], 2, 2 + 16 * 2, 0, "level priority task name");
		for (int i = 0; i < 6; i++) {
			if (TaskController::tasks0_[i].flags_) {
				sprintf(str, "%5d %8d %s", TaskController::tasks0_[i].level_, TaskController::tasks0_[i].priority_, TaskController::tasks0_[i].name_);
				SheetCtl::drawString(SheetCtl::window_[1], 2, 2 + 16 * 3 + i * 16, 0, str);
			}
		}
		SheetCtl::refresh(SheetCtl::window_[1], 2, 2, SheetCtl::scrnx_ - 1, 2 + 16 * 3 + 5 * 16 + 8);
	}
}
