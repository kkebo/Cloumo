#include "../headers.h"

// GDT/IDT初期化
void DescriptorInit() {
	SegmentDescriptor *gdt = (SegmentDescriptor *) kAdrGdt;
	GateDescriptor    *idt = (GateDescriptor *)    kAdrIdt;

	/* GDT初期化 */
	for (int i = 0; i <= kLimitGdt / 8; i++) {
		SetSegmentDescriptor(gdt + i, 0, 0, 0);
	}
	SetSegmentDescriptor(gdt + 1, 0xffffffff, 0x00000000, kArData32Rw);
	SetSegmentDescriptor(gdt + 2, kLimitBootpack, kAdrBootpack, kArCode32Er);
	LoadGdtr(kLimitGdt, kAdrGdt);

	/* IDT初期化 */
	for (int i = 0; i <= kLimitIdt / 8; i++) {
		SetGateDescriptor(idt + i, 0, 0, 0);
	}
	LoadIdtr(kLimitIdt, kAdrIdt);

	/* IDTの設定 */
	//SetGateDescriptor(idt + 0x07, (int)&AsmIntHandler07, 2 * 8, kArIntGate32);
	SetGateDescriptor(idt + 0x20, (int)&AsmIntHandler20, 2 * 8, kArIntGate32);
	SetGateDescriptor(idt + 0x21, (int)&AsmIntHandler21, 2 * 8, kArIntGate32);
	SetGateDescriptor(idt + 0x27, (int)&AsmIntHandler27, 2 * 8, kArIntGate32);
	SetGateDescriptor(idt + 0x2c, (int)&AsmIntHandler2c, 2 * 8, kArIntGate32);
}

// Segment Descriptorセット
void SetSegmentDescriptor(SegmentDescriptor *sd, unsigned int limit, int base, int ar) {
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
}

// Gate Descriptorセット
void SetGateDescriptor(GateDescriptor *gd, int offset, int selector, int ar) {
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
}
