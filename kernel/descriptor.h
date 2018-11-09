/*
 * Descriptor
 */
const int kAdrIdt = 0x0026f800;
const int kLimitIdt = 0x000007ff;
const int kAdrGdt = 0x00270000;
const int kLimitGdt = 0x0000ffff;
const int kAdrBootpack = 0x00280000;
const int kLimitBootpack = 0x0007ffff;
const int kArData32Rw = 0x4092;
const int kArCode32Er = 0x409a;
const int kArLdt = 0x0082;
const int kArTss32 = 0x0089;
const int kArIntGate32 = 0x008e;

struct SegmentDescriptor {
	short limit_low, base_low;
	char  base_mid, access_right;
	char  limit_high, base_high;
};

struct GateDescriptor {
	short offset_low, selector;
	char  dw_count, access_right;
	short offset_high;
};

void DescriptorInit();
void SetSegmentDescriptor(SegmentDescriptor*, unsigned int, uintptr_t, int);
void SetGateDescriptor(GateDescriptor*, int, int, int);

