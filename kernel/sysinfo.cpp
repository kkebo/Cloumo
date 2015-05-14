#include "../headers.h"
#include <stdio.h>
#include <pistring.h>

void showSysInfo(int benchScore) {
	string str;
	auto memTotal = MemoryTotal();
	
	// Clear the screen
	SheetCtl::fillRect(SheetCtl::window_[0], Rgb(255, 255, 255), 2, 2, SheetCtl::window_[0]->bxsize - 1, SheetCtl::window_[0]->bysize - 1);
	
	// Benchmark Result
	//sprintf(str.c_str(), "%11d", benchScore);
	str << benchScore;
	SheetCtl::drawString(SheetCtl::window_[0], 2, 2, 0, "Benchmark Score:");
	SheetCtl::drawString(SheetCtl::window_[0], 2 + 8 * 17, 2, 0, str);
	
	// Memory Information
	str = "RAM: ";
	str << MemoryTest(0x00400000, 0xbfffffff) / 1024 / 1024 << " MB    FREE: " << memTotal / 1024 / 1024 << " MB (" << memTotal << " Byte)";
	//sprintf(str.c_str(), "RAM: %d MB    FREE: %u MB (%u Byte)", MemoryTest(0x00400000, 0xbfffffff) / 1024 / 1024, memTotal / 1024 / 1024, memTotal);
	SheetCtl::drawString(SheetCtl::window_[0], 2, 2 + 16, 0, str);
	
	// Display Information
	str = "Resoultion: ";
	str << SheetCtl::scrnx_ << " x " << SheetCtl::scrny_ << " (" << SheetCtl::color_ << "-bit color)";
	//sprintf(str.c_str(), "Resoultion: %d x %d (%d-bit color)", SheetCtl::scrnx_, SheetCtl::scrny_, SheetCtl::color_);
	SheetCtl::drawString(SheetCtl::window_[0], 2, 2 + 16 * 2, 0, str);
	
	// Task List
	SheetCtl::drawString(SheetCtl::window_[0], 2 + 1, 2 + 16 * 4 + 1, 0, "level priority flag task name");
	int j = 0;
	str.reserve(20);
	for (int i = 0; i < MAX_TASKS; i++) {
		Task *task = &TaskController::tasks0_[i];
		if (task->flags_ != TaskFlag::Free) {
			sprintf(str.c_str(), "%5d %8d %4s ", task->level_, task->priority_, (task->flags_ == TaskFlag::Running) ? "(oo)" : "(__)");
			str += task->name_;
			SheetCtl::drawString(SheetCtl::window_[0], 2 + 1, 2 + 16 * 5 + j * 16 + 2, 0, str);
			j++;
		}
	}
	SheetCtl::drawRect(SheetCtl::window_[0], 0, 2, 2 + 16 * 4, SheetCtl::window_[0]->bxsize - 1 - 1, 2 + 16 * 5 + j * 16 + 3);
	SheetCtl::drawLine(SheetCtl::window_[0], 0, 3, 2 + 16 * 5 + 1, SheetCtl::window_[0]->bxsize - 1 - 2, 2 + 16 * 5 + 1);
	SheetCtl::drawLine(SheetCtl::window_[0], 0, 3 + 5 * 8 + 3, 2 + 16 * 4 + 1, 3 + 5 * 8 + 3, 2 + 16 * 5 + j * 16 + 2);
	SheetCtl::drawLine(SheetCtl::window_[0], 0, 3 + 14 * 8 + 3, 2 + 16 * 4 + 1, 3 + 14 * 8 + 3, 2 + 16 * 5 + j * 16 + 2);
	SheetCtl::drawLine(SheetCtl::window_[0], 0, 3 + 19 * 8 + 3, 2 + 16 * 4 + 1, 3 + 19 * 8 + 3, 2 + 16 * 5 + j * 16 + 2);
	
	// Refresh the screen
	SheetCtl::refresh(*SheetCtl::window_[0], 2, 2, SheetCtl::window_[0]->bxsize - 1, SheetCtl::window_[0]->bysize - 1);
}

void SysinfoMain() {
	Task *task = TaskController::getNowTask();
	int count = 0, count0 = 0;
	
	Timer *timer = new Timer(task->queue_);
	timer->set(100);
	
	showSysInfo(0);
	
	for (;;) {
		count++;
		Cli();
		if (task->queue_->isempty()) {
			//task->sleep(); ベンチマーク測定のため
			Sti();
		} else {
			int data = task->queue_->pop();
			Sti();
			if (data == timer->data()) {
				showSysInfo(count - count0);
				count0 = count;
				timer->set(100);
			}
		}
	}
}
