#include "sdk.h"
#include "hooks.h"
#include "Menu.h"
#include "global.h"
#include "Hitmarker.h"
#include "ESP.h"
#include "ImGUI\imconfig.h"
#include "ImGUI\imgui.h"
#include "ImGUI\imgui_internal.h"
#include "ImGUI\stb_rect_pack.h"
#include "ImGUI\stb_textedit.h"
#include "ImGUI\stb_truetype.h"
#include "ImGUI\DX9\imgui_impl_dx9.h"
#include "Items.h"
#include "ImgsByte.h"
#include "Config.h"
#include "GameUtils.h"
#include "Fonts.h"
#include "Inventory_changer.h"
#define nl ImGui::NewLine();
#define sl ImGui::SameLine();
typedef void(*CL_FullUpdate_t)(void);
CL_FullUpdate_t CL_FullUpdate = nullptr;
void DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float&
	flRainbow) {
	ImDrawList *windowDrawList = ImGui::GetWindowDrawList();
	Color colColor(255, 255, 255, 255);
	flRainbow += flSpeed;
	if (flRainbow > 1.f) {
		flRainbow = 0.f;
	}
	for (int i = 0; i < width; i++) {
		float hue = (1.f / (float)width) *i;
		hue -= flRainbow;
		if (hue < 0.f) {
			hue += 1.f;
		}
		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height),
			colRainbow.GetU32());
	}
}

//-------------------------------------------------------------------------
void KnifeApplyCallbk() {
	static auto CL_FullUpdate = reinterpret_cast < CL_FullUpdate_t >
		(FindPattern(XorStr("engine.dll"), reinterpret_cast < PBYTE > (
			"\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x56\xFF\x50\x14\x8B\x34\x85"),
			"x????x????xxxxxxx"));
	CL_FullUpdate();
}

//-------------------------------------------------------------------------
PresentFn oPresent;
ImFont *Impact;
ImFont *Default;
ImFont *Tabs;
ImFont *csgo_icons;
ImFont *badcache;
ImFont *TabsGetZe;
ImFont *TabsGetZe2;
ImFont *GlobalFont;
IDirect3DTexture9 *LogoImage = nullptr;
tReset oResetScene;
ImGuiWindowFlags zeusflag = ImGuiWindowFlags_ChildZeus;
void GUI_Init(IDirect3DDevice9 *pDevice) {
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX9_Init(G::Window, pDevice);
	ImGuiStyle& style = ImGui::GetStyle();
	Default = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Ruda-Bold.ttf", 14.0f);
	Tabs = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Skeet-tabs.ttf", 40.0f);
	csgo_icons = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\csgo_icons.ttf", 40.0f);
	TabsGetZe2 = io.Fonts->AddFontFromMemoryCompressedBase85TTF(TabsFontGetZe,
		48.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	TabsGetZe = io.Fonts->AddFontFromMemoryCompressedBase85TTF(TabsFontGetZe,
		35.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	TabsGetZe2 = io.Fonts->AddFontFromMemoryCompressedBase85TTF(TabsFontGetZe,
		48.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	GlobalFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(Pharamcy, 10.0f,
		NULL, io.Fonts->GetGlyphRangesCyrillic());
	if (LogoImage == nullptr) {
		D3DXCreateTextureFromFileInMemoryEx(pDevice, &logobyte, sizeof(logobyte),
			45, 45, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
			D3DX_DEFAULT, 0, NULL, NULL, &LogoImage);
	}
	ImGuiStyle& styled = ImGui::GetStyle();
	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildWindowRounding = 3.0f;
	style.FramePadding = ImVec2(4, 1);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(15, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 0.1f;
	style.GrabRounding = 16.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.AntiAliasedShapes = true;
	style.CurveTessellationTol = 1.25f;
	G::Init = true;
}
//-------------------------------------------------------------------------
void TextColor(bool active) {
	auto& style = ImGui::GetStyle();
	if (active) {
		style.Colors[ImGuiCol_Text] = ImColor(205, 205, 205, 255);
	}
	else {
		style.Colors[ImGuiCol_Text] = ImColor(80, 80, 80, 255);
	}
}

//-------------------------------------------------------------------------
void buttonColor(bool bActive) {
	ImGuiStyle& style = ImGui::GetStyle();
	if (bActive) {
		style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_WindowBg];
		style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_WindowBg];
		style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_WindowBg];
	}
	else {
		style.Colors[ImGuiCol_Button] = ImColor(26, 25, 27);
		style.Colors[ImGuiCol_ButtonHovered] = ImColor(26, 25, 27);
		style.Colors[ImGuiCol_ButtonActive] = ImColor(26, 25, 27);
	}
}

//-------------------------------------------------------------------------
void lineColor(bool bActive) {
	ImGuiStyle& style = ImGui::GetStyle();
	if (bActive) {
		style.Colors[ImGuiCol_Button] = ImColor(60, 60, 60);
		style.Colors[ImGuiCol_ButtonHovered] = ImColor(60, 60, 60);
		style.Colors[ImGuiCol_ButtonActive] = ImColor(60, 60, 60);
	}
	else {
		style.Colors[ImGuiCol_Button] = ImColor(26, 25, 27);
		style.Colors[ImGuiCol_ButtonHovered] = ImColor(26, 25, 27);
		style.Colors[ImGuiCol_ButtonActive] = ImColor(26, 25, 26);
	}
}

//-------------------------------------------------------------------------
void lineColor1(bool bActive) {
	ImGuiStyle& style = ImGui::GetStyle();
	if (bActive) {
		style.Colors[ImGuiCol_Button] = ImColor(60, 60, 60);
		style.Colors[ImGuiCol_ButtonHovered] = ImColor(60, 60, 60);
		style.Colors[ImGuiCol_ButtonActive] = ImColor(60, 60, 60);
	}
	else {
		style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_WindowBg];
		style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_WindowBg];
		style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_WindowBg];
	}
}

//-------------------------------------------------------------------------
void BtnNormalTab() {
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImVec4(0.13f, 0.13f, 0.13f, 0.f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.13f, 0.13f, 0.f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.13f, 0.13f, 0.f);
	style.Colors[ImGuiCol_Text] = ImColor(90, 90, 90, 255);
}

//-------------------------------------------------------------------------
void Border(bool active) {
	auto& style = ImGui::GetStyle();
	if (active) {
		style.Colors[ImGuiCol_Border] = ImColor(80, 80, 80, 0);
	}
	else {
		style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
	}
}void BtnActiveTab() {
	int MenuRed, MenuGreen, MenuBlue;
	MenuRed = Clientvariables->Colors.MenuColor[0] * 255;
	MenuGreen = Clientvariables->Colors.MenuColor[1] * 255;
	MenuBlue = Clientvariables->Colors.MenuColor[2] * 255;
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImColor(0.13f, 0.13f, 0.13f, 0.f);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(0.13f, 0.13f, 0.13f, 0.f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.13f, 0.13f, 0.f);
	//style.Colors[ImGuiCol_Text] = ImColor(MenuRed, MenuGreen, MenuBlue, 255);
	style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
}

