/*
 * Queue
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "../kernel/memory.h"

template <typename T>
class Queue {
protected:
	T *buf_;
	int head_ = 0, tail_ = 0, size_;

public:
	Queue(int size) : buf_(new T[size + 1]), size_(size) {}
	virtual ~Queue() {
		delete[] buf_;
	}
	bool push(const T &data) {
		if ((tail_ + 1) % size_ == head_) { // queue is full
			return false;
		}
		buf_[tail_] = data;
		tail_ = (tail_ + 1) % size_;
		return true;
	}
	T pop() {
		T data = buf_[head_];
		head_ = (head_ + 1) % size_;
		return data;
	}
	bool isempty() const {
		return head_ == tail_;
	}
};

#endif
