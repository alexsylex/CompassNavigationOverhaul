#pragma once

#include "GFxObject.h"

#include "RE/G/GFxMovieDataDef.h"
#include "RE/G/GFxSprite.h"
#include "RE/G/GFxSpriteDef.h"

namespace IUI
{
	class GFxDisplayObject : public GFxObject
	{
	public:

		GFxDisplayObject(const RE::GFxValue& a_value)
		: GFxObject{ a_value }
		{
			assert(a_value.IsDisplayObject());
		}

		GFxDisplayObject CreateEmptyMovieClip(const std::string_view& a_name, std::int32_t a_depth)
		{
			RE::GFxValue mc;
			RE::GFxValue::CreateEmptyMovieClip(&mc, a_name.data(), a_depth);
			return mc;
		}

		std::int32_t GetNextHighestDepth()
		{
			return static_cast<std::int32_t>(Invoke("getNextHighestDepth").GetNumber());
		}

		void SwapDepths(std::int32_t a_depth)
		{
			Invoke("swapDepths", a_depth);
		}

		void LoadMovie(const std::string_view& a_swfRelativePath)
		{
			Invoke("loadMovie", a_swfRelativePath.data());
		}

		void RemoveMovieClip()
		{
			Invoke("removeMovieClip");
		}

		std::pair<RE::GFxMovieDefImpl*, RE::GFxSpriteDef*> GetDefs() const
		{
			auto sprite = static_cast<RE::GFxSprite*>(static_cast<RE::GFxCharacterHandle*>(_value.obj)->character);
			auto movieDef = sprite->GetResourceMovieDef();

			RE::GFxMovieDataDef::LoadTaskData::ResourceHash& movieResourceSources = movieDef->bindData->dataDef->loadTaskData->resources;

			auto resourceSourceIt = movieResourceSources.Find(sprite->resourceID);

			if (resourceSourceIt != movieResourceSources.end())
			{
				RE::GFxResource* resource = resourceSourceIt->second.data.resource;

				if (resource->GetResourceType() == RE::GFxResource::ResourceType::kSpriteDef)
				{
					auto spriteDef = static_cast<RE::GFxSpriteDef*>(resource);

					return { movieDef, spriteDef };
				}
			}

			return { nullptr, nullptr };
		}

		RE::GPointF LocalToGlobal()
		{
			GFxObject pt(GetMovieView());

			assert(pt.IsObject());

			RE::GFxValue x = 0.0;
			pt.SetMember("x", x);

			RE::GFxValue y = 0.0;
			pt.SetMember("y", y);

			Invoke("localToGlobal", pt);

			x = pt.GetMember("x");
			y = pt.GetMember("y");

			return { static_cast<float>(x.GetNumber()), static_cast<float>(y.GetNumber()) };
		}
	};
}
