/*
 * Queue
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

const int FLAGS_OVERRUN = 0x0001;

class Task;

template <typename T>
class Queue {
private:
	T *buf_;
	int head_, tail_, size_, free_, flags_;

public:
	Task *task_;

public:
	Queue(int size, Task *task = nullptr) : buf_(new T[size]), head_(0), tail_(0), size_(size), free_(size), flags_(0), task_(task) {}
	~Queue() {
		delete buf_;
	}
	bool push(T data) {
		if (!free_) { // queue is full
			flags_ |= FLAGS_OVERRUN;
			return false;
		}
		buf_[tail_] = data;
		tail_++;
		if (tail_ == size_) {
			tail_ = 0;
		}
		free_--;
		if (task_ && task_->flags_ != TaskFlag::Running) {
			task_->run(-1, 0);
		}
		return true;
	}
	T pop() {
		T data;
		if (isempty()) { // queue is empty
			return -1;
		}
		data = buf_[head_];
		head_++;
		if (head_ == size_) {
			head_ = 0;
		}
		free_++;
		return data;
	}
	bool isempty() {
		return size_ == free_;
	}
};

#endif
