#pragma once

#include "IUI/GFxArray.h"
#include "IUI/GFxDisplayObject.h"

#include "RE/H/HUDMarkerManager.h"

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

namespace extended
{
	class Compass : public GFxDisplayObject
	{
	public:

		struct FocusedMarker
		{
			std::uint32_t index;
			std::uint32_t gotoFrame;
			float distanceToPlayer;
			float angleToPlayerCamera;
			bool markedForDelete = false;
		};

		struct FocusedQuestMarker : FocusedMarker
		{
			FocusedQuestMarker(std::uint32_t a_index, std::uint32_t a_gotoFrame, RE::TESObjectREFR* a_markerRef, float a_angleToPlayerCamera,
							   const RE::TESQuest* a_quest, const RE::BGSInstancedQuestObjective* a_instancedObjective);

			const RE::TESQuest* quest;
			const RE::BGSInstancedQuestObjective* instancedObjective;

			// cache
			RE::QUEST_DATA::Type questType = quest->GetType();
			std::string questName = (questType == RE::QUEST_DATA::Type::kMiscellaneous) ? "$MISCELLANEOUS" : quest->GetName();
			std::string questDescription = quest->GetCurrentDescriptionWithReplacedTags().c_str();
			std::string questObjective = instancedObjective->GetDisplayTextWithReplacedTags().c_str();
			std::string questLocation;
			std::string questCharacterName;
		};

		struct FocusedLocationMarker : FocusedMarker
		{
			FocusedLocationMarker(std::uint32_t a_index, std::uint32_t a_gotoFrame, RE::TESObjectREFR* a_markerRef, float a_angleToPlayerCamera,
								  const RE::MapMarkerData* a_data);

			const RE::MapMarkerData* data;

			// cache
			std::string locationName = data->locationName.GetFullName();
		};

		using FocusedMarkerVariant = std::variant<std::monostate, FocusedLocationMarker, FocusedQuestMarker>;

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

		static constexpr inline std::size_t maxNumberOfMarkers = std::extent<decltype(RE::HUDMarkerManager::scaleformMarkerData)>::value;

		static Compass* InitSingleton(const GFxDisplayObject& a_originalCompass)
		{
			if (!singleton)
			{
				static Compass singletonInstance{ a_originalCompass };
				singleton = &singletonInstance;
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

		bool ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef,
								std::uint32_t a_markerGotoFrame);

		bool ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef,
								   std::uint32_t a_markerGotoFrame);

		bool ProcessEnemyMarker(RE::Character* a_enemy, std::uint32_t a_markerGotoFrame);

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

		FocusedMarkerVariant* GetBestFocusedMarker();

		void SetQuestInfo(RE::QUEST_DATA::Type a_questType, const std::string& a_questName, 
						  const std::string& a_questObjective, float a_distance);

		void ClearQuestInfos();

		void SetLocationInfo(const std::string& a_locationName, float a_distance);

		void Update(std::uint32_t a_markerIndex);

		static inline Compass* singleton = nullptr;

		double _x;
		double _y;
		bool hasTemperatureMeter;

		static constexpr inline float focusMarkerAngle = 20.0F;
		float timeFocusingMarker = 0.0F;

		std::array<FocusedMarkerVariant, maxNumberOfMarkers> focusedMarkers;
		std::size_t focusedMarkersCount = 0;

		RE::HUDMarkerManager* hudMarkerManager = RE::HUDMarkerManager::GetSingleton();
	};
}
