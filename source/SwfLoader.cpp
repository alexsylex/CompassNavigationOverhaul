#include "SwfLoader.h"

#include "RE/G/GFxMovieView.h"

#include "utils/Logger.h"

namespace IUI
{
	SwfLoader::SwfLoader(RE::GFxMovieView* a_movieView, const std::string_view& a_movieUrl)
	: movieView{ a_movieView }, movieUrl{ a_movieUrl }
	{		
		if (!_root->Invoke("createEmptyMovieClip", "swfloader", -1000))
		{
			logger::error("Something went wrong creating an empty movieclip for the swf loader");
			logger::flush();
		}
		else 
		{
			swfloader = std::make_unique<GFxMovieClip>(movieView, "_root.swfloader");

			std::string swfloaderPath = (movieUrl.find("Interface//Exported//") == std::string_view::npos) ?
											  "../" : "";
			swfloaderPath += "swfloader.swf";

			if (!swfloader->Invoke("loadMovie", swfloaderPath.c_str()))
			{
				logger::error("Something went wrong loading the swf loader movieclip");
				logger::flush();
			}
		}
	}

	SwfLoader::~SwfLoader()
	{
		if (!_root->Invoke("removeMovieClip", "swfloader"))
		{
			logger::error("Something went wrong removing the swf loader movieclip");
			logger::flush();
		}
	}

	bool SwfLoader::LoadMovieClip()
	{
		std::string swfloaderPath = (movieUrl.find("Interface//Exported//") == std::string_view::npos) ?
										  "../" :
										  "";
		if (!swfloader->Invoke("loadMovie", "HUDMenu/HUDMovieBaseInstance/CompassShoutMeterHolder.swf"))
		{
			return false;
		}

		return true;
	}
}
