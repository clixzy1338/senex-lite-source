#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <dwmapi.h>
#include <d3d9.h>
#include "settings/settings.hpp"
#include "overlay/overlay-hjack.hpp"
#include "game/aimbot/Aimbot.hpp"
#include "game/Gameloop.hpp"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"


#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { NULL };
MSG messager = { NULL };
HWND my_wnd = NULL;
HWND game_wnd = NULL;

int getFps()
{
	using namespace std::chrono;
	static int count = 0;
	static auto last = high_resolution_clock::now();
	auto now = high_resolution_clock::now();
	static int fps = 0;

	count++;

	if (duration_cast<milliseconds>(now - last).count() > 1000)
	{
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}

HRESULT directx_init()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object))) exit(3);

	ZeroMemory(&p_params, sizeof(p_params));
	p_params.Windowed = TRUE;
	p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_params.hDeviceWindow = my_wnd;
	p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_params.BackBufferWidth = settings::width;
	p_params.BackBufferHeight = settings::height;
	p_params.EnableAutoDepthStencil = TRUE;
	p_params.AutoDepthStencilFormat = D3DFMT_D16;
	p_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
	{
		p_object->Release();
		exit(4);
	}

	ImGui::CreateContext();
	ImGui_ImplWin32_Init(my_wnd);
	ImGui_ImplDX9_Init(p_device);

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImFontAtlas* fontAtlas = io.Fonts;
	ImFontConfig arialConfig;
	arialConfig.FontDataOwnedByAtlas = false;
	ImFont* arialFont = fontAtlas->AddFontFromFileTTF("c:\\Windows\\Fonts\\impact.ttf", 14.0f, &arialConfig);
	io.Fonts = fontAtlas;
	io.IniFilename = 0;

	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowTitleAlign = { 0.5f, 0.5f };
	style->WindowRounding = 0.0f;
	style->WindowBorderSize = 4;
	style->FrameBorderSize = 1;
	style->FramePadding = { 0, 0 };
	style->Colors[ImGuiCol_WindowBg] = ImColor(10, 10, 10);
	style->Colors[ImGuiCol_TitleBg] = ImColor(15, 15, 15);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(15, 15, 15);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(15, 15, 15);
	style->Colors[ImGuiCol_Border] = ImColor(25, 25, 25);
	style->Colors[ImGuiCol_ChildBg] = ImColor(15, 15, 15);
	style->Colors[ImGuiCol_FrameBg] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_PopupBg] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_CheckMark] = ImColor(255, 0, 255);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(255, 0, 255);
	style->Colors[ImGuiCol_Button] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_Separator] = ImColor(25, 25, 25);
	style->Colors[ImGuiCol_SeparatorActive] = ImColor(25, 25, 25);
	style->Colors[ImGuiCol_SeparatorHovered] = ImColor(25, 25, 25);

	p_object->Release();
	return S_OK;
}

void create_overlay()
{
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		0,
		LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW),
		0,
		0,
		"husky",
		LoadIcon(0, IDI_APPLICATION)
	};
	ATOM rce = RegisterClassExA(&wcex);
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	my_wnd = gui::create_window_in_band(0, rce, L"husky", WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, 0, 0, wcex.hInstance, 0, gui::ZBID_UIACCESS);
	SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(my_wnd, &margin);
	ShowWindow(my_wnd, SW_SHOWDEFAULT);
	UpdateWindow(my_wnd);
}

