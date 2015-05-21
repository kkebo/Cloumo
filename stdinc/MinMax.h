#ifndef _MINMAX_H_
#define _MINMAX_H_

template <typename T> constexpr T min(T a, T b) { return a < b ? a : b; }
template <typename T> constexpr T max(T a, T b) { return a > b ? a : b; }

#endif