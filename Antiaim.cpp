#include "sdk.h"
#include "Antiaim.h"
#include "global.h"
#include "GameUtils.h"
#include "Math.h"
#include "Aimbot.h"
#include "Autowall.h"
CAntiaim *g_Antiaim = new CAntiaim;
float get3ddist2(Vector myCoords, Vector enemyCoords) {
	return sqrt(pow(double(enemyCoords.x - myCoords.x), 2.0) + pow(double
	(enemyCoords.y - myCoords.y), 2.0) + pow(double(enemyCoords.z -
		myCoords.z), 2.0));
}

//-------------------------------------------------------------------------
bool ShouldPredict() {
	INetChannelInfo *nci = g_pEngine->GetNetChannelInfo();
	float server_time = g_pGlobals->curtime + nci->GetLatency(FLOW_OUTGOING);
	if (g_pGlobals->frametime != 0.0 | G::UserCmd->hasbeenpredicted) {
		server_time++;
	}
	static bool initialized;
	bool will_update = false;
	if (!initialized && !G::LocalPlayer->IsMoving()) {
		initialized = true;
		G::PredictedTime = server_time + 0.22f;
	}
	else if (G::LocalPlayer->IsMoving()) {
		initialized = false;
	}
	if (server_time >= (G::PredictedTime) && !G::LocalPlayer->IsMoving() && *G
		::LocalPlayer->GetFlags()& FL_ONGROUND) {
		G::PredictedTime = server_time + 1.1f;
		will_update = true;
	}
	return will_update;
}

//-------------------------------------------------------------------------
void AimAtPlayers() {
	int value = G::UserCmd->viewangles.y, num = 0;
	float best_dist = G::MainWeapon->GetCSWpnData()->flRange;
	for (int i = 1; i <= g_pGlobals->maxClients; ++i) {
		if (i == g_pEngine->GetLocalPlayer()) {
			continue;
		}
		CBaseEntity *pTarget = g_pEntitylist->GetClientEntity(i);
		Vector pos;
		if (!pTarget || pTarget->GetHealth() < 1) {
			continue;
		} if (G::LocalPlayer->GetTeamNum() != pTarget->GetTeamNum()) {
			pos = pTarget->GetEyePosition();
			float dist = get3ddist2(pos, G::LocalPlayer->GetEyePosition());
			if (Clientvariables->Antiaim.AtPlayer == 1) {
				if (dist >= best_dist) {
					continue;
				}
			}
			best_dist = dist;
			QAngle angle = GameUtils::CalculateAngle(G::LocalPlayer
				->GetEyePosition(), pos);
			angle.y = Math::NormalizeYaw(angle.y);
			Math::ClampAngles(angle);
			if (Clientvariables->Antiaim.AtPlayer == 1) {
				value = angle.y;
			}
			else {
				num++;
				value += angle.y;
			}
		}
	}
	if (Clientvariables->Antiaim.AtPlayer == 2 && num != 0) {
		value /= num;
	}
	G::UserCmd->viewangles.y = value;
}

//-------------------------------------------------------------------------
void CAntiaim::Pitch_AA() {
	float temp = 0;
	int velocity = G::LocalPlayer->GetVelocity().Length();
	switch (Clientvariables->Antiaim.Pitch) {
	case 1:
		temp = 88.f;
		break;
	case 2:
		temp -= 88.9f;
		break;
	case 3:
		temp = 0.f;
		break;
	case 4:
		temp = 0.f + Math::RandomFloat2(-85.f, 85.f);
		break;
	case 5:
	{
		if (ShouldPredict()) {
			temp -= 88.999f;
		}
		else {
			temp = 86.f;
		}
	}
	break;
	case 6:
	//	temp = 64799.63712 + 1337 * 1137;
		break;
	case 7:
		//temp = 3432243424.000000;
		break;
	default:
		break;
	}
	G::UserCmd->viewangles.x = temp + Clientvariables
		->Antiaim.pitchadd;
}

