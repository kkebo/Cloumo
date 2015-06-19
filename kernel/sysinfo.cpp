#include <stdio.h>
#include <pistring.h>
#include "../headers.h"

void showSysInfo(Sheet *sht, int benchScore) {
	string str;
	auto memTotal = MemoryTotal();
	
	// Clear the screen
	Rectangle clearRange(Point(2, 2), Size(sht->frame.size.width - 3, sht->frame.size.height - 3));
	sht->fillRect(clearRange, 0xffffff);
	
	// Benchmark Result
	str = to_string(benchScore);
	sht->drawString("Benchmark Score:", Point(2, 2), 0);
	sht->drawString(str, Point(2 + 8 * 17, 2), 0);
	
	// Memory Information
	str = "RAM: " + to_string(MemoryTest(0x00400000, 0xbfffffff) / 1024 / 1024) + " MB    FREE: " + to_string(memTotal / 1024 / 1024) + " MB (" + to_string(memTotal) + " Byte)";
	sht->drawString(str, Point(2, 2 + 16), 0);
	
	// Display Information
	str = "Resoultion: " + to_string(SheetCtl::resolution.width) + " x " + to_string(SheetCtl::resolution.height) + " (" + to_string(SheetCtl::colorDepth) + "-bit color)";
	sht->drawString(str, Point(2, 2 + 16 * 2), 0);
	
	// Task List
	sht->drawString("level priority flag task name", Point(2 + 1, 2 + 16 * 4 + 1), 0);
	int j = 0;
	char s[20];
	for (auto &&task : TaskSwitcher::taskList) {
		sprintf(s, "%5d %8d %4s ", task->level, task->priority, task->running ? "(oo)" : "(__)");
		str = s + task->name;
		sht->drawString(str, Point(2 + 1, 2 + 16 * 5 + j * 16 + 2), 0);
		++j;
	}
	sht->drawRect(Rectangle(2, 2 + 16 * 4, sht->frame.size.width - 1 - 1 - 2, 16 + j * 16 + 3), 0);
	sht->drawLine(Line(3, 2 + 16 * 5 + 1, sht->frame.size.width - 1 - 2, 2 + 16 * 5 + 1), 0);
	sht->drawLine(Line(3 + 5 * 8 + 3, 2 + 16 * 4 + 1, 3 + 5 * 8 + 3, 2 + 16 * 5 + j * 16 + 2), 0);
	sht->drawLine(Line(3 + 14 * 8 + 3, 2 + 16 * 4 + 1, 3 + 14 * 8 + 3, 2 + 16 * 5 + j * 16 + 2), 0);
	sht->drawLine(Line(3 + 19 * 8 + 3, 2 + 16 * 4 + 1, 3 + 19 * 8 + 3, 2 + 16 * 5 + j * 16 + 2), 0);
	
	// Refresh the screen
	sht->refresh(clearRange);
}

void SysinfoMain(Tab *tab) {
	Task *task = TaskSwitcher::getNowTask();
	int count = 0, count0 = 0;
	Sheet *sht = tab->sheet;
	
	// タイマー作成
	Timer *timer = new Timer(task->queue);
	// タブが閉じられた時消えるように
	tab->timer = timer;
	// タイマーセット (1s)
	timer->set(100);
	
	showSysInfo(sht, 0);
	
	for (;;) {
		++count;
		Cli();
		if (task->queue->isempty()) {
			//task->sleep(); ベンチマーク測定のため
			Sti();
		} else {
			int data = task->queue->pop();
			Sti();
			if (data == timer->data) {
				showSysInfo(sht, count - count0);
				count0 = count;
				timer->set(100);
			}
		}
	}
}
