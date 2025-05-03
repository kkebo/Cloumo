#include "headers.h"

void Timer::init(Queue* queue) {
	queue_ = queue;
	for (int i = 0; i < MAX_TIMER; i++) {
		if (this == &TimerController::timers0_[i]) {
			data_ = i;
			return;
		}
	}
	data_ = -1;
}

void Timer::init(Queue* queue, int data) {
	queue_ = queue;
	data_ = data;
}

void Timer::free() {
	flags_ = TIMERFLAG_FREE;
}

void Timer::set(unsigned int timeout) {
	Timer* t;
	Timer* s;
	timeout_ = timeout + TimerController::count_; // 絶対時間に変換
	flags_ = TIMERFLAG_INUSE;
	Cli();
	t = TimerController::t0_;

	// this が先頭に入る
	if (timeout_ <= t->timeout_) {
		TimerController::t0_ = this;
		next_ = t;
		TimerController::next_ = timeout_;
		Sti();
		return;
	}

	// this が入る位置を決める
	for (;;) {
		s = t;
		t = t->next_;
		if (timeout_ <= t->timeout_) {
			s->next_ = this;
			next_ = t;
			Sti();
			return;
		}
	}
}

// getter of data_
int Timer::data() {
	return data_;
}

unsigned int TimerController::count_ = 0;
unsigned int TimerController::next_ = 0xffffffff;
Timer* TimerController::t0_ = 0;
Timer* TimerController::timers0_ = 0;

void TimerController::init() {
	Output8(PIT_CTRL, 0x34);
	Output8(PIT_CNT0, 0x9c);
	Output8(PIT_CNT0, 0x2e);

	timers0_ = (Timer*)malloc4k(MAX_TIMER * sizeof(Timer));//new Timer[MAX_TIMER];
	for (int i = 0; i < MAX_TIMER; i++) {
		timers0_[i].flags_ = TIMERFLAG_FREE;
	}

	t0_ = &timers0_[0];
	t0_->timeout_ = 0xffffffff;
	t0_->flags_ = TIMERFLAG_INUSE;
	t0_->next_ = 0;
}

// タイマーを確保
Timer* TimerController::alloc() {
	for (int i = 0; i < MAX_TIMER; i++) {
		if (!timers0_[i].flags_) {
			timers0_[i].flags_ = TIMERFLAG_ALLOCATED;
			return &timers0_[i];
		}
	}
	// 空きがない
	return 0;
}

// count_ をリセット
void TimerController::reset() {
	int lastcount = count_;
	count_ = 0;

	// t0 以外の動作中のタイマーを調整
	for (int i = 1; i < MAX_TIMER; i++) {
		Timer& timer = timers0_[i];
		if (timer.flags_ == TIMERFLAG_INUSE) {
			timer.timeout_ -= lastcount;
		}
	}
}

