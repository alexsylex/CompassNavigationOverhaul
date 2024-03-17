#pragma once

#include "RE/G/GNewOverrideBase.h"
#include "RE/G/GFxPlayerStats.h"
#include "RE/G/GASString.h"

namespace RE
{
	class GFxCharacter;

	class GFxCharacterHandle : public GNewOverrideBase<GFxStatMovieViews::kGFxStatMV_MovieClip_Mem>
	{
	public:

		// members
		std::int32_t  refCount;      // 00
		std::uint32_t pad04;         // 04
		GFxCharacter* character;     // 08
		GASString     name;          // 10
		GASString     namePath;      // 18
		GASString     originalName;  // 20
	};
	static_assert(sizeof(GFxCharacterHandle) == 0x28);
}
