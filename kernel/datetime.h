/*
 * 時刻
 */

#pragma once

class Timer;

class DateTime {
private:
	static unsigned char now[];
	static Timer *timer;

public:
	static void Main();
};
