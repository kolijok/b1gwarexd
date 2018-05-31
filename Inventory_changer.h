#pragma once
#include "CEconItem.h"

/*auto LocalInventory = **reinterpret_cast<CPlayerInventory***>(Utilities::Memory::FindPatternIDA("client.dll", "8B 3D ? ? ? ? 85 FF 74 1A") + 0x2);
auto g_CSInventoryManager = *reinterpret_cast<CSInventoryManager**>(Utilities::Memory::FindPatternIDA("client.dll", "B9 ?? ?? ?? ?? 8D 44 24 10 89 54 24 14") + 0x1);
auto LocalInventory = g_CSInventoryManager->GetLocalPlayerInventory();*/
class C_EconItemView
{
public:
	char pad_0x0000[0x194]; //0x0000
	__int32 m_iItemDefinitionIndex; //0x0194 
	__int32 m_iEntityQuality; //0x0198 
	__int32 m_iEntityLevel; //0x019C 
	char pad_0x01A0[0x8]; //0x01A0
	__int32 m_iItemIDHigh; //0x01A8 
	__int32 m_iItemIDLow; //0x01AC 
	__int32 m_iAccountID; //0x01B0 
	char pad_0x01B4[0x8]; //0x01B4
	unsigned char m_bInitialized; //0x01BC 
	char pad_0x01BD[0x67]; //0x01BD
	char m_szCustomName[32]; //0x0224 

};//Size=0x0244
class CSharedObjectTypeCache {
public:
	void CSharedObjectTypeCache::AddObject(void* obj)
	{
		typedef void(__thiscall* tOriginal)(void*, void*);
		CallVFunction<tOriginal>(this, 1)(this, obj);
	}

	void CSharedObjectTypeCache::RemoveObject(void* obj)
	{
		typedef void(__thiscall* tOriginal)(void*, void*);
		CallVFunction<tOriginal>(this, 3)(this, obj);
	}

	std::vector<CEconItem*> CSharedObjectTypeCache::GetEconItems()
	{
		std::vector<CEconItem*> ret;

		auto size = *reinterpret_cast<size_t*>(this + 0x18);

		auto data = *reinterpret_cast<uintptr_t**>(this + 0x4);

		for (size_t i = 0; i < size; i++)
			ret.push_back(reinterpret_cast<CEconItem*>(data[i]));

		return ret;
	}
};

/*class CSInventoryManagera
{
public:
	
	
};
*/



class CPlayerInventory
{
public:

	void CPlayerInventory::RemoveItem(uint64_t ID)
	{
		static auto fnRemoveItem
			= reinterpret_cast<int(__thiscall*)(void*, int64_t)>(
				Utilities::Memory::FindPatternIDA("client.dll", "55 8B EC 83 E4 F8 56 57 FF 75 0C 8B F1")
				);

		fnRemoveItem(this, ID);
	}
	CPlayerInventory* CPlayerInventory::GetLocalPlayerInventoary() {
		static auto local_inventory_offset = *reinterpret_cast<uintptr_t*>(Utilities::Memory::FindPatternIDA("client.dll", "8B 8B ? ? ? ? E8 ? ? ? ? 89 44 24 18") + 0x2);
		return *reinterpret_cast<CPlayerInventory**>(this + local_inventory_offset);
	}
	void CPlayerInventory::RemoveItem(CEconItem* item)
	{
		RemoveItem(*item->GetItemID());
		GetBaseTypeCache()->RemoveObject(item);
	}

	void CPlayerInventory::ClearInventory()
	{
		auto BaseTypeCache = this->GetBaseTypeCache();
		auto items = BaseTypeCache->GetEconItems();
		for (auto item : items)
		{
			RemoveItem(*item->GetItemID());
			BaseTypeCache->RemoveObject(item);
		}
	}

	CSharedObjectTypeCache* CPlayerInventory::GetBaseTypeCache()
	{
		static auto fnGCSDK_CGCClient_FindSOCache
			= reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, uint64_t, uint64_t, bool)>(
				Utilities::Memory::FindPatternIDA("client.dll", "55 8B EC 83 E4 F8 83 EC 1C 0F 10 45 08")
				);

		static auto fnGCSDK_CSharedObjectCache_FindBaseTypeCache
			= reinterpret_cast<CSharedObjectTypeCache**(__fastcall*)(uintptr_t, uintptr_t, int*)>(
				Utilities::Memory::FindPatternIDA("client.dll", "55 8B EC 56 57 8B F2 8B F9")
				);
		auto g_GCClientSystem = **reinterpret_cast<uintptr_t**>(Utilities::Memory::FindPatternIDA("client.dll", "8B 0D ? ? ? ? 6A 00 83 EC 10") + 0x2);
		auto SOCahce = fnGCSDK_CGCClient_FindSOCache(g_GCClientSystem + 0x60, *reinterpret_cast<uint64_t*>(this + 0x8), *reinterpret_cast<uint64_t*>(this + 0x10), 0);

		auto unk1 = *(DWORD*)(SOCahce + 0x1C);
		auto unk2 = *(DWORD*)(SOCahce + 0x10);
		auto type = 1;

		__asm push this;
		auto ret = fnGCSDK_CSharedObjectCache_FindBaseTypeCache(unk2, unk2 + 4 * unk1, &type);
		__asm add esp, 8;
		return *ret;
	}

	uint32_t CPlayerInventory::GetSteamID()
	{
		return *reinterpret_cast<uint32_t*>(this + 0x8);
	}

	CUtlVector< C_EconItemView* >* CPlayerInventory::GetInventoryItems()
	{
		return reinterpret_cast<CUtlVector<C_EconItemView*>*>(this + 0x2C);
	}

	bool CPlayerInventory::AddEconItem(CEconItem* item, int a3, int a4, char a5)
	{
		static auto fnAddEconItem
			= reinterpret_cast<CEconItem*(__thiscall*)(void*, CEconItem*, int, int, char)>(
				Utilities::Memory::FindPatternIDA("client.dll", "55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 14 53 56 57 8B F9 8B 08")
				);

		GetBaseTypeCache()->AddObject(item);

		return fnAddEconItem(this, item, a3, a4, a5);
	}


};
class iv
{
public:
	void move(int ob, int skin);

};

extern iv g_inve;