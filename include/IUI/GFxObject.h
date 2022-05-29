#pragma once

#include "RE/G/GFxMovieView.h"
#include "RE/G/GFxValue.h"

class GFxObject : public RE::GFxValue
{
public:

	GFxObject() = default;

	GFxObject(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToObject)
	{
		a_movieView->GetVariable(this, a_pathToObject.data());

		assert(GetMovieView() == a_movieView);
		assert(IsObject());
	}

	GFxObject(const RE::GFxValue& a_value)
	{
		*static_cast<RE::GFxValue*>(this) = a_value;

		assert(IsObject());
	}

	RE::GFxMovieView* GetMovieView()
	{
		return **reinterpret_cast<RE::GFxMovieView***>(static_cast<RE::GFxValue*>(this));
	}

	RE::GFxValue GetMember(const std::string_view& a_memberName) const
	{
		RE::GFxValue value;
		RE::GFxValue::GetMember(a_memberName.data(), &value);
		return value;
	}

	RE::GFxValue GetMember(int a_index) const
	{
		// TODO: Get member by index (instead of by name)
		// _root.container: DisplayObject
		// {
		//		var instance67: DisplayObject
		// }
		// Anonymous movieclips are given name = instance + i

		RE::GFxValue val = a_index;

		return val;
	}

	bool SetMember(const std::string_view& a_memberName, const RE::GFxValue& a_value)
	{
		return RE::GFxValue::SetMember(a_memberName.data(), a_value);
	}

	template <typename... Args>
	RE::GFxValue Invoke(const std::string_view& a_functionName, Args&&... args)
	{
		RE::GFxValue result;
		Invoke(a_functionName, &result, args...);
		return result;
	}

	template <typename... Args>
	bool Invoke(const std::string_view& a_functionName, RE::GFxValue* a_result, Args&&... args)
	{
		std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

		bool invokeOk = RE::GFxValue::Invoke(a_functionName.data(), a_result,
			sizeof...(Args) ? &gfxArgs[0] : nullptr, sizeof...(Args));

		if (invokeOk) {
			GetMovieView()->Advance(0.0F);
		}

		return invokeOk;
	}
};