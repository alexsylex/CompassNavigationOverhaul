#pragma once

namespace RE
{
	class GFxMovieView;
}

namespace IUI
{
	class GFxMemberVisitor : public RE::GFxValue::ObjectVisitor
	{
	protected:

		using ValueType = RE::GFxValue::ValueType;

		const char* const ValueTypeToString(ValueType a_valueType)
		{
			switch (a_valueType) 
			{
			case ValueType::kNull: return "Null";
			case ValueType::kBoolean: return "Boolean";
			case ValueType::kNumber: return "Number";
			case ValueType::kString: return "String";
			case ValueType::kStringW: return "StringW";
			case ValueType::kArray: return "Array";
			case ValueType::kObject: return "Object";
			case ValueType::kDisplayObject: return "DisplayObject";
			default: return "Undefined";
			}
		};

		void Visit(const char* a_name, const RE::GFxValue& a_gfxValue) override;
	};

	class GFxMovieClip : GFxMemberVisitor
	{
		constexpr std::string GetMemberPath(const std::string_view& a_memberName) const
		{
			std::string pathToMember = pathToThis;
			pathToMember.append(".").append(a_memberName);

			return pathToMember;
		}

		template <typename... Args>
		bool Invoke(const std::string_view& a_functionName, Args&&... args)
		{
			if (!movieView->IsAvailable(pathToThis.c_str()))
			{
				return false;
			}

			std::string methodName = GetMemberPath(a_functionName);

			std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

			bool retVal = movieView->Invoke(methodName.c_str(), nullptr, &gfxArgs[0], sizeof...(Args));

			movieView->Advance(0.0);

			return retVal;
		}

	public:

		GFxMovieClip(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToMovieClip) 
		: movieView{ a_movieView }, pathToThis{ a_pathToMovieClip }
		{ }

		bool CreateEmptyMovieClip(const std::string_view& a_name, int a_depth)
		{
			return Invoke("createEmptyMovieClip", a_name.data(), a_depth);
		}

		bool LoadMovie(const std::string_view& a_swfPath)
		{
			return Invoke("loadMovie", a_swfPath.data());
		}

		bool RemoveMovieClip(const std::string_view& a_memberName)
		{
			return Invoke("removeMovieClip", a_memberName.data());
		}

		RE::GFxValue GetMember(const std::string_view& a_memberName) const
		{
			std::string memberName = GetMemberPath(a_memberName);

			RE::GFxValue member;
			movieView->GetVariable(&member, memberName.c_str());

			return member;
		}

	private:

		RE::GFxMovieView* movieView;
		const std::string pathToThis;
	};

	class SwfLoader
	{
	public:

		SwfLoader(RE::GFxMovieView* a_movieView, const std::string_view& a_movieUrl);

		~SwfLoader();

		std::string GetMovieFilename() const {return movieFilename.substr(0, movieFilename.find('.')); }

		int LoadAvailableMovieClipPatches();

	private:

		RE::GFxMovieView* movieView;
		const std::string movieDir;
		const std::string movieFilename;
		const std::unique_ptr<GFxMovieClip> _root = std::make_unique<GFxMovieClip>(movieView, "_root");
		std::unique_ptr<GFxMovieClip> swfloader;
	};
}
