/*
 * Queue
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

const int FLAGS_OVERRUN = 0x0001;

template <typename T>
class Queue {
protected:
	T *buf_;
	int head_, tail_, size_, free_;
	
	Queue() {}

public:
	Queue(int size) : buf_(new T[size]), head_(0), tail_(0), size_(size), free_(size) {}
	virtual ~Queue() {
		delete buf_;
	}
	bool push(const T &data) {
		if (!free_) { // queue is full
			return false;
		}
		buf_[tail_++] = data;
		if (tail_ == size_) {
			tail_ = 0;
		}
		free_--;
		return true;
	}
	T pop() {
		T data = buf_[head_++];
		if (head_ == size_) {
			head_ = 0;
		}
		free_++;
		return data;
	}
	bool isempty() const {
		return size_ == free_;
	}
};

#endif
