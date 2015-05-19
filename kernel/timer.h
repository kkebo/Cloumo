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
	int data_ = -1;

public:
	Timer *next_;
	unsigned int timeout_;
	TimerFlag flags_;
	TaskQueue *queue_;

public:
	Timer() = default;
	Timer(TaskQueue *);
	Timer(TaskQueue *, int);
	~Timer();
	static void *operator new(size_t size);
	static void operator delete(void *p) {}
	void set(unsigned int);
	bool cancel();
	int data();
};

class TimerController {
public:
	static Timer *timers0_;
	static unsigned int count_;
	static unsigned int next_;
	static Timer *t0_;

public:
	static void init();
	static void reset();
};

#endif
