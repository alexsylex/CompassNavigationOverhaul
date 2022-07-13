#include "Compass.h"

#include <numbers>

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
	Compass::FocusedQuestMarker::FocusedQuestMarker(std::uint32_t a_index, std::uint32_t a_gotoFrame, RE::TESObjectREFR* a_markerRef,
													float a_angleToPlayerCamera, const RE::TESQuest* a_quest,
													const RE::BGSInstancedQuestObjective* a_instancedObjective) :
		FocusedMarker{ a_index, a_gotoFrame, util::GetDistanceBetween(RE::PlayerCharacter::GetSingleton(), a_markerRef), a_angleToPlayerCamera },
		quest{ a_quest }, instancedObjective{ a_instancedObjective }
	{
		// A quest marker can reference to a character or a location
		switch (a_markerRef->GetFormType()) {
		case RE::FormType::Reference:
			{
				if (auto questRef = a_markerRef->As<RE::TESObjectREFR>()) {
					// If it is a teleport door, we can get the door at the other side
					if (auto teleportLinkedDoor = questRef->extraList.GetTeleportLinkedDoor().get()) {
						// First, try interior cell
						if (RE::TESObjectCELL* cell = teleportLinkedDoor->GetParentCell()) {
							questLocation = cell->GetName();
						}
						// Exterior cell
						else if (RE::TESWorldSpace* worldSpace = teleportLinkedDoor->GetWorldspace()) {
							questLocation = worldSpace->GetName();
						}
					}
				}
				break;
			}
		case RE::FormType::ActorCharacter:
			{
				if (auto questCharacter = a_markerRef->As<RE::Character>()) {
					questCharacterName = questCharacter->GetName();
				}
				break;
			}
		default:
			{
				logger::debug("Unknown quest marker type: {}", (int)a_markerRef->GetFormType());
				break;
			}
		}
	}

	bool Compass::ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef, 
									 std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCameraRads = util::GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_markerRef);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraRads);

		if (angleToPlayerCamera > 180.0F) 
		{
			angleToPlayerCamera = 360.0F - angleToPlayerCamera; 
		}

		if (angleToPlayerCamera < focusMarkerAngle)
		{
			for (RE::BGSInstancedQuestObjective& questObjective : RE::PlayerCharacter::GetSingleton()->objectives) 
			{
				if (questObjective.objective->ownerQuest == a_quest &&
					questObjective.instanceState == RE::QUEST_OBJECTIVE_STATE::kDisplayed)
				{
					focusedMarkers[focusedMarkersCount]  = FocusedQuestMarker
					{
						hudMarkerManager->currentMarkerIndex - 1,
						a_markerGotoFrame,
						a_markerRef,
						angleToPlayerCamera,
						a_quest,
						&questObjective
					};
					focusedMarkersCount++;
					break;
				}
			}
		}			

		return true;
	}

	Compass::FocusedLocationMarker::FocusedLocationMarker(std::uint32_t a_index, std::uint32_t a_gotoFrame, RE::TESObjectREFR* a_markerRef,
														  float a_angleToPlayerCamera, const RE::MapMarkerData* a_data) 
	:	FocusedMarker{ a_index, a_gotoFrame, util::GetDistanceBetween(RE::PlayerCharacter::GetSingleton(), a_markerRef), a_angleToPlayerCamera },
		data{ a_data }
	{ }

	bool Compass::ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef,
										std::uint32_t a_markerGotoFrame)
	{
		float angleToPlayerCameraRads = util::GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_markerRef);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraRads);

		if (angleToPlayerCamera > 180.0F) 
		{
			angleToPlayerCamera = 360.0F - angleToPlayerCamera; 
		}

		if (angleToPlayerCamera < focusMarkerAngle)
		{
			focusedMarkers[focusedMarkersCount] = FocusedLocationMarker
			{
				hudMarkerManager->currentMarkerIndex - 1,
				a_markerGotoFrame,
				a_markerRef,
				angleToPlayerCamera,
				a_mapMarker->mapData
			};
			focusedMarkersCount++;
		}

		return true;
	}

	bool Compass::ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t)
	{
		float angleToPlayerCameraRads = util::GetAngleBetween(RE::PlayerCamera::GetSingleton(), a_enemy);
		float angleToPlayerCamera = util::RadiansToDegrees(angleToPlayerCameraRads);

		if (angleToPlayerCamera < focusMarkerAngle || angleToPlayerCamera > (360.0F - focusMarkerAngle))
		{
			//if (!focusedMarker || focusedMarker->questType == RE::QUEST_DATA::Type::kNone) 
			//{
			//	focusedMarker = FocusedMarker{ .questName = a_enemy->GetName(), .relativeAngle = relativeAngle };
			//}
		}

		return true;
	}

	Compass::FocusedMarkerVariant* Compass::GetBestFocusedMarker()
	{
		float closestAngleToPlayerCamera = std::numeric_limits<float>::max();

		FocusedMarkerVariant* bestFocusedMarker = nullptr;

		for (int i = 0; i < focusedMarkersCount; i++) 
		{
			FocusedMarkerVariant* focusedMarker = &focusedMarkers[i];
			if (auto focusedQuestMarker = std::get_if<FocusedQuestMarker>(focusedMarker))
			{
				float angle = focusedQuestMarker->angleToPlayerCamera;

				if (angle < closestAngleToPlayerCamera)
				{
					bestFocusedMarker = focusedMarker;
					closestAngleToPlayerCamera = angle;
				}
			}
			else if (auto focusedLocationMarker = std::get_if<FocusedLocationMarker>(focusedMarker))
			{
				if (focusedLocationMarker->angleToPlayerCamera < closestAngleToPlayerCamera)
				{
					bestFocusedMarker = focusedMarker;
					closestAngleToPlayerCamera = focusedLocationMarker->angleToPlayerCamera;
				}
			}
		}

		return bestFocusedMarker;
	}

	void Compass::SetMarkersExtraInfo()
	{
		auto test = Test::GetSingleton();

		FocusedMarkerVariant* bestFocusedMarker = GetBestFocusedMarker();

		if (bestFocusedMarker)
		{
			if (auto bestFocusedQuestMarker = std::get_if<FocusedQuestMarker>(bestFocusedMarker)) 
			{
				test->textField0.SetText(bestFocusedQuestMarker->questName.c_str());

				SetQuestInfo(bestFocusedQuestMarker->questType, bestFocusedQuestMarker->questName,
							 bestFocusedQuestMarker->questObjective, bestFocusedQuestMarker->distanceToPlayer * 0.01428F);

				Update(bestFocusedQuestMarker->index);
			}
			else if (auto bestFocusedLocationMarker = std::get_if<FocusedLocationMarker>(bestFocusedMarker)) 
			{
				test->textField0.SetText("");

				SetLocationInfo(bestFocusedLocationMarker->locationName, bestFocusedLocationMarker->distanceToPlayer * 0.01428F);

				Update(bestFocusedLocationMarker->index);
			}

			focusedMarkersCount = 0;
		}
		else 
		{
			ClearQuestInfos();

			test->textField0.SetText("");
		}
	}

	void Compass::SetQuestInfo(RE::QUEST_DATA::Type a_questType, const std::string& a_questName, 
							   const std::string& a_questObjective, float a_distance)
	{
		Invoke("SetQuestInfo", a_questType, a_questName.c_str(), a_questObjective.c_str(), a_distance);
	}

	void Compass::ClearQuestInfos()
	{
		Invoke("ClearQuestInfos");
	}

	void Compass::SetLocationInfo(const std::string& a_locationName, float a_distance)
	{
		Invoke("SetLocationInfo", a_locationName.c_str(), a_distance);
	}

	void Compass::Update(std::uint32_t a_markerIndex)
	{
		Invoke("Update", a_markerIndex);
	}
}
