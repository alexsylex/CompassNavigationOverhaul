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

	}

}