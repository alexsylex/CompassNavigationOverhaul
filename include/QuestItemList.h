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

private:

	QuestItemList(const GFxDisplayObject& a_questItemList) :
		GFxDisplayObject{ a_questItemList }
	{}

	static inline QuestItemList* singleton = nullptr;
};