//-------------------------------------------------------------------------
float BestHeadDirection(float yaw) {
	for (int i = 0; i < g_pEngine->GetMaxClients(); i++) {
		CBaseEntity *player = g_pEntitylist->GetClientEntity(i);
		if (player->IsValidTarget()) {
			QAngle CalculatedAngle = GameUtils::CalculateAngle(G::LocalPlayer
				->GetEyePosition(), player->GetOrigin());
			float Atangle = CalculatedAngle.y;
			Vector Difference = G::LocalPlayer->GetOrigin() - player->GetOrigin();
			if (Difference.y > 0) {
				return (Atangle - 85.f);
			}
			else if (Difference.y < 0) {
				return (Atangle + 85.f);
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------
float GetBestHeadAngle(float yaw) {
	float Back, Right, Left;
	Vector src3D, dst3D, forward, right, up, src, dst;
	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;
	QAngle engineViewAngles;
	g_pEngine->GetViewAngles(engineViewAngles);
	engineViewAngles.x = 0;
	Math::AngleVectors(engineViewAngles, &forward, &right, &up);
	filter.pSkip = G::LocalPlayer;
	src3D = G::LocalPlayer->GetEyePosition();
	dst3D = src3D + (forward * 384);
	ray.Init(src3D, dst3D);
	g_pEngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	Back = (tr.endpos - tr.start).Length();
	ray2.Init(src3D + right * 35, dst3D + right * 35);
	g_pEngineTrace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	Right = (tr.endpos - tr.start).Length();
	ray3.Init(src3D - right * 35, dst3D - right * 35);
	g_pEngineTrace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	Left = (tr.endpos - tr.start).Length();
	if (Left > Right) {
		return (yaw - 85);
	}
	else if (Right > Left) {
		return (yaw + 85);
	}
	else if (Back > Right || Back > Left) {
		return (yaw - 177);
	}
	return 0;
}

//-------------------------------------------------------------------------
void BreakLowerbodyFreestand() {
	Vector vEyePos;
	QAngle Angles;
	g_pEngine->GetViewAngles(Angles);
	float BestHeadPosition = GetBestHeadAngle(Angles.y);
	int LowerbodyDelta = Clientvariables->Antiaim.FreestandingDelta;
	if (G::SendPacket) {
		G::UserCmd->viewangles.y = BestHeadPosition + LowerbodyDelta + Math
			::RandomFloat2(-45.f, 45.f);
	}
	else {
		if (ShouldPredict()) {
			G::UserCmd->viewangles.y = BestHeadPosition + LowerbodyDelta;
		}
		else {
			G::UserCmd->viewangles.y = BestHeadPosition;
		}
	}
}

//-------------------------------------------------------------------------
void Manual_AA() {
	int LowerbodyDelta, Right, Left, Back;
	LowerbodyDelta = Clientvariables->Antiaim.FreestandingDelta;
	Right = Clientvariables->Misc.Rightside;
	Left = Clientvariables->Misc.Leftside;
	Back = Clientvariables->Misc.Backside;
	static bool side1 = false;
	static bool side2 = false;
	static bool back = false;
	if (GetAsyncKeyState(Right)) {
		side1 = true;
		side2 = false;
		back = false;
	}
	if (GetAsyncKeyState(Left)) {
		side1 = false;
		side2 = true;
		back = false;
	}
	if (GetAsyncKeyState(Back)) {
		side1 = false;
		side2 = false;
		back = true;
	}
	if (side1)
		if (G::SendPacket) {
			G::UserCmd->viewangles.y = LowerbodyDelta + Math::RandomFloat2(-55.f,
				55.f);
		}
		else {
			if (ShouldPredict()) {
				G::UserCmd->viewangles.y -= 90 + LowerbodyDelta;
			}
			else {
				G::UserCmd->viewangles.y -= 90;
			}
		}
		if (side2)
			if (G::SendPacket) {
				G::UserCmd->viewangles.y = LowerbodyDelta + Math::RandomFloat2(-55.f,
					55.f);
			}
			else {
				if (ShouldPredict()) {
					G::UserCmd->viewangles.y += 90 + LowerbodyDelta;
				}
				else {
					G::UserCmd->viewangles.y += 90;
				}
			}
			if (back)
				if (G::SendPacket) {
					G::UserCmd->viewangles.y = LowerbodyDelta + Math::RandomFloat2(-55.f,
						55.f);
				}
				else {
					if (ShouldPredict()) {
						G::UserCmd->viewangles.y -= 180 + LowerbodyDelta;
					}
					else {
						G::UserCmd->viewangles.y -= 180;
					}
				}
}void Jitter_AAKey() {
	int LowerbodyDelta, Right, Left, Back;
	LowerbodyDelta = Clientvariables->Antiaim.FreestandingDelta;
	Right = Clientvariables->Misc.Rightside;
	Left = Clientvariables->Misc.Leftside;
	Back = Clientvariables->Misc.Backside;
	static bool side1 = false;
	static bool side2 = false;
	static bool back = false;
	if (GetAsyncKeyState(Right)) {
		side1 = true;
		side2 = false;
		back = false;
	}
	if (GetAsyncKeyState(Left)) {
		side1 = false;
		side2 = true;
		back = false;
	}
	if (GetAsyncKeyState(Back)) {
		side1 = false;
		side2 = false;
		back = true;
	}
	if (side1)
		if (G::SendPacket) {
			G::UserCmd->viewangles.y = LowerbodyDelta + Math::RandomFloat2(-6.f,
				6.f);
		}
		else {
			if (ShouldPredict()) {
				G::UserCmd->viewangles.y -= 90 + LowerbodyDelta;
			}
			else {
				G::UserCmd->viewangles.y -= 90 + Math::RandomFloat2(-2.f, 2.f);
			}
		}
		if (side2)
			if (G::SendPacket) {
				G::UserCmd->viewangles.y = LowerbodyDelta + Math::RandomFloat2(-6.f,
					6.f);
			}
			else {
				if (ShouldPredict()) {
					G::UserCmd->viewangles.y += 90 + LowerbodyDelta;
				}
				else {
					G::UserCmd->viewangles.y += 90 + Math::RandomFloat2(-2.f, 2.f);
				}
			}
			if (back)
				if (G::SendPacket) {
					G::UserCmd->viewangles.y = LowerbodyDelta + Math::RandomFloat2(-6.f,
						6.f);
				}
				else {
					if (ShouldPredict()) {
						G::UserCmd->viewangles.y -= 180 + LowerbodyDelta;
					}
					else {
						G::UserCmd->viewangles.y -= 180 + Math::RandomFloat2(-2.f, 2.f);
					}
				}
}

//-------------------------------------------------------------------------
void CAntiaim::AntiAimYaw_Real() {
	if (G::LocalPlayer->GetVelocity().Length() > 0.1f) {
		float temp = G::UserCmd->viewangles.y;
		static float add = 0;
		static bool reset = true;
		static int Ticks = 0;
		float temp_base = temp;
		switch (Clientvariables->Antiaim.YawRunning) {
		case 1:
			temp -= 180.f;
			break;
		case 2:
			temp -= 180.f + Math::RandomFloat2(-25.f, 25.f);
			break;
		case 3:
			if (reset) {
				add = 0;
				reset = false;
			}
			temp += 135;
			temp += add;
			add += 15;
			if (temp_base + 225 < temp) {
				reset = true;
				temp = temp_base + 225;
			}
			break;
		case 4:
			temp -= Ticks;
			Ticks += 3;
			if (Ticks > 235) {
				Ticks = 125;
			} break;
		case 5:
			temp -= Ticks;
			Ticks += 5;
			if (Ticks > 360) {
				Ticks = 0;
			} break;
		case 6:
			temp = (G::LocalPlayer->LowerBodyYaw() + 45 + rand() % 33) - rand() %
				18;
			//temp -= G::LocalPlayer->LowerBodyYaw() + Math::RandomFloat2(-22.f, 22.f);
			break;
		default:
			break;
		}
		G::UserCmd->viewangles.y = temp + Clientvariables->Antiaim.YawRunningAdd;
	}
	else {
		float temp = G::UserCmd->viewangles.y;
		static float add = 0;
		static bool reset = true;
		static int Ticks = 120;
		float temp_base = temp;
		switch (Clientvariables->Antiaim.Yaw) {
		case 1:
			temp -= 180.f;
			break;
		case 2:
			temp -= 180.f + Math::RandomFloat2(-25.f, 25.f);
			break;
		case 3:
			if (reset) {
				add = 0;
				reset = false;
			}
			temp += 135;
			temp += add;
			add += 15;
			if (temp_base + 225 < temp) {
				reset = true;
				temp = temp_base + 225;
			}
			break;
		case 4:
			temp -= Ticks;
			Ticks += 3;
			if (Ticks > 235) {
				Ticks = 125;
			} break;
		case 5:
			temp -= Ticks;
			Ticks += 5;
			if (Ticks > 360) {
				Ticks = 0;
			} break;
		case 6:
			temp -= G::LocalPlayer->LowerBodyYaw() + Math::RandomFloat2(-25.f,
				25.f);
			break;
		default:
			break;
		}
		G::UserCmd->viewangles.y = temp + Clientvariables->Antiaim.YawAdd;
	}
}

//-------------------------------------------------------------------------

void FakeLowerBody135(CUserCmd *pCmd, bool& bSendPacket) {
}
void CAntiaim::AntiAimYaw_Fake() {
	float temp = G::UserCmd->viewangles.y;
	static int Ticks;
	static int Ticksa;
	static int Side;
	switch (Clientvariables->Antiaim.FakeYaw) {
	case 1:
		temp -= 180.f;
		break;
	case 2:
		temp -= G::RealAngle.y - G::RealAngle.y;
		break; case 3:
			temp -= 180.f + Math::RandomFloat2(-45.f, 45.f);
			break;
		case 4:
			temp -= Ticks;
			Ticks += 2;
			if (Ticks > 360) {
				Ticks = 0;
			} break;
		case 5:
			temp -= Ticks;
			Ticks += 3;
			if (Ticks > 235) {
				Ticks = 125;
			} break;
		case 6:
			temp = 0.f + Math::RandomFloat2(-90.f, 90.f);
			break;
		case 7:
			temp = G::LocalPlayer->LowerBodyYaw() + Math::RandomFloat2(-34.5f, 34.5f);
			break;
		case 8:
			temp = (G::LocalPlayer->LowerBodyYaw() + 140 + rand() % 123) - rand() % 20;
			break;
		case 9:
			temp = (G::LocalPlayer->LowerBodyYaw() + 135 + rand() % 78) - rand() % 24;
			break;
		case 10:
		{
			Ticks += 3;
			Ticksa += 3;
			temp = (G::LocalPlayer->LowerBodyYaw() + Ticks + rand() % Ticksa) - rand() % 33;

			if (Ticks > 177) {
				Ticks = 0;
			}
			if (Ticksa > 66) {
				Ticksa = 0;
			}
		}
		//	temp = (G::LocalPlayer->LowerBodyYaw() + 66 + rand() % 63) - rand() % 24;
		break;
		default:
			break;
	}

	//-------------------------------------------------------------------------

	G::UserCmd->viewangles.y = temp + Clientvariables->Antiaim.FakeYawAdd;
}

//-------------------------------------------------------------------------
bool GetAlive(CBaseEntity *pLocal) {
	for (int i = 1; i < 64; ++i) {
		if (i == g_pEngine->GetLocalPlayer()) {
			continue;
		}
		CBaseEntity *target = g_pEntitylist->GetClientEntity(i);
		player_info_t info;
		Vector pos;
		if (!target || target->GetHealth() < 1) {
			continue;
		} if (pLocal->GetTeamNum() != target->GetTeamNum()) {
			return true;
		}
	}
	return false;
}
inline float ClampYaw(float yaw) {
	while (yaw > 180.f)
		yaw -= 360.f;
	while (yaw < -180.f)
		yaw += 360.f;
	return yaw;
}
//-------------------------------------------------------------------------
void CAntiaim::Run(QAngle org_view) {
	if (Clientvariables->Antiaim.AntiaimEnable) {
		static int iChokedPackets = -1;
		if ((g_Aimbot->fired_in_that_tick && iChokedPackets < 4 && GameUtils
			::IsAbleToShoot()) && !G::ForceRealAA) {
			G::SendPacket = false;
			iChokedPackets++;
		}
		else {
			iChokedPackets = 0;
			CGrenade *pCSGrenade = (CGrenade*)G::LocalPlayer->GetWeapon();
			if (G::UserCmd->buttons& IN_USE || !GetAlive(G::LocalPlayer) &&
				!Clientvariables->Misc.AntiUT || G::LocalPlayer->GetMoveType() ==
				MOVETYPE_LADDER && G::LocalPlayer->GetVelocity().Length() > 0 || G
				::LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || pCSGrenade &&
				pCSGrenade->GetThrowTime() > 0.f) {
				return;
			}
			if (G::LocalPlayer->isAlive()) {
				choke = !choke;
				if (!Clientvariables->Misc.FakelagEnable || (*G::LocalPlayer
					->GetFlags()& FL_ONGROUND && !Clientvariables
					->Misc.FakelagOnground || *G::LocalPlayer->GetFlags()&
					FL_ONGROUND && G::LocalPlayer->GetVelocity().Length() < 0.1f)) {
					G::SendPacket = choke;
				}
				G::UserCmd->viewangles = org_view;
				if (Clientvariables->Antiaim.AtPlayer > 0) {
					AimAtPlayers();
				}
				Fakewalk(G::UserCmd);
				if (G::bShouldChoke) {
					G::SendPacket = G::bShouldChoke = false;
				}
				if (!G::SendPacket) {
					G::nChokedTicks++;
				}
				else {
					G::nChokedTicks = 0;
				}
				static int Ticks;
				if (Clientvariables->Antiaim.Freestanding == 1) {
					Pitch_AA();
					BreakLowerbodyFreestand();
					/*if (G::SendPacket)
					AntiAimYaw_Fake();
					else {
					AntiAimYaw_Real();
					}*/
					if (G::SendPacket) {
						AntiAimYaw_Fake();
					}
				}
				else if (Clientvariables->Antiaim.Freestanding == 2) {
					Pitch_AA();
					float server_time = G::LocalPlayer->GetTickBase() * g_pGlobals->interval_per_tick;
					QAngle angle_for_yawa;
					
					g_pEngine->GetViewAngles(angle_for_yawa);
					QAngle angle_for_yaw;
					if (G::Target > 0)
						Math::CalcAngle(G::LocalPlayer->GetOrigin(), G::Target->GetOrigin(), angle_for_yaw);
					else
					//	angle_for_yaw.y =	BestHeadDirection(angle_for_yawa.y);
						g_pEngine->GetViewAngles(angle_for_yaw);
				
					if (!G::SendPacket)
					{
						//G::SendPacket = false;

						switch ((int)server_time % 2)
						{
						case 0: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y - 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 180.f) : ClampYaw(angle_for_yaw.y - 90.f); break;
						case 1: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 90.f) : ClampYaw(angle_for_yaw.y + 90.f); break;
						}

					}
					else
					{
					//	G::SendPacket = true;
						G::UserCmd->viewangles.y = angle_for_yaw.y + (rand() % 100 > 33 ? (rand() % 50 > 13 ? (rand() % 20 + 40) : -(rand() % 20 + 40)) : (rand() % 100 > 71 ? (rand() % 20 + 150) : -(rand() % 20 + 150)));
					}
					
				}
				else if (Clientvariables->Antiaim.Freestanding == 3 ) {
					Pitch_AA();
					
					float server_time = G::LocalPlayer->GetTickBase() * g_pGlobals->interval_per_tick;
					QAngle angle_for_yawa;

					g_pEngine->GetViewAngles(angle_for_yawa);
					QAngle angle_for_yaw;
					if (G::Target > 0)
						Math::CalcAngle(G::LocalPlayer->GetOrigin(), G::Target->GetOrigin(), angle_for_yaw);
					else
						//	angle_for_yaw.y =	BestHeadDirection(angle_for_yawa.y);
						g_pEngine->GetViewAngles(angle_for_yaw);
					if (!G::SendPacket)
					{
						//G::SendPacket = false;

						switch ((int)server_time % 4)
						{
						case 0: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y - 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 179.f) : ClampYaw(angle_for_yaw.y - 68.f); break;
						case 1: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 68.f) : ClampYaw(angle_for_yaw.y + 67.f); break;
						case 2: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 88.f) : ClampYaw(angle_for_yaw.y + 92.f); break;
						case 3: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 84.f) : ClampYaw(angle_for_yaw.y + 78.f); break;
						}

					}
					else
					{
						//	G::SendPacket = true;
						G::UserCmd->viewangles.y = angle_for_yaw.y + (rand() % 100 > 33 ? (rand() % 50 > 13 ? (rand() % 20 + 40) : -(rand() % 20 + 40)) : (rand() % 100 > 71 ? (rand() % 20 + 150) : -(rand() % 20 + 150)));
					}
				}
				else if (Clientvariables->Antiaim.Freestanding == 4) {
					Pitch_AA();

					float server_time = G::LocalPlayer->GetTickBase() * g_pGlobals->interval_per_tick;
					QAngle angle_for_yawa;

					g_pEngine->GetViewAngles(angle_for_yawa);
					QAngle angle_for_yaw;
					if (G::Target > 0)
						Math::CalcAngle(G::LocalPlayer->GetOrigin(), G::Target->GetOrigin(), angle_for_yaw);
					else
						//	angle_for_yaw.y =	BestHeadDirection(angle_for_yawa.y);
						g_pEngine->GetViewAngles(angle_for_yaw);
					BreakLowerbodyFreestand();
					if (G::SendPacket)
					{
						//G::SendPacket = false;

						switch ((int)server_time % 4)
						{
						case 0: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y - 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 179.f) : ClampYaw(angle_for_yaw.y - 68.f); break;
						case 1: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 68.f) : ClampYaw(angle_for_yaw.y + 67.f); break;
						case 2: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 88.f) : ClampYaw(angle_for_yaw.y + 92.f); break;
						case 3: G::UserCmd->viewangles.y = fabs(G::LocalPlayer->LowerBodyYaw() - ClampYaw(angle_for_yaw.y + 90.f)) > 35 ? ClampYaw(G::LocalPlayer->LowerBodyYaw() - 84.f) : ClampYaw(angle_for_yaw.y + 78.f); break;
						}
						G::UserCmd->viewangles.y += 99;

					}

				}
				else if (Clientvariables->Antiaim.Freestanding == 5) {
					Pitch_AA();
					float server_time = G::LocalPlayer->GetTickBase() * g_pGlobals->interval_per_tick;
					BreakLowerbodyFreestand();
					if (G::SendPacket)
					{
							switch ((int)server_time % 2)
						{
						case 0:  G::UserCmd->viewangles.y = +90.5; break;
						case 1:  G::UserCmd->viewangles.y = -90.7; break;
						}
					}
					else
					{
						switch ((int)server_time % 2)
						{
						case 0:  G::UserCmd->viewangles.y = +179.5; break;
						case 1:  G::UserCmd->viewangles.y = -180.7; break;
						}
					}
					/*if (G::SendPacket)
					{
						G::UserCmd->viewangles.y -= Ticks;
						Ticks += 6;
						if (Ticks == 180.7 && Ticks == 179.5) {
							Ticks = 0;
						}
						else if (Ticks > 360) {
							Ticks = 0;
						}
					}
					else
					{
						switch ((int)server_time % 2)
						{
						case 0:  G::UserCmd->viewangles.y = 179.5; break;
						case 1:  G::UserCmd->viewangles.y = 180.7; break;
						}
					}*/
				}
				else if (Clientvariables->Antiaim.Freestanding == 6) {
					Pitch_AA();
					float server_time = G::LocalPlayer->GetTickBase() * g_pGlobals->interval_per_tick;
					BreakLowerbodyFreestand();
					if (G::SendPacket)
					{
						switch ((int)server_time % 2)
						{
						case 0:  G::UserCmd->viewangles.y = G::LocalPlayer->LowerBodyYaw() +90.5; break;
						case 1:  G::UserCmd->viewangles.y = G::LocalPlayer->LowerBodyYaw() -90.7; break;
						}
					}
					else
					{
						switch ((int)server_time % 2)
						{
						case 0:  G::UserCmd->viewangles.y = G::LocalPlayer->LowerBodyYaw() +179.5; break;
						case 1:  G::UserCmd->viewangles.y = G::LocalPlayer->LowerBodyYaw() -180.7; break;
						}
					}
				
				}
				else if (Clientvariables->Antiaim.Freestanding == 7) {
					Pitch_AA();
					float server_time = G::LocalPlayer->GetTickBase() * g_pGlobals->interval_per_tick;
					QAngle Angles;
					g_pEngine->GetViewAngles(Angles);
					float BestHeadPosition = GetBestHeadAngle(Angles.y) + 180;

					if (G::SendPacket) {
						switch ((int)server_time % 3)
						{
						case 0:  G::UserCmd->viewangles.y = BestHeadPosition + 76.3; break;
						case 1:  G::UserCmd->viewangles.y = BestHeadPosition + 82.6; break;
						case 2:  G::UserCmd->viewangles.y = BestHeadPosition + 57.9; break;
						}
					}
					else
					{
						if (ShouldPredict()) {
							G::UserCmd->viewangles.y += G::LocalPlayer->LowerBodyYaw() - 29.8;
						}
						else
						{
							switch ((int)server_time % 2)
							{
							case 0:  G::UserCmd->viewangles.y = BestHeadPosition + 154.5; break;
							case 1:  G::UserCmd->viewangles.y = BestHeadPosition + 120.9; break;
							}
						}
					}

				}
				else {
					Pitch_AA();
					if (G::SendPacket) {
						AntiAimYaw_Fake();
					}
					else {
						AntiAimYaw_Real();
					}
				}
			}
		}
	}
}

