#include "HUDMarkerManager.h"

#include "RE/B/BSTimer.h"

namespace extended
{
	void HUDMarkerManager::ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_marker,
											  std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_marker);

		if (angleToPlayerCamera < potentiallyFocusedAngle)
		{
			std::shared_ptr<FocusedMarker> potentiallyFocusedMarker = GetUpdatedPotentiallyFocusedMarker(a_marker, angleToPlayerCamera);

			// For this marker, find the data for this quest
			auto dataFound = std::ranges::find_if(potentiallyFocusedMarker->data,
				[a_quest](std::shared_ptr<FocusedMarker::Data> a_data) -> bool
				{
					auto questData = std::dynamic_pointer_cast<FocusedMarker::QuestData>(a_data);

					// Group miscellaneous quests as a single quest, like in the journal
					if (questData->type == RE::QUEST_DATA::Type::kMiscellaneous)
					{
						return a_quest->GetType() == RE::QUEST_DATA::Type::kMiscellaneous;
					}
					else
					{
						return questData->quest == a_quest;
					}
				});

			// Get if found, create if not
			std::shared_ptr<FocusedMarker::QuestData> questData;
			if (dataFound != potentiallyFocusedMarker->data.end())
			{
				questData = std::dynamic_pointer_cast<FocusedMarker::QuestData>(*dataFound);
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

			auto player = RE::PlayerCharacter::GetSingleton();

			// Add objectives to the quest data. The objectives are in oldest-to-newest order,
			// so we iterate from newest-to-oldest to have it in the same order as in the journal
			for (int i = player->objectives.size() - 1; i >= 0; i--)
			{
				const RE::BGSInstancedQuestObjective& instancedObjective = player->objectives[i];

				if (instancedObjective.objective->ownerQuest == a_quest &&
					instancedObjective.instanceState == RE::QUEST_OBJECTIVE_STATE::kDisplayed)
				{
					// Add each objective only once per quest
					if (std::ranges::find(questData->addedInstancedObjectives, &instancedObjective) == questData->addedInstancedObjectives.end()) 
					{
						if (questData->ageIndex == -1)
						{
							questData->ageIndex = i;
						}

						questData->addedInstancedObjectives.push_back(&instancedObjective);
						questData->objectives.push_back(instancedObjective.GetDisplayTextWithReplacedTags().c_str());
					}
				}
			}
		}
	}

	void HUDMarkerManager::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_marker,
												 std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_marker);

		if (angleToPlayerCamera < potentiallyFocusedAngle)
		{
			std::shared_ptr<FocusedMarker> potentiallyFocusedMarker = GetUpdatedPotentiallyFocusedMarker(a_marker, angleToPlayerCamera);

			// Add the location data for this marker
			auto locationData = std::make_shared<FocusedMarker::LocationData>
			(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame,
				a_mapMarker->mapData
			);

			if (potentiallyFocusedMarker->data.empty())
			{
				potentiallyFocusedMarker->data.push_back(locationData);
			}
			else
			{
				potentiallyFocusedMarker->data.back() = locationData;
			}
		}
	}

	void HUDMarkerManager::ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_enemy);

		if (angleToPlayerCamera < potentiallyFocusedAngle)
		{
			std::shared_ptr<FocusedMarker> potentiallyFocusedMarker = GetUpdatedPotentiallyFocusedMarker(a_enemy, angleToPlayerCamera);

			// Add the enemy data for this marker
			auto enemyData = std::make_shared<FocusedMarker::EnemyData>
			(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame,
				a_enemy
			);

			if (potentiallyFocusedMarker->data.empty())
			{
				potentiallyFocusedMarker->data.push_back(enemyData);
			}
			else
			{
				potentiallyFocusedMarker->data.back() = enemyData;
			}
		}
	}

	void HUDMarkerManager::ProcessPlayerSetMarker(RE::TESObjectREFR* a_marker, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_marker);

		if (angleToPlayerCamera < potentiallyFocusedAngle)
		{
			std::shared_ptr<FocusedMarker> potentiallyFocusedMarker = GetUpdatedPotentiallyFocusedMarker(a_marker, angleToPlayerCamera);

			// Add the player set data for this marker
			auto playerSetData = std::make_shared<FocusedMarker::PlayerSetData>
			(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame
			);

			if (potentiallyFocusedMarker->data.empty())
			{
				potentiallyFocusedMarker->data.push_back(playerSetData);
			}
			else
			{
				potentiallyFocusedMarker->data.back() = playerSetData;
			}
		}
	}

	void HUDMarkerManager::SetMarkersExtraInfo()
	{
		auto player = RE::PlayerCharacter::GetSingleton();

		float timeSinceLastFrame = RE::BSTimer::GetTimeManager()->realTimeDelta;

		Compass* compass = Compass::GetSingleton();
		QuestItemList* questItemList = QuestItemList::GetSingleton();

		auto CanDisplayQuestItemListHere = [player]() -> bool
		{
			if (RE::TESObjectCELL* playerCell = player->GetParentCell())
			{
				if ((playerCell->IsInteriorCell() && settings::questlist::showInInteriors) ||
					(playerCell->IsExteriorCell() && settings::questlist::showInExteriors))
				{
					return true;
				}
			}

			return false;
		};

		std::shared_ptr<FocusedMarker> nextFocusedMarker = GetNextFocusedMarker();

		bool focusChanged = DidFocusChange(nextFocusedMarker);

		if (focusChanged)
		{
			timeFocusingMarker = 0.0F;

			if (focusedMarker)
			{
				compass->UnfocusMarker(focusedMarker->data.back()->gfxIndex);
			}
		}

		if (focusChanged || (settings::questlist::hideInCombat && player->IsWeaponDrawn()))
		{
			if (questItemList)
				questItemList->RemoveAllQuests();
		}

		focusedMarker = nextFocusedMarker;

		if (focusedMarker)
		{
			bool canFocusPlayerSetMarker = true;
			uint32_t gfxIndex = 0;

			for (std::shared_ptr<FocusedMarker::Data> focusedMarkerData : focusedMarker->data)
			{
				if (auto questData = std::dynamic_pointer_cast<FocusedMarker::QuestData>(focusedMarkerData))
				{
					canFocusPlayerSetMarker = false;

					const std::string& targetText = [questData]()
					{
						if (settings::display::showObjectiveAsTarget)
						{
							return questData->objectives.back();
						}
						else
						{
							return questData->locationName.empty() ? questData->characterName : questData->locationName;
						}
					}();

					compass->SetMarkerInfo(targetText, focusedMarker->distanceToPlayer, focusedMarker->heightDifference);

					if (focusChanged && questItemList && CanDisplayQuestItemListHere())
					{
						questItemList->AddQuest(questData->type, questData->name,questData->isInSameLocation, questData->objectives, questData->ageIndex);
						questItemList->SetQuestSide(GetSideInQuest(questData->type));
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

				gfxIndex = focusedMarkerData->gfxIndex;
			}

			
			if (focusChanged)
			{
				compass->FocusMarker(gfxIndex);
			} 
			else
			{
				compass->UpdateMarker(gfxIndex);
			}
		}

		// Set smaller so that when there is a focused marker it appears bigger
		compass->SetMarkersSize();

		if (focusedMarker && (!settings::questlist::hideInCombat || !player->IsWeaponDrawn()))
		{
			timeFocusingMarker += timeSinceLastFrame;

			if (questItemList && CanDisplayQuestItemListHere())
			{
				float timeThreshold;
				if (player->DoGetMovementSpeed() < player->GetWalkSpeed())
				{
					timeThreshold = settings::questlist::walkingDelayToShow;
				}
				else if (player->DoGetMovementSpeed() < player->GetJogSpeed())
				{
					timeThreshold = settings::questlist::joggingDelayToShow;
				}
				else
				{
					timeThreshold = settings::questlist::sprintingDelayToShow;
				}

				if (timeFocusingMarker > timeThreshold)
				{
					questItemList->ShowAllQuests();
				}	
			}
		}

		if (questItemList && CanDisplayQuestItemListHere())
		{
			questItemList->Update();
		}
	}

	void HUDMarkerManager::RemovePotentiallyFocusedMarkersOutOfRange()
	{
		std::erase_if(potentiallyFocusedMarkers, [this](const auto& a_item) -> bool
		{
			const auto& [markerRef, marker] = a_item;

			if (IsFocusedMarker(markerRef))
			{
				if (marker->angleToPlayerCamera > keepFocusedAngle)
				{
					return true;
				}
			}
			else
			{
				return true;
			}

			return false;
		});
	}

	std::shared_ptr<FocusedMarker> HUDMarkerManager::GetNextFocusedMarker()
	{
		float closestAngleToPlayerCamera;
		std::shared_ptr<FocusedMarker> bestFocusedMarker;

		if (focusedMarker)
		{
			closestAngleToPlayerCamera = GetAngleBetween(RE::PlayerCamera::GetSingleton(), focusedMarker->ref);
			focusedMarker->UpdateGeometry(closestAngleToPlayerCamera);

			bestFocusedMarker = (focusedMarker->angleToPlayerCamera <= keepFocusedAngle) ? focusedMarker : nullptr;
		}
		else
		{
			closestAngleToPlayerCamera = std::numeric_limits<float>::max();
			bestFocusedMarker = nullptr;
		}

		for (const auto& [markerRef, potentiallyFocusedMarker] : potentiallyFocusedMarkers)
		{
			if (potentiallyFocusedMarker->angleToPlayerCamera < closestAngleToPlayerCamera)
			{
				bestFocusedMarker = potentiallyFocusedMarker;
				closestAngleToPlayerCamera = potentiallyFocusedMarker->angleToPlayerCamera;
			}
		}

		potentiallyFocusedMarkers.clear();

		return bestFocusedMarker;
	}

	bool HUDMarkerManager::DidFocusChange(std::shared_ptr<FocusedMarker> a_nextFocusedMarker) const
	{
		if ((focusedMarker && !a_nextFocusedMarker) || (!focusedMarker && a_nextFocusedMarker))
		{
			return true;
		}
		else if (focusedMarker && a_nextFocusedMarker)
		{
			if (focusedMarker->ref != a_nextFocusedMarker->ref)
			{
				return true;
			}
			else
			{
				// TODO: return true also if the data of the markers changed
				//for (focusedMarker->data)
			}
		}

		return false;
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
		auto player = RE::PlayerCharacter::GetSingleton();

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
		auto player = RE::PlayerCharacter::GetSingleton();

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
