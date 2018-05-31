#pragma once
#include "sdk.h"
#include "global.h"
#include "vmt.h"

using do_post_screen_space_effects_t = bool(__thiscall*)(void*, CViewSetup*);
bool _fastcall hkDoPostScreenSpaceEffects(void *ecx, void *edx, CViewSetup
	*pSetup) {
	static auto ofunc = g_pClientModeHook->GetMethod <
		do_post_screen_space_effects_t >(44);
	IMaterial *pMatGlowColor = g_pMaterialSystem->FindMaterial("dev/glow_color",
		TEXTURE_GROUP_OTHER, true);
	g_pModelRender->ForcedMaterialOverride(pMatGlowColor);
	float Red = Clientvariables->Colors.Glow[0];
	float Green = Clientvariables->Colors.Glow[1];
	float Blue = Clientvariables->Colors.Glow[2];
	float Opacity = Clientvariables->Visuals.Opacity;
	float LRed = Clientvariables->Colors.LocalGlow[0];
	float LGreen = Clientvariables->Colors.LocalGlow[1];
	float LBlue = Clientvariables->Colors.LocalGlow[2];
	if (Clientvariables->Visuals.Glow && g_GlowObjManager && g_pEngine
		->IsConnected()) {
		if (G::LocalPlayer) {
			for (int i = 0; i <= g_GlowObjManager->m_GlowObjectDefinitions.Count()
				; i++) {
				if (g_GlowObjManager->m_GlowObjectDefinitions[i].IsUnused() ||
					g_GlowObjManager->m_GlowObjectDefinitions[i].getEnt()
					->IsDormant() || !g_GlowObjManager
					->m_GlowObjectDefinitions[i].getEnt()) {
					continue;
				}
				CGlowObjectManager::GlowObjectDefinition_t *glowEnt =
					&g_GlowObjManager->m_GlowObjectDefinitions[i];
				if (glowEnt != nullptr) {
					ClientClass *pClass = (ClientClass*)glowEnt->getEnt()
						->GetClientClass();
					switch (pClass->m_ClassID) {
					case 35:
						if (Clientvariables->Visuals.LocalGlow && glowEnt->getEnt()
							== G::LocalPlayer) {
							glowEnt->set(LRed, LGreen, LBlue, Opacity);
						}
						if (Clientvariables->Visuals.EnemyOnly && glowEnt->getEnt()
							->GetTeamNum() == G::LocalPlayer->GetTeamNum()) {
							break;
						}
						glowEnt->set(Red, Green, Blue, Opacity);
						break;
					default:
						if (Clientvariables->Visuals.DroppedWeapons && pClass
							->m_pNetworkName, "Weapon") {
							glowEnt->set(Red, Green, Blue, Opacity);
						}
					}
				}
			}
		}
		return ofunc(ecx, pSetup);
	}
}