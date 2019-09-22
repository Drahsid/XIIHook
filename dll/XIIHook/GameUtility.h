#pragma once

#ifndef GAMEUTILITY_H
#define GAMEUTILITY_H

#include "pch.h"
#include "Globals.h"
#include "Interpolator.h"
#include "Config.h"
#include "Input.h"
#include "GameVars.h"

__forceinline float getIGM(gameVars& gVars, bool current) {
	switch (current) {
	case true:
		return	*gVars.igmState == 0 ? gVars.uConfig.igmState0Override
			: *gVars.igmState == 1 ? gVars.uConfig.igmState1Override
			: gVars.uConfig.igmState2Override;
		break;

	case false:
		return	gVars.lastInGameMultiState == 0 ? gVars.uConfig.igmState0Override
			: gVars.lastInGameMultiState == 1 ? gVars.uConfig.igmState1Override
			: gVars.uConfig.igmState2Override;
		break;
	}
}

__forceinline void interpolatorStep(gameVars& gVars)
{
	float nTime = (float)gVars.cTime;

	if (!gVars.gameStateEnum == 1 && !*gVars.inCutscene == 1 && gVars.lastInGameMultiState != *gVars.igmState)
	{
		float base0 = getIGM(gVars, false);
		float base = getIGM(gVars, true);

		gVars.igmInterp.currentPosition = base0;
		gVars.igmInterp.initialPosition = base0;
		gVars.igmInterp.smoothingTime = gVars.uConfig.easingTime;
		gVars.igmInterp.targetPosition = base;
		gVars.igmInterp.initialTime = nTime;

		print("Starting igm interp: %f -> %f\n", base0, base);
	}
	else if (!gVars.gameStateEnum == 1 && !*gVars.inCutscene == 1)
	{
		float base = 1;
		switch (gVars.uConfig.bEnableEasing)
		{
		case true:
			base = gVars.igmInterp.interpolate(nTime);

			if (nTime > gVars.igmInterp.endTime) base = getIGM(gVars, true);
			if (base != base || base == 0) base = 1; //If NaN set to 1
			break;
		case false:
			base = getIGM(gVars, true);
			break;
		}

		*gVars.inGameMultiplier = base;
	}
	else *gVars.inGameMultiplier = 1; //Attempt to fix menu and loading slowdowns, as well as cutscene scaling
}


__forceinline void tickf(gameVars& gVars)
{
	interpolatorStep(gVars);

	if (*gVars.worldTime > *gVars.inGameMultiplier * 2)
		*gVars.worldTime = *gVars.inGameMultiplier;

	gVars.lastInGameMultiState = *gVars.igmState;
}


__forceinline void updateGState(gameVars& gVars) {
	uint8_t inCutscene, uiState, cMenState, inMovieState;

	inCutscene = *gVars.inCutscene;
	uiState = *gVars.uiState;
	cMenState = *gVars.cMenState;
	inMovieState = *gVars.inMovieState;

	gVars.focusState = gVars.FFXIIWND == GetForegroundWindow() ? 0 : 1;

	gVars.gameStateEnum
		= inCutscene == 1
		? 4 : inMovieState == 1 && uiState == 0
		? 3 : gVars.focusState == 1
		? 2 : (uiState == 1 || cMenState == 1) //TITLESTATE BREAKS REKS (Must find a more accurate value)
		? 1
		: 0;
}

__forceinline void updateFPSCoef(gameVars& gVars)
{
	switch (gVars.gameStateEnum)
	{
	case 0:
		//printf("User has exited a menu!\n");
		*gVars.targetFrameTime = gVars.uConfig.requestedMinFramerate;
		*gVars.targetFrameTime1 = gVars.uConfig.requestedMinFramerate;
		break;
	case 1:
		//printf("User has entered a menu!\n");
		*gVars.targetFrameTime = gVars.uConfig.requestedMinFramerateMenus;
		*gVars.targetFrameTime1 = gVars.uConfig.requestedMinFramerateMenus;
		break;
	case 2:
		//printf("Game window has lost focus!\n");
		*gVars.targetFrameTime = gVars.uConfig.requestedMinFramerateNoFocus;
		*gVars.targetFrameTime1 = gVars.uConfig.requestedMinFramerateNoFocus;
		break;
	case 3:
		//printf("User has entered a movie!\n");
		*gVars.targetFrameTime = gVars.uConfig.requestedMinFramerateMovies;
		*gVars.targetFrameTime1 = gVars.uConfig.requestedMinFramerateMovies;
		break;
	case 4:
		//printf("The user is in a cutscene; reducing framerate!\n");
		*gVars.targetFrameTime = gVars.uConfig.requestedMinFramerateMovies;
		*gVars.targetFrameTime1 = gVars.uConfig.requestedMinFramerateMovies;
		break;
	}
	gVars.lastUseMenuLimitState = gVars.gameStateEnum;
}

__forceinline void updateMouse(gameVars& gVars) {
	*gVars.inGameMouseMultiplier = gVars.uConfig.bEnableAdaptiveMouse ?
		(gVars.uConfig.lockedMouseMulti * *gVars.inGameMultiplier) / *gVars.timeScale
		: gVars.uConfig.lockedMouseMulti;
}

__forceinline void step(gameVars& gVars) {
	//Fix page permissions
	DWORD protection;
	VirtualProtect((LPVOID)minFrameTimePtr, sizeof(double), PAGE_READWRITE, &protection);
	VirtualProtect((LPVOID)minFrameTime1Ptr, sizeof(double), PAGE_READWRITE, &protection);
	VirtualProtect((LPVOID)timeScalePtr, sizeof(float), PAGE_READWRITE, &protection);
	VirtualProtect((LPVOID)framerateCoefPtr, sizeof(float), PAGE_READWRITE, &protection);
	VirtualProtect((LPVOID)worldMPtr, sizeof(float), PAGE_READWRITE, &protection);
	VirtualProtect((LPVOID)igmPtr, sizeof(float), PAGE_READWRITE, &protection);
	VirtualProtect((LPVOID)mouseCoefPtr, sizeof(float), PAGE_READWRITE, &protection);
	VirtualProtect((LPVOID)gammaPtr, sizeof(float), PAGE_READWRITE, &protection);
	VirtualProtect((LPVOID)fovPtr, sizeof(float), PAGE_READWRITE, &protection);

	updateGState(gVars);
	updateFPSCoef(gVars);
	updateMouse(gVars);
	gVars.cTime = ((double)clock() / CLOCKS_PER_SEC);
	tickf(gVars);
	PollInput(gVars.IM);
}

#endif // !GAMEUTILITY_H