//-------------------------------------------------------------------------
void BtnActive() {
	int MenuRed, MenuGreen, MenuBlue;
	MenuRed = Clientvariables->Colors.MenuColor[0] * 255;
	MenuGreen = Clientvariables->Colors.MenuColor[1] * 255;
	MenuBlue = Clientvariables->Colors.MenuColor[2] * 255;
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImColor(0.13f, 0.13f, 0.13f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(0.13f, 0.13f, 0.13f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
	//style.Colors[ImGuiCol_Text] = ImColor(MenuRed, MenuGreen, MenuBlue, 255);
	style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
}

//-------------------------------------------------------------------------
void BtnNormal() {
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
	style.Colors[ImGuiCol_Text] = ImColor(90, 90, 90, 255);
}

//-------------------------------------------------------------------------
void DrawRageTab() {
	auto& style = ImGui::GetStyle();
	TextColor(true);
	nl
		ImGui::BeginChild("aimbot", ImVec2(175, 310), true, zeusflag);
	{
		style.ItemSpacing = ImVec2(0, 0);
		static int ragebottober = 0;
		static int aas = 0;
		TextColor(ragebottober == 0);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (ragebottober == 0) {
			BtnActive();
		}
		else {
			BtnNormal();
		} if (ImGui::Button("main", ImVec2(170 / 3, 20))) {
			ragebottober = 0;
		}
		sl TextColor(ragebottober == 1);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (ragebottober == 1) {
			BtnActive();
		}
		else {
			BtnNormal();
		} if (ImGui::Button("accuracy", ImVec2(170 / 3, 20))) {
			ragebottober = 1;
		}
		sl
			TextColor(ragebottober == 2);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (ragebottober == 2) {
			BtnActive();
		}
		else {
			BtnNormal();
		} if (ImGui::Button("hitscan", ImVec2(170 / 3, 20))) {
			ragebottober = 2;
		}
		TextColor(true);
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::NewLine();
		style.ItemSpacing = ImVec2(7, 7);
		if (ragebottober == 0) {
			ImGui::Checkbox("enable", &Clientvariables->Ragebot.EnableAimbot);
			ImGui::Checkbox("team fire", &Clientvariables->Ragebot.FriendlyFire);
			ImGui::Checkbox("auto fire", &Clientvariables->Ragebot.AutomaticFire);
			ImGui::Text("aim selection");
			ImGui::Combo("##selection", &Clientvariables->Ragebot.AimbotSelection,
				SelectionMode, ARRAYSIZE(SelectionMode));
			ImGui::Checkbox("silent aim", &Clientvariables->Ragebot.SilentAimbot);
			ImGui::Checkbox("autowall", &Clientvariables->Ragebot.Autowall);
			ImGui::Text("min damage");
			ImGui::SliderInt("##mindamage", &Clientvariables->Ragebot.Mindamage, 1,
				101, "%.0f%%");
			/*ImGui::Checkbox("delay shot", &Clientvariables->Ragebot.PositionAdjustment);
			if (Clientvariables->Ragebot.PositionAdjustment)
			{
			ImGui::SliderInt("##delay", &Clientvariables->Ragebot.EnginePred1, 0, 10, "%.fms%");
			}*/
		}
		if (ragebottober == 1) {
			ImGui::Checkbox("remove spread", &Clientvariables->Ragebot.NoSpread);
			ImGui::Checkbox("remove recoil", &Clientvariables->Ragebot.NoRecoil);
			ImGui::Combo("resolver", &Clientvariables->Ragebot.ResolverMode,
				Resolvers, ARRAYSIZE(Resolvers));
			ImGui::Checkbox("auto scope", &Clientvariables->Ragebot.AutomaticScope)
				;
			ImGui::Checkbox("engine prediction", &Clientvariables
				->Ragebot.EnginePred);
			ImGui::Checkbox("autowall hitscan", &Clientvariables
				->Ragebot.AutowallHitscan);
			ImGui::Text("hit chance");
			ImGui::SliderInt("##hitchance", &Clientvariables->Ragebot.Minhitchance,
				0, 100, "%.0f%%");
		}
		if (ragebottober == 2) {
			ImGui::Text("hitscan");
			ImGui::Checkbox("multipoint", &Clientvariables->Ragebot.enablescale);
			ImGui::Combo("##hitbox", &Clientvariables->Ragebot.Hitbox, HitboxMode,
				ARRAYSIZE(HitboxMode));
			ImGui::Text("head scale");
			ImGui::SliderFloat("##headscale", &Clientvariables->Ragebot.Headscale,
				0.f, 1.f, "%.2f%%");
			ImGui::Combo("##multibox", &Clientvariables->Ragebot.Hitscan,
				HitscanMode, ARRAYSIZE(HitscanMode));
			if (Clientvariables->Ragebot.Hitscan == 4) {
				ImGui::Text("bones to hitscan");
				nl;
				ImGui::Selectable("head", &Clientvariables->Ragebot.HeadH);
				ImGui::Selectable("neck", &Clientvariables->Ragebot.NeckH);
				ImGui::Selectable("body", &Clientvariables->Ragebot.BodyH);
				ImGui::Selectable("arms", &Clientvariables->Ragebot.ArmsH);
				ImGui::Selectable("legs", &Clientvariables->Ragebot.LegsH);
				ImGui::Selectable("foot", &Clientvariables->Ragebot.FootsH);
			}
		}
	}
	ImGui::EndChild();
	sl
		ImGui::BeginChild("anti aim", ImVec2(175, 310), true, zeusflag);
	{
		ImGui::Checkbox("enable", &Clientvariables->Antiaim.AntiaimEnable);
		ImGui::Text("pitch");
		ImGui::Combo("##pitch", &Clientvariables->Antiaim.Pitch, AntiaimbotPitch,
			ARRAYSIZE(AntiaimbotPitch));
		ImGui::SliderInt("##pitchadd", &Clientvariables
			->Antiaim.pitchadd, -1800, 1800);
		ImGui::Text("anti aim ");
		ImGui::Combo("##freestanding", &Clientvariables->Antiaim.Freestanding,
			Freestanding, ARRAYSIZE(Freestanding));
		ImGui::Spacing();
		if (Clientvariables->Antiaim.Freestanding == 1) {
			ImGui::Text("yaw");
			ImGui::Combo("##yaw", &Clientvariables->Antiaim.Yaw, AntiaimbotYaw,
				ARRAYSIZE(AntiaimbotYaw));
			ImGui::SliderInt("##yawadd", &Clientvariables->Antiaim.YawAdd, -180,
				180);
			ImGui::Text("move");
			ImGui::Combo("##yawrunning", &Clientvariables->Antiaim.YawRunning,
				AntiaimbotYawRun, ARRAYSIZE(AntiaimbotYawRun));
			ImGui::SliderInt("##yawrunadd", &Clientvariables->Antiaim.YawRunningAdd,
				-180, 180);
			ImGui::Text("fake");
			ImGui::Combo("##fakeyaw", &Clientvariables->Antiaim.FakeYaw,
				AntiaimbotYawFake, ARRAYSIZE(AntiaimbotYawFake));
			ImGui::SliderInt("##yawfakeadd", &Clientvariables->Antiaim.FakeYawAdd, -
				180, 180);
			ImGui::Text("lby delta");
			ImGui::SliderInt("##freestand", &Clientvariables
				->Antiaim.FreestandingDelta, -180, 180);
			ImGui::Text("at target");
			ImGui::Combo("##dynamic", &Clientvariables->Antiaim.AtPlayer, AtTarget,
				ARRAYSIZE(AtTarget));
		}
		if (Clientvariables->Antiaim.Freestanding == 2) {
			ImGui::Text("fake");
			ImGui::Combo("##fakeyaw", &Clientvariables->Antiaim.FakeYaw,
				AntiaimbotYawFake, ARRAYSIZE(AntiaimbotYawFake));
			ImGui::SliderInt("##yawfakeadd", &Clientvariables->Antiaim.FakeYawAdd, -
				180, 180);
			ImGui::Text("lby delta");
			ImGui::SliderInt("##freestand", &Clientvariables
				->Antiaim.FreestandingDelta, -180, 180);
		}
		if (Clientvariables->Antiaim.Freestanding == 3) {
			ImGui::Text("fake");
			ImGui::Combo("##fakeyaw", &Clientvariables->Antiaim.FakeYaw,
				AntiaimbotYawFake, ARRAYSIZE(AntiaimbotYawFake));
			ImGui::SliderInt("##yawfakeadd", &Clientvariables->Antiaim.FakeYawAdd, -
				180, 180);
			ImGui::Text("lby delta");
			ImGui::SliderInt("##freestand", &Clientvariables
				->Antiaim.FreestandingDelta, -180, 180);
		}
		ImGui::Checkbox("fakewalk", &Clientvariables->Misc.FakewalkEnable);
		if (Clientvariables->Misc.FakewalkEnable) {
			ImGui::Hotkey("##walkkey", &Clientvariables->Misc.FakewalkKey, ImVec2(150,
				20));
		}
	}

	//-------------------------------------------------------------------------

	ImGui::EndChild();
	sl
		ImGui::BeginChild("body aim", ImVec2(175, 310), true, zeusflag);
	{
		ImGui::Text("body scale");
		ImGui::SliderFloat("##bodyscale", &Clientvariables->Ragebot.Bodyscale, 0.f,
			1.f, "%.2f%%");
	}

	//-------------------------------------------------------------------------

	ImGui::EndChild();
}
#define M_PI       3.14159265358979323846   // pi
void ButtonColor(int r, int g, int b)
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Button] = ImColor(r, g, b);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(r, g, b);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(r, g, b);
}
void color()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
	style.Colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, 255);
	style.Colors[ImGuiCol_ChildWindowBg] = ImColor(10, 10, 10, 255);
	style.Colors[ImGuiCol_Border] = ImColor(60, 60, 60, 255);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 0.88f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.15f, 0.60f, 0.78f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(1.30f, 1.30f, 1.30f, 1.f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.02f, 0.60f, 0.22f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	
}

