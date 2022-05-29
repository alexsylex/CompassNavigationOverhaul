#include "Hooks.h"

#include <numbers>

#include "utils/Logger.h"

#include "QuestHandler.h"
#include "IUI/GFxDisplayObject.h"

namespace HCN
{
	void InfinityUIMessageListener(SKSE::MessagingInterface::Message* a_msg)
	{
		enum Status
		{
			kPreLoad,
			kPosLoad,
			kAbortLoad
		};

		// DONE: Load position of the CompassShoutMeterHolder object (localToGlobal)
		// TODO: Get if HUDMovieBaseInstance had temperature meter (HUDMovieBaseInstance["TemperatureMeter_mc"] != undefined)
		// TODO: Get the index of the HUD element for replacement (HUDMovieBaseInstance.HudElements[i] == HUDMovieBaseInstance.CompassShoutMeterHolder)

		switch (a_msg->type) 
		{
		case Status::kPreLoad:
			{
				auto originalMember = static_cast<GFxDisplayObject*>(a_msg->data);
				RE::GPointF pos = originalMember->LocalToGlobal();

				logger::info("Message received ({}) from {}: DisplayObject position = ({}, {})",
					a_msg->type, a_msg->sender, pos.x, pos.y);
				break;
			}
		case Status::kPosLoad:
			{
				auto msgData = *static_cast<const char**>(a_msg->data);
				logger::info("Message received ({}) from {}: {}", a_msg->type, a_msg->sender, msgData);
				break;
			}
		case Status::kAbortLoad:
			{
				auto msgData = *static_cast<const char**>(a_msg->data);
				logger::info("Message received ({}) from {}: {}", a_msg->type, a_msg->sender, msgData);
				break;
			}
		default:
			break;
		}

		logger::flush();
	}

	bool ProcessQuestHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId, RE::TESQuest*& a_quest)
	{
		if (HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId)) 
		{
			RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

			RE::GFxMovieView* hudMenuMovieView = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME)->uiMovie.get();

			logger::trace("\"{}\" quest hooked", a_quest->GetName());

			RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

			RE::NiPoint3 markerPos = markerRef->GetLookingAtLocation();

			static constexpr float pi = std::numbers::pi_v<float>;

			static auto CropAngleRange = [](float& a_angle) -> void
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

			float playerCameraAngle = RE::PlayerCamera::GetSingleton()->yaw;

			float compassAngle = playerCameraAngle;

			if (RE::TESObjectCELL* parentCell = player->GetParentCell())
			{
				compassAngle += parentCell->GetNorthRotation();
			}

			float headingAngle = player->GetPosition().GetHorizontalAngleTo(markerPos);

			CropAngleRange(playerCameraAngle);
			CropAngleRange(compassAngle);
			CropAngleRange(headingAngle);

			IUI::GFxMovieClip _parent{ hudMenuMovieView, "_root.HUDMovieBaseInstance.CompassShoutMeterHolder._parent" };

			RE::GFxValue textField0 = _parent.GetMember("TextField0");
			RE::GFxValue textField1 = _parent.GetMember("TextField1");
			RE::GFxValue textField2 = _parent.GetMember("TextField2");

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

			switch (markerRef->GetFormType())
			{
				case RE::FormType::Reference:
				{
					if (auto questTeleporter = markerRef->As<RE::TESObjectREFR>()) 
					{
						questInfo += ", ";
						questInfo += questTeleporter->GetName();

						// TODO: Get the location where the marker teleports us to (this attempt returns nullptr)
						//auto teleportLocation = questTeleporter->extraList.GetByType<RE::ExtraLocationRefType>()->locRefType->As<RE::TESObjectREFR>();
						//questInfo += teleportLocation->GetName();
					}
					break;
				}
				case RE::FormType::ActorCharacter:
				{
					if (auto questCharacter = markerRef->As<RE::Character>()) 
					{
						questInfo += ", ";
						questInfo += questCharacter->GetName();
					}
					break;
				}
				default:
				{
					questInfo += ", Unknown quest marker type ";
					questInfo += std::to_string((int)markerRef->GetFormType());
					break;
				}
			}

			auto ToDegrees = [](float a_angle) -> float { return a_angle * 180.0F / pi; };

			//textField0.SetText(questInfo.c_str());
			textField1.SetText((std::string{ "Heading angle: " } + std::to_string(ToDegrees(headingAngle))).c_str());
			textField2.SetText((std::string{ "Relative angle: " } + std::to_string(ToDegrees(headingAngle - playerCameraAngle))).c_str());

			return true;
		}

		return false;
	}

	bool ProcessLocationHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		if (HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId)) 
		{
			RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);
			for (auto locationRef : a_hudMarkerManager->locationRefs) 
			{
				RE::TESObjectREFRPtr locationRefOut = RE::TESObjectREFR::LookupByHandle(locationRef);
				if (locationRefOut == markerRef)
				{
					logger::info("Marker found");
				}
			}

			if (markerRef && markerRef->GetFormType() == RE::FormType::Reference) 
			{
				if (RE::BSExtraData* extraData = markerRef->extraList.GetByType(RE::ExtraDataType::kMapMarker))
				{
					auto mapMarker = skyrim_cast<RE::ExtraMapMarker*>(extraData);
					
					logger::trace("\"{}\" location hooked", mapMarker->mapData->locationName.GetFullName());
					logger::flush();
				}
			}

			return true;
		}

		return false;
	}

	bool ProcessEnemyHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		if (HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId)) 
		{
			RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

			if (markerRef && markerRef->GetFormType() == RE::FormType::ActorCharacter)
			{
				if (auto enemy = markerRef->As<RE::Character>()) 
				{
					logger::trace("\"{}\" enemy hooked", enemy->GetName());
					logger::flush();
				}
			}

			return true;
		}

		return false;
	}

	bool ProcessPlayerSetHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		if (HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId)) 
		{
			RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

			logger::trace("Player-set marker hooked, {}/?", (int)markerRef->GetFormType());
			logger::flush();

			return true;
		}

		return false;
	}
}
