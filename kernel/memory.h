/*
 * メモリ
 */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stddef.h>

const int EFLAGS_AC_BIT     = 0x00040000;
const int CR0_CACHE_DISABLE = 0x60000000;

const int MEMORY_FREES = 4090;
const int ADDRESS_MEMORY_MANAGER = 0x003c0000;

struct FreeInfo {
	unsigned int addr, size;
};

struct MemoryManager {
	int frees, maxfrees, lostsize, losts;
	FreeInfo freeinfo[MEMORY_FREES];
};

void         MemoryInit();
unsigned int MemoryTotal();
unsigned int MemoryTest(unsigned int, unsigned int);

extern "C" {
	void *malloc(unsigned int);
	bool free(void *);
	void *malloc4k(unsigned int);
	bool free4k(void *);
}

void *operator new(size_t) throw();
void *operator new[](size_t) throw();
void operator delete(void *);
void operator delete[](void *);

#endif
