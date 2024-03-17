#include "HUDMarkerManager.h"

#include "RE/B/BSTimer.h"

#include "RE/I/IMenu.h"

RE::BSTArray<RE::BGSInstancedQuestObjective>& GetPlayerObjectives();

namespace extended
{
	void HUDMarkerManager::ProcessQuestMarker(RE::TESQuest* a_quest, RE::BGSInstancedQuestObjective& a_instancedObjective, RE::TESQuestTarget* a_target,
											  RE::TESObjectREFR* a_marker, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(playerCamera, a_marker);

		if ((IsTheFocusedMarker(a_marker) && angleToPlayerCamera < keepFocusedAngle) || 
			angleToPlayerCamera < facingAngle)
		{
			std::shared_ptr<FocusedMarker> facedMarker = GetFacedMarkerUpdated(a_marker, angleToPlayerCamera);

			// For this marker, find the data for this quest
			auto dataFound = std::ranges::find_if(facedMarker->data,
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
			if (dataFound != facedMarker->data.end())
			{
				questData = std::dynamic_pointer_cast<FocusedMarker::QuestData>(*dataFound);
				questData->gfxIndex = hudMarkerManager->currentMarkerIndex - 1;
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
				facedMarker->data.push_back(questData);
			}

			RE::BSTArray<RE::BGSInstancedQuestObjective>& playerObjectives = GetPlayerObjectives();

			// Add objectives to the quest data. The objectives are in oldest-to-newest order,
			// so we iterate from newest-to-oldest to have it in the same order as in the journal
			for (int i = playerObjectives.size() - 1; i >= 0; i--)
			{
				const RE::BGSInstancedQuestObjective& instancedObjective = playerObjectives[i];

				bool objectiveHasTarget = false;
				for (int j = 0; j < instancedObjective.objective->numTargets; j++)
				{
					if (a_target == instancedObjective.objective->targets[j])
					{
						objectiveHasTarget = true;
						break;
					}
				}

				if (instancedObjective.objective->ownerQuest == a_quest &&
					instancedObjective.instanceState == RE::QUEST_OBJECTIVE_STATE::kDisplayed && objectiveHasTarget)
				{
					// Add each objective only once per quest
					if (std::ranges::find(questData->addedInstancedObjectives, &instancedObjective) == questData->addedInstancedObjectives.end())
					{
						// We need to assign an age index (only the first time we process this quest data)
						// to order the quests as in the journal
						if (questData->ageIndex == -1)
						{
							questData->ageIndex = i;
						}

						questData->addedInstancedObjectives.push_back(&instancedObjective);
						questData->objectives.push_back(instancedObjective.GetDisplayTextWithReplacedTags().c_str());
					}
				}
			}

			if (questData->addedInstancedObjectives.empty())
			{
				// Make sure we don't process invalid data, otherwise CTD
				facedMarker->data.pop_back();
			}
		}
	}

	void HUDMarkerManager::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_marker,
												 std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(playerCamera, a_marker);

		bool isDiscoveredLocation = a_mapMarker->mapData->flags.all(RE::MapMarkerData::Flag::kVisible);

		if (isDiscoveredLocation || !settings::display::undiscoveredMeansUnknownInfo)
		{
			if ((IsTheFocusedMarker(a_marker) && angleToPlayerCamera < keepFocusedAngle) ||
				angleToPlayerCamera < facingAngle)
			{
				std::shared_ptr<FocusedMarker> facedMarker = GetFacedMarkerUpdated(a_marker, angleToPlayerCamera);

				// Add the location data for this marker
				auto locationData = std::make_shared<FocusedMarker::LocationData>
				(
					hudMarkerManager->currentMarkerIndex - 1,
					a_markerGotoFrame,
					a_mapMarker->mapData
				);

				facedMarker->data.push_back(locationData);
			}
		}

		if (!isDiscoveredLocation && settings::display::undiscoveredMeansUnknownMarkers)
		{
			hudMarkerManager->scaleformMarkerData[hudMarkerManager->currentMarkerIndex - 1].type.SetNumber(0);
		}
	}

	void HUDMarkerManager::ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(playerCamera, a_enemy);

