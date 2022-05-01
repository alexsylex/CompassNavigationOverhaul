#pragma once

namespace RE
{
	class GFxMovieView;
}

constexpr const char* const GFxValueTypeToString(RE::GFxValue::ValueType a_valueType)
{
	switch (a_valueType)
	{
	case RE::GFxValue::ValueType::kNull: return "Null";
	case RE::GFxValue::ValueType::kBoolean: return "Boolean";
	case RE::GFxValue::ValueType::kNumber: return "Number";
	case RE::GFxValue::ValueType::kString: return "String";
	case RE::GFxValue::ValueType::kStringW: return "StringW";
	case RE::GFxValue::ValueType::kArray: return "Array";
	case RE::GFxValue::ValueType::kObject: return "Object";
	case RE::GFxValue::ValueType::kDisplayObject: return "DisplayObject";
	default: return "Undefined";
	}
};

namespace IUI
{
	class GFxMemberVisitor : public RE::GFxValue::ObjectVisitor
	{
	protected:

		using ValueType = RE::GFxValue::ValueType;

		void Visit(const char* a_name, const RE::GFxValue& a_gfxValue) override;
	};

	void VisitMembersForDebug(RE::GFxMovieView* a_view, const char* a_pathToMember);

	class GFxMovieClip : GFxMemberVisitor
	{
	public:

		GFxMovieClip(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToMovieClip) 
		: movieView{ a_movieView }, pathToThis{ a_pathToMovieClip }
		{ }

		bool CreateEmptyMovieClip(const std::string_view& a_name, int a_depth)
		{
			return Invoke("createEmptyMovieClip", nullptr, a_name.data(), a_depth);
		}

		bool LoadMovie(const std::string_view& a_swfPath)
		{
			return Invoke("loadMovie", nullptr, a_swfPath.data());
		}

		bool UnloadMovie()
		{
			return Invoke("unloadMovie", nullptr);
		}

		bool RemoveMovieClip()
		{
			// MovieClip.removeMovieClip() does not remove a movie clip assigned
			// to a negative depth value. Movie clips created in the authoring tool
			// are assigned negative depth values by default. To remove a movie clip
			// that is assigned to a negative depth value, first use the MovieClip.swapDepths()
			// method to move the movie clip to a positive depth value.
			// Reference: http://homepage.divms.uiowa.edu/~slonnegr/flash/ActionScript2Reference.pdf#page=917
			return  Invoke("swapDepths", nullptr, 1) ?
					Invoke("removeMovieClip", nullptr) :
					false;
		}

		RE::GFxValue GetMember(const std::string_view& a_memberName) const
		{
			std::string memberName = GetMemberPath(a_memberName);

			RE::GFxValue member;
			movieView->GetVariable(&member, memberName.c_str());

			return member;
		}

	protected:

		constexpr std::string GetMemberPath(const std::string_view& a_memberName) const
		{
			std::string pathToMember = pathToThis;
			pathToMember.append(".").append(a_memberName);

			return pathToMember;
		}

		template <typename... Args>
		bool Invoke(const std::string_view& a_functionName, RE::GFxValue* a_result, Args&&... args)
		{
			if (!movieView->IsAvailable(pathToThis.c_str()))
			{
				return false;
			}

			std::string methodName = GetMemberPath(a_functionName);

			std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

			bool retVal = movieView->Invoke(methodName.c_str(), a_result,
											sizeof...(Args) ? &gfxArgs[0] : nullptr, sizeof...(Args));

			movieView->Advance(0.0);

			return retVal;
		}

		RE::GFxMovieView* movieView;
		const std::string pathToThis;
	};

	class SwfLoader : public GFxMovieClip
	{
	public:

		SwfLoader(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToMovieClip) 
		: GFxMovieClip{ a_movieView, a_pathToMovieClip }
		{ }

		bool LoadPatch(const std::string_view& a_pathToSwfPatch)
		{
			return Invoke("loadPatch", nullptr, a_pathToSwfPatch.data());
		}

		bool Replace(const std::string_view& a_pathToDestMember)
		{
			return Invoke("replace", nullptr, a_pathToDestMember.data());
		}
	};

	class GFxMoviePatcher
	{
	public:

		GFxMoviePatcher(RE::GFxMovieView* a_movieView, const std::string_view& a_movieUrl);

		bool IsReady() const { return movieView->IsAvailable("_root.swfloader"); }

		std::string GetMovieFilename() const {return movieFilename.substr(0, movieFilename.find('.')); }

		int LoadAvailableSwfPatches();

		void TestLog();

	private:

		RE::GFxMovieView* movieView;
		const std::string movieDir;
		const std::string movieFilename;
		const std::unique_ptr<GFxMovieClip> _root = std::make_unique<GFxMovieClip>(movieView, "_root");
		std::unique_ptr<SwfLoader> swfloader;
	};
}
