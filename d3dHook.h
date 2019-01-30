#ifndef D3DHOOK_H
#define D3DHOOK_H

#include "pch.h"
#include "D3DMenu.h"
#include "Config.h"

//TODO: Get image data from the game and belnd transparency

namespace d3dHook
{
	D3DPRESENT_PARAMETERS p_Params;

	int DirectXInit(HWND hWnd)
	{
		if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
			exit(1);

		ZeroMemory(&p_Params, sizeof(p_Params));
		p_Params.Windowed = TRUE;
		p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		p_Params.hDeviceWindow = hWnd;
		p_Params.MultiSampleType = D3DMULTISAMPLE_NONE;
		p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
		p_Params.BackBufferWidth = Width;
		p_Params.BackBufferHeight = Height;
		p_Params.EnableAutoDepthStencil = TRUE;
		p_Params.AutoDepthStencilFormat = D3DFMT_D16;

		if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
			exit(1);

		if (!p_Line)
			D3DXCreateLine(p_Device, &p_Line);

		D3DXCreateFont(p_Device, 18, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DRAFT_QUALITY, FIXED_PITCH, L"Fira Code", &pFontSmall);
		D3DXCreateFont(p_Device, 18, 0, 900, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DRAFT_QUALITY, FIXED_PITCH, L"Fira Code", &pFontLarge);
		D3DXCreateFont(p_Device, 16, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DRAFT_QUALITY, FIXED_PITCH, L"Fira Code", &pFontSmaller);
		D3DXCreateFont(p_Device, 14, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DRAFT_QUALITY, FIXED_PITCH, L"Fira Code", &pFontSmallest);

		return 0;
	}

	int Render(D3DMenu&Menu, UserConfig&uConfig, uint8_t&gameFocus, HWND&overlay, HWND&XIIWND)
	{
		p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		p_Device->BeginScene();

		Menu.step(gameFocus, overlay, uConfig);

		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			Menu.SeeMenu = !Menu.SeeMenu;
			printf("FPS Unlocker menu is now %s!\n", Menu.SeeMenu == true ? "opened" : "closed");
		}

		p_Device->EndScene();
		p_Device->PresentEx(0, 0, 0, 0, 0);

		return 0;
	}
}

#endif


