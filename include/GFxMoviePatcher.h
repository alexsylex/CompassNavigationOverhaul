#pragma once

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

	class GFxMovieClip : 
		public RE::GFxValue,
		GFxMemberVisitor
	{
	public:

		GFxMovieClip() = default;

		GFxMovieClip(RE::GFxMovieView* a_movieView, const std::string_view& a_pathToMovieClip)
		{
			a_movieView->GetVariable(this, a_pathToMovieClip.data());

			while (GetMovieView() != a_movieView);
		}

		GFxMovieClip(const RE::GFxValue& a_value)
		{
			assert(a_value.IsDisplayObject());

			*static_cast<RE::GFxValue*>(this) = a_value;
		}

		GFxMovieClip CreateEmptyMovieClip(const std::string_view& a_name)
		{
			RE::GFxValue nextHighestDepth;
			Invoke("getNextHighestDepth", &nextHighestDepth);
			return CreateEmptyMovieClip(a_name, nextHighestDepth.GetNumber());
		}

		GFxMovieClip CreateEmptyMovieClip(const std::string_view& a_name, double a_depth)
		{
			GFxMovieClip mc;
			Invoke("createEmptyMovieClip", &mc, a_name.data(), a_depth);
			return mc;
		}

		GFxMovieClip AttachMovie(const std::string_view& a_className, const std::string_view& a_name, double a_depth)
		{
			GFxMovieClip mc;
			Invoke("attachMovie", &mc, a_className.data(), a_name.data(), a_depth);
			return mc;
		}

		void LoadMovie(const std::string_view& a_swfPath)
		{
			Invoke("loadMovie", nullptr, a_swfPath.data());
		}

		void UnloadMovie()
		{
			Invoke("unloadMovie", nullptr);
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
			RE::GFxValue member;
			RE::GFxValue::GetMember(a_memberName.data(), &member);

			return member;
		}

		RE::GFxValue GetMember(int a_index) const
		{
			// TODO: Get member by index (instead of by name)
			// _root.container: DisplayObject
			// {
			//		var instance67: DisplayObject
			// }
			// Anonymous movieclips are given name = instance + i
		}

		bool SetMember(const std::string_view& a_memberName, const RE::GFxValue& a_value)
		{
			RE::GFxValue::SetMember(a_memberName.data(), a_value);
		}

		RE::GFxMovieView* GetMovieView()
		{
			return **reinterpret_cast<RE::GFxMovieView***>(static_cast<RE::GFxValue*>(this));
		}

	protected:

		template <typename... Args>
		bool Invoke(const std::string_view& a_functionName, RE::GFxValue* a_result, Args&&... args)
		{
			if (!IsDisplayObject())
			{
				return false;
			}

			std::array<RE::GFxValue, sizeof...(Args)> gfxArgs{ std::forward<Args>(args)... };

			bool ret = RE::GFxValue::Invoke(a_functionName.data(), a_result, sizeof...(Args) ? &gfxArgs[0] : nullptr, sizeof...(Args));

			RE::GFxMovieView* movieView = GetMovieView();

			movieView->Advance(0.0);

			return ret;
		}
	};

	class GFxMoviePatcher
	{
	public:

		GFxMoviePatcher(RE::GFxMovieView* a_movieView, const std::string_view& a_movieUrl);

		std::string GetMovieFilename() const {return movieFilename.substr(0, movieFilename.find('.')); }

		int LoadAvailableSwfPatches();

	private:

		RE::GFxMovieView* movieView;
		const std::string movieDir;
		const std::string movieFilename;
		GFxMovieClip _root{ movieView, "_root" };
	};
}
