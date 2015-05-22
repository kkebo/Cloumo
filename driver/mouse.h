/*
 * マウス
 */

#ifndef _MOUSE_H_
#define _MOUSE_H_

const int kKeyCmdSendToMouse = 0xd4;
const int kMouseCmdEnable = 0xf4;

//class Browser;

struct MouseDecode {
	unsigned char buf[4];
	unsigned char phase;
	Point cod;
	int btn;
	int scroll;
};

class Mouse {
private:
	static const char *cursor[];
	static Sheet *sheet;
	static MouseDecode mdec;
	static bool scroll;
	static Point newCod;
	static int scroll_x, scroll_y;

public:
	static TaskQueue *queue;
	//static Task *browserTask;

public:
	static void Main();
};

#endif
