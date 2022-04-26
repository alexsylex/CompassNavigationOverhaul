#include "Hooks.h"

#include "utils/Logger.h"

#include "QuestHandler.h"
#include "SwfLoader.h"

namespace HCN
{
	// 1. Create instance of swfloader by loading swfloader.swf
	// 2. Check for folders in "Data/Interface/" named as the loaded file
	// 3. Go through the folders until a .swf is found. The path to the .swf file
	//    is the path to the member to create/replace
	// 4a. If the member does not exist, createEmptyMovieClip -> loadMovie
	// 4b. If the member does exist, unloadMovie -> loadMovie

	static void VisitMembersForDebug(RE::GFxMovieView* a_view, const char* a_member)
	{
		IUI::GFxMemberVisitor memberVisitor;

		if (a_view)
		{
			RE::GFxValue member;
			if (a_view->GetVariable(&member, a_member)) 
			{
				logger::debug("{}", a_member);
				logger::debug("{");
				member.VisitMembers(&memberVisitor);
				logger::debug("}");
				logger::flush();
			}
		}
	};

	void PatchGFxMovie(RE::GFxMovieView* a_view, float a_deltaT, std::uint32_t a_frameCatchUpCount)
	{
		std::string_view movieUrl = a_view->GetMovieDef()->GetFileURL();

		logger::info("Detected GFx movie load from {}", movieUrl);
		logger::flush();

		{
			IUI::SwfLoader swfloader(a_view, movieUrl);

			if (a_view->IsAvailable("_root.swfloader")) 
			{
				if (int loadedMovieClipPatches = swfloader.LoadAvailableMovieClipPatches())
				{
					std::string fmtMessage = "Loaded {} movieclip patch";
					fmtMessage += loadedMovieClipPatches > 1 ? "es" : "";
					fmtMessage += " for {}";

					logger::info(fmtMessage, loadedMovieClipPatches, swfloader.GetMovieFilename());
				}
				else 
				{
					logger::info("Could not load any movieclip patches for {}", swfloader.GetMovieFilename());
				}
				logger::flush();
			}
		}

		VisitMembersForDebug(a_view, "_root");
		VisitMembersForDebug(a_view, "_root.swfloader");

		a_view->Advance(a_deltaT, a_frameCatchUpCount);
	}

	bool ProcessQuestHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId, RE::TESQuest*& a_quest)
	{
		RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

		if (markerRef->GetFormType() == RE::FormType::Reference)
		{
			QuestHandler::GetSingleton()->Process(a_quest, markerRef);
		}
		else
		{
			//logger::info("Unknown quest marker type hooked, {}/{}", (int)markerRef->GetFormType(), (int)RE::FormType::Reference);
			//logger::flush();
		}

		return HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId);
	}

	bool ProcessLocationHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

		if (markerRef && markerRef->GetFormType() == RE::FormType::Reference) 
		{
			if (RE::BSExtraData* extraData = markerRef->extraList.GetByType(RE::ExtraDataType::kMapMarker))
			{
				//auto mapMarker = skyrim_cast<RE::ExtraMapMarker*>(extraData);
				//
				//logger::info("\"{}\" location hooked", mapMarker->mapData->locationName.GetFullName());
				//logger::flush();
			}
		}

		return HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId);
	}

	bool ProcessEnemyHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

		if (markerRef && markerRef->GetFormType() == RE::FormType::ActorCharacter)
		{
			if (auto character = markerRef->As<RE::Character>()) 
			{
				//logger::info("\"{}\" enemy hooked", character->GetName());
				//logger::flush();
			}
		}

		return HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId);
	}

	bool ProcessPlayerSetHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId)
	{
		RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

		//logger::info("Player-set marker hooked, {}/?", (int)markerRef->GetFormType());
		//logger::flush();

		return HUDMarkerManager::UpdateHUDMarker(a_hudMarkerManager, a_markerData, a_pos, a_refHandle, a_markerId);
	}
}
