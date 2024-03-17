#pragma once

#include "RE/G/GFxResource.h"
#include "RE/G/GFxResourceID.h"
#include "RE/G/GPoint.h"

namespace RE
{
	class GFxCharacter;
	class GFxDisplayContext;

	class GFxASCharacter;
	class GFxMovieDefImpl;
	

	class GFxCharacterDef : public GFxResource
	{
	public:
		// add
		virtual void			Display(GFxDisplayContext& a_displayContext, GFxCharacter* a_character);								// 04
		virtual bool			DefPointTestLocal(const GPointF& a_pt, bool a_arg2, const GFxCharacter* a_character) const;				// 05 - { return false; }
		virtual GRectF			GetBoundsLocal() const;																					// 06
		virtual GRectF			GetRectBoundsLocal() const;																				// 07
		virtual std::uint32_t	GetVersion() const;																						// 08
		virtual GFxCharacter*	CreateCharacter(GFxASCharacter* a_parent, GFxResourceID a_resourceID, GFxMovieDefImpl* a_bindingImpl);  // 09

		// members
		GFxResourceID resourceId;  // 18
		std::uint32_t pad1C;       // 1C
	};
	static_assert(sizeof(GFxCharacterDef) == 0x20);
}
