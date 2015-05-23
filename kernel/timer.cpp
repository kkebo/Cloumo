#include "../headers.h"

unsigned int TimerController::count = 0;
unsigned int TimerController::next = 0xffffffff;
Timer TimerController::timers0[MAX_TIMER];
Timer *TimerController::t0 = &TimerController::timers0[0];

Timer::Timer(TaskQueue *queue_) : queue(queue_) {
	for (int i = 0; i < MAX_TIMER; ++i) {
		if (this == &TimerController::timers0[i]) {
			data = i;
			return;
		}
	}
}

Timer::Timer(TaskQueue *queue_, int data_) : queue(queue_), data(data_) {}

Timer::~Timer() {
	cancel();
	flags = TimerFlag::Free;
}

void *Timer::operator new(size_t){
	// 0: TimerController::t0
	// 1: TaskController::timer
	for (auto &&timer : TimerController::timers0) {
		if (timer.flags == TimerFlag::Free) {
			timer.flags = TimerFlag::Reserved;
			return &timer;
		}
	}
	// 空きがない
	return nullptr;
}

void Timer::set(unsigned int newTimeout) {
	int e = LoadEflags();
	Cli();
	if (flags == TimerFlag::Reserved) {
		Timer *timer0, *timer1;
		timeout = newTimeout + TimerController::count; // 絶対時間に変換
		flags = TimerFlag::Running;
		timer0 = TimerController::t0;
	
		if (timeout <= timer0->timeout) {
			// 先頭に入る
			TimerController::t0 = this;
			next = timer0;
			TimerController::next = timeout;
		} else {
			// this が入る位置を決める
			do {
				timer1 = timer0;
				timer0 = timer0->next;
			} while (timeout > timer0->timeout);
			timer1->next = this;
			next = timer0;
		}
	}
	StoreEflags(e);
}

bool Timer::cancel() {
	int e = LoadEflags();
	Cli();
	if (flags == TimerFlag::Running) {
		if (this == TimerController::t0) {
			// 先頭だった場合
			TimerController::t0 = next;
			TimerController::next = next->timeout;
		} else {
			// 1つ前と1つ後をつなげる
			Timer *timer = TimerController::t0;
			while (timer->next != this) {
				timer = timer->next;
			}
			timer->next = next;
		}
		flags = TimerFlag::Reserved;
		StoreEflags(e);
		return true;
	}
	StoreEflags(e);
	return false;
}

// getter of data_
int Timer::getData() {
	return data;
}

void TimerController::init() {
	// Initialize PIT
	Output8(PIT_CTRL, 0x34);
	Output8(PIT_CNT0, 0x9c);
	Output8(PIT_CNT0, 0x2e);

	// Initialize member variables
	t0->timeout = 0xffffffff;
	t0->flags = TimerFlag::Running;
	t0->next = 0;
}

// count_ をリセット (これじゃだめだった)
/*void TimerController::reset() {
	int e = LoadEflags();
	Cli();
	
	int lastcount = count;
	count = 0;

	// t0 以外の動作中のタイマーを調整
	for (auto &&timer : timers0) {
		if (timer.flags == TimerFlag::Running) {
			timer.timeout -= lastcount;
		}
	}
	
	StoreEflags(e);
}*/
