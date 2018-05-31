#include "InventoryChanger.h"
#include <vector>
#include "SteamID.h"
#include "SteamClient.h"
ISteamUser* g_pSteamUser = nullptr;
void AddItem(CMsgSOCacheSubscribed::SubscribedType* Object, int index, int itemDefIndex, int rarity, int paintKit, int seed, float wear, std::string name)
{
	CSOEconItem Skin;

	Skin.set_id(20000 + index);
	Skin.set_account_id(g_pSteamUser->GetSteamID().GetAccountID());
	Skin.set_def_index(itemDefIndex);
	Skin.set_inventory(20000 + index);
	Skin.set_origin(24);
	Skin.set_quantity(1);
	Skin.set_level(1);
	Skin.set_style(0);
	Skin.set_flags(0);
	Skin.set_in_use(false);
	Skin.set_original_id(0);
	Skin.set_rarity(rarity);
	Skin.set_quality(0);

	if (name.size() > 0)
		Skin.set_custom_name(name.data());

	// Paint Kit
	auto PaintKitAttribute = Skin.add_attribute();
	float PaintKitAttributeValue = (float)paintKit;

	PaintKitAttribute->set_def_index(6);

	PaintKitAttribute->set_value_bytes(&PaintKitAttributeValue, 4);

	// Paint Seed
	auto SeedAttribute = Skin.add_attribute();
	float SeedAttributeValue = (float)seed;

	SeedAttribute->set_def_index(7);

	SeedAttribute->set_value_bytes(&SeedAttributeValue, 4);

	// Paint Wear
	auto WearAttribute = Skin.add_attribute();
	float WearAttributeValue = wear;

	WearAttribute->set_def_index(8);

	WearAttribute->set_value_bytes(&WearAttributeValue, 4);

	// Stickers
	for (int j = 0; j < 4; j++)
	{
		// Sticker Kit
		CSOEconItemAttribute* StickerKitAttribute = Skin.add_attribute();
		uint32_t StickerKitAttributeValue = 0;

		StickerKitAttribute->set_def_index(113 + 4 * j);

		StickerKitAttribute->set_value_bytes(&StickerKitAttributeValue, 4);

		// Sticker Wear
		CSOEconItemAttribute* StickerWearAttribute = Skin.add_attribute();
		float StickerWearAttributeValue = 0.001f;

		StickerWearAttribute->set_def_index(114 + 4 * j);

		StickerWearAttribute->set_value_bytes(&StickerWearAttributeValue, 4);

		// Sticker Scale
		CSOEconItemAttribute* StickerScaleAttribute = Skin.add_attribute();
		float StickerScaleAttributeValue = 1.f;

		StickerScaleAttribute->set_def_index(115 + 4 * j);

		StickerScaleAttribute->set_value_bytes(&StickerScaleAttributeValue, 4);

		// Sticker Rotation
		CSOEconItemAttribute* StickerRotationAttribute = Skin.add_attribute();
		float StickerRotationAttributeValue = 0.f;

		StickerRotationAttribute->set_def_index(116 + 4 * j);

		StickerRotationAttribute->set_value_bytes(&StickerRotationAttributeValue, 4);
	}

	Object->add_object_data(Skin.SerializeAsString());
}

void CInventoryChanger::PostRetrieveMessage(uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize)
{
	if (!true)//Settings::InventoryChanger::enabled
		return;

	uint32_t MessageType = *punMsgType & 0x7FFFFFFF;

	if (MessageType != k_EMsgGCClientWelcome)
		return;
	CMsgClientWelcome Message;

	try
	{
		if (!Message.ParsePartialFromArray((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8))
			return;
	}
	catch (...)
	{
		return;
	}

	if (Message.outofdate_subscribed_caches_size() <= 0)
		return;

	CMsgSOCacheSubscribed* Cache = Message.mutable_outofdate_subscribed_caches(0);

	for (int i = 0; i < Cache->objects_size(); i++)
	{
		CMsgSOCacheSubscribed::SubscribedType* Object = Cache->mutable_objects(i);

		if (!Object->has_type_id())
			continue;

		if (Object->type_id() == 1) {
			for (int j = 0; j < Object->object_data_size(); j++)
			{
				std::string* ObjectData = Object->mutable_object_data(j);

				CSOEconItem Item;

				if (!Item.ParseFromArray((void*)const_cast<char*>(ObjectData->data()), ObjectData->size()))
					continue;

				if (Item.equipped_state_size() <= 0)
					continue;
			}

			AddItem(Object, 1, WEAPON_AK47, 6, 600, 0, 0.001f, "");
			AddItem(Object, 2, WEAPON_AK47, 6, 524, 0, 0.001f, "");
			AddItem(Object, 3, WEAPON_KNIFE_KARAMBIT, 6, 558, 38, 0.001f, "");
			AddItem(Object, 4, WEAPON_KNIFE_KARAMBIT, 6, 576, 38, 0.001f, "");
			AddItem(Object, 5, WEAPON_KNIFE_BAYONET, 6, 419, 38, 0.001f, "");
			AddItem(Object, 5, WEAPON_AWP, 6, 344, 38, 0.001f, "");
			AddItem(Object, 5, WEAPON_AWP, 6, 446, 38, 0.001f, "");
			AddItem(Object, 5, WEAPON_FAMAS, 6, 260, 38, 0.001f, "");

			ApplyMedals(Object);
		}
	}

	if ((uint32_t)Message.ByteSize() <= cubDest - 8)
	{
		Message.SerializeToArray((void*)((DWORD)pubDest + 8), Message.ByteSize());

		*pcubMsgSize = Message.ByteSize() + 8;
	}
}

bool CInventoryChanger::PreSendMessage(uint32_t &unMsgType, void* pubData, uint32_t &cubData)
{
	if (!true)//Settings::InventoryChanger::enabled
		return true;

	uint32_t MessageType = unMsgType & 0x7FFFFFFF;

	return true;
}

void CInventoryChanger::ApplyMedals(CMsgSOCacheSubscribed::SubscribedType* pInventoryCacheObject)
{
	CSOEconItem Medal;

	Medal.set_account_id(g_pSteamUser->GetSteamID().GetAccountID());
	Medal.set_origin(9);
	Medal.set_rarity(6);
	Medal.set_quantity(1);
	Medal.set_quality(4);
	Medal.set_level(1);

	CSOEconItemAttribute* TimeAcquiredAttribute = Medal.add_attribute();
	uint32_t TimeAcquiredAttributeValue = 0;

	TimeAcquiredAttribute->set_def_index(222);

	TimeAcquiredAttribute->set_value_bytes(&TimeAcquiredAttributeValue, 4);
	std::vector<uint32_t> medals = { 930,931,948,958,6021,6020,6019,6012,6032,6027,6002 };
	int i = 10000;
	for (uint32_t MedalIndex : medals)
	{
		Medal.set_def_index(MedalIndex);
		Medal.set_inventory(i);
		Medal.set_id(i);

		pInventoryCacheObject->add_object_data(Medal.SerializeAsString());

		i++;
	}
}















