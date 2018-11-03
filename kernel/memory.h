/*
 * メモリ
 */

#pragma once

#include <stddef.h>

const int EFLAGS_AC_BIT     = 0x00040000;
const int CR0_CACHE_DISABLE = 0x60000000;

const size_t MEMORY_FREES = 4090;
const uintptr_t ADDRESS_MEMORY_MANAGER = 0x003c0000;

struct FreeInfo {
	uintptr_t addr;
	size_t size;
};

struct MemoryManager {
	size_t frees, maxfrees, lostsize, losts;
	FreeInfo freeinfo[MEMORY_FREES];
};

void   MemoryInit();
size_t MemoryTotal();
size_t MemoryTest(size_t, size_t);

extern "C" {
	void *malloc(size_t);
	bool free(void *);
	void *malloc4k(size_t);
	bool free4k(void *);
}
