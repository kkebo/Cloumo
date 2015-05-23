#ifndef _FILE_H_
#define _FILE_H_

#include <SmartPointer.h>
#include <pistring.h>

class File {
private:
	string name;
	shared_ptr<unsigned char> buf;
	int size;

public:
	File() = default;
	File(const string &name_) : name(name_) {}
	bool open();
	bool open(const string &fileName);
	inline const shared_ptr<unsigned char> &File::read() {
		return buf;
	}
	inline const string &getName() {
		return name;
	}
	inline int File::getSize() {
		return size;
	}
};

#endif