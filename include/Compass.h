#pragma once

#include "IUI/GFxArray.h"
#include "IUI/GFxDisplayObject.h"

namespace HCN
{
	class Test : GFxDisplayObject
	{
	public:
		static constexpr inline std::string_view path = "_level0.Test";

		static Test* InitSingleton(const GFxDisplayObject& a_compassShoutMeterHolder)
		{
			if (!singleton) {
				// First time
				singleton = new Test(a_compassShoutMeterHolder);
			}

			return singleton;
		}

		static Test* GetSingleton() { return singleton; }

		GFxDisplayObject textField0 = GetMember("TextField0");
		GFxDisplayObject textField1 = GetMember("TextField1");
		GFxDisplayObject textField2 = GetMember("TextField2");

	private:
		Test(const GFxDisplayObject& a_test) :
			GFxDisplayObject{ a_test }
		{}

		static inline Test* singleton = nullptr;
	};
}

namespace HCN::extended
{
	class Compass : public GFxDisplayObject
	{
	public:

		struct FocusedMarker
		{
			std::int32_t id; 
			float relativeAngle = 0.0F;
			float timeShown = 0.0F;
			bool markedForDelete = false;
		};

		struct FocusedLocationMarker : FocusedMarker
		{
			std::string locationName;
		};

		struct FocusedQuestMarker : FocusedMarker
		{
			const RE::TESQuest* quest;
			RE::QUEST_DATA::Type questType = RE::QUEST_DATA::Type::kNone;
			std::string questName;
			std::string questObjective;
			std::string questLocation;
			std::string questCharacterName;
		};

		template<typename Key, typename MappedValue>
		struct Map : std::unordered_map<Key, MappedValue>
		{
			using parent = std::unordered_map<Key, MappedValue>;

			MappedValue* GetAt(Key a_key)
			{
				if (!parent::contains(a_key))
				{
					parent::emplace(std::make_pair(a_key, MappedValue()));
				}

				MappedValue& mappedValue = parent::at(a_key);
				return &mappedValue;
			}

			parent::size_type EraseElementsMarkedForDelete()
			{
				return std::erase_if(*this, [](const auto& a_pair) -> bool
				{
					const auto& [key, mappedValue] = a_pair;

					return mappedValue.markedForDelete;
				});
			}
		};

		static constexpr inline std::string_view path = "_level0.HUDMovieBaseInstance.CompassShoutMeterHolder.Compass";

		static Compass* InitSingleton(const GFxDisplayObject& a_originalCompass)
		{
			if (!singleton) 
			{
				// First time
				singleton = new Compass(a_originalCompass);
			}

			return singleton;
		}

		static Compass* UpdateSingleton(const GFxDisplayObject& a_replaceCompass)
		{
			if (singleton) 
			{
				singleton->SetupMod(a_replaceCompass);
			}

			return singleton;
		}

		static Compass* GetSingleton() { return singleton; }

		float ProcessRelativeAngle(RE::TESObjectREFR* a_markerRef);
		bool ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef, std::int32_t a_markerId);
		bool ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef, std::int32_t a_markerId);
		bool ProcessEnemyMarker(RE::Character* a_enemy, std::int32_t a_markerId);
		void SetMarkersExtraInfo();

	private:

		Compass(const GFxDisplayObject& a_originalCompass) 
		:	GFxDisplayObject{ a_originalCompass },
			_x{ GetMember("_x").GetNumber() },
			_y{ GetMember("_y").GetNumber() },
			hasTemperatureMeter{ HasMember("CompassTemperatureHolderInstance") }
		{ }

		void SetupMod(const GFxDisplayObject& a_replaceCompass)
		{
			if (a_replaceCompass.HasMember("Compass"))
			{
				*static_cast<GFxDisplayObject*>(this) = a_replaceCompass;

				Invoke("Compass", _x, _y, hasTemperatureMeter);
			}
		}

		void SetQuestInfo(RE::QUEST_DATA::Type a_questType, const std::string& a_questName, const std::string& a_questObjective);

		void ClearQuestInfos();

		void SetLocationInfo(const std::string& a_locationName);

		static inline Compass* singleton = nullptr;

		double _x;
		double _y;
		bool hasTemperatureMeter;

		float tolerance = 10.0F;
		Map<RE::BGSInstancedQuestObjective*, FocusedQuestMarker> focusedQuestMarkerMap;
		Map<RE::ExtraMapMarker*, FocusedLocationMarker> focusedLocationMarkerMap;
	};
}
