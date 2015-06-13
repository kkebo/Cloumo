#pragma once

#include <pistring.h>

class Tab {
private:
	Task *task = nullptr;
	int index;
	Sheet *tabBar;
	
	Tab(const string &tabName);

public:
	Sheet *sheet;
	Timer *timer = nullptr;
	string name;
	
	Tab(const string &tabName, void (*mainLoop)(Tab *));
	Tab(const string &tabName, int queueSize, void (*mainLoop)(Tab *));
	~Tab();
	void active();
};
