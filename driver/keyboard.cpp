#include "../headers.h"
#include "../kernel/HTMLTokenizer.h"
#include "../kernel/HTMLTreeConstructor.h"

unsigned char KeyboardController::asciiTable[0x80] = {
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0/*0x08*/, 0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '@', '[', 0/*0x0a*/, 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', ':', 0,   0,   ']', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
};
unsigned char KeyboardController::asciiShiftTable[0x80] = {
	0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0/*0x08*/, 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0/*0x0a*/, 0, 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
};
int KeyboardController::shift = 0;
bool KeyboardController::alt = false;
int KeyboardController::cmdWait = -1;
int KeyboardController::leds = 0;
Queue<int> *KeyboardController::cmd;
TaskQueue *KeyboardController::queue;

void KeyboardController::Main() {
	Task *task = TaskSwitcher::getNowTask();
	
	// キーボード初期化
	wait();
	Output8(kPortKeyCmd, kKeyCmdWriteMode);
	wait();
	Output8(kPortKeyData, kKBCMode);
	
	// メンバ変数初期化
	cmd = new Queue<int>(32);
	BootInfo *binfo = (BootInfo *)ADDRESS_BOOTINFO;
	leds = (binfo->leds >> 4) & 7;
	queue = task->queue;
	
	// キャレットの表示とタイマー設定
	SheetCtl::back->drawLine(
		Line(
			SheetCtl::caretPosition + 2,
			SheetCtl::back->frame.size.height - 20 - 22 + 2,
			SheetCtl::caretPosition + 2,
			SheetCtl::back->frame.size.height - 20 - 22 + 2 + 18
		), SheetCtl::caretColor);
	SheetCtl::back->refresh(Rectangle(SheetCtl::caretPosition + 2, SheetCtl::back->frame.size.height - 20 - 22 + 2, 1, 18));
	SheetCtl::caretColor = 0xffffff;
	SheetCtl::caretTimer = new Timer(queue, 256);
	SheetCtl::caretTimer->set(50);
	
	for (;;) {
		if (!cmd->isempty() && cmdWait < 0) {
			cmdWait = cmd->pop();
			wait();
			Output8(kPortKeyData, cmdWait);
		}
		Cli();
		if (queue->isempty()) {
			task->sleep();
			Sti();
		} else {
			int code = queue->pop();
			Sti();
			if (code == SheetCtl::caretTimer->data) {
				SheetCtl::back->drawLine(
					Line(
						SheetCtl::caretPosition + 2,
						SheetCtl::back->frame.size.height - 20 - 22 + 2,
						SheetCtl::caretPosition + 2,
						SheetCtl::back->frame.size.height - 20 - 22 + 2 + 18
					), SheetCtl::caretColor);
				SheetCtl::back->refresh(Rectangle(SheetCtl::caretPosition + 2, SheetCtl::back->frame.size.height - 20 - 22 + 2, 1, 18));
				SheetCtl::caretColor ^= 0xffffff;
				SheetCtl::caretTimer->set(50);
			} else if (code < 256) {
				Decode(static_cast<unsigned char>(code));
			}
		}
	}
}

