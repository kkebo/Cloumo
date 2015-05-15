#ifndef _FILE_H_
#define _FILE_H_

class File {
public: // private にしたい
	unsigned char *buf_;
	FileInfo *info_;
	int size_;

public:
	~File();
	unsigned char *read();
	int size();
};

#endif