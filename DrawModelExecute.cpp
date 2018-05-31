#include "hooks.h"
#include "Menu.h"
#include "global.h"
#include "MaterialHelper.h"
#include "xor.h"
#include "GameUtils.h"
void __fastcall Hooks::scene_end(void *thisptr, void *edx) {
	static auto scene_end_o = renderviewVMT->GetOriginalMethod < decltype
	(&scene_end) >(9);
	scene_end_o(thisptr, edx);
	if (Clientvariables->Visuals.ChamsEnable) {
		static IMaterial *CoveredLit = g_MaterialHelper->CreateMaterial(true);
		static IMaterial *OpenLit = g_MaterialHelper->CreateMaterial(false);
		static IMaterial *CoveredFlat = g_MaterialHelper->CreateMaterial(true,
			false);
		static IMaterial *OpenFlat = g_MaterialHelper->CreateMaterial(false,
			false);
		IMaterial *covered = Clientvariables->Visuals.ChamsStyle == 1 ?
			CoveredLit : CoveredFlat;
		IMaterial *open = Clientvariables->Visuals.ChamsStyle == 1 ? OpenLit :
			OpenFlat;
		IMaterial *material = nullptr;
		IMaterial *visible = g_pMaterialSystem->FindMaterial("cstm_visible",
			TEXTURE_GROUP_MODEL);
		auto b_IsThirdPerson = *reinterpret_cast < bool * > (reinterpret_cast <
			DWORD > (g_pInput) + 0xA5);
		for (int i = 1; i < g_pEngine->GetMaxClients(); ++i) {
			CBaseEntity *ent = (CBaseEntity*)g_pEntitylist->GetClientEntity(i);
			if (ent == G::LocalPlayer && G::LocalPlayer != nullptr) {
				if (G::LocalPlayer->isAlive()) {
					if (b_IsThirdPerson && Clientvariables->Visuals.FakeAngleGhost
						!= 0) {
						Vector OrigAng;
						OrigAng = ent->GetEyeAngles();
						if (Clientvariables->Visuals.FakeAngleGhost == 1) {
							ent->SetAngle2(Vector(0, G::FakeAngle.y, 0));
						}
						else {
							ent->SetAngle2(Vector(0, G::LocalPlayer->LowerBodyYaw(), 0)
							);
						}
						g_pRenderView->SetColorModulation(Clientvariables
							->Colors.FakeAngleGhost);
						g_pModelRender->ForcedMaterialOverride(visible);
						ent->draw_model(0x1, 200);
						g_pModelRender->ForcedMaterialOverride(nullptr);
						ent->SetAngle2(OrigAng);
					}
				}
			}
			if (ent->IsValidRenderable() && Clientvariables->Visuals.ChamsPlayer) {
				if (Clientvariables->Visuals.ChamsPlayerWall) {
					g_pRenderView->SetColorModulation(Clientvariables
						->Colors.PlayerChamsWall);
					g_pModelRender->ForcedMaterialOverride(covered);
					ent->draw_model(0x1 /*STUDIO_RENDER*/, 255);
					g_pModelRender->ForcedMaterialOverride(nullptr);
				}
				g_pRenderView->SetColorModulation(Clientvariables
					->Colors.PlayerChams);
				g_pModelRender->ForcedMaterialOverride(open);
				ent->draw_model(0x1 /*STUDIO_RENDER*/, 255);
				g_pModelRender->ForcedMaterialOverride(nullptr);
			}
			if (ent == G::LocalPlayer && Clientvariables->Visuals.LocalChams) {
				g_pRenderView->SetColorModulation(Clientvariables
					->Colors.LocalChams);
				g_pModelRender->ForcedMaterialOverride(open);
				ent->draw_model(0x1, 255);
				g_pModelRender->ForcedMaterialOverride(nullptr);
			}
		}
	}
}

//-------------------------------------------------------------------------
void __fastcall Hooks::DrawModelExecute(void *ecx, void *edx, void **ctx, void
	*state, const ModelRenderInfo_t& pInfo,
	matrix3x4_t *pCustomBoneToWorld) {
	if (!G::LocalPlayer) {
		modelrenderVMT->GetOriginalMethod < DrawModelExecuteFn >(21)(ecx, ctx,
			state, pInfo, pCustomBoneToWorld);
		return;
	}
	const char *ModelName = g_pModelInfo->GetModelName((model_t*)pInfo.pModel);
	static IMaterial *pWireframeMaterial = g_MaterialHelper->CreateMaterial
	(false, false, true);
	static IMaterial *OpenFlat = g_MaterialHelper->CreateMaterial(false, false);
	static IMaterial *Glass1 = g_pMaterialSystem->FindMaterial(
		"models/inventory_items/cologne_prediction/cologne_prediction_glass",
		nullptr);
	static IMaterial *Crystalclear = g_pMaterialSystem->FindMaterial(
		"models/inventory_items/trophy_majors/crystal_clear", nullptr);
	if (G::LocalPlayer->IsScoped()) {
		if (strstr(ModelName, "models/player")) {
			g_pRenderView->SetBlend(0.5f);
		}
	}
	if (!*(bool*)((DWORD)g_pInput + 0xA5)) {
		// Thirdperson
	}
	modelrenderVMT->GetOriginalMethod < DrawModelExecuteFn >(21)(ecx, ctx,
		state, pInfo, pCustomBoneToWorld);
	g_pModelRender->ForcedMaterialOverride(NULL);
}