void KeyboardController::Decode(unsigned char code) {
	if (code < 0x80 && asciiTable[code]) {
		char s[2];
		s[0] = ((shift && !(leds & 4)) || (!shift && leds & 4)) ? asciiShiftTable[code] : asciiTable[code];
		s[1] = 0;
		SheetCtl::back->drawLine(
			Line(
				SheetCtl::caretPosition + 2,
				SheetCtl::back->frame.size.height - 20 - 22 + 2,
				SheetCtl::caretPosition + 2,
				SheetCtl::back->frame.size.height - 20 - 22 + 2 + 18
			), 0xffffff);
		SheetCtl::back->drawString(s, Point(SheetCtl::caretPosition + 2, SheetCtl::back->frame.size.height - 20 - 22 + 3), 0);
		SheetCtl::caretPosition += 8;
		SheetCtl::caretColor = 0;
		SheetCtl::back->drawLine(
			Line(
				SheetCtl::caretPosition + 2,
				SheetCtl::back->frame.size.height - 20 - 22 + 2,
				SheetCtl::caretPosition + 2,
				SheetCtl::back->frame.size.height - 20 - 22 + 2 + 18
			), SheetCtl::caretColor);
		SheetCtl::back->refresh(Rectangle(SheetCtl::caretPosition - 8 + 2, SheetCtl::back->frame.size.height - 20 - 22 + 2, 9, 18));
		SheetCtl::caretTimer->cancel();
		SheetCtl::caretTimer->set(50);
		*SheetCtl::tboxString += s[0];
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
				Rectangle nextTabRange(2, 35 + 23 * newActive, SheetCtl::back->frame.size.width - 2, 22);
				SheetCtl::back->changeColor(nextTabRange, kPassiveTabColor, kActiveTabColor);
				SheetCtl::back->changeColor(nextTabRange, kPassiveTextColor, kActiveTextColor);
				SheetCtl::back->refresh(nextTabRange);
				// アクティブだったタブ
				Rectangle prevTabRange(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.size.width - 2, 22);
				SheetCtl::back->changeColor(prevTabRange, kActiveTabColor, kPassiveTabColor);
				SheetCtl::back->changeColor(prevTabRange, kActiveTextColor, kPassiveTextColor);
				SheetCtl::back->refresh(prevTabRange);
				
				SheetCtl::window[SheetCtl::activeTab]->upDown(-1);
				SheetCtl::window[newActive]->upDown(1);
				
				SheetCtl::activeTab = newActive;
			}
			break;
		
		case 0x1c: { // Enter pressed
			if (SheetCtl::tboxString->length() == 0) break;
			
			string filename = *SheetCtl::tboxString;
			
			// file:/// の削除
			if (filename.compare(0, 8, "file:///") == 0) {
				filename.erase(0, 8);
			}
			
			unique_ptr<File> htmlFile(new File(filename));
			if (htmlFile->open()) {
				// ファイルが存在した
				filename = "file:///" + filename;
				// タブ表示
				Rectangle newTabRange(2, 35 + 23 * SheetCtl::numOfTab, SheetCtl::back->frame.size.width - 2, 22);
				SheetCtl::back->drawString(filename, Point(6, 39 + 23 * SheetCtl::numOfTab), kActiveTextColor);
				SheetCtl::back->changeColor(newTabRange, kBackgroundColor, kActiveTabColor);
				SheetCtl::back->refresh(newTabRange);
				// アクティブだったタブ
				Rectangle prevTabRange(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.size.width - 2, 22);
				SheetCtl::back->changeColor(prevTabRange, kActiveTabColor, kPassiveTabColor);
				SheetCtl::back->changeColor(prevTabRange, kActiveTextColor, kPassiveTextColor);
				SheetCtl::back->refresh(prevTabRange);
				// ページ表示
				SheetCtl::window[SheetCtl::numOfTab] = new Sheet(Size(SheetCtl::resolution.width - SheetCtl::back->frame.size.width, SheetCtl::resolution.height), false);
				SheetCtl::window[SheetCtl::numOfTab]->fillRect(SheetCtl::window[SheetCtl::numOfTab]->frame, 0xffffff);
				SheetCtl::window[SheetCtl::numOfTab]->drawRect(SheetCtl::window[SheetCtl::numOfTab]->frame, 0);
				SheetCtl::window[SheetCtl::numOfTab]->moveTo(Point(SheetCtl::back->frame.size.width, 0));
				// レンダリング
				string source(reinterpret_cast<char *>(htmlFile->read().get()), htmlFile->size);
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
					SheetCtl::window[SheetCtl::numOfTab]->drawString(str, Point(1, 1 + i * 16), 0);
				}
				SheetCtl::window[SheetCtl::activeTab]->upDown(-1);
				SheetCtl::window[SheetCtl::numOfTab]->upDown(1);
				SheetCtl::activeTab = SheetCtl::numOfTab;
				++SheetCtl::numOfTab;
			} else {
				// 一致するファイルなし
				filename = "file:///" + filename;
				// タブ表示
				Rectangle newTabRange(2, 35 + 23 * SheetCtl::numOfTab, SheetCtl::back->frame.size.width - 2, 22);
				SheetCtl::back->drawString(filename, Point(6, 39 + 23 * SheetCtl::numOfTab), kActiveTextColor);
				SheetCtl::back->changeColor(newTabRange, kBackgroundColor, kActiveTabColor);
				SheetCtl::back->refresh(newTabRange);
				// アクティブだったタブ
				Rectangle prevTabRange(2, 35 + 23 * SheetCtl::activeTab, SheetCtl::back->frame.size.width - 2, 22);
				SheetCtl::back->changeColor(prevTabRange, kActiveTabColor, kPassiveTabColor);
				SheetCtl::back->changeColor(prevTabRange, kActiveTextColor, kPassiveTextColor);
				SheetCtl::back->refresh(prevTabRange);
				// ページ表示
				SheetCtl::window[SheetCtl::numOfTab] = new Sheet(Size(SheetCtl::resolution.width - SheetCtl::back->frame.size.width, SheetCtl::resolution.height), false);
				SheetCtl::window[SheetCtl::numOfTab]->fillRect(SheetCtl::window[SheetCtl::numOfTab]->frame, 0xffffff);
				SheetCtl::window[SheetCtl::numOfTab]->drawRect(SheetCtl::window[SheetCtl::numOfTab]->frame, 0);
				SheetCtl::window[SheetCtl::numOfTab]->moveTo(Point(SheetCtl::back->frame.size.width, 0));
				// レンダリング
				SheetCtl::window[SheetCtl::numOfTab]->drawString("File not found", Point(1, 1), 0);
				SheetCtl::window[SheetCtl::numOfTab]->drawString("Can't find the file at '" + filename + "'", Point(1, 1 + 16), 0);
				SheetCtl::window[SheetCtl::activeTab]->upDown(-1);
				SheetCtl::window[SheetCtl::numOfTab]->upDown(1);
				SheetCtl::activeTab = SheetCtl::numOfTab;
				++SheetCtl::numOfTab;
			}
			
			*SheetCtl::tboxString = "";
			SheetCtl::caretPosition = 2;
			Rectangle clearRange(2, SheetCtl::back->frame.size.height - 20 - 22, SheetCtl::back->frame.size.width - 2 - 2, 22);
			SheetCtl::back->fillRect(clearRange, 0xffffff);
			SheetCtl::back->refresh(clearRange);
			break;
		}

		case 0x3a: // Caps Lock
			leds ^= 4;
			cmd->push(kKeyCmdLED);
			cmd->push(leds);
			break;

		case 0x45: // Num Lock
			leds ^= 2;
			cmd->push(kKeyCmdLED);
			cmd->push(leds);
			break;

		case 0x46: // Scroll Lock
			leds ^= 1;
			cmd->push(kKeyCmdLED);
			cmd->push(leds);
			break;

		case 0xfa:
			cmdWait= -1;
			break;

		case 0xfe:
			wait();
			Output8(kPortKeyData, cmdWait);
			break;

		case 0x2a:
			shift |= 1;
			break;

		case 0x36:
			shift |= 2;
			break;

		case 0xaa:
			shift &= ~1;
			break;

		case 0xb6:
			shift &= ~2;
			break;

		case 0x0e: // Backspace
			if (SheetCtl::caretPosition > 2) {
				SheetCtl::caretPosition -= 8;
				Rectangle clearRange(SheetCtl::caretPosition + 2, SheetCtl::back->frame.size.height - 20 - 22 + 2, 9, 18);
				SheetCtl::back->fillRect(clearRange, 0xffffff);
				SheetCtl::caretColor = 0;
				SheetCtl::back->drawLine(Line(clearRange.offset, clearRange.offset + Point(0, 18)), SheetCtl::caretColor);
				SheetCtl::back->refresh(clearRange);
				SheetCtl::caretTimer->cancel();
				SheetCtl::caretTimer->set(50);
				SheetCtl::tboxString->erase(SheetCtl::tboxString->length() - 1, SheetCtl::tboxString->length());
			}
			break;
	}
}

void KeyboardController::wait() {
	while (Input8(kPortKeyStatus) & kKeyStatusSendNotReady) {}
}
