#include "Hooks.h"

#include "utils/Logger.h"

#include "QuestHandler.h"

namespace HCN
{
	void VisitMembersForDebug(RE::GFxMovieView* a_view)
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
			RE::GFxValue root;
			if (a_view->GetVariable(&root, "_root")) 
			{
				logger::info("_root:");
				root.VisitMembers(&memberVisitor);
				logger::info("");

				RE::GFxValue test;
				if (a_view->GetVariable(&test, "_root.test")) 
				{
					logger::info("test:");
					test.VisitMembers(&memberVisitor);
					logger::info("");
				}

				RE::GFxValue widgetLoaderContainer;
				if (a_view->GetVariable(&widgetLoaderContainer, "_root.widgetLoaderContainer")) 
				{
					logger::info("WidgetContainer:");
					widgetLoaderContainer.VisitMembers(&memberVisitor);
					logger::info("");
				}
			}

			logger::flush();
		}
	};

	void PatchMovie(RE::GFxMovieView* a_viewOut, float a_deltaT, std::uint32_t a_frameCatchUpCount)
	{
		logger::info("BSScaleformManager -> load movie: {}", a_viewOut->GetMovieDef()->GetFileURL());
		logger::flush();

		RE::GFxValue hudMovieBaseInstance;
		if (a_viewOut->GetVariable(&hudMovieBaseInstance, "_root.HUDMovieBaseInstance")) 
		{
			RE::GFxValue args[2];

			args[0].SetString("test");
			args[1].SetNumber(-1000);

			if (!a_viewOut->Invoke("_root.createEmptyMovieClip", nullptr, args, 2)) 
			{
				logger::error("Something went wrong creating an empty movieclip");
			}

			// Paths are relative to the folder from where the GFxMovie was loaded
			// E.g. Interface/Exported/HUDMenu.gfx -> start in exported/
			// E.g. Interface/StartMenu.swf -> start in ./

			// Compass.swf does not work, movieclips inside do not get loaded for some reason...
			//args[0].SetString("../Compass.swf");
			args[0].SetString("widgets/skyui/activeeffects.swf");
			if (!a_viewOut->Invoke("_root.test.loadMovie", nullptr, args, 1)) 
			{
				logger::error("Something went wrong loading the movieclip");
			}
		}

		// After calling advance, the loadMovie() AS function is executed
		a_viewOut->Advance(a_deltaT, a_frameCatchUpCount);

		VisitMembersForDebug(a_viewOut);
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
