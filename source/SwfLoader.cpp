#include "SwfLoader.h"

#include "RE/G/GFxMovieView.h"

#include "utils/Logger.h"

namespace IUI
{
	void GFxMemberVisitor::Visit(const char* a_name, const RE::GFxValue& a_gfxValue)
	{
		if (a_gfxValue.GetType() == ValueType::kDisplayObject)
		{
			logger::info("Found member: var {}: {}", a_name, ValueTypeToString(a_gfxValue.GetType()));
			logger::flush();
		}
	}

	SwfLoader::SwfLoader(RE::GFxMovieView* a_movieView, const std::string_view& a_movieUrl)
	: movieView{ a_movieView }, movieDir{ a_movieUrl.substr(0, a_movieUrl.rfind('/') + 1) }, 
	  movieFilename{ a_movieUrl.substr(a_movieUrl.rfind('/') + 1) }
	{		
		if (_root->CreateEmptyMovieClip("swfloader", -1000))
		{
			swfloader = std::make_unique<GFxMovieClip>(movieView, "_root.swfloader");

			std::string swfloaderPath = rootDir + "swfloader.swf";

			if (swfloader->LoadMovie(swfloaderPath))
			{
				RE::GFxValue version = swfloader->GetMember("version");
				if (!version.IsUndefined())
				{
					logger::info("Successfully loaded swfloader v{}", version.GetNumber());
					logger::info("");
					logger::flush();

					return;
				}
			}

			logger::error("Something went wrong loading the swf loader movieclip");
		}
		else 
		{
			logger::error("Something went wrong creating an empty movieclip for the swf loader");
		}

		logger::flush();
	}

	SwfLoader::~SwfLoader()
	{
		if (!_root->RemoveMovieClip("swfloader"))
		{
			logger::error("Something went wrong removing the swf loader movieclip");
			logger::flush();
		}
	}

	bool SwfLoader::LoadAvailableMovieClipPatches()
	{
		// Non-recursive Depth-First Search to traverse all nodes
		// Reference: https://en.wikipedia.org/wiki/Depth-first_search
		

		if (!swfloader->LoadMovie("HUDMenu/HUDMovieBaseInstance/CompassShoutMeterHolder.swf"))
		{
			return false;
		}

		return true;
	}	
}
