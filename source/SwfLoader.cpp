#include "SwfLoader.h"

#include "RE/G/GFxMovieView.h"

#include "utils/Logger.h"

namespace IUI
{
	void GFxMemberVisitor::Visit(const char* a_name, const RE::GFxValue& a_gfxValue)
	{
		if (a_gfxValue.GetType() == ValueType::kDisplayObject)
		{
			logger::debug("\tvar {}: {}", a_name, ValueTypeToString(a_gfxValue.GetType()));
		}
	}

	SwfLoader::SwfLoader(RE::GFxMovieView* a_movieView, const std::string_view& a_movieUrl)
		: movieView{ a_movieView }, movieDir{ a_movieUrl.substr(0, a_movieUrl.rfind('/') + 1) },
		movieFilename{ a_movieUrl.substr(a_movieUrl.rfind('/') + 1) }
	{
		if (_root->CreateEmptyMovieClip("swfloader", -1000))
		{
			swfloader = std::make_unique<GFxMovieClip>(movieView, "_root.swfloader");

			std::string swfloaderPath = (movieDir.find("Interface/Exported/") == std::string_view::npos) ? "" : "../";
			swfloaderPath += "swfloader.swf";

			if (swfloader->LoadMovie(swfloaderPath))
			{
				RE::GFxValue version = swfloader->GetMember("version");
				if (!version.IsUndefined())
				{
					logger::debug("Successfully loaded swfloader v{} in this GFx movie", version.GetNumber());
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
		logger::trace("~SwfLoader() called");

		if (!_root->RemoveMovieClip("swfloader"))
		{
			logger::error("Something went wrong removing the swf loader movieclip");
		}

		logger::flush();
	}

	int SwfLoader::LoadAvailableMovieClipPatches()
	{
		namespace fs = std::filesystem;

		fs::path gfxMoviePath = fs::current_path().append("Data\\Interface");
		if (movieDir.find("Interface/Exported/") != std::string_view::npos) 
		{
			gfxMoviePath.append("Exported");
		}

		fs::path rootPath = gfxMoviePath;
		rootPath.append(GetMovieFilename());
		
		int loadedMovieClipPatches = 0;

		if (fs::exists(rootPath))
		{
			// Non-recursive Depth-First Search to traverse all nodes
			// Reference: https://en.wikipedia.org/wiki/Depth-first_search
			std::stack<fs::path> stack;

			stack.push(rootPath);

			while (!stack.empty())
			{
				fs::path path = stack.top();
				stack.pop();
		
				logger::debug("{}", path.string().c_str());
				logger::flush();
		
				if (fs::is_directory(path))
				{
					for (const fs::directory_entry& childPath : fs::directory_iterator{ path }) 
					{
						stack.push(childPath);
					}
				}
				else
				{
					fs::path moviePath = fs::relative(path, gfxMoviePath);

					if (swfloader->LoadMovie(moviePath.string().c_str()))
					{
						loadedMovieClipPatches++;
					}
				}
			}
		}

		return loadedMovieClipPatches;
	}	
}
