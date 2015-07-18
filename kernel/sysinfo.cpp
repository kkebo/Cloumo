#include <stdio.h>
#include <pistring.h>
#include "../headers.h"

void showSysInfo(Sheet *sht) {
	string str;
	auto memTotal = MemoryTotal();
	
	// Clear the screen
	Rectangle clearRange(Point(2, 18), Size(sht->frame.size.width - 3, sht->frame.size.height - 3 - 16));
	sht->fillRect(clearRange, 0xffffff);
	
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
	for (auto &&task : *TaskSwitcher::taskList) {
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
	Sheet *sht = tab->sheet;
	
	// ベンチマーク用タイマー
	static Timer *benchTimer;
	if (benchTimer) delete benchTimer;
	benchTimer = new Timer(task->queue, 1); // about:sysinfo を複数開いた時に問題が発生
	
	// Benchmark button
	Sheet *button = new Sheet(Size(200, 18), true);
	button->fillRect(Rectangle(button->frame), 0xaaaaaa);
	button->drawString("ベンチマーク測定開始", Point(20, 1), 0);
	button->borderRadius(true, true, true, true);
	button->moveTo(Point(1, 1));
	sht->appendChild(button, true);
	button->onClick = [](const Point&, Sheet &sht) {
		// 10秒後にベンチマーク結果表示を要求
		benchTimer->set(1000);
	};
	
	// 毎秒システム情報更新用のタイマー作成
	Timer *timer = new Timer(task->queue, 0);
	// タイマーセット (1s)
	timer->set(100);
	
	showSysInfo(sht);
	
	for (int count = 0;; ++count) {
		Cli();
		if (task->queue->isempty()) {
			Sti();
		} else {
			int data = task->queue->pop();
			Sti();
			switch (data) {
				case 0: // 毎秒再描画
					showSysInfo(sht);
					timer->set(100);
					break;
				
				case 1: { // ベンチマーク測定結果表示
					sht->fillRect(Rectangle(Point(202, 2), Size(sht->frame.size.width - 202, 16)), 0xffffff);
					sht->drawString("Benchmark Score:", Point(202, 2), 0);
					sht->drawString(to_string(count), Point(202 + 8 * 17, 2), 0);
					sht->refresh(Rectangle(Point(202, 2), Size(sht->frame.size.width - 202, 16)));
					break;
				}
			}
		}
	}
}
