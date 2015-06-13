#include "../headers.h"

Tab::Tab(const string &tabName) : index(SheetCtl::numOfTab++), sheet(new Sheet(Size(SheetCtl::resolution.width - 150, SheetCtl::resolution.height), false)), name(tabName) {
	// タブ一覧に登録
	SheetCtl::tabs[index] = this;
	
	// 基本デザイン
	sheet->fillRect(sheet->frame, 0xffffff);
	sheet->drawRect(sheet->frame, 0);
	sheet->moveTo(Point(150, 0));
	
	// タブ作成
	Rectangle newTabRange(2, 35 + 23 * index, 150 - 2, 22);
	SheetCtl::back->drawLine(Line(9, 7 + 35 + 23 * index, 17, 15 + 35 + 23 * index), kActiveTextColor);
	SheetCtl::back->drawLine(Line(9, 15 + 35 + 23 * index, 17, 7 + 35 + 23 * index), kActiveTextColor);
	SheetCtl::back->drawString(tabName, Point(6 + 22, 39 + 23 * index), kActiveTextColor);
	SheetCtl::back->changeColor(newTabRange, kBackgroundColor, kActiveTabColor);
	SheetCtl::back->refresh(newTabRange);
	
	// タブ切り替え
	active();
}

Tab::Tab(const string &tabName, void (*mainLoop)(Tab *)) : Tab(tabName) {
	int args[] = { (int)this };
	task = new Task(name, 3, 2, mainLoop, args);
}
Tab::Tab(const string &tabName, int queueSize, void (*mainLoop)(Tab *)) : Tab(tabName) {
	int args[] = { (int)this };
	task = new Task(name, 3, 2, queueSize, mainLoop, args);
}

Tab::~Tab() {
	if (SheetCtl::activeTab == index) {
		if (SheetCtl::numOfTab > 1) {
			// アクティブタブでかつ他のタブがあれば，他のタブへ切り替える
			if (SheetCtl::numOfTab - 1 == index) {
				SheetCtl::tabs[index - 1]->active();
			} else {
				// 次のタブを選ばせる
				int newIndex = (SheetCtl::activeTab + 1) % SheetCtl::numOfTab;
				SheetCtl::tabs[newIndex]->active();
			}
		} else {
			// 最後の1つのタブなら，SheetCtl::activeTab を -1 へ
			SheetCtl::activeTab = -1;
		}
	}
	
	if (SheetCtl::activeTab > index) {
		--SheetCtl::activeTab;
	}
	
	for (int i = index; i < SheetCtl::numOfTab - 1; ++i) {
		// tabs をずらす
		SheetCtl::tabs[i] = SheetCtl::tabs[i + 1];
		--SheetCtl::tabs[i]->index;
		
		// タブバーをずらす
		for (int y = 35 + 23 * i; y < 35 + 23 * i + 22; ++y) {
			for (int x = 2; x < 150; ++x) {
				SheetCtl::back->buf[y * SheetCtl::back->frame.size.width + x] = SheetCtl::back->buf[(y + 23) * SheetCtl::back->frame.size.width + x];
			}
		}
	}
	
	// 最後の1つタブバーを消す
	SheetCtl::back->fillRect(Rectangle(2, 35 + 23 * (SheetCtl::numOfTab - 1), 150 - 2, 22), kBackgroundColor);
	
	// リフレッシュ
	SheetCtl::back->refresh(Rectangle(2, 35 + 23 * index, 150 - 2, 23 * (SheetCtl::numOfTab - index + 1) - 1));
	
	// タブの個数を減らす
	--SheetCtl::numOfTab;
	
	// 解放
	delete sheet;
	if (timer) delete timer;
	if (task) delete task;
}

void Tab::active() {
	if (SheetCtl::activeTab == index) return;
	
	// 新しくアクティブになるタブ
	Rectangle nextTabRange(2, 35 + 23 * index, 150 - 2, 22);
	SheetCtl::back->changeColor(nextTabRange, kPassiveTabColor, kActiveTabColor);
	SheetCtl::back->changeColor(nextTabRange, kPassiveTextColor, kActiveTextColor);
	SheetCtl::back->refresh(nextTabRange);
	SheetCtl::tabs[index]->sheet->upDown(SheetCtl::top);
	
	// アクティブだったタブ
	if (SheetCtl::activeTab >= 0) {
		Rectangle prevTabRange(2, 35 + 23 * SheetCtl::activeTab, 150 - 2, 22);
		SheetCtl::back->changeColor(prevTabRange, kActiveTabColor, kPassiveTabColor);
		SheetCtl::back->changeColor(prevTabRange, kActiveTextColor, kPassiveTextColor);
		SheetCtl::back->refresh(prevTabRange);
		SheetCtl::tabs[SheetCtl::activeTab]->sheet->upDown(-1);
	}
	
	SheetCtl::activeTab = index;
}
