#pragma once

#include <numbers>

namespace util
{
	static constexpr float pi = std::numbers::pi_v<float>;

	constexpr void CropAngleRange(float& a_angle)
	{
		if (a_angle <= 2 * pi) {
			if (a_angle < 0.0F) {
				a_angle = std::fmodf(a_angle, 2 * pi) + 2 * pi;
			}
		} else {
			a_angle = std::fmodf(a_angle, 2 * pi);
		}
	};

	constexpr float RadiansToDegrees(float a_angle)
	{
		return a_angle * 180.0F / pi;
	}

	inline float GetAngleBetween(RE::PlayerCamera* a_playerCamera, RE::TESObjectREFR* a_markerRef)
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

	inline float GetDistanceBetween(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_markerRef)
	{
		RE::NiPoint3 playerPos = a_player->GetPosition();
		RE::NiPoint3 markerPos = a_markerRef->GetPosition();
		//GetLookingAtLocation();
		return playerPos.GetDistance(markerPos);
	}
}