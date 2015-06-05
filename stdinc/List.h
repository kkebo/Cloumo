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
	} *head = nullptr, *tail = nullptr;
	
public:
	const int &length = _length;

	List() = default;
	
	virtual ~List() {
		// head から tail まで順に解放
		for (Node *p = head; p != nullptr; ) {
			Node *q = p;
			p = p->next;
			delete q;
		}
	}
	
	bool isEmpty() const {
		return head == nullptr;
	}
	
	T &getFirst() const {
		return head->data;
	}
	
	T &getLast() const {
		return tail->data;
	}
	
	void append(const T &data) {
		Node *node = new Node(data, tail);
		if (head == nullptr) {
			head = node;
		}
		if (tail != nullptr) {
			tail->next = node;
		}
		tail = node;
		++_length;
	}
	
	struct Iterator {
	private:
		Node *node;
	
	public:
		friend class List;
		Iterator &operator ++() {
			if (node) {
				node = node->next;
			}
			return *this;
		}
		Iterator &operator ++(int) {
			if (node) {
				node = node->next;
			}
			return *this;
		}
		Iterator &operator --() {
			if (node) {
				node = node->prev;
			}
			return *this;
		}
		Iterator &operator --(int) {
			if (node) {
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
		it.node = head;
		return it;
	}
	
	Iterator end() {
		Iterator it;
		it.node = nullptr;
		return it;
	}
};