//-------------------------------------------------------------------------
void Accelerate(CBaseEntity *player, Vector& wishdir, float wishspeed, float
	accel, Vector& outVel) {
	float currentspeed = outVel.Dot(wishdir);
	float addspeed = wishspeed - currentspeed;
	if (addspeed <= 0) {
		return;
	}
	float accelspeed = accel * g_pGlobals->frametime *wishspeed * player
		->m_surfaceFriction();
	if (accelspeed > addspeed) {
		accelspeed = addspeed;
	}
	for (int i = 0; i < 3; i++) {
		outVel[i] += accelspeed * wishdir[i];
	}
}void WalkMove(CBaseEntity *player, Vector& outVel) {
	Vector forward, right, up, wishvel, wishdir, dest;
	float_t fmove, smove, wishspeed;
	Math::AngleVectors(player->GetEyeAngles(), forward, right, up);
	g_pMoveHelper->SetHost(player);
	fmove = g_pMoveHelper->m_flForwardMove;
	smove = g_pMoveHelper->m_flSideMove;
	g_pMoveHelper->SetHost((CBaseEntity*)nullptr);
	if (forward[2] != 0) {
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}
	if (right[2] != 0) {
		right[2] = 0;
		Math::NormalizeVector(right);
	}
	for (int i = 0; i < 2; i++) {
		wishvel[i] = forward[i] * fmove + right[i] * smove;
	}
	wishvel[2] = 0;
	wishdir = wishvel;
	wishspeed = wishdir.Normalize();
	g_pMoveHelper->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > g_pMoveHelper->m_flMaxSpeed)) {
		VectorMultiply(wishvel, player->m_flMaxspeed() / wishspeed, wishvel);
		wishspeed = player->m_flMaxspeed();
	}
	g_pMoveHelper->SetHost(nullptr);
	outVel[2] = 0;
	Accelerate(player, wishdir, wishspeed, g_pCvar->FindVar(XorStr(
		"sv_accelerate"))->GetFloat(), outVel);
	outVel[2] = 0;
	VectorAdd(outVel, player->GetBaseVelocity(), outVel);
	float spd = outVel.Length();
	if (spd < 1.0f) {
		outVel.Init();
		VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
		return;
	}
	g_pMoveHelper->SetHost(player);
	g_pMoveHelper->m_outWishVel += wishdir * wishspeed;
	g_pMoveHelper->SetHost(nullptr);
	if (!(*player->GetFlags()& FL_ONGROUND)) {
		VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
		return;
	}
	VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
}

