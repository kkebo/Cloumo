#include "../headers.h"

Timer::Timer(Queue *queue) {
	queue_ = queue;
	for (int i = 0; i < MAX_TIMER; i++) {
		if (this == &TimerController::timers0_[i]) {
			data_ = i;
			return;
		}
	}
	data_ = -1;
}

Timer::Timer(Queue *queue, int data) {
	queue_ = queue;
	data_ = data;
}

void *Timer::operator new(size_t size){
	for (int i = 0; i < MAX_TIMER; i++) {
		if (TimerController::timers0_[i].flags_ == TimerFlag::Free) {
			TimerController::timers0_[i].flags_ = TimerFlag::Reserved;
			return &TimerController::timers0_[i];
		}
	}
	// 空きがない
	return nullptr;
}

void Timer::operator delete(void *p) {
	if (p) reinterpret_cast<Timer *>(p)->flags_ = TimerFlag::Free;
	// メモリが開放されてしまう？
}

void Timer::set(unsigned int timeout) {
	Timer *t, *s;
	timeout_ = timeout + TimerController::count_; // 絶対時間に変換
	flags_ = TimerFlag::Running;
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
Timer *TimerController::t0_ = nullptr;
Timer *TimerController::timers0_ = nullptr;

void TimerController::init() {
	Output8(PIT_CTRL, 0x34);
	Output8(PIT_CNT0, 0x9c);
	Output8(PIT_CNT0, 0x2e);

	timers0_ = (Timer *)malloc4k(MAX_TIMER * sizeof(Timer));
	for (int i = 0; i < MAX_TIMER; i++) {
		timers0_[i].flags_ = TimerFlag::Free;
	}

	t0_ = &timers0_[0];
	t0_->timeout_ = 0xffffffff;
	t0_->flags_ = TimerFlag::Running;
	t0_->next_ = 0;
}

// count_ をリセット
void TimerController::reset() {
	int lastcount = count_;
	count_ = 0;

	// t0 以外の動作中のタイマーを調整
	for (int i = 1; i < MAX_TIMER; i++) {
		Timer &timer = timers0_[i];
		if (timer.flags_ == TimerFlag::Running) {
			timer.timeout_ -= lastcount;
		}
	}
}
