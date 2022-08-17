#pragma once

namespace SKSE::log
{
	using level = spdlog::level::level_enum;
}
namespace logger = SKSE::log;

namespace settings
{
	struct Collection
	{
		struct Debug
		{
			logger::level logLevel;
		};

		struct Display
		{
			bool showObjectiveAsTarget;
			bool showInteriorMarkers;
			bool useMetricUnits;
			bool showUnvisitedLocationMarkers;
			bool showEnemyMarkers;
			bool showQuestListInInteriors;
		};

		Debug debug;
		Display display;
	};

	void Init(const std::string& a_iniFileName);
}

inline settings::Collection g_settings;