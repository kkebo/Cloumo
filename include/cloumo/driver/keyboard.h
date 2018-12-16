/*
 * キーボード
 */

#pragma once

#include <Queue.h>
#include "../kernel/multitask.h"

const int kPortKeyData = 0x0060;
const int kPortKeyStatus = 0x0064;
const int kPortKeyCmd = 0x0064;
const int kKeyStatusSendNotReady = 0x02;
const int kKeyCmdWriteMode = 0x60;
const int kKBCMode = 0x47;
const int kKeyCmdLED = 0xed;

class KeyboardController {
private:
	static unsigned char asciiTable[];
	static unsigned char asciiShiftTable[];
	static unsigned char asciiTableUS[];
	static unsigned char asciiShiftTableUS[];
	static unsigned char *primaryTable;
	static unsigned char *primaryShiftTable;
	static int _shift;
	static bool _alt;
	static int leds;
	static Queue<int> *cmd;
	static int cmdWait;

public:
	static const int &shift;
	static const bool &alt;
	static TaskQueue *queue;

	static void Main();
	static void switchToUS();
	static void switchToJIS();
	static void Decode(unsigned char code);
	static void wait();
};
