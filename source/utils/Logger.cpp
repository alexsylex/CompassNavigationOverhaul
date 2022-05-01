#include "utils/Logger.h"

#include "utils/GFxFunction.h"

template <>
void GFxStaticFunction<GFxLogger::info>::Call(Params& a_params)
{
	assert(a_params.argCount == 1);
	assert(a_params.args[0].GetType() == RE::GFxValue::ValueType::kString);

	const char* str = a_params.args[0].GetString();
	logger::info(": [gfx] {}", str);
	logger::flush();
}

template <>
void GFxStaticFunction<GFxLogger::error>::Call(Params& a_params)
{
	assert(a_params.argCount == 1);
	assert(a_params.args[0].GetType() == RE::GFxValue::ValueType::kString);

	const char* str = a_params.args[0].GetString();
	logger::error(": [gfx] {}", str);
	logger::flush();
}

namespace GFxLogger
{
	bool RegisterStaticFunctions(RE::GFxMovieView* a_view)
	{
		static bool registered = false;
		bool registeredThisTime = false;

		if (!registered) 
		{
			RE::GFxValue skse;
			a_view->CreateObject(&skse);
			a_view->SetVariable("_global.skse", skse, RE::GFxMovie::SetVarType::kPermanent);

			RE::GFxValue log;
			a_view->CreateObject(&log);
			a_view->SetVariable("_global.skse.log", log, RE::GFxMovie::SetVarType::kPermanent);

			if (a_view->GetVariable(&log, "_global.skse.log"))
			{
				if (GFxStaticFunction<info>::Register(a_view, "_global.skse.log") &&
					GFxStaticFunction<error>::Register(a_view, "_global.skse.log"))
				{
					a_view->Advance(0.0);

					registered = true;
					registeredThisTime = true;
				}
			}
		}

		return registeredThisTime;
	}
}
