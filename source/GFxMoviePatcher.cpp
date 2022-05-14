#include "GFxMoviePatcher.h"

#include "RE/G/GFxMovieView.h"

#include "utils/Logger.h"

namespace IUI
{
	void GFxMemberVisitor::Visit(RE::GFxMovieView* a_view, const char* a_pathToMember)
	{
		if (a_view) 
		{
			RE::GFxValue member;
			if (a_view->GetVariable(&member, a_pathToMember)) 
			{
				logger::debug("{}: {}", a_pathToMember, GFxValueTypeToString(member.GetType()));
				if (member.IsObject())
				{
					logger::debug("{}", "{");
					member.VisitMembers(this);
					logger::debug("{}", "}");
				}
				logger::flush();
			}
		}
	};

	void GFxMemberVisitor::Visit(const char* a_name, const RE::GFxValue& a_value)
	{
		std::string_view name = a_name;
		if (name != "PlayReverse" && name != "PlayForward")
		{
			logger::debug("\tvar {}: {}", a_name, GFxValueTypeToString(a_value.GetType()));
		}
	}

	void GFxRecursiveMemberVisitor::Visit(const char* a_name, const RE::GFxValue& a_value)
	{
		std::string_view name = a_name;
		if (name != "PlayReverse" && name != "PlayForward") 
		{
			if (a_value.IsDisplayObject()) 
			{
				GFxMovieClip movieClip{ a_value };
				GFxMemberVisitor::Visit(movieClip.GetMovieView(), movieClip.ToString().c_str());
			}
			else
			{
				logger::debug("\tvar {}: {}", a_name, GFxValueTypeToString(a_value.GetType()));
			}
		}
	}
	
	void GFxElementVisitor::Visit(RE::GFxMovieView* a_view, const char* a_pathToMember)
	{
		if (a_view)
		{
			RE::GFxValue member;
			if (a_view->GetVariable(&member, a_pathToMember)) 
			{
				logger::debug("{}: {}", a_pathToMember, GFxValueTypeToString(member.GetType()));
				if (member.IsArray())
				{
					logger::debug("{}", "{");
					member.VisitElements(this);
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
		
		int loadedPatchesCount = 0;

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
					std::string filePath = fs::relative(currentPath, rootPath).string().c_str();

					std::size_t filePathEnd = filePath.find(".swf", filePath.size() - 4);
					if (filePathEnd != std::string::npos)
					{
						logger::debug("{}", currentPath.string().c_str());
						logger::flush();

						std::size_t filePathStart = filePath.find("\\") + 1;
						std::size_t filePathLen = filePathEnd - filePathStart;

						std::string fullQualyMemberName = filePath.substr(filePathStart, filePathLen);

						std::replace(fullQualyMemberName.begin(), fullQualyMemberName.end(), '\\', '.');

						RE::GFxValue member;
						if (movieView->GetVariable(&member, fullQualyMemberName.c_str()))
						{
							std::string patchContainerName = "patch";
							patchContainerName += std::to_string(loadedPatchesCount);

							GFxMovieClip patch = _root.CreateEmptyMovieClip(patchContainerName);
							patch.LoadMovie(filePath);

							loadedPatchesCount++;
						}
					}
				}
			}
		}

		return loadedPatchesCount;
	}
}
