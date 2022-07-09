#include "Compass.h"

#include <numbers>
#include <chrono>

#include "RE/B/BGSInstancedQuestObjective.h"

#include "utils/Logger.h"

static constexpr float pi = std::numbers::pi_v<float>;

namespace util
{
	static void CropAngleRange(float& a_angle)
	{
		if (a_angle <= 2 * pi) {
			if (a_angle < 0.0F) {
				a_angle = std::fmodf(a_angle, 2 * pi) + 2 * pi;
			}
		} else {
			a_angle = std::fmodf(a_angle, 2 * pi);
		}
	};

	static float RadiansToDegrees(float a_angle)
	{
		return a_angle * 180.0F / pi;
	}

	static float GetAngleBetween(RE::PlayerCamera* a_playerCamera, RE::TESObjectREFR* a_markerRef)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::NiPoint3 playerPos = player->GetPosition();

		RE::NiPoint3 markerPos = a_markerRef->GetLookingAtLocation();

		float playerCameraYawAngle = a_playerCamera->yaw;

		float compassAngle = playerCameraYawAngle;

		if (RE::TESObjectCELL* parentCell = player->GetParentCell()) {
			compassAngle += parentCell->GetNorthRotation();
		}

		float headingAngle = playerPos.GetHorizontalAngleTo(markerPos);

		CropAngleRange(playerCameraYawAngle);
		CropAngleRange(compassAngle);
		CropAngleRange(headingAngle);

		float angle = headingAngle - playerCameraYawAngle;

		CropAngleRange(angle);

		return angle;
	}

	static float GetDistanceBetween(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_markerRef)
	{
		RE::NiPoint3 playerPos = a_player->GetPosition();
		RE::NiPoint3 markerPos = a_markerRef->GetPosition();
		//GetLookingAtLocation();
		return playerPos.GetDistance(markerPos);
	}
}

