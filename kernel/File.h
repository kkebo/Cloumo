#ifndef _FILE_H_
#define _FILE_H_

#include <SmartPointer.h>
#include <pistring.h>

class File {
private:
	string _name;
	shared_ptr<unsigned char> buf;
	int _size;

public:
	const string &name = _name;
	const int &size = _size;
	
	File() = default;
	File(const string &name_) : _name(name_) {}
	bool open();
	bool open(const string &fileName);
	inline const shared_ptr<unsigned char> &File::read() {
		return buf;
	}
};

#endif