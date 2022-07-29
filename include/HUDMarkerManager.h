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

		void ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_marker,
								std::uint32_t a_markerGotoFrame);

		void ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_marker,
								   std::uint32_t a_markerGotoFrame);

		void ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t a_markerGotoFrame);

		void ProcessPlayerSetMarker(RE::TESObjectREFR* a_marker, std::uint32_t a_markerGotoFrame);

		void SetMarkersExtraInfo();

	private:

		void RemoveFromPotentiallyFocusedIfOutOfAngle(RE::TESObjectREFR* a_marker, float a_angleToPlayerCamera,
													  bool a_isFocusedMarker);

		std::shared_ptr<FocusedMarker> GetNextFocusedMarker();

		float GetAngleBetween(const RE::PlayerCamera* a_playerCamera, const RE::TESObjectREFR* a_marker) const;

		bool IsPlayerAllyOfFaction(const RE::TESFaction* a_faction) const;

		bool IsPlayerOpponentOfFaction(const RE::TESFaction* a_faction) const;

		std::string GetSideInQuest(RE::QUEST_DATA::Type a_questType) const;

		static constexpr inline float potentiallyFocusedAngle = 10.0F;
		static constexpr inline float keepFocusedAngle = 35.0F;

		std::unordered_map<const RE::TESObjectREFR*, std::shared_ptr<FocusedMarker>> potentiallyFocusedMarkers;

		std::shared_ptr<FocusedMarker> focusedMarker;

		// cached
		RE::HUDMarkerManager* hudMarkerManager = RE::HUDMarkerManager::GetSingleton();

		// Factions to lookup
		// Reference: Creation Kit -> Skyrim.esm, Dawnguard.esm
		const RE::TESFaction* imperialLegionFaction = RE::TESForm::LookupByID(0x0002BF9A)->As<RE::TESFaction>();
		const RE::TESFaction* stormCloaksFaction = RE::TESForm::LookupByID(0x00028849)->As<RE::TESFaction>();
		const RE::TESFaction* sonsOfSkyrimFaction = RE::TESForm::LookupByID(0x0002BF9B)->As<RE::TESFaction>();
		const RE::TESFaction* dawnGuardFaction = RE::TESForm::LookupByID(0x02014217)->As<RE::TESFaction>();
		const RE::TESFaction* vampireFaction = RE::TESForm::LookupByID(0x02003376)->As<RE::TESFaction>();
	};
}