void features()
{
	if (settings::aimbot::enable)
	{
		if (settings::aimbot::mouseAim && GetAsyncKeyState(settings::aimbot::current_key))
		{
			memoryaim(cache::closest_mesh);
		}

		if (settings::aimbot::crosshair)
		{
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(GetSystemMetrics(0) / 2 - 8, GetSystemMetrics(1) / 2), ImVec2(GetSystemMetrics(0) / 2 + 8, GetSystemMetrics(1) / 2), ImColor(0, 0, 0), 2.0f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(GetSystemMetrics(0) / 2, GetSystemMetrics(1) / 2 - 8), ImVec2(GetSystemMetrics(0) / 2, GetSystemMetrics(1) / 2 + 8), ImColor(0, 0, 0), 2.0f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(GetSystemMetrics(0) / 2 - 8, GetSystemMetrics(1) / 2), ImVec2(GetSystemMetrics(0) / 2 + 8, GetSystemMetrics(1) / 2), settings::colors::icCrosshairColor, 1.0f);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(GetSystemMetrics(0) / 2, GetSystemMetrics(1) / 2 - 8), ImVec2(GetSystemMetrics(0) / 2, GetSystemMetrics(1) / 2 + 8), settings::colors::icCrosshairColor, 1.0f);
		}

		if (settings::aimbot::show_fov)
		{
			ImGui::GetForegroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, ImColor(0, 0, 0), 100, 2.0f);
			ImGui::GetForegroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, settings::colors::icFovColor, 100, 1.0f);
		}
	}

	actorLoop();

	if (settings::visuals::radar)
	{
		int windowFlags;
		if (settings::show_menu)
		{
			windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
		}
		else
		{
			windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
		}

		ImGui::SetNextWindowSize({ 200, 200 });
		ImGui::Begin("##radar", 0, windowFlags);
		{
			ImVec2 winpos = ImGui::GetWindowPos();
			ImVec2 winsize = ImGui::GetWindowSize();

			ImGui::GetWindowDrawList()->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y), ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y), ImColor(25, 25, 25), 1.f);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(winpos.x, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y + winsize.y * 0.5f), ImColor(25, 25, 25), 1.f);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x, winpos.y), ImColor(25, 25, 25), 1.f);
			ImGui::GetWindowDrawList()->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y), ImColor(25, 25, 25), 1.f);
			ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y * 0.5f), 5.0f, ImColor(255, 255, 255));

			for (int i = 0; i < cache::radarPoints.size(); i++)
			{
				ImVec2 radarPos = cache::radarPoints[i];
				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(winpos.x + radarPos.x, winpos.y + radarPos.y), 5.0f, ImColor(255, 0, 0));
			}
		}
		ImGui::End();
	}
}

void renderTabs(const char* tabLabels[], int tabCount, int& currentTab)
{
	float tabWidth = (ImGui::GetWindowWidth() - (ImGui::GetStyle().FramePadding.x) * (tabCount - 1)) / tabCount;

	for (int i = 0; i < tabCount; i++)
	{
		bool selected = currentTab == i;
		if (ImGui::customTab(tabLabels[i], selected, { tabWidth, 20 })) settings::tab = i;
		ImGui::SameLine(0, 0);
	}
}

