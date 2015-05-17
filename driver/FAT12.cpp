#include "../headers.h"

int *FAT12::fat_ = nullptr;

void FAT12::init() {
	unsigned char *img = (unsigned char *)(ADDRESS_DISK_IMAGE + 0x000200);

	fat_ = new int[2880];
	for (int i = 0, j = 0; i < 2880; i += 2, j += 3) {
		fat_[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat_[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
	}
}

File *FAT12::open(const char *name) {
	File *file = new File;
	file->info_ = FAT12::search(name, (FileInfo *)(ADDRESS_DISK_IMAGE + 0x002600), 224);
	if (!file->info_) return nullptr;
	file->size_ = file->info_->size;
	file->buf_ = (unsigned char *)FAT12::loadFile2(file->info_->clustno, file->size_);
	return file;
}

void FAT12::loadFile(int clustno, int size, char *buf, char *img) {
	for (;;) {
		if (size <= 512) {
			for (int i = 0; i < size; ++i) {
				buf[i] = img[clustno  *512 + i];
			}
			break;
		}
		for (int i = 0; i < 512; ++i) {
			buf[i] = img[clustno  *512 + i];
		}

		size -= 512;
		buf += 512;
		clustno = FAT12::fat_[clustno];
	}
}

unsigned char *FAT12::loadFile2(int clustno, int &psize) {
	unsigned char *buf = new unsigned char[psize];
	FAT12::loadFile(clustno, psize, (char *)buf, (char *)(ADDRESS_DISK_IMAGE + 0x003e00));
	if (psize >= 17) {
		int tekSize = TekGetSize(buf);
		if (tekSize > 0) {
			unsigned char *buf2 = new unsigned char[tekSize];
			TekDecomp(buf, buf2, tekSize);
			delete buf;
			psize = tekSize;
			return buf2;
		}
	}
	return buf;
}

FileInfo *FAT12::search(const char *name, FileInfo *finfo, int max) {
	char s[11];
	int j = 0;

	for (int i = 0; name[i]; ++i) {
		if (j >= 11) return nullptr;
		if (name[i] == '.' && j <= 8) {
			while (j < 8) {
				s[j++] = ' ';
			}
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') s[j] -= 0x20;
			++j;
		}
	}
	while (j < 11) {
		s[j++] = ' ';
	}

	for (int i = 0; i < max; ++i) {
		if (!(finfo[i].type & 0x18) && !strncmp(finfo[i].name, s, 11)) {
			return finfo + i;
		}
	}

	return nullptr;
}
