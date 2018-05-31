#pragma once
struct FireBulletData {
	FireBulletData(const Vector& eye_pos) : src(eye_pos) {}
	Vector src;
	trace_t enter_trace;
	Vector direction;
	CTraceFilter filter;
	float trace_length;
	float trace_length_remaining;
	float current_damage;
	int penetrate_count;
};
class CAutowall {
public:
	//Old Awall
	bool TraceToExit(Vector& end, trace_t *enter_trace, Vector start, Vector
		dir, trace_t *exit_trace);
	bool HandleBulletPenetration(CSWeaponInfo *pWeaponData, FireBulletData&
		BulletData);
	bool SimulateFireBullet(CBaseEntity *entity, CBaseEntity *local,
		CBaseCombatWeapon *weapon, FireBulletData& data);
	bool CanHit(CBaseEntity *entity, const Vector& point, float *damage_given)
		;
	bool CanWallBang(float& dmg);
};
extern CAutowall *g_Autowall;
extern void ScaleDamage(int hitgroup, CBaseEntity *enemy, float
	weapon_armor_ratio, float& current_damage);