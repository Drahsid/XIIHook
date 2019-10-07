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
#define userConfig gVars.uConfig

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
	Vector3 rawMouseDelta;
	CriticallyDampedV3 smoothRawMouseDelta;

	DWORD pId;
	DWORD threadID;

	HANDLE hProcess;
	HANDLE asyncHandle;

	//TODO: Add these to config
	float moveSpeed, lookSpeed;
	float cameraMoveSpeed;
	bool useMouseSmoothing = false;
	bool demandMinFrametime = false;

	bool breakStep;

	LONG_PTR wndProc = NULL;
	LONG_PTR originalProc = NULL;
	//RAWINPUTDEVICE rid;
	//std::vector<BYTE> rawInputBuffer;
}

inline void rampupSpeed() {
	moveSpeed += (2 * (float)*gVars.realFrameTime);
	float wishspeed = userConfig.baseLookSpeed * ((moveSpeed / userConfig.baseMoveSpeed) / 8);
	lookSpeed = wishspeed >= userConfig.baseLookSpeed ? wishspeed : userConfig.baseLookSpeed;
}

LRESULT CALLBACK newProc(HWND Hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_MOUSEMOVE) {
		POINTS newPos = MAKEPOINTS(lParam);
		Vector3 delta;

		RECT windowRect, desktopRect;
		GetWindowRect(GetDesktopWindow(), &desktopRect);
		GetWindowRect(gVars.FFXIIWND, &windowRect);

		if (FullscreenCheck(windowRect, desktopRect)) {
			delta = Vector3(
				(double)newPos.x - ((double)windowRect.right / 2),
				(double)newPos.y - ((double)windowRect.bottom / 2), 0);
		}
		else {
			delta = Vector3(
				(double)newPos.x - ((double)windowRect.right / 2) + 4,
				(double)newPos.y - ((double)windowRect.bottom / 2) + 31, 0);
		}
		
		rawMouseDelta += delta;
		smoothRawMouseDelta.SetTarget(rawMouseDelta);
	}

	//TODO: Figure out why this isn't sent?
	/*
	if (message == WM_INPUT)
	{
		UINT size = sizeof(RAWINPUT);
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
		rawInputBuffer.resize(size);
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, rawInputBuffer.data(), &size, sizeof(RAWINPUTHEADER));

		RAWINPUT ri = (RAWINPUT&)(*rawInputBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE)
		{
			print("Raw mouse delta: (%ld, %ld)\n", ri.data.mouse.lLastX, ri.data.mouse.lLastY);
			rawMouseDelta += Vector3l(ri.data.mouse.lLastX, ri.data.mouse.lLastY, 0);
		}
	}*/
	
	return CallWindowProc((WNDPROC)originalProc, gVars.FFXIIWND, message, wParam, lParam);
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
	//InitImGui(hModule, gVars, hookVars);

	v3 = Vector3f();
	wishLookAtPos = v3;
	wishPos = v3;
	lastPos = v3;
	lfwd = Vector3f(0, 0, 1);
	lrgh = Vector3f(1, 0, 0);
	eulerAngles = v3;

	rawMouseDelta = Vector3();
	smoothRawMouseDelta = CriticallyDampedV3(25);

	/*
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;

	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
		print("Error: registering raw input devices!\n");
	}*/

	originalProc = GetWindowLongPtr(gVars.FFXIIWND, GWLP_WNDPROC);
	wndProc = SetWindowLongPtr(gVars.FFXIIWND, GWLP_WNDPROC, (LONG_PTR)newProc);

	clock_t lastUpdateTime = 0;
	double loopDeltaTime = 0;

	for (;;) {

		Vector3f rhs;
		v3.fromVolatile(gVars.cameraPosition, rhs);
		//print("%s\n", rhs.toString().c_str());

		clock_t thisClock = clock();
		loopDeltaTime = (double)(thisClock - lastUpdateTime) / CLOCKS_PER_SEC;
		lastUpdateTime = thisClock;

		step(gVars, demandMinFrametime);
		//hookVars.menu.Render(&gVars);
		//ImGuiIO& io = ImGui::GetIO();
		//io.MouseDrawCursor = true;

		//print("Mouse delta: (%ld, %ld)\n", rawMouseDelta.x, rawMouseDelta.y);

		//gVars.frametimeMethod1((double)100);

		*gVars.fov = userConfig.fov * RAD2DEG;

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
			float hv[3];
			hv[0] = 0;
			hv[1] = 0;
			hv[2] = 0;

			//Increase baseMoveSpeed
			if (keyCodes['X'].state == KeyState::Pressed) {
				userConfig.baseMoveSpeed += 0.5f;
				print("Base baseMoveSpeed is %f\n", userConfig.baseMoveSpeed);
			}

			//Decrease baseMoveSpeed
			if (keyCodes['Z'].state == KeyState::Pressed) {
				userConfig.baseMoveSpeed -= 0.5f;
				print("Base baseMoveSpeed is %f\n", userConfig.baseMoveSpeed);
			}

			//Increase baseLookSpeed
			if (keyCodes['V'].state == KeyState::Pressed) {
				userConfig.baseLookSpeed += 0.5f;
				print("Base baseLookSpeed is %f\n", userConfig.baseLookSpeed);
			}

			//Decrease baseLookSpeed
			if (keyCodes['C'].state == KeyState::Pressed) {
				userConfig.baseLookSpeed -= 0.5f;
				print("Base baseLookSpeed is %f\n", userConfig.baseLookSpeed);
			}

			userConfig.baseMoveSpeed = clamp(userConfig.baseMoveSpeed, userConfig.speedMax, 0.5f);
			userConfig.baseLookSpeed = clamp(userConfig.baseLookSpeed, userConfig.speedMax, 0.5f);

			//Forwards
			if (keyCodes['W'].state == KeyState::Pressed || keyCodes['W'].state == KeyState::Down) {
				hv[0] += 1;
				if (clock() > keyCodes['W'].invokeTime + userConfig.timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Backwards
			if (keyCodes['S'].state == KeyState::Pressed || keyCodes['S'].state == KeyState::Down) {
				hv[0] -= 1;
				if (clock() > keyCodes['S'].invokeTime + userConfig.timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Right Strafe
			if (keyCodes['D'].state == KeyState::Pressed || keyCodes['D'].state == KeyState::Down) {
				hv[1] += 1;
				if (clock() > keyCodes['D'].invokeTime + userConfig.timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Strafe
			if (keyCodes['A'].state == KeyState::Pressed || keyCodes['A'].state == KeyState::Down) {
				hv[1] -= 1;
				if (clock() > keyCodes['A'].invokeTime + userConfig.timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Raise
			if (keyCodes['E'].state == KeyState::Pressed || keyCodes['E'].state == KeyState::Down) {
				hv[2] += 1;
				if (clock() > keyCodes['E'].invokeTime + userConfig.timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			//Lower
			if (keyCodes['Q'].state == KeyState::Pressed || keyCodes['Q'].state == KeyState::Down) {
				hv[2] -= 1;
				if (clock() > keyCodes['Q'].invokeTime + userConfig.timeUntilSpeedRamp && keyCodes[VK_SHIFT].state != KeyState::Down) rampupSpeed();
			}

			if (keyCodes[VK_TAB].state == KeyState::Pressed) {
				useMouseSmoothing = !useMouseSmoothing;
				print("Mouse Smoothing: %d\n", useMouseSmoothing);
			}

			if (keyCodes[VK_OEM_3].state == KeyState::Pressed) {
				demandMinFrametime = !demandMinFrametime;
				print("Demand Minimum Frametime: %d\n", demandMinFrametime);
			}

			if (keyCodes[VK_OEM_PERIOD].state == KeyState::Pressed) {
				smoothRawMouseDelta.dampening += 0.5;
				print("Mouse Smoothing Dampening: %f\n", smoothRawMouseDelta.dampening);
			}

			if (keyCodes[VK_OEM_COMMA].state == KeyState::Pressed) {
				smoothRawMouseDelta.dampening -= 0.5;
				print("Mouse Smoothing Dampening: %f\n", smoothRawMouseDelta.dampening);
			}

			if (keyCodes[VK_OEM_PLUS].state == KeyState::Pressed) {
				userConfig.mYaw += 0.0005;
				userConfig.mPitch += 0.0005;
				print("Mouse Pitch and Yaw Coefs: (%f, %f)\n", userConfig.mPitch, userConfig.mYaw);
			}

			if (keyCodes[VK_OEM_MINUS].state == KeyState::Pressed) {
				userConfig.mYaw -= 0.0005;
				userConfig.mPitch -= 0.0005;
				print("Mouse Pitch and Yaw Coefs: (%f, %f)\n", userConfig.mPitch, userConfig.mYaw);
			}

			if (keyCodes['M'].state == KeyState::Pressed) {
				userConfig.timeUntilSpeedRamp += 50;
				print("Time Until Speed Ramp: %f\n", userConfig.timeUntilSpeedRamp);
			}

			if (keyCodes['N'].state == KeyState::Pressed) {
				userConfig.timeUntilSpeedRamp -= 50;
				print("Time Until Speed Ramp: %f\n", userConfig.timeUntilSpeedRamp);
			}

			Vector3 mouseDelta = Vector3();

			// Mouse Look
			if (keyCodes[VK_RBUTTON].state == KeyState::Pressed || keyCodes[VK_RBUTTON].state == KeyState::Down
				|| keyCodes[VK_LBUTTON].state == KeyState::Pressed || keyCodes[VK_LBUTTON].state == KeyState::Down
				) {
				// Apply Mouse Delta scaled to deltatime
				if (useMouseSmoothing) {
					mouseDelta = smoothRawMouseDelta.GetPosition();
				}
				else {
					mouseDelta.x = rawMouseDelta.x;
					mouseDelta.y = rawMouseDelta.y;
				}
			}

			cameraMoveSpeed = 0;
			//Movement Effectors
			if (keyCodes[VK_LBUTTON].state == KeyState::Pressed || keyCodes[VK_LBUTTON].state == KeyState::Down) {
				mouseDelta.y = 0; // Disable pitch look
				hv[0] += -rawMouseDelta.y; // Steering? Not sure what this is called
				cameraMoveSpeed = abs(rawMouseDelta.y) * userConfig.mPitch;
			}

			if (keyCodes[VK_MBUTTON].state == KeyState::Pressed || keyCodes[VK_MBUTTON].state == KeyState::Down) {
				mouseDelta = Vector3(); //Disable Look
				hv[1] += rawMouseDelta.x * loopDeltaTime; // Horizontal Panning
				hv[2] += -rawMouseDelta.y * loopDeltaTime; // Vertical Panning

				cameraMoveSpeed = (abs(rawMouseDelta.x) * userConfig.mYaw) + (abs(rawMouseDelta.y) * userConfig.mPitch);
			}

			if ((keyCodes[VK_MENU].state == KeyState::Pressed || keyCodes[VK_MENU].state == KeyState::Down)
				&& (keyCodes[VK_RBUTTON].state == KeyState::Pressed || keyCodes[VK_RBUTTON].state == KeyState::Down)
				) {
				mouseDelta = Vector3(); //Disable Look
				hv[0] += -rawMouseDelta.y; // Dollying
				cameraMoveSpeed = abs(rawMouseDelta.y) * userConfig.mPitch;
			}

			eulerAngles.x += (mouseDelta.y * userConfig.mPitch) * loopDeltaTime;
			eulerAngles.y += (mouseDelta.x * userConfig.mYaw) * loopDeltaTime;

			// Negative Pitch
			if (keyCodes[VK_UP].state == KeyState::Pressed || keyCodes[VK_UP].state == KeyState::Down) {
				eulerAngles.x -= lookSpeed * RAD2DEG * loopDeltaTime;
			}

			// Positive Pitch
			if (keyCodes[VK_DOWN].state == KeyState::Pressed || keyCodes[VK_DOWN].state == KeyState::Down) {
				eulerAngles.x += lookSpeed * RAD2DEG * loopDeltaTime;
			}

			// Positive Yaw
			if (keyCodes[VK_RIGHT].state == KeyState::Pressed || keyCodes[VK_RIGHT].state == KeyState::Down) {
				eulerAngles.y += lookSpeed * RAD2DEG * loopDeltaTime;
			}

			// Negative Yaw
			if (keyCodes[VK_LEFT].state == KeyState::Pressed || keyCodes[VK_LEFT].state == KeyState::Down) {
				eulerAngles.y -= lookSpeed * RAD2DEG * loopDeltaTime;
			}

			//TODO: clamp eulerAngles to -PI : PI or 0 : 2PI to prevent issues with float rounding errors if they go too high

			if (hv[0] == 0 && hv[1] == 0 && hv[2] == 0) {
				moveSpeed = userConfig.baseMoveSpeed;
				lookSpeed = userConfig.baseLookSpeed;
			}

			moveSpeed = clamp(moveSpeed, userConfig.speedMax, 0.5f) + cameraMoveSpeed;
			lookSpeed = clamp(lookSpeed, userConfig.speedMax, 0.5f);

			if (eulerAngles.x > PI) {
				eulerAngles.x += -TAU;
				eulerAngles.y += HPI; //Fix wrong flipping
			}
			if (eulerAngles.x < -PI) {
				eulerAngles.x += TAU;
				eulerAngles.y += HPI; //Fix wrong flipping
			}

			if (eulerAngles.y > TAU) {
				eulerAngles.y += -DTAU;
			}
			if (eulerAngles.y < -TAU) {
				eulerAngles.y += DTAU;
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
				wishMove = wishMove.normalized() * (moveSpeed * loopDeltaTime);
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

		rawMouseDelta = Vector3();

		Sleep((*gVars.realFrameTime / userConfig.mainThreadUpdateCoef) * 1000);
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
		//rid.hwndTarget = NULL;
		//rid.dwFlags = RIDEV_REMOVE;
		//RegisterRawInputDevices(&rid, 1, sizeof(rid));

		break;

	}

	return TRUE;
}



