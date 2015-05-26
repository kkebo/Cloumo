/*
 * ブラウザー
 */

#pragma once

#include <SmartPointer.h>

class Browser {
private:
	shared_ptr<unsigned char> source;
	unique_ptr<Sheet> sheet;
	unsigned int size;
	int dy;

public:
	Browser(const char *url);
	void Render();
	void Scroll(int data);
	void Mapping();
};
