#pragma once

#include "RE/H/HUDObject.h"

namespace RE
{
	class Compass : HUDObject
	{
		~Compass() override;  // 00

		// override (HUDObject)
		void Update() override;								  // 01
		void RegisterHUDComponent(FxDelegateArgs& a_params);  // 03

		bool SetMarkers()
		{
			using func_t = decltype(&Compass::SetMarkers);
			REL::Relocation<func_t> func{ RELOCATION_ID(50775, 51670) };
			return func(this);
		}

		GFxValue targetDataArray;  // 28
	};
	static_assert(sizeof(Compass) == 0x40);
}