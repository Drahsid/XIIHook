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
#define tftPtr 0x0090D398 //Min Frametime 1
#define tft1Ptr 0x0090E0C0 //Min Frametime 2 (Doesn't seem to do anything)
#define rftPtr 0x01F97058  //Reported frametime 1
#define rft1Ptr 0x01F98170 //Reported frametime 2 (Not to be trusted)
#define igmPtr 0x0207D798 /*
In game multiplier
Generally becomes 1,2,4 though you can make it anything. Disabling instructions related to this can have some weird results.
*/
#define worldMPtr 0x0207D794 /*
Wolrd multiplier (Eg skies and grass anims) @ < 1 this freezes the game, but there is still camera control
This is equal to the igm * the global time multiplier * (*=usually)
*/
#define mouseMPtr 0x01E1A820 /*
Mouse ~~multiplier~~ Min angle coef
Realative to all other multipliers
*/
#define tScalePtr 0x0207D79C /*
Global time multiplier
When I found this everything else made so much more sense, this is what we will be changing
*/
#define framerateCoefPtr 0x01E160BC /*
1 at 30 fps, 0.5 at 60
This should fix everything.
*/
#define igmSPtr 0x01FED754 /*
Currently selected state of igmptr
*/
#define uiOPtr 0x01E160C0 //Menu enabled
#define titlPtr 0x020A3380 //0 In game 1 In title
//Instruction pointers
#define cMenPtr 0x01FED3A4 /*
Combat menu pointer.
*/
#define cScenePtr 0x020AB454 /*
In cutscene pointer, is 1 when in a cutscene or at the title.
*/
#define moviePtr 0x022C0C51 /*
In Move pointer, is 1 when in a movie.
*/
#define gammaPtr 0x00900150 /*
In game gamma
*/
#define mouseFPtr 0x01F111E0 /*
Chaning this does some weird stuff to the input
*/
#define fovPtr 0x020B0258 /*
The in game FOV. This does not effect cutscenes (They use different camera)
*/
#define gammaPtr 0x00900150 /*
The in game gamma.
*/
#define shadowTypePtr 0x0090FF10 /*
One byte, 0-6 (modulus) changes quality of shadow.
*/
#define igmUnlockPtr 0x0022AC27 /*
First overwrite to fully control the igm*/
#define igmUnlockPtr0 0x0022AC1D /*
Second overwrite to fully control the igm*/
#define igmUnlockPtr1 0x0022AC1D /*
Third overwrite to fully control the igm*/
#define igmUnlockPtr2 0x04556E00
#define igmUnlockPtr3 0x0022AC3A
#define mouseUnlockPtr 0x0022AC58 /*
Instruction writes to the mouse multi.*/
#define mouseUnlockPtr0 0x04551428 /*
Instruction writes to the mouse multi.
*/
#define fullSpeedUnlockPtr 0x00622274 /*
Erasing this results in completely unlocked framerate*/
#define fovUnlockPtr 0x06E63EE0 /*
Fixes fullscreen problem
*/
#define actionAoeFixPtr 0x0024B840 /*
This is overwritten to fix the lock that happens when a character uses an AOE, Mist, or Summon.
This was caused when the FPS coef was not 1 or 0.5 and a register recieved no value
*/
#define D3D11EndScene 0x7FFAD2985070 /*DX11*/

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


