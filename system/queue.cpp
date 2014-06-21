#include "headers.h"

Queue::Queue(int size, Task* task) : buf_(new int[size]), head_(0), tail_(0), size_(size), free_(size), flags_(0), task_(task) {}

Queue::~Queue() {
	delete buf_;
}

// キューへデータを送り込んで蓄える
bool Queue::push(int data) {
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
	if (task_ && task_->flags_ != 2) {
		task_->run(-1, 0);
	}
	return true;
}

// キューからデータを１つとってくる
int Queue::pop() {
	int data;
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

// 空かどうか
bool Queue::isempty() {
	return size_ == free_;
}
