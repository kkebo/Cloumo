#include "../headers.h"

unsigned char KeyboardController::ascii_table_[0x80] = {
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0/*0x08*/, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '@', '[', 0/*0x0a*/, 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ':', 0,   0,   ']', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
};
unsigned char KeyboardController::ascii_shift_table_[0x80] = {
	0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0/*0x08*/, 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0/*0x0a*/, 0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
};
int KeyboardController::shift_ = 0;
bool KeyboardController::alt = false;
int KeyboardController::cmd_wait_ = -1;
int KeyboardController::leds_ = 0;
Queue<int> *KeyboardController::cmd_ = nullptr;
TaskQueue *KeyboardController::queue_ = nullptr;

void KeyboardController::Main() {
	Task *task = TaskController::getNowTask();
	
	// キーボード初期化
	wait();
	Output8(kPortKeyCmd, kKeyCmdWriteMode);
	wait();
	Output8(kPortKeyData, kKBCMode);
	
	// メンバ初期化
	BootInfo *binfo = (BootInfo *)ADDRESS_BOOTINFO;
	leds_ = (binfo->leds >> 4) & 7;
	queue_ = task->queue_;
	cmd_ = new Queue<int>(32);
	
	// キャレットの表示とタイマー設定
	SheetCtl::drawLine(SheetCtl::back_, SheetCtl::tbox_col_, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 2);
	SheetCtl::back_->refresh(SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ + 3, SheetCtl::back_->bysize - 20 - 2);
	SheetCtl::tbox_col_ = Rgb(255, 255, 255);
	SheetCtl::tbox_timer_ = new Timer(task->queue_, 256);
	SheetCtl::tbox_timer_->set(50);
	
	for (;;) {
		if (!cmd_->isempty() && cmd_wait_ < 0) {
			cmd_wait_ = cmd_->pop();
			wait();
			Output8(kPortKeyData, cmd_wait_);
		}
		Cli();
		if (task->queue_->isempty()) {
			task->sleep();
			Sti();
		} else {
			int code = task->queue_->pop();
			Sti();
			if (code == SheetCtl::tbox_timer_->data()) {
				SheetCtl::drawLine(SheetCtl::back_, SheetCtl::tbox_col_, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 2);
				SheetCtl::back_->refresh(SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ + 3, SheetCtl::back_->bysize - 20 - 2);
				SheetCtl::tbox_col_ ^= Rgb(255, 255, 255);
				SheetCtl::tbox_timer_->set(50);
			} else if (code < 256) {
				Decode(static_cast<unsigned char>(code));
			}
		}
	}
}

