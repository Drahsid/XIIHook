#include "pch.h"
#include "Globals.h"
#include "Input.h"
#include "Interpolator.h"
#include "Config.h"
#include "Utility.h"
#include "GameUtility.h"
#include "HookVars.h"
#include "d3d11hook.h"

#define keyCodes gVars.IM.keyCode


namespace
{
	HookVars hookVars;
	gameVars gVars;

	Vector3f v3;
	Vector3f lfwd, lrgh;
	Vector3f wishPos;
	Vector3f wishLookAtPos;
	Vector3f lastPos;
	Vector3f eulerAngles;
	Vector3l rawMouseDelta;

	DWORD pId;
	DWORD threadID;
	DWORD rawInputThreadID;

	HANDLE hProcess;
	HANDLE asyncHandle;
	HANDLE asyncRawInputHandle;

	//TODO: Add these to config
	const float timeUntilSpeedRamp = 1000;
	const float speedMax = 128;
	float baseMoveSpeed = 4;
	float baseLookSpeed = 16;
	float moveSpeed = baseMoveSpeed;
	float lookSpeed = baseLookSpeed;
	float mPitch = 0.0105f;
	float mYaw = 0.0105f;

	double lastMessageTick = 0;

	bool breakStep;

	MSG msg;
	HHOOK hhk = NULL;
	RAWINPUTDEVICE rid;
	std::vector<BYTE> rawInputBuffer;
}

inline void rampupSpeed() {
	moveSpeed += (2 * (float)*gVars.realFrameTime);
	float wishspeed = baseLookSpeed * ((moveSpeed / baseMoveSpeed) / 8);
	lookSpeed = wishspeed >= baseLookSpeed ? wishspeed : baseLookSpeed;
}

LRESULT CALLBACK msgHook(int nCode, WPARAM wParam, LPARAM lParam) {
	PostMessage(NULL, WM_INPUT, wParam, lParam);
	return CallNextHookEx(hhk, nCode, wParam, lParam);
}

DWORD WINAPI asyncRawInputThread(HMODULE hModule) {
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;

	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
		print("Error: registering raw input devices!\n");
	}

	hhk = SetWindowsHookEx(WH_MOUSE_LL, msgHook, NULL, NULL);

	if (hhk == NULL) print("Failed to create Windows hook!\nError: %lu\n", GetLastError());

	while (true) {
		GetMessage(&msg, NULL, 0, 0);

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_INPUT) 
		{
			SetLastError(0);

			UINT size = sizeof(RAWINPUT);
			UINT rvalue = GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

			if (rvalue == -1) print("Error: GetRawInputData 0\nLast Error: %lu\n", GetLastError());

			rawInputBuffer.resize(size);

			rvalue = GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, rawInputBuffer.data(), &size, sizeof(RAWINPUTHEADER));

			if (rvalue != size) print("Error: GetRawInputData 1\nLast Error: %lu\n", GetLastError());

			RAWINPUT ri = (RAWINPUT&)(*rawInputBuffer.data());

			if (ri.header.dwType == RIM_TYPEMOUSE)
			{
				print("Raw mouse delta: (%ld, %ld)\n", ri.data.mouse.lLastX, ri.data.mouse.lLastY);
				rawMouseDelta += Vector3l(ri.data.mouse.lLastX, ri.data.mouse.lLastY, 0);
			}
		}

		Sleep(1);
	}

	return 0;
}

