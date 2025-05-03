/*
 * サウンド
 */
#include <cloumo/kernel/asmfunc.h>
#include <cloumo/driver/sound.h>

void Beep(char c, int len, Timer *timer, bool st) {
	static int tonetable[12] = {
		1071618315, 1135340056, 1202850889, 1274376125, 1350154473, 1430438836,
		1515497155, 1605613306, 1701088041, 1802240000, 1909406767, 2022946002
	};
	static int notetable[7] = { 9, 11, 0, 2, 4, 5, 7 };
	int i;

	i = 192 / len;
	if (st) i += i / 2;
	timer->set(i * 125 / 120 * 7 / 8);

	if (c) {
		i = 48 + notetable[c - 'A'] + 12;
		i = 1193180000 / (tonetable[i % 12] >> (17 - i / 12));
		Output8(0x43, 0xb6);
		Output8(0x42, i & 0xff);
		Output8(0x42, i >> 8);
		i = (Input8(0x61) | 0x03) & 0x0f;
	} else {
		i = Input8(0x61) & 0x0d;
	}
	Output8(0x61, i);

	for (;;) {
		Cli();
		if (!timer->queue->isempty()) {
			i = timer->queue->pop();
			Sti();
			if (i == timer->data) return;
		} else {
			TaskSwitcher::getNowTask()->sleep();
			Sti();
		}
	}
}
