#include "GFxMoviePatcher.h"

#include "RE/G/GFxMovieView.h"

#include "utils/Logger.h"

namespace IUI
{
	void GFxMemberVisitor::Visit(const char* a_name, const RE::GFxValue& a_value)
	{
		std::string_view name = a_name;
		if (name != "PlayReverse" && name != "PlayForward")
		{
			logger::debug("\tvar {}: {}", a_name, GFxValueTypeToString(a_value.GetType()));
		}
	}

	void VisitMembersForDebug(RE::GFxMovieView* a_view, const char* a_pathToMember)
	{
		IUI::GFxMemberVisitor memberVisitor;

		if (a_view) {
			RE::GFxValue member;
			if (a_view->GetVariable(&member, a_pathToMember)) 
			{
				logger::debug("{}: {}", a_pathToMember, GFxValueTypeToString(member.GetType()));
				if (member.IsObject())
				{
					logger::debug("{}", "{");
					member.VisitMembers(&memberVisitor);
					logger::debug("{}", "}");
				}
				logger::flush();
			}
		}
	};

	void GFxElementVisitor::Visit(std::uint32_t a_idx, const RE::GFxValue& a_value)
	{
		logger::debug("\t[{}] {}: {}", a_idx, a_value.ToString(), GFxValueTypeToString(a_value.GetType()));
	}

	void VisitElementsForDebug(RE::GFxMovieView* a_view, const char* a_pathToMember)
	{
		IUI::GFxElementVisitor elementVisitor;

		if (a_view) {
			RE::GFxValue member;
			if (a_view->GetVariable(&member, a_pathToMember)) 
			{
				logger::debug("{}: {}", a_pathToMember, GFxValueTypeToString(member.GetType()));
				if (member.IsArray())
				{
					logger::debug("{}", "{");
					member.VisitElements(&elementVisitor);
					logger::debug("{}", "}");
				}
				logger::flush();
			}
		}
	};

	GFxMoviePatcher::GFxMoviePatcher(RE::GFxMovieView* a_movieView, const std::string_view& a_movieUrl)
	: movieView{ a_movieView }, movieDir{ a_movieUrl.substr(0, a_movieUrl.rfind('/') + 1) },
	  movieFilename{ a_movieUrl.substr(a_movieUrl.rfind('/') + 1) }
	{ }

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

					std::size_t pathToMemberEnd = pathToSwfPatch.find(".swf", pathToSwfPatch.size() - 4);
					if (pathToMemberEnd != std::string::npos)
					{
						logger::debug("{}", currentPath.string().c_str());
						logger::flush();

						std::size_t pathToMemberStart = pathToSwfPatch.find("\\") + 1;
						std::size_t pathToMemberLen = pathToMemberEnd - pathToMemberStart;

						std::string pathToMember = pathToSwfPatch.substr(pathToMemberStart, pathToMemberLen);

						std::replace(pathToMember.begin(), pathToMember.end(), '\\', '.');

						RE::GFxValue member;
						if (movieView->GetVariable(&member, pathToMember.c_str()))
						{
							VisitMembersForDebug(movieView, "_root.HUDMovieBaseInstance");

							VisitElementsForDebug(movieView, "_root.HUDMovieBaseInstance.HudElements");

							GFxMovieClip container = _root.CreateEmptyMovieClip("container");
							container.LoadMovie("HUDMenu/HUDMovieBaseInstance/CompassShoutMeterHolder.swf");

							VisitElementsForDebug(movieView, "_root.HUDMovieBaseInstance.HudElements");

							RE::GFxValue CompassRect;
							movieView->GetVariable(&CompassRect, "_root.HUDMovieBaseInstance.CompassRect");
							logger::info("CompassRect = {}", CompassRect.ToString());

							VisitMembersForDebug(movieView, "_root.container");

							VisitMembersForDebug(movieView, "_root.HUDMovieBaseInstance");

							loadedSwfPatches++;
						}
					}
				}
			}
		}

		return loadedSwfPatches;
	}
}
