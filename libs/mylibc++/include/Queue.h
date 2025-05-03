/*
 * Queue
 */

#pragma once

#include <cloumo/kernel/memory.h>

template <typename T>
class Queue {
protected:
	T *buf;
	int head = 0, tail = 0, size;

public:
	explicit Queue(int size_) : buf(new T[size_ + 1]), size(size_) {}
	virtual ~Queue() {
		delete[] buf;
	}
	bool push(const T &data) {
		if ((tail + 1) % size == head) { // queue is full
			return false;
		}
		buf[tail] = data;
		tail = (tail + 1) % size;
		return true;
	}
	T pop() {
		T data = buf[head];
		head = (head + 1) % size;
		return data;
	}
	bool isempty() const {
		return head == tail;
	}
};
