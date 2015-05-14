#include "../headers.h"

Timer::Timer(TaskQueue *queue) {
	queue_ = queue;
	for (int i = 0; i < MAX_TIMER; i++) {
		if (this == &TimerController::timers0_[i]) {
			data_ = i;
			return;
		}
	}
}

Timer::Timer(TaskQueue *queue, int data) {
	queue_ = queue;
	data_ = data;
}

Timer::~Timer() {
	flags_ = TimerFlag::Free;
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

void Timer::set(unsigned int timeout) {
	int e = LoadEflags();
	Cli();
	if (flags_ == TimerFlag::Reserved) {
		Timer *timer0, *timer1;
		timeout_ = timeout + TimerController::count_; // 絶対時間に変換
		flags_ = TimerFlag::Running;
		timer0 = TimerController::t0_;
	
		if (timeout_ <= timer0->timeout_) {
			// 先頭に入る
			TimerController::t0_ = this;
			next_ = timer0;
			TimerController::next_ = timeout_;
		} else {
			// this が入る位置を決める
			while (timeout_ > timer0->timeout_) {
				timer1 = timer0;
				timer0 = timer0->next_;
			}
			timer1->next_ = this;
			next_ = timer0;
		}
	}
	StoreEflags(e);
}

bool Timer::cancel() {
	int e = LoadEflags();
	Cli();
	if (flags_ == TimerFlag::Running) {
		if (this == TimerController::t0_) {
			// 先頭だった場合
			TimerController::t0_ = next_;
			TimerController::next_ = next_->timeout_;
		} else {
			// 1つ前と1つ後をつなげる
			Timer *timer = TimerController::t0_;
			while (timer->next_ != this) {
				timer = timer->next_;
			}
			timer->next_ = next_;
		}
		flags_ = TimerFlag::Reserved;
		StoreEflags(e);
		return true;
	}
	StoreEflags(e);
	return false;
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

	timers0_ = ::new Timer[MAX_TIMER];
	for (int i = 0; i < MAX_TIMER; i++) {
		timers0_[i].flags_ = TimerFlag::Free;
	}

	t0_ = &timers0_[0];
	t0_->timeout_ = 0xffffffff;
	t0_->flags_ = TimerFlag::Running;
	t0_->next_ = 0;
}

// count_ をリセット (これじゃだめだった)
/*void TimerController::reset() {
	int e = LoadEflags();
	Cli();
	
	int lastcount = count_;
	count_ = 0;

	// t0 以外の動作中のタイマーを調整
	for (int i = 1; i < MAX_TIMER; i++) {
		Timer &timer = timers0_[i];
		if (timer.flags_ == TimerFlag::Running) {
			timer.timeout_ -= lastcount;
		}
	}
	
	StoreEflags(e);
}*/
