#pragma once

#include "RE/G/GFxCharacterDef.h"

namespace RE
{
	class GASExecuteTag;

	class GFxTimelineDef : public GFxCharacterDef
	{
	public:
		struct Frame
		{
			std::uint32_t  GetTagCount() const { return size; }
			GASExecuteTag* GetTag(std::uint32_t a_index) const { return data[a_index]; }

			GASExecuteTag** data;  // 00
			std::uint32_t   size;  // 08
		};

		// add
		virtual std::uint32_t   GetFrameCount() const = 0;                                                                        // 0A
		virtual const Frame		GetPlayList(std::int32_t a_frameNumber) const = 0;							                      // 0B
		virtual bool			GetInitActions(Frame* a_frame, std::int32_t a_frameNumber) const = 0;                             // 0C
		virtual bool            GetLabeledFrame(const char* a_label, std::uint32_t* a_frameNumber, bool a_translateNumbers) = 0;  // 0D
	};
	static_assert(sizeof(GFxTimelineDef) == 0x20);
}