namespace extended
{
	bool Compass::ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef, 
									 std::uint32_t a_markerGotoFrame, RE::NiPoint3* a_markerPos)
	{
		float angleToPlayerCameraRads = util::GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_markerRef);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraRads);

		if (angleToPlayerCamera < filterAngle || angleToPlayerCamera > (360.0F - filterAngle))
		{
			FocusedQuestMarker* focusedQuestMarker = nullptr;

			for (RE::BGSInstancedQuestObjective& questObjective : RE::PlayerCharacter::GetSingleton()->objectives) 
			{
				if (questObjective.objective->ownerQuest == a_quest &&
					questObjective.instanceState == RE::QUEST_OBJECTIVE_STATE::kDisplayed)
				{
					focusedQuestMarker = focusedQuestMarkerMap.GetAt(&questObjective);

					focusedQuestMarker->questObjective = questObjective.GetDisplayTextWithReplacedTags();

#if DEBUG_QUEST_DESCRIPTION
					static std::vector<std::pair<RE::TESQuest*, std::uint16_t>> questStageShown;

					auto questStage = std::make_pair(a_quest, a_quest->currentStage);

					if (std::find(questStageShown.begin(), questStageShown.end(), questStage) == questStageShown.end())
					{
						RE::BSString questDescription = a_quest->GetCurrentDescriptionWithReplacedTags();

						logger::info("Description: {}", questDescription);
						logger::flush();

						questStageShown.push_back(questStage);
					}
#endif
					break;
				}
			}

			if (focusedQuestMarker) 
			{
				if (a_markerPos)
				{
					logger::trace("");
				}

				focusedQuestMarker->index = hudMarkerManager->currentMarkerIndex - 1;
				focusedQuestMarker->gotoFrame = a_markerGotoFrame;
				focusedQuestMarker->distanceToPlayer = util::GetDistanceBetween(RE::PlayerCharacter::GetSingleton(), a_markerRef);
				focusedQuestMarker->angleToPlayerCamera = angleToPlayerCamera;
				focusedQuestMarker->quest = a_quest;
				focusedQuestMarker->questType = a_quest->GetType();
				focusedQuestMarker->questName = (a_quest->GetType() == RE::QUEST_DATA::Type::kMiscellaneous) ? "$MISCELLANEOUS" : a_quest->GetName();

				// A quest marker can reference to a character or a location
				switch (a_markerRef->GetFormType())
				{
					case RE::FormType::Reference:
					{
						if (auto questRef = a_markerRef->As<RE::TESObjectREFR>()) 
						{
							// If it is a teleport door, we can get the door at the other side
							if (auto teleportLinkedDoor = questRef->extraList.GetTeleportLinkedDoor().get()) 
							{
								// First, try interior cell
								if (RE::TESObjectCELL* cell = teleportLinkedDoor->GetParentCell()) 
								{
									focusedQuestMarker->questLocation = cell->GetName();
								}
								// Exterior cell
								else if (RE::TESWorldSpace* worldSpace = teleportLinkedDoor->GetWorldspace())
								{
									focusedQuestMarker->questLocation = worldSpace->GetName();
								}
							}
						}
						break;
					}
					case RE::FormType::ActorCharacter:
					{
						if (auto questCharacter = a_markerRef->As<RE::Character>())
						{
							focusedQuestMarker->questCharacterName = questCharacter->GetName();
						}
						break;
					}
					default:
					{
						logger::debug("Unknown quest marker type: {}", (int)a_markerRef->GetFormType());
						break;
					}
				}

				focusedQuestMarker->markedForDelete = false;
			}
		}			

		return true;
	}

	bool Compass::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef,
										std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCameraRads = util::GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_markerRef);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraRads);

		auto test = RE::GameSettingCollection::GetSingleton();

		if (test) {}

		if (angleToPlayerCamera < filterAngle || angleToPlayerCamera > (360.0F - filterAngle))
		{
			FocusedLocationMarker* focusedLocationMarker = focusedLocationMarkerMap.GetAt(a_mapMarker);

			focusedLocationMarker->index = hudMarkerManager->currentMarkerIndex - 1;
			focusedLocationMarker->gotoFrame = a_markerGotoFrame;
			focusedLocationMarker->distanceToPlayer = util::GetDistanceBetween(RE::PlayerCharacter::GetSingleton(), a_markerRef);
			focusedLocationMarker->angleToPlayerCamera = angleToPlayerCamera;
			focusedLocationMarker->locationName = a_mapMarker->mapData->locationName.GetFullName();

			focusedLocationMarker->markedForDelete = false;
		}

		return true;
	}

	bool Compass::ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t)
	{
		float angleToPlayerCameraRads = util::GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_enemy);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraRads);

		if (angleToPlayerCamera < filterAngle || angleToPlayerCamera > (360.0F - filterAngle))
		{
			//if (!focusedMarker || focusedMarker->questType == RE::QUEST_DATA::Type::kNone) 
			//{
			//	focusedMarker = FocusedMarker{ .questName = a_enemy->GetName(), .relativeAngle = relativeAngle };
			//}
		}

		return true;
	}

	void Compass::SetMarkersExtraInfo()
	{
		auto test = Test::GetSingleton();

		focusedQuestMarkerMap.EraseElementsMarkedForDelete();
		focusedLocationMarkerMap.EraseElementsMarkedForDelete();

		if (!focusedQuestMarkerMap.empty()) 
		{
			FocusedQuestMarker* focusedQuestMarker = nullptr;

			for (auto& [objective, questMarker] : focusedQuestMarkerMap) 
			{
				if (questMarker.timeShown < 2.5F)
				{
					focusedQuestMarker = &questMarker;
				}

				questMarker.markedForDelete = true;
			}

			// All over the timeout to show
			if (!focusedQuestMarker) 
			{
				for (auto& [objective, questMarker] : focusedQuestMarkerMap)
				{
					questMarker.timeShown = 0.0F;
				}

				auto& [objective, questMarker] = *focusedQuestMarkerMap.begin();

				focusedQuestMarker = &questMarker;
			}

			focusedQuestMarker->timeShown += 1.0F / 60.0F;

			test->textField0.SetText(focusedQuestMarker->questName.c_str());
			test->textField1.SetText((std::string{ "Relative angle: " } + std::to_string(focusedQuestMarker->angleToPlayerCamera)).c_str());
			test->textField2.SetText((std::string{ "Distance: " } + std::to_string(static_cast<unsigned int>(focusedQuestMarker->distanceToPlayer * 0.01428F)) + " m").c_str());

			SetQuestInfo(focusedQuestMarker->questType, focusedQuestMarker->questName, focusedQuestMarker->questObjective, focusedQuestMarker->index);
		}
		else if (!focusedLocationMarkerMap.empty())
		{
			focusedQuestMarkerMap.clear();
			ClearQuestInfos();

			FocusedLocationMarker* focusedLocationMarker = nullptr;

			for (auto& [extraMapMarker, locationMarker] : focusedLocationMarkerMap)
			{
				if (!focusedLocationMarker) 
				{
					focusedLocationMarker = &locationMarker;
				}

				locationMarker.markedForDelete = true;
			}

			test->textField0.SetText(focusedLocationMarker->locationName.c_str());
			test->textField1.SetText((std::string{ "Relative angle: " } + std::to_string(focusedLocationMarker->angleToPlayerCamera)).c_str());
			test->textField2.SetText((std::string{ "Distance: " } + std::to_string(static_cast<unsigned int>(focusedLocationMarker->distanceToPlayer * 0.01428F)) + " m").c_str());

			SetLocationInfo(focusedLocationMarker->locationName, focusedLocationMarker->index);
		}
		else
		{
			focusedQuestMarkerMap.clear();
			ClearQuestInfos();

			focusedLocationMarkerMap.clear();

			test->textField0.SetText("");
			test->textField1.SetText("");
			test->textField2.SetText("");
		}
	}

	void Compass::SetQuestInfo(RE::QUEST_DATA::Type a_questType, const std::string& a_questName, const std::string& a_questObjective, std::uint32_t a_markerIndex)
	{
		Invoke("SetQuestInfo", a_questType, a_questName.c_str(), a_questObjective.c_str(), a_markerIndex);
	}

	void Compass::ClearQuestInfos()
	{
		Invoke("ClearQuestInfos");
	}

	void Compass::SetLocationInfo(const std::string& a_locationName, std::uint32_t a_markerIndex)
	{
		Invoke("SetLocationInfo", a_locationName.c_str(), a_markerIndex);
	}
}
