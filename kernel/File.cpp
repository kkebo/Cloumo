#include "../headers.h"

bool File::open() {
	// FAT12
	FAT12::FileInfo *info = FAT12::search(_name.c_str(), (FAT12::FileInfo *)(ADDRESS_DISK_IMAGE + 0x002600), 224);
	if (info) {
		_size = info->size;
		buf.reset(FAT12::loadFile2(info->clustno, _size));
		return true;
	}
	return false;
}

bool File::open(const string &fileName) {
	_name = fileName;
	return open();
}
