#pragma once

#include "RE/H/HUDMarkerManager.h"

#include "Settings.h"

#include "FocusedMarker.h"
#include "Compass.h"
#include "QuestItemList.h"

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
		std::shared_ptr<FocusedMarker> GetUpdatedPotentiallyFocusedMarker(const RE::TESObjectREFR* a_marker, float a_angleToPlayerCamera)
		{
			std::shared_ptr<FocusedMarker> potentiallyFocusedMarker;

			if (potentiallyFocusedMarkers.contains(a_marker))
			{
				potentiallyFocusedMarker = potentiallyFocusedMarkers.at(a_marker);
				potentiallyFocusedMarker->UpdateGeometry(a_angleToPlayerCamera);
			}
			else
			{
				potentiallyFocusedMarker = std::make_shared<FocusedMarker>(a_marker, a_angleToPlayerCamera);
				potentiallyFocusedMarkers.emplace(a_marker, potentiallyFocusedMarker);
			}

			return potentiallyFocusedMarker;
		}

		bool IsFocusedMarker(const RE::TESObjectREFR* a_marker) const { return focusedMarker && a_marker == focusedMarker->ref; }

		void RemovePotentiallyFocusedMarkersOutOfRange();

		std::shared_ptr<FocusedMarker> GetNextFocusedMarker();

		bool DidFocusChange(std::shared_ptr<FocusedMarker> a_nextFocusedMarker) const;

		float GetAngleBetween(const RE::PlayerCamera* a_playerCamera, const RE::TESObjectREFR* a_marker) const;

		bool IsPlayerAllyOfFaction(const RE::TESFaction* a_faction) const;

		bool IsPlayerOpponentOfFaction(const RE::TESFaction* a_faction) const;

		std::string GetSideInQuest(RE::QUEST_DATA::Type a_questType) const;

		float potentiallyFocusedAngle = settings::display::angleToShowMarkerDetails;
		float keepFocusedAngle = settings::display::angleToKeepMarkerDetailsShown;

		std::unordered_map<const RE::TESObjectREFR*, std::shared_ptr<FocusedMarker>> potentiallyFocusedMarkers;

		std::shared_ptr<FocusedMarker> focusedMarker;
		float timeFocusingMarker = 0.0F;

		// cached
		RE::HUDMarkerManager* const hudMarkerManager = RE::HUDMarkerManager::GetSingleton();

		// Factions to lookup
		// Reference: Creation Kit -> Skyrim.esm, Dawnguard.esm
		const RE::TESFaction* const imperialLegionFaction = RE::TESForm::LookupByID(0x0002BF9A)->As<RE::TESFaction>();
		const RE::TESFaction* const stormCloaksFaction = RE::TESForm::LookupByID(0x00028849)->As<RE::TESFaction>();
		const RE::TESFaction* const sonsOfSkyrimFaction = RE::TESForm::LookupByID(0x0002BF9B)->As<RE::TESFaction>();
		const RE::TESFaction* const dawnGuardFaction = RE::TESForm::LookupByID(0x02014217)->As<RE::TESFaction>();
		const RE::TESFaction* const vampireFaction = RE::TESForm::LookupByID(0x02003376)->As<RE::TESFaction>();
	};
}