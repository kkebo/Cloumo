/*
 * ブラウザー
 */

#ifndef _BROWSER_H_
#define _BROWSER_H_

class Browser {
private:
	File *htmlFile;
	unsigned char *source;
	Sheet *sheet;
	unsigned int size;
	int dy;

public:
	Browser(const char *);
	~Browser();
	void Render();
	void Scroll(int);
	void Mapping();
};

#endif
