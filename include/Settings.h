#pragma once

namespace SKSE::log
{
	using level = spdlog::level::level_enum;
}
namespace logger = SKSE::log;

namespace settings
{
	void Init(const std::string& a_iniFileName);

	// Default values

	namespace debug
	{
		inline logger::level logLevel = logger::level::err;
	}

	namespace display
	{
		inline bool useMetricUnits = false;
		inline bool showUndiscoveredLocationMarkers = true;
		inline bool undiscoveredMeansUnknownMarkers = true;
		inline bool undiscoveredMeansUnknownInfo = true;
		inline bool showEnemyMarkers = true;
		inline bool showObjectiveAsTarget = true;
		inline bool showInteriorMarkers = true;
		inline float angleToShowMarkerDetails = 10.0F;
		inline float angleToKeepMarkerDetailsShown = 35.0F;
	}

	namespace questlist
	{
		inline float positionX = 0.008F;
		inline float positionY = 0.125F;
		inline float maxHeight = 0.5F;
		inline bool showInExteriors = true;
		inline bool showInInteriors = true;
		inline float walkingDelayToShow = 0.0F;
		inline float joggingDelayToShow = 1.0F;
		inline float sprintingDelayToShow = 1.5F;
		inline bool hideInCombat = false;
	}
}