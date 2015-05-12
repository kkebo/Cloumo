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
	ReferenceCounter *reference;

private:
	void operator = (const unique_ptr<T> &) {}

public:
	unique_ptr(T *p = nullptr) : pointer(p) {
		reference = new ReferenceCounter();
		reference->Add();
	}
	virtual ~unique_ptr() {
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
	T *release() {
		T *p = pointer;
		pointer = nullptr;
		delete reference;
		reference = nullptr;
		return p;
	}
	ReferenceCounter &getRC() {
		return reference;
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
		if (p.getRC()) {
			pointer = p.get();
			reference = p.getRC();
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
	ReferenceCounter &getRC() {
		return reference;
	}
	int use_count() {
		return reference->Count();
	}
	
	shared_ptr<T> &operator = (const shared_ptr<T> &p) {
		if (this != &p && p.getRC()) {
			if (reference && reference->Release() == 0) {
				delete pointer;
				delete reference;
			}
			
			pointer = p.get();
			reference = p.getRC();
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
};

#endif