#include "Hooks.h"

#include "utils/Logger.h"

#include "IUI/API.h"

#include "CompassShoutMeterHolder.h"

namespace HCN
{
	bool ProcessQuestHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId, RE::TESQuest*& a_quest)
	{
		if (HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId)) 
		{
			if (CompassShoutMeterHolder::GetSingleton()) 
			{
				RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

				CompassShoutMeterHolder::GetSingleton()->ProcessQuestMarker(a_quest, markerRef.get());
			}

			return true;
		}

		return false;
	}

	bool ProcessLocationHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		if (HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId)) 
		{
			if (CompassShoutMeterHolder::GetSingleton()) 
			{
				RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);
				for (auto locationRef : a_hudMarkerManager->locationRefs) 
				{
					RE::TESObjectREFRPtr locationRefOut = RE::TESObjectREFR::LookupByHandle(locationRef);
					if (locationRefOut == markerRef)
					{
						logger::info("Marker found");
					}
				}

				if (markerRef) 
				{
					CompassShoutMeterHolder::GetSingleton()->ProcessLocationMarker(markerRef->extraList.GetByType<RE::ExtraMapMarker>(), markerRef.get());
				}
			}

			return true;
		}

		return false;
	}

	bool ProcessEnemyHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		if (HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId)) 
		{
			if (CompassShoutMeterHolder::GetSingleton()) 
			{
				RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

				if (markerRef)
				{
					CompassShoutMeterHolder::GetSingleton()->ProcessEnemyMarker(markerRef->As<RE::Character>());
				}
			}

			return true;
		}

		return false;
	}

	bool ProcessPlayerSetHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		if (HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId)) 
		{
			RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

			logger::trace("Player-set marker hooked, {}/?", (int)markerRef->GetFormType());
			logger::flush();

			return true;
		}

		return false;
	}
}
