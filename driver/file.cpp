#include "../headers.h"

File::~File() {
	if (buf_) delete buf_;
}

unsigned char *File::read() {
	return buf_;
}

int File::size() {
	return size_;
}

int *FAT12::fat_ = nullptr;

void FAT12::init() {
	unsigned char *img = (unsigned char*)(ADDRESS_DISK_IMAGE + 0x000200);

	fat_ = new int[2880];
	for (int i = 0, j = 0; i < 2880; i += 2, j += 3) {
		fat_[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat_[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
	}
}

File *FAT12::open(const char *name) {
	File *file = new File;
	file->info_ = FAT12::search(name, (FileInfo*)(ADDRESS_DISK_IMAGE + 0x002600), 224);
	if (!file->info_) return 0;
	file->size_ = file->info_->size;
	file->buf_ = (unsigned char*)FAT12::loadFile2(file->info_->clustno, &file->size_);
	return file;
}

void FAT12::loadFile(int clustno, int size, char *buf, char *img) {
	int i;
	for (;;) {
		if (size <= 512) {
			for (i = 0; i < size; i++) {
				buf[i] = img[clustno  *512 + i];
			}
			break;
		}
		for (i = 0; i < 512; i++) {
			buf[i] = img[clustno  *512 + i];
		}

		size -= 512;
		buf += 512;
		clustno = FAT12::fat_[clustno];
	}
}

unsigned char *FAT12::loadFile2(int clustno, int *psize) {
	int size = *psize, size2;
	unsigned char *buf = new unsigned char[size];
	unsigned char *buf2;
	FAT12::loadFile(clustno, size, (char*)buf, (char*)(ADDRESS_DISK_IMAGE + 0x003e00));
	if (size >= 17) {
		size2 = TekGetSize(buf);
		if (size2 > 0) {
			buf2 = new unsigned char[size2];
			TekDecomp(buf, buf2, size2);
			delete buf;
			buf = buf2;
			*psize = size2;
		}
	}
	return buf;
}

FileInfo *FAT12::search(const char *name, FileInfo *finfo, int max) {
	char s[12];
	for (int i = 0; i < 12; i++) {
		s[i] = ' ';
	}

	if (finfo->name[0] == 0x00) return 0;

	for (int i = 0, j = 0; name[i]; i++) {
		if (j >= 11) return 0;
		if (name[i] == '.' && j <= 8) {
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') s[j] -= 0x20;
			j++;
		}
	}

	for (int i = 0; i < max; i++) {
		if (!(finfo[i].type & 0x18) && !strncmp(finfo[i].name, s, 11)) {
			return finfo + i;
		}
	}

	return 0;
}
