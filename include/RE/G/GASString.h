#pragma once

namespace RE
{
	class GASStringManager;

	class GASStringNode
	{
	public:

		const char* data;
		GASStringManager* manager;
		GASStringNode* nextAlloc;
		std::uint32_t refCount;
		std::uint32_t hashFlags;
		std::size_t size;
	};

	class GASString
	{
		GASStringNode* node;
	};
}