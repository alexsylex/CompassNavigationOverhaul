#pragma once

#include "IUI/GFxArray.h"
#include "IUI/GFxDisplayObject.h"

#include "utils/GFxVisitors.h"

#include "utils/Logger.h"

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
			std::string name;
			float relativeAngle = 0.0F;
			float playerCameraAngle = 0.0F;
			float compassAngle = 0.0F;
			float headingAngle = 0.0F;
			RE::QUEST_DATA::Type questType = RE::QUEST_DATA::Type::kNone;
		};

		static constexpr inline std::string_view path = "_level0.HUDMovieBaseInstance.CompassShoutMeterHolder.Compass";

		static Compass* InitSingleton(const GFxDisplayObject& a_compass)
		{
			if (!singleton) 
			{
				// First time
				singleton = new Compass(a_compass);
			}

			return singleton;
		}

		static Compass* UpdateSingleton(const GFxDisplayObject& a_compass)
		{
			if (singleton) 
			{
				singleton->SetupMod(a_compass);
			}

			return singleton;
		}

		static Compass* GetSingleton() { return singleton; }

		float ProcessRelativeAngle(RE::TESObjectREFR* a_markerRef);
		bool ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef);
		bool ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef);
		bool ProcessEnemyMarker(RE::Character* a_enemy);
		void SetMarkersExtraInfo();

	private:

		Compass(const GFxDisplayObject& a_compass) 
		:	GFxDisplayObject{ a_compass },
			_x{ GetMember("_x").GetNumber() },
			_y{ GetMember("_y").GetNumber() },
			hadTemperatureMeter{ !GetMember("CompassTemperatureHolderInstance").IsUndefined() }
		{ }

		void SetupMod(const GFxDisplayObject& a_compass)
		{
			if (a_compass.HasMember("Compass"))
			{
				*static_cast<GFxDisplayObject*>(this) = a_compass;

				Invoke("Compass", _x, _y, hadTemperatureMeter);
			}
		}

		void SetQuestInfo(const std::string& a_questTitle, RE::QUEST_DATA::Type a_questType);

		static inline Compass* singleton = nullptr;

		double _x;
		double _y;
		bool hadTemperatureMeter;

		float tolerance = 10.0F;
		std::optional<FocusedMarker> focusedMarker;
	};
}
