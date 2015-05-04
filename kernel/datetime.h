/*
 * 時刻
 */

#ifndef _DATETIME_H_
#define _DATETIME_H_

class Timer;

class DateTime {
private:
	static unsigned char t_[];
	static Timer *timer_;

public:
	static void init();
};

#endif
