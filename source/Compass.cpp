#include "Compass.h"

#include <numbers>
#include <chrono>

#include "RE/B/BGSInstancedQuestObjective.h"

#include "utils/Logger.h"

static constexpr float pi = std::numbers::pi_v<float>;

namespace HCN::extended
{
	namespace util
	{
		static void CropAngleRange(float& a_angle)
		{
			if (a_angle <= 2 * pi) 
			{
				if (a_angle < 0.0F) 
				{
					a_angle = std::fmodf(a_angle, 2 * pi) + 2 * pi;
				}
			} 
			else 
			{
				a_angle = std::fmodf(a_angle, 2 * pi);
			}
		};

		static float RadiansToDeg(float a_angle)
		{
			return a_angle * 180.0F / pi; 
		}
	}

	float Compass::ProcessRelativeAngle(RE::TESObjectREFR* a_markerRef)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		RE::NiPoint3 markerPos = a_markerRef->GetLookingAtLocation();

		float playerCameraAngle = RE::PlayerCamera::GetSingleton()->yaw;

		float compassAngle = playerCameraAngle;

		if (RE::TESObjectCELL* parentCell = player->GetParentCell())
		{
			compassAngle += parentCell->GetNorthRotation();
		}

		float headingAngle = player->GetPosition().GetHorizontalAngleTo(markerPos);

		util::CropAngleRange(playerCameraAngle);
		util::CropAngleRange(compassAngle);
		util::CropAngleRange(headingAngle);

		float relativeAngle = headingAngle - playerCameraAngle;

		util::CropAngleRange(relativeAngle);

		return relativeAngle;
	}

	bool Compass::ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef, std::int32_t a_markerId)
	{
		float relativeAngle = ProcessRelativeAngle(a_markerRef);
		float relativeAngleDeg = util::RadiansToDeg(relativeAngle);

		if (relativeAngleDeg < tolerance || relativeAngleDeg > (360.0F - tolerance))
		{
			FocusedQuestMarker* focusedQuestMarker = nullptr;		

			for (RE::BGSInstancedQuestObjective& playerObjective : RE::PlayerCharacter::GetSingleton()->objectives) 
			{
				if (playerObjective.objective->ownerQuest == a_quest &&
					playerObjective.instanceState == RE::QUEST_OBJECTIVE_STATE::kDisplayed)
				{
					focusedQuestMarker = focusedQuestMarkerMap.GetAt(&playerObjective);

					focusedQuestMarker->questObjective = playerObjective.GetDisplayTextWithReplacedTags();

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
				focusedQuestMarker->id = a_markerId;
				focusedQuestMarker->relativeAngle = relativeAngle;
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

	bool Compass::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef, std::int32_t a_markerId)
	{
		float relativeAngle = ProcessRelativeAngle(a_markerRef);
		float relativeAngleDeg = util::RadiansToDeg(relativeAngle);

		if (relativeAngleDeg < tolerance || relativeAngleDeg > (360.0F - tolerance)) 
		{
			FocusedLocationMarker* focusedLocationMarker = focusedLocationMarkerMap.GetAt(a_mapMarker);

			focusedLocationMarker->id = a_markerId;
			focusedLocationMarker->relativeAngle = relativeAngle;
			focusedLocationMarker->locationName = a_mapMarker->mapData->locationName.GetFullName();

			focusedLocationMarker->markedForDelete = false;
		}

		return true;
	}

	bool Compass::ProcessEnemyMarker(RE::Character* a_enemy, std::int32_t)
	{
		float relativeAngle = ProcessRelativeAngle(a_enemy);
		float relativeAngleDeg = util::RadiansToDeg(relativeAngle);

		if (relativeAngleDeg < tolerance || relativeAngleDeg > (360.0F - tolerance))
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
			test->textField1.SetText((std::string{ "Relative angle: " } + std::to_string(util::RadiansToDeg(focusedQuestMarker->relativeAngle))).c_str());
			//test->textField2.SetText((std::string{ "Heading angle: " } + std::to_string(util::RadiansToDeg(focusedMarker->headingAngle))).c_str());

			SetQuestInfo(focusedQuestMarker->questType, focusedQuestMarker->questName, focusedQuestMarker->questObjective);
		}
		else if (!focusedLocationMarkerMap.empty())
		{
			focusedQuestMarkerMap.clear();
			ClearQuestInfos();

			for (auto& [extraMapMarker, focusedLocationMarker] : focusedLocationMarkerMap) 
			{
				test->textField0.SetText(focusedLocationMarker.locationName.c_str());
				test->textField1.SetText((std::string{ "Relative angle: " } + std::to_string(util::RadiansToDeg(focusedLocationMarker.relativeAngle))).c_str());
				//test->textField2.SetText("");

				SetLocationInfo(focusedLocationMarker.locationName);

				focusedLocationMarker.markedForDelete = true;
			}
		}
		else
		{
			focusedQuestMarkerMap.clear();
			ClearQuestInfos();

			test->textField0.SetText("");
			test->textField1.SetText("");
		}
	}

	void Compass::SetQuestInfo(RE::QUEST_DATA::Type a_questType, const std::string& a_questName, const std::string& a_questObjective)
	{
		Invoke("SetQuestInfo", a_questType, a_questName.c_str(), a_questObjective.c_str());
	}

	void Compass::SetLocationInfo(const std::string& a_locationName)
	{
		Invoke("SetLocation", a_locationName.c_str());
	}

	void Compass::ClearQuestInfos()
	{
		Invoke("ClearQuestInfos");
	}

}
