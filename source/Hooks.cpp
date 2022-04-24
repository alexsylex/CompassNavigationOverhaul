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
		struct GFxMemberVisitor : RE::GFxValue::ObjectVisitor
		{
			void Visit(const char* a_name, const RE::GFxValue& a_gfxValue)
			{
				auto name_sv = std::string_view{ a_name };
				if (name_sv == "PlayReverse" || name_sv == "PlayForward") 
				{
					return;
				}

				using ValueType = RE::GFxValue::ValueType;

				auto ValueTypeToString = [](ValueType a_valueType) -> const char* const
				{
					switch (a_valueType) {
					case ValueType::kNull: return "Null";
					case ValueType::kBoolean: return "Boolean";
					case ValueType::kNumber: return "Number";
					case ValueType::kString: return "String";
					case ValueType::kStringW: return "StringW";
					case ValueType::kArray: return "Array";
					case ValueType::kObject: return "Object";
					case ValueType::kDisplayObject: return "DisplayObject";
					default: return "Undefined";
					}
				};

				logger::info("Found member: var {}: {}", a_name, ValueTypeToString(a_gfxValue.GetType()));
			}
		} memberVisitor;

		if (a_view)
		{
			RE::GFxValue member;
			if (a_view->GetVariable(&member, a_member)) 
			{
				logger::info("{}:", a_member);
				member.VisitMembers(&memberVisitor);
				logger::info("");

				logger::flush();
			}
		}
	};

	void PatchMovie(RE::GFxMovieView* a_view, float a_deltaT, std::uint32_t a_frameCatchUpCount)
	{
		std::string_view movieUrl = a_view->GetMovieDef()->GetFileURL();

		logger::info("BSScaleformManager -> load movie: {}", movieUrl);
		logger::flush();

		a_view->Advance(a_deltaT, a_frameCatchUpCount);

		RE::GFxValue hudMovieBaseInstance;
		if (a_view->GetVariable(&hudMovieBaseInstance, "_root.HUDMovieBaseInstance"))
		{
			VisitMembersForDebug(a_view, "_root");

			{
				IUI::SwfLoader swfloader(a_view, movieUrl);

				VisitMembersForDebug(a_view, "_root");

				if (a_view->IsAvailable("_root.swfloader")) 
				{
					VisitMembersForDebug(a_view, "_root.swfloader");

					if (swfloader.LoadMovieClip()) 
					{
						VisitMembersForDebug(a_view, "_root.swfloader");
					} 
					else 
					{
						logger::error("Something went wrong loading the movieclip");
						logger::flush();
					}
				}
			}

			VisitMembersForDebug(a_view, "_root");
			VisitMembersForDebug(a_view, "_root.swfloader");
		}
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
