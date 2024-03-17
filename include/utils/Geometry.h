#pragma once

#include <numbers>

namespace util
{
	static constexpr float pi = std::numbers::pi_v<float>;

	constexpr void CropAngleRange(float& a_angle)
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

	constexpr float RadiansToDegrees(float a_angle)
	{
		return a_angle * 180.0F / pi;
	}

	inline float GetAngleBetween(const RE::PlayerCamera* a_playerCamera, const RE::TESObjectREFR* a_markerRef)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		RE::NiPoint3 playerPos = player->GetPosition();

		// Skyrim used this instead, but less precise (don't know why)
		//RE::NiPoint3 markerPos = a_markerRef->GetLookingAtLocation();

		RE::NiPoint3 markerPos = a_markerRef->GetPosition();

		float playerCameraYawAngle = a_playerCamera->GetRuntimeData2().yaw;

		float compassAngle = playerCameraYawAngle;

		if (RE::TESObjectCELL* parentCell = player->GetParentCell())
		{
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

	inline RE::NiPoint3 GetRealPosition(const RE::TESObjectREFR* a_objRef)
	{
		RE::NiPoint3 position = a_objRef->GetPosition();

		if (const RE::TESWorldSpace* worldSpace = a_objRef->GetWorldspace())
		{
			RE::NiPoint3 worldSpaceOffset{ worldSpace->worldMapOffsetData.mapOffsetX,
												  worldSpace->worldMapOffsetData.mapOffsetY,
												  worldSpace->worldMapOffsetData.mapOffsetZ };

			position += worldSpaceOffset * worldSpace->worldMapOffsetData.mapScale;
		}

		return position;
	}

	inline float GetDistanceBetween(const RE::PlayerCharacter* a_player, const RE::TESObjectREFR* a_marker)
	{
		RE::NiPoint3 playerPos = GetRealPosition(a_player);
		RE::NiPoint3 markerPos = GetRealPosition(a_marker);

		return playerPos.GetDistance(markerPos);
	}

	inline float GetHeightDifferenceBetween(const RE::PlayerCharacter* a_player, const RE::TESObjectREFR* a_marker)
	{
		RE::NiPoint3 playerPos = GetRealPosition(a_player);
		RE::NiPoint3 markerPos = GetRealPosition(a_marker);

		return markerPos.z - playerPos.z;
	}
}