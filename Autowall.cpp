#include "sdk.h"
#include "Autowall.h"
#include "GameUtils.h"
#include "Math.h"
#include "Menu.h"
#include "global.h"
CAutowall* g_Autowall = new CAutowall;
#define HITGROUP_GENERIC    0
#define HITGROUP_HEAD        1
#define HITGROUP_CHEST        2
#define HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4    
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
#define HITGROUP_GEAR        10
/*
$qch: oh ye you will
$qch: like handlebulletpen doesn't check for the hitbox or something
$qch: it only check for materials
$qch: between you and point
D3dC0m: But the damage will always be 0
$qch: no
$qch: it's based of you current weapon damage
$qch: than it check for the materials and lower damage based on this
$qch: then scale based of hitbox group
$qch: All you will have to fix
D3dC0m: OH
D3dC0m: Use a traceray
$qch: is that you will need to manually pass hitgroup
D3dC0m: That ignores eveerything
D3dC0m: than players
D3dC0m: except*
D3dC0m: to get hitgroup
$qch: ye
$qch: I made a func to dump hitgroup based of hitbox
$qch: so now I have a func hitbox to hitgroup
D3dC0m: ye

*/
float GetHitgroupDamageMult(int iHitGroup) {
	/* Head	400% (4X)
	Chest & Arm	100% (1X)
	Stomach	125% (1.25X)
	Leg	75% (0.75X) */
	switch (iHitGroup) {
	case HITGROUP_GENERIC:
		return 1.0f;
	case HITGROUP_HEAD:
		return 4.0f;
	case HITGROUP_CHEST:
		return 1.0f;
	case HITGROUP_STOMACH:
		return 1.25f;
	case HITGROUP_LEFTARM:
		return 1.0f;
	case HITGROUP_RIGHTARM:
		return 1.0f;
	case HITGROUP_LEFTLEG:
		return 0.75f;
	case HITGROUP_RIGHTLEG:
		return 0.75f;
	case HITGROUP_GEAR:
		return 1.0f;
	default:
		break;
	}
	return 1.0f;
}

//-------------------------------------------------------------------------
void ScaleDamage(int hitgroup, CBaseEntity *enemy, float weapon_armor_ratio,
	float& current_damage) {
	current_damage *= GetHitgroupDamageMult(hitgroup);
	if (enemy->GetArmor() > 0) {
		if (hitgroup == HITGROUP_HEAD) {
			if (enemy->HasHelmet()) {
				current_damage *= weapon_armor_ratio;
			}
		}
		else {
			current_damage *= weapon_armor_ratio;
		}
	}
}

//-------------------------------------------------------------------------
bool DidHitNonWorldEntity(CBaseEntity *entity) {
	return entity != nullptr && entity->GetIndex() != 0;
}

//-------------------------------------------------------------------------
bool IsBreakableEntity(CBaseEntity *entity) {
	ClientClass *pClass = (ClientClass*)entity->GetClientClass();
	if (!pClass) {
		return false;
	}
	return pClass->m_ClassID == (int)CSClasses::CBreakableProp || pClass
		->m_ClassID == (int)CSClasses
		::CBreakableSurface;
}

