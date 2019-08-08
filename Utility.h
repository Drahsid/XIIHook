#pragma once

#ifndef UTILITY_H
#define UTILITY_H

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



