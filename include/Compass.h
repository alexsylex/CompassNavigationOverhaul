#pragma once

#include "utils/GFxArray.h"
#include "utils/GFxDisplayObject.h"

namespace extended
{
	class Compass : public GFxDisplayObject
	{
	public:
		static constexpr inline std::string_view path = "_level0.HUDMovieBaseInstance.CompassShoutMeterHolder.Compass";

		static void InitSingleton(const GFxDisplayObject& a_originalCompass)
		{
			if (!singleton)
			{
				static Compass singletonInstance{ a_originalCompass };
				singleton = &singletonInstance;
			}
		}

		static Compass* GetSingleton() { return singleton; }

		void SetupMod(const GFxDisplayObject& a_replaceCompass)
		{
			if (a_replaceCompass.HasMember("Compass"))
			{
				*static_cast<GFxDisplayObject*>(this) = a_replaceCompass;

				Invoke("Compass", hasTemperatureMeter);
			}
		}

		void SetUnits(bool a_useMetric)
		{
			Invoke("SetUnits", a_useMetric);
		}

		void SetMarkerInfo(const std::string& a_target, float a_distance, float a_heightDifference)
		{
			Invoke("SetMarkerInfo", a_target.c_str(), a_distance, a_heightDifference);
		}

		void FocusMarker(std::uint32_t a_markerIndex)
		{
			Invoke("FocusMarker", a_markerIndex);
		}

		void UnfocusMarker(std::uint32_t a_markerIndex)
		{
			Invoke("UnfocusMarker", a_markerIndex);
		}

		void UpdateMarker(std::uint32_t a_markerIndex)
		{
			Invoke("UpdateMarker", a_markerIndex);
		}

		void SetMarkersSize()
		{
			Invoke("SetMarkersSize");
		}

	private:

		Compass(const GFxDisplayObject& a_originalCompass) :
			GFxDisplayObject{ a_originalCompass },
			hasTemperatureMeter{ HasMember("CompassTemperatureHolderInstance") }
		{}

		static inline Compass* singleton = nullptr;

		bool hasTemperatureMeter;
	};
}
