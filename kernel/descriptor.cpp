#include "../headers.h"

// GDT/IDT初期化
void DescriptorInit() {
	/* GDT初期化 */
	auto gdt = reinterpret_cast<SegmentDescriptor *>(kAdrGdt);
	for (int i = 0; i <= kLimitGdt / 8; ++i) {
		gdt[i].set(0, 0, 0);
	}
	gdt[1].set(0xffffffff, 0x00000000, kArData32Rw);
	gdt[2].set(kLimitBootpack, kAdrBootpack, kArCode32Er);
	LoadGdtr(kLimitGdt, kAdrGdt);

	/* IDT初期化 */
	auto idt = reinterpret_cast<GateDescriptor *>(kAdrIdt);
	for (int i = 0; i <= kLimitIdt / 8; ++i) {
		idt[i].set(0, 0, 0);
	}
	LoadIdtr(kLimitIdt, kAdrIdt);

	/* IDTの設定 */
	idt[0x04].set(reinterpret_cast<uintptr_t>(&AsmIntHandler04), 2 * 8, kArIntGate32);
	idt[0x07].set(reinterpret_cast<uintptr_t>(&AsmIntHandler07), 2 * 8, kArIntGate32);
	idt[0x0d].set(reinterpret_cast<uintptr_t>(&AsmIntHandler0d), 2 * 8, kArIntGate32);
	idt[0x20].set(reinterpret_cast<uintptr_t>(&AsmIntHandler20), 2 * 8, kArIntGate32);
	idt[0x21].set(reinterpret_cast<uintptr_t>(&AsmIntHandler21), 2 * 8, kArIntGate32);
	idt[0x27].set(reinterpret_cast<uintptr_t>(&AsmIntHandler27), 2 * 8, kArIntGate32);
	idt[0x2c].set(reinterpret_cast<uintptr_t>(&AsmIntHandler2c), 2 * 8, kArIntGate32);
}
