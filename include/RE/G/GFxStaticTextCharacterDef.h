#pragma once

#include "RE/G/GFxCharacterDef.h"

namespace RE
{
	class GFxStaticTextCharacterDef : public GFxCharacterDef
	{
	public:
		~GFxStaticTextCharacterDef() override;	// 00

		// override (GFxCharacterDef)
		bool DefPointTestLocal(const GPointF& a_pt, bool a_arg2, const GFxCharacter* a_character) const override;					   // 05
		GRectF GetBoundsLocal() const override;																						   // 06
		GFxCharacter* CreateCharacter(GFxASCharacter* a_parent, GFxResourceID a_resourceID, GFxMovieDefImpl* a_bindingImpl) override;  // 09

		// override (GFxResource)
		std::uint32_t GetResourceTypeCode() const override;	 // 02

		// members
		GRectF bounds;			 // 20
		std::uint64_t unk30[7];	 // 30
	};
	static_assert(sizeof(GFxStaticTextCharacterDef) == 0x68);
}
