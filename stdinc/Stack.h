/*
 * Stack
 */

#ifndef _STACK_H_
#define _STACK_H_

const int FLAGS_OVERRUN = 0x0001;

template <typename T>
class Stack {
protected:
	T *buf_;
	int tail_, size_, free_;
	
	Queue() {}

public:
	Queue(int size) : buf_(new T[size]), tail_(0), size_(size) {}
	virtual ~Queue() {
		delete buf_;
	}
	bool push(const T &data) {
		if (tail_ == size_) { // stack is full
			return false;
		}
		buf_[tail_++] = data;
		return true;
	}
	T pop() {
		return buf_[--tail_];
	}
	bool isempty() const {
		return tail_ == 0;
	}
};

#endif