DWORD WINAPI asyncThread(HMODULE hModule) 
{
	gVars.FFXIIWND = FindWindow(0, L"FINAL FANTASY XII THE ZODIAC AGE");

	GetWindowThreadProcessId(gVars.FFXIIWND, &pId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);

	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stderr);

	*gVars.aoeActionDummy = 0.5f;

	//Overwriting instructions
	if (!WriteProcessMemory(hProcess, (LPVOID)mouseUnlockPtr, "\x90\x90\x90\x90\x90\x90\x90\x90", 8, NULL))
		print("Failed to overwrite mouse unlock instructions\n");
	if (!WriteProcessMemory(hProcess, (LPVOID)igmUnlockPtr, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10, NULL))
		print("Failed to overwrite igm unlock instructions[]\n");
	if (!WriteProcessMemory(hProcess, (LPVOID)igmUnlockPtr0, "\x90\x90\x90\x90\x90\x90\x90\x90", 8, NULL))
		print("Failed to overwrite igm unlock instructions[0]\n");
	if (!WriteProcessMemory(hProcess, (LPVOID)actionAoeFixPtr, "\xF3\x0F\x10\x3C\x25\xBC\x60\xE1\x01\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 22, NULL))
		print("Failed to overwrite aoeActionFix\n");

	breakStep = false;

	hookVars.menu = Menu();
	InitImGui(hModule, gVars, hookVars);

	v3 = Vector3f();
	wishLookAtPos = v3;
	wishPos = v3;
	lastPos = v3;
	lfwd = Vector3f(0, 0, 1);
	lrgh = Vector3f(1, 0, 0);
	eulerAngles = v3;

	rawMouseDelta = Vector3l();

	asyncRawInputHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)asyncRawInputThread, hModule, NULL, &rawInputThreadID);

	for (;;) {
		step(gVars);
		hookVars.menu.Render(&gVars);

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = true;

		//print("Mouse delta: (%ld, %ld)\n", rawMouseDelta.x, rawMouseDelta.y);

		//gVars.frametimeMethod1((double)100);

		if (keyCodes[VK_INSERT].state == KeyState::Pressed) {
			hookVars.menu.showMenu = !hookVars.menu.showMenu;
			print("Menu state: %d\n", hookVars.menu.showMenu);
		}

		if (keyCodes[VK_CONTROL].state == KeyState::Pressed) {
			*gVars.freeCamEnabled = *gVars.freeCamEnabled == 0 ? 1 : 0;
			print("Free cam set to %d\n", *gVars.freeCamEnabled);
			*gVars.hudDisabled = *gVars.freeCamEnabled;
			*gVars.ctrlEnabled = !*gVars.freeCamEnabled;

			//let's initialize it properly once per activation
			if (*gVars.freeCamEnabled)
			{
				Vector3f cameraPos;
				Vector3f cameraLookAt;
				Vector3f worldUp = Vector3f(0, 1, 0);
				Vector3f worldForward = Vector3f(0, 0, 1);
				Vector3f worldRight = Vector3f(1, 0, 0);

				v3.fromVolatile(gVars.cameraPosition, cameraPos);
				v3.fromVolatile(gVars.cameraLookAtPoint, cameraLookAt);
				
				//game won't allow magnitude to be zero, so no point checking, I guess
				Vector3f lookAtVector = (cameraLookAt - cameraPos).normalized();
				Vector3f lookAtProjectionXZ(lookAtVector.x, 0, lookAtVector.z);

				float pitchDot = lookAtVector.dot(lookAtProjectionXZ);
				float pitch = acosf(pitchDot / lookAtProjectionXZ.magnitude()); //lookAtVector is normalized so no need to multiply by 1
				if (lookAtVector.dot(worldUp) > 0)
					pitch = -pitch;

				float yawDot = lookAtProjectionXZ.dot(worldForward);
				float yaw = acosf(yawDot / lookAtProjectionXZ.magnitude()); //worldForward is normalized
				if (lookAtProjectionXZ.dot(worldRight) > 0)
					yaw = -yaw;

				eulerAngles.x = pitch;
				eulerAngles.y = yaw;
			}
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
				baseMoveSpeed += 0.5f;
				print("Base baseMoveSpeed is %f\n", baseMoveSpeed);
			}

			//Decrease baseMoveSpeed
			if (keyCodes['Z'].state == KeyState::Pressed) {
				baseMoveSpeed -= 0.5f;
				print("Base baseMoveSpeed is %f\n", baseMoveSpeed);
			}

			//Increase baseLookSpeed
			if (keyCodes['V'].state == KeyState::Pressed) {
				baseLookSpeed += 0.5f;
				print("Base baseLookSpeed is %f\n", baseLookSpeed);
			}

			//Decrease baseLookSpeed
			if (keyCodes['C'].state == KeyState::Pressed) {
				baseLookSpeed -= 0.5f;
				print("Base baseLookSpeed is %f\n", baseLookSpeed);
			}

			baseMoveSpeed = clamp(baseMoveSpeed, speedMax, 0.5f);
			baseLookSpeed = clamp(baseLookSpeed, speedMax, 0.5f);

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

			// Apply Mouse Delta scaled to frametime
			eulerAngles.x += (rawMouseDelta.y * mPitch) * frameTime;
			eulerAngles.y += (rawMouseDelta.x * mYaw) * frameTime;

			//TODO: clamp eulerAngles to -PI : PI or 0 : 2PI to prevent issues with float rounding errors if they go too high

			if (hv[0] == 0 && hv[1] == 0 && hv[2] == 0) {
				moveSpeed = baseMoveSpeed;
				lookSpeed = baseLookSpeed;
			}

			moveSpeed = clamp(moveSpeed, speedMax, 0.5f);
			lookSpeed = clamp(lookSpeed, speedMax, 0.5f);

			if (eulerAngles.x > PI) {
				eulerAngles.x = -PI + (eulerAngles.x - PI);
			}
			if (eulerAngles.x < -PI) {
				eulerAngles.x = PI - (eulerAngles.x - PI);
			}

			if (eulerAngles.y > TAU) {
				eulerAngles.y = -TAU + (eulerAngles.y - TAU);
			}
			if (eulerAngles.y < -TAU) {
				eulerAngles.y = TAU - (eulerAngles.y - TAU);
			}

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
			up = rgh.cross(fwd).normalized();
			rgh = fwd.cross(up).normalized();

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

		rawMouseDelta = Vector3l();

		Sleep((*gVars.realFrameTime / gVars.uConfig.mainThreadUpdateCoef) * 1000);
		if (breakStep) break;
	}

	return 0;
}


BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID reserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		gVars = gameVars();

		asyncHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)asyncThread, hModule, NULL, &threadID);
		break;

	case DLL_PROCESS_DETACH:
		breakStep = true;
		FreeConsole();
		CloseHandle(asyncHandle);
		CloseHandle(asyncRawInputHandle);
		rid.hwndTarget = NULL;
		rid.dwFlags = RIDEV_REMOVE;
		RegisterRawInputDevices(&rid, 1, sizeof(rid));

		break;

	}

	return TRUE;
}



