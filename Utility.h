#pragma once

#ifndef UTILITY_H
#define UTILITY_H



// Shorthand for the method; inline for optimization
__forceinline HWND findXII() 
{
	return FindWindow(0, L"FINAL FANTASY XII THE ZODIAC AGE");
}



// Inline for optimization
__forceinline void writeMinFrametime(HANDLE&hProcess, double&targetFrameTime)
{
	DWORD protection;
	VirtualProtectEx(hProcess, (LPVOID)tftPtr, sizeof(double), PAGE_READWRITE, &protection);
	VirtualProtectEx(hProcess, (LPVOID)tft1Ptr, sizeof(double), PAGE_READWRITE, &protection);

	WriteProcessMemory(hProcess, (LPVOID)tftPtr, &targetFrameTime, sizeof(double), NULL);
	WriteProcessMemory(hProcess, (LPVOID)tft1Ptr, &targetFrameTime, sizeof(double), NULL);
	float framerateCoef = 30 / (1 / targetFrameTime);
	WriteProcessMemory(hProcess, (LPVOID)framerateCoefPtr, &framerateCoef, sizeof(float), NULL);
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



void failedToErase(int x, HANDLE&hConsole)
{
	SetConTAttribute(hConsole, cc_ERROR, "Failed to erase instructions...");
	SetConTAttribute(hConsole, cc_WARN);
	switch (x)
	{
	case 0:
		printf("mouseUnlock\n");
		break;
	case 1:
		printf("mouseUnlock0\n");
		break;
	case 2:
		printf("inGameMultiUnlock\n");
		break;
	case 3:
		printf("inGameMultiUnlock0\n");
		break;
	case 4:
		printf("inGameMultiUnlock1\n");
		break;
	case 5:
		printf("inGameMultiUnlock2\n");
		break;
	case 6:
		printf("inGameMultiUnlock3\n");
		break;
	case 7:
		printf("fovUnlock\n");
		break;
	case 8:
		printf("fullSpeedUnlock\n");
		break;
	case 9:
		printf("aoefix\n");
		break;
	}
};


#endif


