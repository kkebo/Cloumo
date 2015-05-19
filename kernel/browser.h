/*
 * ブラウザー
 */

#ifndef _BROWSER_H_
#define _BROWSER_H_

#include <SmartPointer.h>

class Browser {
private:
	unique_ptr<File> htmlFile;
	unsigned char *source;
	unique_ptr<Sheet> sheet;
	unsigned int size;
	int dy;

public:
	Browser(const char *);
	void Render();
	void Scroll(int);
	void Mapping();
};

#endif
