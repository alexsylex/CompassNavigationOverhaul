#include "Hooks.h"

#include "utils/Logger.h"

#include "QuestHandler.h"
#include "GFxMoviePatcher.h"

namespace HCN
{
	void ProcessMoviePatcher(RE::GFxMovieView* a_view)
	{
		IUI::GFxMoviePatcher moviePatcher(a_view, a_view->GetMovieDef()->GetFileURL());

		IUI::VisitMembersForDebug(a_view, "_root");
		IUI::VisitMembersForDebug(a_view, "_root.HUDMovieBaseInstance.CompassShoutMeterHolder");

		if (int loadedSwfPatches = moviePatcher.LoadAvailableSwfPatches()) 
		{
			std::string fmtMessage = "Loaded {} swf patch";
			fmtMessage += loadedSwfPatches > 1 ? "es" : "";
			fmtMessage += " for {}";

			logger::info(fmtMessage, loadedSwfPatches, moviePatcher.GetMovieFilename());
			logger::flush();
				
			logger::info("");

			IUI::VisitMembersForDebug(a_view, "_root.HUDMovieBaseInstance.CompassShoutMeterHolder");
		}
	}

	void PatchGFxMovie(RE::GFxMovieView* a_view, float a_deltaT, std::uint32_t a_frameCatchUpCount)
	{
		std::string_view movieUrl = a_view->GetMovieDef()->GetFileURL();

		logger::trace("Detected GFx movie load from {}", movieUrl);

		a_view->Advance(a_deltaT, a_frameCatchUpCount);

		if (a_view->IsAvailable("_root.HUDMovieBaseInstance"))
		{
			ProcessMoviePatcher(a_view);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Test(RE::GFxMovieView* a_view)
	{
		//IUI::VisitMembersForDebug(a_view, "_root.HUDMovieBaseInstance");
		//IUI::VisitMembersForDebug(a_view, "_root.HUDMovieBaseInstance.CompassShoutMeterHolder");

		IUI::GFxMovieClip hudMovieBaseInstance{ a_view, "_root.HUDMovieBaseInstance" };
		IUI::GFxMovieClip compassShoutMeterHolder = hudMovieBaseInstance.GetMember("CompassShoutMeterHolder");

		//if (!compassShoutMeterHolder.IsUndefined())
		//{
		//	IUI::GFxMemberVisitor memberVisitor;
		//	compassShoutMeterHolder.VisitMembers(&memberVisitor);
		//}
		//else 
		//{
		//	logger::info("CompassShoutMeterHolder is undefined, address of HUDMovieBaseInstance 0x{:08x}", ptr[0x10 / sizeof(std::uintptr_t)]);
		//}
	}

	bool ProcessQuestHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId, RE::TESQuest*& a_quest)
	{
		RE::TESObjectREFRPtr markerRef = RE::TESObjectREFR::LookupByHandle(a_refHandle);

		Test(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME)->uiMovie.get());

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