void render_menu()
{
	switch (settings::aimbot::current_aimkey)
	{
	case 0:
		settings::aimbot::current_key = VK_LBUTTON;
		break;
	case 1:
		settings::aimbot::current_key = VK_RBUTTON;
		break;
	case 2:
		settings::aimbot::current_key = VK_XBUTTON1;
		break;
	case 3:
		settings::aimbot::current_key = VK_XBUTTON2;
		break;
	case 4:
		settings::aimbot::current_key = VK_SHIFT;
		break;
	case 5:
		settings::aimbot::current_key = VK_CONTROL;
		break;
	case 6:
		settings::aimbot::current_key = VK_MENU;
		break;
	case 7:
		settings::aimbot::current_key = VK_CAPITAL;
		break;
	default:
		settings::aimbot::current_key = VK_LBUTTON; 
		break;
	}

	char watermarkText[64];
	sprintf_s(watermarkText, "Husky Private | %.i FPS", getFps());
	ImVec2 wmTextSize = ImGui::CalcTextSize(watermarkText);
	ImVec2 rectSize = ImVec2(wmTextSize.x + 2 * 10.0f, wmTextSize.y + 2 * 5.0f);
	ImVec2 rectPos = ImVec2(5, 5);
	ImGui::GetForegroundDrawList()->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y), ImColor(10, 10, 10), 0, 0);
	ImGui::GetForegroundDrawList()->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + 2), ImColor(255, 0, 255), 0, 0);
	ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 14.0f, ImVec2(rectPos.x + (rectSize.x - wmTextSize.x) / 2, rectPos.y + (rectSize.y - wmTextSize.y) / 2), ImColor(255, 255, 255), watermarkText);

	const char* tabLabels[] = { "Aimbot", "Visuals", "Colors", "Settings" };
	if (GetAsyncKeyState(VK_F2) & 1) settings::show_menu = !settings::show_menu;
	if (settings::show_menu)
	{
		ImGui::SetNextWindowSize({ 500, 350 });
		ImGui::Begin("Cheat Name Here", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		{
			ImGui::BeginGroup();
			{
				renderTabs(tabLabels, sizeof(tabLabels) / sizeof(tabLabels[0]), settings::tab);
			}
			ImGui::EndGroup();
			ImGui::Dummy(ImVec2(0, 1));

			ImGui::BeginChild("##content", {}, true);
			{
				switch (settings::tab)
				{
				case 0:
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetColumnWidth(2, 550.0f / 2);
					float width = ImGui::GetColumnWidth() - 10.0f - ImGui::GetStyle().ChildBorderSize * 2;
					ImGui::BeginChild("##aimbot", ImVec2(width, 0), true);
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
						float textWidth = ImGui::CalcTextSize("Aimbot").x;
						float availWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetCursorPosX((availWidth - textWidth) * 0.5f);
						ImGui::Text("Aimbot");
						ImGui::PopStyleColor();
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						ImGui::Checkbox("Enable Aimbot", &settings::aimbot::enable);
						if (settings::aimbot::enable)
						{
							ImGui::Checkbox("Memory Aim", &settings::aimbot::mouseAim);
							ImGui::Checkbox("Crosshair", &settings::aimbot::crosshair);
							ImGui::Checkbox("Show FOV", &settings::aimbot::show_fov);
						}
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("##aimbotOptions", ImVec2(width, 0), true);
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
						float textWidth = ImGui::CalcTextSize("Aimbot Options").x;
						float availWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetCursorPosX((availWidth - textWidth) / 2);
						ImGui::Text("Aimbot Options");
						ImGui::PopStyleColor();
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));
						ImGui::SliderFloat("FOV", &settings::aimbot::fov, 50, 500, "%.f");
						ImGui::SliderFloat("Smoothness", &settings::aimbot::smoothness, 1, 10, "%.f%");
						ImGui::Combo("Aimkey", &settings::aimbot::current_aimkey, settings::aimbot::aimkey, sizeof(settings::aimbot::aimkey) / sizeof(*settings::aimbot::aimkey));
						if (ImGui::BeginCombo("Hitbox", getHitboxTypeName(settings::aimbot::current_hitbox)))
						{
							for (auto hitboxType : hitboxValues)
							{
								if (ImGui::Selectable(getHitboxTypeName(hitboxType), settings::aimbot::current_hitbox == hitboxType)) settings::aimbot::current_hitbox = hitboxType;
							}
						}
						ImGui::EndCombo();
					}
					ImGui::EndChild();
					ImGui::Columns(1);
					break;
				}
				case 1:
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetColumnWidth(2, 550.0f / 2);
					float width = ImGui::GetColumnWidth() - 10.0f - ImGui::GetStyle().ChildBorderSize * 2;
					ImGui::BeginChild("##visual", ImVec2(width, 0), true);
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
						float textWidth = ImGui::CalcTextSize("Visuals").x;
						float availWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetCursorPosX((availWidth - textWidth) * 0.5f);
						ImGui::Text("Visuals");
						ImGui::PopStyleColor();
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						ImGui::Checkbox("Enable Visuals", &settings::visuals::enable);
						if (settings::visuals::enable)
						{
							ImGui::Checkbox("Box", &settings::visuals::box);
							ImGui::Checkbox("Skeleton", &settings::visuals::skeleton);
							ImGui::Checkbox("Line", &settings::visuals::line);
							ImGui::Checkbox("Device", &settings::visuals::platform);
							ImGui::Checkbox("Name", &settings::visuals::name);
							ImGui::Checkbox("Distance", &settings::visuals::distance);
							ImGui::Checkbox("Rank", &settings::visuals::rank);
							ImGui::Checkbox("render count", &settings::RenderCount);
						}
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("##visualsOptions", ImVec2(width, 0), true);
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
						float textWidth = ImGui::CalcTextSize("Visuals Options").x;
						float availWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetCursorPosX((availWidth - textWidth) / 2);
						ImGui::Text("Visuals Options");
						ImGui::PopStyleColor();
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						ImGui::SliderFloat("Render Distance", &settings::visuals::renderDistance, 100, 1000, "%.fm");
						if (ImGui::BeginCombo("Box Type", getBoxTypeName(settings::visuals::boxType)))
						{
							for (auto boxType : boxValues)
							{
								if (ImGui::Selectable(getBoxTypeName(boxType), settings::visuals::boxType == boxType)) settings::visuals::boxType = boxType;
							}
						}
						ImGui::EndCombo();
					}
					ImGui::EndChild();
					ImGui::Columns(1);
					break;
				}
				case 2:
				{
					ImGui::Text("Crosshair");
					ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x, 0.0f);
					ImGui::ColorEdit4("##crosshair", reinterpret_cast<float*>(&settings::colors::icCrosshairColor), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
					ImGui::Text("FOV");
					ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x, 0.0f);
					ImGui::ColorEdit4("##fov", reinterpret_cast<float*>(&settings::colors::icFovColor), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
					ImGui::Text("Box Visible");
					ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x, 0.0f);
					ImGui::ColorEdit4("##boxVisible", reinterpret_cast<float*>(&settings::colors::icBoxColorVisible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
					ImGui::Text("Box Invisible");
					ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x, 0.0f);
					ImGui::ColorEdit4("##boxInsible", reinterpret_cast<float*>(&settings::colors::icBoxColorInvisible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
					ImGui::Text("Skeleton Visible");
					ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x, 0.0f);
					ImGui::ColorEdit4("##skeletonVisible", reinterpret_cast<float*>(&settings::colors::icSkeletonColorVisible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
					ImGui::Text("Skeleton Invisible");
					ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x, 0.0f);
					ImGui::ColorEdit4("##skeletonInvisible", reinterpret_cast<float*>(&settings::colors::icSkeletonColorInvisible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
					ImGui::Text("Line Visible");
					ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x, 0.0f);
					ImGui::ColorEdit4("##lineVisible", reinterpret_cast<float*>(&settings::colors::icTracerColorVisible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
					ImGui::Text("Line Invisible");
					ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x, 0.0f);
					ImGui::ColorEdit4("##lineInvisible", reinterpret_cast<float*>(&settings::colors::icTracerColorInvisible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
					break;
				}
				case 3:
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetColumnWidth(2, 550.0f / 2);
					float width = ImGui::GetColumnWidth() - 10.0f - ImGui::GetStyle().ChildBorderSize * 2;
					ImGui::BeginChild("##info", ImVec2(width, 0), true);
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
						float textWidth = ImGui::CalcTextSize("Info").x;
						float availWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetCursorPosX((availWidth - textWidth) * 0.5f);
						ImGui::Text("Info");
						ImGui::PopStyleColor();
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						ImGui::Text("Updated: ");
						ImGui::SameLine(0, 0);
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 255, 255));
						ImGui::Text(__DATE__);
						ImGui::PopStyleColor();
						ImGui::Text("I HATE NIGGAS");
						ImGui::SameLine(0, 0);
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 255, 255));
						ImGui::Text("HUSKYS PRIVATE PASTE");
						ImGui::PopStyleColor();
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("##options", ImVec2(width, 0), true);
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 155, 155, 255));
						float textWidth = ImGui::CalcTextSize("Options").x;
						float availWidth = ImGui::GetContentRegionAvail().x;
						ImGui::SetCursorPosX((availWidth - textWidth) * 0.5f);
						ImGui::Text("Options");
						ImGui::PopStyleColor();
						ImGui::Separator();
						ImGui::Dummy(ImVec2(0, 1.0f));

						if (ImGui::Button("Unload Cheat", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
						{
							exit(0);
						}
					}
					ImGui::EndChild();
					ImGui::Columns(1);
					break;
				}
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}
}

