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

		if (gVars.IM.keyCode[VK_MENU].State == KeyState::Pressed) {
			*gVars.freeCamEnabled = *gVars.freeCamEnabled == 0 ? 1 : 0;
			Print("Free cam set to %d\n", *gVars.freeCamEnabled);
			*gVars.hudDisabled = *gVars.freeCamEnabled;
		}

		if (*gVars.freeCamEnabled)
		{
			float frametime = (float)*gVars.realFrameTime;

			float hv[5];
			hv[0] = 0;
			hv[1] = 0;
			hv[2] = 0;

			//I Key; Forwards
			if (gVars.IM.keyCode[0x49].State == KeyState::Pressed || gVars.IM.keyCode[0x49].State == KeyState::Down) {
				hv[0] += 1;
			}

			//K Key; Backwards
			if (gVars.IM.keyCode[0x4B].State == KeyState::Pressed || gVars.IM.keyCode[0x4B].State == KeyState::Down) {
				hv[0] -= 1;
			}

			//L Key; Right Strafe
			if (gVars.IM.keyCode[0x4C].State == KeyState::Pressed || gVars.IM.keyCode[0x4C].State == KeyState::Down) {
				hv[1] += 1;
			}

			//J Key; Left Strafe
			if (gVars.IM.keyCode[0x4A].State == KeyState::Pressed || gVars.IM.keyCode[0x4A].State == KeyState::Down) {
				hv[1] -= 1;
			}

			//O Key; Raise
			if (gVars.IM.keyCode[0x4F].State == KeyState::Pressed || gVars.IM.keyCode[0x4F].State == KeyState::Down) {
				hv[2] += 1;
			}

			//U Key; Lower
			if (gVars.IM.keyCode[0x55].State == KeyState::Pressed || gVars.IM.keyCode[0x55].State == KeyState::Down) {
				hv[2] -= 1;
			}

			// Middle Mouse Button; Adjust Pitch & Yaw
			if (gVars.IM.keyCode[VK_MBUTTON].State == KeyState::Pressed || gVars.IM.keyCode[VK_MBUTTON].State == KeyState::Down) {
				
			}

			// Up; Negative Pitch
			if (gVars.IM.keyCode[VK_UP].State == KeyState::Pressed || gVars.IM.keyCode[VK_UP].State == KeyState::Down) {
				pitch -= 16 * Rad2Deg * frametime;
			}

			// Down; Negative Pitch
			if (gVars.IM.keyCode[VK_DOWN].State == KeyState::Pressed || gVars.IM.keyCode[VK_DOWN].State == KeyState::Down) {
				pitch += 16 * Rad2Deg * frametime;
			}

			// Right; Positive Yaw
			if (gVars.IM.keyCode[VK_RIGHT].State == KeyState::Pressed || gVars.IM.keyCode[VK_RIGHT].State == KeyState::Down) {
				yaw += 16 * Rad2Deg * frametime;
			}

			// Left; Negative Yaw
			if (gVars.IM.keyCode[VK_LEFT].State == KeyState::Pressed || gVars.IM.keyCode[VK_LEFT].State == KeyState::Down) {
				yaw -= 16 * Rad2Deg * frametime;
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
				wishmove = wishmove.Normalized() * (4 * frametime);
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
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
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



