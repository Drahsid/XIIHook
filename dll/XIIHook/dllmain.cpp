#include "pch.h"
#include "..\..\Globals.h"
#include "Utility.h"
#include "Input.h"

#define keyCodes gVars.IM.keyCode

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
	Vector3f wishPos;
	Vector3f wishLookAtPos;
	Vector3f lastPos;
	Vector3f eulerAngles;
	//TODO: Add these to config
	const float timeUntilSpeedRamp = 1000;
	const float speedMax = 128;
	float baseMoveSpeed = 4;
	float baseLookSpeed = 16;
	float moveSpeed = baseMoveSpeed;
	float lookSpeed = baseLookSpeed;
}

void rampupSpeed() {
	moveSpeed += (2 * (float)*gVars.realFrameTime);
	float wishspeed = baseLookSpeed * ((moveSpeed / baseMoveSpeed) / 8);
	lookSpeed = wishspeed >= baseLookSpeed ? wishspeed : baseLookSpeed;
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
		print("Failed to overwrite mouse unlock instructions\n");
	if (!WriteProcessMemory(hProcess, (LPVOID)igmUnlockPtr, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10, NULL))
		print("Failed to overwrite igm unlock instructions[]\n");
	if (!WriteProcessMemory(hProcess, (LPVOID)igmUnlockPtr0, "\x90\x90\x90\x90\x90\x90\x90\x90", 8, NULL))
		print("Failed to overwrite igm unlock instructions[0]\n");

	v3 = Vector3f();
	wishLookAtPos = v3;
	wishPos = v3;
	lastPos = v3;
	lfwd = Vector3f(0, 0, 1);
	lrgh = Vector3f(1, 0, 0);
	eulerAngles = v3;

	for (;;) {
		step(gVars);

		if (keyCodes[VK_CONTROL].state == KeyState::Pressed) {
			*gVars.freeCamEnabled = *gVars.freeCamEnabled == 0 ? 1 : 0;
			print("Free cam set to %d\n", *gVars.freeCamEnabled);
			*gVars.hudDisabled = *gVars.freeCamEnabled;
			*gVars.ctrlEnabled = !*gVars.freeCamEnabled;
		}

		if (*gVars.freeCamEnabled)
		{
			float frameTime = (float)*gVars.realFrameTime;

			float hv[3];
			hv[0] = 0;
			hv[1] = 0;
			hv[2] = 0;

			//Increase baseMoveSpeed
			if (keyCodes['X'].state == KeyState::Pressed) {
				baseMoveSpeed += 0.5;
				print("Base baseMoveSpeed is %f\n", baseMoveSpeed);
			}

			//Decrease baseMoveSpeed
			if (keyCodes['Z'].state == KeyState::Pressed) {
				baseMoveSpeed -= 0.5;
				print("Base baseMoveSpeed is %f\n", baseMoveSpeed);
			}

			//Increase baseLookSpeed
			if (keyCodes['V'].state == KeyState::Pressed) {
				baseLookSpeed += 0.5;
				print("Base baseLookSpeed is %f\n", baseLookSpeed);
			}

			//Decrease baseLookSpeed
			if (keyCodes['C'].state == KeyState::Pressed) {
				baseLookSpeed -= 0.5;
				print("Base baseLookSpeed is %f\n", baseLookSpeed);
			}

			baseMoveSpeed = clamp(baseMoveSpeed, speedMax, 0.5);
			baseLookSpeed = clamp(baseLookSpeed, speedMax, 0.5);

			//Forwards
			if (keyCodes['W'].state == KeyState::Pressed || keyCodes['W'].state == KeyState::Down) {
				hv[0] += 1;
				if (clock() > keyCodes['W'].invokeTime + timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Backwards
			if (keyCodes['S'].state == KeyState::Pressed || keyCodes['S'].state == KeyState::Down) {
				hv[0] -= 1;
				if (clock() > keyCodes['S'].invokeTime + timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Right Strafe
			if (keyCodes['D'].state == KeyState::Pressed || keyCodes['D'].state == KeyState::Down) {
				hv[1] += 1;
				if (clock() > keyCodes['D'].invokeTime + timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Strafe
			if (keyCodes['A'].state == KeyState::Pressed || keyCodes['A'].state == KeyState::Down) {
				hv[1] -= 1;
				if (clock() > keyCodes['A'].invokeTime + timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Raise
			if (keyCodes['E'].state == KeyState::Pressed || keyCodes['E'].state == KeyState::Down) {
				hv[2] += 1;
				if (clock() > keyCodes['E'].invokeTime + timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Lower
			if (keyCodes['Q'].state == KeyState::Pressed || keyCodes['Q'].state == KeyState::Down) {
				hv[2] -= 1;
				if (clock() > keyCodes['Q'].invokeTime + timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			// Adjust Pitch & Yaw
			if (keyCodes[VK_MBUTTON].state == KeyState::Pressed || keyCodes[VK_MBUTTON].state == KeyState::Down) {

			}

			// Negative Pitch
			if (keyCodes[VK_UP].state == KeyState::Pressed || keyCodes[VK_UP].state == KeyState::Down) {
				eulerAngles.x -= lookSpeed * RAD2DEG * frameTime;
			}

			// Positive Pitch
			if (keyCodes[VK_DOWN].state == KeyState::Pressed || keyCodes[VK_DOWN].state == KeyState::Down) {
				eulerAngles.x += lookSpeed * RAD2DEG * frameTime;
			}

			// Positive Yaw
			if (keyCodes[VK_RIGHT].state == KeyState::Pressed || keyCodes[VK_RIGHT].state == KeyState::Down) {
				eulerAngles.y += lookSpeed * RAD2DEG * frameTime;
			}

			// Negative Yaw
			if (keyCodes[VK_LEFT].state == KeyState::Pressed || keyCodes[VK_LEFT].state == KeyState::Down) {
				eulerAngles.y -= lookSpeed * RAD2DEG * frameTime;
			}

			if (hv[0] == 0 && hv[1] == 0 && hv[2] == 0) {
				moveSpeed = baseMoveSpeed;
				lookSpeed = baseLookSpeed;
			}

			moveSpeed = clamp(moveSpeed, speedMax, 0.5);
			lookSpeed = clamp(lookSpeed, speedMax, 0.5);

			Vector3f wishMove = v3;

			Vector3f fwd = v3;
			Vector3f rgh = v3;
			Vector3f up = Vector3f(0, 1, 0);

			Vector3f cameraPos = v3;
			Vector3f cameraLookAt = v3;

			v3.fromVolatile(gVars.cameraPosition, cameraPos);
			v3.fromVolatile(gVars.cameraLookAtPoint, cameraLookAt);

			fwd = (cameraLookAt - cameraPos).normalized();
			rgh = fwd.cross(up).normalized();
			up = rgh.cross(fwd);

			wishMove = (
				(fwd * hv[0])
				+ (rgh * hv[1])
				+ (up * hv[2])
				);

			if (wishMove.magnitude() != 0)
			{
				wishMove = wishMove.normalized() * (moveSpeed * frameTime);
				wishPos = cameraPos + wishMove;
				wishLookAtPos = wishPos
					+ Vector3f(cosf(eulerAngles.x) * -sinf(eulerAngles.y), 
						-sinf(eulerAngles.x), 
						cosf(eulerAngles.x) * cosf(eulerAngles.y));

				v3.toVolatile(wishPos, gVars.cameraPosition);
				v3.toVolatile(wishLookAtPos, gVars.cameraLookAtPoint);
			}
			else 
			{
				wishLookAtPos = cameraPos
					+ Vector3f(cosf(eulerAngles.x) * -sinf(eulerAngles.y), 
						-sinf(eulerAngles.x), 
						cosf(eulerAngles.x) * cosf(eulerAngles.y));

				v3.toVolatile(wishLookAtPos, gVars.cameraLookAtPoint);
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



