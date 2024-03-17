#pragma once

#include "RE/G/GRefCountWeakSupportImpl.h"

namespace RE
{
	template <class Base, std::uint32_t StatType>
	class GRefCountBaseWeakSupport : public GRefCountBaseStatImpl<GRefCountWeakSupportImpl, StatType>
	{};
}