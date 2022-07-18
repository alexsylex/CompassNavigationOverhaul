#include "FocusedMarker.h"

FocusedQuestMarker::FocusedQuestMarker(std::uint32_t a_index, std::uint32_t a_gotoFrame, RE::TESObjectREFR* a_markerRef,
									   float a_angleToPlayerCamera, const RE::TESQuest* a_quest,
	const RE::BGSInstancedQuestObjective* a_instancedObjective) :
	FocusedMarker{ a_markerRef, a_index, a_gotoFrame, util::GetDistanceBetween(RE::PlayerCharacter::GetSingleton(), a_markerRef), a_angleToPlayerCamera },
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
			//logger::debug("Unknown quest marker type: {}", (int)a_markerRef->GetFormType());
			break;
		}
	}
}

FocusedLocationMarker::FocusedLocationMarker(std::uint32_t a_index, std::uint32_t a_gotoFrame, RE::TESObjectREFR* a_markerRef,
											 float a_angleToPlayerCamera, const RE::MapMarkerData* a_data) :
	FocusedMarker{ a_markerRef, a_index, a_gotoFrame, util::GetDistanceBetween(RE::PlayerCharacter::GetSingleton(), a_markerRef), a_angleToPlayerCamera },
	data{ a_data }
{}