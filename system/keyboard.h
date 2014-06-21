/*
 * キーボード
 */

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

const int kPortKeyData = 0x0060;
const int kPortKeyStatus = 0x0064;
const int kPortKeyCmd = 0x0064;
const int kKeyStatusSendNotReady = 0x02;
const int kKeyCmdWriteMode = 0x60;
const int kKBCMode = 0x47;
const int kKeyCmdLED = 0xed;

class KeyboardController {
private:
	static unsigned char ascii_table_[];
	static unsigned char ascii_shift_table_[];
	static int shift_;
	static int leds_;

public:
	static Queue* cmd_;
	static int cmd_wait_;
	static Queue* queue_;

public:
	static void init();
	static void mainLoop();
	static void decode(unsigned char);
	static void wait();
};

#endif
