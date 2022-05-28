#pragma once

#include "RE/G/GFxMovieView.h"
#include "RE/G/GFxValue.h"

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

class GFxMemberVisitor : public RE::GFxValue::ObjectVisitor
{
public:

	void VisitMembersOf(const RE::GFxValue& a_value);
	void VisitMembersOf(RE::GFxMovieView* a_view, const char* a_pathToMember);

protected:

	using ValueType = RE::GFxValue::ValueType;

	void Visit(const char* a_name, const RE::GFxValue& a_value) override;
};

class GFxElementVisitor : public RE::GFxValue::ArrayVisitor
{
public:

	void VisitElementsOf(const RE::GFxValue& a_value);
	void VisitElementsOf(RE::GFxMovieView* a_view, const char* a_pathToMember);

protected:

	using ValueType = RE::GFxValue::ValueType;

	void Visit(std::uint32_t a_idx, const RE::GFxValue& a_value) override;
};