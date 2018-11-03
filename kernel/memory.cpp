#include "../headers.h"

bool free0(uintptr_t addr, size_t size);

void MemoryInit() {
	MemoryManager *memoryManager = (MemoryManager *)ADDRESS_MEMORY_MANAGER;
	memoryManager->frees = 0;
	memoryManager->maxfrees = 0;
	memoryManager->lostsize = 0;
	memoryManager->losts = 0;
	free0(0x00001000, 0x0009e000);
	free0(0x00400000, MemoryTest(0x00400000, 0xbfffffff) - 0x00400000);
}

size_t MemoryTotal() {
	MemoryManager *memoryManager = (MemoryManager *)ADDRESS_MEMORY_MANAGER;
	size_t t = 0;
	for (size_t i = 0; i < memoryManager->frees; ++i) {
		t += memoryManager->freeinfo[i].size;
	}
	return t;
}

size_t MemoryTest(size_t start, size_t end) {
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

	i = (size_t)MemoryTestSub((unsigned int)start, (unsigned int)end);

	if (flg486 != 0) {
		cr0 = LoadCr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* Cache 許可 */
		StoreCr0(cr0);
	}

	return i;
}

void *malloc(size_t size) {
	MemoryManager *memoryManager = (MemoryManager *)ADDRESS_MEMORY_MANAGER;

	const size_t truesize = size + sizeof(size_t); // サイズ記録分空ける

	for (int i = 0; i < memoryManager->frees; ++i) {
		if (memoryManager->freeinfo[i].size >= truesize) {
			uintptr_t result = memoryManager->freeinfo[i].addr;
			memoryManager->freeinfo[i].addr += truesize;
			memoryManager->freeinfo[i].size -= truesize;
			if (memoryManager->freeinfo[i].size <= 0) {
				--memoryManager->frees;
				for (; i < memoryManager->frees; ++i) {
					memoryManager->freeinfo[i] = memoryManager->freeinfo[i + 1];
				}
			}
			// サイズ記録(サイズ記録域含む)
			*((size_t *)result) = truesize;

			return (void *)(result + sizeof(size_t));
		}
	}
	return 0;
}

bool free0(uintptr_t addr, size_t size) {
	MemoryManager *memoryManager = (MemoryManager *)ADDRESS_MEMORY_MANAGER;
	size_t i;

	for (i = 0; i < memoryManager->frees; ++i)
		if (memoryManager->freeinfo[i].addr > addr)
			break;

	if (i > 0) {
		if (memoryManager->freeinfo[i - 1].addr + memoryManager->freeinfo[i - 1].size == addr) {
			memoryManager->freeinfo[i - 1].size += size;
			if (i < memoryManager->frees) {
				if (addr + size == memoryManager->freeinfo[i].addr) {
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
		if (addr + size == memoryManager->freeinfo[i].addr) {
			memoryManager->freeinfo[i].addr = addr;
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
		memoryManager->freeinfo[i].addr = addr;
		memoryManager->freeinfo[i].size = size;
		return true;
	}
	++memoryManager->losts;
	memoryManager->lostsize += size;
	return false;
}

bool free(void *addr) {
	uintptr_t trueaddr = (uintptr_t)addr - sizeof(size_t);
	return free0(trueaddr, *((size_t *)trueaddr));
}

void *malloc4k(size_t size) {
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

void *operator new(size_t size) {
	return malloc4k(size);
}

void *operator new[](size_t size) {
	return malloc4k(size);
}

void operator delete(void *address) noexcept {
	if (address) {
		free4k(address);
	}
}

void operator delete[](void *address) noexcept {
	if (address) {
		free4k(address);
	}
}
