/*
 * Stack
 */

#pragma once

template <typename T>
class Stack {
protected:
	T *buf;
	int tail = 0, size;

public:
	explicit Stack(int size_) : buf(new T[size_]), size(size_) {}
	virtual ~Stack() {
		delete buf;
	}
	bool push(const T &data) {
		if (tail == size) { // stack is full
			return false;
		}
		buf[tail++] = data;
		return true;
	}
	T pop() {
		return buf[--tail];
	}
	T &top() {
		return buf[tail - 1];
	}
	bool isempty() const {
		return tail == 0;
	}
};
