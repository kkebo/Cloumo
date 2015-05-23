#include "../headers.h"

bool free0(void *addr, unsigned int size);

void MemoryInit() {
	MemoryManager *memoryManager = (MemoryManager *)ADDRESS_MEMORY_MANAGER;
	memoryManager->frees = 0;
	memoryManager->maxfrees = 0;
	memoryManager->lostsize = 0;
	memoryManager->losts = 0;
	free0((void *)0x00001000, 0x0009e000);
	free0((void *)0x00400000, MemoryTest(0x00400000, 0xbfffffff) - 0x00400000);
}

unsigned int MemoryTotal() {
	MemoryManager *memoryManager = (MemoryManager *)ADDRESS_MEMORY_MANAGER;
	unsigned int t = 0;
	for (int i = 0; i < memoryManager->frees; ++i) {
		t += memoryManager->freeinfo[i].size;
	}
	return t;
}

unsigned int MemoryTest(unsigned int start, unsigned int end) {
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 386 or 486+ */
	eflg = LoadEflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	StoreEflags(eflg);
	eflg = LoadEflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* 386はAC=1にしても自動で戻る */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	StoreEflags(eflg);

	if (flg486 != 0) {
		cr0 = LoadCr0();
		cr0 |= CR0_CACHE_DISABLE; /* Cache 禁止 */
		StoreCr0(cr0);
	}

	i = MemoryTestSub(start, end);

	if (flg486 != 0) {
		cr0 = LoadCr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* Cache 許可 */
		StoreCr0(cr0);
	}

	return i;
}

void *malloc(unsigned int size) {
	MemoryManager *memoryManager = (MemoryManager *)ADDRESS_MEMORY_MANAGER;

	size += sizeof(unsigned int); // サイズ記録分空ける

	for (int i = 0; i < memoryManager->frees; ++i) {
		if (memoryManager->freeinfo[i].size >= size) {
			unsigned int *result = (unsigned int *)memoryManager->freeinfo[i].addr;
			memoryManager->freeinfo[i].addr += size;
			memoryManager->freeinfo[i].size -= size;
			if (!memoryManager->freeinfo[i].size) {
				--memoryManager->frees;
				for (; i < memoryManager->frees; ++i) {
					memoryManager->freeinfo[i] = memoryManager->freeinfo[i + 1];
				}
			}
			// サイズ記録(サイズ記録域含む)
			result[0] = size;

			return result + 1;
		}
	}
	return 0;
}

bool free0(void *addr, unsigned int size) {
	MemoryManager *memoryManager = (MemoryManager *)ADDRESS_MEMORY_MANAGER;
	int i;

	for (i = 0; i < memoryManager->frees; ++i)
		if (memoryManager->freeinfo[i].addr > (unsigned int)addr)
			break;

	if (i > 0) {
		if (memoryManager->freeinfo[i - 1].addr + memoryManager->freeinfo[i - 1].size == (unsigned int)addr) {
			memoryManager->freeinfo[i - 1].size += size;
			if (i < memoryManager->frees) {
				if ((unsigned int)addr + size == memoryManager->freeinfo[i].addr) {
					memoryManager->freeinfo[i - 1].size += memoryManager->freeinfo[i].size;
					--memoryManager->frees;
					for (; i < memoryManager->frees; ++i) {
						memoryManager->freeinfo[i] = memoryManager->freeinfo[i + 1];
					}
				}
			}
			return true;
		}
	}
	if (i < memoryManager->frees) {
		if ((unsigned int)addr + size == memoryManager->freeinfo[i].addr) {
			memoryManager->freeinfo[i].addr = (unsigned int)addr;
			memoryManager->freeinfo[i].size += size;
			return true;
		}
	}
	if (memoryManager->frees < MEMORY_FREES) {
		for (int j = memoryManager->frees; j > i; --j) {
			memoryManager->freeinfo[j] = memoryManager->freeinfo[j - 1];
		}
		++memoryManager->frees;
		if (memoryManager->maxfrees < memoryManager->frees) {
			memoryManager->maxfrees = memoryManager->frees;
		}
		memoryManager->freeinfo[i].addr = (unsigned int)addr;
		memoryManager->freeinfo[i].size = size;
		return true;
	}
	++memoryManager->losts;
	memoryManager->lostsize += size;
	return false;
}

bool free(void *addr) {
	unsigned int *realaddr = (unsigned int *)addr - 1;
	return free0(realaddr, realaddr[0]);
}

void *malloc4k(unsigned int size) {
	return malloc((size + 0xfff) & 0xfffff000);
}

bool free4k(void *addr) {
	return free(addr/*, (size + 0xfff) & 0xfffff000*/);
}

/*extern "C" void *MemCopy(void* s1, void* s2, unsigned int size) {
	int d0, d1, d2;
	asm volatile(
		"cld;"
		"rep movsd;"
		"test %4, 2;"
		"je 1f;"
		"movsw;"
		"1:"
		"test %4, 1;"
		"je 2f;"
		"movsb;"
		"2:"
		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "0" (size / 4), "r" (size), "1" ((long)s1), "2" ((long)s2)
		: "memory"
	);
	return s1;
}*/

void *operator new(size_t size) throw() {
	return malloc4k(size);
}

void *operator new[](size_t size) throw() {
	return malloc4k(size);
}

void operator delete(void *address) {
	if (address) {
		free4k(address);
	}
}

void operator delete[](void *address) {
	if (address) {
		free4k(address);
	}
}
