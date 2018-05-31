#include "sdk.h"
#include "Legitbot.h"
#include "global.h"
#include "Menu.h"
#include "Math.h"
#include "GameUtils.h"
#include "Autowall.h"
#include "BacktrackingHelper.h"

#define TICK_INTERVAL (g_pGlobals->interval_per_tick)
#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / TICK_INTERVAL))
#define TICKS_TO_TIME(t) (TICK_INTERVAL * (t))

template <class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

Vector GetHitboxPOS_Ent(CBaseEntity* Player, int HitboxID)
{
	auto VectorTransform_Wrapper = [](const Vector& in1, const matrix3x4 &in2, Vector &out)
	{
		auto VectorTransform = [](const float *in1, const matrix3x4& in2, float *out)
		{
			auto DotProducts = [](const float *v1, const float *v2)
			{
				return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
			};
			out[0] = DotProducts(in1, in2[0]) + in2[0][3];
			out[1] = DotProducts(in1, in2[1]) + in2[1][3];
			out[2] = DotProducts(in1, in2[2]) + in2[2][3];
		};
		VectorTransform(&in1.x, in2, &out.x);
	};
	matrix3x4 matrix[128];
	if (!Player->SetupBones(matrix, 128, 0x00000100, GetTickCount64())) return Vector(0, 0, 0);
	const model_t* mod = Player->GetModel();
	if (!mod) return Vector(0, 0, 0);
	studiohdr_t* hdr = g_pModelInfo->GetStudioModel(mod);
	if (!hdr) return Vector(0, 0, 0);
	mstudiohitboxset_t* set = hdr->pHitboxSet(0);
	if (!set) return Vector(0, 0, 0);
	mstudiobbox_t* hitbox = set->pHitbox(HitboxID);
	if (!hitbox) return Vector(0, 0, 0);
	Vector vMin, vMax, vCenter, sCenter;
	VectorTransform_Wrapper(hitbox->bbmin, matrix[hitbox->bone], vMin);
	VectorTransform_Wrapper(hitbox->bbmax, matrix[hitbox->bone], vMax);
	vCenter = (vMin + vMax) *0.5f;
	return vCenter;
}

CLegitbot* g_Aimbotv2 = new CLegitbot;

void NormalizeAngles(QAngle& angles)
{
	for (auto i = 0; i < 3; i++) {
		while (angles[i] < -180.0f) angles[i] += 360.0f;
		while (angles[i] >  180.0f) angles[i] -= 360.0f;
	}
}
void SmoothAngle(QAngle src, QAngle &dst, float factor)
{
	QAngle delta = dst - src;

	NormalizeAngles(delta);

	dst = src + delta / factor;
}
void CLegitbot::DropTarget()
{
	target_index = -1;
	best_distance = 99999.f;
	aimbotted_in_current_tick = false;
	fired_in_that_tick = false;
	current_aim_position = Vector();
	pTarget = nullptr;
}
void Smootha(Vector& currentAngle, Vector& aimAngle, Vector& smoothedAngle, float fSmoothPercentage)
{
	Math::ClampAngles(aimAngle);
	smoothedAngle = aimAngle - currentAngle;
	smoothedAngle = currentAngle + smoothedAngle / 100.f * fSmoothPercentage;
	Math::ClampAngles(smoothedAngle);
}
void Smooth(Vector& src, Vector& back, Vector& flLocalAngles, int iSmooth)
{
	float smoothdiff[2];
	src.x -= flLocalAngles.x;
	src.y -= flLocalAngles.y;

	if (src.x >= 180)
		src.x -= 360;
	if (src.y > 180)
		src.y -= 360;
	if (src.x < -180)
		src.x += 360;
	if (src.y < -180)
		src.y += 360;

	smoothdiff[0] = src.x / iSmooth;
	smoothdiff[1] = src.y / iSmooth;
	back.x = flLocalAngles.x + smoothdiff[0];
	back.y = flLocalAngles.y + smoothdiff[1];
	back.z = flLocalAngles.z;

	if (back.x > 180)
		back.x -= 360;
	if (back.y > 180)
		back.y -= 360;
	if (back.x < -180)
		back.x += 360;
	if (back.y < -180)
		back.y += 360;
}
enum eHITBOX
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_BELLY,
	HITBOX_THORAX,
	HITBOX_LOWER_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};
