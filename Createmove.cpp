#include "hooks.h"
#include "global.h"
#include "Misc.h"
#include "Menu.h"
#include "BacktrackingHelper.h"
#include "Math.h"
#include "GameUtils.h"
#include "Aimbot.h"
#include "PredictionSystem.h"
#include "Antiaim.h"
#include "GrenadePrediction.h"
#include "Correction.h"
#include "Hitmarker.h"
#include "Legitbot.h"
static CPredictionSystem *Prediction = new CPredictionSystem();
void __declspec(naked) LagFix()
{
	__asm {
		push edi
		xchg dword ptr[esp], edi
		push eax
		mov eax, 77
		mov eax, dword ptr[esp]
		add esp, 4
		pop edi

		cmp esp, 0
		jne fixentity

		_emit 0x88
		_emit 0xFF

		invlpg dword ptr[eax]

		int 2

		fixentity:
		sub esp, 4
			mov dword ptr[esp], ebp

			call cleanup

			pop ebp

			ret

			cleanup :

		ret
	}
}







static int Ticks = 0;
static int LastReserve = 0;
bool __fastcall Hooks::CreateMove(void *thisptr, void *, float
	flInputSampleTime, CUserCmd *cmd) {
	if (g_pEngine->IsConnected() && g_pEngine->IsInGame()) {
		if (cmd) {
			if (!cmd->command_number) {
				return true;
			}
			g_pEngine->SetViewAngles(cmd->viewangles);
			QAngle org_view = cmd->viewangles;
			CBaseEntity *pLocalPlayer = g_pEntitylist->GetClientEntity(g_pEngine
				->GetLocalPlayer());
			if (pLocalPlayer) {
				G::LocalPlayer = pLocalPlayer;
				G::UserCmd = cmd;
				G::UserCmdForBacktracking = cmd;
				if (pLocalPlayer->GetHealth() > 0) {
					CBaseCombatWeapon *pWeapon = pLocalPlayer->GetWeapon();
					if (pWeapon) {
						PVOID pebp;
						__asm mov pebp, ebp;
						bool *pbSendPacket = (bool*)(*(PDWORD)pebp - 0x1C);
						bool& bSendPacket = *pbSendPacket;
						G::vecUnpredictedVel = G::LocalPlayer->GetVelocity();
						G::MainWeapon = pWeapon;
						G::WeaponData = pWeapon->GetCSWpnData();
						bool IsLadder = pLocalPlayer->GetMoveType() ==
							MOVETYPE_LADDER;
						G::StrafeAngle = G::UserCmd->viewangles;
						g_Aimbot->DropTarget();
						g_Aimbotv2->DropTarget();
						if (Clientvariables->Misc.Walkbot) {
							g_Misc->WalkBotCM(org_view);
						}
						g_Misc->HandleClantag();
						g_Misc->Bunnyhop();
						g_Misc->AutoStrafe();
						if (Clientvariables->Legitbot.test) {
						//	bool filpa = false;
							//filpa = !filpa;
							INetChannelInfo *nci = g_pEngine->GetNetChannelInfo();
							/*if (GetKeyState(VK_SHIFT) & 0x8000 && G::SendPacket) {
								g_ClientState->chokedcommands = 8;
								g_ClientState->m_nClientTick += 1;
							//	G::SendPacket = false;
								G::UserCmd->tick_count = 1;
							//	G::SendPacket = false;

								g_ClientState->m_nDeltaTick += 1;
							}*/
							
						}
						if ((G::UserCmd->buttons& IN_ATTACK || (G::UserCmd->buttons&
							IN_ATTACK2 && (G::MainWeapon->WeaponID() == REVOLVER || G
								::MainWeapon->IsKnife()))) && GameUtils::IsAbleToShoot()) {
							g_Aimbot->fired_in_that_tick = true;
						}
						Prediction->EnginePrediction(cmd);
						g_Aimbot->Run(pLocalPlayer, pWeapon, cmd);
						g_Aimbotv2->Run(pLocalPlayer, pWeapon, cmd);
						g_Aimbot->CompensateInaccuracies();
				//		g_Aimbotv2->RecoilControlSystem();
						g_Aimbotv2->fakelagonkey();
						if (Clientvariables->Misc.FakelagEnable) {
							g_Misc->FakeLag();
						}
						G::ForceRealAA = false;
						if (G::ChokedPackets >= 14) {
							G::SendPacket = true;
							G::ChokedPackets = 0;
							G::ForceRealAA = true;
						}
						if (!IsLadder) {
							g_Antiaim->Run(org_view);
						}
						if ((G::UserCmd->buttons& IN_ATTACK || G::UserCmd->buttons&
							IN_ATTACK2 && G::MainWeapon->WeaponID() == REVOLVER) &&
							(G::MainWeapon->IsPistol() || G::MainWeapon->WeaponID()
								== AWP || G::MainWeapon->WeaponID() == SSG08)) {
							static bool bFlip = false;
							if (bFlip) {
								if (G::MainWeapon->WeaponID() == REVOLVER) {}
								else {
									G::UserCmd->buttons &= ~IN_ATTACK;
								}
							}
							bFlip = !bFlip;
						}
						if (G::SendPacket) {
							if (Clientvariables->Misc.TPangles == 0) {
								G::AAAngle = G::UserCmd->viewangles;
							}
							G::ChokedPackets = 0;
							G::FakeAngle = G::UserCmd->viewangles;
						}
						else {
							if (Clientvariables->Misc.TPangles == 1 && !g_Aimbot
								->aimbotted_in_current_tick) {
								G::AAAngle = G::UserCmd->viewangles;
							}
							G::ChokedPackets++;
							G::RealAngle = G::UserCmd->viewangles;
						}
						if (Clientvariables->Misc.TPangles == 2) {
							G::AAAngle = QAngle(G::UserCmd->viewangles.x, G
								::LocalPlayer->LowerBodyYaw());
						}
						g_Misc->FixMovement();
						g_Misc->FixCmd();
						cmd = G::UserCmd;
						bSendPacket = G::SendPacket;
						grenade_prediction::instance().Tick(G::UserCmd->buttons);
					}
				}
			}
		}
	}
	return false;
}















