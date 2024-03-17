#pragma once

#include "RE/G/GFxShapeCharacterDef.h"

namespace RE
{
	class GFxMorphCharacterDef : public GFxShapeCharacterDef
	{
	public:
		~GFxMorphCharacterDef() override;

		// override (GFxCharacterDef)
		void Display(GFxDisplayContext& a_displayContext, GFxCharacter* a_character) override;					   // 04
		bool DefPointTestLocal(const GPointF& a_pt, bool a_arg2, const GFxCharacter* a_character) const override;  // 05
		GRectF GetRectBoundsLocal() const override;																   // 07

		// override (GFxShapeBaseCharacterDef)
		void Unk_0C(void) override;	 // 0C
		void Unk_0D(void) override;	 // 0D

		// add
		virtual void Unk_16(void);	// 16

		// members
		GFxConstShapeNoStyles* unkF0;  // F0
		std::uint64_t unkF8[16];	   // F8
	};
	static_assert(sizeof(GFxMorphCharacterDef) == 0x178);
}