HWND get_process_wnd(uint32_t pid)
{
	std::pair<HWND, uint32_t> params = { 0, pid };
	BOOL bresult = EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL
		{
			auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
			uint32_t processid = 0;
			if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second)
			{
				SetLastError((uint32_t)-1);
				pparams->first = hwnd;
				return FALSE;
			}
			return TRUE;
		}, (LPARAM)&params);

	if (!bresult && GetLastError() == -1 && params.first) return params.first;

	return 0;
}

WPARAM render_loop()
{
	static RECT old_rc;

	bool running = true;
	while (running)
	{
		while (PeekMessage(&messager, my_wnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&messager);
			DispatchMessage(&messager);
			if (messager.message == WM_QUIT)
			{
				running = false;
			}
		}

		if (!running) break;

		if (game_wnd == NULL) exit(0);

		HWND active_wnd = GetForegroundWindow();
		if (active_wnd == game_wnd)
		{
			HWND target_wnd = GetWindow(active_wnd, GW_HWNDPREV);
			SetWindowPos(my_wnd, target_wnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			game_wnd = get_process_wnd(Driver::process_id);
			Sleep(250);
		}

		RECT rc;
		POINT xy;
		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		rc.left = xy.x;
		rc.top = xy.y;
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(0x1))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
		{
			io.MouseDown[0] = false;
		}

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;
			settings::width = rc.right;
			settings::height = rc.bottom;
			p_params.BackBufferWidth = settings::width;
			p_params.BackBufferHeight = settings::height;
			SetWindowPos(my_wnd, (HWND)0, xy.x, xy.y, settings::width, settings::height, SWP_NOREDRAW);
			p_device->Reset(&p_params);
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			features();
			render_menu();
		}
		ImGui::EndFrame();
		ImGui::Render();

		p_device->SetRenderState(D3DRS_ZENABLE, false);
		p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

		if (p_device->BeginScene() >= 0)
		{
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_device->EndScene();
		}

		HRESULT result = p_device->Present(0, 0, 0, 0);
		if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_device->Reset(&p_params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (p_device != 0)
	{
		p_device->EndScene();
		p_device->Release();
	}

	if (p_object != 0) p_object->Release();

	DestroyWindow(my_wnd);

	return messager.wParam;
}