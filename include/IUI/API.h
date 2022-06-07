#pragma once

#include "GFxArray.h"
#include "GFxDisplayObject.h"

namespace IUI::API
{
	enum Status
	{
		kPreLoad,
		kPostLoad,
		kAbortLoad
	};

	struct Message
	{
		std::string_view movieUrl;
	};

	struct PreLoadMessage : Message
	{
		static constexpr inline Status type = Status::kPreLoad;

		GFxDisplayObject& originalDisplayObject;
	};

	struct PostLoadMessage : Message
	{
		static constexpr inline Status type = Status::kPostLoad;

		GFxDisplayObject& newDisplayObject;
	};

	struct AbortLoadMessage : Message
	{
		static constexpr inline Status type = Status::kAbortLoad;

		RE::GFxValue& originalValue;
	};

	template <typename T>
	concept valid_message = std::is_base_of_v<Message, T>;

	template <typename MessageT> requires valid_message<MessageT>
	inline const MessageT* TranslateAs(SKSE::MessagingInterface::Message* a_msg)
	{
		if constexpr (std::is_same_v<Message, MessageT>)
		{
			return static_cast<Message*>(a_msg->data);
		}
		else
		{
			if (a_msg->type == MessageT::type && a_msg->dataLen == sizeof(MessageT)) 
			{
				return static_cast<MessageT*>(a_msg->data);
			}

			return nullptr;
		}
	}
}