#pragma once

template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T &val) {
	while (first != last) {
		if (*first == val) break;
		++first;
	}
	return first;
}