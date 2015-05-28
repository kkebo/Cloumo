/*
 * アセンブラ関数
 */
#pragma once

extern "C" {
	void Hlt();
	void Cli();
	void Sti();

	int Input8(int port);
	void Output8(int port, int data);
	int Input16(int port);
	void Output16(int port, int data);
	int Input32(int port);
	void Output32(int port, int data);

	void AsmIntHandler07();
	void AsmIntHandler20();
	void AsmIntHandler21();
	void AsmIntHandler27();
	void AsmIntHandler2c();

	int  LoadEflags(void);
	void StoreEflags(int);

	void LoadGdtr(int, int);
	void LoadIdtr(int, int);
	int  LoadCr0(void);
	void StoreCr0(int);
	void LoadTr(int);

	unsigned int MemoryTestSub(unsigned int, unsigned int);

	void FarJump(int, int);
}
