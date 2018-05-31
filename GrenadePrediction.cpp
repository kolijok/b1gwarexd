#include "GrenadePrediction.h"
#include "SDK.h"
#include "sdk.h"
#include <assert.h>
#include "global.h"
#include "GameUtils.h"
#include "Draw.h"

void grenade_prediction::Tick(int buttons)
{
	if (!Clientvariables->Visuals.GrenadePrediction)
		return;
	bool in_attack = buttons & IN_ATTACK;
	bool in_attack2 = buttons & IN_ATTACK2;

	act = (in_attack && in_attack2) ? ACT_LOB : (in_attack2) ? ACT_DROP : (in_attack) ? ACT_THROW : ACT_NONE;
}

void grenade_prediction::View(CViewSetup* setup)
{
	if (!Clientvariables->Visuals.GrenadePrediction)
		return;

	if (G::LocalPlayer && G::LocalPlayer->isAlive()) {
		CBaseCombatWeapon* weapon = G::LocalPlayer->GetWeapon();

		if (weapon && weapon->IsGrenade() && act != ACT_NONE) {
			type = weapon->WeaponID();
			Simulate(setup);
		}
		else {
			type = 0;
		}
	}
}

void grenade_prediction::Paint()
{
	if (!Clientvariables->Visuals.GrenadePrediction)
		return;

	if ((type) && path.size() > 1) {
		float Red, Green, Blue;
		Red = Clientvariables->Colors.GrenadePrediction[0] * 255;
		Green = Clientvariables->Colors.GrenadePrediction[1] * 255;
		Blue = Clientvariables->Colors.GrenadePrediction[2] * 255;
		Vector nadeStart, nadeEnd;

		Color lineColor(Red, Green, Blue);
		Vector prev = path[0];

		for (auto it = path.begin(), end = path.end(); it != end; ++it) {
			if (GameUtils::WorldToScreen(prev, nadeStart) && GameUtils::WorldToScreen(*it, nadeEnd)) {
				g_pSurface->SetDrawColor(lineColor);
				g_pSurface->DrawLine((int)nadeStart.x, (int)nadeStart.y, (int)nadeEnd.x, (int)nadeEnd.y);
			}
			prev = *it;
		}


		if (GameUtils::WorldToScreen(prev, nadeEnd)) {
			g_Draw.Box(nadeEnd.x, nadeEnd.y, 10, 10, 255, 255, 255, 255);
		}
	}
}

static const constexpr auto PIRAD = 0.01745329251f;

void angle_vectors2(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float sr, sp, sy, cr, cp, cy;

	sp = static_cast<float>(sin(double(angles.x) * PIRAD));
	cp = static_cast<float>(cos(double(angles.x) * PIRAD));
	sy = static_cast<float>(sin(double(angles.y) * PIRAD));
	cy = static_cast<float>(cos(double(angles.y) * PIRAD));
	sr = static_cast<float>(sin(double(angles.z) * PIRAD));
	cr = static_cast<float>(cos(double(angles.z) * PIRAD));

	if (forward) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right) {
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up) {
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void grenade_prediction::Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles)
{
	if (!Clientvariables->Visuals.GrenadePrediction)
		return;

	Vector angThrow = viewangles;
	float pitch = angThrow.x;

	if (pitch <= 90.0f) {
		if (pitch < -90.0f) {
			pitch += 360.0f;
		}
	}
	else {
		pitch -= 360.0f;
	}
	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.x = a;

	float flVel = 750.0f * 0.9f;

	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];

	b = b * 0.7f;
	b = b + 0.3f;
	flVel *= b;

	Vector vForward, vRight, vUp;
	angle_vectors2(angThrow, &vForward, &vRight, &vUp); 

	vecSrc = G::LocalPlayer->GetEyePosition();
	float off = (power[act] * 12.0f) - 12.0f;
	vecSrc.z += off;

	trace_t tr;
	Vector vecDest = vecSrc;
	vecDest += vForward * 22.0f;

	TraceHull(vecSrc, vecDest, tr);

	Vector vecBack = vForward;
	vecBack *= 6.0f;
	vecSrc = tr.endpos;
	vecSrc -= vecBack;

	vecThrow = G::LocalPlayer->GetVelocity();
	vecThrow *= 1.25f;
	vecThrow += vForward * flVel;
}

