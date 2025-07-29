#pragma once
#include <SmartPointer.h>

class ModalWindow {
private:
	unique_ptr<Sheet> sheet;
	string message;

public:
	ModalWindow(const string &msg);
};
