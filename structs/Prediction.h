#pragma once
class IMoveHelper {
public:
	void SetHost(CBaseEntity *pPlayer) {
		typedef void(__thiscall *OriginalFn)(void *, CBaseEntity *pPlayer);
		CallVFunction < OriginalFn >(this, 1)(this, pPlayer);
	}
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	int m_nPlayerHandle;
	int m_nImpulseCommand;
	QAngle m_vecViewAngles;
	QAngle m_vecAbsViewAngles;
	int m_nButtons;
	int m_nOldButtons;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;
	float m_flMaxSpeed;
	float m_flClientMaxSpeed;
	Vector m_vecVelocity;
	QAngle m_vecAngles;
	QAngle m_vecOldAngles;
	float m_outStepHeight;
	Vector m_outWishVel;
	Vector m_outJumpVel;
	Vector m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	float m_flUnknown[5];
	Vector m_vecAbsOrigin;
};
class CPrediction {
public:
	void SetupMove(CBaseEntity *ent, CUserCmd *cmd, void *move, void
		*movedata) {
		typedef void(__thiscall *fn)(void *, CBaseEntity *, CUserCmd *,
			void *, void*);
		CallVFunction < fn >(this, 20)(this, ent, cmd, move, movedata);
	}
	void FinishMove(CBaseEntity *ent, CUserCmd *cmd, void *movedata) {
		typedef void(__thiscall *fn)(void *, CBaseEntity *, CUserCmd *,
			void*);
		CallVFunction < fn >(this, 21)(this, ent, cmd, movedata);
	}
	void RunCommand(CBaseEntity *pEntity, CUserCmd *pCmd, void *moveHelper) {
		typedef void(__thiscall *fn)(void *, CBaseEntity *, CUserCmd *,
			void*);
		CallVFunction < fn >(this, 19)(this, pEntity, pCmd, moveHelper);
	}
};
class CGameMovement {
public:
	void ProcessMovement(CBaseEntity *ent, void *movedata) {
		typedef void(__thiscall *fn)(void *, CBaseEntity *, void*);
		CallVFunction < fn >(this, 1)(this, ent, movedata);
	}
	void StartTrackPredictionErrors(CBaseEntity *ent) {
		typedef void(__thiscall *fn)(void *, CBaseEntity*);
		CallVFunction < fn >(this, 3)(this, ent);
	}
	void FinishTrackPredictionErrors(CBaseEntity *ent) {
		typedef void(__thiscall *fn)(void *, CBaseEntity*);
		CallVFunction < fn >(this, 4)(this, ent);
	}
	void DecayPunchAngle() {
		typedef void(__thiscall *fn)(void*);
		CallVFunction < fn >(this, 19)(this);
	}
};