#include "Compass.h"

#include <numbers>

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

		std::string questInfo{ a_quest->GetName() };
		if (a_quest->GetType() == RE::QUEST_DATA::Type::kMiscellaneous) 
		{
			for (RE::BGSQuestObjective* objective : a_quest->objectives) 
			{
				if (objective->index == a_quest->currentStage) 
				{
					questInfo += objective->displayText;
					break;
				}
			}
		}

		// A quest marker can reference to a character or a location
		switch (a_markerRef->GetFormType())
		{
			case RE::FormType::Reference:
			{
				if (auto questTeleporter = a_markerRef->As<RE::TESObjectREFR>()) 
				{
					questInfo += ", ";
					//questInfo += questTeleporter->GetName();

					auto startingPosition = questTeleporter->extraList.GetByType<RE::ExtraStartingPosition>();
					if (startingPosition) 
					{
						questInfo += startingPosition->location->GetFullName();
					}
				}
				break;
			}
			case RE::FormType::ActorCharacter:
			{
				if (auto questCharacter = a_markerRef->As<RE::Character>())
				{
					questInfo += ", ";
					questInfo += questCharacter->GetName();
				}
				break;
			}
			default:
			{
				questInfo += ", Unknown quest marker type ";
				questInfo += std::to_string((int)a_markerRef->GetFormType());
				break;
			}
		}

		float relativeAngleDeg = util::RadiansToDeg(relativeAngle);

		if (relativeAngleDeg < tolerance || relativeAngleDeg > (360.0F - tolerance))
		{
			focusedMarker = FocusedMarker { .name = questInfo.c_str(), .relativeAngle = relativeAngle, .questType = a_quest->data.questType.get() };
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
				focusedMarker = FocusedMarker{ .name = a_mapMarker->mapData->locationName.GetFullName(), .relativeAngle = relativeAngle };
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
				focusedMarker = FocusedMarker{ .name = a_enemy->GetName(), .relativeAngle = relativeAngle };
			}
		}

		return true;
	}

	void Compass::SetMarkersExtraInfo()
	{
		auto test = Test::GetSingleton();

		if (focusedMarker) 
		{
			test->textField0.SetText(focusedMarker->name.c_str());
			test->textField1.SetText((std::string{ "Relative angle: " } + std::to_string(util::RadiansToDeg(focusedMarker->relativeAngle))).c_str());
			//test->textField2.SetText((std::string{ "Heading angle: " } + std::to_string(util::RadiansToDeg(focusedMarker->headingAngle))).c_str());

			SetQuestInfo(focusedMarker->name, focusedMarker->questType);

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
