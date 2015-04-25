/*
 * アセンブラ関数
 */
#ifndef _ASMFUNC_H_
#define _ASMFUNC_H_

extern "C" {
	void Hlt();
	void Cli();
	void Sti();

	int Input8(int);
	void Output8(int, int);

//	void AsmIntHandler07();
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

#endif
