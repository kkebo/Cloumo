/*
 * 起動情報
 */

#pragma once

const int ADDRESS_BOOTINFO  = 0x00000ff0;
const int ADDRESS_DISK_IMAGE = 0x00100000;

struct BootInfo {
	char  cyls;
	char  leds;
	char  vmode;
	char  reserve;
	short scrnx;
	short scrny;
	unsigned char *vram;
};
