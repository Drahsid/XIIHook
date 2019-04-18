#pragma once

#ifndef UTILITY_H
#define UTILITY_H



struct gameVars
{
	double cTime;
	volatile double* targetFrameTime, realFrameTime;
	volatile float* timeScale, framerateCoef, worldTime, inGameMultiplier, inGameMouseMultiplier;
	uint8_t gameStateEnum, igmState, lastigm, focusState, lastFocusState;
	volatile uint8_t* titleState, uiState, cMenState, inMovie, inCutscene;
	Interp::Interp igmInterp;
	UserConfig uConfig;

	gameVars()
	{
		double* targetFrameTime = reinterpret_cast<double*>(minFrameTimePtr);
		double* realFrametime = reinterpret_cast<double*>(realFrameTimePtr);
		float* timeScale = reinterpret_cast<float*>(timeScalePtr);
		float* framerateCoef = reinterpret_cast<float*>(framerateCoefPtr);
		float* worldTime = reinterpret_cast<float*>(worldMPtr);
		float* inGameMultiplier = reinterpret_cast<float*>(igmPtr);
		float* inGameMouseMultiplier = reinterpret_cast<float*>(mouseCoefPtr);
		uint8_t* titleState = reinterpret_cast<uint8_t*>(inTitlePtr);
		uint8_t* uiState = reinterpret_cast<uint8_t*>(uiEnabledPtr);
		uint8_t* cMenState = reinterpret_cast<uint8_t*>(inCombatMenPtr);
		uint8_t* inMovie = reinterpret_cast<uint8_t*>(inMoviePtr);

		uConfig = UserConfig();
		Config::UpdateUserConfig(uConfig);
		igmInterp = Interp::Interp();
		igmInterp.setType(uConfig.easingType);
		igmInterp.position = 1; igmInterp.position0 = 1; igmInterp.target = 1;
	}
};





// Shorthand for the method; inline for optimization
__forceinline HWND findXII() 
{
	return FindWindow(0, L"FINAL FANTASY XII THE ZODIAC AGE");
}



// Inline for optimization
__forceinline void writeMinFrametime(HANDLE&hProcess, double&targetFrameTime, float framerateCoef = -1)
{
	DWORD protection;
	VirtualProtectEx(hProcess, (LPVOID)minFrameTimePtr, sizeof(double), PAGE_READWRITE, &protection);
	VirtualProtectEx(hProcess, (LPVOID)minFrameTime1Ptr, sizeof(double), PAGE_READWRITE, &protection);

	WriteProcessMemory(hProcess, (LPVOID)minFrameTimePtr, &targetFrameTime, sizeof(double), NULL);
	WriteProcessMemory(hProcess, (LPVOID)minFrameTime1Ptr, &targetFrameTime, sizeof(double), NULL);
	if (framerateCoef == -1) {
		float framerateCoef = 30 / (1 / targetFrameTime);
	}
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
	case 10:
		printf("animRateFix\n");
	}
};


__forceinline void tick_interp(gameVars& gVars)
{
	float nTime = (float)gVars.cTime;

	if (!gVars.gameStateEnum == 1 && !gVars.inCutscene == 1 && gVars.lastigm != gVars.igmState)
	{
		float base0 = (gVars.lastigm == 0 ? gVars.uConfig.igmState0Override
			: (gVars.lastigm == 1 ? gVars.uConfig.igmState1Override : gVars.uConfig.igmState2Override));
		float base = (gVars.igmState == 0 ? gVars.uConfig.igmState0Override
			: (gVars.igmState == 1 ? gVars.uConfig.igmState1Override : gVars.uConfig.igmState2Override));

		gVars.igmInterp.position = base0;
		gVars.igmInterp.position0 = base0;
		gVars.igmInterp.smoothTime = gVars.uConfig.easingTime;
		gVars.igmInterp.target = base;
		gVars.igmInterp.time0 = nTime;

		printf("Starting igm interp: %f -> %f\n", base0, base);
	}
	else if (!gVars.gameStateEnum == 1 && !gVars.inCutscene == 1)
	{
		float base = 1;
		switch (gVars.uConfig.bEnableEasing)
		{
		case true:
			base = gVars.igmInterp.interp(nTime);

			if (nTime > gVars.igmInterp.time1) base = (gVars.igmState == 0 ? gVars.uConfig.igmState0Override
				: (gVars.igmState == 1 ? gVars.uConfig.igmState1Override : gVars.uConfig.igmState2Override));
			if (base != base || base == 0) base = 1; //If NaN set to 1
			break;
		case false:
			base = (gVars.igmState == 0 ? gVars.uConfig.igmState0Override
				: (gVars.igmState == 1 ? gVars.uConfig.igmState1Override : gVars.uConfig.igmState2Override));
			break;
		}

		gVars.inGameMultiplier = base;
	}
	else gVars.inGameMultiplier = 1; //Attempt to fix menu and loading slowdowns, as well as cutscene scaling
}


__forceinline void tickf(HANDLE & hProcess, gameVars & gVars)
{
	tick_interp(gVars);

	uint8_t wtChanged = 0;
	if (gVars.worldTime > gVars.inGameMultiplier * 2)
	{
		gVars.worldTime = gVars.inGameMultiplier;
		wtChanged = 1;
	}

	gVars.lastigm = gVars.igmState;
}


__forceinline void updateGState(gameVars & gVars) {
	gVars.gameStateEnum =
		gVars.inCutscene == 1
		? 4 : gVars.inMovie == 1 && gVars.uiState == 0
		? 3 : gVars.focusState == 1
		? 2 : (gVars.uiState == 1 || gVars.cMenState == 1) //(uiState == 1 || titleState == 1 || cMenState == 1) TITLESTATE BREAKS REKS (Must find a more accurate value)
		? 1
		: 0;
}

/*
DETOURED METHODS
*/

__forceinline void setAnimationRateCustom(HINSTANCE* myHInst, std::vector<float*>& animRates)
{
	float* animDummy;
	bool dontWrite = false;

	ReadProcessMemory(myHInst, (LPVOID)animDummyPtr, &animDummy, sizeof(float), NULL);

	for (int i = 0; i < animRates.size(); i++)
		if (animDummy == animRates[i]) dontWrite = true;

	if (!dontWrite)
		animRates.push_back(animDummy);
}


__forceinline void Step(HANDLE& hProcess, gameVars& gVars) {
	updateGState(gVars);
	tickf(hProcess, gVars);
}




#endif



