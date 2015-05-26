#pragma once

template <typename T>
class List {
protected:
	class Node {
	public:
		T data;
		Node *prev = nullptr, *next = nullptr;
		
		Node() = default;
		Node(T newData, Node *p = nullptr, Node *n = nullptr) : data(newData), prev(p), next(n) {}
	} *dummy;
	// dummy の prev が head で，next が tail
	
public:
	List() : dummy(new Node()) {}
	
	virtual ~List() {
		// dummy から順に解放
		for (Node *p = dummy; p != nullptr; ) {
			Node *q = p;
			p = p->next;
			delete q;
		}
	}
	
	bool isEmpty() const {
		return dummy->prev == nullptr;
	}
	
	T &getFirst() const {
		return dummy->prev->data;
	}
	
	T &getLast() const {
		return dummy->next->data;
	}
	
	void append(const T &data) {
		Node *node = new Node(data, dummy->next);
		if (!dummy->prev) {
			// 空だったとき head も tail も node
			dummy->prev = dummy->next = node;
		} else {
			// tail の次を node にして tail も node
			dummy->next = dummy->next->next = node;
		}
	}
};
