#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

//Game funcs
typedef bool(*bPa1a2)(int a1, int a2);
typedef void(*voidPn)();
typedef void(*voidPa1)(int a1);
typedef void(*voidPd1)(double a1);
typedef void(*voidPd1a2)(double a1, int a2);
typedef void(*voidPa1d2)(int a1, double a2);
typedef void(*voidPa1s2)(int a1, const char* a2);

//Constexprs
constexpr long double RAD2DEG = 0.0174532925199L;
constexpr long double RAD90 = 1.570796L;
constexpr long double ONE_OVER_SIXTY = 0.01666666666666667L;
constexpr long double PI = 3.14159265358979323846L;
constexpr long double TAU = 2L * PI;
constexpr long double HPI = PI / 2L;

//TODO: clean this file up

//Console colors
#define cc_NORM 15
#define cc_VERBOSE 2
#define cc_FUN 5
#define cc_ERROR 12
#define cc_WARN 14


#define NOP 0x90 //NOP instruction TODO: replace NOP strings with cleaner code using this

//Game's base addr
#define baseAddress 0x00120000

//In-game pointers
#define minFrameTimePtr 0x008FE378		//0x0090D398 //Min Frametime 1
#define minFrameTime1Ptr 0x008FF0A0		//0x0090E0C0 //Min Frametime 2 (Doesn't seem to do anything)
#define realFrameTimePtr 0x01F7C358		//0x01F97058 //Reported frametime 1
#define realFrameTime1Ptr 0x01F7D470	//0x01F98170 //Reported frametime 2 (Not to be trusted)

/*
In game multiplier
Generally becomes 1,2,4 though you can make it anything. Disabling instructions related to this can have some weird results.
*/
#define igmPtr 0x2062A88

/*
Wolrd multiplier (Eg skies and grass anims) @ < 1 this freezes the game, but there is still camera control
This is equal to the igm * the global time multiplier * (*=usually)
*/
#define worldMPtr 0x02062A84 

/*
Global time multiplier
*/
#define timeScalePtr 0x02062A8C

/*
1 at 30 fps, 0.5 at 60
Pretty much everything scales to this and the time scale
*/
#define framerateCoefPtr 0x1DFC0BC
#define invFramerateCoefPtr 0x01DFC0B8 

/*
The playback rate of animations. This is always 30.
*/
#define animationBaseRatePtr 0x008F6D80

/*
Mouse coefficient
Realative to all other multipliers; does not effect controler input
*/
#define mouseCoefPtr 0x01E00820

/*
In game gamma
*/
#define gammaPtr 0x008F1130

/*
The in game FOV. This does not effect cutscenes (They use different camera)
*/
#define fovPtr 0x02095558 //0x020B0258 

/*
Currently selected state of igm. Default 0 = 1x, 1 = 2x, 2 = 4x
*/
#define igmStatePtr 0x01FD2A44

#define uiEnabledPtr 0x01DFC0C0
#define inTitlePtr 0x2088680
#define inCombatMenPtr 0x01FD2694
#define inCScenePtr 0x2090754
#define inMoviePtr 0x215D1C2

// Credit to ffgriever for finding these
#define freeCamEnabledPtr		0x02096DFA
#define cameraPositionPtr		0x020955E0
#define cameraLookAtPointPtr	0x020955F0
#define cameraRollPtr			0x02094504
#define hudDisabledPtr			0x215D1BC

/*
This address disables all user input but leaves camera controls active
0 - disabled
1 - enabled
*/
#define ctrlEnabledPtr 0x02096E34


/*
Changing this does some weird stuff to the input
*/
#define mouseFPtr 0x01F111E0

/*
One byte, 0-6 (modulus) changes quality of shadow.
*/
#define shadowTypePtr 0x0090FF10

//Instruction pointers

/*
First overwrite to fully control the igm
*/
#define igmUnlockPtr 0x0022A847

/*
Second overwrite to fully control the igm
*/
#define igmUnlockPtr0 0x0022A83D

/*
TODO: Check if I need to find similar instructions
#define igmUnlockPtr1 0x04556E00
#define igmUnlockPtr2 0x0022AC3A
*/

/*
Instruction writes to the mouse multi
*/
#define mouseUnlockPtr 0x0022A878 //0x0022AC58

/*
TODO: Check if I need to find similar instructions
#define mouseUnlockPtr0 0x04551428

Erasing this results in completely unlocked framerate
#define fullSpeedUnlockPtr 0x00622274 

Fixes fullscreen problem
#define fovUnlockPtr 0x06E63EE0 //Nothing seems to activley write in 1.0.3?!
*/
/*
This is overwritten to fix the lock that happens when a character uses an AOE, Mist, or Summon.
This was caused when the FPS coef was not 1 or 0.5 and a register recieved no value*/
#define actionAoeFixPtr 0x0024B48E
#define actionAoeFixDummyPtr 0x01E160BC


/*
This subroutine sets the precomputed animation rates of each object which uses them.
I assume the original developers decided to waste more memory in favor of faster code, though they could have still saved memory by only doing this once.
*/
#define setAnimationRatePtr 0x075D334

#endif


