#pragma once

#include "RE/G/GFxValue.h"
#include "RE/G/GFxFunctionHandler.h"

#include "type_name.h"

// Scaleform GFx function objects

template<class ObjectT, class MethodT>
class GFxDelegate : public RE::GFxFunctionHandler
{
public:

	GFxDelegate(ObjectT* a_object, void(ObjectT::* a_method)(Params&))
	: object{ a_object }, method{ a_method }
	{ 
		Register(object->view, object->root);
	}

	bool Register(RE::GPtr<RE::GFxMovieView> a_view, RE::GFxValue& a_property)
	{
		RE::GFxValue gfxMethod;
		a_view->CreateFunction(&gfxMethod, this);

		static constinit std::string_view name{ unscoped_type_name<MethodT>() };

		return a_property.SetMember(name.data(), gfxMethod);
	}

private:

	virtual void Call(Params& a_params) override  // 01
	{
		(object->*method)(a_params);
	}

	ObjectT* object;
	void(ObjectT::* method)(Params& a_params);
};

template<class FunctionName>
class GFxStaticFunction : public RE::GFxFunctionHandler
{
public:

	static bool Register(RE::GFxMovieView* a_view, const std::string_view& a_pathToFunctionHolder)
	{
		static GFxStaticFunction g_function;

		RE::GFxValue g_functionObj;
		a_view->CreateFunction(&g_functionObj, &g_function);

		static constinit std::string_view g_functionName{ unscoped_type_name<FunctionName>() };

		std::string functionPath{ a_pathToFunctionHolder };
		functionPath.append(".").append(g_functionName);

		if (a_view->SetVariable(functionPath.c_str(), g_functionObj, RE::GFxMovie::SetVarType::kPermanent)) 
		{
			return a_view->IsAvailable(functionPath.c_str());
		}

		return false;
	}

private:

	virtual void Call(Params& a_params) override;  // 01
};