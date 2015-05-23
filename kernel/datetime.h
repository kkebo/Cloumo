/*
 * 時刻
 */

#ifndef _DATETIME_H_
#define _DATETIME_H_

class Timer;

class DateTime {
private:
	static unsigned char now[];
	static Timer *timer;

public:
	static void Main();
};

#endif
