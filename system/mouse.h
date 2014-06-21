/*
 * マウス
 */

#ifndef _MOUSE_H_
#define _MOUSE_H_

const int kKeyCmdSendToMouse = 0xd4;
const int kMouseCmdEnable = 0xf4;

class Queue;

class Mouse {
private:
	static unsigned char phase_;
	static unsigned char buf_[];
	static const char* cursor_[];
	static Sheet* sheet_;
	static int x_, y_;
	static int btn_;
	static int new_mx_, new_my_;

public:
	static Queue* queue_;

public:
	static void Init();
	static void Main();
};

#endif
