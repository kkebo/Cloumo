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
	
	struct iterator {
	private:
		Node *node;
	
	public:
		friend class List;
		iterator &operator ++() {
			if (node) {
				node = node->next;
			}
			return *this;
		}
		iterator &operator ++(int) {
			if (node) {
				node = node->next;
			}
			return *this;
		}
		iterator &operator --() {
			if (node) {
				node = node->prev;
			}
			return *this;
		}
		iterator &operator --(int) {
			if (node) {
				node = node->prev;
			}
			return *this;
		}
		bool operator!=(const iterator &it) {
			return node != it.node;
		}
		T &operator *() {
			return node->data;
		}
	};
	struct const_iterator {
	private:
		const Node *node;
	
	public:
		friend class List;
		const_iterator &operator ++() {
			if (node) {
				node = node->next;
			}
			return *this;
		}
		const_iterator &operator ++(int) {
			if (node) {
				node = node->next;
			}
			return *this;
		}
		const_iterator &operator --() {
			if (node) {
				node = node->prev;
			}
			return *this;
		}
		const_iterator &operator --(int) {
			if (node) {
				node = node->prev;
			}
			return *this;
		}
		bool operator!=(const const_iterator &it) {
			return node != it.node;
		}
		const T &operator *() {
			return node->data;
		}
	};
	
	bool empty() const {
		return head == nullptr;
	}
	
	T &front() {
		return head->data;
	}
	
	T &back() {
		return tail->data;
	}
	
	void push_front(const T &data) {
		Node *node = new Node(data, nullptr, head);
		if (tail == nullptr) {
			tail = node;
		}
		if (head != nullptr) {
			head->prev = node;
		}
		head = node;
		++_length;
	}
	
	void pop_front() {
		head = head->next;
		delete head->prev;
		head->prev = nullptr;
	}
	
	void push_back(const T &data) {
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
	
	void pop_back() {
		tail = tail->prev;
		delete tail->next;
		tail->next = nullptr;
	}
	
	void insert(const_iterator pos, const T &data) {
		Node *p = new Node(data, pos.node->prev, pos.node);
		if (p->prev != nullptr) {
			p->prev->next = p;
		} else {
			head = p;
		}
		if (p->next != nullptr) {
			p->next->prev = p;
		} else {
			tail = p;
		}
	}
	
	void erase(const_iterator pos) {
		Node *p = const_cast<Node *>(pos.node);
		if (p->prev != nullptr) {
			p->prev->next = p->next;
		} else {
			head = p->next;
		}
		if (p->next != nullptr) {
			p->next->prev = p->prev;
		} else {
			tail = p->prev;
		}
		delete p;
	}
	
	void erase(const_iterator first, const_iterator last) {
		if (first.node->prev != nullptr) {
			first.node->prev->next = last.node->next;
		} else {
			head = last.node->next;
		}
		if (last.node->next != nullptr) {
			last.node->next->prev = first.node->prev;
		} else {
			tail = first.node->prev;
		}
		for (const_iterator it = first; it != last; ++it) {
			delete const_cast<Node *>(it.node);
		}
	}
	
	void remove(const T &data) {
		for (iterator it = begin(); it != end(); ++it) {
			if (it.node->data == data) {
				if (it.node->prev != nullptr) {
					it.node->prev->next = it.node->next;
				} else {
					head = it.node->next;
				}
				if (it.node->next != nullptr) {
					it.node->next->prev = it.node->prev;
				} else {
					tail = it.node->prev;
				}
				delete it.node;
			}
		}
	}
	
	iterator begin() {
		iterator it;
		it.node = head;
		return it;
	}
	
	iterator end() {
		iterator it;
		it.node = nullptr;
		return it;
	}
	
	const_iterator begin() const {
		const_iterator it;
		it.node = head;
		return it;
	}
	
	const_iterator end() const {
		const_iterator it;
		it.node = nullptr;
		return it;
	}
};
