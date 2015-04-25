#include "../headers.h"

unsigned char KeyboardController::ascii_table_[0x80] = {
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0x08, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '@', '[', 0x0a, 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ':', 0,   0,   ']', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
};
unsigned char KeyboardController::ascii_shift_table_[0x80] = {
	0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a, 0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
};
int KeyboardController::shift_ = 0;
int KeyboardController::cmd_wait_ = -1;
int KeyboardController::leds_ = 0;
Queue *KeyboardController::cmd_ = nullptr;
Queue *KeyboardController::queue_ = nullptr;

void KeyboardController::init() {
	BootInfo *binfo = (BootInfo*)ADDRESS_BOOTINFO;
	leds_ = (binfo->leds >> 4) & 7;

	wait();
	Output8(kPortKeyCmd, kKeyCmdWriteMode);
	wait();
	Output8(kPortKeyData, kKBCMode);

	Task* task = TaskController::alloc();
	task->name_ = (char*)kKeyboardTaskName;
	task->tss_.esp = (int)malloc4k(64 * 1024) + 64 * 1024 - 12;
	task->tss_.eip = (int)&mainLoop;
	task->tss_.es = 1 * 8;
	task->tss_.cs = 2 * 8;
	task->tss_.ss = 1 * 8;
	task->tss_.ds = 1 * 8;
	task->tss_.fs = 1 * 8;
	task->tss_.gs = 1 * 8;
	task->run(2, 1);
	task->queue_ = Queue(128, task);
	queue_ = &task->queue_;
	cmd_ = new Queue(32);
}

void KeyboardController::mainLoop() {
	Task *task = TaskController::getNowTask();

	for (;;) {
		if (!cmd_->isempty() && cmd_wait_ < 0) {
			cmd_wait_ = cmd_->pop();
			wait();
			Output8(kPortKeyData, cmd_wait_);
		}
		Cli();
		if (task->queue_.isempty()) {
			task->sleep();
			Sti();
		} else {
			unsigned char code = task->queue_.pop();
			Sti();
			decode(code);
		}
	}
}

void KeyboardController::decode(unsigned char code) {
	char s[2];
	if (code < 0x80 && ascii_table_[code]) {
		// shift xor caps lock
		s[0] = (shift_ != leds_ & 4) ? ascii_shift_table_[code] : ascii_table_[code];
		s[1] = 0;
		SheetCtl::drawString(SheetCtl::back_, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22, 0, s);
		SheetCtl::refresh(SheetCtl::back_, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22, SheetCtl::tbox_cpos_ + 2 + 8, SheetCtl::back_->bysize - 20);
		SheetCtl::tbox_cpos_ += 8;
	}
	switch (code) {
		case 0x57:
			SheetCtl::upDown(SheetCtl::sheets_[1], SheetCtl::top_ - 1);
			break;

		case 0x3a:
			leds_ ^= 4;
			cmd_->push(kKeyCmdLED);
			cmd_->push(leds_);
			break;

		case 0x45:
			leds_ ^= 2;
			cmd_->push(kKeyCmdLED);
			cmd_->push(leds_);
			break;

		case 0x46:
			leds_ ^= 1;
			cmd_->push(kKeyCmdLED);
			cmd_->push(leds_);
			break;

		case 0xfa:
			cmd_wait_= -1;
			break;

		case 0xfe:
			wait();
			Output8(kPortKeyData, cmd_wait_);
			break;

		case 0x2a:
			shift_ |= 1;
			break;

		case 0x36:
			shift_ |= 2;
			break;

		case 0xaa:
			shift_ &= ~1;
			break;

		case 0xb6:
			shift_ &= ~2;
			break;

		case 0x0e:
			if (SheetCtl::tbox_cpos_ > 2) {
				SheetCtl::tbox_cpos_ -= 8;
				SheetCtl::fillRect(SheetCtl::back_, Rgb(255, 255, 255), SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22, SheetCtl::tbox_cpos_ + 2 + 8, SheetCtl::back_->bysize - 20);
				SheetCtl::refresh(SheetCtl::back_, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22, SheetCtl::tbox_cpos_ + 2 + 8, SheetCtl::back_->bysize - 20);
			}
			break;
	}
}

void KeyboardController::wait() {
	while (Input8(kPortKeyStatus) & kKeyStatusSendNotReady) {}
}
