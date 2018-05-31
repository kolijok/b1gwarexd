#include "sdk.h"
#include "hooks.h"
#include "Menu.h"
#include "MaterialHelper.h"
#include "BacktrackingHelper.h"
#include "security.h"
#include "Math.h"
#include "global.h"
#include <dbghelp.h>
#include <tchar.h>
#include "Config.h"
#include "ScreenEffects.h"
HINSTANCE hAppInstance;
CClient *g_pClient;
CClientEntityList *g_pEntitylist;
CEngine *g_pEngine;
CClientState **g_pClientState;
CInput *g_pInput;
CModelInfo *g_pModelInfo;
CInputSystem *g_pInputSystem;
CPanel *g_pPanel;
CClientState*  g_ClientState;
CSurface *g_pSurface;
CEngineTrace *g_pEngineTrace;
CDebugOverlay *g_pDebugOverlay;
IPhysicsSurfaceProps *g_pPhysics;
CRenderView *g_pRenderView;
CClientModeShared *g_pClientMode;
CGlobalVarsBase *g_pGlobals;
CModelRender *g_pModelRender;
CGlowObjectManager *g_GlowObjManager;
CMaterialSystem *g_pMaterialSystem;
IMoveHelper *g_pMoveHelper;
CPrediction *g_pPrediction;
CGameMovement *g_pGameMovement;
IGameEventManager *g_pGameEventManager;
IEngineVGui *g_pEngineVGUI;
ICvar *g_pCvar;
CInterfaces Interfaces;
CCRC gCRC;
C_TEFireBullets *g_pFireBullet;
IViewRenderBeams *g_pViewRenderBeams;
VMT *panelVMT;
VMT *clientmodeVMT;
VMT *enginevguiVMT;
VMT *modelrenderVMT;
VMT *clientVMT;
VMT *firebulletVMT;
VMT *d3d9VMT;
VMT *enginetraceVMT;
VMT *renderviewVMT;
CVMTHookManager *g_pD3D = nullptr;
CVMTHookManager *g_pClientModeHook = nullptr;
HMODULE h_ThisModule;
void Init() {
	SetupOffsets();
	g_MaterialHelper = new CMaterialHelper();
	g_Event.RegisterSelf();
	srand(time(0));
	ConSys->CheckConfigs();
	ConSys->Handle();
	panelVMT = new VMT(g_pPanel);
	panelVMT->HookVM((void*)Hooks::PaintTraverse, 41);
	panelVMT->ApplyVMT();
	g_GlowObjManager = *(CGlowObjectManager **)(FindPatternIDA("client.dll",
		"0F 11 05 ? ? ? ? 83 C8 01 C7 05 ? ? ? ? 00 00 00 00")
		+ 3);
	clientmodeVMT = new VMT(g_pClientMode);
	clientmodeVMT->HookVM((void*)Hooks::CreateMove, 24);
	clientmodeVMT->HookVM((void*)Hooks::OverrideView, 18);
	clientmodeVMT->ApplyVMT();
	g_pClientModeHook = new CVMTHookManager((PDWORD*)g_pClientMode);
	g_pClientModeHook->HookMethod((DWORD)hkDoPostScreenSpaceEffects, 44);
	g_pClientModeHook->HookMethod((DWORD)& GGetViewModelFOV, 35);
	g_pClientModeHook->ReHook();
	renderviewVMT = new VMT(g_pRenderView);
	renderviewVMT->HookVM((void*)Hooks::scene_end, 9);
	renderviewVMT->ApplyVMT();
	modelrenderVMT = new VMT(g_pModelRender);
	modelrenderVMT->HookVM((void*)Hooks::DrawModelExecute, 21);
	modelrenderVMT->ApplyVMT();
	clientVMT = new VMT(g_pClient);
	clientVMT->HookVM((void*)Hooks::FrameStageNotify, 36);
	clientVMT->ApplyVMT();
	auto dwDevice = **(uint32_t **)(FindPatternIDA(XorStr("shaderapidx9.dll"),
		XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);
	d3d9VMT = new VMT((void*)dwDevice);
	d3d9VMT->HookVM((void*)Hooks::D3D9_EndScene, 42);
	d3d9VMT->ApplyVMT();
	g_pD3D = new CVMTHookManager(reinterpret_cast < DWORD ** > (dwDevice));
	oResetScene = reinterpret_cast < tReset > (g_pD3D->HookMethod
	(reinterpret_cast < DWORD > (Hooks::hkdReset), 16));
	Hooks::g_pOldWindowProc = (WNDPROC)SetWindowLongPtr(G::Window, GWLP_WNDPROC,
		(LONG_PTR)Hooks::WndProc);
	g_ClientState = **(CClientState***)(FindPatternIDA(XorStr("engine.dll"), "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1);
}

//-------------------------------------------------------------------------
void StartCheat() {
	if (Interfaces.GetInterfaces() && g_pPanel && g_pClientMode) {
		Sleep(500);
		Init();
	}
}

//-------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE Instance, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(Instance);
		G::Window = FindWindowA(("Valve001"), 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartCheat, 0, 0, 0);
		break;
	}
	return true;
}
#define exampleHWID 418875332
#define exampleHWID 100897102
#define exampleHWID -1532426359
#define exampleHWID -826390674


//-------------------------------------------------------------------------
MsgFn oMsg;
void __cdecl Msg(char const *msg, ...) {
	if (!oMsg) {
		oMsg = (MsgFn)GetProcAddress(GetModuleHandle(XorStr("tier0.dll")), XorStr
		("Msg"));
	}
	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf_s(buffer, msg, list);
	va_end(list);
	oMsg(buffer, list);
}















