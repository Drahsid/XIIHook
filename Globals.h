#ifndef GLOBALS_H
#define GLOBALS_H

//Console colors
#define cc_NORM 15
#define cc_VERBOSE 2
#define cc_FUN 5
#define cc_ERROR 12
#define cc_WARN 14


#define NOP 0x90 //NOP instruction TODO: replace NOP strings with cleaner code using this

//In-game pointers
#define minFrameTimePtr 0x008FE378		//0x0090D398 //Min Frametime 1
#define minFrameTime1Ptr 0x008FF0A0		//0x0090E0C0 //Min Frametime 2 (Doesn't seem to do anything)
#define realFrameTimePtr 0x01F7C358		//0x01F97058 //Reported frametime 1
#define realFrameTime1Ptr 0x01F7D470	//0x01F98170 //Reported frametime 2 (Not to be trusted)

/*
In game multiplier
Generally becomes 1,2,4 though you can make it anything. Disabling instructions related to this can have some weird results.
*/
#define igmPtr 0x2062A88 //0x0207D798

/*
Wolrd multiplier (Eg skies and grass anims) @ < 1 this freezes the game, but there is still camera control
This is equal to the igm * the global time multiplier * (*=usually)
*/
#define worldMPtr 0x02062A84 //0x0207D794 

/*
Mouse ~~multiplier~~ Min angle coef
Realative to all other multipliers
*/
#define mouseCoefPtr 0x01E00820 //0x01E1A820 

/*
Global time multiplier
When I found this everything else made so much more sense, this is what we will be changing
*/
#define timeScalePtr 0x02062A8C //0x0207D79C 

/*
1 at 30 fps, 0.5 at 60
This should fix everything.
*/

#define framerateCoefPtr 0x1DFC0BC //0x01E160BC 

#define animationBaseRatePtr 0x008F6D80 //0x00905DA0

/*
Currently selected state of igm
*/
#define igmStatePtr 0x01FD2A44 //0x01FED754

#define uiEnabledPtr 0x01DFC0C0 //0x01E160C0 //Menu enabled

#define inTitlePtr 0x2088680 //0x020A3380 //0 In game 1 In title

// Credit to ffgriever for finding these
#define freeCamEnabledPtr		0x02096DFA
#define cameraPositionPtr		0x020955E0
#define cameraLookAtPointPtr	0x020955F0
#define cameraRollPtr			0x02094504
#define hudDisabledPtr			0x215D1BC

#define cameraQuatPtr			0x02C964C0


//Instruction pointers

/*
Combat menu pointer.
*/
#define inCombatMenPtr 0x01FD2694 //0x01FED3A4

/*
In cutscene pointer, is 1 when in a cutscene or at the title.
*/
#define inCScenePtr 0x2090754 //0x020AB454 

/*
In Move pointer, is 1 when in a movie.
*/
#define inMoviePtr 0x215D1C2 //0x022C0C51

/*
In game gamma
*/
#define gammaPtr 0x008F1130 //0x00900150

/*
Changing this does some weird stuff to the input
*/
#define mouseFPtr 0x01F111E0

/*
The in game FOV. This does not effect cutscenes (They use different camera)
*/
#define fovPtr 0x02095558 //0x020B0258 

/*
One byte, 0-6 (modulus) changes quality of shadow.
*/
#define shadowTypePtr 0x0090FF10




/*
First overwrite to fully control the igm
*/
#define igmUnlockPtr 0x0022A847 //0x0022AC27 

/*
Second overwrite to fully control the igm
*/
#define igmUnlockPtr0 0x0022A83D //0x0022AC1D

/*
Third overwrite to fully control the igm
*/
#define igmUnlockPtr1 0x04556E00 //No longer in 1.0.3?!

#define igmUnlockPtr2 0x0022AC3A //No longer in 1.0.3?!

/*
Instruction writes to the mouse multi
*/
#define mouseUnlockPtr 0x0022A878 //0x0022AC58

/*
Instruction writes to the mouse multi
*/
#define mouseUnlockPtr0 0x04551428 //No longer a second sub in 1.0.3?!

/*
Erasing this results in completely unlocked framerate
*/
#define fullSpeedUnlockPtr 0x00622274 

/*
Fixes fullscreen problem
*/
#define fovUnlockPtr 0x06E63EE0 //Nothing seems to activley write in 1.0.3?!

/*
This is overwritten to fix the lock that happens when a character uses an AOE, Mist, or Summon.
This was caused when the FPS coef was not 1 or 0.5 and a register recieved no value
*/
#define actionAoeFixPtr 0x0024B840 

/*
This subroutine sets the precomputed animation rates of each object which uses them.
I assume the original developers decided to waste more memory in favor of faster code, though they could have still saved memory by only doing this once.
*/
#define setAnimationRatePtr 0x075D3341 //0x075D3330 

/*
The address which addresses will be moved onto after patch
*/
#define animDummyPtr 0x01E160D4 

/*
The secondary frametime gets updates on this instruction.
*/
#define ftUpdatePtr 0x0B94C262 

/*
Overwrite to modify cutscene timescale
*/
#define cutsceneTimeScalePtr 0x06BB6240


#ifndef DLL_BUILD

	namespace {
		extern IDirect3D9Ex* p_Object = 0;
		extern IDirect3DDevice9Ex* p_Device = 0;
		extern D3DPRESENT_PARAMETERS p_Params;
		extern ID3DXLine* p_Line = 0;
		extern ID3DXFont* pFontSmall = 0;
		extern ID3DXFont* pFontSmaller = 0;
		extern ID3DXFont* pFontSmallest = 0;
		extern ID3DXFont* pFontLarge = 0;

		extern int Width = 1920;
		extern int Height = 1080;
	}
#endif

#endif


