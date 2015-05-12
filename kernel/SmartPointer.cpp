#include <SmartPointer.h>

void ReferenceCounter::Add() {
	count++;
}

int ReferenceCounter::Release() {
	return --count;
}

int ReferenceCounter::Count() {
	return count;
}