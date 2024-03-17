#pragma once

#include "RE/G/GAtomic.h"
#include "RE/G/GFxLoadUpdateSync.h"
#include "RE/G/GFxMovieDef.h"
#include "RE/G/GFxMovieDefBindStates.h"
#include "RE/G/GFxResource.h"
#include "RE/G/GFxResourceKey.h"
#include "RE/G/GFxResourceLib.h"
#include "RE/G/GFxStateBagImpl.h"
#include "RE/G/GPtr.h"
#include "RE/G/GRefCountBase.h"
#include "RE/G/GStats.h"

namespace RE
{
	class GFxMovieDataDef;
	class GFxMovieDefImpl;

	struct GFxResourceBinding;

	struct GFxResourceBindData
	{
		GPtr<GFxResource>   resource;  // 00
		GFxResourceBinding* binding;   // 08
	};

	struct GFxResourceBinding
	{
		GMemoryHeap*                  heap;           // 00
		volatile std::uint32_t        resourceCount;  // 08
		std::uint32_t                 pad0C;          // 0C
		GFxResourceBindData* volatile resources;      // 10
		GLock                         resourceLock;   // 18
		volatile bool                 frozen;         // 40
		std::uint8_t                  pad41;          // 41
		std::uint16_t                 pad42;          // 42
		std::uint32_t                 pad44;          // 44
		GFxMovieDefImpl*              movieDef;       // 48
	};
	static_assert(sizeof(GFxResourceBinding) == 0x50);

	class GFxMovieDefImpl : public GFxMovieDef
	{
	public:

		enum BindStateType
		{
			kNotStarted = 0,
			kInProgress = 1,
			kFinished = 2,
			kCanceled = 3,  // Canceled due to a user request.
			kError = 4,
			// Mask for above states.
			kStateMask = 0xF,

			// These bits store the status of what was
			// actually loaded; we can wait based on them.
			kFrame1Loaded = 0x100,
			kLastFrameLoaded = 0x200,
		};

		class BindTaskData : public GRefCountBase<BindTaskData, GStatGroups::kGStat_Default_Mem>
		{
		public:
			virtual ~BindTaskData();  // 00

			// members
			GMemoryHeap*                                                            heap;                // 10
			GPtr<GFxMovieDataDef>                                                   dataDef;             // 18
			GFxMovieDefImpl*                                                        defImplUnsafe;       // 20
			std::uint32_t                                                           loadFlags;           // 28
			std::uint32_t                                                           pad2C;               // 2C
			GFxResourceBinding                                                      resourceBinding;     // 30
			GArrayLH<GFxMovieDef*, GFxStatMovieData::kGFxStatMD_Other_Mem>			importSourceMovies;  // 80
			GLock                                                                   importSourceLock;    // 98
			GArrayLH<GPtr<GFxMovieDefImpl>, GFxStatMovieData::kGFxStatMD_Other_Mem> resourceImports;     // C0
			std::uint32_t                                                           bindState;           // D8
			std::uint32_t                                                           padDC;               // DC
			GPtr<GFxLoadUpdateSync>                                                 bindUpdate;          // E0
			std::uint32_t                                                           bindingFrame;        // E8
			std::uint32_t                                                           bytesLoaded;         // EC
			bool                                                                    bindingCancelled;    // F0
			std::uint8_t                                                            padF1;               // F1
			std::uint16_t                                                           padF2;               // F2
			std::uint32_t                                                           padF4;               // F4
		};
		static_assert(sizeof(BindTaskData) == 0xF8);
		static_assert(sizeof(GArrayLH<GPtr<GFxMovieDefImpl>, GFxStatMovieData::kGFxStatMD_Other_Mem>) == 0x18);

		virtual ~GFxMovieDefImpl();  // 00

		// add
		virtual GFxResource* GetFont(const char* a_name, std::uint32_t a_fontFlags, std::int32_t& a_arg3);  // 1C

		// members
		GPtr<GFxStateBagImpl>       stateBag;    // 20
		GPtr<GFxLoaderImpl>         loaderImpl;  // 28
		GPtr<GFxMovieDefBindStates> bindStates;  // 30
		GPtr<BindTaskData>          bindData;    // 38
	};
	static_assert(sizeof(GFxMovieDefImpl) == 0x40);
}
