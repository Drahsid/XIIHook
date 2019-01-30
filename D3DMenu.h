#ifndef D3DMenu_H
#define D3DMenu_H

#include "pch.h"
#include "D3DMenu.h"
#include "d3dDraw.h"
#include "Config.h"

enum currentTab {
	general,
	config,
	stats
};

namespace d3dHook 
{
	struct ProgressBar {
		double min, max, current;
		inline float progress() {
			if (current <= min) return 0;
			if (current >= max) return 1;
			return (float)((current - min) / (max - min));
		}
	};

	class MenuObject 
	{
	public:
		virtual void Render() = 0;
	public:
		Box2D box;
		Color4 c0, c1, c2;
		
	protected:
		MenuObject* parent;
	};

	class mBox : public MenuObject {
	public:
		mBox(MenuObject* p = nullptr, Box2D b = Box2D(), Color4 col0 = Color4(), Color4 col1 = Color4(), Color4 col2 = Color4())
		{
			parent = p;
			box = b;
			c0 = col0;
			c1 = col1;
			c2 = col2;

			if (p != nullptr) {
				int x = p->box.Pos.x + b.Pos.x;
				int y = p->box.Pos.y + b.Pos.y;
				box = Box2D(Vector2(x, y), b.Size);
			}
		}

		void Render() {
			DrawAndFillBox(box, c0, c1);
		}
	};

	class mBar : public MenuObject {
	public:
		ProgressBar pBar;

		mBar(MenuObject* p = nullptr, Box2D b = Box2D(), Color4 col0 = Color4(), Color4 col1 = Color4(), Color4 col2 = Color4(), float min = 0, float max = 0)
		{
			parent = p;
			box = b;
			c0 = col0;
			c1 = col1;
			c2 = col2;
			pBar.min = min;
			pBar.max = max;

			if (p != nullptr) {
				int x = p->box.Pos.x + b.Pos.x;
				int y = p->box.Pos.y + b.Pos.y;
				box = Box2D(Vector2(x, y), b.Size);
			}
		}

		void Render() {
			DrawProgressBar(box, c0, c1, c2, pBar.progress());
		}

		void Update(double lhs, double rhs) {
			pBar.current = lhs / rhs;
		}

		void Update(double lhs) {
			pBar.current = lhs;
		}
	};

	class D3DMenu
	{
	public:
		D3DMenu() {}
		~D3DMenu() {}

	public:
		// TODO: save/load
		double iStep = 0.5;
		bool SeeMenu = false;
		bool LoadedConfig = false;

		currentTab eTab = general;

		Color4 accent0 = Color4(36, 36, 40, 255);
		Color4 accent1 = Color4(48, 48, 52, 255);
		Color4 accent2 = Color4(24, 24, 28, 255);
		Color4 accent3 = Color4(115, 50, 115, 255);
		Color4 bEnabled = Color4(24, 128, 24, 255);
		Color4 bDisabled = Color4(128, 24, 24, 255);
		Color4 text = Color4(200, 200, 200, 255);
		Color4 text1 = Color4(178, 178, 178, 255);
		
		Color4 selectionStart = Color4(50, 115, 115, 255);
		Color4 selectionColor = selectionStart;
		float selectionFadeTime = 4;
		float lastSelectionTime = 0;

		mBox mainBox = mBox(nullptr, Box2D(-1, -1, 640, 480), accent1, accent2, accent2);
		mBox configTabBox = mBox(&mainBox, Box2D(128, 18, 128, 16), accent0, accent0, accent0);
		mBox statsTabBox = mBox(&mainBox, Box2D(256, 18, 128, 16), accent0, accent0, accent0);

		mBar gFPS = mBar(&mainBox, Box2D(3, 37, 224, 24), accent0, accent1, accent3, 10, 255);
		mBar gFPSMen = mBar(&gFPS, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 10, 255);
		mBar gFPSMov = mBar(&gFPSMen, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 10, 255);
		mBar gFPSNoFo = mBar(&gFPSMov, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 10, 255);
		mBar gFOV = mBar(&gFPSNoFo, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 1, 360);
		mBar gGamma = mBar(&gFOV, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 1, 255);
		mBar gigmState0 = mBar(&gGamma, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 1, 32);
		mBar gigmState1 = mBar(&gigmState0, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 1, 32);
		mBar gigmState2 = mBar(&gigmState1, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 1, 32);
		mBar gEasingTime = mBar(&gigmState2, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 0.1, 10);
		mBar gMouseMulti = mBar(&gEasingTime, Box2D(0, 26, 224, 24), accent0, accent1, accent3, 0.1, 10);

