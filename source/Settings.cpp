#include "Settings.h"

#include "utils/INISettingCollection.h"

namespace settings
{
	using namespace utils;

	void Init(const std::string& a_iniFileName)
	{
		INISettingCollection* iniSettingCollection = INISettingCollection::GetSingleton();

		{
			using namespace debug;
			iniSettingCollection->AddSettings
			(
				MakeSetting("uLogLevel:Debug", static_cast<std::uint32_t>(logLevel))
			);
		}
		{
			using namespace display;
			iniSettingCollection->AddSettings
			(
				MakeSetting("bUseMetricUnits:Display", useMetricUnits),
				MakeSetting("bShowUnvisitedLocationMarkers:Display", showUnvisitedLocationMarkers),
				MakeSetting("bShowEnemyMarkers:Display", showEnemyMarkers),
				MakeSetting("bShowObjectiveAsTarget:Display", showObjectiveAsTarget),
				MakeSetting("bShowInteriorMarkers:Display", showInteriorMarkers)
			);
		}
		{
			using namespace questlist;
			iniSettingCollection->AddSettings
			(
				MakeSetting("fPositionY:QuestList", positionY),
				MakeSetting("fMaxHeight:QuestList", maxHeight),
				MakeSetting("bShowInExteriors:QuestList", showInExteriors),
				MakeSetting("bShowInInteriors:QuestList", showInInteriors),
				MakeSetting("fWalkingDelayToShow:QuestList", walkingDelayToShow),
				MakeSetting("fJoggingDelayToShow:QuestList", joggingDelayToShow),
				MakeSetting("fSprintingDelayToShow:QuestList", sprintingDelayToShow)
			);
		}

		if (!iniSettingCollection->ReadFromFile(a_iniFileName))
		{
			logger::warn("Could not read {}, falling back to default options", a_iniFileName);
		}

		{
			using namespace debug;
			logLevel = static_cast<logger::level>(iniSettingCollection->GetSetting<std::uint32_t>("uLogLevel:Debug"));
		}
		{
			using namespace display;
			showObjectiveAsTarget = iniSettingCollection->GetSetting<bool>("bShowObjectiveAsTarget:Display");
			showInteriorMarkers = iniSettingCollection->GetSetting<bool>("bShowInteriorMarkers:Display");
			useMetricUnits = iniSettingCollection->GetSetting<bool>("bUseMetricUnits:Display");
			showUnvisitedLocationMarkers = iniSettingCollection->GetSetting<bool>("bShowUnvisitedLocationMarkers:Display");
			showEnemyMarkers = iniSettingCollection->GetSetting<bool>("bShowEnemyMarkers:Display");
		}
		{
			using namespace questlist;
			positionY = iniSettingCollection->GetSetting<float>("fPositionY:QuestList");
			maxHeight = iniSettingCollection->GetSetting<float>("fMaxHeight:QuestList");
			showInExteriors = iniSettingCollection->GetSetting<bool>("bShowInExteriors:QuestList");
			showInInteriors = iniSettingCollection->GetSetting<bool>("bShowInInteriors:QuestList");
			walkingDelayToShow = iniSettingCollection->GetSetting<float>("fWalkingDelayToShow:QuestList");
			joggingDelayToShow = iniSettingCollection->GetSetting<float>("fJoggingDelayToShow:QuestList");
			sprintingDelayToShow = iniSettingCollection->GetSetting<float>("fSprintingDelayToShow:QuestList");
		}
	}
}