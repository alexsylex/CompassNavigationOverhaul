#include "Compass.h"

#include <numbers>

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

	bool Compass::ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef)
	{
		float relativeAngle = ProcessRelativeAngle(a_markerRef);

		std::string questName{ a_quest->GetName() };
		
		static REL::Relocation<void (RE::TESQuest::*)(RE::BSString&, std::int32_t)> GetQuestDescription{ REL::ID(24549) };

		static std::vector<std::pair<RE::TESQuest*, std::uint16_t>> questStageShown;

		for (RE::BGSInstancedQuestObjective& playerObjective : RE::PlayerCharacter::GetSingleton()->objectives) 
		{
			if (playerObjective.objective->ownerQuest == a_quest)
			{
				RE::BSString objectiveText = playerObjective.GetDisplayTextWithReplacedTags();

				if (a_quest->GetType() == RE::QUEST_DATA::Type::kMiscellaneous) 
				{
					questName += objectiveText;
				}

				auto questStage = std::make_pair(a_quest, a_quest->currentStage);

				if (std::find(questStageShown.begin(), questStageShown.end(), questStage) == questStageShown.end())
				{
					RE::BSString questDescription = a_quest->GetCurrentDescriptionWithReplacedTags();

					logger::info("Description: {}", questDescription);
					logger::info("Objective: {}", objectiveText);
					logger::flush();

					questStageShown.push_back(questStage);
				}

				break;
			}
		}

		std::string questLocation;

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
							questLocation = cell->GetName();
						}
						// Exterior cell
						else if (RE::TESWorldSpace* worldSpace = teleportLinkedDoor->GetWorldspace())
						{
							questLocation = worldSpace->GetName();
						}
					}
				}
				break;
			}
			case RE::FormType::ActorCharacter:
			{
				if (auto questCharacter = a_markerRef->As<RE::Character>())
				{
					questName += ", ";
					questName += questCharacter->GetName();
				}
				break;
			}
			default:
			{
				logger::debug("Unknown quest marker type: {}", (int)a_markerRef->GetFormType());
				break;
			}
		}

		float relativeAngleDeg = util::RadiansToDeg(relativeAngle);

		if (relativeAngleDeg < tolerance || relativeAngleDeg > (360.0F - tolerance))
		{
			focusedMarker = FocusedMarker{ .questName = questName.c_str(), .relativeAngle = relativeAngle, .questType = a_quest->data.questType.get() };
		}

		return true;
	}

	bool Compass::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef)
	{
		float relativeAngle = ProcessRelativeAngle(a_markerRef);

		float relativeAngleDeg = util::RadiansToDeg(relativeAngle);

		if (relativeAngleDeg < tolerance || relativeAngleDeg > (360.0F - tolerance))
		{
			if (!focusedMarker || focusedMarker->questType == RE::QUEST_DATA::Type::kNone) 
			{
				focusedMarker = FocusedMarker{ .questName = a_mapMarker->mapData->locationName.GetFullName(), .relativeAngle = relativeAngle };
			}
		}

		return true;
	}

	bool Compass::ProcessEnemyMarker(RE::Character* a_enemy)
	{
		float relativeAngle = ProcessRelativeAngle(a_enemy);

		float relativeAngleDeg = util::RadiansToDeg(relativeAngle);

		if (relativeAngleDeg < tolerance || relativeAngleDeg > (360.0F - tolerance))
		{
			if (!focusedMarker || focusedMarker->questType == RE::QUEST_DATA::Type::kNone) 
			{
				focusedMarker = FocusedMarker{ .questName = a_enemy->GetName(), .relativeAngle = relativeAngle };
			}
		}

		return true;
	}

	void Compass::SetMarkersExtraInfo()
	{
		auto test = Test::GetSingleton();

		if (focusedMarker) 
		{
			test->textField0.SetText(focusedMarker->questName.c_str());
			test->textField1.SetText((std::string{ "Relative angle: " } + std::to_string(util::RadiansToDeg(focusedMarker->relativeAngle))).c_str());
			//test->textField2.SetText((std::string{ "Heading angle: " } + std::to_string(util::RadiansToDeg(focusedMarker->headingAngle))).c_str());

			SetQuestInfo(focusedMarker->questName, focusedMarker->questType);

			focusedMarker = { };
		}
		else 
		{
			SetQuestInfo("", RE::QUEST_DATA::Type::kNone);

			test->textField0.SetText("");
			test->textField1.SetText("");
			//test->textField2.SetText("");
		}
	}

	void Compass::SetQuestInfo(const std::string& a_questTitle, RE::QUEST_DATA::Type a_questType)
	{
		Invoke("SetQuestInfo", a_questTitle.c_str(), a_questType, a_questType != RE::QUEST_DATA::Type::kNone);
	}
}
