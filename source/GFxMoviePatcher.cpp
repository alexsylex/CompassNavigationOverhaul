#include "SwfLoader.h"

#include "RE/G/GFxMovieView.h"

#include "utils/Logger.h"

namespace IUI
{
	void GFxMemberVisitor::Visit(const char* a_name, const RE::GFxValue& a_gfxValue)
	{
		switch (a_gfxValue.GetType())
		{
		case ValueType::kObject:
		case ValueType::kDisplayObject:
			logger::debug("\tvar {}: {}", a_name, ValueTypeToString(a_gfxValue.GetType()));
		}
	}

	GFxMoviePatcher::GFxMoviePatcher(RE::GFxMovieView* a_movieView, const std::string_view& a_movieUrl)
		: movieView{ a_movieView }, movieDir{ a_movieUrl.substr(0, a_movieUrl.rfind('/') + 1) },
		movieFilename{ a_movieUrl.substr(a_movieUrl.rfind('/') + 1) }
	{
		if (_root->CreateEmptyMovieClip("swfloader", -1000))
		{
			swfloader = std::make_unique<SwfLoader>(movieView, "_root.swfloader");

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

	GFxMoviePatcher::~GFxMoviePatcher()
	{
		logger::trace("~GFxMoviePatcher() called");

		if (!swfloader->RemoveMovieClip())
		{
			logger::error("Something went wrong removing the swf loader movieclip");
		}

		logger::flush();
	}

	int GFxMoviePatcher::LoadAvailableSwfPatches()
	{
		namespace fs = std::filesystem;

		fs::path rootPath = fs::current_path().append("Data\\Interface");
		if (movieDir.find("Interface/Exported/") != std::string_view::npos) 
		{
			rootPath.append("Exported");
		}

		fs::path startPath = rootPath;
		startPath.append(GetMovieFilename());
		
		int loadedSwfPatches = 0;

		if (fs::exists(startPath))
		{
			// Non-recursive Depth-First Search to traverse all nodes
			// Reference: https://en.wikipedia.org/wiki/Depth-first_search
			std::stack<fs::path> stack;

			stack.push(startPath);

			while (!stack.empty())
			{
				fs::path currentPath = stack.top();
				stack.pop();
		
				logger::debug("{}", currentPath.string().c_str());
				logger::flush();
		
				if (fs::is_directory(currentPath))
				{
					for (const fs::directory_entry& childPath : fs::directory_iterator{ currentPath }) 
					{
						stack.push(childPath);
					}
				}
				else
				{
					std::string pathToSwfPatch = fs::relative(currentPath, rootPath).string().c_str();

					std::size_t pathToMemberStart = pathToSwfPatch.find("\\") + 1;
					std::size_t pathToMemberEnd = pathToSwfPatch.rfind('.');
					std::size_t pathToMemberLen = pathToMemberEnd - pathToMemberStart;

					std::string pathToMember = pathToSwfPatch.substr(pathToMemberStart, pathToMemberLen);

					std::replace(pathToMember.begin(), pathToMember.end(), '\\', '.');

					RE::GFxValue member;
					if (movieView->GetVariable(&member, pathToMember.c_str())) 
					{
						RE::GFxValue result = swfloader->Replace(pathToSwfPatch, pathToMember);

						loadedSwfPatches++;
					}
				}
			}
		}

		return loadedSwfPatches;
	}	
}