void grenade_prediction::Simulate(CViewSetup* setup)
{
	if (!Clientvariables->Visuals.GrenadePrediction)
		return;
	Vector vecSrc, vecThrow;
	Vector angles;
	g_pEngine->GetViewAngles(angles);
	Setup(vecSrc, vecThrow, angles);

	float interval = g_pGlobals->interval_per_tick;

	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;

	path.clear();
	for (unsigned int i = 0; i < path.max_size() - 1; ++i) {
		if (!logtimer)
			path.push_back(vecSrc);

		int s = Step(vecSrc, vecThrow, i, interval);
		if ((s & 1))
			break;

		if ((s & 2) || logtimer >= logstep)
			logtimer = 0;
		else
			++logtimer;
	}
	path.push_back(vecSrc);
}

int grenade_prediction::Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
{

	Vector move;
	AddGravityMove(move, vecThrow, interval, false);

	trace_t tr;
	PushEntity(vecSrc, move, tr);

	int result = 0;
	if (CheckDetonate(vecThrow, tr, tick, interval)) {
		result |= 1;
	}

	if (tr.fraction != 1.0f) {
		result |= 2; 
		ResolveFlyCollisionCustom(tr, vecThrow, interval);
	}

	vecSrc = tr.endpos;

	return result;
}

bool grenade_prediction::CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
{
	switch (type) {
	case WEAPON_SMOKE:
	case WEAPON_DECOY:
		if (vecThrow.Length2D() < 0.1f) {
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick % det_tick_mod);
		}
		return false;

	case WEAPON_MOLOTOV:
	case WEAPON_INC:
		if (tr.fraction != 1.0f && tr.plane.normal.z > 0.7f)
			return true;

	case WEAPON_FLASH:
	case WEAPON_HE:
		return static_cast<float>(tick) * interval > 1.5f && !(tick % static_cast<int>(0.2f / interval));

	default:
		assert(false);
		return false;
	}
}

void grenade_prediction::TraceHull(Vector& src, Vector& end, trace_t& tr)
{
	if (!Clientvariables->Visuals.GrenadePrediction)
		return;
	Ray_t ray;
	ray.Init(src, end, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f));

	CTraceFilterWorldAndPropsOnly filter;

	g_pEngineTrace->TraceRay_NEW(ray, 0x200400B, &filter, &tr);
}

void grenade_prediction::AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{
	if (!Clientvariables->Visuals.GrenadePrediction)
		return;
	Vector basevel(0.0f, 0.0f, 0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground) {
		move.z = (vel.z + basevel.z) * frametime;
	}
	else {
		float gravity = 800.0f * 0.4f;

		float newZ = vel.z - (gravity * frametime);
		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

		vel.z = newZ;
	}
}

void grenade_prediction::PushEntity(Vector& src, const Vector& move, trace_t& tr)
{
	if (!Clientvariables->Visuals.GrenadePrediction)
		return;
	Vector vecAbsEnd = src;
	vecAbsEnd += move;

	TraceHull(src, vecAbsEnd, tr);
}

void grenade_prediction::ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval)
{
	if (!Clientvariables->Visuals.GrenadePrediction)
		return;

	float flSurfaceElasticity = 1.0; 
	float flGrenadeElasticity = 0.45f; 
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity > 0.9f)
		flTotalElasticity = 0.9f;
	if (flTotalElasticity < 0.0f)
		flTotalElasticity = 0.0f;

	Vector vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
	if (flSpeedSqr < flMinSpeedSqr) {
		vecAbsVelocity.x = 0.0f;
		vecAbsVelocity.y = 0.0f;
		vecAbsVelocity.z = 0.0f;
	}

	if (tr.plane.normal.z > 0.7f) {
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= ((1.0f - tr.fraction) * interval);
		PushEntity(tr.endpos, vecAbsVelocity, tr);
	}
	else {
		vecVelocity = vecAbsVelocity;
	}
}

int grenade_prediction::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float backoff;
	float change;
	float angle;
	int i, blocked;

	blocked = 0;

	angle = normal[2];

	if (angle > 0) {
		blocked |= 1; 
	}
	if (!angle) {
		blocked |= 2; 
	}

	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i < 3; i++) {
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON) {
			out[i] = 0;
		}
	}

	return blocked;
}







































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































