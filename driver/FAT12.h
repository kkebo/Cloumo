/*
 * FAT12
 */

#pragma once

class FAT12 {
public:
	struct FileInfo {
		char name[8];
		unsigned char ext[3];
		unsigned char type;
		char reserve[10];
		unsigned short time, date, clustno;
		unsigned int size;
	};

private:
	static int fat[];

public:
	static void init();
	static FileInfo *search(const char *name, FileInfo *finfo, int max);
	static void loadFile(int clustno, int size, char *buf, char *img);
	static unsigned char *loadFile2(int clustno, int &psize);
};
