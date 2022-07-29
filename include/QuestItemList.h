#pragma once

#include "utils/GFxArray.h"
#include "utils/GFxDisplayObject.h"

class QuestItemList : public GFxDisplayObject
{
public:

	static constexpr inline std::string_view path = "_level0.HUDMovieBaseInstance.QuestItemList";

	static void InitSingleton(const GFxDisplayObject& a_questItemList)
	{
		if (!singleton)
		{
			static QuestItemList singletonInstance{ a_questItemList };
			singleton = &singletonInstance;
		}
	}

	static QuestItemList* GetSingleton() { return singleton; }

	void AddQuest(RE::QUEST_DATA::Type a_questType, const std::string& a_questName,
				  [[maybe_unused]]const std::vector<std::string>& a_questObjectives)
	{
		//GFxArray gfxObjectives;

		Invoke("AddQuest", a_questType, a_questName.c_str()); //, a_questObjective.c_str());
	}

	void SetQuestSide(const std::string& a_sideName)
	{
		Invoke("SetQuestSide", a_sideName.c_str());
	}

	void ShowQuest()
	{
		Invoke("ShowQuest");
	}

	void RemoveQuest()
	{
		Invoke("RemoveQuest");
	}

	void RemoveAllQuests()
	{
		Invoke("RemoveAllQuests");
	}

private:

	QuestItemList(const GFxDisplayObject& a_questItemList) :
		GFxDisplayObject{ a_questItemList }
	{
		if (HasMember("QuestItemList"))
		{
			Invoke("QuestItemList");
		}
	}

	static inline QuestItemList* singleton = nullptr;

	//GFxArray questItemDataA;
};