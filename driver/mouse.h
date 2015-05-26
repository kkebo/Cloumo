/*
 * マウス
 */

#pragma once

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
	static TaskQueue *queue;
	//static Task *browserTask;

public:
	friend void IntHandler2c(int *esp);
	static void Main();
};
