#include "utils/GFxVisitors.h"

#include "utils/Logger.h"

void GFxMemberVisitor::VisitMembersOf(const RE::GFxValue& a_value)
{
	logger::debug("{}: {}", a_value.ToString(), GFxValueTypeToString(a_value.GetType()));
	if (a_value.IsObject())
	{
		logger::debug("{}", "{");
		a_value.VisitMembers(this);
		logger::debug("{}", "}");
	}
	logger::flush();
}

void GFxMemberVisitor::VisitMembersOf(RE::GFxMovieView* a_view, const char* a_pathToMember)
{
	if (a_view) 
	{
		RE::GFxValue value;
		if (a_view->GetVariable(&value, a_pathToMember)) 
		{
			VisitMembersOf(value);
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

void GFxElementVisitor::VisitElementsOf(const RE::GFxValue& a_value)
{
	logger::debug("{}: {}", a_value.ToString(), GFxValueTypeToString(a_value.GetType()));
	if (a_value.IsArray())
	{
		logger::debug("{}", "{");
		a_value.VisitElements(this);
		logger::debug("{}", "}");
	}
	logger::flush();
}

void GFxElementVisitor::VisitElementsOf(RE::GFxMovieView* a_view, const char* a_pathToMember)
{
	if (a_view)
	{
		RE::GFxValue value;
		if (a_view->GetVariable(&value, a_pathToMember)) 
		{
			VisitElementsOf(value);
		}
	}
};

void GFxElementVisitor::Visit(std::uint32_t a_idx, const RE::GFxValue& a_value)
{
	logger::debug("\t[{}] {}: {}", a_idx, a_value.ToString(), GFxValueTypeToString(a_value.GetType()));
}
