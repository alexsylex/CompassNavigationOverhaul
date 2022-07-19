#pragma once

#include "IUI/GFxArray.h"
#include "IUI/GFxDisplayObject.h"

class QuestItemList : public GFxDisplayObject
{
public:

	static constexpr inline std::string_view path = "_level0.HUDMovieBaseInstance.QuestItemList";

	static void InitSingleton(const GFxDisplayObject& a_questItemList)
	{
		if (!singleton) {
			static QuestItemList singletonInstance{ a_questItemList };
			singleton = &singletonInstance;
		}
	}

	static QuestItemList* GetSingleton() { return singleton; }

	void SetQuestInfo(RE::QUEST_DATA::Type a_questType, const std::string& a_questName,
					  const std::string& a_questObjective)
	{
		Invoke("SetQuestInfo", a_questType, a_questName.c_str(), a_questObjective.c_str());
	}

	void ShowQuestInfo()
	{
		Invoke("ShowQuestInfo");
	}

	void HideQuestInfo()
	{
		Invoke("HideQuestInfo");
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
};