//-------------------------------------------------------------------------
bool CAutowall::TraceToExit(Vector& end, trace_t *enter_trace, Vector start,
	Vector dir, trace_t *exit_trace) {
	float distance = 0.0f;
	while (distance <= 90.0f) {
		distance += 4.0f;
		end = start + dir * distance;
		auto point_contents = g_pEngineTrace->GetPointContents(end,
			MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);
		if (point_contents& MASK_SHOT_HULL && (!(point_contents& CONTENTS_HITBOX))
			) {
			continue;
		}
		auto new_end = end - (dir *4.0f);
		GameUtils::UTIL_TraceLine(end, new_end, 0x4600400B, G::LocalPlayer, 0,
			exit_trace);
		if (exit_trace->startSolid && exit_trace->surface.flags& SURF_HITBOX) {
			GameUtils::UTIL_TraceLine(end, new_end, 0x4600400B, exit_trace->m_pEnt,
				0, exit_trace);
			if ((exit_trace->fraction < 1.0f || exit_trace->allSolid) &&
				!exit_trace->startSolid) {
				end = exit_trace->endpos;
				return true;
			}
			continue;
		}
		if (!(exit_trace->fraction < 1.0 || exit_trace->allSolid || exit_trace
			->startSolid) || exit_trace->startSolid) {
			if (exit_trace->m_pEnt) {
				if (DidHitNonWorldEntity(exit_trace->m_pEnt) && IsBreakableEntity
				(enter_trace->m_pEnt)) {
					return true;
				}
			}
			continue;
		}
		if (((exit_trace->surface.flags >> 7) & 1) && !((enter_trace
			->surface.flags >> 7) & 1)) {
			continue;
		}
		if (exit_trace->plane.normal.Dot(dir) <= 1.0f) {
			auto fraction = exit_trace->fraction *4.0f;
			end = end - (dir *fraction);
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------
bool CAutowall::HandleBulletPenetration(CSWeaponInfo *wpn_data, FireBulletData&
	data) {
	surfacedata_t *enter_surface_data = g_pPhysics->GetSurfaceData
	(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data
		->game.flPenetrationModifier;
	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= (float)(pow(wpn_data->flRangeModifier,
		(data.trace_length *0.002)));
	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f)) {
		data.penetrate_count = 0;
	}
	if (data.penetrate_count <= 0) {
		return false;
	}
	Vector dummy;
	trace_t trace_exit;
	if (!TraceToExit(dummy, &data.enter_trace, data.enter_trace.endpos,
		data.direction, &trace_exit)) {
		return false;
	}
	surfacedata_t *exit_surface_data = g_pPhysics->GetSurfaceData
	(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;
	float exit_surf_penetration_mod = exit_surface_data
		->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;
	if (((data.enter_trace.contents& CONTENTS_GRATE) != 0) || (enter_material ==
		89) || (enter_material == 71)) {
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else {
		combined_penetration_modifier = (enter_surf_penetration_mod +
			exit_surf_penetration_mod) *0.5f;
	}if (enter_material == exit_material) {
		if (exit_material == 87 || exit_material == 85) {
			combined_penetration_modifier = 3.0f;
		}
		else if (exit_material == 76) {
			combined_penetration_modifier = 2.0f;
		}
	}

	//-------------------------------------------------------------------------
	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage *final_damage_modifier) + v34 * 3.0f * fmaxf
	(0.0f, (3.0f / wpn_data->flPenetration) *1.25f);
	float thickness = VectorLength(trace_exit.endpos - data.enter_trace.endpos);
	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;
	float lost_damage = fmaxf(0.0f, v35 + thickness);
	if (lost_damage > data.current_damage) {
		return false;
	}
	if (lost_damage >= 0.0f) {
		data.current_damage -= lost_damage;
	}
	if (data.current_damage < 1.0f) {
		return false;
	}
	data.src = trace_exit.endpos;
	data.penetrate_count--;
	return true;
}

//-------------------------------------------------------------------------
void GameUtils::UTIL_TraceLine(const Vector& vecAbsStart, const Vector&
	vecAbsEnd, unsigned int mask, CBaseEntity
	*ignore, int collisionGroup, trace_t *ptr) {
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilter traceFilter;
	traceFilter.pSkip = ignore;
	g_pEngineTrace->TraceRay(ray, mask, &traceFilter, ptr);
}

//-------------------------------------------------------------------------
void GameUtils::UTIL_ClipTraceToPlayers(CBaseEntity *pEntity, Vector start,
	Vector end, unsigned int mask,
	ITraceFilter *filter, trace_t *tr) {
	trace_t playerTrace;
	Ray_t ray;
	float smallestFraction = tr->fraction;
	ray.Init(start, end);
	if (!pEntity || !pEntity->isAlive() || pEntity->IsDormant()) {
		return;
	}
	if (filter && filter->ShouldHitCBaseEntity(pEntity, mask) == false) {
		return;
	}
	g_pEngineTrace->ClipRayToCBaseEntity(ray, mask | CONTENTS_HITBOX, pEntity,
		&playerTrace);
	if (playerTrace.fraction < smallestFraction) {
		*tr = playerTrace;
		smallestFraction = playerTrace.fraction;
	}
}

//-------------------------------------------------------------------------
bool CAutowall::SimulateFireBullet(CBaseEntity *entity, CBaseEntity *local,
	CBaseCombatWeapon *weapon, FireBulletData&
	data) {
	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	auto *wpn_data = weapon->GetCSWpnData();
	if (!wpn_data) {
		return false;
	}
	data.current_damage = static_cast < float > (wpn_data->iDamage);
	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f)) {
		data.trace_length_remaining = wpn_data->flRange - data.trace_length;
		Vector end = data.src + data.direction * data.trace_length_remaining;
		GameUtils::UTIL_TraceLine(data.src, end, 0x4600400B, local, 0,
			&data.enter_trace);
		GameUtils::UTIL_ClipTraceToPlayers(entity, data.src, end + data.direction
			* 40.f, 0x4600400B, &data.filter, &data.enter_trace);
		if (data.enter_trace.fraction == 1.0f) {
			break;
		}
		if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0)
			&& (local->GetTeamNum() != data.enter_trace.m_pEnt->GetTeamNum())) {
			data.trace_length += (float)(data.enter_trace.fraction
				*data.trace_length_remaining);
			data.current_damage *= (float)(pow(wpn_data->flRangeModifier,
				data.trace_length *0.002));
			ScaleDamage(data.enter_trace.hitgroup, data.enter_trace.m_pEnt,
				wpn_data->flArmorRatio, data.current_damage);
			return true;
		}
		if (!HandleBulletPenetration(wpn_data, data)) {
			break;
		}
	}
	return false;
}

