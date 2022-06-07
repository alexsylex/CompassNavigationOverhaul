#pragma once

#include "IUI/GFxArray.h"
#include "IUI/GFxDisplayObject.h"

namespace HCN
{
	class CompassShoutMeterHolder : public GFxDisplayObject
	{
	public:

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

		static CompassShoutMeterHolder* GetSingleton()
		{
			return singleton;
		}

		std::string GetPathToMember() { return pathToMember; }

		std::tuple<float, float, float> ProcessRelativeAngle(RE::TESObjectREFR* a_markerRef);

		bool ProcessQuestMarker(RE::TESQuest* a_quest, RE::TESObjectREFR* a_markerRef);

		bool ProcessLocationMarker(RE::ExtraMapMarker* a_mapMarker, RE::TESObjectREFR* a_markerRef);

		bool ProcessEnemyMarker(RE::Character* a_enemy);

	private:

		CompassShoutMeterHolder(const GFxDisplayObject& a_compassShoutMeterHolder) 
		:	GFxDisplayObject{ a_compassShoutMeterHolder },
			pathToMember{ ToString().c_str() },
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
				*this = a_compassShoutMeterHolder;

				Invoke("CompassShoutMeterHolder", _x, _y, hadTemperatureMeter, hudElementIndex);
			}
		}

		static inline CompassShoutMeterHolder* singleton = nullptr;

		std::string pathToMember;
		GFxDisplayObject _parent;
		double _x;
		double _y;
		bool hadTemperatureMeter;
		std::int32_t hudElementIndex;
	};
}