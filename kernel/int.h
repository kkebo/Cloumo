/*
 * 割り込み
 */
#define kPic0Icw1 0x0020
#define kPic0Ocw2 0x0020
#define kPic0Imr  0x0021
#define kPic0Icw2 0x0021
#define kPic0Icw3 0x0021
#define kPic0Icw4 0x0021
#define kPic1Icw1 0x00a0
#define kPic1Ocw2 0x00a0
#define kPic1Imr  0x00a1
#define kPic1Icw2 0x00a1
#define kPic1Icw3 0x00a1
#define kPic1Icw4 0x00a1

void PICInit();

extern "C" {
	void IntHandler07(int *);
	void IntHandler20(int *);
	void IntHandler21(int *);
	void IntHandler2c(int *);
}
