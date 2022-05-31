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

	struct PreLoadMessage
	{
		static constexpr inline Status type = Status::kPreLoad;

		const GFxDisplayObject* const originalDisplayObject;
	};

	struct PostLoadMessage
	{
		static constexpr inline Status type = Status::kPostLoad;

		const char* payload;
	};

	struct AbortLoadMessage
	{
		static constexpr inline Status type = Status::kAbortLoad;

		const RE::GFxValue* const originalValue;
	};

	template <typename Message>
	concept is_valid_message = std::is_enum<decltype(Message::type)>::value;

	template <typename Message>
	requires is_valid_message<Message> inline const Message* TranslateAs(SKSE::MessagingInterface::Message* a_msg)
	{
		if (a_msg->type == Message::type && a_msg->dataLen == sizeof(Message)) {
			return static_cast<Message*>(a_msg->data);
		}

		return nullptr;
	}
}