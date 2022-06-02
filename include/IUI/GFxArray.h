#pragma once

#include "GFxObject.h"

class GFxArray : public GFxObject
{
public:

	GFxArray() = default;

	GFxArray(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToMovieClip)
	: GFxObject{ a_movieView, a_pathToMovieClip }
	{
		assert(IsArray());
	}

	GFxArray(const RE::GFxValue& a_value)
	: GFxObject{ a_value }
	{
		assert(a_value.IsArray());
	}

	RE::GFxValue GetElement(std::uint32_t a_index) const
	{
		RE::GFxValue value;
		RE::GFxValue::GetElement(a_index, &value);
		return value;
	}

	bool SetElement(std::uint32_t a_index, const RE::GFxValue& a_value)
	{
		return RE::GFxValue::SetElement(a_index, &a_value);
	}

	std::int32_t FindElement(const RE::GFxValue& a_value)
	{
		for (std::uint32_t i = 0; i < GetArraySize(); i++) 
		{
			if (GetElement(i) == a_value)
			{
				return i;
			}
		}

		return -1;
	}
};