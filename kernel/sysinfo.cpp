#include "../headers.h"
#include <stdio.h>
#include <pistring.h>

void showSysInfo(int benchScore) {
	string str;
	auto memTotal = MemoryTotal();
	
	// Clear the screen
	SheetCtl::fillRect(SheetCtl::window[0], Rgb(255, 255, 255), 2, 2, SheetCtl::window[0]->frame.vector.x - 1, SheetCtl::window[0]->frame.vector.y - 1);
	
	// Benchmark Result
	str = to_string(benchScore);
	SheetCtl::drawString(SheetCtl::window[0], 2, 2, 0, "Benchmark Score:");
	SheetCtl::drawString(SheetCtl::window[0], 2 + 8 * 17, 2, 0, str);
	
	// Memory Information
	str = "RAM: " + to_string(MemoryTest(0x00400000, 0xbfffffff) / 1024 / 1024) + " MB    FREE: " + to_string(memTotal / 1024 / 1024) + " MB (" + to_string(memTotal) + " Byte)";
	SheetCtl::drawString(SheetCtl::window[0], 2, 2 + 16, 0, str);
	
	// Display Information
	str = "Resoultion: " + to_string(SheetCtl::scrnx) + " x " + to_string(SheetCtl::scrny) + " (" + to_string(SheetCtl::color) + "-bit color)";
	SheetCtl::drawString(SheetCtl::window[0], 2, 2 + 16 * 2, 0, str);
	
	// Task List
	SheetCtl::drawString(SheetCtl::window[0], 2 + 1, 2 + 16 * 4 + 1, 0, "level priority flag task name");
	int j = 0;
	str.reserve(20);
	for (int i = 0; i < MAX_TASKS; ++i) {
		Task *task = &TaskController::tasks0_[i];
		if (task->flags_ != TaskFlag::Free) {
			sprintf(str.c_str(), "%5d %8d %4s ", task->level_, task->priority_, (task->flags_ == TaskFlag::Running) ? "(oo)" : "(__)");
			str = str.c_str(); // サイズの数え直し
			str += task->name_;
			SheetCtl::drawString(SheetCtl::window[0], 2 + 1, 2 + 16 * 5 + j * 16 + 2, 0, str);
			++j;
		}
	}
	SheetCtl::drawRect(SheetCtl::window[0], 0, 2, 2 + 16 * 4, SheetCtl::window[0]->frame.vector.x - 1 - 1, 2 + 16 * 5 + j * 16 + 3);
	SheetCtl::drawLine(SheetCtl::window[0], 0, 3, 2 + 16 * 5 + 1, SheetCtl::window[0]->frame.vector.x - 1 - 2, 2 + 16 * 5 + 1);
	SheetCtl::drawLine(SheetCtl::window[0], 0, 3 + 5 * 8 + 3, 2 + 16 * 4 + 1, 3 + 5 * 8 + 3, 2 + 16 * 5 + j * 16 + 2);
	SheetCtl::drawLine(SheetCtl::window[0], 0, 3 + 14 * 8 + 3, 2 + 16 * 4 + 1, 3 + 14 * 8 + 3, 2 + 16 * 5 + j * 16 + 2);
	SheetCtl::drawLine(SheetCtl::window[0], 0, 3 + 19 * 8 + 3, 2 + 16 * 4 + 1, 3 + 19 * 8 + 3, 2 + 16 * 5 + j * 16 + 2);
	
	// Refresh the screen
	SheetCtl::window[0]->refresh(Rect(2, 2, SheetCtl::window[0]->frame.vector.x - 1 - 2, SheetCtl::window[0]->frame.vector.y - 1 - 2));
}

void SysinfoMain() {
	Task *task = TaskController::getNowTask();
	int count = 0, count0 = 0;
	
	Timer *timer = new Timer(task->queue_);
	timer->set(100);
	
	showSysInfo(0);
	
	for (;;) {
		++count;
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
