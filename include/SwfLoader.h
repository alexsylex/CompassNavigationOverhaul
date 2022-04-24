#pragma once

namespace RE
{
	class GFxMovieView;
}

namespace IUI
{
	class GFxMovieClip
	{
	public:

		GFxMovieClip(RE::GFxMovieView* a_movieView, std::string_view a_pathToMember) 
		: movieView{ a_movieView }, pathToMember{ a_pathToMember }
		{ }

		template <typename... Args>
		bool Invoke(std::string_view a_functionName, Args... args)
		{
			if (!movieView->IsAvailable(pathToMember.c_str()))
			{
				return false;
			}

			std::string methodName = pathToMember;
			methodName.append(".").append(a_functionName);

			std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ args... };

			bool retVal = movieView->Invoke(methodName.c_str(), nullptr, &gfxArgs[0], sizeof...(Args));

			movieView->Advance(0.0);

			return retVal;
		}

	private:

		RE::GFxMovieView* movieView;
		std::string pathToMember;
	};

	class SwfLoader
	{
	public:

		SwfLoader(RE::GFxMovieView* a_movieView, const std::string_view& a_movieUrl);

		~SwfLoader();

		bool LoadMovieClip();

	private:

		RE::GFxMovieView* movieView;
		std::string moviePath;
		std::string movieName;
		std::unique_ptr<GFxMovieClip> _root = std::make_unique<GFxMovieClip>(movieView, "_root");
		std::unique_ptr<GFxMovieClip> swfloader;
	};
}
