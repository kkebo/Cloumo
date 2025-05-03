/*
 * マウス
 */

#pragma once

#include "../kernel/graphic.h"

const int kKeyCmdSendToMouse = 0xd4;
const int kMouseCmdEnable = 0xf4;

//class Browser;

struct MouseDecode {
	unsigned char buf[4];
	unsigned char phase;
	Point pos;
	int btn;
	int scroll;
};

class Mouse {
private:
	static MouseDecode mdec;
	static bool scroll;
	//static Task *browserTask;

public:
	static TaskQueue *queue;

	static void Main();
};
