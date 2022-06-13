#pragma once

#include "IUI/GFxArray.h"
#include "IUI/GFxDisplayObject.h"

#include "utils/Logger.h"

namespace HCN
{
	class Test : GFxDisplayObject
	{
	public:

		static constexpr inline std::string_view path = "_level0.Test";

		static Test* InitSingleton(const GFxDisplayObject& a_compassShoutMeterHolder)
		{
			if (!singleton) 
			{
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

		Test(const GFxDisplayObject& a_test) 
		:	GFxDisplayObject{ a_test }
		{ }

		static inline Test* singleton = nullptr;
	};

	class CompassShoutMeterHolder : public GFxDisplayObject
	{
	public:

		struct FocusedMarker
		{
			std::string name;
			float relativeAngle = 0.0F;
			float playerCameraAngle = 0.0F;
			float compassAngle = 0.0F;
			float headingAngle = 0.0F;
		};

		static constexpr inline std::string_view path = "_level0.HUDMovieBaseInstance.CompassShoutMeterHolder";

		static CompassShoutMeterHolder* InitSingleton(const GFxDisplayObject& a_compassShoutMeterHolder)
		{
			if (!singleton) 
			{
				// First time
				singleton = new CompassShoutMeterHolder(a_compassShoutMeterHolder);
			}

			return singleton;
		}

		static CompassShoutMeterHolder* UpdateSingleton(const GFxDisplayObject& a_compassShoutMeterHolder)
		{
			if (singleton) 
			{
				singleton->SetupMod(a_compassShoutMeterHolder);
			}

			return singleton;
		}

		static CompassShoutMeterHolder* GetSingleton() { return singleton; }

		float ProcessRelativeAngle(RE::TESObjectREFR* a_markerRef);
		bool ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef);
		bool ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef);
		bool ProcessEnemyMarker(RE::Character* a_enemy);
		void SetMarkersInfoEx();

	private:

		CompassShoutMeterHolder(const GFxDisplayObject& a_compassShoutMeterHolder) 
		:	GFxDisplayObject{ a_compassShoutMeterHolder },
			_parent{ GetMember("_parent") },
			_x{ GetMember("_x").GetNumber() },
			_y{ GetMember("_y").GetNumber() },
			hadTemperatureMeter{ !_parent.GetMember("TemperatureMeter_mc").IsUndefined() },
			hudElementIndex{ GFxArray(_parent.GetMember("HudElements")).FindElement(this) }
		{ }

		void SetupMod(const GFxDisplayObject& a_compassShoutMeterHolder)
		{
			if (a_compassShoutMeterHolder.HasMember("CompassShoutMeterHolder"))
			{
				*static_cast<GFxDisplayObject*>(this) = a_compassShoutMeterHolder;

				Invoke("CompassShoutMeterHolder", _x, _y, hadTemperatureMeter, hudElementIndex);
			}
		}

		static inline CompassShoutMeterHolder* singleton = nullptr;

		GFxDisplayObject _parent;
		double _x;
		double _y;
		bool hadTemperatureMeter;
		std::int32_t hudElementIndex;

		float tolerance = 10.0F;
		std::optional<FocusedMarker> focusedMarker;
	};
}
