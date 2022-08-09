#include "Plugin.h"
#include "Hooks.h"
#include "Settings.h"

#include "utils/Logger.h"

static constexpr Plugin plugin{ "CompassNavigationOverhaul" };

void SKSEMessageListener(SKSE::MessagingInterface::Message* a_msg);

#if BUILD_SE
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = plugin.name;
	a_info->version = a_info->version = (static_cast<std::uint8_t>(plugin.versionMajor) << 24) |
										(static_cast<std::uint8_t>(plugin.versionMinor) << 16) |
										(static_cast<std::uint8_t>(plugin.versionPatch) << 8);

	if (a_skse->IsEditor())
	{
		logger::critical("Loaded in editor, marking as incompatible");
		return false;
	}

	REL::Version runtimeVersion = a_skse->RuntimeVersion();

	if (runtimeVersion < SKSE::RUNTIME_1_5_39 || runtimeVersion > SKSE::RUNTIME_1_5_97)
	{
		logger::critical("Unsupported runtime version {}", runtimeVersion.string());
		return false;
	}

	return true;
}
#else
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() constexpr -> SKSE::PluginVersionData 
{
	SKSE::PluginVersionData v;

	v.PluginVersion(REL::Version{ plugin.versionMajor, plugin.versionMinor, plugin.versionPatch });
	v.PluginName(plugin.name);

	v.UsesAddressLibrary(true);

	return v;
}();
#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	using namespace utils;

	if (!logger::init(plugin.name))
	{
		return false;
	}

	logger::info("Loading {} {}.{}...", plugin.name, plugin.versionMajor, plugin.versionMinor);

	SKSE::Init(a_skse);

	settings::Init(std::string(plugin.fileName) + ".ini");

	logger::set_level(g_settings.debug.logLevel, g_settings.debug.logLevel);

	if (!SKSE::GetMessagingInterface()->RegisterListener("SKSE", SKSEMessageListener))
	{
		return false;
	}

	hooks::Install();

	logger::set_level(logger::level::info, logger::level::info);
	logger::info("Succesfully loaded!");

	logger::set_level(g_settings.debug.logLevel, g_settings.debug.logLevel);

	return true;
}
