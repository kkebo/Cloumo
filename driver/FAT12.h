/*
 * ファイル
 */

#ifndef _FAT12_H_
#define _FAT12_H_

class File;

struct FileInfo {
	char name[8];
	unsigned char ext[3];
	unsigned char type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};

class FAT12 {
private:
	static int *fat_;

	static void loadFile(int, int, char *, char *);
	static unsigned char *loadFile2(int, int &);
	static FileInfo *search(const char *, FileInfo *, int);

public:
	static void init();
	static File *open(const char *);
};

#endif
