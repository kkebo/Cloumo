#include "../headers.h"

void set8254xRegister(int reg, int data) {
	Output32(ioaddr + 0x00, reg); // set the IOADDR window
	Output32(ioaddr + 0x04, data); // write the value to the IOADDR window which will end up in the register in IOADDR
	Input32(ioaddr + 0x04); // read back the value
}