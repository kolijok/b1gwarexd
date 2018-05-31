#include "hooks.h"
#include "GameUtils.h"
#include "Draw.h"
#include "Menu.h"
#include "ESP.h"
#include "Misc.h"
#include "global.h"
#include "SpoofedConvar.h"
#include "Hitmarker.h"
#include "GrenadePrediction.h"
#include "global.h"
#include "Antiaim.h"
#include <stdio.h>  
#include <intrin.h>  
bool first_frame_passed = false;
std::string sPanel = XorStr("FocusOverlayPanel");


void __fastcall Hooks::PaintTraverse(void* ecx, void* edx, unsigned int vgui_panel, bool force_repaint, bool allow_force)
{
	if (Clientvariables->Visuals.EspEnable && Clientvariables->Visuals.Noscope && strcmp("HudZoom", g_pPanel->GetName(vgui_panel)) == 0)
		return;

	panelVMT->GetOriginalMethod<PaintTraverseFn>(41)(ecx, vgui_panel, force_repaint, allow_force);

	const char* pszPanelName = g_pPanel->GetName(vgui_panel);

	if (!strstr(pszPanelName, sPanel.data()))
		return;
	static uintptr_t gameoverlay_return_address = 0;

	if (!gameoverlay_return_address) {
		MEMORY_BASIC_INFORMATION info;
		VirtualQuery(_ReturnAddress(), &info, sizeof(MEMORY_BASIC_INFORMATION));

		char mod[MAX_PATH];
		GetModuleFileNameA((HMODULE)info.AllocationBase, mod, MAX_PATH);

		if (strstr(mod, XorStr("gameoverlay")))
			gameoverlay_return_address = (uintptr_t)(_ReturnAddress());
	}

	if (gameoverlay_return_address != (uintptr_t)(_ReturnAddress()) && Clientvariables->Misc.OBSBypass)
	{

	


	int cur_height, cur_width;
	g_pEngine->GetScreenSize(cur_width, cur_height);

	if (!first_frame_passed || cur_width != G::Screen.width || cur_height != G::Screen.height) {
		first_frame_passed = true;
		g_Draw.Init();

		g_pEngine->GetScreenSize(cur_width, cur_height);
		G::Screen.height = cur_height;
		G::Screen.width = cur_width;
	}

	if (g_pEngine->IsInGame())
	{
		static float rainbow1 = 0.050f; rainbow1 += 0.0005f; 
				if (rainbow1 > 0.280f) rainbow1 = 0.050f;
		g_Draw.StringA(g_Draw.font_build, false, 9, 9, 255, 0, 150, 255, "%s", "b1gware");
		g_Draw.StringA(g_Draw.font_build, false, 9, 9, 255, 255, 255, 255, "%s", "b1gware");

	}

	if (g_pEngine->IsInGame() && g_pEngine->IsConnected() && G::LocalPlayer) {

		if (Clientvariables->Antiaim.Freestanding != 0) {
			if (abs(G::RealAngle.y - G::LocalPlayer->LowerBodyYaw()) > 35.f)
				g_Draw.StringA(g_Draw.font_build, false, 10, G::Screen.height - 100, 0, 255, 0, 255, "%s", "LBY");
			else
				g_Draw.StringA(g_Draw.font_build, false, 10, G::Screen.height - 100, 255, 0, 0, 255, "%s", "LBY");

		}

		int w, h;
		int centerW, centerh, topH;
		g_pEngine->GetScreenSize(w, h);
		centerW = w / 2;
		centerh = h / 2;

		if (Clientvariables->Antiaim.Freestanding == 2 && G::LocalPlayer->isAlive())	{
			int Right, Left, Back;
			Right = Clientvariables->Misc.Rightside;
			Left = Clientvariables->Misc.Leftside;
			Back = Clientvariables->Misc.Backside;

			static bool side1 = false;
			static bool side2 = false; 
			static bool inside1 = false; 
			static bool inside2 = false; 
			static bool back1 = false; 
			static bool back2 = false;

			if (GetAsyncKeyState(Right)) {
				side1 = true; //v
				side2 = false;//v
				inside1 = true; //f
				inside2 = false; //f
				back1 = false; //v
				back2 = true;//f
			}
			if (GetAsyncKeyState(Left)) {
				side1 = false; //v
				side2 = true; //v
				inside1 = false; //f
				inside2 = true; //f
				back1 = false; 
				back2 = true;
			}
			if (GetAsyncKeyState(Back)) {
				side1 = false; 
				side2 = false; 
				inside1 = true;
				inside2 = true; 
				back1 = true; 
				back2 = false;
			}

			if (side1) //v
				g_Draw.StringA(g_Draw.font_build, false, centerW + 50, centerh - 18, 0, 255, 0, 255, "%s", ">");
			if (inside1) //f
				g_Draw.StringA(g_Draw.font_build, false, centerW - 80, centerh - 18, 255, 255, 255, 255, "%s", "<");
			if (side2) //v
				g_Draw.StringA(g_Draw.font_build, false, centerW - 80, centerh - 18, 0, 255, 0, 255, "%s", "<");
			if (inside2)//f
				g_Draw.StringA(g_Draw.font_build, false, centerW + 50, centerh - 18, 255, 255, 255, 255, "%s", ">");
			if (back1) //v
				g_Draw.StringA(g_Draw.font_build, false, centerW - 10, centerh + 45, 0, 255, 0, 255, "%s", "V");
			if (back2) //f
				g_Draw.StringA(g_Draw.font_build, false, centerW - 10, centerh + 45, 255, 255, 255, 255, "%s", "V");
		}
		if (Clientvariables->Antiaim.Freestanding == 3 && G::LocalPlayer->isAlive()) {
			int Right, Left, Back;
			Right = Clientvariables->Misc.Rightside;
			Left = Clientvariables->Misc.Leftside;
			Back = Clientvariables->Misc.Backside;
			static bool side1 = false;
			static bool side2 = false;
			static bool inside1 = false;
			static bool inside2 = false;
			static bool back1 = false;
			static bool back2 = false;
			if (GetAsyncKeyState(Right)) {
				side1 = true; //v
				side2 = false; //v
				inside1 = true; //f
				inside2 = false; //f
				back1 = false; //v
				back2 = true; //f
			}
			if (GetAsyncKeyState(Left)) {
				side1 = false; //v
				side2 = true; //v
				inside1 = false; //f
				inside2 = true; //f
				back1 = false;
				back2 = true;
			}
			if (GetAsyncKeyState(Back)) {
				side1 = false;
				side2 = false;
				inside1 = true;
				inside2 = true;
				back1 = true;
				back2 = false;
			}

			if (side1) //v
				g_Draw.StringA(g_Draw.font_build, false, centerW + 50, centerh - 18, 0, 255, 0, 255, "%s", ">");
			if (inside1) //f
				g_Draw.StringA(g_Draw.font_build, false, centerW - 80, centerh - 18, 255, 255, 255, 255, "%s", "<");
			if (side2) //v
				g_Draw.StringA(g_Draw.font_build, false, centerW - 80, centerh - 18, 0, 255, 0, 255, "%s", "<");
			if (inside2)//f
				g_Draw.StringA(g_Draw.font_build, false, centerW + 50, centerh - 18, 255, 255, 255, 255, "%s", ">");
			if (back1) //v
				g_Draw.StringA(g_Draw.font_build, false, centerW - 10, centerh + 45, 0, 255, 0, 255, "%s", "V");
			if (back2) //f
				g_Draw.StringA(g_Draw.font_build, false, centerW - 10, centerh + 45, 255, 255, 255, 255, "%s", "V");
		}

		if (Clientvariables->Antiaim.Freestanding == 4 && G::LocalPlayer->isAlive()) {
			int Right, Left, Back;
			Right = Clientvariables->Misc.Rightside;
			Left = Clientvariables->Misc.Leftside;
			Back = Clientvariables->Misc.Backside;
			static bool side1 = false;
			static bool side2 = false;
			static bool inside1 = false;
			static bool inside2 = false;
			static bool back1 = false;
			static bool back2 = false;
			if (GetAsyncKeyState(Right)) {
				side1 = true; //v
				side2 = false; //v
				inside1 = true; //f
				inside2 = false; //f
				back1 = false; //v
				back2 = true; //f
			}
			if (GetAsyncKeyState(Left)) {
				side1 = false; //v
				side2 = true; //v
				inside1 = false; //f
				inside2 = true; //f
				back1 = false;
				back2 = true;
			}
			if (GetAsyncKeyState(Back)) {
				side1 = false;
				side2 = false;
				inside1 = true;
				inside2 = true;
				back1 = true;
				back2 = false;
			}

			if (side1) //v
				g_Draw.StringA(g_Draw.font_build, false, centerW + 50, centerh - 18, 0, 255, 0, 255, "%s", ">");
			if (inside1) //f
				g_Draw.StringA(g_Draw.font_build, false, centerW - 80, centerh - 18, 255, 255, 255, 255, "%s", "<");
			if (side2) //v
				g_Draw.StringA(g_Draw.font_build, false, centerW - 80, centerh - 18, 0, 255, 0, 255, "%s", "<");
			if (inside2)//f
				g_Draw.StringA(g_Draw.font_build, false, centerW + 50, centerh - 18, 255, 255, 255, 255, "%s", ">");
			if (back1) //v
				g_Draw.StringA(g_Draw.font_build, false, centerW - 10, centerh + 45, 0, 255, 0, 255, "%s", "V");
			if (back2) //f
				g_Draw.StringA(g_Draw.font_build, false, centerW - 10, centerh + 45, 255, 255, 255, 255, "%s", "V");
		}

		if (Clientvariables->Visuals.DamageIndicators) {
			G::CurrTime = g_pGlobals->interval_per_tick *(G::LocalPlayer->GetTickBase()
				+ 1);
			if (!G::DamageHit.empty()) {
				for (auto it = G::DamageHit.begin(); it != G::DamageHit.end();) {
					if (G::CurrTime > it->ExpireTime) {
						it = G::DamageHit.erase(it);
						continue;
					}
					it->Draw();
					++it;
				}
			}
		}

		//-------------------------------------------------------------------------
		static bool bSpoofed;
		if (Clientvariables->Misc.TPKey > 0 && !bSpoofed) {
			ConVar *svcheats = g_pCvar->FindVar("sv_cheats");
			SpoofedConvar *svcheatsspoof = new SpoofedConvar(svcheats);
			svcheatsspoof->SetInt(1);
			bSpoofed = true;
		}

		//-------------------------------------------------------------------------
		if (Clientvariables->Visuals.RemoveParticles) {
			auto postprocessing = g_pCvar->FindVar("mat_postprocess_enable");
			auto postprocessingspoof = new SpoofedConvar(postprocessing);
			postprocessingspoof->SetInt(0);
		}

		//-------------------------------------------------------------------------

		g_Hitmarker->draw();
		if (Clientvariables->Visuals.Noscope) {
			g_ESP->DrawScope(G::LocalPlayer);
			auto postprocessing = g_pCvar->FindVar("mat_postprocess_enable");
			auto postprocessingspoof = new SpoofedConvar(postprocessing);
			postprocessingspoof->SetInt(0);
		}

		//-------------------------------------------------------------------------
		g_ESP->Loop();
		grenade_prediction::instance().Paint();
}
}
//-------------------------------------------------------------------------

}







































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































