/*
 * タイマー
 */

#ifndef _TIMER_H_
#define _TIMER_H_

const int PIT_CTRL = 0x0043;
const int PIT_CNT0 = 0x0040;
const int MAX_TIMER = 500;

enum TimerFlag { TIMERFLAG_FREE, TIMERFLAG_ALLOCATED, TIMERFLAG_INUSE };

class Timer {
private:
	int data_;

public:
	Timer *next_;
	unsigned int timeout_;
	TimerFlag flags_;
	Queue *queue_;

public:
	void init(Queue *);
	void init(Queue *, int);
	void free();
	void set(unsigned int);
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
	static Timer *alloc();
	static void reset();
};

#endif
