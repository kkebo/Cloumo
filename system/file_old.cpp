#include "headers.h"

File::File() : info_(0), buf_(0) {}

File::File(const char* name, FileInfo* finfo, const int max) : info_(0), buf_(0) {
	open(name, finfo, max);
}

File::~File() {
	if (buf_) delete buf_;
}

bool File::open(const char *name, FileInfo* finfo, const int max) {
	char s[12];
	/* スペースで初期化(null文字はなし)*/
	for (int i = 0; i < 12; i++) {
		s[i] = ' ';
	}

	if (!finfo->name[0]) {
		return false;
	}

	for (int i = 0, j = 0; name[i]; i++) {
		// ファイル名が長すぎる
		if (j >= 11) return false;
		
		if (name[i] == '.' && j <= 8) {
			// 拡張子前が短かったら調節
			j = 8;
		} else {
			// 大文字でコピー
			s[j] = ('a' <= name[i] && name[i] <= 'z') ? name[i] - 0x20 : name[i];
			j++;
		}
	}

	for (int i = 0; i < max; i++) {
		if ((finfo + i)->type & 0x18 == 0 && !strcmp((finfo + i)->name, s)) {
			info_ = finfo + i;
			// ファイル読み込み
			if (buf_) delete buf_;
			buf_ = new unsigned char[info_->size];
			load((char*)buf_, (char*)(ADDRESS_DISK_IMAGE + 0x003e00));
			if (info_->size >= 17) {
				int size2 = TekGetSize(buf_);
				if (size2 > 0) {
					unsigned char *buf2 = new unsigned char[size2];
					TekDecomp(buf_, buf2, size2);
					delete buf_;
					buf_ = buf2;
					info_->size = size2; // 解凍後のサイズに更新
				}
			}
			return true;
		}
	}

	return false;
}

void File::load(char* buf, const char* img) {
	while (info_->size > 512) {
		for (int i = 0; i < 512; i++) {
			buf[i] = img[info_->clustno * 512 + i];
		}
		
		info_->size -= 512;
		buf += 512;
		info_->clustno = FAT12::fat_[info_->clustno];
	}

	for (int i = 0; i < info_->size; i++) {
		buf[i] = img[info_->clustno * 512 + i];
	}
}

unsigned char* File::get() {
	return buf_;
}

unsigned int File::getSize() {
	return info_->size;
}

int* FAT12::fat_ = 0;

void FAT12::init() {
	unsigned char* img = (unsigned char*)(ADDRESS_DISK_IMAGE + 0x000200);

	fat_ = new int[2880];
	for (int i = 0, j = 0; i < 2880; i += 2, j += 3) {
		fat_[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat_[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
	}
}
