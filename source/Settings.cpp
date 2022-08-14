#include "Settings.h"

#include "utils/INISettingCollection.h"

namespace settings
{
	struct DefaultCollection : Collection
	{
		DefaultCollection() :
			Collection
			{
				.debug
				{
					.logLevel = logger::level::err
				},
				.display
				{
					.showObjectiveAsTarget = true,
					.showInteriorMarkers = true,
					.useMetricUnits = false,
					.showUndiscoveredLocationMarkers = true
				}
			}
		{ }
	};
}

static const settings::DefaultCollection g_defaultSettings;

namespace settings
{
	using namespace utils;

	void Init(const std::string& a_iniFileName)
	{
		INISettingCollection* iniSettingCollection = INISettingCollection::GetSingleton();

		iniSettingCollection->AddSettings
		(
			// Debug
			MakeSetting("uLogLevel:Debug", static_cast<std::uint32_t>(g_defaultSettings.debug.logLevel)),

			// Display
			MakeSetting("bShowObjectiveAsTarget:Display", g_defaultSettings.display.showObjectiveAsTarget),
			MakeSetting("bShowInteriorMarkers:Display", g_defaultSettings.display.showInteriorMarkers),
			MakeSetting("bUseMetricUnits:Display", g_defaultSettings.display.useMetricUnits),
			MakeSetting("bShowUndiscoveredLocationMarkers:Display", g_defaultSettings.display.showUndiscoveredLocationMarkers)
		);

		if (!iniSettingCollection->ReadFromFile(a_iniFileName))
		{
			logger::warn("Could not read {}, falling back to default options", a_iniFileName);
		}

		// Debug
		g_settings.debug.logLevel = static_cast<logger::level>(iniSettingCollection->GetSetting<std::uint32_t>("uLogLevel:Debug"));

		// Display
		g_settings.display.showObjectiveAsTarget = iniSettingCollection->GetSetting<bool>("bShowObjectiveAsTarget:Display");
		g_settings.display.showInteriorMarkers = iniSettingCollection->GetSetting<bool>("bShowInteriorMarkers:Display");
		g_settings.display.useMetricUnits = iniSettingCollection->GetSetting<bool>("bUseMetricUnits:Display");
		g_settings.display.showUndiscoveredLocationMarkers = iniSettingCollection->GetSetting<bool>("bShowUndiscoveredLocationMarkers:Display");
	}
}