#include "../headers.h"

Tab::Tab(const string &tabName) : index(SheetCtl::numOfTab++), tabBar(new Sheet(Size(150 - 2, 22), true)), sheet(new Sheet(Size(SheetCtl::resolution.width - 150, SheetCtl::resolution.height), false)), name(tabName) {
	// タブ一覧に登録
	SheetCtl::tabs[index] = this;
	
	// 基本デザイン
	sheet->fillRect(sheet->frame, 0xffffff);
	sheet->drawRect(sheet->frame, 0);
	sheet->moveTo(Point(150, 0));
	SheetCtl::back->appendChild(sheet);
	
	// タブバー初期化の続き
	tabBar->tab = this;
	tabBar->onClick = [](const Point &pos, Sheet &sht) {
		if (2 <= pos.x && pos.x <= 2 + 22) {
			// 閉じる
			delete sht.tab;
		} else if (sht.tab->index != SheetCtl::activeTab && 2 <= pos.x) {
			// アクティブ化
			sht.tab->active();
		}
	};
	Rectangle newTabRange(2, 35 + 23 * index, 150 - 2, 22);
	tabBar->fillRect(tabBar->frame, kActiveTabColor);
	tabBar->borderRadius(true, false, true, false);
	tabBar->drawLine(Line(7, 7, 15, 15), kActiveTextColor);
	tabBar->drawLine(Line(7, 15, 15, 7), kActiveTextColor);
	tabBar->drawString(string(tabName, 0, 15), Point(4 + 22, 4), kActiveTextColor);
	tabBar->moveTo(Point(2, 35 + 23 * index));
	SheetCtl::back->appendChild(tabBar);
	tabBar->upDown(1);
	
	// タブ切り替え
	active();
}

Tab::Tab(const string &tabName, void (*mainLoop)(Tab *)) : Tab(tabName) {
	int args[] = { (int)this };
	_task = new Task(name, 3, 2, mainLoop, args);
}
Tab::Tab(const string &tabName, int queueSize, void (*mainLoop)(Tab *)) : Tab(tabName) {
	int args[] = { (int)this };
	_task = new Task(name, 3, 2, queueSize, mainLoop, args);
}

Tab::~Tab() {
	// アクティブタブの調整
	if (SheetCtl::activeTab == index) {
		if (SheetCtl::numOfTab > 1) {
			// アクティブタブでかつ他のタブがあれば，他のタブへ切り替える
			if (SheetCtl::numOfTab - 1 == index) {
				SheetCtl::tabs[index - 1]->active();
			} else {
				// 次のタブを選ばせる
				SheetCtl::tabs[(SheetCtl::activeTab + 1) % SheetCtl::numOfTab]->active();
			}
		} else {
			// 最後の1つのタブなら，SheetCtl::activeTab を -1 へ
			SheetCtl::activeTab = -1;
		}
	}
	if (SheetCtl::activeTab > index) {
		--SheetCtl::activeTab;
	}
	
	// タブを閉じる
	delete tabBar;
	
	// タブをずらす
	for (int i = index; i < SheetCtl::numOfTab - 1; ++i) {
		SheetCtl::tabs[i] = SheetCtl::tabs[i + 1];
		--SheetCtl::tabs[i]->index;
		SheetCtl::tabs[i]->tabBar->moveTo(Point(SheetCtl::tabs[i]->tabBar->frame.offset.x, SheetCtl::tabs[i]->tabBar->frame.offset.y - 23));
	}
	
	// タブの個数を減らす
	--SheetCtl::numOfTab;
	
	// その他の解放
	delete sheet;
	if (_task) delete _task;
}

void Tab::active() {
	if (SheetCtl::activeTab == index) return;
	
	Rectangle tabRect(0, 0, tabBar->frame.size.width, tabBar->frame.size.height);
	
	// 新しくアクティブになるタブ
	tabBar->changeColor(tabRect, kPassiveTabColor, kActiveTabColor);
	tabBar->changeColor(tabRect, kPassiveTextColor, kActiveTextColor);
	tabBar->refresh(tabRect);
	sheet->upDown(1);
	
	// アクティブだったタブ
	if (SheetCtl::activeTab >= 0) {
		SheetCtl::tabs[SheetCtl::activeTab]->tabBar->changeColor(tabRect, kActiveTabColor, kPassiveTabColor);
		SheetCtl::tabs[SheetCtl::activeTab]->tabBar->changeColor(tabRect, kActiveTextColor, kPassiveTextColor);
		SheetCtl::tabs[SheetCtl::activeTab]->tabBar->refresh(tabRect);
		SheetCtl::tabs[SheetCtl::activeTab]->sheet->upDown(-1);
	}
	
	SheetCtl::activeTab = index;
}
