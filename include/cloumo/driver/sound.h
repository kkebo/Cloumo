/*
 * サウンド
 */

#pragma once

#include "../kernel/timer.h"

void Beep(char c, int len, Timer *timer, bool st = false);
