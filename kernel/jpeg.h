/*
 * jpeg
 */

#pragma once

struct RGB {
	unsigned char b, g, r, t;
};

extern "C" {
	int info_JPEG(DLL_STRPICENV *, int *, int, unsigned char *);
	int decode0_JPEG(DLL_STRPICENV *, int, unsigned char *, int, unsigned char *, int);
}
