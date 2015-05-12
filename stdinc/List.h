#ifndef _LIST_H_
#define _LIST_H_

template <typename T>
class List {
protected:
	class Node {
	public:
		T data;
		Node *prev, *next;
	
		Node(T newData, Node *p = nullptr, Node *n = nullptr) : data(newData), prev(p), next(n) {}
	} *dummy;
	// dummy の prev が head で，next が tail
	
public:
	List() {
		dummy = new Node(T(), nullptr, nullptr);
	}
	
	virtual ~List() {
		// head から順に解放
		for (Node *p = dummy; p != nullptr; ) {
			Node *q = p;
			p = p->next;
			delete q;
		}
	}
	
	bool isEmpty() {
		return dummy->prev == nullptr;
	}
	
	T &getFirst() {
		return dummy->prev->data;
	}
	
	T &getLast() {
		return dummy->next->data;
	}
	
	void append(T data) {
		Node *node = new Node(data);
		if (!dummy->prev) {
			// 空だったとき
			dummy->prev = dummy->next = node;
		} else {
			dummy->next->next = node;
		}
	}
};

#endif