void styled()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.f;
	style.FramePadding = ImVec2(0, 0);
	style.WindowPadding = ImVec2(0, 0);
	style.ItemSpacing = ImVec2(0, 0);
	style.ScrollbarSize = 10.f;
	style.ScrollbarRounding = 0.f;
	style.GrabMinSize = 5.f;
}
Vector MEME(Vector EntityPos, Vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool * viewCheck)
{
	float r_1, r_2;
	float x_1, y_1;
	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;
	float Yaw = angle - 90.0f;
	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;
	*viewCheck = y_1 < 0;
	x_1 *= zoom;
	y_1 *= zoom;
	int sizX = sizeX / 2;
	int sizY = sizeY / 2;
	x_1 += sizX;
	y_1 += sizY;
	if (x_1 < 5) x_1 = 5;
	if (x_1 > sizeX - 5) x_1 = sizeX - 5;
	if (y_1 < 5) y_1 = 5;
	if (y_1 > sizeY - 5) y_1 = sizeY - 5;
	x_1 += posX;
	y_1 += posY;
	return Vector(x_1, y_1, 0);
}
bool SkinsTab = false;
bool ColorsTab = false;
void Drawskins() {
	nl;
	auto& style = ImGui::GetStyle();
	TextColor(true);
	style.ItemSpacing = ImVec2(7, 7);
	ImGui::BeginChild("main", ImVec2(175, 310), true, zeusflag);
	{
		if (ImGui::Button("ak47")) {
			g_inve.move(WEAPON_AK47, 180);
			g_inve.move(WEAPON_AWP, 255);
		}
	

	}
	ImGui::EndChild();
	// legit bot lol
	// hoope nn coder
}
//-------------------------------------------------------------------------
bool test1;
bool test2;
bool test3;
bool test4;
void DrawLegitTab() {
	nl;
	auto& style = ImGui::GetStyle();
	TextColor(true);
	style.ItemSpacing = ImVec2(7, 7);
	if (ImGui::Button("Pistols", ImVec2(177 / 2, 20))) {
		test1 = true;
		test2 = false;
		test3 = false;
		test4 = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rifles", ImVec2(177 / 2, 20))) {
		test1 = false;
		test2 = true;
		test3 = false;
		test4 = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("", ImVec2(177 / 2, 20))){
		test1 = false;
		test2 = false;
		test3 = true;
		test4 = false;
	}
	ImGui::SameLine();
	if(ImGui::Button("Sniper", ImVec2(177 / 2, 20))) {
		test1 = false;
		test2 = false;
		test3 = false;
		test4 = true;
	}
	ImGui::SameLine();
	ImGui::NewLine();
	if (test1) {
		ImGui::BeginChild("main", ImVec2(175, 310), true, zeusflag);
		{
			ImGui::Checkbox("Enable Legitbot", &Clientvariables
				->LegitbotPistols.EnableAimbot);
			ImGui::Text("key");
			ImGui::Hotkey("##LegitBotkey", &Clientvariables->LegitbotPistols.LegitBotkey, ImVec2(140,
				20));
			ImGui::Text("Fov");
			ImGui::SliderFloat("##fov", &Clientvariables->LegitbotPistols.fov,
				0.f, 100.f, "%.1f%%");
			ImGui::Text("Smooth");
			ImGui::SliderFloat("##Smooth", &Clientvariables->LegitbotPistols.Smooth,
				0.f, 35.f, "%.1f%%");
			ImGui::Combo("##hitbox", &Clientvariables->LegitbotPistols.hitboxtype, hitboxtype,
				ARRAYSIZE(hitboxtype));
			ImGui::Checkbox("RCS", &Clientvariables
				->LegitbotPistols.RCS);
			ImGui::Text("RCS Y");
			ImGui::SliderFloat("##RCSy", &Clientvariables->LegitbotPistols.RCSy,
				0.00f, 2.00f, "%.01f%%");
			ImGui::Text("RCS X");
			ImGui::SliderFloat("##RCSx", &Clientvariables->LegitbotPistols.RCSx,
				0.00f, 2.00f, "%.01f%%");

			//		ImGui::Checkbox("test ", &Clientvariables
			//		->LegitbotPistolsRifles.test);

		}
		ImGui::EndChild();
		ImGui::SameLine();
		/*ImGui::BeginChild("misc", ImVec2(175, 310), true, zeusflag);
		{
			ImGui::NewLine();
			ImGui::Checkbox("FakeLag On Key", &Clientvariables
				->LegitbotPistols.FakeLag);
			ImGui::Hotkey("##FakeLagOnKey", &Clientvariables->LegitbotPistols.FakeLagOnKey, ImVec2(140,
				20));


		}
		ImGui::EndChild();*/
	}
	else if (test2)
	{
		ImGui::BeginChild("main", ImVec2(175, 310), true, zeusflag);
		{
			ImGui::Checkbox("Enable Legitbot", &Clientvariables
				->LegitbotRifles.EnableAimbot);
			ImGui::Text("key");
			ImGui::Hotkey("##LegitBotkey", &Clientvariables->LegitbotRifles.LegitBotkey, ImVec2(140,
				20));
			ImGui::Text("Fov");
			ImGui::SliderFloat("##fov", &Clientvariables->LegitbotRifles.fov,
				0.f, 100.f, "%.1f%%");
			ImGui::Text("Smooth");
			ImGui::SliderFloat("##Smooth", &Clientvariables->LegitbotRifles.Smooth,
				0.f, 35.f, "%.1f%%");
			ImGui::Combo("##hitbox", &Clientvariables->LegitbotRifles.hitboxtype, hitboxtype,
				ARRAYSIZE(hitboxtype));
			ImGui::Checkbox("RCS", &Clientvariables
				->LegitbotRifles.RCS);
			ImGui::Text("RCS Y");
			ImGui::SliderFloat("##RCSy", &Clientvariables->LegitbotRifles.RCSy,
				0.00f, 2.00f, "%.01f%%");
			ImGui::Text("RCS X");
			ImGui::SliderFloat("##RCSx", &Clientvariables->LegitbotRifles.RCSx,
				0.00f, 2.00f, "%.01f%%");
		

			//		ImGui::Checkbox("test ", &Clientvariables
			//		->LegitbotRifles.test);

		}
		ImGui::EndChild();
		ImGui::SameLine();
		/*ImGui::BeginChild("misc", ImVec2(175, 310), true, zeusflag);
		{
			ImGui::NewLine();
			ImGui::Checkbox("FakeLag On Key", &Clientvariables
				->LegitbotRifles.FakeLag);
			ImGui::Hotkey("##FakeLagOnKey", &Clientvariables->LegitbotRifles.FakeLagOnKey, ImVec2(140,
				20));
			ImGui::SliderFloat("##FakeLaga", &Clientvariables->LegitbotRifles.FakeLaga,
				1.00f, 500.00f, "%1.f%%");


		}
		ImGui::EndChild();*/
	}
	else if (test4)
	{
		ImGui::BeginChild("main", ImVec2(175, 310), true, zeusflag);
		{
			ImGui::Checkbox("Enable Legitbot", &Clientvariables
				->LegitbotSniper.EnableAimbot);
			ImGui::Text("key");
			ImGui::Hotkey("##LegitBotkey", &Clientvariables->LegitbotSniper.LegitBotkey, ImVec2(140,
				20));
			ImGui::Text("Fov");
			ImGui::SliderFloat("##fov", &Clientvariables->LegitbotSniper.fov,
				0.f, 100.f, "%.1f%%");
			ImGui::Text("Smooth");
			ImGui::SliderFloat("##Smooth", &Clientvariables->LegitbotSniper.Smooth,
				0.f, 35.f, "%.1f%%");
			ImGui::Combo("##hitbox", &Clientvariables->LegitbotSniper.hitboxtype, hitboxtype,
				ARRAYSIZE(hitboxtype));
			
			ImGui::Checkbox("RCS", &Clientvariables
				->LegitbotSniper.RCS);
			ImGui::Text("RCS Y");
			ImGui::SliderFloat("##RCSy", &Clientvariables->LegitbotSniper.RCSy,
				0.00f, 2.00f, "%.01f%%");
			ImGui::Text("RCS X");
			ImGui::SliderFloat("##RCSx", &Clientvariables->LegitbotSniper.RCSx,
				0.00f, 2.00f, "%.01f%%");


			//		ImGui::Checkbox("test ", &Clientvariables
			//		->LegitbotSniperRifles.test);

		}
		ImGui::EndChild();
		ImGui::SameLine();
		/*ImGui::BeginChild("misc", ImVec2(175, 310), true, zeusflag);
		{
			ImGui::NewLine();
			ImGui::Checkbox("FakeLag On Key", &Clientvariables
				->LegitbotSniper.FakeLag);
			ImGui::Hotkey("##FakeLagOnKey", &Clientvariables->LegitbotSniper.FakeLagOnKey, ImVec2(140,
				20));
			ImGui::SliderFloat("##FakeLaga", &Clientvariables->LegitbotSniper.FakeLaga,
		1.00f, 500.00f, "%1.f%%");

		}
		ImGui::EndChild();*/
	}

	// legit bot lol
	// hoope nn coder
}
int iTab = 0;
/*ImFont* Tabs;
ImFont* Impact;
ImFont* Default;*/
//-------------------------------------------------------------------------
void DrawVisualTab() {
	auto& style = ImGui::GetStyle();
	static int visualpageWorld;
	static int visualpagePlayer;
	static int visualpageMisc;
	nl ImGui::BeginChild("world modifications", ImVec2(175, 310), true, zeusflag)
		;
	{
		style.ItemSpacing = ImVec2(0, 0);
		TextColor(visualpageWorld == 0);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (visualpageWorld == 0) {
			BtnActive();
		}
		else {
			BtnNormal();
		}

		if (ImGui::Button("effects", ImVec2(177 / 2, 20))) {
			visualpageWorld = 0;
		}
		ImGui::SameLine();
		TextColor(visualpageWorld == 1);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (visualpageWorld == 1) {
			BtnActive();
		}
		else {
			BtnNormal();
		}

		if (ImGui::Button("removals", ImVec2(177 / 2, 20))) {
			visualpageWorld = 1;
		}
		TextColor(true);
		nl if (visualpageWorld == 0) {
			style.ItemSpacing = ImVec2(7, 7);
			ImGui::Checkbox("nightmode", &Clientvariables->Visuals.nightmode);
			ImGui::Checkbox("disable post processing", &Clientvariables
				->Visuals.RemoveParticles);
			ImGui::Text("skybox");
			ImGui::Combo("##Skybox", &Clientvariables->Visuals.Skybox, Skyboxmode,
				ARRAYSIZE(Skyboxmode));
			ImGui::Text("droppedweapon");
			ImGui::Combo("##droppedweapon", &Clientvariables
				->Visuals.DroppedWeapons, DroppedWeapons, ARRAYSIZE
				(DroppedWeapons));
			ImGui::MyColorEdit3("weapon color", Clientvariables
				->Colors.DroppedWeapon, 1 << 7);
		}
		else if (visualpageWorld == 1) {
			style.ItemSpacing = ImVec2(7, 7);
			ImGui::Checkbox("remove smoke grenades", &Clientvariables
				->Visuals.Nosmoke);
			//ImGui::Checkbox("remove visual recoil", &Clientvariables->Visuals.Novisrevoil);
			ImGui::Checkbox("remove scope", &Clientvariables->Visuals.Noscope);
		}
	}
	ImGui::EndChild();
	sl;
	ImGui::BeginChild("players", ImVec2(175, 310), true, zeusflag);
	{
		style.ItemSpacing = ImVec2(0, 0);
		TextColor(visualpagePlayer == 0);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (visualpagePlayer == 0) {
			BtnActive();
		}
		else {
			BtnNormal();
		} if (ImGui::Button("esp", ImVec2(56, 20))) {
			visualpagePlayer = 0;
		}
		sl
			TextColor(visualpagePlayer == 1);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (visualpagePlayer == 1) {
			BtnActive();
		}
		else {
			BtnNormal();
		} if (ImGui::Button("esp colors", ImVec2(56, 20))) {
			visualpagePlayer = 1;
		}
		sl
			TextColor(visualpagePlayer == 2);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (visualpagePlayer == 2) {
			BtnActive();
		}
		else {
			BtnNormal();
		} if (ImGui::Button("models", ImVec2(56, 20))) {
			visualpagePlayer = 2;
		}
		TextColor(true);
		nl
			if (visualpagePlayer == 0) {
				style.ItemSpacing = ImVec2(7, 7);
				ImGui::Checkbox("enable", &Clientvariables->Visuals.EspEnable);
				ImGui::Checkbox("enemy only", &Clientvariables->Visuals.EnemyOnly);
				ImGui::Checkbox("bounding box", &Clientvariables->Visuals.BoundingBox);
				ImGui::Checkbox("health bar", &Clientvariables->Visuals.Health);
				ImGui::Checkbox("name", &Clientvariables->Visuals.Name);
				ImGui::Checkbox("weapons", &Clientvariables->Visuals.Weapon);
				ImGui::Checkbox("ammo", &Clientvariables->Visuals.Ammo);
				ImGui::Checkbox("resolver info", &Clientvariables->Visuals.Flags);
				ImGui::Checkbox("armor", &Clientvariables->Visuals.Armor);
				ImGui::Checkbox("snap lines", &Clientvariables->Visuals.SnapLines);
				ImGui::Checkbox("skeleton", &Clientvariables->Visuals.Bones);
				ImGui::Checkbox("glow", &Clientvariables->Visuals.Glow);
				if (Clientvariables->Visuals.Glow) {
					ImGui::SliderFloat("##glowforce", &Clientvariables->Visuals.Opacity,
						0.f, 1.f, "%.2f%%");
				}
				ImGui::SliderInt("hitmarker", &Clientvariables->Visuals.Hitmarker, 0,
					1);
			}
			else if (visualpagePlayer == 1) {
				style.ItemSpacing = ImVec2(0, 0);
				static int colorpages = 0;
				TextColor(colorpages == 0);
				buttonColor(false);
				style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
				if (colorpages == 0) {
					BtnActive();
				}
				else {
					BtnNormal();
				} if (ImGui::Button("part 1", ImVec2(172 / 2, 20))) {
					colorpages = 0;
				}
				sl
					TextColor(colorpages == 1);
				buttonColor(false);
				style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
				if (colorpages == 1) {
					BtnActive();
				}
				else {
					BtnNormal();
				} if (ImGui::Button("part 2", ImVec2(172 / 2, 20))) {
					colorpages = 1;
				}
				TextColor(true);
				style.ItemSpacing = ImVec2(5, 5);
				if (colorpages == 0) {
					ImGui::MyColorEdit3("box", Clientvariables->Colors.BoundingBox, 1
						<< 7);
					ImGui::MyColorEdit3("color weapons", Clientvariables
						->Colors.WeaponsCAT, 1 << 7);
					ImGui::MyColorEdit3("armor color", Clientvariables
						->Colors.ArmorColor, 1 << 7);
					ImGui::MyColorEdit3("snap line", Clientvariables->Colors.Snaplines,
						1 << 7);
					ImGui::MyColorEdit3("skeleton", Clientvariables->Colors.Skeletons,
						1 << 7);
					ImGui::MyColorEdit3("glow", Clientvariables->Colors.Glow, 1 << 7);
					ImGui::MyColorEdit3("hand chams", Clientvariables->Colors.ChamsHand,
						1 << 7);
					ImGui::MyColorEdit3("hand wireframe", Clientvariables
						->Colors.WireframeHand, 1 << 7);
					ImGui::MyColorEdit3("weapon chams", Clientvariables
						->Colors.ChamsWeapon, 1 << 7);
					ImGui::MyColorEdit3("weapon wireframe", Clientvariables
						->Colors.WireframeWeapon, 1 << 7);
					ImGui::MyColorEdit3("ghost chams color", Clientvariables
						->Colors.FakeAngleGhost, 1 << 7);
					ImGui::MyColorEdit3("chams visible", Clientvariables
						->Colors.PlayerChams, 1 << 7);
					ImGui::MyColorEdit3("chams wall", Clientvariables
						->Colors.PlayerChamsWall, 1 << 7);
					ImGui::MyColorEdit3("grenade trajectory", Clientvariables
						->Colors.GrenadePrediction, 1 << 7);
					ImGui::MyColorEdit3("bullet tracers", Clientvariables
						->Colors.Bulletracer, 1 << 7);
					//ImGui::MyColorEdit3("damage indicator", Clientvariables->Colors.DamageIndicator, 1 << 7);
				}if (colorpages == 1) {
					ImGui::MyColorEdit3("weapon spread", Clientvariables->Colors.SpreadCrosshair,
						1 << 7);
				}

				//-------------------------------------------------------------------------
				style.ItemSpacing = ImVec2(7, 7);
			}
			else if (visualpagePlayer == 2) {
				style.ItemSpacing = ImVec2(7, 7);
				ImGui::Checkbox("enable", &Clientvariables->Visuals.ChamsEnable);
				if (Clientvariables->Visuals.ChamsEnable) {
					ImGui::Combo("##texture", &Clientvariables->Visuals.ChamsStyle,
						ModelsMode, ARRAYSIZE(ModelsMode));
				}
				ImGui::Checkbox("player visible", &Clientvariables->Visuals.ChamsPlayer);
				ImGui::Checkbox("player", &Clientvariables->Visuals.ChamsPlayerWall);
				ImGui::Checkbox("hand chams", &Clientvariables->Visuals.HandEnableChams);
				if (Clientvariables->Visuals.HandEnableChams) {
					ImGui::Checkbox("hand chams", &Clientvariables->Visuals.ChamsHands);
					ImGui::Checkbox("hand wireframe", &Clientvariables
						->Visuals.ChamsHandsWireframe);
				}
				ImGui::Checkbox("weapon chams", &Clientvariables->Visuals.WeaponEnableChams);
				if (Clientvariables->Visuals.WeaponEnableChams) {
					ImGui::Checkbox("weapon chams", &Clientvariables->Visuals.WeaponChams);
					ImGui::Checkbox("weapon wireframe", &Clientvariables
						->Visuals.WeaponWireframe);
				}
			}

			//-------------------------------------------------------------------------
}

//-------------------------------------------------------------------------

ImGui::EndChild();
sl;
ImGui::BeginChild(XorStr("misc"), ImVec2(175, 310), true, zeusflag);
{
	style.ItemSpacing = ImVec2(0, 0);
	TextColor(visualpageMisc == 0);
	buttonColor(false);
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	if (visualpageMisc == 0) {
		BtnActive();
	}
	else {
		BtnNormal();
	} if (ImGui::Button("part 1", ImVec2(177 / 2, 20))) {
		visualpageMisc = 0;
	}
	ImGui::SameLine();
	TextColor(visualpageMisc == 1);
	buttonColor(false);
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	if (visualpageMisc == 1) {
		BtnActive();
	}
	else {
		BtnNormal();
	} if (ImGui::Button("part 2", ImVec2(177 / 2, 20))) {
		visualpageMisc = 1;
	}
	TextColor(true);
	nl
		if (visualpageMisc == 0) {
			style.ItemSpacing = ImVec2(7, 7);
			//ImGui::Checkbox("damage indicator", &Clientvariables->Visuals.DamageIndicators);
			ImGui::Checkbox("bullet tracers", &Clientvariables->Visuals.BulletTracers)
				;
			ImGui::Text("ghost fake angle chams");
			ImGui::Combo("##ghost", &Clientvariables->Visuals.FakeAngleGhost,
				Fakeghost, ARRAYSIZE(Fakeghost));
			ImGui::Text("thirdperson");
			ImGui::Combo("##angles", &Clientvariables->Misc.TPangles,
				ThirdpersonAngles, ARRAYSIZE(ThirdpersonAngles));
			ImGui::Hotkey("##thirdperson", &Clientvariables->Misc.TPKey, ImVec2(164,
				20));
		}
		else if (visualpageMisc == 1) {
			style.ItemSpacing = ImVec2(7, 7);
			ImGui::Checkbox("grenades", &Clientvariables->Visuals.ThrownNades);
			ImGui::Checkbox("bomb", &Clientvariables->Visuals.Bomb);
			ImGui::Checkbox("grenade trajectory", &Clientvariables
				->Visuals.GrenadePrediction);
		}
}

//-------------------------------------------------------------------------

ImGui::EndChild();
/*ImGui::Checkbox("Enemy only", &Clientvariables->Visuals.EnemyOnly);
ImGui::Checkbox("Self ESP", &Clientvariables->Visuals.LocalPlayer);
ImGui::Checkbox("Self Chams", &Clientvariables->Visuals.LocalChams);
ImGui::SameLine(216);
ImGui::MyColorEdit3("##localchams", Clientvariables->Colors.LocalChams, 1 << 7);
ImGui::Checkbox("Self Glow", &Clientvariables->Visuals.LocalGlow);
ImGui::SameLine(216);
ImGui::MyColorEdit3("##localglow", Clientvariables->Colors.LocalGlow, 1 << 7);*/
}

