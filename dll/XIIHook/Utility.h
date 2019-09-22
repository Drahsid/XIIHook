#pragma once

#ifndef UTILITY_H
#define UTILITY_H

#include "pch.h"

__forceinline void print(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

__forceinline void setConTAttribute(HANDLE h, WORD w, const char *fmt, ...)
{
	SetConsoleTextAttribute(h, w);
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

__forceinline float& clamp(float lhs, float upper, float lower)
{
	return min(upper, max(lhs, lower));
}

#endif



