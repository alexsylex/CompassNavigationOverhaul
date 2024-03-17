#include "Hooks.h"

#include "Settings.h"

#include "HUDMarkerManager.h"

RE::BSTArray<RE::BGSInstancedQuestObjective>& GetPlayerObjectives()
{
	auto playerAddress = reinterpret_cast<std::uintptr_t>(RE::PlayerCharacter::GetSingleton());

	std::size_t objectivesOffset;
	if (REL::Module::IsVR())
	{
		objectivesOffset = 0xB70;
	} else {
		objectivesOffset = REL::Module::get().version().compare(SKSE::RUNTIME_SSE_1_6_629) == std::strong_ordering::less ? 0x580 : 0x588;
	}

	return *reinterpret_cast<RE::BSTArray<RE::BGSInstancedQuestObjective>*>(playerAddress + objectivesOffset);
}

namespace hooks
{
	bool UpdateQuests(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
					  RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame,
					  RE::TESQuestTarget* a_questTarget)
	{
		// The game loops through active quest targets, and calls `AddMarker` for each one.
		// If multiple targets correspond to the same marker, `AddMarker` returns the 
		// previously-created marker (via `a_refHandle`), so we can iteratively
		// build the structure containing all the targets, objectives, etc. corresponding
		// to the marker.
		if (HUDMarkerManager::AddMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerGotoFrame))
		{
			RE::TESObjectREFR* marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();

			RE::BSTArray<RE::BGSInstancedQuestObjective>& playerObjectives = GetPlayerObjectives();

			RE::TESQuest* quest = nullptr;
			RE::BGSInstancedQuestObjective objective;
			RE::TESQuestTarget* target = nullptr;
			for (int i = 0; i < playerObjectives.size(); i++)
			{
				RE::BGSQuestObjective* questObjective = playerObjectives[i].objective;

				for (int j = 0; j < questObjective->numTargets; j++)
				{
					auto questObjectiveTarget = reinterpret_cast<RE::TESQuestTarget*>(a_questTarget->unk00);

					if (questObjectiveTarget == questObjective->targets[j])
					{
						quest = questObjective->ownerQuest;
						objective = playerObjectives[i];
						target = questObjectiveTarget;
						break;
					}
				}
			}

			extended::HUDMarkerManager::GetSingleton()->ProcessQuestMarker(quest, objective, target, marker, a_markerGotoFrame);

			return true;
		}

		return false;
	}

	RE::TESWorldSpace* AllowedToShowMapMarker(const RE::TESObjectREFR* a_marker)
	{
		RE::TESWorldSpace* markerWorldspace = a_marker->GetWorldspace();

		if (settings::display::showInteriorMarkers)
		{
			auto player = RE::PlayerCharacter::GetSingleton();

			RE::TESWorldSpace* playerWorldspace = player->GetWorldspace();

			if (playerWorldspace && markerWorldspace && playerWorldspace != markerWorldspace)
			{
				if (!playerWorldspace->parentWorld && markerWorldspace->parentWorld == playerWorldspace) 
				{
					return playerWorldspace;
				}
			}
		}
			
		return markerWorldspace;
	}

	bool UpdateLocations(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
							   RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame)
	{
		RE::TESObjectREFR* marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		RE::NiPoint3 markerPos = util::GetRealPosition(marker);
		RE::NiPoint3 playerPos = util::GetRealPosition(player);

		float sqDistanceToMarker = playerPos.GetSquaredDistance(markerPos);

		if (sqDistanceToMarker < RE::HUDMarkerManager::GetSingleton()->sqRadiusToAddLocation)
		{
			auto mapMarker = marker->extraList.GetByType<RE::ExtraMapMarker>();

			auto frameOffsets = RE::HUDMarker::FrameOffsets::GetSingleton();

			// Unvisited markers keep being shown in any case
			if (settings::display::showUndiscoveredLocationMarkers || mapMarker->mapData->flags.all(RE::MapMarkerData::Flag::kVisible))
			{
				if (HUDMarkerManager::AddMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerGotoFrame)) 
				{
					extended::HUDMarkerManager::GetSingleton()->ProcessLocationMarker(mapMarker, marker, a_markerGotoFrame);

					return true;
				}
			}
		}

		return false;
	}

	bool UpdateEnemies(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
							RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame)
	{
		if (settings::display::showEnemyMarkers)
		{
			if (HUDMarkerManager::AddMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerGotoFrame)) 
			{
				RE::TESObjectREFR* marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();

				extended::HUDMarkerManager::GetSingleton()->ProcessEnemyMarker(marker->As<RE::Character>(), a_markerGotoFrame);

				return true;
			}
		}

		return false;
	}

	bool UpdatePlayerSetMarker(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
								RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame)
	{
		if (HUDMarkerManager::AddMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerGotoFrame)) 
		{
			RE::TESObjectREFR* marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();

			extended::HUDMarkerManager::GetSingleton()->ProcessPlayerSetMarker(marker, a_markerGotoFrame);

			return true;
		}

		return false;
	}

	bool SetCompassMarkers(RE::GFxValue::ObjectInterface* a_objectInterface, void* a_data,
						   RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args,
						   std::uint32_t a_numArgs, bool a_isDObj)
	{
		extended::HUDMarkerManager::GetSingleton()->SetMarkers();

		return true;
	}

	namespace compat
	{
		RE::GFxMovieDef* MapMarkerFramework::GetCompassMovieDef()
		{
			return compassMovieDef;
		}
	}
}
