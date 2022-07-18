#pragma once

#include "utils/Angles.h"

struct FocusedMarker
{
	FocusedMarker(const RE::TESObjectREFR* a_ref, std::uint32_t a_gfxIndex, std::uint32_t a_gfxGotoFrame,
		float a_distanceToPlayer, float a_angleToPlayerCamera) :
		ref{ a_ref },
		gfxIndex{ a_gfxIndex }, gfxGotoFrame{ a_gfxGotoFrame },
		distanceToPlayer{ a_distanceToPlayer }, angleToPlayerCamera{ a_angleToPlayerCamera }
	{}

	virtual ~FocusedMarker() = default;

	const RE::TESObjectREFR* ref;
	std::uint32_t gfxIndex;
	std::uint32_t gfxGotoFrame;
	float distanceToPlayer;
	float angleToPlayerCamera;
};

struct FocusedQuestMarker : FocusedMarker
{
	FocusedQuestMarker(std::uint32_t a_index, std::uint32_t a_gotoFrame, RE::TESObjectREFR* a_markerRef, float a_angleToPlayerCamera,
		const RE::TESQuest* a_quest, const RE::BGSInstancedQuestObjective* a_instancedObjective);

	const RE::TESQuest* quest;
	const RE::BGSInstancedQuestObjective* instancedObjective;

	// cache
	RE::QUEST_DATA::Type questType = quest->GetType();
	std::string questName = (questType == RE::QUEST_DATA::Type::kMiscellaneous) ? "$MISCELLANEOUS" : quest->GetName();
	std::string questDescription = quest->GetCurrentDescriptionWithReplacedTags().c_str();
	std::string questObjective = instancedObjective->GetDisplayTextWithReplacedTags().c_str();
	std::string questLocation;
	std::string questCharacterName;
};

struct FocusedLocationMarker : FocusedMarker
{
	FocusedLocationMarker(std::uint32_t a_index, std::uint32_t a_gotoFrame, RE::TESObjectREFR* a_markerRef, float a_angleToPlayerCamera,
		const RE::MapMarkerData* a_data);

	const RE::MapMarkerData* data;

	// cache
	std::string locationName = data->locationName.GetFullName();
};