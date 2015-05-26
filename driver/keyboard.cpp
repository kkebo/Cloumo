#include "../headers.h"

unsigned char KeyboardController::asciiTable[0x80] = {
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0x08, 0x09,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '@', '[', 0x0a, 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ':', 0,   0,   ']', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
};
unsigned char KeyboardController::asciiShiftTable[0x80] = {
	0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0x09,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a, 0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
};
unsigned char KeyboardController::asciiTableUS[0x80] = {
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08, 0x09,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0x0a, 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,   '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};
unsigned char KeyboardController::asciiShiftTableUS[0x80] = {
	0,   0,   '!', '@', '#', '$', '%', '^', '&', '\'', '(', ')', '_', '+', 0x08, 0x09,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0x0a, 0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0,   '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};
unsigned char *KeyboardController::primaryTable = asciiTable;
unsigned char *KeyboardController::primaryShiftTable = asciiShiftTable;
int KeyboardController::_shift = 0;
const int &KeyboardController::shift = _shift;
bool KeyboardController::_alt = false;
const bool &KeyboardController::alt = _alt;
int KeyboardController::cmdWait = -1;
int KeyboardController::leds = 0;
Queue<int> *KeyboardController::cmd;
TaskQueue *KeyboardController::queue;

void KeyboardController::Main() {
	Task &task = *TaskSwitcher::getNowTask();
	
	// キーボード初期化
	wait();
	Output8(kPortKeyCmd, kKeyCmdWriteMode);
	wait();
	Output8(kPortKeyData, kKBCMode);
	
	// メンバ変数初期化
	cmd = new Queue<int>(32);
	BootInfo *binfo = (BootInfo *)ADDRESS_BOOTINFO;
	leds = (binfo->leds >> 4) & 7;
	queue = task.queue;
	
	for (;;) {
		if (!cmd->isempty() && cmdWait < 0) {
			cmdWait = cmd->pop();
			wait();
			Output8(kPortKeyData, cmdWait);
		}
		Cli();
		if (queue->isempty()) {
			task.sleep();
			Sti();
		} else {
			unsigned char code = queue->pop();
			Sti();
			Decode(code);
		}
	}
}

void KeyboardController::switchToUS() {
	primaryTable = asciiTableUS;
	primaryShiftTable = asciiShiftTableUS;
}

void KeyboardController::switchToJIS() {
	primaryTable = asciiTable;
	primaryShiftTable = asciiShiftTable;
}

void KeyboardController::Decode(unsigned char code) {
	if (code < 0x80 && asciiTable[code]) {
		SheetCtl::queue->push(((shift && !(leds & 4)) || (!shift && leds & 4)) ? primaryShiftTable[code] : primaryTable[code]);
	}
	switch (code) {
		case 0x38: // Alt pressed
			_alt = true;
			break;
		
		case 0xe8: // Alt released
			_alt = false;
			break;

		case 0x3a: // Caps Lock
			leds ^= 4;
			cmd->push(kKeyCmdLED);
			cmd->push(leds);
			break;

		case 0x45: // Num Lock
			leds ^= 2;
			cmd->push(kKeyCmdLED);
			cmd->push(leds);
			break;

		case 0x46: // Scroll Lock
			leds ^= 1;
			cmd->push(kKeyCmdLED);
			cmd->push(leds);
			break;

		case 0xfa:
			cmdWait= -1;
			break;

		case 0xfe:
			wait();
			Output8(kPortKeyData, cmdWait);
			break;

		case 0x2a:
			_shift |= 1;
			break;

		case 0x36:
			_shift |= 2;
			break;

		case 0xaa:
			_shift &= ~1;
			break;

		case 0xb6:
			_shift &= ~2;
			break;
	}
}

void KeyboardController::wait() {
	while (Input8(kPortKeyStatus) & kKeyStatusSendNotReady) {}
}
