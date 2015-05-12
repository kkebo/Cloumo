/*
 * マウス
 */

#ifndef _MOUSE_H_
#define _MOUSE_H_

const int kKeyCmdSendToMouse = 0xd4;
const int kMouseCmdEnable = 0xf4;

class Browser;

struct MouseDecode {
	unsigned char buf_[4];
	unsigned char phase_;
	int x_, y_;
	int btn_;
	int scroll_;
};

class Mouse {
private:
	static const char *cursor_[];
	static Sheet *sheet_;
	static MouseDecode mdec_;
	static bool scroll_;
	static int new_mx_, new_my_;
	static int scroll_x, scroll_y;

public:
	static Queue<int> *queue_;
	static Task *browserTask;

public:
	static void Main();
};

#endif
