#pragma once

template <typename T>
class List {
protected:
	int length = 0;
	class Node {
	public:
		T data;
		Node *prev = nullptr, *next = nullptr;
		
		Node() = default;
		explicit Node(T newData, Node *p = nullptr, Node *n = nullptr) : data(newData), prev(p), next(n) {}
	} *dummy, *head, *tail;
	
public:
	List() : dummy(new Node), head(dummy), tail(dummy) {}
	
	virtual ~List() {
		// head から tail まで順に解放
		for (Node *p = head; p; ) {
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
			if (node->next) {
				node = node->next;
			}
			return *this;
		}
		iterator &operator ++(int) {
			if (node->next) {
				node = node->next;
			}
			return *this;
		}
		iterator &operator --() {
			if (node->prev) {
				node = node->prev;
			}
			return *this;
		}
		iterator &operator --(int) {
			if (node->prev) {
				node = node->prev;
			}
			return *this;
		}
		bool operator==(const iterator &it) {
			return node == it.node;
		}
		bool operator!=(const iterator &it) {
			return node != it.node;
		}
		T &operator *() const {
			return node->data;
		}
	};
	struct const_iterator {
	private:
		Node *node;
	
	public:
		friend class List;
		const_iterator &operator ++() {
			if (node->next) {
				node = node->next;
			}
			return *this;
		}
		const_iterator &operator ++(int) {
			if (node->next) {
				node = node->next;
			}
			return *this;
		}
		const_iterator &operator --() {
			if (node->prev) {
				node = node->prev;
			}
			return *this;
		}
		const_iterator &operator --(int) {
			if (node->prev) {
				node = node->prev;
			}
			return *this;
		}
		bool operator==(const const_iterator &it) {
			return node == it.node;
		}
		bool operator!=(const const_iterator &it) {
			return node != it.node;
		}
		const T &operator *() const {
			return node->data;
		}
	};
	
	bool empty() const {
		return head == dummy;
	}
	
	int size() const {
		return length;
	}
	
	T &front() {
		return head->data;
	}
	
	T &back() {
		return tail->data;
	}
	
	const T &front() const {
		return head->data;
	}
	
	const T &back() const {
		return tail->data;
	}
	
	void push_front(const T &data) {
		// 先頭に入るべきノードを作成
		Node *node = new Node(data, nullptr, head);
		
		if (tail == dummy) {
			// 空なら
			tail = node;
		}
		
		head->prev = node;
		head = node;
		
		++length;
	}
	
	void push_back(const T &data) {
		// 最後に入るべきノードを作成
		Node *node = new Node(data, tail, dummy);
		
		if (head == dummy) {
			// 空なら
			head = node;
			node->prev = nullptr;
		} else {
			// 空じゃなかったら
			tail->next = node;
		}
		
		tail = node;
		dummy->prev = node;
		
		++length;
	}
	
	void pop_front() {
		// 空なら無視
		if (head == dummy) return;
		
		if (head == tail) {
			// 要素が1つ
			delete head;
			head = tail = dummy;
			dummy->prev = nullptr;
		} else {
			// 要素が2つ以上
			head = head->next;
			delete head->prev;
			head->prev = nullptr;
		}
		
		--length;
	}
	
	void pop_back() {
		// 空なら無視
		if (tail == dummy) return;
		
		if (head == tail) {
			// 要素が1つ
			delete tail;
			head = tail = dummy;
			dummy->prev = nullptr;
		} else {
			// 要素が2つ以上
			tail = tail->prev;
			delete tail->next;
			tail->next = dummy;
			dummy->prev = tail;
		}
		
		--length;
	}
	
	void insert(const_iterator pos, const T &data) {
		Node *p = new Node(data, pos.node->prev, pos.node);
		if (p->prev != nullptr) {
			p->prev->next = p;
		} else {
			head = p;
		}
		if (p->next != dummy) {
			p->next->prev = p;
		} else {
			tail = p;
			tail->next = dummy;
			dummy->prev = tail;
		}
		
		++length;
	}
	
	void erase(const_iterator pos) {
		Node *p = pos.node;
		if (p->prev != nullptr) {
			p->prev->next = p->next;
		} else {
			head = p->next;
			head->prev = nullptr;
		}
		if (p->next != dummy) {
			p->next->prev = p->prev;
		} else {
			tail = p->prev;
			tail->next = dummy;
			dummy->prev = tail;
		}
		delete p;
		--length;
	}
	
	void erase(const_iterator first, iterator last) {
		if (first.node->prev != nullptr) {
			first.node->prev->next = last.node;
		} else {
			head = last.node;
		}
		last.node->prev = first.node->prev;
		for (iterator it = first; it != last; ++it) {
			delete it.node;
			--length;
		}
	}
	
	void remove(const T &data) {
		for (iterator it = begin(); it != end(); ++it) {
			if (it.node->data == data) {
				// data を持つ node が見つかった
				if (it.node->prev != nullptr) {
					// node より前が存在する
					it.node->prev->next = it.node->next;
				} else {
					// node が先頭
					head = it.node->next;
					head->prev = nullptr;
				}
				if (it.node->next != dummy) {
					// node より後が存在する
					it.node->next->prev = it.node->prev;
				} else {
					// node が最後
					tail = it.node->prev;
					tail->next = dummy;
					dummy->prev = tail;
				}
				delete it.node;
				--length;
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
		it.node = dummy;
		return it;
	}
	
	const_iterator begin() const {
		const_iterator it;
		it.node = head;
		return it;
	}
	
	const_iterator end() const {
		const_iterator it;
		it.node = dummy;
		return it;
	}
};
