#include "CompassShoutMeterHolder.h"

#include <numbers>

#include "utils/Logger.h"

static constexpr float pi = std::numbers::pi_v<float>;

namespace HCN
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

	class Test : GFxDisplayObject
	{
	public:

		Test(const GFxDisplayObject& a_test) 
		: GFxDisplayObject{ a_test }
		{ }

		// DEBUG
		GFxDisplayObject textField0 = GetMember("TextField0");
		GFxDisplayObject textField1 = GetMember("TextField1");
		GFxDisplayObject textField2 = GetMember("TextField2");
	};

	std::tuple<float, float, float> CompassShoutMeterHolder::ProcessRelativeAngle(RE::TESObjectREFR* a_markerRef)
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

		return std::make_tuple(playerCameraAngle, compassAngle, headingAngle);
	}

	bool CompassShoutMeterHolder::ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef)
	{
		logger::trace("\"{}\" quest hooked", a_quest->GetName());

		auto [playerCameraAngle, compassAngle, headingAngle] = ProcessRelativeAngle(a_markerRef);

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

		float relativeAngle = util::RadiansToDeg(headingAngle - playerCameraAngle);

		if (std::abs(relativeAngle) < 20.0F) 
		{
			Test test{ GFxDisplayObject{ GetMovieView(), "_root.Test" } };

			test.textField0.SetText(questInfo.c_str());
			test.textField1.SetText((std::string{ "Heading angle: " } + std::to_string(util::RadiansToDeg(headingAngle))).c_str());
			test.textField2.SetText((std::string{ "Relative angle: " } + std::to_string(relativeAngle)).c_str());
		}

		return true;
	}

	bool CompassShoutMeterHolder::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef)
	{
		auto [playerCameraAngle, compassAngle, headingAngle] = ProcessRelativeAngle(a_markerRef);

		logger::trace("\"{}\" location hooked", a_mapMarker->mapData->locationName.GetFullName());
		logger::flush();

		float relativeAngle = util::RadiansToDeg(headingAngle - playerCameraAngle);

		if (std::abs(relativeAngle) < 20.0F) 
		{
			Test test{ GFxDisplayObject{ GetMovieView(), "_root.Test" } };

			test.textField0.SetText(a_mapMarker->mapData->locationName.GetFullName());
			test.textField1.SetText((std::string{ "Heading angle: " } + std::to_string(util::RadiansToDeg(headingAngle))).c_str());
			test.textField2.SetText((std::string{ "Relative angle: " } + std::to_string(relativeAngle)).c_str());
		}

		return true;
	}

	bool CompassShoutMeterHolder::ProcessEnemyMarker(RE::Character* a_enemy)
	{
		auto [playerCameraAngle, compassAngle, headingAngle] = ProcessRelativeAngle(a_enemy);

		logger::trace("\"{}\" enemy hooked", a_enemy->GetName());
		logger::flush();

		float relativeAngle = util::RadiansToDeg(headingAngle - playerCameraAngle);

		if (std::abs(relativeAngle) < 20.0F) 
		{
			Test test{ GFxDisplayObject{ GetMovieView(), "_root.Test" } };

			test.textField0.SetText(a_enemy->GetName());
			test.textField1.SetText((std::string{ "Heading angle: " } + std::to_string(util::RadiansToDeg(headingAngle))).c_str());
			test.textField2.SetText((std::string{ "Relative angle: " } + std::to_string(relativeAngle)).c_str());
		}

		return true;
	}
}
