#pragma once

template <typename T>
class List {
protected:
	int _length = 0;
	class Node {
	public:
		T data;
		Node *prev = nullptr, *next = nullptr;
		
		Node() = default;
		Node(T newData, Node *p = nullptr, Node *n = nullptr) : data(newData), prev(p), next(n) {}
	} *dummy, *dummyTail;
	// dummy の prev が head で，next が tail
	
public:
	const int &length = _length;

	List() : dummy(new Node), dummyTail(new Node) {
		dummy->prev = dummy->next = dummyTail;
		dummyTail->prev = dummy;
	}
	
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
		if (dummy->prev == dummyTail) {
			// 空だったとき head も tail も node
			dummy->prev = dummy->next = node;
			node->next = dummyTail;
			dummyTail->prev = node;
		} else {
			// tail の次を node にして tail も node
			dummy->next = dummy->next->next = node;
			node->next = dummyTail;
			dummyTail->prev = node;
		}
		++_length;
	}
	
	struct Iterator {
	private:
		Node *node;
	
	public:
		friend class List;
		Iterator &operator ++() {
			if (node->next) {
				node = node->next;
			}
			return *this;
		}
		Iterator &operator ++(int) {
			if (node->next) {
				node = node->next;
			}
			return *this;
		}
		Iterator &operator --() {
			if (node->prev) {
				node = node->prev;
			}
			return *this;
		}
		Iterator &operator --(int) {
			if (node->prev) {
				node = node->prev;
			}
			return *this;
		}
		bool operator!=(const Iterator &it) {
			return node != it.node;
		}
		T &operator *() {
			return node->data;
		}
	};
	
	Iterator begin() {
		Iterator it;
		it.node = dummy->prev;
		return it;
	}
	
	Iterator end() {
		Iterator it;
		it.node = dummyTail;
		return it;
	}
};
