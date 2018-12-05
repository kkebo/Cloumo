/*
 * Descriptor
 */
const uintptr_t kAdrIdt = 0x0026f800;
const uintptr_t kLimitIdt = 0x000007ff;
const uintptr_t kAdrGdt = 0x00270000;
const uintptr_t kLimitGdt = 0x0000ffff;
const uintptr_t kAdrBootpack = 0x00280000;
const uintptr_t kLimitBootpack = 0x0007ffff;
const uintptr_t kArData32Rw = 0x4092;
const uintptr_t kArCode32Er = 0x409a;
const uintptr_t kArLdt = 0x0082;
const uintptr_t kArTss32 = 0x0089;
const uintptr_t kArIntGate32 = 0x008e;

struct SegmentDescriptor {
	short limit_low, base_low;
	char  base_mid, access_right;
	char  limit_high, base_high;

	void set(unsigned int limit, uintptr_t base, int ar) {
		if (limit > 0xfffff) {
			ar |= 0x8000; /* G_bit = 1 */
			limit /= 0x1000;
		}
		this->limit_low    = limit & 0xffff;
		this->base_low     = base & 0xffff;
		this->base_mid     = (base >> 16) & 0xff;
		this->access_right = ar & 0xff;
		this->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
		this->base_high    = (base >> 24) & 0xff;
	}
};

struct GateDescriptor {
	short offset_low, selector;
	char  dw_count, access_right;
	short offset_high;

	void set(uintptr_t offset, int selector, int ar) {
		this->offset_low   = offset & 0xffff;
		this->selector     = selector;
		this->dw_count     = (ar >> 8) & 0xff;
		this->access_right = ar & 0xff;
		this->offset_high  = (offset >> 16) & 0xffff;
	}
};

void DescriptorInit();
