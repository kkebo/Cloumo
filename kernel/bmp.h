/*
 * bmp
 */

#ifndef _BMP_H_
#define _BMP_H_

struct DLL_STRPICENV {
	int work[64 * 1024 / 4];
};

extern "C" {
	int _info_BMP(DLL_STRPICENV *, int *, int, unsigned char *);
	int _decode0_BMP(DLL_STRPICENV *, int, unsigned char *, int, unsigned char*, int);
}

#endif
