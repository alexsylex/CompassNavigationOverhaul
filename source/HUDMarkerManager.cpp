#include "HUDMarkerManager.h"

#include "RE/B/BSTimer.h"

#include "utils/Logger.h"

namespace extended
{
	void HUDMarkerManager::ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_marker,
											  std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_marker);

		bool isFocusedMarker = focusedMarker && a_marker == focusedMarker->ref;

		if (isFocusedMarker || angleToPlayerCamera < potentiallyFocusedAngle)
		{
			std::shared_ptr<FocusedMarker> potentiallyFocusedMarker;
			if (potentiallyFocusedMarkers.contains(a_marker))
			{
				potentiallyFocusedMarker = potentiallyFocusedMarkers.at(a_marker);
			}
			else
			{
				potentiallyFocusedMarker = std::make_shared<FocusedMarker>(a_marker, angleToPlayerCamera);

				potentiallyFocusedMarkers.emplace(a_marker, potentiallyFocusedMarker);
			}

			auto dataIt = std::ranges::find_if(potentiallyFocusedMarker->data,
				[a_quest](std::shared_ptr<FocusedMarker::Data> a_data) -> bool
				{
					auto questData = std::static_pointer_cast<FocusedMarker::QuestData>(a_data);

					// Group miscellaneous quests
					if (questData->type == RE::QUEST_DATA::Type::kMiscellaneous)
					{
						return a_quest->GetType() == RE::QUEST_DATA::Type::kMiscellaneous;
					}
					else
					{
						return questData->quest == a_quest;
					}
				});

			std::shared_ptr<FocusedMarker::QuestData> questData;
			if (dataIt != potentiallyFocusedMarker->data.end())
			{
				questData = std::static_pointer_cast<FocusedMarker::QuestData>(*dataIt);
			}
			else
			{
				questData = std::make_shared<FocusedMarker::QuestData>
				(
					hudMarkerManager->currentMarkerIndex - 1,
					a_markerGotoFrame,
					a_marker,
					a_quest
				);

				potentiallyFocusedMarker->data.push_back(questData);
			}

			for (RE::BGSInstancedQuestObjective& questObjective : RE::PlayerCharacter::GetSingleton()->objectives) 
			{
				if (questObjective.objective->ownerQuest == a_quest &&
					questObjective.instanceState == RE::QUEST_OBJECTIVE_STATE::kDisplayed)
				{
					questData->objectives.push_back(questObjective.GetDisplayTextWithReplacedTags().c_str());
				}
			}
		}
		
		RemoveFromPotentiallyFocusedIfOutOfAngle(a_marker, angleToPlayerCamera, isFocusedMarker);
	}

	void HUDMarkerManager::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_marker,
												 std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_marker);

		bool isFocusedMarker = focusedMarker && a_marker == focusedMarker->ref;

		if (isFocusedMarker || angleToPlayerCamera < potentiallyFocusedAngle)
		{
			auto potentiallyFocusedMarker = std::make_shared<FocusedMarker>(a_marker, angleToPlayerCamera);

			potentiallyFocusedMarkers.emplace(a_marker, potentiallyFocusedMarker);

			auto locationData = std::make_shared<FocusedMarker::LocationData>
			(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame,
				a_mapMarker->mapData
			);

			potentiallyFocusedMarkers.at(a_marker)->data.push_back(locationData);
		} 
		
		RemoveFromPotentiallyFocusedIfOutOfAngle(a_marker, angleToPlayerCamera, isFocusedMarker);
	}

	void HUDMarkerManager::ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_enemy);

		bool isFocusedMarker = focusedMarker && a_enemy == focusedMarker->ref;

		if (isFocusedMarker || angleToPlayerCamera < potentiallyFocusedAngle)
		{
			auto potentiallyFocusedMarker = std::make_shared<FocusedMarker>(a_enemy, angleToPlayerCamera);

			potentiallyFocusedMarkers.emplace(a_enemy, potentiallyFocusedMarker);

			auto enemyData = std::make_shared<FocusedMarker::EnemyData>
			(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame,
				a_enemy
			);

			potentiallyFocusedMarkers.at(a_enemy)->data.push_back(enemyData);
		}

		RemoveFromPotentiallyFocusedIfOutOfAngle(a_enemy, angleToPlayerCamera, isFocusedMarker);
	}

	void HUDMarkerManager::ProcessPlayerSetMarker(RE::TESObjectREFR* a_marker, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_marker);

		bool isFocusedMarker = focusedMarker && a_marker == focusedMarker->ref;

		if (isFocusedMarker || angleToPlayerCamera < potentiallyFocusedAngle)
		{
			auto potentiallyFocusedMarker = std::make_shared<FocusedMarker>(a_marker, angleToPlayerCamera);

			potentiallyFocusedMarkers.emplace(a_marker, potentiallyFocusedMarker);

			auto playerSetData = std::make_shared<FocusedMarker::PlayerSetData>
			(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame
			);

			potentiallyFocusedMarkers.at(a_marker)->data.push_back(playerSetData);
		}

		RemoveFromPotentiallyFocusedIfOutOfAngle(a_marker, angleToPlayerCamera, isFocusedMarker);
	}

	void HUDMarkerManager::SetMarkersExtraInfo()
	{
		// May be used for timed logic
		//float timeSinceLastFrame = RE::BSTimer::GetTimeManager()->realTimeDelta;

		Compass* compass = Compass::GetSingleton();
		QuestItemList* questItemList = QuestItemList::GetSingleton();

		std::shared_ptr<FocusedMarker> nextFocusedMarker = GetNextFocusedMarker();

		bool focusChanged = ((focusedMarker && nextFocusedMarker && (focusedMarker->ref != nextFocusedMarker->ref)) ||
							 (focusedMarker && !nextFocusedMarker) || (!focusedMarker && nextFocusedMarker));

		if (focusedMarker && focusChanged)
		{
			compass->UnfocusMarker(focusedMarker->data.back()->gfxIndex);
			questItemList->RemoveAllQuests();
		}

		focusedMarker = nextFocusedMarker;

		if (focusedMarker && !focusedMarker->data.empty())
		{
			bool canFocusPlayerSetMarker = true;

			for (std::shared_ptr<FocusedMarker::Data> focusedMarkerData : focusedMarker->data)
			{
				if (auto questData = std::dynamic_pointer_cast<FocusedMarker::QuestData>(focusedMarkerData))
				{
					canFocusPlayerSetMarker = false;

					// TODO: Give the user an option to chose between the objective and the location/character name
					// questData->location.empty() ? questData->characterName : questData->location;

					compass->SetMarkerInfo(questData->objectives.back(), focusedMarker->distanceToPlayer, focusedMarker->heightDifference);

					if (focusChanged)
					{
						questItemList->AddQuest(questData->type, questData->name, questData->objectives);

						questItemList->SetQuestSide(GetSideInQuest(questData->type));

						questItemList->ShowQuest();
					}
				}
				else if (auto locationData = std::dynamic_pointer_cast<FocusedMarker::LocationData>(focusedMarkerData))
				{
					compass->SetMarkerInfo(locationData->locationName, focusedMarker->distanceToPlayer, focusedMarker->heightDifference);
				}
				else if (auto enemyData = std::dynamic_pointer_cast<FocusedMarker::EnemyData>(focusedMarkerData))
				{
					compass->SetMarkerInfo(enemyData->enemyName, focusedMarker->distanceToPlayer, focusedMarker->heightDifference);
				}
				else if (auto playerSetData = std::dynamic_pointer_cast<FocusedMarker::PlayerSetData>(focusedMarkerData))
				{
					if (canFocusPlayerSetMarker)
					{
						compass->SetMarkerInfo(playerSetData->locationName, focusedMarker->distanceToPlayer, focusedMarker->heightDifference);
					}
				}

				if (focusChanged)
				{
					compass->FocusMarker(focusedMarkerData->gfxIndex);
				} 
				else
				{
					compass->UpdateMarker(focusedMarkerData->gfxIndex);
				}
			}
		}

		compass->SetMarkersSize();

		potentiallyFocusedMarkers.clear();
	}

	void HUDMarkerManager::RemoveFromPotentiallyFocusedIfOutOfAngle(RE::TESObjectREFR* a_marker, float a_angleToPlayerCamera, bool a_isFocusedMarker)
	{
		if (potentiallyFocusedMarkers.contains(a_marker))
		{
			if (a_isFocusedMarker)
			{
				if (a_angleToPlayerCamera > keepFocusedAngle)
				{
					potentiallyFocusedMarkers.erase(a_marker);
				}
			}
			else if (a_angleToPlayerCamera > potentiallyFocusedAngle)
			{
				potentiallyFocusedMarkers.erase(a_marker);
			}
		}
	}

	std::shared_ptr<FocusedMarker> HUDMarkerManager::GetNextFocusedMarker()
	{
		float closestAngleToPlayerCamera = std::numeric_limits<float>::max();

		std::shared_ptr<FocusedMarker> bestFocusedMarker = nullptr;

		for (const auto& [markerRef, potentiallyFocusedMarker] : potentiallyFocusedMarkers) 
		{
			if (potentiallyFocusedMarker->angleToPlayerCamera < closestAngleToPlayerCamera)
			{
				bestFocusedMarker = potentiallyFocusedMarker;
				closestAngleToPlayerCamera = potentiallyFocusedMarker->angleToPlayerCamera;
			}
		}

		return bestFocusedMarker;
	}

	float HUDMarkerManager::GetAngleBetween(const RE::PlayerCamera* a_playerCamera, const RE::TESObjectREFR* a_marker) const
	{
		float angleToPlayerCameraInRadians = util::GetAngleBetween(a_playerCamera, a_marker);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraInRadians);

		if (angleToPlayerCamera > 180.0F)
			angleToPlayerCamera = 360.0F - angleToPlayerCamera;

		return angleToPlayerCamera;
	}

	bool HUDMarkerManager::IsPlayerAllyOfFaction(const RE::TESFaction* a_faction) const
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		if (player->IsInFaction(a_faction)) 
		{
			return true;
		}

		return player->VisitFactions([a_faction](RE::TESFaction* a_visitedFaction, std::int8_t a_rank) -> bool
		{
			if (a_visitedFaction == a_faction && a_rank > 0)
			{
				return true;
			}

			for (RE::GROUP_REACTION* reactionToFaction : a_visitedFaction->reactions)
			{
				auto relatedFaction = reactionToFaction->form->As<RE::TESFaction>();
				if (relatedFaction == a_faction && reactionToFaction->fightReaction >= RE::FIGHT_REACTION::kAlly)
				{
					return true;
				}
			}

			return false;
		});
	}

	bool HUDMarkerManager::IsPlayerOpponentOfFaction(const RE::TESFaction* a_faction) const
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		return player->VisitFactions([a_faction](RE::TESFaction* a_visitedFaction, std::int8_t a_rank) -> bool
		{
			if (a_visitedFaction == a_faction && a_rank < 0)
			{
				return true;
			}

			for (RE::GROUP_REACTION* reactionToFaction : a_visitedFaction->reactions)
			{
				auto relatedFaction = reactionToFaction->form->As<RE::TESFaction>();
				if (relatedFaction == a_faction && reactionToFaction->fightReaction == RE::FIGHT_REACTION::kEnemy)
				{
					return true;
				}
			}

			return false;
		});
	}

	std::string HUDMarkerManager::GetSideInQuest(RE::QUEST_DATA::Type a_questType) const
	{
		switch (a_questType)
		{
		case RE::QUEST_DATA::Type::kCivilWar:
			if (IsPlayerAllyOfFaction(sonsOfSkyrimFaction) || 
				IsPlayerAllyOfFaction(stormCloaksFaction) ||
				IsPlayerOpponentOfFaction(imperialLegionFaction))
			{
				return "StormCloaks";
			}
			else
			{
				return "ImperialLegion";
			}
		case RE::QUEST_DATA::Type::kDLC01_Vampire:
			if (RE::PlayerCharacter::GetSingleton()->HasKeywordString("Vampire") || 
				IsPlayerAllyOfFaction(vampireFaction) ||
				IsPlayerOpponentOfFaction(dawnGuardFaction))
			{
				return "Vampires";
			}
			else
			{
				return "Dawnguard";
			}
		}

		return { };
	}
}
