#include "sdk.h"
#include "Math.h"
#include "global.h"
#include "GameUtils.h"
#include "xor.h"

CBaseCombatWeapon* CBaseEntity::GetWeapon()
{
	ULONG WeaponUlong = *(PULONG)((DWORD)this + offys.m_hActiveWeapon); // hActiveWeapon
	return (CBaseCombatWeapon*)(g_pEntitylist->GetClientEntityFromHandle(WeaponUlong));
}

DWORD GetCSWpnDataAddr;

int CBaseEntity::GetSequenceActivity(int sequence)
{
	auto hdr = g_pModelInfo->GetStudioModel(this->GetModel());

	if (!hdr)
		return -1;

	static auto getSequenceActivity = (DWORD)(Utilities::Memory::FindPatternIDA(XorStr("client.dll"), XorStr("55 8B EC 83 7D 08 FF 56 8B F1 74 3D")));
	static auto GetSequenceActivity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(getSequenceActivity);

	return GetSequenceActivity(this, hdr, sequence);
}

void CBaseEntity::SetCurrentCommand(CUserCmd* cmd)
{
	static int offset = g_pNetVars->GetOffset("DT_BasePlayer", "m_hConstraintEntity");
	*Member<CUserCmd**>(this, (offset - 0xC)) = cmd;
}

bool CBaseEntity::IsValidRenderable()
{

	if (!this || this == nullptr || G::LocalPlayer == nullptr)
		return false;

	if (this == G::LocalPlayer)
		return false;

	if (this->GetTeamNum() == G::LocalPlayer->GetTeamNum())
		return false;

	if (this->IsDormant())
		return false;

	if (!this->isAlive())
		return false;

	return true;
}

void CBaseEntity::SetAbsOrigin(const Vector& origin)
{
	using SetAbsOriginFn = void(__thiscall*)(void*, const Vector& origin);
	static SetAbsOriginFn SetAbsOrigin = (SetAbsOriginFn)FindPatternIDA("client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");
	SetAbsOrigin(this, origin);
}

bool CBaseEntity::IsValidTarget()
{

	if (!this || this == nullptr)
		return false;

	ClientClass* pClass = (ClientClass*)this->GetClientClass(); 

	if (this == G::LocalPlayer)
		return false;

	if (pClass->m_ClassID != 35)
		return false;

	if (this->GetTeamNum() == G::LocalPlayer->GetTeamNum())
		return false;

	if (this->IsDormant())
		return false;

	if (!this->isAlive())
		return false;

	if (this->IsProtected())
		return false;

	return true;
}

void CBaseEntity::SetAngle2(Vector wantedang)
{
	typedef void(__thiscall * SetAngleFn)(void*, const Vector&);
	static SetAngleFn SetAngle = (SetAngleFn)((DWORD)Utilities::Memory::FindPatternIDA("client.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1"));
	SetAngle(this, wantedang);
}

bool CBaseEntity::canHit(Vector end, CBaseEntity* ent)
{
	Ray_t ray;
	trace_t tr;
	CTraceFilter traceFilter;
	traceFilter.pSkip = this;
	ray.Init(this->GetEyePosition(), end);
	g_pEngineTrace->ClipRayToCBaseEntity(ray, MASK_SHOT, ent, &tr); // ignore grate

	if (!tr.m_pEnt)
		return false;

	CBaseEntity* pEnt = (CBaseEntity*)tr.m_pEnt;

	if (pEnt->GetTeamNum() != this->GetTeamNum())
		return true;

	return false;
}

Vector CBaseEntity::GetBonePos(int i)
{
	VMatrix boneMatrix[128];
	if (this->SetupBones(boneMatrix, 128, BONE_USED_BY_HITBOX, g_pGlobals->curtime)) {
		return Vector(boneMatrix[i][0][3], boneMatrix[i][1][3], boneMatrix[i][2][3]);
	}
	return Vector(0, 0, 0);
}

CSWeaponInfo* CBaseCombatWeapon::GetCSWpnData()
{

	typedef CSWeaponInfo*(__thiscall * OriginalFn)(void*);
	return CallVFunction<OriginalFn>(this, 445)(this);
}
#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / g_pGlobals->interval_per_tick))
int CBaseEntity::GetChockedPackets()
{
	if (GetSimulationTime() > GetOldSimulationTime())
		return TIME_TO_TICKS(fabs(GetSimulationTime() - GetOldSimulationTime()));
	return 0;
}
Vector& CBaseEntity::m_vecNetworkOrigin()
{

	static int offset = g_pNetVars->GetOffset("DT_CSPlayer", "m_flFriction") - sizeof(Vector);
	return *(Vector*)((DWORD)this + offset);
}

float CBaseEntity::GetOldSimulationTime()
{

	static uintptr_t offset = g_pNetVars->GetOffset("DT_CSPlayer", "m_flSimulationTimen") + 0x4;
	return *(float*)((DWORD)this + offset);
}
bool CBaseEntity::SetupBones(VMatrix* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{

	__asm {
		mov edi, this
		lea ecx, dword ptr ds : [edi + 0x4]
		mov edx, dword ptr ds : [ecx]
		push currentTime
		push boneMask
		push nMaxBones
		push pBoneToWorldOut
		call dword ptr ds : [edx + 0x34]
	}
}

bool CBaseEntity::IsTargettingLocal()
{

	Vector src, dst, forward;
	trace_t tr;

	if (!this || !G::LocalPlayer || G::LocalPlayer->GetHealth() < 0)
		return false;

	Vector viewangle = this->GetEyeAngles();

	Math::AngleVectors(viewangle, &forward);
	forward *= 8142.f;
	src = this->GetEyePosition();
	dst = src + forward;

	Ray_t ray;
	ray.Init(src, dst);
	CTraceCBaseEntity filter;
	filter.pHit = G::LocalPlayer;

	g_pEngineTrace->TraceRay_NEW(ray, MASK_SHOT, &filter, &tr);

	if (tr.m_pEnt && tr.m_pEnt->GetTeamNum() != this->GetTeamNum())
		return true;

	return false;
}
bool CBaseEntity::IsPlayer()
{
	ClientClass* pClass = (ClientClass*)this->GetClientClass();
	return pClass->m_ClassID == 35;
}

bool CBaseCombatWeapon::IsReloadingVisually()
{
	static int m_bReloadVisuallyComplete = g_pNetVars->GetOffset(XorStr("DT_WeaponCSBase"), XorStr("m_bReloadVisuallyComplete"));
	return !GetFieldValue<bool>(m_bReloadVisuallyComplete);
}

float_t& CBaseEntity::m_flMaxspeed()
{
	static unsigned int _m_flMaxspeed = g_pData->Find(GetPredDescMap(), "m_flMaxspeed");
	return *(float_t*)((uintptr_t)this + _m_flMaxspeed);
}

float_t& CBaseEntity::m_surfaceFriction()
{
	static unsigned int _m_surfaceFriction = g_pData->Find(GetPredDescMap(), "m_surfaceFriction");
	return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
}

















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































