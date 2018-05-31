#pragma once
namespace GameUtils
{
	bool WorldToScreen(const Vector& in, Vector& position);
	std::vector<Vector> GetMultiplePointsForHitbox(CBaseEntity * pBaseEntity, int iHitbox, VMatrix BoneMatrix[128]);

	float BestHitPoint(CBaseEntity * player, int prioritized, float minDmg, mstudiohitboxset_t * hitset, VMatrix matrix[], Vector & vecOut);

	Vector CalculateBestPoint(CBaseEntity * player, int prioritized, float minDmg, bool onlyPrioritized, VMatrix matrix[]);
	
	float GetFoV(QAngle qAngles, Vector vecSource, Vector vecDestination, bool bDistanceBased);
	QAngle CalculateAngle(Vector vecOrigin, Vector vecOther);
	Vector GetBonePosition(CBaseEntity * pPlayer, int Bone, VMatrix MatrixArray[128]);
	void TraceLine(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, CBaseEntity* ignore, trace_t* ptr);
	bool IsVisible_Fix(Vector vecOrigin, Vector vecOther, unsigned int mask, CBaseEntity* pCBaseEntity, CBaseEntity* pIgnore, int& hitgroup);
	bool IsAbleToShoot();
	bool IsBreakableEntity(CBaseEntity* pBaseEntity);
	void UTIL_TraceLine(const Vector & vecAbsStart, const Vector & vecAbsEnd, unsigned int mask, CBaseEntity * ignore, int collisionGroup, trace_t * ptr);
	void UTIL_ClipTraceToPlayers(CBaseEntity * pEntity, Vector start, Vector end, unsigned int mask, ITraceFilter * filter, trace_t * tr);
	void ClipTraceToPlayers(const Vector & vecAbsStart, const Vector & vecAbsEnd, unsigned int mask, ITraceFilter * filter, CGameTrace * tr);
}