#pragma once

#ifndef UTILITY_H
#define UTILITY_H

//Console colors
#define cc_NORM 15
#define cc_VERBOSE 2
#define cc_FUN 5
#define cc_ERROR 12
#define cc_WARN 14

// Shorthand for the method; inline for optimization
__forceinline HWND findXII() 
{
	return FindWindow(0, L"FINAL FANTASY XII THE ZODIAC AGE");
}


// Clean up SetConsoleTextAttribute ... printf; shorthand; inline for optimization
__forceinline void SetConTAttribute(HANDLE h, WORD w, std::string s = "", ...)
{
	SetConsoleTextAttribute(h, w);
	if (!s.empty())
	{
		va_list args;
		va_start(args, s);
		vprintf(s.c_str(), args);
		va_end(args);
	}

}

#endif



