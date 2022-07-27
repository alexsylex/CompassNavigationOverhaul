#include "HUDMarkerManager.h"

#include "RE/B/BSTimer.h"

#include "utils/Logger.h"

namespace extended
{
	void HUDMarkerManager::ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef,
											  std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCameraRads = util::GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_markerRef);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraRads);

		if (angleToPlayerCamera > 180.0F) angleToPlayerCamera = 360.0F - angleToPlayerCamera; 

		bool isFocusedMarker = focusedMarker && a_markerRef == focusedMarker->ref;

		if (isFocusedMarker || angleToPlayerCamera < potentiallyFocusedAngle)
		{
			for (RE::BGSInstancedQuestObjective& questObjective : RE::PlayerCharacter::GetSingleton()->objectives) 
			{
				if (questObjective.objective->ownerQuest == a_quest &&
					questObjective.instanceState == RE::QUEST_OBJECTIVE_STATE::kDisplayed)
				{
					auto potentiallyFocusedQuestMarker = std::make_shared<FocusedQuestMarker>
					(
						hudMarkerManager->currentMarkerIndex - 1,
						a_markerGotoFrame,
						a_markerRef,
						angleToPlayerCamera,
						a_quest,
						&questObjective
					);

					potentiallyFocusedMarkers.insert_or_assign(a_markerRef, std::static_pointer_cast<FocusedMarker>(potentiallyFocusedQuestMarker));

					//break;
				}
			}
		}
		
		if (potentiallyFocusedMarkers.contains(a_markerRef))
		{
			if (isFocusedMarker)
			{
				if (angleToPlayerCamera > keepFocusedAngle) 
				{
					potentiallyFocusedMarkers.erase(a_markerRef);
				}
			}
			else if (angleToPlayerCamera > potentiallyFocusedAngle)
			{
				potentiallyFocusedMarkers.erase(a_markerRef);
			}
		}
	}

	void HUDMarkerManager::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef,
												 std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCameraRads = util::GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_markerRef);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraRads);

		if (angleToPlayerCamera > 180.0F) angleToPlayerCamera = 360.0F - angleToPlayerCamera;

		bool isFocusedMarker = focusedMarker && a_markerRef == focusedMarker->ref;

		if (isFocusedMarker || angleToPlayerCamera < potentiallyFocusedAngle)
		{
			auto potentiallyocusedLocationMarker = std::make_shared<FocusedLocationMarker>
			(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame,
				a_markerRef,
				angleToPlayerCamera,
				a_mapMarker->mapData
			);

			potentiallyFocusedMarkers.insert_or_assign(a_markerRef, std::static_pointer_cast<FocusedMarker>(potentiallyocusedLocationMarker));
		} 
		
		if (potentiallyFocusedMarkers.contains(a_markerRef))
		{
			if (isFocusedMarker)
			{
				if (angleToPlayerCamera > keepFocusedAngle)
				{
					potentiallyFocusedMarkers.erase(a_markerRef);
				}
			}
			else if (angleToPlayerCamera > potentiallyFocusedAngle)
			{
				potentiallyFocusedMarkers.erase(a_markerRef);
			}
		}
	}

	void HUDMarkerManager::ProcessEnemyMarker(RE::Character* a_enemy, [[maybe_unused]] std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCameraRads = util::GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_enemy);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraRads);

		if (angleToPlayerCamera < potentiallyFocusedAngle)
		{
			//if (!focusedMarker || focusedMarker->questType == RE::QUEST_DATA::Type::kNone) 
			//{
			//	focusedMarker = FocusedMarker{ .questName = a_enemy->GetName(), .relativeAngle = relativeAngle };
			//}
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

	void HUDMarkerManager::SetMarkersExtraInfo()
	{
		//auto test = Test::GetSingleton();

		//float timeSinceLastFrame = RE::BSTimer::GetTimeManager()->realTimeDelta;

		//test->textField0.SetText((std::to_string(timeSinceLastFrame) + " spf").c_str());

		//test->textField0.SetText(std::string(std::string("Potentially focused markers: ") + std::to_string(potentiallyFocusedMarkers.size())).c_str());

		std::shared_ptr<FocusedMarker> nextFocusedMarker = GetNextFocusedMarker();

		bool focusChanged = ((focusedMarker && nextFocusedMarker && (focusedMarker->ref != nextFocusedMarker->ref)) ||
							 (focusedMarker && !nextFocusedMarker) || (!focusedMarker && nextFocusedMarker));

		if (focusedMarker && focusChanged)
		{
			compass->UnfocusMarker(focusedMarker->gfxIndex);

			if (focusedQuestMarker) 
			{
				questItemList->RemoveQuest();
			}
		}

		focusedMarker = nextFocusedMarker;

		if (focusedMarker)
		{
			focusedQuestMarker = std::dynamic_pointer_cast<FocusedQuestMarker>(focusedMarker);
			focusedLocationMarker = std::dynamic_pointer_cast<FocusedLocationMarker>(focusedMarker);

			if (focusedQuestMarker)
			{
				std::string& markerTarget = focusedQuestMarker->questLocation.empty() ? focusedQuestMarker->questCharacterName : focusedQuestMarker->questLocation;

				compass->SetMarkerInfo(markerTarget, focusedMarker->distanceToPlayer, focusedMarker->heightDifference);

				if (focusChanged)
				{
					questItemList->AddQuest(focusedQuestMarker->questType, focusedQuestMarker->questName, focusedQuestMarker->questObjective);

					switch (focusedQuestMarker->questType)
					{
					case RE::QUEST_DATA::Type::kCivilWar:
						(IsPlayerAllyOfFaction(sonsOfSkyrimFaction) || IsPlayerAllyOfFaction(stormCloaksFaction)) ? 
							questItemList->SetQuestSide("StormCloaks") : questItemList->SetQuestSide("ImperialLegion");
						break;
					case RE::QUEST_DATA::Type::kDLC01_Vampire:
						IsPlayerAllyOfFaction(vampireFaction) ? questItemList->SetQuestSide("Vampires") : questItemList->SetQuestSide("Dawnguard");
						break;
					}

					questItemList->ShowQuest();
				}
			} 
			else if (focusedLocationMarker) 
			{
				compass->SetMarkerInfo(focusedLocationMarker->locationName, focusedMarker->distanceToPlayer, focusedMarker->heightDifference);
			}

			if (focusChanged)
			{
				compass->FocusMarker(focusedMarker->gfxIndex);
			} 
			else
			{
				compass->UpdateMarker(focusedMarker->gfxIndex);
			}
		}
		else 
		{
			focusedLocationMarker = nullptr;
			focusedQuestMarker = nullptr;
		}

		compass->SetMarkersSize();

		potentiallyFocusedMarkers.clear();
	}

	bool HUDMarkerManager::IsPlayerAllyOfFaction(const RE::TESFaction* a_faction)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		if (player->IsInFaction(a_faction)) 
		{
			return true;
		}

		return player->VisitFactions([a_faction](RE::TESFaction* a_visitedFaction, [[maybe_unused]] std::int8_t a_rank) -> bool
		{
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
}
