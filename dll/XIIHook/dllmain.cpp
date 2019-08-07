#include "pch.h"
#include "../../out/detours/include/detours.h"
#include "../../Globals.h"
#include "Interp.h"
#include "Config.h"
#include "Utility.h"

namespace
{
	HINSTANCE* myHInst;
	HANDLE myHandle;
	static gameVars gVars;
	static std::vector<volatile float*> animRates;
	bool breakStep = false;
	DWORD threadID;
	HANDLE asyncHandle;
}

DWORD WINAPI asyncThread(LPVOID lpParameter) {
	for (;;) {
		Step(gVars, animRates);
		if (breakStep) break;
	}

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID reserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		AllocConsole();
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);

		gVars = gameVars();

		myHInst = &hInst;
		myHandle = GetCurrentProcess();
		asyncHandle = CreateThread(NULL , NULL, asyncThread, NULL, NULL, &threadID);
		
		break;

	case DLL_PROCESS_DETACH:
		breakStep = true;
		FreeConsole();
		CloseHandle(asyncHandle);
		break;

	}

	return TRUE;
}



