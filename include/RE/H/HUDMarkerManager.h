#pragma once

#include "RE/G/GFxValue.h"

namespace RE
{
	struct ScaleformHUDMarkerData
	{
		GFxValue heading;  // 00 - angle relative to camera
		GFxValue alpha;	   // 18
		GFxValue type;	   // 30 - Goto and stop frame in flash
		GFxValue scale;	   // 48
	};
	static_assert(sizeof(ScaleformHUDMarkerData) == 0x60);

	class HUDMarkerManager
	{
	public:

		ScaleformHUDMarkerData scaleformMarkerData[49];	 // 0000
		NiPoint3 position[48];							 // 1260
		void* unk14A0;									 // 14A0
		std::uint32_t unk14A8;							 // 14A8
		std::uint32_t unk14AC;							 // 14AC
		std::uint32_t unk14B0;							 // 14B0
		std::uint32_t unk14B4;							 // 14B4
		std::uint32_t unk14B8;							 // 14B8
		std::uint32_t currentIndex;						 // 14BC
		std::uint32_t unk14C0;							 // 14C0
	};
	static_assert(sizeof(HUDMarkerManager) == 0x14C8);
}