		if ((IsTheFocusedMarker(a_enemy) && angleToPlayerCamera < keepFocusedAngle) ||
			angleToPlayerCamera < facingAngle)
		{
			std::shared_ptr<FocusedMarker> facedMarker = GetFacedMarkerUpdated(a_enemy, angleToPlayerCamera);

			// Add the enemy data for this marker
			auto enemyData = std::make_shared<FocusedMarker::EnemyData>
			(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame,
				a_enemy
			);

			facedMarker->data.push_back(enemyData);
		}
	}

	void HUDMarkerManager::ProcessPlayerSetMarker(RE::TESObjectREFR* a_marker, std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCamera = GetAngleBetween(playerCamera, a_marker);

		if ((IsTheFocusedMarker(a_marker) && angleToPlayerCamera < keepFocusedAngle) ||
			angleToPlayerCamera < facingAngle)
		{
			std::shared_ptr<FocusedMarker> facedMarker = GetFacedMarkerUpdated(a_marker, angleToPlayerCamera);

			// Add the player set data for this marker
			auto playerSetData = std::make_shared<FocusedMarker::PlayerSetData>
			(
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame
			);

			facedMarker->data.push_back(playerSetData);
		}
	}

	void HUDMarkerManager::SetMarkers()
	{
		RE::ActorState* playerState = player->AsActorState();

		bool canQuestItemListBeDisplayed = questItemList->CanBeDisplayed(player->GetParentCell(), playerState->IsWeaponDrawn());

		if (!canQuestItemListBeDisplayed)
		{
			questItemList->SetHiddenByForce(true);
		}

		bool focusChanged = UpdateFocusedMarker();

		if (focusChanged)
		{
			compass->UnfocusMarker();
			timeFocusingMarker = 0.0F;
		}
		else if (focusedMarker)
		{
			timeFocusingMarker += timeManager->realTimeDelta;
		}

		if (focusChanged || questItemList->IsHiddenByForce())
		{
			questItemList->RemoveAllQuests();
		}

		bool isFocusedMarkerQuest = false;
		std::int32_t focusedMarkerIndex = -1;

		if (focusedMarker)
		{
			std::string targetText;

			std::uint32_t objectivesCount = 0;

			for (std::shared_ptr<FocusedMarker::Data> focusedMarkerData : focusedMarker->data)
			{
				if (auto questData = std::dynamic_pointer_cast<FocusedMarker::QuestData>(focusedMarkerData))
				{
					targetText = questData->GetTargetText();

					isFocusedMarkerQuest = true;

					objectivesCount += questData->objectives.size(); 

					if (canQuestItemListBeDisplayed && (focusChanged || questItemList->IsHiddenByForce()))
					{
						questItemList->AddQuest(questData->type, questData->name,questData->isInSameLocation, 
												questData->objectives, questData->ageIndex);
						questItemList->SetQuestSide(GetSideInQuest(questData->type));

						// If we call a function more than once per frame (like in this for-loop)
						// we need to update the stage with `GFxMovieView::Advance`, otherwise graphical
						// glitches occur to the element when showing up
						questItemList->GetMovieView()->Advance(0.0F);
					}
				}
				else if (auto locationData = std::dynamic_pointer_cast<FocusedMarker::LocationData>(focusedMarkerData))
				{
					targetText = locationData->locationName;
				}
				else if (auto enemyData = std::dynamic_pointer_cast<FocusedMarker::EnemyData>(focusedMarkerData))
				{
					targetText = settings::display::showEnemyNameUnderMarker ? enemyData->enemyName : "";
				}
				else if (auto playerSetData = std::dynamic_pointer_cast<FocusedMarker::PlayerSetData>(focusedMarkerData))
				{
					if (isFocusedMarkerQuest)
					{
						continue;
					}

					targetText = playerSetData->locationName;
				}

				focusedMarkerIndex = focusedMarkerData->gfxIndex;
			}

			if (settings::display::showObjectiveAsTarget && settings::display::showOtherObjectivesCount && objectivesCount > 1)
			{
				targetText += " (+" + std::to_string(objectivesCount - 1) + ")";
			}

			compass->SetFocusedMarkerInfo(targetText, focusedMarker->distanceToPlayer,
										  focusedMarker->heightDifference, focusedMarkerIndex);

			if (focusChanged)
			{
				compass->FocusMarker(focusedMarkerIndex);
			}

			compass->UpdateFocusedMarker(focusedMarkerIndex);
		}

		compass->SetMarkers();

		if (isFocusedMarkerQuest && canQuestItemListBeDisplayed)
		{
			questItemList->SetHiddenByForce(false);

			float playerSpeed = playerState->DoGetMovementSpeed();

			float delayToShow = (playerSpeed < player->GetWalkSpeed()) ? settings::questlist::walkingDelayToShow :
								(playerSpeed < player->GetJogSpeed())  ? settings::questlist::joggingDelayToShow :
																		 settings::questlist::sprintingDelayToShow;

			if (timeFocusingMarker > delayToShow)
			{
				questItemList->ShowAllQuests();
				questItemList->Update();
			}
		}
	}

	std::shared_ptr<FocusedMarker> 
	HUDMarkerManager::GetMostCenteredMarkerOf(const std::unordered_map<const RE::TESObjectREFR*, 
																	   std::shared_ptr<FocusedMarker>>& a_facedMarkers) const
	{
		float closestAngleToPlayerCamera = std::numeric_limits<float>::max();
		std::shared_ptr<FocusedMarker> mostCenteredMarker = nullptr;

		for (const auto& [markerRef, facedMarker] : a_facedMarkers)
		{
			if (facedMarker->angleToPlayerCamera < closestAngleToPlayerCamera)
			{
				mostCenteredMarker = facedMarker;
				closestAngleToPlayerCamera = facedMarker->angleToPlayerCamera;
			}
		}

		return mostCenteredMarker;
	}

	bool HUDMarkerManager::UpdateFocusedMarker()
	{
		std::shared_ptr<FocusedMarker> mostCenteredMarker = GetMostCenteredMarkerOf(facedMarkers);
		facedMarkers.clear();

		auto IsSameFacedMarker = [](const std::shared_ptr<FocusedMarker>& a_left, const std::shared_ptr<FocusedMarker>& a_right) -> bool
		{
			return a_left && a_right && a_left->ref == a_right->ref;
		};

		if (IsSameFacedMarker(lastMostCenteredMarker, mostCenteredMarker))
		{
			timeFacingMarker += timeManager->realTimeDelta;
		}
		else
		{
			timeFacingMarker = 0.0F;
		}

		lastMostCenteredMarker = mostCenteredMarker;

		if (!mostCenteredMarker && focusedMarker)
		{
			focusedMarker = nullptr;
			return true;
		}
		else
		{
			if (timeFacingMarker > settings::display::focusingDelayToShow)
			{
				if (!IsSameFacedMarker(focusedMarker, mostCenteredMarker))
				{
					focusedMarker = mostCenteredMarker;
					return true;
				}
				else if (IsSameFacedMarker(focusedMarker, lastMostCenteredMarker))
				{
					focusedMarker = mostCenteredMarker;
				}
			}
		}

		return false;
	}

	std::shared_ptr<FocusedMarker> HUDMarkerManager::GetFacedMarkerUpdated(const RE::TESObjectREFR* a_marker, float a_angleToPlayerCamera)
	{
		std::shared_ptr<FocusedMarker> facedMarker;

		if (facedMarkers.contains(a_marker))
		{
			facedMarker = facedMarkers.at(a_marker);
			facedMarker->UpdateGeometry(a_angleToPlayerCamera);
		}
		else
		{
			facedMarker = std::make_shared<FocusedMarker>(a_marker, a_angleToPlayerCamera);
			facedMarkers.emplace(a_marker, facedMarker);
		}

		return facedMarker;
	}

	float HUDMarkerManager::GetAngleBetween(const RE::PlayerCamera* a_playerCamera,
											const RE::TESObjectREFR* a_marker) const
	{
		float angleToPlayerCameraInRadians = util::GetAngleBetween(a_playerCamera, a_marker);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraInRadians);

		if (angleToPlayerCamera > 180.0F)
			angleToPlayerCamera = 360.0F - angleToPlayerCamera;

		return angleToPlayerCamera;
	}

	bool HUDMarkerManager::IsPlayerAllyOfFaction(const RE::TESFaction* a_faction) const
	{
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
			if (IsPlayerAllyOfFaction(sonsOfSkyrimFaction) || IsPlayerAllyOfFaction(stormCloaksFaction) ||
				IsPlayerOpponentOfFaction(imperialLegionFaction))
			{
				return "StormCloaks";
			}
			else
			{
				return "ImperialLegion";
			}
		case RE::QUEST_DATA::Type::kDLC01_Vampire:
			if (player->HasKeywordString("Vampire") || IsPlayerAllyOfFaction(vampireFaction) ||
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
