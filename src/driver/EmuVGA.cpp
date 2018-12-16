#include <cloumo/kernel/asmfunc.h>
#include <cloumo/driver/EmuVGA.h>

void setEmuVGARegister(int reg, int data) {
	Output16(0x01ce, reg);
	Output16(0x01cf, data);
}

void initEmuVGA(int x, int y, int color) {
	setEmuVGARegister(0x0001, x);
	setEmuVGARegister(0x0002, y);
	setEmuVGARegister(0x0003, color);
	setEmuVGARegister(0x0004, 0xc1);
	setEmuVGARegister(0x0005, 0x0000);
	// 0x0006: 実際よりも大きく設定できる仮想の width
	// 0x0007: 実際よりも大きく設定できる仮想の height
	// 0x0008: 表示開始位置の x 座標
	// 0x0009: 表示開始位置の y 座標
}
