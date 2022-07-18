#pragma once

#include "RE/H/HUDMarkerManager.h"

#include "FocusedMarker.h"
#include "Compass.h"
#include "QuestItemList.h"
#include "Test.h"

namespace extended
{
	class HUDMarkerManager
	{
	public:

		static HUDMarkerManager* GetSingleton()
		{
			static HUDMarkerManager singleton;

			return &singleton;
		}

		void ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef,
								std::uint32_t a_markerGotoFrame);

		void ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef,
								   std::uint32_t a_markerGotoFrame);

		void ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t a_markerGotoFrame);

		std::shared_ptr<FocusedMarker> GetNextFocusedMarker();

		void SetMarkersExtraInfo();

		static constexpr inline float potentiallyFocusedAngle = 10.0F;
		static constexpr inline float keepFocusedAngle = 20.0F;

		std::unordered_map<RE::TESObjectREFR*, std::shared_ptr<FocusedMarker>> potentiallyFocusedMarkers;
		std::shared_ptr<FocusedMarker> focusedMarker;

		RE::HUDMarkerManager* hudMarkerManager = RE::HUDMarkerManager::GetSingleton();

		Compass* compass = extended::Compass::GetSingleton();
		QuestItemList* questItemList = QuestItemList::GetSingleton();
	};
}