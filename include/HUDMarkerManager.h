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

		void ProcessPlayerSetMarker(RE::TESObjectREFR* a_markerRef, std::uint32_t a_markerGotoFrame);

		std::shared_ptr<FocusedMarker> GetNextFocusedMarker();

		void SetMarkersExtraInfo();

		bool IsPlayerAllyOfFaction(const RE::TESFaction* a_faction);
		bool IsPlayerOpponentOfFaction(const RE::TESFaction* a_faction);

		static constexpr inline float potentiallyFocusedAngle = 10.0F;
		static constexpr inline float keepFocusedAngle = 35.0F;

		std::unordered_map<RE::TESObjectREFR*, std::shared_ptr<FocusedMarker>> potentiallyFocusedMarkers;
		std::shared_ptr<FocusedMarker> focusedMarker;
		std::shared_ptr<FocusedQuestMarker> focusedQuestMarker;
		std::shared_ptr<FocusedLocationMarker> focusedLocationMarker;

		RE::HUDMarkerManager* hudMarkerManager = RE::HUDMarkerManager::GetSingleton();

		Compass* compass = extended::Compass::GetSingleton();
		QuestItemList* questItemList = QuestItemList::GetSingleton();

		const RE::TESFaction* imperialLegionFaction = RE::TESForm::LookupByID(0x0002BF9A)->As<RE::TESFaction>();
		const RE::TESFaction* stormCloaksFaction = RE::TESForm::LookupByID(0x00028849)->As<RE::TESFaction>();
		const RE::TESFaction* sonsOfSkyrimFaction = RE::TESForm::LookupByID(0x0002BF9B)->As<RE::TESFaction>();
		const RE::TESFaction* dawnGuardFaction = RE::TESForm::LookupByID(0x02014217)->As<RE::TESFaction>();
		const RE::TESFaction* vampireFaction = RE::TESForm::LookupByID(0x02003376)->As<RE::TESFaction>();
	};
}