//-------------------------------------------------------------------------
void CAntiaim::Fakewalk(CUserCmd *userCMD) {
	if (!Clientvariables->Misc.FakewalkEnable || !GetAsyncKeyState
	(Clientvariables->Misc.FakewalkKey)) {
		return;
	}
	{
		static int iChoked = -1;
		iChoked++;
		if (iChoked < 3) {
			static int choked = 0;
			choked = choked > 7 ? 0 : choked + 1;
			G::UserCmd->forwardmove = choked < 2 || choked > 5 ? 0 : G::UserCmd
				->forwardmove;
			G::UserCmd->sidemove = choked < 2 || choked > 5 ? 0 : G::UserCmd
				->sidemove;
			G::SendPacket = false;
		}
		else {
			G::SendPacket = true;
			iChoked = -1;
			g_pGlobals->frametime *= (G::LocalPlayer->GetVelocity().Length2D()) /
				10;
			G::UserCmd->buttons |= G::LocalPlayer->GetMoveType() == IN_FORWARD;
		}
	}
}

//-------------------------------------------------------------------------
void CAntiaim::Friction(Vector& outVel) {
	float speed, newspeed, control;
	float friction;
	float drop;
	speed = outVel.Length();
	if (speed <= 0.1f) {
		return;
	}
	drop = 0;
	if (*G::LocalPlayer->GetFlags()& FL_ONGROUND) {
		friction = g_pCvar->FindVar(XorStr("sv_friction"))->GetFloat() *G
			::LocalPlayer->m_surfaceFriction();
		control = (speed < g_pCvar->FindVar(XorStr("sv_stopspeed"))->GetFloat())
			? g_pCvar->FindVar(XorStr("sv_stopspeed"))->GetFloat() : speed;
		drop += control * friction * g_pGlobals->frametime;
	}
	newspeed = speed - drop;
	if (newspeed < 0) {
		newspeed = 0;
	}
	if (newspeed != speed) {
		newspeed /= speed;
		VectorMultiply(outVel, newspeed, outVel);
	}
}
















