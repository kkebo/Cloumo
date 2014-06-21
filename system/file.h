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
public: // private に
	unsigned char* buf_;
	FileInfo* info_;
	int size_;

public:
	~File();
	unsigned char* read();
	int size();
};

class FAT12 {
private:
	static int* fat_;

	static void loadFile(int, int, char*, char*);
	static unsigned char* loadFile2(int, int*);
	static FileInfo* search(const char*, FileInfo*, int);

public:
	static void init();
	static File* open(const char*);
};

#endif
