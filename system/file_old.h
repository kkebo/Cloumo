/*
 * ファイル
 */

#ifndef _FILE_H_
#define _FILE_H_

struct FileInfo {
	char name[8];
	unsigned char ext[3];
	unsigned char type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};

class File {
private:
	FileInfo* info_;
	unsigned char* buf_;

	void load(char*, const char*);
	
public:
	File();
	File(const char*, FileInfo*, const int);
	~File();
	bool open(const char*, FileInfo*, const int);
	unsigned char* get();
	unsigned int getSize();
};

class FAT12 {
public:
	static int* fat_;

public:
	static void init();
};

#endif
