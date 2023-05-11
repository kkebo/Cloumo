/*
 * タイマー
 */

#pragma once

const int PIT_CTRL = 0x0043;
const int PIT_CNT0 = 0x0040;

class Timer {
private:
	int _data;
	Timer *next = nullptr;
	unsigned int timeout;
	bool running = false;
	TaskQueue *_queue;

public:
	TaskQueue *const &queue = _queue;
	const int &data = _data;

	friend class TimerController;
	friend void IntHandler20(int *esp);
	Timer(TaskQueue *queue_);
	Timer(TaskQueue *queue_, int data_);
	~Timer();
	void set(unsigned int newTimeout);
	bool cancel();
};

class TimerController {
private:
	static unsigned int count;
	static unsigned int next;
	static Timer *t0;

public:
	friend class Timer;
	friend void IntHandler20(int *esp);
	static void init();
	static void remove(const Task &task);
	//static void reset();
};
