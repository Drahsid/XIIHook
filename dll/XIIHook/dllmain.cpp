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
}

void CallStep() {
	Step(gVars, animRates);
}

void CallSetAnimRate() {
	setAnimationRate(gVars, animRates);
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID reserved)
{
	void* animRoutineAddr = (void*)setAnimationRatePtr;
	void* updateRoutineAddr = (void*)ftUpdatePtr;

	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		AllocConsole();
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);

		gVars = gameVars();

		myHInst = &hInst;
		myHandle = GetCurrentProcess();

		/*DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&animRoutineAddr, CallSetAnimRate);
		DetourTransactionCommit();*/

		for (;;) {
			CallStep();
			if (breakStep) break;
		}

		break;

	case DLL_PROCESS_DETACH:
		breakStep = true;
		/*FreeConsole();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&animRoutineAddr, CallSetAnimRate);
		DetourTransactionCommit();*/
		break;

	}

	return TRUE;
}



