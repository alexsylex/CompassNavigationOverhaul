#include "Hooks.h"

#include <numbers>

#include "utils/Logger.h"

#include "IUI/API.h"

#include "QuestHandler.h"

namespace HCN
{
	void InfinityUIMessageListener(SKSE::MessagingInterface::Message* a_msg)
	{
		if (!a_msg || std::string_view(a_msg->sender) != "Infinity UI") 
		{
			return;
		}

		const char* msgType = a_msg->type == IUI::API::kPreLoad	  ? "PreLoad" :
							  a_msg->type == IUI::API::kPostLoad  ? "PostLoad" :
							  a_msg->type == IUI::API::kAbortLoad ? "AbortLoad" :
																	"Unknown";

		logger::debug("{} message received", msgType);

		switch (a_msg->type) 
		{
		case IUI::API::kPreLoad:
		{
			if (auto preLoadMessage = IUI::API::TranslateAs<IUI::API::PreLoadMessage>(a_msg))
			{
				GFxDisplayObject compassShoutMeterHolder = *preLoadMessage->originalDisplayObject;

				// DONE: Load position of the CompassShoutMeterHolder object (localToGlobal)
				RE::GPointF pos = compassShoutMeterHolder.LocalToGlobal();

				logger::info("CompassShoutMeterHolder position = ({}, {})", pos.x, pos.y);

				GFxDisplayObject hudMovieBaseInstance = compassShoutMeterHolder.GetMember("_parent");

				// DONE: Get if HUDMovieBaseInstance had temperature meter (HUDMovieBaseInstance["TemperatureMeter_mc"] != undefined)
				if (hudMovieBaseInstance.GetMember("TemperatureMeter_mc").IsUndefined())
				{
					logger::info("HUDMovieBaseInstance had NOT a TemperatureMeter_mc");
				}
				else 
				{
					logger::info("HUDMovieBaseInstance had a TemperatureMeter_mc");
				}

				GFxArray hudElements = hudMovieBaseInstance.GetMember("HudElements");

				// DONE: Get the index of the HUD element for replacement (HUDMovieBaseInstance.HudElements[i] == HUDMovieBaseInstance.CompassShoutMeterHolder)
				std::uint32_t hudElementIndex = hudElements.FindElement(compassShoutMeterHolder);

				if (hudElementIndex != static_cast<std::uint32_t>(-1)) 
				{
					logger::info("CompassShoutMeterHolder found at index {}", hudElementIndex);
				}
				else 
				{
					logger::info("Could not find CompassShoutMeterHolder in HudElements array");
				}
			}
			break;
		}
		case IUI::API::Status::kPostLoad:
		{
			if (auto postLoadMessage = IUI::API::TranslateAs<IUI::API::PostLoadMessage>(a_msg))
			{
				auto msgData = postLoadMessage->payload;
				logger::info("{}", msgData);
			}
			break;
		}
		case IUI::API::Status::kAbortLoad:
		{
			if (auto abortLoadMessage = IUI::API::TranslateAs<IUI::API::AbortLoadMessage>(a_msg))
			{
				RE::GFxValue originalValue = *abortLoadMessage->originalValue;
				logger::warn("Aborted replacement of {}", originalValue.ToString());
			}
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

			GFxDisplayObject _parent{ hudMenuMovieView, "_root.HUDMovieBaseInstance.CompassShoutMeterHolder._parent" };

			GFxDisplayObject textField0 = _parent.GetMember("TextField0");
			GFxDisplayObject textField1 = _parent.GetMember("TextField1");
			GFxDisplayObject textField2 = _parent.GetMember("TextField2");

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
