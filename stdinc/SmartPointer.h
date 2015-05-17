#ifndef _SMARTPOINTER_H_
#define _SMARTPOINTER_H_

class ReferenceCounter {
private:
	int count = 0;
	
public:
	void Add();
	int Release();
	int Count();
};

template <typename T>
class unique_ptr {
private:
	T *pointer;

private:
	void operator = (const unique_ptr<T> &) = delete;

public:
	unique_ptr(T *p = nullptr) : pointer(p) {}
	virtual ~unique_ptr() {
		delete pointer;
	}
	
	T *get() {
		return pointer;
	}
	void reset(T *p) {
		delete pointer;
		
		pointer = p;
	}
	T *release() {
		T *p = pointer;
		pointer = nullptr;
		return p;
	}
	
	T *operator -> () const {
		return pointer;
	}
	T *operator & () {
		return &pointer;
	}
	T &operator * () const {
		return *pointer;
	}
	operator T* () const { // cast
		return pointer;
	}
	operator bool() const {
		return pointer != nullptr;
	}
};

template <typename T>
class shared_ptr {
private:
	T *pointer;
	ReferenceCounter *reference;
	
public:
	shared_ptr(T *p = nullptr) : pointer(p) {
		reference = new ReferenceCounter();
		reference->Add();
	}
	shared_ptr(const shared_ptr<T> &p) {
		if (p.reference) {
			pointer = p.pointer;
			reference = p.reference;
			reference->Add();
		}
	}
	virtual ~shared_ptr() {
		if (reference && reference->Release() == 0) {
			delete pointer;
			delete reference;
		}
	}
	
	T *get() {
		return pointer;
	}
	void reset(T *p) {
		if (reference && reference->Release() == 0) {
			delete pointer;
			delete reference;
		}
		
		pointer = p;
		reference = new ReferenceCounter();
		reference->Add();
	}
	ReferenceCounter *getRC() {
		return reference;
	}
	int use_count() {
		return reference->Count();
	}
	bool unique() {
		return reference->Count() == 1;
	}
	
	shared_ptr<T> &operator = (const shared_ptr<T> &p) {
		if (this != &p) {
			if (reference && reference->Release() == 0) {
				delete pointer;
				delete reference;
			}
			
			pointer = p.pointer;
			reference = p.reference;
			reference->Add();
		}
	}
	T *operator -> () const {
		return pointer;
	}
	T *operator & () {
		return &pointer;
	}
	T &operator * () const {
		return *pointer;
	}
	operator T* () const { // cast
		return pointer;
	}
	operator bool() const {
		return pointer != nullptr;
	}
};

#endif