#include "../headers.h"

File::~File() {
	delete buf_;
}

unsigned char *File::read() {
	return buf_;
}

int File::size() {
	return size_;
}
