#include "pch.h"
#include "../../Globals.h"
#include "Interp.h"
#include "Config.h"
#include "Utility.h"
#include "Input.h"

namespace
{
	HINSTANCE* myHInst;
	HANDLE myHandle;
	static gameVars gVars;
	bool breakStep = false;
	DWORD threadID;
	HANDLE asyncHandle;
	double lastMessageTick = 0;
	Vector3f v3;
	Vector3f lfwd, lrgh;
	Vector3f wishpos;
	Vector3f wishlookatpos;
	Vector3f lastPos;
	Quaternion quat;
	Quaternion cameraQuat;
	float pitch, yaw;
	const float timeUntilSpeedRamp = 1000;
	float movespeed = 4;
	float lookspeed = 16;
	float moveSpeed = movespeed;
	float lookSpeed = lookspeed;
}

void rampupSpeed() {
	moveSpeed += (2 * (float)*gVars.realFrameTime);
	float wishspeed = lookspeed * ((moveSpeed / movespeed) / 8);
	lookSpeed = wishspeed >= lookspeed ? wishspeed : lookspeed;
}

DWORD WINAPI asyncThread(LPVOID lpParameter) {
	gVars.FFXIIWND = FindWindow(0, L"FINAL FANTASY XII THE ZODIAC AGE");

	DWORD pId;
	GetWindowThreadProcessId(gVars.FFXIIWND, &pId);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);

	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stderr);

	//Overwriting instructions
	if (!WriteProcessMemory(hProcess, (LPVOID)mouseUnlockPtr, "\x90\x90\x90\x90\x90\x90\x90\x90", 8, NULL))
		Print("Failed to overwrite mouse unlock instructions\n");
	if (!WriteProcessMemory(hProcess, (LPVOID)igmUnlockPtr, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10, NULL))
		Print("Failed to overwrite igm unlock instructions[]\n");
	if (!WriteProcessMemory(hProcess, (LPVOID)igmUnlockPtr0, "\x90\x90\x90\x90\x90\x90\x90\x90", 8, NULL))
		Print("Failed to overwrite igm unlock instructions[0]\n");

	quat = Quaternion();
	cameraQuat = quat;
	v3 = Vector3f();
	wishlookatpos = v3;
	wishpos = v3;
	lastPos = v3;
	lfwd = Vector3f(0, 0, 1);
	lrgh = Vector3f(1, 0, 0);
	pitch = 0; yaw = 0;

	for (;;) {
		Step(gVars);

		if (gVars.IM.keyCode[VK_CONTROL].State == KeyState::Pressed) {
			*gVars.freeCamEnabled = *gVars.freeCamEnabled == 0 ? 1 : 0;
			Print("Free cam set to %d\n", *gVars.freeCamEnabled);
			*gVars.hudDisabled = *gVars.freeCamEnabled;
			*gVars.ctrlEnabled = !*gVars.freeCamEnabled;
		}

		if (*gVars.freeCamEnabled)
		{
			float frametime = (float)*gVars.realFrameTime;

			float hv[3];
			hv[0] = 0;
			hv[1] = 0;
			hv[2] = 0;

			//Increase movespeed
			if (gVars.IM.keyCode['X'].State == KeyState::Pressed) {
				movespeed += 0.5;
				Print("Base movespeed is %f\n", movespeed);
			}

			//Decrease movespeed
			if (gVars.IM.keyCode['Z'].State == KeyState::Pressed) {
				movespeed -= 0.5;
				Print("Base movespeed is %f\n", movespeed);
			}

			//Increase lookspeed
			if (gVars.IM.keyCode['V'].State == KeyState::Pressed) {
				lookspeed += 0.5;
				Print("Base lookspeed is %f\n", lookspeed);
			}

			//Decrease lookspeed
			if (gVars.IM.keyCode['C'].State == KeyState::Pressed) {
				lookspeed -= 0.5;
				Print("Base lookspeed is %f\n", lookspeed);
			}

			movespeed = clamp(movespeed, 1024, 0.5);
			lookspeed = clamp(lookspeed, 1024, 0.5);

			//Forwards
			if (gVars.IM.keyCode['W'].State == KeyState::Pressed || gVars.IM.keyCode['W'].State == KeyState::Down) {
				hv[0] += 1;
				if (clock() > gVars.IM.keyCode['W'].invokeTime + timeUntilSpeedRamp && gVars.IM.keyCode[VK_SHIFT].State != KeyState::Down) rampupSpeed();
			}

			//Backwards
			if (gVars.IM.keyCode['S'].State == KeyState::Pressed || gVars.IM.keyCode['S'].State == KeyState::Down) {
				hv[0] -= 1;
				if (clock() > gVars.IM.keyCode['S'].invokeTime + timeUntilSpeedRamp && gVars.IM.keyCode[VK_SHIFT].State != KeyState::Down) rampupSpeed();
			}

			//Right Strafe
			if (gVars.IM.keyCode['D'].State == KeyState::Pressed || gVars.IM.keyCode['D'].State == KeyState::Down) {
				hv[1] += 1;
				if (clock() > gVars.IM.keyCode['D'].invokeTime + timeUntilSpeedRamp && gVars.IM.keyCode[VK_SHIFT].State != KeyState::Down) rampupSpeed();
			}

			//Strafe
			if (gVars.IM.keyCode['A'].State == KeyState::Pressed || gVars.IM.keyCode['A'].State == KeyState::Down) {
				hv[1] -= 1;
				if (clock() > gVars.IM.keyCode['A'].invokeTime + timeUntilSpeedRamp && gVars.IM.keyCode[VK_SHIFT].State != KeyState::Down) rampupSpeed();
			}

			//Raise
			if (gVars.IM.keyCode['E'].State == KeyState::Pressed || gVars.IM.keyCode['E'].State == KeyState::Down) {
				hv[2] += 1;
				if (clock() > gVars.IM.keyCode['E'].invokeTime + timeUntilSpeedRamp && gVars.IM.keyCode[VK_SHIFT].State != KeyState::Down) rampupSpeed();
			}

			//Lower
			if (gVars.IM.keyCode['Q'].State == KeyState::Pressed || gVars.IM.keyCode['Q'].State == KeyState::Down) {
				hv[2] -= 1;
				if (clock() > gVars.IM.keyCode['Q'].invokeTime + timeUntilSpeedRamp && gVars.IM.keyCode[VK_SHIFT].State != KeyState::Down) rampupSpeed();
			}

			// Adjust Pitch & Yaw
			if (gVars.IM.keyCode[VK_MBUTTON].State == KeyState::Pressed || gVars.IM.keyCode[VK_MBUTTON].State == KeyState::Down) {

			}

			// Negative Pitch
			if (gVars.IM.keyCode[VK_UP].State == KeyState::Pressed || gVars.IM.keyCode[VK_UP].State == KeyState::Down) {
				pitch -= lookSpeed * Rad2Deg * frametime;
			}

			// Positive Pitch
			if (gVars.IM.keyCode[VK_DOWN].State == KeyState::Pressed || gVars.IM.keyCode[VK_DOWN].State == KeyState::Down) {
				pitch += lookSpeed * Rad2Deg * frametime;
			}

			// Positive Yaw
			if (gVars.IM.keyCode[VK_RIGHT].State == KeyState::Pressed || gVars.IM.keyCode[VK_RIGHT].State == KeyState::Down) {
				yaw += lookSpeed * Rad2Deg * frametime;
			}

			// Negative Yaw
			if (gVars.IM.keyCode[VK_LEFT].State == KeyState::Pressed || gVars.IM.keyCode[VK_LEFT].State == KeyState::Down) {
				yaw -= lookSpeed * Rad2Deg * frametime;
			}

			if (hv[0] == 0 && hv[1] == 0 && hv[2] == 0) {
				moveSpeed = movespeed;
				lookSpeed = lookspeed;
			}

			Vector3f wishmove = Vector3f();

			Vector3f fwd = Vector3f();
			Vector3f rgh = Vector3f();
			Vector3f up = Vector3f(0, 1, 0);

			Vector3f cameraPos = Vector3f();
			Vector3f cameraLookAt = Vector3f();

			v3.FromVolatile(gVars.cameraPosition, cameraPos);
			v3.FromVolatile(gVars.cameraLookAtPoint, cameraLookAt);

			fwd = (cameraLookAt - cameraPos).Normalized();
			rgh = fwd.Cross(up).Normalized();
			up = rgh.Cross(fwd);

			wishmove = (
				(fwd * hv[0])
				+ (rgh * hv[1])
				+ (up * hv[2])
				);
			if (wishmove.Magnitude() != 0)
			{
				wishmove = wishmove.Normalized() * (moveSpeed * frametime);
				wishpos = cameraPos + wishmove;
				wishlookatpos = wishpos
					+ Vector3f(cosf(pitch) * -sinf(yaw), -sinf(pitch), cosf(pitch) * cosf(yaw));

				v3.ToVolatile(wishpos, gVars.cameraPosition);
				v3.ToVolatile(wishlookatpos, gVars.cameraLookAtPoint);
			}
			else {
				wishlookatpos = cameraPos
					+ Vector3f(cosf(pitch) * -sinf(yaw), -sinf(pitch), cosf(pitch) * cosf(yaw));

				v3.ToVolatile(wishlookatpos, gVars.cameraLookAtPoint);
			}
		}

		Sleep((*gVars.realFrameTime / gVars.uConfig.mainThreadUpdateCoef) * 1000);
		if (breakStep) break;
	}

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID reserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		gVars = gameVars();

		myHInst = &hInst;
		myHandle = GetCurrentProcess();
		asyncHandle = CreateThread(NULL, NULL, asyncThread, NULL, NULL, &threadID);

		break;

	case DLL_PROCESS_DETACH:
		breakStep = true;
		FreeConsole();
		CloseHandle(asyncHandle);
		break;

	}

	return TRUE;
}



