#include "../headers.h"

Timer::Timer(TaskQueue *queue_) : _data(TimerController::count), _queue(queue_) {}

Timer::Timer(TaskQueue *queue_, int data_) : _data(data_), _queue(queue_) {}

Timer::~Timer() {
	cancel();
}

void Timer::set(unsigned int newTimeout) {
	int e = LoadEflags();
	Cli();
	if (!running) {
		Timer *timer0, *timer1;
		timeout = newTimeout + TimerController::count; // 絶対時間に変換
		running = true;
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
	if (running) {
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
		running = false;
		StoreEflags(e);
		return true;
	}
	StoreEflags(e);
	return false;
}

unsigned int TimerController::count = 0;
unsigned int TimerController::next = 0xffffffff;
Timer *TimerController::t0;

void TimerController::init() {
	// Initialize PIT
	Output8(PIT_CTRL, 0x34);
	Output8(PIT_CNT0, 0x9c);
	Output8(PIT_CNT0, 0x2e);

	// Initialize member variables
	t0 = new Timer(nullptr);
	t0->timeout = 0xffffffff;
	t0->running = true;
	t0->next = nullptr;
}

// task にセットされた動作中のタイマーを削除
void TimerController::remove(const Task &task) {
	for (Timer *timer = t0; timer; timer = timer->next) {
		if (timer->queue->task == &task) {
			delete timer;
		}
	}
}

// count_ をリセット (これじゃだめだった)
/*void TimerController::reset() {
	int e = LoadEflags();
	Cli();
	
	int lastcount = count;
	count = 0;

	// t0 以外の動作中のタイマーを調整
	for (auto &&timer : timers0) {
		if (timer.runnig) {
			timer.timeout -= lastcount;
		}
	}
	
	StoreEflags(e);
}*/
