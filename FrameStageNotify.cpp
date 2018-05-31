#include "hooks.h"
#include "global.h"
#include "Menu.h"
#include "BacktrackingHelper.h"
#include "xor.h"
#include <intrin.h>
#include "SpoofedConvar.h"
#include "Math.h"
#include "Skinchanger.h"
#include "Items.h"
#include <deque>
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define TICK_INTERVAL (g_pGlobals->interval_per_tick)
#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / TICK_INTERVAL))
#define TICKS_TO_TIME(t) (TICK_INTERVAL * (t))
ConVar *mp_facefronttime;
ConVar *r_DrawSpecificStaticProp;
DWORD eyeangles;
int Shots;
bool IsNearEqual(float v1, float v2, float Tolerance) {
	return std::abs(v1 - v2) <= std::abs(Tolerance);
}

//-------------------------------------------------------------------------
struct STickRecord {
	void SaveRecord(CBaseEntity *player) {
		m_flLowerBodyYawTarget = player->LowerBodyYaw();
		;
		m_angEyeAngles = player->GetEyeAngles();
		m_flSimulationTime = player->GetSimulationTime();
		m_flPoseParameter = player->m_flPoseParameter();
		m_flCurTime = g_pGlobals->curtime;
		m_nFlags = *player->GetFlags();
		m_flVelocity = player->Velocity().Length2D();
		m_vecVelocity = player->Velocity();
		m_iLayerCount = player->GetNumAnimOverlays();
		for (int i = 0; i < m_iLayerCount; i++) {
			animationLayer[i] = player->GetAnimOverlays()[i];
		}
	}
	bool operator == (STickRecord& other) {
		return other.m_flSimulationTime == m_flSimulationTime;
	}
	int tickcount = 0;
	float m_flVelocity = 0.f;
	Vector m_vecVelocity = Vector(0, 0, 0);
	float m_flSimulationTime = 0.f;
	float m_flLowerBodyYawTarget = 0.f;
	Vector m_angEyeAngles = Vector(0, 0, 0);
	std::array < float, 24 > m_flPoseParameter = {}
	;
	float m_flCurTime = 0.f;
	int m_nFlags = 0;
	int m_iLayerCount = 0;
	AnimationLayer animationLayer[15];
};
struct SResolveInfo {
	std::deque < STickRecord > arr_tickRecords;
	STickRecord curTickRecord;
	STickRecord prevTickRecord;
	float m_flLastLbyTime = 0.f;
	Vector m_angDirectionFirstMoving = Vector(0, 0, 0);
}; bool IsAdjustingBalance(CBaseEntity *player, STickRecord& record,
	AnimationLayer *layer) {
	for (int i = 0; i < record.m_iLayerCount; i++) {
		const int activity = player->GetSequenceActivity
		(record.animationLayer[i].m_nSequence);
		if (activity == 979) {
			*layer = record.animationLayer[i];
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------
int get_model_index(int item_def_index) {
	int ret = 0;
	switch (item_def_index) {
	case KNIFE:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_default_ct.mdl"));
		break;
	case KNIFE_T:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_default_t.mdl"));
		break;
	case KNIFE_KARAMBIT:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_karam.mdl"));
		break;
	case KNIFE_BAYONET:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_bayonet.mdl"));
		break;
	case KNIFE_M9_BAYONET:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_m9_bay.mdl"));
		break;
	case KNIFE_TACTICAL:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_tactical.mdl"));
		break;
	case KNIFE_GUT:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_gut.mdl"));
		break;
	case KNIFE_FALCHION:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_falchion_advanced.mdl"));
		break;
	case KNIFE_PUSH:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_push.mdl"));
		break;
	case KNIFE_BUTTERFLY:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_butterfly.mdl"));
		break;
	case KNIFE_FLIP:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_flip.mdl"));
		break;
	case KNIFE_BOWIE:
		ret = g_pModelInfo->GetModelIndex(XorStr(
			"models/weapons/v_knife_survival_bowie.mdl"));
		break;
	default:
		break;
	}
	return ret;
}

//-------------------------------------------------------------------------
std::vector < const char * > vistasmoke_mats = {
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
};
std::vector < const char * > smoke_materials = {
	"particle/beam_smoke_01", "particle/particle_smokegrenade",
	"particle/particle_smokegrenade1", "particle/particle_smokegrenade2",
	"particle/particle_smokegrenade3", "particle/particle_smokegrenade_sc",
	"particle/smoke1/smoke1", "particle/smoke1/smoke1_ash",
	"particle/smoke1/smoke1_nearcull", "particle/smoke1/smoke1_nearcull2",
	"particle/smoke1/smoke1_snow", "particle/smokesprites_0001",
	"particle/smokestack", "particle/vistasmokev1/vistasmokev1",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_nearcull",
	"particle/vistasmokev1/vistasmokev1_nearcull_fog",
	"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev4_emods_nocull",
	"particle/vistasmokev1/vistasmokev4_nearcull",
	"particle/vistasmokev1/vistasmokev4_nocull"
};
bool lowerBodyYawUpdated(CBaseEntity *pEntity) {
	for (int w = 0; w < 13; w++) {
		AnimationLayer currentLayer = pEntity->GetAnimOverlay(w);
		const int activity = pEntity->GetSequenceActivity
		(currentLayer.m_nSequence);
		float flcycle = currentLayer.m_flCycle, flprevcycle =
			currentLayer.m_flPrevCycle, flweight = currentLayer.m_flWeight,
			flweightdatarate = currentLayer.m_flWeightDeltaRate;
		uint32_t norder = currentLayer.m_nOrder;
		if (activity == 973 && flweight == 1.f || activity == 979 && flweight ==
			1.0f && currentLayer.m_flPrevCycle != currentLayer.m_flCycle) {
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------
bool BreakingMoreThan120(CBaseEntity *pEntity) {
	for (int w = 0; w < 13; w++) {
		AnimationLayer prevlayer;
		AnimationLayer currentLayer = pEntity->GetAnimOverlay(w);
		const int activity = pEntity->GetSequenceActivity
		(currentLayer.m_nSequence);
		float flcycle = currentLayer.m_flCycle, flprevcycle =
			currentLayer.m_flPrevCycle, flweight = currentLayer.m_flWeight,
			flweightdatarate = currentLayer.m_flWeightDeltaRate;
		uint32_t norder = currentLayer.m_nOrder;
		if (activity == ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING || activity ==
			ACT_CSGO_IDLE_TURN_BALANCEADJUST && flweight >= .99 &&
			currentLayer.m_flPrevCycle != currentLayer.m_flCycle) {
			float flanimTime = currentLayer.m_flCycle, flsimtime = pEntity
				->GetSimulationTime();
			return true;
		}
		prevlayer = currentLayer;
		return false;
	}
	return false;
}bool Under120(CBaseEntity *pEntity) {
	for (int w = 0; w < 13; w++) {
		AnimationLayer prevlayer;
		AnimationLayer currentLayer = pEntity->GetAnimOverlay(w);
		const int activity = pEntity->GetSequenceActivity
		(currentLayer.m_nSequence);
		float flcycle = currentLayer.m_flCycle, flprevcycle =
			currentLayer.m_flPrevCycle, flweight = currentLayer.m_flWeight,
			flweightdatarate = currentLayer.m_flWeightDeltaRate;
		uint32_t norder = currentLayer.m_nOrder;
		if (activity == 979 && currentLayer.m_flWeight == 0.f &&
			currentLayer.m_flPrevCycle != currentLayer.m_flCycle) {
			return true;
		}
		prevlayer = currentLayer;
	}
	return false;
}

//-------------------------------------------------------------------------
bool playerStoppedMoving(CBaseEntity *pEntity) {
	for (int w = 0; w < 13; w++) {
		AnimationLayer currentLayer = pEntity->GetAnimOverlay(w);
		const int activity = pEntity->GetSequenceActivity
		(currentLayer.m_nSequence);
		float flcycle = currentLayer.m_flCycle, flprevcycle =
			currentLayer.m_flPrevCycle, flweight = currentLayer.m_flWeight,
			flweightdatarate = currentLayer.m_flWeightDeltaRate;
		uint32_t norder = currentLayer.m_nOrder;
		if (activity == ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING) {
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------
bool lastLbyupdate(CBaseEntity *pEntity) {
	static float prediction = 0.f;
	static bool secondrun = false;
	float flServerTime = (float)pEntity->GetTickBase() *g_pGlobals
		->interval_per_tick;
	if (playerStoppedMoving(pEntity) && !secondrun) {
		prediction = flServerTime + 0.22;
		secondrun = true;
	}
	else if (pEntity->GetVelocity().Length2D() < 0.1f && secondrun &&
		prediction <= pEntity->GetSimulationTime()) {
		prediction = pEntity->GetTickBase() + 1.1f;
	}
	else {
		secondrun = false;
		return false;
	}
	if (prediction <= pEntity->GetSimulationTime()) {
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------
bool IsOnAir(CBaseEntity *pEntity) {
	if (!pEntity->GetFlags()& FL_ONGROUND) {
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------
enum {
	TE_SPRITE = 0x01, TE_BEAMDISK = 0x02, TE_BEAMCYLINDER = 0x03, TE_BEAMFOLLOW
	= 0x04, TE_BEAMRING = 0x05, TE_BEAMSPLINE = 0x06, TE_BEAMRINGPOINT = 0x07,
	TE_BEAMLASER = 0x08, TE_BEAMTESLA = 0x09,
};
enum {
	FBEAM_STARTENTITY = 0x00000001, FBEAM_ENDENTITY = 0x00000002, FBEAM_FADEIN =
	0x00000004, FBEAM_FADEOUT = 0x00000008, FBEAM_SINENOISE = 0x00000010,
	FBEAM_SOLID = 0x00000020, FBEAM_SHADEIN = 0x00000040, FBEAM_SHADEOUT =
	0x00000080, FBEAM_ONLYNOISEONCE = 0x00000100, FBEAM_NOTILE = 0x00000200,
	FBEAM_USE_HITBOXES = 0x00000400, FBEAM_STARTVISIBLE = 0x00000800,
	FBEAM_ENDVISIBLE = 0x00001000, FBEAM_ISACTIVE = 0x00002000, FBEAM_FOREVER
	= 0x00004000, FBEAM_HALOBEAM = 0x00008000, FBEAM_REVERSED = 0x00010000,
	NUM_BEAM_FLAGS = 17
};
void DrawBeam(Vector src, Vector end, Color color) {
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;
	beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
	beamInfo.m_nModelIndex = -1;
	beamInfo.m_flHaloScale = 2.0f;
	beamInfo.m_flLife = 1.0f;
	beamInfo.m_flWidth = 3.8f;
	beamInfo.m_flEndWidth = 3.8f;
	beamInfo.m_flFadeLength = 0.2f;
	beamInfo.m_flAmplitude = 2.0f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = 0;
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;
	Beam_t *myBeam = g_pViewRenderBeams->CreateBeamPoints(beamInfo);
	if (myBeam) {
		g_pViewRenderBeams->DrawBeam(myBeam);
	}
}

//-------------------------------------------------------------------------
void __stdcall Hooks::FrameStageNotify(ClientFrameStage_t curStage) {
	if (g_pEngine->IsConnected() && g_pEngine->IsInGame()) {
		static std::string old_Skyname = XorStr("");
		static bool OldNightmode;
		static int OldSky;
		if (!G::LocalPlayer || !g_pEngine->IsConnected() || !g_pEngine->IsInGame()
			) {
			clientVMT->GetOriginalMethod < FrameStageNotifyFn >(36)(curStage);
			old_Skyname = XorStr("");
			OldNightmode = false;
			OldSky = 0;
			return;
		}
		if (OldNightmode != Clientvariables->Visuals.nightmode) {
			if (!r_DrawSpecificStaticProp) {
				r_DrawSpecificStaticProp = g_pCvar->FindVar(XorStr(
					"r_DrawSpecificStaticProp"));
			}
			r_DrawSpecificStaticProp->SetValue(0);
			for (MaterialHandle_t i = g_pMaterialSystem->FirstMaterial(); i !=
				g_pMaterialSystem->InvalidMaterial(); i = g_pMaterialSystem
				->NextMaterial(i)) {
				IMaterial *pMaterial = g_pMaterialSystem->GetMaterial(i);
				if (!pMaterial) {
					continue;
				}
				if (strstr(pMaterial->GetTextureGroupName(), XorStr("World")) ||
					strstr(pMaterial->GetTextureGroupName(), XorStr("StaticProp")))
				{
					if (Clientvariables->Visuals.nightmode)
						if (strstr(pMaterial->GetTextureGroupName(), XorStr(
							"StaticProp"))) {
							pMaterial->ColorModulate(0.3f, 0.3f, 0.3f);
						}
						else {
							pMaterial->ColorModulate(0.05f, 0.05f, 0.05f);
						}
					else {
						pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
					}
				}
			}
			OldNightmode = Clientvariables->Visuals.nightmode;
		}
		if (OldSky != Clientvariables->Visuals.Skybox) {
			auto LoadNamedSky = reinterpret_cast < void(__fastcall*)(const char*)
			> (FindPatternIDA(XorStr("engine.dll"), XorStr(
				"55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45")));
			if (old_Skyname == XorStr("")) {
			}
			int type = Clientvariables->Visuals.Skybox;
			if (type == 1) {
				LoadNamedSky(XorStr("cs_baggage_skybox_"));
			}
			else if (type == 2) {
				LoadNamedSky(XorStr("cs_tibet"));
			}
			else if (type == 3) {
				LoadNamedSky(XorStr("italy"));
			}
			else if (type == 4) {
				LoadNamedSky(XorStr("jungle"));
			}
			else if (type == 5) {
				LoadNamedSky(XorStr("office"));
			}
			else if (type == 6) {
				LoadNamedSky(XorStr("sky_cs15_daylight02_hdr"));
			}
			else if (type == 7) {
				LoadNamedSky(XorStr("sky_csgo_night02"));
			}
			else if (type == 8) {
				LoadNamedSky(XorStr("vertigo"));
			}
			OldSky = Clientvariables->Visuals.Skybox;
		}
		static bool TransparensWalls = false;
		if (TransparensWalls != Clientvariables->Visuals.TransparensWallsEnable
			&& g_pEngine->IsConnected() && g_pEngine->IsInGame()) {
			if (!r_DrawSpecificStaticProp) {
				r_DrawSpecificStaticProp = g_pCvar->FindVar(XorStr(
					"r_DrawSpecificStaticProp"));
			}
			r_DrawSpecificStaticProp->SetValue(0);
			for (MaterialHandle_t i = g_pMaterialSystem->FirstMaterial(); i !=
				g_pMaterialSystem->InvalidMaterial(); i = g_pMaterialSystem
				->NextMaterial(i)) {
				IMaterial *pMaterial = g_pMaterialSystem->GetMaterial(i);
				if (!pMaterial) {
					continue;
				}
				if (strstr(pMaterial->GetTextureGroupName(), XorStr("StaticProp")))
				{
					pMaterial->AlphaModulate(0.35f);
				}
			}
			TransparensWalls = Clientvariables->Visuals.TransparensWallsEnable;
		}
		static Vector oldViewPunch;
		static Vector oldAimPunch;
		Vector *view_punch = G::LocalPlayer->GetViewPunchPtr();
		Vector *aim_punch = G::LocalPlayer->GetPunchAnglePtr();
		if (curStage == FRAME_RENDER_START && G::LocalPlayer->GetHealth() > 0) {
			static bool enabledtp = false, check = false;
			if (GetAsyncKeyState(Clientvariables->Misc.TPKey)) {
				if (!check) {
					enabledtp = !enabledtp;
				} check = true;
			}
			else {
				check = false;
			}if (enabledtp) {
				*reinterpret_cast < QAngle * > (reinterpret_cast < DWORD > (G::LocalPlayer +
					0x31C0 + 0x8)) = G::AAAngle;
			}

			//-------------------------------------------------------------------------
			if (view_punch && aim_punch && Clientvariables->Visuals.Novisrevoil) {
				oldViewPunch = *view_punch;
				oldAimPunch = *aim_punch;
				view_punch->Init();
				aim_punch->Init();
			}

			//-------------------------------------------------------------------------
			if (enabledtp && G::LocalPlayer->isAlive()) {
				*(bool*)((DWORD)g_pInput + 0xA5) = true;
				*(float*)((DWORD)g_pInput + 0xA8 + 0x8) = 150;
			}
			else {
				*(bool*)((DWORD)g_pInput + 0xA5) = false;
				*(float*)((DWORD)g_pInput + 0xA8 + 0x8);
			}

			//-------------------------------------------------------------------------

		}

		//-------------------------------------------------------------------------
		if (curStage == FRAME_NET_UPDATE_START) {
			if (Clientvariables->Visuals.BulletTracers) {
				float Red, Green, Blue;
				Red = Clientvariables->Colors.Bulletracer[0] * 255;
				Green = Clientvariables->Colors.Bulletracer[1] * 255;
				Blue = Clientvariables->Colors.Bulletracer[2] * 255;
				for (unsigned int i = 0; i < trace_logs.size(); i++) {
					auto *shooter = g_pEntitylist->GetClientEntity(g_pEngine
						->GetPlayerForUserID(trace_logs[i].userid));
					if (!shooter) {
						return;
					}
					Color color;
					if (shooter->GetTeamNum() == 3) {
						color = Color(Red, Green, Blue, 255);
					}
					else {
						color = Color(Red, Green, Blue, 255);
					}
					DrawBeam(trace_logs[i].start, trace_logs[i].position, color);
					trace_logs.erase(trace_logs.begin() + i);
				}
			}
			for (auto mat_s : vistasmoke_mats) {
				IMaterial *mat = g_pMaterialSystem->FindMaterial(mat_s,
					TEXTURE_GROUP_OTHER);
				mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Clientvariables
					->Visuals.Nosmoke ? true : false);
			}
			if (Clientvariables->Visuals.Nosmoke) {
				static int *smokecount = *(int **)(FindPatternIDA(XorStr("client.dll"),
					XorStr("A3 ? ? ? ? 57 8B CB")) + 0x1);
				*smokecount = 0;
			}
			for (int i = 1; i < g_pGlobals->maxClients; i++) {
				CBaseEntity *pEntity = g_pEntitylist->GetClientEntity(i);
				if (pEntity) {
					if (pEntity->GetHealth() > 0) {
						if (i != g_pEngine->GetLocalPlayer()) {
							VarMapping_t *map = pEntity->GetVarMap();
							if (map) {
								if (Clientvariables->Ragebot.PositionAdjustment) {
									map->m_nInterpolatedEntries = 0;
								}
								else {
									if (map->m_nInterpolatedEntries == 0) {
										map->m_nInterpolatedEntries = 6;
									}
								}
							}
						}
					}
				}
			}
		}

		//-------------------------------------------------------------------------
		if (curStage == FRAME_RENDER_START) {
			for (int i = 1; i < g_pGlobals->maxClients; i++) {
				if (i == g_pEngine->GetLocalPlayer()) {
					continue;
				} CBaseEntity *pEntity = g_pEntitylist->GetClientEntity(i);
				if (pEntity) {
					if (pEntity->GetHealth() > 0 && !pEntity->IsDormant()) {
						*(int*)((uintptr_t)pEntity + 0xA30) = g_pGlobals->framecount;
						*(int*)((uintptr_t)pEntity + 0xA28) = 0;
					}
				}
			}
		}

		//-------------------------------------------------------------------------
		if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
			for (int i = 1; i < g_pGlobals->maxClients; i++) {
				if (i == g_pEngine->GetLocalPlayer()) {
					continue;
				} CBaseEntity *pEntity = g_pEntitylist->GetClientEntity(i);
				if (pEntity && pEntity->GetHealth() > 0) {
					if (pEntity->IsDormant()) {
						continue;
					}
					if (Clientvariables->Ragebot.ResolverMode == 1) {
						{
							static int missedshots[65];
							static int lbyupdated[64];
							static float Yaw[64];
							static float lastLby[64];
							static float newYaw[64];
							auto lby = pEntity->LowerBodyYaw();
							auto idx = pEntity->GetIndex();
							auto velocity = pEntity->GetVelocity().Length();
							auto length2d = pEntity->GetVelocity().Length2D();
							auto target = G::TargetIDO;
							SResolveInfo arr_infos[64];
							AnimationLayer curBalanceLayer, prevBalanceLayer;
							SResolveInfo& player_recs = arr_infos[idx]; Yaw[idx] = pEntity->GetEyeAnglesPtr()->y;
							{
								if (velocity > 0.1 && !IsOnAir(pEntity)) {
									Yaw[idx] = lby;
									lastLby[idx] = lby;
								}
								else if (velocity > 20 && velocity < 50 && !IsOnAir(pEntity)) {
									Yaw[idx] = lby + 180;
									newYaw[idx] = lby + 180;
								}
								if (IsAdjustingBalance(pEntity, player_recs.curTickRecord, &curBalanceLayer))
								{
									if (IsAdjustingBalance(pEntity, player_recs.prevTickRecord,
										&prevBalanceLayer)) {
										if ((prevBalanceLayer.m_flCycle != curBalanceLayer.m_flCycle) ||
											curBalanceLayer.m_flWeight == 1.f) {
											float flAnimTime = curBalanceLayer.m_flCycle, flSimTime = pEntity
												->GetSimulationTime();
											if (flAnimTime < 0.01f && prevBalanceLayer.m_flCycle > 0.01f &&
												g_BacktrackHelper->IsTickValid(TIME_TO_TICKS(flSimTime -
													flAnimTime))) {
												lastLby[idx] = Yaw[idx] + lby;
											}
											lastLby[idx] = Yaw[idx] - 180.f;
										}
										else if (curBalanceLayer.m_flWeight == 0.f &&
											(prevBalanceLayer.m_flCycle > 0.92f &&
												curBalanceLayer.m_flCycle > 0.92f)) {
											Yaw[idx] = lastLby[idx];
										}
									}
								}
								else if (Under120(pEntity) && missedshots[target] < 2 && !IsOnAir(pEntity)
									&& !BreakingMoreThan120(pEntity) && !lastLbyupdate(pEntity)) {
									Yaw[idx] = lby;
								}
								else if (BreakingMoreThan120(pEntity) && missedshots[target] < 2 &&
									!lastLbyupdate(pEntity) && !IsOnAir(pEntity)) {
									Yaw[idx] = newYaw[idx] - 160;
								}
								else if (lastLbyupdate(pEntity)) {
									Yaw[idx] = lby;
									newYaw[idx] = lby;
									lbyupdated[idx] = true;
								}
								if (missedshots[target] > 2 || IsOnAir(pEntity)) {
									Yaw[idx] = lby + 60 * missedshots[target];
								}
								else {
									Yaw[idx] = lastLby[idx];
								}
								pEntity->GetEyeAnglesPtr()->y = Yaw[idx];
							}

							//-------------------------------------------------------------------------

						}

						//-------------------------------------------------------------------------
						if (!Clientvariables->Misc.AntiUT) {
							std::string strPitch = std::to_string(pEntity->GetEyeAnglesPtr()->x);
							if (pEntity->GetEyeAnglesPtr()->x <  -179.f) {
								pEntity->GetEyeAnglesPtr()->x += 360.f;
							}
							else if (pEntity->GetEyeAnglesPtr()->x > 90.0 || pEntity->GetEyeAnglesPtr()
								->x <  -90.0) {
								pEntity->GetEyeAnglesPtr()->x = 89.f;
							}
							else if (pEntity->GetEyeAnglesPtr()->x > 89.0 && pEntity->GetEyeAnglesPtr()
								->x < 91.0) {
								pEntity->GetEyeAnglesPtr()->x -= 90.f;
							}
							else if (pEntity->GetEyeAnglesPtr()->x > 179.0 && pEntity->GetEyeAnglesPtr
							()->x < 181.0) {
								pEntity->GetEyeAnglesPtr()->x -= 180;
							}
							else if (pEntity->GetEyeAnglesPtr()->x >  -179.0 && pEntity
								->GetEyeAnglesPtr()->x <  -181.0) {
								pEntity->GetEyeAnglesPtr()->x += 180;
							}
							else if (fabs(pEntity->GetEyeAnglesPtr()->x) == 0) {
								pEntity->GetEyeAnglesPtr()->x = std::copysign(89.0f, pEntity
									->GetEyeAnglesPtr()->x);
							}
						}

						//-------------------------------------------------------------------------

					}
else if (Clientvariables->Ragebot.ResolverMode == 2) {
	static float MoveLBY[65];
	static int ShootsFired[65];
	auto idx = pEntity->GetIndex();
	SResolveInfo arr_infos[64];
	SResolveInfo& player_recs = arr_infos[idx];
	for (int i = 0; i <= g_pGlobals->maxClients; i++) {
		CBaseEntity *ent = g_pEntitylist->GetClientEntity(i);
		if (!ent) {
			continue;
		} if (ent->IsDormant()) {
			continue;
		} if (!ent->isAlive()) {
			continue;
		} if (ent == G::LocalPlayer) {
			continue;
		} if (ent->GetTeamNum() == G::LocalPlayer->GetTeamNum()) {
			continue;
		}
		if (G::Aimbotting) {
			ShootsFired[i]++;
		}
		if (ShootsFired[i] > 5) {
			ShootsFired[i] = 0;
		} if (ent->GetVelocity().Length2D() > 31) {
			MoveLBY[i] = ent->LowerBodyYaw();
			ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw();
		}
		else {
			float Delta = fabsf(ent->LowerBodyYaw() - MoveLBY[i]);
			if (IsNearEqual(Delta, 180, 45)) {
				switch (ShootsFired[i]) {
				case 0:
					ent->GetEyeAnglesPtr()->y = MoveLBY[i];
					break;
				case 1:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() + Delta -
						rand() % 10;
					break;
				case 2:
					ent->GetEyeAnglesPtr()->y = MoveLBY[i] - (Delta - 90) + rand()
						% 45;
					break;
				case 3:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() - (Delta +
						180) + rand() % 25;
					break;
				case 4:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() - (Delta - 45)
						+ rand() % 10;
					break;
				case 5:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() + (Delta + 90)
						+ rand() % 10;
					break;
				}
			}
			else if (Delta == 0) {
				switch (ShootsFired[i]) {
				case 0:
					ent->GetEyeAnglesPtr()->y = MoveLBY[i];
					break;
				case 1:
					ent->GetEyeAnglesPtr()->y = MoveLBY[i] + 180 - rand() % 10;
					break;
				case 2:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() - 45 + rand() %
						45;
					break;
				case 3:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() - 165 + rand()
						% 25;
					break;
				case 4:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() - 90 + rand() %
						10;
					break;
				case 5:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() + 90 + rand() %
						10;
					break;
				}
			}
			else {
				switch (ShootsFired[i]) {
				case 0:
					ent->GetEyeAnglesPtr()->y = MoveLBY[i];
					break;
				case 1:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() + Delta -
						rand() % 10;
					break;
				case 2:
					ent->GetEyeAnglesPtr()->y = MoveLBY[i] - (Delta - 90) + rand()
						% 45;
					break;
				case 3:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() - (Delta +
						180) + rand() % 25;
					break;
				case 4:
					ent->GetEyeAnglesPtr()->y = MoveLBY[i] - (Delta - 45) + rand()
						% 10;
					break;
				case 5:
					ent->GetEyeAnglesPtr()->y = ent->LowerBodyYaw() + (Delta + 90)
						+ rand() % 10;
					break;
				}
			}
		}
	}
}
else if (Clientvariables->Ragebot.ResolverMode == 3) {

}
else {
	g_BacktrackHelper->PlayerRecord[i].records.clear();
}

//-------------------------------------------------------------------------

}

//-------------------------------------------------------------------------

}

//-------------------------------------------------------------------------

}

//-------------------------------------------------------------------------
g_BacktrackHelper->AnimationFix(curStage);
if (curStage == FRAME_RENDER_START) {
	for (int i = 1; i < g_pGlobals->maxClients; i++) {
		CBaseEntity *pEntity = g_pEntitylist->GetClientEntity(i);
		if (pEntity) {
			if (pEntity->GetHealth() > 0 && !pEntity->IsDormant()) {
				g_BacktrackHelper->UpdateBacktrackRecords(pEntity);
				g_BacktrackHelper->UpdateExtrapolationRecords(pEntity);
			}
		}
	}
}

//-------------------------------------------------------------------------
clientVMT->GetOriginalMethod < FrameStageNotifyFn >(36)(curStage);
if (curStage == FRAME_RENDER_START && G::LocalPlayer && G::LocalPlayer
	->GetHealth() > 0) {
	if (Clientvariables->Visuals.Novisrevoil) {
		*aim_punch = oldAimPunch;
		*view_punch = oldViewPunch;
	}
}

//-------------------------------------------------------------------------

}
}







































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































