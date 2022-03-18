#include "Hooks.h"

#include "Logger.h"

namespace HCN
{
	bool ProcessQuestHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId, RE::TESQuest*& a_quest)
	{
		RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

		if (markerRef->GetFormType() == RE::FormType::Reference)
		{
			logger::info("\"{}\" quest hooked", a_quest->GetName());

			//auto questTeleporter = markerRef.get();

			//logger::info("\"{}\" quest teleporter hooked", questTeleporter->GetName());
		}
		else
		{
			logger::info("Unknown quest marker type hooked, {}/{}", (int)markerRef->GetFormType(), (int)RE::FormType::Reference);
		}
		logger::flush();

		return HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId);
	}

	bool ProcessLocationHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

		if (markerRef && markerRef->GetFormType() == RE::FormType::Reference) 
		{
			if (RE::BSExtraData* extraData = markerRef->extraList.GetByType(RE::ExtraDataType::kMapMarker))
			{
				auto mapMarker = skyrim_cast<RE::ExtraMapMarker*>(extraData);

				logger::info("\"{}\" location hooked", mapMarker->mapData->locationName.GetFullName());
				logger::flush();
			}
		}

		return HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId);
	}

	bool ProcessEnemyHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

		if (markerRef && markerRef->GetFormType() == RE::FormType::ActorCharacter)
		{
			if (auto character = markerRef->As<RE::Character>()) 
			{
				logger::info("\"{}\" enemy hooked", character->GetName());
				logger::flush();
			}
		}

		return HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId);
	}

	bool ProcessPlayerSetHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

		logger::info("Player-set marker hooked, {}/?", (int)markerRef->GetFormType());
		logger::flush();

		return HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId);
	}
}
