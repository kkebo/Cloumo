/*
 * Queue
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

const int FLAGS_OVERRUN = 0x0001;

class Task;

class Queue {
private:
	int *buf_;
	int head_, tail_, size_, free_, flags_;
	Task *task_;

public:
	Queue() {}; // とりあえずつけている状態．すぐに再考すること．
	Queue(int, Task * = nullptr);
	~Queue();
	bool push(int);
	int pop();
	bool isempty();
};

#endif
