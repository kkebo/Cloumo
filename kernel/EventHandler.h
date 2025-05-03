#pragma once

using MouseEventHandler = void (*)(MouseEvent&);

struct MouseEvent {
public:
	Sheet *sheet;
	Task *task;
	
	MouseEvent(Sheet *sht, Task *t);
};

struct TaskEvent {
public:
	Task *task;
	
	TaskEvent(Task *t);
};