void KeyboardController::Decode(unsigned char code) {
	if (code < 0x80 && ascii_table_[code]) {
		char s[2];
		s[0] = ((shift_ && !(leds_ & 4)) || (!shift_ && leds_ & 4)) ? ascii_shift_table_[code] : ascii_table_[code];
		s[1] = 0;
		SheetCtl::drawLine(SheetCtl::back_, Rgb(255, 255, 255), SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 2);
		SheetCtl::drawString(SheetCtl::back_, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 3, 0, s);
		SheetCtl::tbox_cpos_ += 8;
		SheetCtl::tbox_col_ = 0;
		SheetCtl::drawLine(SheetCtl::back_, SheetCtl::tbox_col_, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 2);
		SheetCtl::back_->refresh(SheetCtl::tbox_cpos_ - 8 + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ - 8 + 2 + 8 + 1, SheetCtl::back_->bysize - 20 - 2);
		SheetCtl::tbox_timer_->cancel();
		SheetCtl::tbox_timer_->set(50);
		*SheetCtl::tbox_str_ += s[0];
	}
	switch (code) {
		case 0x38: // Alt pressed
			alt = true;
			break;
		
		case 0xe8: // Alt released
			alt = false;
			break;
		
		case 0x0f: // Tab pressed
			if (alt) {
				// タブ切り替え
				int newActive = SheetCtl::activeTab + 1;
				if (newActive >= SheetCtl::numOfTab) newActive = 0;
				
				// 次のタブ
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * newActive, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * newActive, kPassiveTabColor, kActiveTabColor);
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * newActive, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * newActive, kPassiveTextColor, kActiveTextColor);
				SheetCtl::back_->refresh(2, 35 + 23 * newActive, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * newActive);
				// アクティブだったタブ
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTabColor, kPassiveTabColor);
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTextColor, kPassiveTextColor);
				SheetCtl::back_->refresh(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab);
				
				SheetCtl::window_[SheetCtl::activeTab]->upDown(-1);
				SheetCtl::window_[newActive]->upDown(1);
				
				SheetCtl::activeTab = newActive;
			}
			break;
		
		case 0x1c: { // Enter pressed
			if (SheetCtl::tbox_str_->length() == 0) break;
			
			string filename = *SheetCtl::tbox_str_;
			
			// file:/// の削除
			if (filename.compare(0, 8, "file:///") == 0) {
				filename.erase(0, 8);
			}
			
			if (unique_ptr<File> htmlFile = FAT12::open(filename.c_str())) {
				// ファイルが存在した
				filename = "file:///" + filename;
				// タブ表示
				SheetCtl::drawString(SheetCtl::back_, 6, 39 + 23 * SheetCtl::numOfTab, kActiveTextColor, filename.c_str());
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::numOfTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::numOfTab, kBackgroundColor, kActiveTabColor);
				SheetCtl::back_->refresh(2, 35 + 23 * SheetCtl::numOfTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::numOfTab);
				// アクティブだったタブ
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTabColor, kPassiveTabColor);
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTextColor, kPassiveTextColor);
				SheetCtl::back_->refresh(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab);
				// ページ表示
				SheetCtl::window_[SheetCtl::numOfTab] = new Sheet(SheetCtl::scrnx_ - SheetCtl::back_->bxsize, SheetCtl::scrny_, false);
				SheetCtl::drawRect(SheetCtl::window_[SheetCtl::numOfTab], 0, 0, 0, SheetCtl::window_[SheetCtl::numOfTab]->bxsize, SheetCtl::window_[SheetCtl::numOfTab]->bysize);
				SheetCtl::fillRect(SheetCtl::window_[SheetCtl::numOfTab], Rgb(255, 255, 255), 1, 1, SheetCtl::window_[SheetCtl::numOfTab]->bxsize - 1, SheetCtl::window_[SheetCtl::numOfTab]->bysize - 1);
				SheetCtl::window_[SheetCtl::numOfTab]->slide(SheetCtl::back_->bxsize, 0);
				// レンダリング
				string source(reinterpret_cast<char *>(htmlFile->read()), htmlFile->size());
				HTML::Tokenizer tokenizer;
				Queue<shared_ptr<HTML::Token>> &tokens = tokenizer.tokenize(source.c_str());
				for (int i = 0; !tokens.isempty(); ++i) {
					string str;
					shared_ptr<HTML::Token> token(tokens.pop());
					switch (token->type) {
						case HTML::Token::Type::Character:
							str = "Character Token";
							break;
							
						case HTML::Token::Type::StartTag:
							str = "StartTag Token";
							break;
							
						case HTML::Token::Type::EndTag:
							str = "EndTag Token";
							break;
							
						case HTML::Token::Type::DOCTYPE:
							str = "DOCTYPE Token";
							break;
							
						case HTML::Token::Type::Comment:
							str = "Comment Token";
							break;
							
						case HTML::Token::Type::EndOfFile:
							str = "EndOfFile Token";
							break;
					}
					str += " (data='" + token->data + "')";
					SheetCtl::drawString(SheetCtl::window_[SheetCtl::numOfTab], 1, 1 + i * 16, 0, str.c_str());
				}
				SheetCtl::window_[SheetCtl::activeTab]->upDown(-1);
				SheetCtl::window_[SheetCtl::numOfTab]->upDown(1);
				SheetCtl::activeTab = SheetCtl::numOfTab;
				++SheetCtl::numOfTab;
			} else {
				// 一致するファイルなし
				filename = "file:///" + filename;
				// タブ表示
				SheetCtl::drawString(SheetCtl::back_, 6, 39 + 23 * SheetCtl::numOfTab, kActiveTextColor, filename.c_str());
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::numOfTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::numOfTab, kBackgroundColor, kActiveTabColor);
				SheetCtl::back_->refresh(2, 35 + 23 * SheetCtl::numOfTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::numOfTab);
				// アクティブだったタブ
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTabColor, kPassiveTabColor);
				SheetCtl::colorChange(*SheetCtl::back_, 2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab, kActiveTextColor, kPassiveTextColor);
				SheetCtl::back_->refresh(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back_->bxsize, 33 + 16 + 8 + 23 * SheetCtl::activeTab);
				// ページ表示
				SheetCtl::window_[SheetCtl::numOfTab] = new Sheet(SheetCtl::scrnx_ - SheetCtl::back_->bxsize, SheetCtl::scrny_, false);
				SheetCtl::drawRect(SheetCtl::window_[SheetCtl::numOfTab], 0, 0, 0, SheetCtl::window_[SheetCtl::numOfTab]->bxsize, SheetCtl::window_[SheetCtl::numOfTab]->bysize);
				SheetCtl::fillRect(SheetCtl::window_[SheetCtl::numOfTab], Rgb(255, 255, 255), 1, 1, SheetCtl::window_[SheetCtl::numOfTab]->bxsize - 1, SheetCtl::window_[SheetCtl::numOfTab]->bysize - 1);
				SheetCtl::window_[SheetCtl::numOfTab]->slide(SheetCtl::back_->bxsize, 0);
				// レンダリング
				SheetCtl::drawString(SheetCtl::window_[SheetCtl::numOfTab], 1, 1, 0, "File not found");
				SheetCtl::drawString(SheetCtl::window_[SheetCtl::numOfTab], 1, 1 + 16, 0,  "Can't find the file at '" + filename + "'");
				SheetCtl::window_[SheetCtl::activeTab]->upDown(-1);
				SheetCtl::window_[SheetCtl::numOfTab]->upDown(1);
				SheetCtl::activeTab = SheetCtl::numOfTab;
				++SheetCtl::numOfTab;
			}
			
			*SheetCtl::tbox_str_ = "";
			SheetCtl::tbox_cpos_ = 2;
			SheetCtl::fillRect(SheetCtl::back_, Rgb(255, 255, 255), 2, SheetCtl::back_->bysize - 20 - 22, SheetCtl::back_->bxsize - 2, SheetCtl::back_->bysize - 20);
			SheetCtl::back_->refresh(2, SheetCtl::back_->bysize - 20 - 22, SheetCtl::back_->bxsize - 2, SheetCtl::back_->bysize - 20);
			break;
		}

		case 0x3a: // Caps Lock
			leds_ ^= 4;
			cmd_->push(kKeyCmdLED);
			cmd_->push(leds_);
			break;

		case 0x45: // Num Lock
			leds_ ^= 2;
			cmd_->push(kKeyCmdLED);
			cmd_->push(leds_);
			break;

		case 0x46: // Scroll Lock
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
				SheetCtl::fillRect(SheetCtl::back_, Rgb(255, 255, 255), SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ + 2 + 8 + 1, SheetCtl::back_->bysize - 20 - 1);
				SheetCtl::tbox_col_ = 0;
				SheetCtl::drawLine(SheetCtl::back_, SheetCtl::tbox_col_, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 2);
				SheetCtl::back_->refresh(SheetCtl::tbox_cpos_ + 2, SheetCtl::back_->bysize - 20 - 22 + 2, SheetCtl::tbox_cpos_ + 2 + 8 + 1, SheetCtl::back_->bysize - 20 - 2);
				SheetCtl::tbox_timer_->cancel();
				SheetCtl::tbox_timer_->set(50);
				SheetCtl::tbox_str_->erase(SheetCtl::tbox_str_->length() - 1, SheetCtl::tbox_str_->length());
			}
			break;
	}
}

void KeyboardController::wait() {
	while (Input8(kPortKeyStatus) & kKeyStatusSendNotReady) {}
}