//-------------------------------------------------------------------------
void DrawMiscTab() {
	auto& style = ImGui::GetStyle();
	nl;
	static int miscpage;
	ImGui::BeginChild("local player", ImVec2(250, 300), true, zeusflag);
	{
		style.ItemSpacing = ImVec2(0, 0);
		TextColor(miscpage == 0);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (miscpage == 0) {
			BtnActive();
		}
		else {
			BtnNormal();
		} if (ImGui::Button("move", ImVec2(245 / 3, 20))) {
			miscpage = 0;
		}
		ImGui::SameLine();
		TextColor(miscpage == 1);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (miscpage == 1) {
			BtnActive();
		}
		else {
			BtnNormal();
		} if (ImGui::Button("lag", ImVec2(245 / 3, 20))) {
			miscpage = 1;
		}
		ImGui::SameLine();
		TextColor(miscpage == 2);
		buttonColor(false);
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (miscpage == 2) {
			BtnActive();
		}
		else {
			BtnNormal();
		} if (ImGui::Button("safety", ImVec2(245 / 3, 20))) {
			miscpage = 2;
		}
		TextColor(true);
		nl
			if (miscpage == 0) {
				style.ItemSpacing = ImVec2(7, 7);
				ImGui::Checkbox("bhop", &Clientvariables->Misc.AutoJump);
				ImGui::Checkbox("auto strafe", &Clientvariables->Misc.AutoStrafe);
				ImGui::Hotkey("##speedkey", &Clientvariables->Misc.PrespeedKey, ImVec2
				(150, 20));
				//	ImGui::SliderInt("##retrack", &Clientvariables->Misc.Retrack, 1, 8);
				ImGui::Text("anti aim left");
				ImGui::Hotkey("##leftkey", &Clientvariables->Misc.Leftside, ImVec2(150,
					20));
				ImGui::Text("anti aim right");
				ImGui::Hotkey("##rightkey", &Clientvariables->Misc.Rightside, ImVec2
				(150, 20));
				ImGui::Text("anti aim back");
				ImGui::Hotkey("##backkey", &Clientvariables->Misc.Backside, ImVec2(150,
					20));
			}
			else if (miscpage == 1) {
				style.ItemSpacing = ImVec2(7, 7);
				ImGui::Text("fake lag");
				ImGui::Checkbox("enable", &Clientvariables->Misc.FakelagEnable);
				ImGui::Combo("##mode", &Clientvariables->Misc.FakelagMode, FakelagMode,
					ARRAYSIZE(FakelagMode));
				ImGui::Text("ticks");
				ImGui::SliderInt("##choke", &Clientvariables->Misc.FakelagAmount, 1, 14);
				ImGui::Checkbox("fakelag on ground", &Clientvariables->Misc.FakelagOnground);
			}
			else if (miscpage == 2) {
				style.ItemSpacing = ImVec2(7, 7);
				TextColor(true);
				ImGui::Checkbox("anti untrusted", &Clientvariables->Misc.AntiUT);
				ImGui::Checkbox("ClanTag", &Clientvariables->Misc.clantag);
				ImGui::Combo("##clantag_", &Clientvariables->Misc.clantag_b1gware, Clantag_b1gware,
					ARRAYSIZE(Clantag_b1gware));
				ImGui::Checkbox("OBS Bypass", &Clientvariables->Misc.OBSBypass);
				ImGui::Checkbox("enable buybot", &Clientvariables->Misc.buybot);
				if (Clientvariables->Misc.buybot) {
					ImGui::Text("buy bot");
					ImGui::Combo("##buybot", &Clientvariables->Misc.buybot2, BuyBot,
						ARRAYSIZE(BuyBot));
					ImGui::Checkbox("helmet and grenades", &Clientvariables->Misc.buybot3);
				}
			}

			//-------------------------------------------------------------------------
	}

	//-------------------------------------------------------------------------

	ImGui::EndChild();
	sl;
	ImGui::BeginChild("configs", ImVec2(290, 300), true, zeusflag);
	{
		style.ItemSpacing = ImVec2(7, 7);
		//ImGui::NewLine();
		//ImGui::SameLine(40);
		//ImGui::Text("menu color");
		//ImGui::SameLine(216);
		//ImGui::MyColorEdit3("##menucolor", Clientvariables->Colors.MenuColor, 1 << 7);
		ImGui::NewLine();
		ImGui::SameLine(40);
		ImGui::Text("menu key");
		ImGui::NewLine();
		ImGui::SameLine(35);
		ImGui::Hotkey("##menukey", &Clientvariables->Misc.MenuKey, ImVec2(150, 20));
		ImGui::NewLine();
		style.Colors[ImGuiCol_Button] = ImColor(26, 28, 33, 180);
		ImGui::SameLine(40);
		ImGui::Combo("##cfg", &Clientvariables->Misc.ConfigSelection, Configs,
			ARRAYSIZE(Configs));
		nl
			style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (ImGui::Button("save cfg", ImVec2(90, 27))) {
			ConSys->SaveConfig();
		}
		sl
			style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (ImGui::Button("load cfg", ImVec2(90, 27))) {
			ConSys->LoadConfig();
		} sl
			style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		if (ImGui::Button("resset cfg", ImVec2(90, 27))) {
			ConSys->Reset();
		}
	}

	//-------------------------------------------------------------------------

	ImGui::EndChild();
}HRESULT __stdcall Hooks::D3D9_EndScene(IDirect3DDevice9 *pDevice) {
	HRESULT result = d3d9VMT->GetOriginalMethod < EndSceneFn >(42)(pDevice);
	if (!G::Init) {
		GUI_Init(pDevice);
	}
	else {
		/*if (g_pEngine->IsInGame() && g_pEngine->IsConnected())
		ImGui::GetIO().MouseDrawCursor = G::Opened;
		else
		ImGui::GetIO().MouseDrawCursor = true;*/
		ImGui_ImplDX9_NewFrame();
		auto& style = ImGui::GetStyle();
		if (G::Opened /*&& style.Alpha < 1.f*/) {
			G::ShowMenu = true;
			ImGui::GetIO().MouseDrawCursor = true;
		}
		else {
			G::ShowMenu = false;
			ImGui::GetIO().MouseDrawCursor = false;
		}
		static int tab;
		if (G::ShowMenu) {
			bool aa = true;
			ImVec2 pos;
			style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
			style.Colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, 255);
			style.Colors[ImGuiCol_ChildWindowBg] = ImColor(10, 10, 10, 255);
			style.Colors[ImGuiCol_Border] = ImColor(60, 60, 60, 255);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 0.88f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImColor(100, 100, 100, 255); //Scrollbar background color
			style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(90, 90, 90, 255); //Scrollbar color
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(80, 80, 80, 255); //Scrollbar color when hovered
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(139, 0, 0, 255); //Scrollbar color when grabed
			style.Colors[ImGuiCol_SliderGrab] = ImColor(139, 0, 0, 255); //Slider color 
			style.Colors[ImGuiCol_SliderGrabActive] = ImColor(139, 0, 0, 255);//Slider color when grabed
			style.Colors[ImGuiCol_Button] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
			style.Colors[ImGuiCol_Column] = ImVec4(1.30f, 1.30f, 1.30f, 1.f);
			style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.02f, 0.60f, 0.22f);
			style.Colors[ImGuiCol_CloseButton] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
			style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
			style.Colors[ImGuiCol_Separator] = ImColor(60, 60, 60, 255);//Color of the separator
			ImGui::SetNextWindowSize(ImVec2(649, 539));
			ImGui::Begin("skeetttt", &G::ShowMenu, ImVec2(550, 660), 0.98f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{
				
				pos = ImGui::GetWindowPos();

				style.WindowPadding = ImVec2(2, 4);
				style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);

				ImGui::SetNextWindowPos(ImVec2(pos.x - 5, pos.y - 5));
				style.Colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.f);
				ImGui::SetNextWindowSize(ImVec2(659, 549));
				ImGui::Begin("##border2", &G::ShowMenu, ImVec2(659, 549), 0.98f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_ShowBorders);
				{
					pos = ImGui::GetWindowPos();

				} ImGui::End();

				// rainbow bar
				static float flRainbow;
				float flSpeed = 0.0003f;
				int curWidth = 1;
				ImVec2 curPos = ImGui::GetCursorPos();
				ImVec2 curWindowPos = ImGui::GetWindowPos();
				curPos.x += curWindowPos.x;
				curPos.y += curWindowPos.y;
				int size;
				int y;
				g_pEngine->GetScreenSize(y, size);
					DrawRectRainbow(curPos.x - 10, curPos.y - 7, ImGui::GetWindowSize().x + size, curPos.y + -4, flSpeed, flRainbow);
				// rainbow bar

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnOffset(1, 90);


				ButtonColor(15, 15, 15);
				ImGui::Button("##upprtabs", ImVec2(60, 20));

				ImGui::SameLine();
				styled();
				color();
				ButtonColor(50, 50, 50);
				ImGui::Button("##rageupline", ImVec2(1, 35));
				ImGui::PushFont(Tabs);


				/*rage*/
				{
					if (iTab == 0) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##rageupline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));

					ButtonColor(15, 15, 15);
					if (iTab == 0) style.Colors[ImGuiCol_Text] = ImColor(200, 200, 200); else style.Colors[ImGuiCol_Text] = ImColor(80, 80, 80);
					if (ImGui::Button("Rage", ImVec2(64, 76))) iTab = 0;

					ImGui::SameLine();

					if (iTab != 0)ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##gfgfgfgfgfgf", ImVec2(1, 76));

					if (iTab == 0) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##ragedownline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));

				}

				/*legit*/
				{
					if (iTab == 1) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##lupline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));

					ButtonColor(15, 15, 15);
					if (iTab == 1) style.Colors[ImGuiCol_Text] = ImColor(200, 200, 200); else style.Colors[ImGuiCol_Text] = ImColor(80, 80, 80);
					if (ImGui::Button("Legit", ImVec2(64, 76))) iTab = 1;

					ImGui::SameLine();

					if (iTab != 1)ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##gfgfgfgfgfgf", ImVec2(1, 76));

					if (iTab == 1) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##ldownline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));
				}

				/*visuals*/
				{
					if (iTab == 2) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##vupline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));

					ButtonColor(15, 15, 15);
					if (iTab == 2) style.Colors[ImGuiCol_Text] = ImColor(200, 200, 200); else style.Colors[ImGuiCol_Text] = ImColor(80, 80, 80);
					if (ImGui::Button("Visuals", ImVec2(64, 76))) iTab = 2;
					ImGui::SameLine();

					if (iTab != 2)ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##gfgfgfgfgfgf", ImVec2(1, 76));

					if (iTab == 2) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##vdownline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));
				}

				/*misc*/
				{
					if (iTab == 3) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##mupline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));

					ButtonColor(15, 15, 15);
					if (iTab == 3) style.Colors[ImGuiCol_Text] = ImColor(200, 200, 200); else style.Colors[ImGuiCol_Text] = ImColor(80, 80, 80);
					if (ImGui::Button("Misc", ImVec2(64, 76))) iTab = 3;
					ImGui::SameLine();

					if (iTab != 3)ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##gfgfgfgfgfgf", ImVec2(1, 76));

					if (iTab == 3) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##mdownline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));
				}

				/*skins*/
				{
					if (iTab == 4) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##supline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));

					ButtonColor(15, 15, 15);
					if (iTab == 4) style.Colors[ImGuiCol_Text] = ImColor(200, 200, 200); else style.Colors[ImGuiCol_Text] = ImColor(80, 80, 80);
					if (ImGui::Button("WIP", ImVec2(64, 76))) iTab = 4;
					ImGui::SameLine();

					if (iTab != 4)ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##gfgfgfgfgfgf", ImVec2(1, 76));

					if (iTab == 4) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##sdownline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##fgfgfg", ImVec2(1, 1));
				}

				/*playerlist*/
				{
					if (iTab == 5) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##pupline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##ffgfgfg", ImVec2(1, 1));

					ButtonColor(15, 15, 15);
					if (iTab == 5) style.Colors[ImGuiCol_Text] = ImColor(200, 200, 200); else style.Colors[ImGuiCol_Text] = ImColor(80, 80, 80);
					if (ImGui::Button("WIP", ImVec2(64, 76))) iTab = 5;
					ImGui::SameLine();

					if (iTab != 5)ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##fgfgfgfgfgfgf", ImVec2(1, 76));

					if (iTab == 5) ButtonColor(50, 50, 50); else ButtonColor(15, 15, 15);
					ImGui::Button("##pdownline", ImVec2(64, 1));

					ImGui::SameLine();

					ButtonColor(50, 50, 50);
					ImGui::Button("##ffgfgfg", ImVec2(1, 1));
				}


				ImGui::PushFont(Default);
				ButtonColor(15, 15, 15);
				ImGui::Button("##upprtabs", ImVec2(64, 20));

				ImGui::SameLine();

				ButtonColor(50, 50, 50);
				ImGui::Button("##rageupline", ImVec2(1, 35));

				ImGui::NextColumn();
				style.WindowPadding = ImVec2(8, 8);
				style.ItemSpacing = ImVec2(4, 4);
				style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
				style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
				style.Colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, 255);
				style.Colors[ImGuiCol_ChildWindowBg] = ImColor(15, 15, 15, 255);
				style.Colors[ImGuiCol_Border] = ImColor(60, 60, 60, 255);
				style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f);
				style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
				style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 0.88f);
				style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
				style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
				style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
				style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
				style.Colors[ImGuiCol_ScrollbarBg] = ImColor(15, 15, 15, 255); //Scrollbar background color
				style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(100, 100, 100, 255); //Scrollbar color
				style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(90, 90, 90, 255); //Scrollbar color when hovered
				style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(80, 80, 80, 255); //Scrollbar color when grabed
				style.Colors[ImGuiCol_SliderGrab] = ImColor(139, 0, 0, 255); //Slider color 
				style.Colors[ImGuiCol_SliderGrabActive] = ImColor(139, 0, 0, 255);//Slider color when grabed
				style.Colors[ImGuiCol_Button] = ImColor(139, 0, 0, 255);
				style.Colors[ImGuiCol_ButtonHovered] = ImColor(100, 0, 0, 255);
				style.Colors[ImGuiCol_ButtonActive] = ImColor(100, 0, 0, 255);
				style.Colors[ImGuiCol_Header] = ImColor(100, 0, 0, 255);
				style.Colors[ImGuiCol_HeaderHovered] = ImColor(139, 0, 0, 255);
				style.Colors[ImGuiCol_HeaderActive] = ImColor(139, 0, 0, 255);
				style.Colors[ImGuiCol_Column] = ImVec4(1.30f, 1.30f, 1.30f, 1.f);
				style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.02f, 0.60f, 0.22f);
				style.Colors[ImGuiCol_CloseButton] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
				style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
				style.Colors[ImGuiCol_Separator] = ImColor(60, 60, 60, 255);//Color of the separator
				ImGui::BeginChild("main", ImVec2(548, 525), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
				{

					if (iTab == 0) {
						DrawRageTab();
						//DrawRageTab();
					//	ragebotTab();
					}
					else if (iTab == 1)
					{
						DrawLegitTab();
						//	hvhTab();
							//DrawAntiAimTab();
					}
					else if (iTab == 2)
					{
						DrawVisualTab();
						//	visualTab();
							//	DrawVisualsTab();
					}
					else if (iTab == 3)
					{
						DrawMiscTab();
						//miscTab();
						//	DrawMiscTab();
					}
					else if (iTab == 4)
					{
						Drawskins();
						//skinchangerTab();
					}
					else if (iTab == 5)
					{

					}
					//
					//do tabs here
				}ImGui::EndChild();

				ImGui::End();
			}
		}
		ImGui::Render();
	}
	return result;
}

//-------------------------------------------------------------------------
HRESULT __stdcall Hooks::hkdReset(IDirect3DDevice9 *pDevice,
	D3DPRESENT_PARAMETERS *pPresParam) {
	if (!G::Init) {
		return oResetScene(pDevice, pPresParam);
	}
	ImGui_ImplDX9_InvalidateDeviceObjects();
	auto newr = oResetScene(pDevice, pPresParam);
	ImGui_ImplDX9_CreateDeviceObjects();
	return newr;
}















