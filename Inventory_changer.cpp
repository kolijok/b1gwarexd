#include "Inventory_changer.h"
#include "sdk.h"
#include "global.h"
#include "Menu.h"
#include "Math.h"
#include "GameUtils.h"
#include "Autowall.h"
#include "BacktrackingHelper.h"
CEconItem * CreateEconItem()
{
	static auto fnCreateSharedObjectSubclass_EconItem_
		= reinterpret_cast<CEconItem*(__stdcall*)()>(
			*reinterpret_cast<uintptr_t*>(Utilities::Memory::FindPatternIDA("client.dll", "C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4") + 3)
			);
	return fnCreateSharedObjectSubclass_EconItem_();
}
uintptr_t GetItemSchema()
{
	static auto fnGetItemSchema
		= reinterpret_cast<uintptr_t(__stdcall*)()>(
			Utilities::Memory::FindPatternIDA("client.dll", "A1 ? ? ? ? 85 C0 75 53")
			);
	return fnGetItemSchema();
}
auto Item = CreateEconItem();

iv g_inve;
void iv::move(int ob,  int skin) {
	auto LocalInventory = **reinterpret_cast<CPlayerInventory***>(Utilities::Memory::FindPatternIDA("client.dll", "8B 3D ? ? ? ? 85 FF 74 1A") + 0x2);
	auto g_CSInventoryManagera = *reinterpret_cast<CPlayerInventory**>(Utilities::Memory::FindPatternIDA("client.dll", "B9 ?? ?? ?? ?? 8D 44 24 10 89 54 24 14") + 0x1);
	auto LocalInventorya = g_CSInventoryManagera->GetLocalPlayerInventoary();





			*Item->GetAccountID() = LocalInventorya->GetSteamID();
			*Item->GetDefIndex() = ob;
			*Item->GetItemID() = Math::RandomFloat(1, 50000);
			*Item->GetInventory() = 1;
			*Item->GetFlags() = 0;
			*Item->GetOriginalID() = 0;
			Item->AddSticker(0, 4, 0, 1, 1);
			Item->SetStatTrak(150);
			Item->SetPaintKit(skin);
			Item->SetPaintSeed(0);
			Item->SetPaintWear(0);
			Item->SetOrigin(24);
			Item->SetRarity(ITEM_RARITY_MYTHICAL);
			Item->SetLevel(1);
			Item->SetInUse(true);
			Item->EquipAll();
			LocalInventory->AddEconItem(Item, 1, 0, 1);
	
/*
	*Item->GetAccountID() = LocalInventorya->GetSteamID();
	*Item->GetDefIndex() = WEAPON_AWP;
	*Item->GetItemID() = RandomInt(1, 50000);
	*Item->GetInventory() = 1;
	*Item->GetFlags() = 0;
	*Item->GetOriginalID() = 0;
	Item->AddSticker(0, 4, 0, 1, 1);
	Item->SetStatTrak(150);
	Item->SetPaintKit(38);
	Item->SetPaintSeed(0);
	Item->SetPaintWear(0);
	Item->SetOrigin(24);
	Item->SetRarity(ITEM_RARITY_MYTHICAL);
	Item->SetLevel(1);
	Item->SetInUse(false);
	LocalInventory->AddEconItem(Item, 1, 0, 1);*/

}






































































































































