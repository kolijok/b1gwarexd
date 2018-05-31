#pragma once

class CLegitbot{
public:
	bool HitChance(CBaseEntity* pCSTarget, CBaseCombatWeapon* pCSWeapon, QAngle qAngle, int chance);
	void DropTarget();
	void RecoilControlSystem(QAngle& angle, bool on, float y,float x);
	void Run(CBaseEntity* local, CBaseCombatWeapon* weapon, CUserCmd* cmd);
	void CompensateInaccuracies();
	void fakelagonkey();
	int target_index = -1;
	float best_distance = 8192.f;
	bool aimbotted_in_current_tick;
	bool fired_in_that_tick;
	float current_aim_simulationtime;
	int current_minusticks;
	Vector current_aim_position;
	Vector current_aim_player_origin;
	CBaseEntity* pTarget;
};

extern CLegitbot* g_Aimbotv2;