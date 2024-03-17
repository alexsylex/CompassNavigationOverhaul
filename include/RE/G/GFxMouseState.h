#pragma once

namespace RE
{
	class GFxMouseState
	{
	public:

		// members
		void* unk00;						// 00
		void* unk08;						// 08
		void* unk10;						// 10
		std::uint32_t buttonsState;			// 18
		std::uint32_t prevButtonsState;		// 1C
		GPointF position;					// 20
		std::uint32_t cursorType;			// 28
		std::uint8_t flags;					// 2C
		std::uint8_t pad2D;					// 2D
		std::uint16_t pad2E;				// 2E
	};
	static_assert(sizeof(GFxMouseState) == 0x30);

}
