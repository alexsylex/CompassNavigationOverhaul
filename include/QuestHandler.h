#pragma once

#include "RE/T/TESQuest.h"

namespace HCN
{
	class QuestHandler
	{
	public:

		static QuestHandler* GetSingleton()
		{
			static QuestHandler inst;

			return &inst;
		}

		void Process(RE::TESQuest* a_quest, RE::TESObjectREFRPtr a_markerRef);

	private:

		RE::HUDMenu* hudMenu = skyrim_cast<RE::HUDMenu*>(RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME).get());
	};
}