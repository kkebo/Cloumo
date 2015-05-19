/*
 * Stack
 */

#ifndef _STACK_H_
#define _STACK_H_

template <typename T>
class Stack {
protected:
	T *buf_;
	int tail_ = 0, size_;

public:
	Stack(int size) : buf_(new T[size]), size_(size) {}
	virtual ~Stack() {
		delete[] buf_;
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
