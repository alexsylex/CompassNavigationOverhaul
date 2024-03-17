#pragma once

#include "RE/G/GFxConstShapeWithStyles.h"
#include "RE/G/GFxShapeBaseCharacterDef.h"

namespace RE
{
	class GFxConstShapeCharacterDef : public GFxShapeBaseCharacterDef
	{
	public:
		~GFxConstShapeCharacterDef() override;	// 00

		// override (GFxCharacterDef)
		void Display(GFxDisplayContext& a_displayContext, GFxCharacter* a_character) override;					   // 04
		bool DefPointTestLocal(const GPointF& a_pt, bool a_arg2, const GFxCharacter* a_character) const override;  // 05
		GRectF GetBoundsLocal() const override;																	   // 06
		GRectF GetRectBoundsLocal() const override;																   // 07

		// override (GFxShapeBaseCharacterDef)
		void Unk_0A(void) override;	 // 0A
		void Unk_0B(void) override;	 // 0B
		void Unk_0C(void) override;	 // 0C
		void Unk_0D(void) override;	 // 0D
		void Unk_0E(void) override;	 // 0E
		void Unk_0F(void) override;	 // 0F
		void Unk_10(void) override;	 // 10
		void Unk_11(void) override;	 // 11

		// add
		virtual void Unk_12(void);	// 12
		virtual void Unk_13(void);	// 13
		virtual void Unk_14(void);	// 14
		virtual void Unk_15(void);	// 15

		// members
		GFxConstShapeWithStyles shape;	// 20
	};
	static_assert(sizeof(GFxConstShapeCharacterDef) == 0x98);
}
