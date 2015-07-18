#pragma once

#include <pistring.h>

class Tab {
private:
	Task *_task = nullptr;
	int index;
	Sheet *tabBar;
	
	Tab(const string &tabName);

public:
	Sheet *sheet;
	string name;
	Task *const &task = _task;
	
	Tab(const string &tabName, void (*mainLoop)(Tab *));
	Tab(const string &tabName, int queueSize, void (*mainLoop)(Tab *));
	~Tab();
	void active();
};