		mBox bAdaptiveMouse = mBox(&gMouseMulti, Box2D(228, 0, 112, 24), accent0, accent2, accent0);
		mBox bLockedMouse = mBox(&bAdaptiveMouse, Box2D(116, 0, 112, 24), accent0, accent2, accent0);
		mBox bEasing = mBox(&gEasingTime, Box2D(228, 0, 112, 24), accent0, accent2, accent0);

		Box2D generalGroup = Box2D();
		Box2D configGroup = Box2D();
		Box2D statsGroup = Box2D();

		Vector2 currentSelection;

		void step(uint8_t&gameFocus, HWND&overlayWindow, UserConfig&uConfig)
		{
			//Create default menu values then save
			if (!LoadedConfig) 
			{
				generalGroup.Pos = Vector2(1, 18);
				generalGroup.Size = Vector2(637, 460);

				configGroup.Pos = Vector2(1, 18);
				configGroup.Size = Vector2(637, 460);

				statsGroup.Pos = Vector2(1, 18);
				statsGroup.Size = Vector2(637, 460);

				LoadedConfig = true;
			}
			if (SeeMenu && gameFocus != 2 ) 
			{
				mainBox.Render();
				DrawString("FPS UNLOCKER MENU", Vector2(1, 0), text, pFontLarge);
				switch (eTab) {
				case general:
					//Tabs
					DrawTabBox(generalGroup, accent0, accent1, text, "General");
					configTabBox.Render();
					statsTabBox.Render();

					DrawLine(Vector2(configTabBox.box.Pos.x + configTabBox.box.Size.x, configTabBox.box.Pos.y), Vector2(configTabBox.box.Pos.x + configTabBox.box.Size.x, configTabBox.box.Pos.y + configTabBox.box.Size.y + 2), accent2);
					DrawLine(Vector2(configTabBox.box.Pos.x + configTabBox.box.Size.x - 1, configTabBox.box.Pos.y), Vector2(configTabBox.box.Pos.x + configTabBox.box.Size.x - 1, configTabBox.box.Pos.y + configTabBox.box.Size.y), accent2);

					//Contents of general tab
					gFPS.Update(1, uConfig.requestedMinFramerate);
					gFPSMen.Update(1, uConfig.requestedMinFramerateMenus);
					gFPSMov.Update(1, uConfig.requestedMinFramerateMovies);
					gFPSNoFo.Update(1, uConfig.requestedMinFramerateNoFocus);
					gFOV.Update(uConfig.fov, Rad2Deg);
					gGamma.Update(uConfig.gamma);
					gigmState0.Update(uConfig.igmState0Override);
					gigmState1.Update(uConfig.igmState1Override);
					gigmState2.Update(uConfig.igmState2Override);
					gEasingTime.Update(uConfig.easingTime);
					gMouseMulti.Update(uConfig.lockedMouseMulti);

					gFPS.Render();
					gFPSMen.Render();
					gFPSMov.Render();
					gFPSNoFo.Render();
					gFOV.Render();
					gGamma.Render();
					gigmState0.Render();
					gigmState1.Render();
					gigmState2.Render();
					gEasingTime.Render();
					gMouseMulti.Render();

					bAdaptiveMouse.c1 = uConfig.bEnableAdaptiveMouse ? bEnabled : bDisabled;
					bLockedMouse.c1 = uConfig.bEnableLockedMouseMulti ? bEnabled : bDisabled;
					bEasing.c1 = uConfig.bEnableEasing ? bEnabled : bDisabled;
					bAdaptiveMouse.Render();
					bLockedMouse.Render();
					bEasing.Render();


					//Selection Fading
					float cTime = ((float)clock() / CLOCKS_PER_SEC);
					float coef = abs(((cTime - lastSelectionTime) / (selectionFadeTime / 2)) - 1);
					if (coef > 1) coef = 1;
					if (cTime > lastSelectionTime + selectionFadeTime) lastSelectionTime = cTime;
					selectionColor = Color4(36, 44, 64, 255) + (selectionStart * coef);

					//Cursor Input
					if (GetAsyncKeyState(VK_NUMPAD8) & 1) currentSelection.y -= 1;
					if (GetAsyncKeyState(VK_NUMPAD2) & 1) currentSelection.y += 1;
					if (GetAsyncKeyState(VK_NUMPAD4) & 1) currentSelection.x -= 1;
					if (GetAsyncKeyState(VK_NUMPAD6) & 1) currentSelection.x += 1;
					
					//Clamping Cursor Position
					if (currentSelection.y < 0) currentSelection.y = 10;
					if (currentSelection.y > 10) currentSelection.y = 0;
					if (currentSelection.x > 0 && currentSelection.y != 9 && currentSelection.y != 10) currentSelection.x = 0;
					else if (currentSelection.y == 9 && currentSelection.x > 1) currentSelection.x = 0;
					else if (currentSelection.y == 9 && currentSelection.x < 0) currentSelection.x = 1;
					else if (currentSelection.y == 10 && currentSelection.x > 2) currentSelection.x = 0;
					else if (currentSelection.y == 10 && currentSelection.x < 0) currentSelection.x = 2;

					//Input and rendering of selection box
					switch (currentSelection.y)
					{
					case 0: {
						double nFPS = 1 / uConfig.requestedMinFramerate;
						if (GetAsyncKeyState(VK_PRIOR)) nFPS -= iStep;
						if (GetAsyncKeyState(VK_NEXT)) nFPS += iStep;
						if (nFPS > gFPS.pBar.max) nFPS = gFPS.pBar.max;
						if (nFPS < gFPS.pBar.min) nFPS = gFPS.pBar.min;
						uConfig.requestedMinFramerate = 1 / nFPS;
						DrawBox(gFPS.box, 2, selectionColor);
						break;
					}
					case 1: {
						double nFPS = 1 / uConfig.requestedMinFramerateMenus;
						if (GetAsyncKeyState(VK_PRIOR)) nFPS -= iStep;
						if (GetAsyncKeyState(VK_NEXT)) nFPS += iStep;
						if (nFPS > gFPS.pBar.max) nFPS = gFPS.pBar.max;
						if (nFPS < gFPS.pBar.min) nFPS = gFPS.pBar.min;
						uConfig.requestedMinFramerateMenus = 1 / nFPS;
						DrawBox(gFPSMen.box, 2, selectionColor);
						break;
					}
					case 2: {
						double nFPS = 1 / uConfig.requestedMinFramerateMovies;
						if (GetAsyncKeyState(VK_PRIOR)) nFPS -= iStep;
						if (GetAsyncKeyState(VK_NEXT)) nFPS += iStep;
						if (nFPS > gFPS.pBar.max) nFPS = gFPS.pBar.max;
						if (nFPS < gFPS.pBar.min) nFPS = gFPS.pBar.min;
						uConfig.requestedMinFramerateMovies = 1 / nFPS;
						DrawBox(gFPSMov.box, 2, selectionColor);
						break;
					}
					case 3: {
						double nFPS = 1 / uConfig.requestedMinFramerateNoFocus;
						if (GetAsyncKeyState(VK_PRIOR)) nFPS -= iStep;
						if (GetAsyncKeyState(VK_NEXT)) nFPS += iStep;
						if (nFPS > gFPS.pBar.max) nFPS = gFPS.pBar.max;
						if (nFPS < gFPS.pBar.min) nFPS = gFPS.pBar.min;
						uConfig.requestedMinFramerateNoFocus = 1 / nFPS;
						DrawBox(gFPSNoFo.box, 2, selectionColor);
						break;
					}
					case 4: {
						float nFOV = uConfig.fov / Rad2Deg;
						if (GetAsyncKeyState(VK_PRIOR)) nFOV -= iStep;
						if (GetAsyncKeyState(VK_NEXT)) nFOV += iStep;
						if (nFOV > gFOV.pBar.max) nFOV = gFOV.pBar.max;
						if (nFOV < gFOV.pBar.min) nFOV = gFOV.pBar.min;
						uConfig.fov = nFOV * Rad2Deg;
						DrawBox(gFOV.box, 2, selectionColor);
						break; 
					}
					case 5:
						if (GetAsyncKeyState(VK_PRIOR)) uConfig.gamma -= iStep;
						if (GetAsyncKeyState(VK_NEXT)) uConfig.gamma += iStep;
						if (uConfig.gamma > gGamma.pBar.max) uConfig.gamma = gGamma.pBar.max;
						if (uConfig.gamma < gGamma.pBar.min) uConfig.gamma = gGamma.pBar.min;
						DrawBox(gGamma.box, 2, selectionColor);
						break;
					case 6:
						if (GetAsyncKeyState(VK_PRIOR)) uConfig.igmState0Override -= iStep;
						if (GetAsyncKeyState(VK_NEXT)) uConfig.igmState0Override += iStep;
						if (uConfig.igmState0Override > gigmState0.pBar.max) uConfig.igmState0Override = gigmState0.pBar.max;
						if (uConfig.igmState0Override < gigmState0.pBar.min) uConfig.igmState0Override = gigmState0.pBar.min;
						DrawBox(gigmState0.box, 2, selectionColor);
						break;
					case 7:
						if (GetAsyncKeyState(VK_PRIOR)) uConfig.igmState1Override -= iStep;
						if (GetAsyncKeyState(VK_NEXT)) uConfig.igmState1Override += iStep;
						if (uConfig.igmState1Override > gigmState1.pBar.max) uConfig.igmState1Override = gigmState1.pBar.max;
						if (uConfig.igmState1Override < gigmState1.pBar.min) uConfig.igmState1Override = gigmState1.pBar.min;
						DrawBox(gigmState1.box, 2, selectionColor);
						break;
					case 8:
						if (GetAsyncKeyState(VK_PRIOR)) uConfig.igmState2Override -= iStep;
						if (GetAsyncKeyState(VK_NEXT)) uConfig.igmState2Override += iStep;
						if (uConfig.igmState2Override > gigmState2.pBar.max) uConfig.igmState2Override = gigmState2.pBar.max;
						if (uConfig.igmState2Override < gigmState2.pBar.min) uConfig.igmState2Override = gigmState2.pBar.min;
						DrawBox(gigmState2.box, 2, selectionColor);
						break;
					case 9:
						if (currentSelection.x == 0) {
							if (GetAsyncKeyState(VK_PRIOR)) uConfig.easingTime -= iStep;
							if (GetAsyncKeyState(VK_NEXT)) uConfig.easingTime += iStep;
							if (uConfig.easingTime > gEasingTime.pBar.max) uConfig.easingTime = gEasingTime.pBar.max;
							if (uConfig.easingTime < gEasingTime.pBar.min) uConfig.easingTime = gEasingTime.pBar.min;
							DrawBox(gEasingTime.box, 2, selectionColor);
						}
						else if (currentSelection.x == 1) {
							if (GetAsyncKeyState(VK_PRIOR)) uConfig.bEnableEasing = false;
							if (GetAsyncKeyState(VK_NEXT)) uConfig.bEnableEasing = true;
							DrawBox(bEasing.box, 2, selectionColor);
						}
						break;
					case 10:
						if (currentSelection.x == 0) {
							if (GetAsyncKeyState(VK_PRIOR)) uConfig.lockedMouseMulti -= iStep;
							if (GetAsyncKeyState(VK_NEXT)) uConfig.lockedMouseMulti += iStep;
							if (uConfig.lockedMouseMulti > gMouseMulti.pBar.max) uConfig.lockedMouseMulti = gMouseMulti.pBar.max;
							if (uConfig.lockedMouseMulti < gMouseMulti.pBar.min) uConfig.lockedMouseMulti = gMouseMulti.pBar.min;
							DrawBox(gMouseMulti.box, 2, selectionColor);
						}
						else if (currentSelection.x == 1) {
							if (GetAsyncKeyState(VK_PRIOR)) uConfig.bEnableAdaptiveMouse = false;
							if (GetAsyncKeyState(VK_NEXT)) uConfig.bEnableAdaptiveMouse = true;
							DrawBox(bAdaptiveMouse.box, 2, selectionColor);
						}
						else {
							if (GetAsyncKeyState(VK_PRIOR)) uConfig.bEnableLockedMouseMulti = false;
							if (GetAsyncKeyState(VK_NEXT)) uConfig.bEnableLockedMouseMulti = true;
							DrawBox(bLockedMouse.box, 2, selectionColor);
						}
						break;
					}

					//Text
					char sFps[48] = "FPS Cap: ";
					char sFpsMen[48] = "Menu FPS Cap: ";
					char sFpsMov[48] = "Movie FPS Cap: ";
					char sFpsNoFo[48] = "No Focus FPS Cap: ";
					char sFov[48] = "Field of View: ";
					char sGamma[48] = "Gamma: ";
					char sigm0[48] = "IGM [0] Override: ";
					char sigm1[48] = "IGM [1] Override: ";
					char sigm2[48] = "IGM [2] Override: ";
					char sEase[48] = "IGM Easing Time: ";
					char sMulti[48] = "Input Multiplier: ";
					char sAMouse[48] = "Adapt Mouse";
					char sLMouse[48] = "Lcked Mouse";
					char sbEase[48] = "Easing";

					size_t sos = 48 * sizeof(char);

					strncat(sFps, std::to_string(gFPS.pBar.current).c_str(), sos);
					strncat(sFpsMen, std::to_string(gFPSMen.pBar.current).c_str(), sos);
					strncat(sFpsMov, std::to_string(gFPSMov.pBar.current).c_str(), sos);
					strncat(sFpsNoFo, std::to_string(gFPSNoFo.pBar.current).c_str(), sos);
					strncat(sFov, std::to_string(gFOV.pBar.current).c_str(), sos);
					strncat(sGamma, std::to_string(gGamma.pBar.current).c_str(), sos);
					strncat(sigm0, std::to_string(gigmState0.pBar.current).c_str(), sos);
					strncat(sigm1, std::to_string(gigmState1.pBar.current).c_str(), sos);
					strncat(sigm2, std::to_string(gigmState2.pBar.current).c_str(), sos);
					strncat(sEase, std::to_string(gEasingTime.pBar.current).c_str(), sos);
					strncat(sMulti, std::to_string(gMouseMulti.pBar.current).c_str(), sos);

					DrawString("Config", Vector2(configTabBox.box.Pos.x + 4, configTabBox.box.Pos.y), text1, pFontSmall);
					DrawString("Stats", Vector2(statsTabBox.box.Pos.x + 4, configTabBox.box.Pos.y), text1, pFontSmall);
					DrawString(sFps, gFPS.box.Pos, text, pFontSmaller);
					DrawString(sFpsMen, gFPSMen.box.Pos, text, pFontSmaller);
					DrawString(sFpsMov, gFPSMov.box.Pos, text, pFontSmaller);
					DrawString(sFpsNoFo, gFPSNoFo.box.Pos, text, pFontSmaller);
					DrawString(sFov, gFOV.box.Pos, text, pFontSmaller);
					DrawString(sGamma, gGamma.box.Pos, text, pFontSmaller);
					DrawString(sigm0, gigmState0.box.Pos, text, pFontSmaller);
					DrawString(sigm1, gigmState1.box.Pos, text, pFontSmaller);
					DrawString(sigm2, gigmState2.box.Pos, text, pFontSmaller);
					DrawString(sEase, gEasingTime.box.Pos, text, pFontSmaller);
					DrawString(sMulti, gMouseMulti.box.Pos, text, pFontSmaller);
					DrawString(sAMouse, bAdaptiveMouse.box.Pos + (bAdaptiveMouse.box.Size / 4), text, pFontSmallest);
					DrawString(sLMouse, bLockedMouse.box.Pos + (bLockedMouse.box.Size / 4), text, pFontSmallest);
					DrawString(sbEase, bEasing.box.Pos + (bEasing.box.Size / 4), text, pFontSmallest);

					break;
				}
			}
		}

	};

}

#endif


