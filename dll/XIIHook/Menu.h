#pragma once

#ifndef MENU_H
#define MENU_H

#include "out/ImGui/imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "GameVars.h"

__forceinline const ImVec4 FromRGBA(float x, float y, float z, float w) {
	x /= 255; y /= 255; z /= 255; w /= 255;
	return ImVec4(x, y, z, w);
}

struct Menu
{
	uint8_t initState = 0;
	bool showMenu = true;
	float fov = 45;

	ImVec4 clear_color = ImVec4(0, 0, 0, 1.f);

	void Render(gameVars* gVars)
	{
		static bool noTitlebar = false;
		static bool noBorder = false;
		static bool noResize = false;
		static bool autoResize = false;
		static bool noMove = false;
		static bool noScrollbar = false;
		static bool noCollapse = false;
		static bool noMenu = false;
		static bool startPosSet = false;

		if (initState == 1) {
			initState = 2;
			fov = gVars->uConfig.fov;

			ImGuiStyle* theme = &ImGui::GetStyle();
			ImGui::StyleColorsDark(theme);
			theme->Colors[ImGuiCol_Text] = FromRGBA(172, 172, 172, 255);
			theme->Colors[ImGuiCol_TextDisabled] = FromRGBA(128, 128, 128, 96);
			theme->Colors[ImGuiCol_WindowBg] = FromRGBA(14, 14, 14, 170);
			theme->Colors[ImGuiCol_ChildBg] = FromRGBA(24, 24, 24, 85);
			theme->Colors[ImGuiCol_PopupBg] = FromRGBA(20, 20, 20, 170);
			theme->Colors[ImGuiCol_Border] = FromRGBA(8, 8, 8, 128);
			theme->Colors[ImGuiCol_BorderShadow] = FromRGBA(0, 0, 0, 0);
			theme->Colors[ImGuiCol_FrameBg] = FromRGBA(48, 48, 48, 85);
			theme->Colors[ImGuiCol_FrameBgHovered] = FromRGBA(56, 56, 56, 128);
			theme->Colors[ImGuiCol_FrameBgActive] = FromRGBA(32, 32, 32, 128);
			theme->Colors[ImGuiCol_TitleBg] = FromRGBA(12, 12, 12, 255);
			theme->Colors[ImGuiCol_TitleBgActive] = FromRGBA(10, 10, 10, 255);
			theme->Colors[ImGuiCol_TitleBgCollapsed] = FromRGBA(8, 8, 8, 255);
			theme->Colors[ImGuiCol_MenuBarBg] = FromRGBA(24, 24, 24, 128);
			theme->Colors[ImGuiCol_ScrollbarBg] = FromRGBA(36, 36, 36, 128);
			theme->Colors[ImGuiCol_ScrollbarGrab] = FromRGBA(48, 48, 48, 128);
			theme->Colors[ImGuiCol_ScrollbarGrabHovered] = FromRGBA(64, 64, 64, 255);
			theme->Colors[ImGuiCol_ScrollbarGrabActive] = FromRGBA(48, 48, 48, 154);
			theme->Colors[ImGuiCol_CheckMark] = FromRGBA(64, 172, 65, 172);
			theme->Colors[ImGuiCol_SliderGrab] = FromRGBA(48, 48, 48, 128);
			theme->Colors[ImGuiCol_SliderGrabActive] = FromRGBA(36, 36, 36, 255);
			theme->Colors[ImGuiCol_Button] = FromRGBA(48, 48, 48, 85);
			theme->Colors[ImGuiCol_ButtonHovered] = FromRGBA(56, 56, 56, 128);
			theme->Colors[ImGuiCol_ButtonActive] = FromRGBA(32, 32, 32, 128);
			theme->Colors[ImGuiCol_Header] = FromRGBA(14, 14, 14, 170);
			theme->Colors[ImGuiCol_HeaderHovered] = FromRGBA(14, 14, 14, 255);
			theme->Colors[ImGuiCol_HeaderActive] = FromRGBA(14, 14, 14, 128);
			theme->Colors[ImGuiCol_Separator] = FromRGBA(0, 0, 0, 128);
			theme->Colors[ImGuiCol_SeparatorHovered] = FromRGBA(0, 0, 0, 255);
			theme->Colors[ImGuiCol_SeparatorActive] = FromRGBA(0, 0, 0, 172);
			theme->Colors[ImGuiCol_ResizeGrip] = FromRGBA(96, 96, 96, 128);
			theme->Colors[ImGuiCol_ResizeGripHovered] = FromRGBA(128, 128, 128, 172);
			theme->Colors[ImGuiCol_ResizeGripActive] = FromRGBA(64, 64, 64, 128);
			theme->Colors[ImGuiCol_PlotLines] = FromRGBA(154, 154, 154, 128);
			theme->Colors[ImGuiCol_PlotLinesHovered] = FromRGBA(154, 154, 154, 255);
			theme->Colors[ImGuiCol_PlotHistogram] = FromRGBA(154, 154, 154, 64);
			theme->Colors[ImGuiCol_PlotHistogramHovered] = FromRGBA(128, 128, 128, 255);
			theme->Colors[ImGuiCol_TextSelectedBg] = FromRGBA(48, 48, 48, 128);
			theme->Colors[ImGuiCol_ModalWindowDarkening] = FromRGBA(200, 200, 200, 85);
			theme->Colors[ImGuiCol_DragDropTarget] = FromRGBA(255, 255, 0, 220);
			theme->AntiAliasedFill = true;
			theme->AntiAliasedLines = true;

			theme->ChildBorderSize = 1;
			theme->ChildRounding = 3;
			theme->FrameRounding = 3;
			theme->WindowRounding = 3;
			theme->PopupRounding = 3;
			theme->ScrollbarRounding = 0;
			theme->GrabRounding = 12;
			theme->FramePadding = ImVec2(8, 6);
			theme->WindowPadding = ImVec2(6, 6);
			theme->ItemSpacing = ImVec2(4, 8);
			theme->IndentSpacing = 24;
			theme->ScrollbarSize = 16;
			theme->GrabMinSize = 12;

			theme->WindowBorderSize = 1;
			theme->ChildBorderSize = 1;
			theme->PopupBorderSize = 1;
			theme->FrameBorderSize = 1;
		}
		else if (initState == 0) return;

		if (showMenu)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.MouseDrawCursor = true;
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImGuiWindowFlags	window_flags = 0;
			if (noTitlebar)		window_flags |= ImGuiWindowFlags_NoTitleBar;
			if (noResize)		window_flags |= ImGuiWindowFlags_NoResize;
			if (autoResize)		window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			if (noMove)			window_flags |= ImGuiWindowFlags_NoMove;
			if (noScrollbar)	window_flags |= ImGuiWindowFlags_NoScrollbar;
			if (noCollapse)		window_flags |= ImGuiWindowFlags_NoCollapse;
			if (!noMenu)		window_flags |= ImGuiWindowFlags_MenuBar;
			ImGui::SetNextWindowSize(ImVec2(600, 800), ImGuiCond_FirstUseEver);
			if (!startPosSet) { ImGui::SetNextWindowPos(ImVec2(32, 32)); startPosSet = true; }

			ImGui::GetIO().MouseDrawCursor = showMenu;
		

			ImGui::Begin("FPS Unlocker Menu", &showMenu, window_flags);

			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text("Visuals");
			ImGui::Separator();
			//ImGui::SliderDouble("Target Framerate", &targetFramerate, 1, 480);

			
			ImGui::SliderFloat("Field Of View", &fov, 0.001f, 179.998f);
			*gVars->fov = fov * RAD2DEG;

			ImGui::SliderFloat("Gamma", (float*)gVars->gamma, 0.001f, 256);
			ImGui::Separator();
			ImGui::Text("In Game Speed Multiplier");
			ImGui::Separator();
			ImGui::SliderFloat("In Game Speed Multiplier [1x]", &gVars->uConfig.igmState0Override, 0.1f, 32, "%.3f", 2.f);
			ImGui::SliderFloat("In Game Speed Multiplier [2x]", &gVars->uConfig.igmState1Override, 0.1f, 32, "%.3f", 2.f);
			ImGui::SliderFloat("In Game Speed Multiplier [4x]", &gVars->uConfig.igmState2Override, 0.1f, 32, "%.3f", 2.f);
			//ImGui::SliderDouble("Interpolation Time", &gVars->uConfig.easingTime, 0, 8);

			/*if (ImGui::BeginCombo("Easing Type", gVars->uConfig.easingType)) {
				for (int i = 0; i < NUM_EASING_TYPES; i++) {
					bool selected = (gVars->uConfig.easingType == EASING_TYPE_NAMES[i]);
					if (ImGui::Selectable(EASING_TYPE_NAMES[i], selected))
						gVars->uConfig.easingType = EASING_TYPE_NAMES[i];
						if (selected)
							ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}*/

			ImGui::Separator();
			ImGui::Text("Quality of Life");
			ImGui::Separator();
			ImGui::SliderFloat("Mouse Speed Coeficient", &gVars->uConfig.lockedMouseMulti, 0.01f, 512);


			ImGui::End();

			ImGui::Render();
			//g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
			//g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			//g_pSwapChain->Present(0, 0);
		}
		else {
			ImGuiIO& io = ImGui::GetIO();
			io.MouseDrawCursor = false;
		}

		//*gVars->targetFrameTime = 1L / targetFramerate;
		
	}
};

#endif // !MENU_H