void CLegitbot::Run(CBaseEntity* local, CBaseCombatWeapon* weapon, CUserCmd* cmd)
{
	if (!Clientvariables
		->Legitbot.EnableAimbot)
		return;

	QAngle view;
	g_pEngine->GetViewAngles(view);
	CBaseCombatWeapon* pWeapon = G::LocalPlayer->GetWeapon();



	if (!pWeapon || pWeapon->Clip1() == 0 || pWeapon->IsMiscWeapon() || !GameUtils::IsAbleToShoot())
		return;

	for (int i = 1; i < 64; ++i) {
		if (i == g_pEngine->GetLocalPlayer())
			continue;

		CBaseEntity* target = g_pEntitylist->GetClientEntity(i);

		if (!target->IsValidTarget())
			continue;

		G::Target = target;
		int fova;
		int hitboxtypea;
		int LegitBotkeya;
		bool RCSon;
		float RCSy;
		float RCSx;
		float Smooth;
		if(pWeapon->GetWeaponType() != WEAPONCLASS::WEPCLASS_PISTOL){
			fova = Clientvariables->LegitbotPistols.fov;
			hitboxtypea = Clientvariables->LegitbotPistols.hitboxtype;
			LegitBotkeya = Clientvariables->LegitbotPistols.LegitBotkey;
			RCSon = Clientvariables->LegitbotPistols.RCS;
			RCSy = Clientvariables->LegitbotPistols.RCSy;
			RCSx = Clientvariables->LegitbotPistols.RCSx;
			Smooth = Clientvariables->LegitbotPistols.Smooth;
		}
		else if (pWeapon->GetWeaponType() != WEAPONCLASS::WEPCLASS_RIFLE) {
			fova = Clientvariables->LegitbotRifles.fov;
			hitboxtypea = Clientvariables->LegitbotRifles.hitboxtype;
			LegitBotkeya = Clientvariables->LegitbotRifles.LegitBotkey;
			RCSon = Clientvariables->LegitbotRifles.RCS;
			RCSy = Clientvariables->LegitbotRifles.RCSy;
			RCSx = Clientvariables->LegitbotRifles.RCSx;
			Smooth = Clientvariables->LegitbotRifles.Smooth;

		}
		else if (pWeapon->GetWeaponType() != WEAPONCLASS::WEPCLASS_SNIPER) {
			fova = Clientvariables->LegitbotSniper.fov;
			hitboxtypea = Clientvariables->LegitbotSniper.hitboxtype;
			LegitBotkeya = Clientvariables->LegitbotSniper.LegitBotkey;
			RCSon = Clientvariables->LegitbotSniper.RCS;
			RCSy = Clientvariables->LegitbotSniper.RCSy;
			RCSx = Clientvariables->LegitbotSniper.RCSx;
			Smooth = Clientvariables->LegitbotSniper.Smooth;
		}
		float fov = GameUtils::GetFoV(view, G::LocalPlayer->GetEyePosition(), target->GetEyePosition(), false);

		if (fov > fova / 2)
			continue;
		QAngle viewangles = cmd->viewangles;
		float simtime = 0;
		Vector minus_origin = Vector(0, 0, 0);
		Vector aim_position;
		if (hitboxtypea == 5) {
			for (auto i = 0; i < HITBOX_MAX; i++) {
				aim_position = GetHitboxPOS_Ent(target, i);
			}
			
		}
		else
		{
			aim_position = GetHitboxPOS_Ent(target, hitboxtypea);
		}
		

		if (aim_position == Vector(0, 0, 0))
			continue;

		aimbotted_in_current_tick = true;
	
		QAngle aim = GameUtils::CalculateAngle(G::LocalPlayer->GetEyePosition(), aim_position);

		SmoothAngle(viewangles, aim, Clientvariables->Legitbot.Smooth + 1 / 4);
		aim.y = Math::NormalizeYaw(aim.y);
		if (GetKeyState(LegitBotkeya) & 0x8000) {

			RecoilControlSystem(aim, RCSon, RCSy, RCSx);
			G::UserCmd->viewangles = aim;
			g_pEngine->SetViewAngles(G::UserCmd->viewangles);
		}


		//if (Clientvariables->Ragebot.EnginePred)
		//G::UserCmd->tick_count = TIME_TO_TICKS(current_aim_simulationtime) + TIME_TO_TICKS(g_BacktrackHelper->GetLerpTime());
	}
	
		

}

QAngle oldPunch;
bool shouldAim;
QAngle AimStepLastAngle;
QAngle RCSLastPunch;
void CLegitbot::RecoilControlSystem(QAngle& angle,bool on,float y,float x)
{
	if (!on)
		return;
	int firedShots = G::LocalPlayer->GetShotsFired();
	
	if (!(G::UserCmd->buttons & IN_ATTACK))
		return;

//	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
	QAngle CurrentPunch = G::LocalPlayer->GetPunchAngle();
	bool isSilent = false;
//	bool hasTarget = Settings::Aimbot::AutoAim::enabled && player && shouldAim;

	//if (!Settings::Aimbot::RCS::always_on && !hasTarget)
		//return;

	if (G::LocalPlayer->GetShotsFired() > 1)
	{
		QAngle NewPunch = { CurrentPunch.x - RCSLastPunch.x, CurrentPunch.y - RCSLastPunch.y, 0 };

		angle.x -= NewPunch.x * x;
		angle.y -= NewPunch.y * y;
	}

	RCSLastPunch = CurrentPunch;
}

int speed = 0;
void CLegitbot::fakelagonkey() {
	if (!Clientvariables
		->Legitbot.FakeLag)
		return;
	
	if (GetKeyState(Clientvariables->Legitbot.FakeLagOnKey) & 0x8000) {
		
		if (speed == Clientvariables->Legitbot.FakeLaga) {
			G::SendPacket = true;
			speed = 0;
		}
		else
		{
			G::SendPacket = false;
			G::SendPacket = false;
			speed += 1;
			G::SendPacket = false;
			G::SendPacket = false;
		}
	}
}
void CLegitbot::CompensateInaccuracies()
{
	if (G::UserCmd->buttons & IN_ATTACK) {
		if (Clientvariables->Ragebot.NoRecoil) {
			ConVar* recoilscale = g_pCvar->FindVar(XorStr("weapon_recoil_scale"));

			if (recoilscale) {
				QAngle qPunchAngles = G::LocalPlayer->GetPunchAngle();
				QAngle qAimAngles = G::UserCmd->viewangles;
				qAimAngles -= qPunchAngles * recoilscale->GetFloat();
				G::UserCmd->viewangles = qAimAngles;
			}
		}
	}
}















