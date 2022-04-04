#include "QuestHandler.h"

#include "utils/Logger.h"

namespace HCN
{
	RE::GFxMovieRoot* GetGFxMovieRoot(RE::GFxValue& a_root)
	{
		return **reinterpret_cast<RE::GFxMovieRoot***>(&a_root);
	}

	//void QuestHandler::Process(RE::TESQuest* a_quest, RE::TESObjectREFRPtr a_markerRef)
	void QuestHandler::Process(RE::TESQuest*, RE::TESObjectREFRPtr)
	{
		//logger::info("\"{}\" quest hooked", a_quest->GetName());

		//auto questTeleporter = a_markerRef.get();
		//logger::info("\"{}\" quest teleporter hooked", questTeleporter->GetName());

		RE::GFxMovieRoot* movieRoot = GetGFxMovieRoot(hudMenu->root);

		if (movieRoot) 
		{
			if (!hudMenu->root.HasMember("CompassShoutMeterHolder")) 
			{
				//logger::info("HUDInstanceMovie does not have a CompassShoutMeterHolder");
			}
			else 
			{
				//static bool loaded = false;

				//RE::GFxValue compassShoutMeterHolder;
				//hudMenu->root.GetMember("CompassShoutMeterHolder", &compassShoutMeterHolder);

				////static_cast<RE::GFxMovie*>(movieRoot)->GetVariable(&compassShoutMeterHolder, "_root.HUDMovieBaseInstance.CompassShoutMeterHolder");

				//bool hasCompass = compassShoutMeterHolder.HasMember("Compass");
				//logger::info("CompassShoutMeterHolder {} the Compass", hasCompass ? "holds" : "does not hold");
			}
		}
	}

}