//-------------------------------------------------------------------------
bool CAutowall::CanHit(CBaseEntity *entity, const Vector& point, float
	*damage_given) {
	auto data = FireBulletData(G::LocalPlayer->GetEyePosition());
	data.filter = CTraceFilter();
	data.filter.pSkip = G::LocalPlayer;
	Vector angles = GameUtils::CalculateAngle(data.src, point);
	Math::AngleVectors(angles, &data.direction);
	VectorNormalize(data.direction);
	if (SimulateFireBullet(entity, G::LocalPlayer, G::LocalPlayer->GetWeapon(),
		data)) {
		*damage_given = data.current_damage;
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------
bool CAutowall::CanWallBang(float& dmg) {
	auto m_local = G::LocalPlayer;
	if (m_local && m_local->isAlive()) {
		FireBulletData data = FireBulletData(m_local->GetEyePosition());
		data.filter = CTraceFilter();
		data.filter.pSkip = m_local;
		Vector eye_angle;
		g_pEngine->GetViewAngles(eye_angle);
		Vector dst, forward;
		Math::AngleVectors(eye_angle, &forward);
		dst = data.src + (forward *8196.f);
		Vector angles;
		Math::CalcAngle(data.src, dst, angles);
		Math::AngleVectors(angles, &data.direction);
		Math::NormalizeVector(data.direction);
		auto m_weapon = m_local->GetWeapon();
		if (m_weapon) {
			data.penetrate_count = 1;
			data.trace_length = 0.0f;
			CSWeaponInfo *weapon_data = m_weapon->GetCSWpnData();
			if (weapon_data) {
				data.current_damage = weapon_data->iDamage;
				data.trace_length_remaining = weapon_data->flRange -
					data.trace_length;
				Vector end = data.src + data.direction *
					data.trace_length_remaining;
				GameUtils::TraceLine(data.src, end, MASK_SHOT | CONTENTS_GRATE,
					m_local, &data.enter_trace);
				if (data.enter_trace.fraction == 1.0f) {
					return false;
				}
				if (HandleBulletPenetration(weapon_data, data)) {
					dmg = data.current_damage;
					return true;
				}
			}
		}
	}
	return false;
}















