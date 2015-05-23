/*
 * タイマー
 */

#ifndef _TIMER_H_
#define _TIMER_H_

const int PIT_CTRL = 0x0043;
const int PIT_CNT0 = 0x0040;
const int MAX_TIMER = 500;

enum class TimerFlag { Free, Reserved, Running };

class Timer {
private:
	int data = -1;
	
	Timer() : flags(TimerFlag::Free) {}

public:
	Timer *next;
	unsigned int timeout;
	TimerFlag flags;
	TaskQueue *queue;

public:
	friend class TimerController;
	friend Task *TaskController::init();
	Timer(TaskQueue *queue_);
	Timer(TaskQueue *queue_, int data_);
	~Timer();
	static void *operator new(size_t);
	static void operator delete(void *) {}
	void set(unsigned int newTimeout);
	bool cancel();
	int getData();
};

class TimerController {
public:
	static Timer timers0[];
	static unsigned int count;
	static unsigned int next;
	static Timer *t0;

public:
	static void init();
	//static void reset();
};

#endif
