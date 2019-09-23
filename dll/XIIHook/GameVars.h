#pragma once

#ifndef GAMEVARS_H
#define GAMEVARS_H

#include "pch.h"
#include "Globals.h"
#include "Interpolator.h"
#include "Config.h"
#include "Input.h"

struct gameVars
{
	double cTime = 0;
	volatile double* targetFrameTime = (double*)minFrameTimePtr;
	volatile double* targetFrameTime1 = (double*)minFrameTime1Ptr;
	volatile double* realFrameTime = (double*)realFrameTimePtr;
	volatile float* timeScale = (float*)timeScalePtr;
	volatile float* framerateCoef = (float*)framerateCoefPtr;
	volatile float* invFramerateCoef = (float*)invFramerateCoefPtr;
	volatile float* worldTime = (float*)worldMPtr;
	volatile float* inGameMultiplier = (float*)igmPtr;
	volatile float* inGameMouseMultiplier = (float*)mouseCoefPtr;
	volatile float* gamma = (float*)gammaPtr;
	volatile float* fov = (float*)fovPtr;
	volatile float* aoeActionDummy = (float*)actionAoeFixDummyPtr;

	volatile uint8_t* ctrlEnabled = (uint8_t*)ctrlEnabledPtr;
	volatile uint8_t* titleState = (uint8_t*)inTitlePtr;
	volatile uint8_t* uiState = (uint8_t*)uiEnabledPtr;
	volatile uint8_t* cMenState = (uint8_t*)inCombatMenPtr;
	volatile uint8_t* inMovieState = (uint8_t*)inMoviePtr;
	volatile uint8_t* inCutscene = (uint8_t*)inCScenePtr;
	volatile uint8_t* igmState = (uint8_t*)igmStatePtr;
	volatile uint8_t* hudDisabled = (uint8_t*)hudDisabledPtr;
	volatile uint8_t* freeCamEnabled = (uint8_t*)freeCamEnabledPtr;

	volatile Vector3f* cameraPosition = (Vector3f*)cameraPositionPtr;
	volatile Vector3f* cameraLookAtPoint = (Vector3f*)cameraLookAtPointPtr;

	HWND FFXIIWND = NULL;

	volatile bPa1a2 hideHUD = (bPa1a2)0x0253CE0; //Parent call of below
	volatile voidPn hideBillboards = (voidPn)0x028F720; //Hides some billboards such as the health above the player and enemie's heads
	volatile voidPa1 hideBillboards2 = (voidPa1)0x02517E0; //Hides some billboards as well; come back when this is NOT being called
	volatile voidPn hidePartyMenus = (voidPn)0x035C730; //Causes word to NOT unpause after PC tries to open a party menu

	volatile voidPd1a2 writeFrametime = (voidPd1a2)0x03420130; //Crashes when we call it manually; used to write minimum frametime
	volatile voidPa1s2 gamePrint = (voidPa1s2)0x05CC020; //Doesn't out to any standard streams; must go somewhere (Game uses it for internal errors)
	volatile voidPa1d2 frametimeMethod0 = (voidPa1d2)0x0621B30;
	volatile voidPd1 frametimeMethod1 = (voidPd1)0x01E0850;

	uint8_t gameStateEnum = 0, lastInGameMultiState = 0, focusState = 0, lastFocusState = 0, lastUseMenuLimitState = 0;

	Interpolator igmInterp = Interpolator();
	UserConfig uConfig = UserConfig();
	InputManager IM = InputManager();

	gameVars()
	{
		Config::UpdateUserConfig(uConfig);
		igmInterp.setType(uConfig.easingType);
		igmInterp.currentPosition = 1; igmInterp.initialPosition = 1; igmInterp.targetPosition = 1;
		focusState = 0; lastFocusState = 0; cTime = 0; gameStateEnum = 0; lastInGameMultiState = 0; lastUseMenuLimitState = 0;

		print("Normalizing config...\n");
		*framerateCoef = 30.f / uConfig.requestedMinFrametime;
		*invFramerateCoef = 1.f / *framerateCoef;
		uConfig.requestedMinFrametime = 1.f / uConfig.requestedMinFrametime;
		uConfig.requestedMinFrametimeMenus = 1.f / uConfig.requestedMinFrametimeMenus;
		uConfig.requestedMinFrametimeNoFocus = 1.f / uConfig.requestedMinFrametimeNoFocus;
		uConfig.requestedMinFrametimeMovies = 1.f / uConfig.requestedMinFrametimeMovies;
		//uConfig.fov = uConfig.fov * RAD2DEG;
		print("Config done... \n");

		/**gamma = uConfig.gamma;
		*fov = uConfig.fov;*/
	}

};


#endif



