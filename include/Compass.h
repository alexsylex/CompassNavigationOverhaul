#pragma once

#include "IUI/GFxArray.h"
#include "IUI/GFxDisplayObject.h"

namespace extended
{
	class Compass : public IUI::GFxDisplayObject
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

				Invoke("Compass");
			}
		}

		void SetUnits(bool a_useMetric)
		{
			Invoke("SetUnits", a_useMetric);
		}

		void SetMarkers()
		{
			Invoke("SetMarkers");
		}

		void SetFocusedMarkerInfo(const std::string_view& a_targetText, float a_distance,
								  float a_heightDifference, std::uint32_t a_markerIndex)
		{
			Invoke("SetFocusedMarkerInfo", a_targetText.data(), a_distance, a_heightDifference,
										   a_markerIndex);
		}

		void FocusMarker(std::uint32_t a_focusedMarkerIndex)
		{
			Invoke("FocusMarker", a_focusedMarkerIndex);
		}

		void UnfocusMarker()
		{
			Invoke("UnfocusMarker");
		}

		void UpdateFocusedMarker(std::uint32_t a_focusedMarkerIndex)
		{
			Invoke("UpdateFocusedMarker", a_focusedMarkerIndex);
		}

		void PostProcessMarkers(const std::unordered_map<std::uint32_t, bool>& a_unknownLocations, std::uint32_t a_markersCount)
		{
			GFxArray gfxIsUnknownLocations{ GetMovieView() };

			for (std::uint32_t i = 0; i < a_markersCount; i++)
			{
				gfxIsUnknownLocations.PushBack(a_unknownLocations.contains(i));
			}

			Invoke("PostProcessMarkers", gfxIsUnknownLocations);
		}

	private:

		Compass(const GFxDisplayObject& a_originalCompass) : GFxDisplayObject{ a_originalCompass }
		{}

		static inline Compass* singleton = nullptr;
	};
}
