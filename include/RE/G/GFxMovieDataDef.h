#pragma once

#include "RE/G/GAtomic.h"
#include "RE/G/GFxLoadUpdateSync.h"
#include "RE/G/GFxResourceReport.h"
#include "RE/G/GFxTimelineDef.h"
#include "RE/G/GHashSet.h"
#include "RE/G/GHashUncached.h"
#include "RE/G/GRefCountBase.h"
#include "RE/G/GStringHash.h"

namespace RE
{
	class GFxLog;
	class GFxPathAllocator;
	class GFxFrameBindData;
	class GFxFontDataUseNode;
	class GFxResourceDataNode;

	class GFxExporterInfo
	{
	public:

		enum ExportFlagConstants
		{
			kGlyphTexturesExported = 0x01,
			kGradientTexturesExported = 0x02,
			kGlyphsStripped = 0x10
		};

		GFxFileConstants::FileFormatType                     format;       // 00
		std::uint32_t                                        pad04;        // 04
		const char*                                          pPrefix;      // 08
		const char*                                          pSwfName;     // 10
		std::uint16_t                                        version;      // 18
		std::uint16_t                                        pad1A;        // 1A
		stl::enumeration<ExportFlagConstants, std::uint32_t> exportFlags;  // 1C
	};
	static_assert(sizeof(GFxExporterInfo) == 0x20);

	class GFxExporterInfoImpl : GFxExporterInfo
	{
		GString prefix;                     // 20
		GString swfName;                    // 28
		GArray<std::uint32_t> codeOffsets;  // 30
	};
	static_assert(sizeof(GFxExporterInfoImpl) == 0x48);

	class GFxResourceHandle
	{
		enum HandleType
		{
			kPointer,
			kIndex
		};

		HandleType HType;
		union
		{
			unsigned BindIndex;
			GFxResource* pResource;
		};
	};

	class GFxDataAllocator
	{
	public:
		void* Alloc(UPInt a_size)
		{
			if (a_size > 0xFF8) {
				auto newBlock = static_cast<void**>(heap->Alloc(a_size + 8, 0));
				if (!newBlock) {
					return nullptr;
				}

				newBlock[0] = lastBlock;
				lastBlock = newBlock;

				return &newBlock[1];
			}

			if (a_size > memAvailable) {
				auto newBlock = static_cast<void**>(heap->Alloc(0x1FF8, 0));
				if (!newBlock) {
					return nullptr;
				}

				newBlock[0] = lastBlock;
				lastBlock = newBlock;

				allocPtr = &newBlock[1];
				memAvailable = 0x1FF0;
			}

			auto memPtr = allocPtr;

			memAvailable -= a_size;
			allocPtr = static_cast<char*>(allocPtr) + a_size;

			return memPtr;
		}

		template <class T>
		T* Alloc()
		{
			return static_cast<T*>(Alloc(sizeof(T)));
		}

		void Free()
		{
			const auto globalHeap = GMemory::GetGlobalHeap();
			void* block = lastBlock;
			while (block) {
				globalHeap->Free(block);
				block = static_cast<void**>(block)[0];
			}
		}

		// members
		void* allocPtr;				 // 00
		std::uint64_t memAvailable;	 // 08
		void* lastBlock;			 // 10
		GMemoryHeap* heap;			 // 18
	};

	struct GFxMovieHeaderData
	{
		std::uint32_t       fileLength;    // 00
		std::uint32_t       version;       // 04
		GRectF              frameRect;     // 08
		float               frameRate;     // 18
		std::uint32_t       frameCount;    // 1C
		std::uint32_t       swfFlags;      // 20
		GFxExporterInfoImpl exporterInfo;  // 24
	};
	static_assert(sizeof(GFxMovieHeaderData) == 0x70);

	struct GFxResourceSource
	{
		enum SourceType : std::int32_t
		{
			kNormal,
			kImported,
		};

		struct ImportSource
		{
			std::uint32_t index;
			std::uint32_t unk04;
		};

		union ResourceUnion
		{
			GFxResource* resource;
			ImportSource importSource;
		};

		SourceType    type;   // 00
		std::uint32_t pad04;  // 04
		ResourceUnion data;   // 08
	};
	static_assert(sizeof(GFxResourceSource) == 0x10);

	struct GFxImportNode
	{
		struct ImportAssetInfo
		{
			GString       name;         // 00
			std::uint32_t id;           // 08
			std::uint32_t importIndex;  // 0C
		};
		static_assert(sizeof(ImportAssetInfo) == 0x10);

		GArray<ImportAssetInfo> assets;       // 00
		GString                 filename;     // 18
		std::uint32_t           frame;        // 20
		std::uint32_t           movieIndex;   // 24
		GFxImportNode*          nextInChain;  // 28
	};
	static_assert(sizeof(GFxImportNode) == 0x30);

	class GFxMovieDataDef :
		public GFxTimelineDef,    // 00
		public GFxResourceReport  // 20
	{
	public:
		enum MovieDataType
		{
			kEmpty, // No data, empty clip - useful for LoadVars into new level
			kFlash, // SWF or GFX file
			kImage	// An image file
		};

		class DefBindingData
		{
		public:

			GFxFrameBindData*    frameData;			 // 00 - GAtomicPtr<GFxFrameBindData>
			GFxFrameBindData*    frameDataLast;		 // 08
			GFxImportNode*       imports;			 // 10 - GAtomicPtr<GFxImportNode>
			GFxImportNode*       importsLast;		 // 18
			GFxFontDataUseNode*  fonts;				 // 20 - GAtomicPtr<GFxFontDataUseNode>
			GFxFontDataUseNode*  fontsLast;			 // 28
			GFxResourceDataNode* resourceNodes;		 // 30 - GAtomicPtr<GFxResourceDataNode>
			GFxResourceDataNode* resourceNodesLast;  // 38
		};
		static_assert(sizeof(DefBindingData) == 0x40);

		enum MovieLoadState
		{
			kUninitialized,
			kLoadingFrames,
			kLoadFinished,
			kLoadCanceled,  // Canceled by user
			kLoadError
		};

		class LoadTaskDataBase : public GRefCountBase<LoadTaskDataBase, GFxStatMovieData::kGFxStatMD_Other_Mem>
		{
		public:
			// members
			GFxDataAllocator  tagMemAllocator;  // 10
			GFxPathAllocator* pathAllocator;    // 30
		};
		static_assert(sizeof(LoadTaskDataBase) == 0x38);

		class LoadTaskData : public LoadTaskDataBase
		{
		public:
			using ResourceHash = GHashUncached<GFxResourceID, GFxResourceSource, GFxResourceID::HashOp>;

			~LoadTaskData() override;  // 00

			// add
			virtual void AddFrameName(const GString& a_label, GFxLog* a_log);  // 01
			virtual void SetLoadingPlaylistFrame(const Frame& a_frame);        // 02
			virtual void SetLoadingInitActionFrame(const Frame& a_frame);      // 03

			// members
			GMemoryHeap*                                                 loadDataHeap;                  // 038
			GPtr<GMemoryHeap>                                            imageHeap;                     // 040
			GString                                                      fileURL;                       // 048
			GFxMovieHeaderData                                           header;                        // 050
			std::uint32_t                                                fileAttributes;                // 0C0
			std::uint32_t                                                padC4;                         // 0C4
			void*                                                        metadata;                      // 0C8
			std::uint32_t                                                metadataSize;                  // 0D0
			stl::enumeration<MovieLoadState, std::uint32_t>              loadState;	                    // 0D4
			std::int32_t                                                 loadingFrame;                  // 0D8
			bool                                                         loadingCancelled;              // 0DC
			std::uint8_t                                                 padDD;                         // 0DD
			std::uint16_t                                                padDE;                         // 0DE
			std::uint32_t                                                tagCount;                      // 0E0
			std::uint32_t                                                padE4;                         // 0E4
			GPtr<GFxLoadUpdateSync>                                      frameUpdate;                   // 0E8
			DefBindingData                                               bindData;				        // 0F0
			std::uint32_t                                                importedResourceIndexCounter;  // 130
			std::uint32_t                                                pad134;                        // 134
			GLock                                                        resourceLock;                  // 138
			ResourceHash                                                 resources;                     // 160
			GStringHash<GFxResourceHandle>                               exports;                       // 168
			GHash<GFxResourceID, GString, GFixedSizeHash<GFxResourceID>> inverseExportsMap;             // 170
			GLock                                                        playlistLock;                  // 178
			GArrayLH<Frame>                                              playlist;                      // 1A0
			GArrayLH<Frame>                                              initActionList;                // 1B8
			std::int32_t                                                 initActionsCount;              // 1D0
			std::uint32_t                                                pad1D4;                        // 1D4
			GStringHash<std::uint32_t>                                   namedFrames;                   // 1D8
			std::uint64_t                                                unk1E0;                        // 1E0
		};
		static_assert(sizeof(LoadTaskData) == 0x1E8);

		~GFxMovieDataDef() override;  // 00

		// override (GFxResource)
		GFxResourceKey     GetKey() override;                     // 01
		std::uint32_t      GetResourceTypeCode() const override;  // 02
		GFxResourceReport* GetResourceReport() override;          // 03

		// override (GFxCharacterDef)
		std::uint32_t GetVersion() const override;  // 08

		// override (GFxTimelineDef)
		std::uint32_t GetFrameCount() const override;															             // 0A
		const Frame   GetPlayList(std::int32_t a_frameNumber) const override;									             // 0B
		bool          GetInitActions(Frame* a_frame, std::int32_t a_frameNumber) const override;				             // 0C
		bool          GetLabeledFrame(const char* a_label, std::uint32_t* a_frameNumber, bool a_translateNumbers) override;  // 0D

		// add
		virtual std::uint32_t GetLoadingFrame() const;	// 0E
		virtual std::uint32_t GetSWFFlags();            // 0F

		// members
		GFxResourceKey                                 resourceKey;   // 28
		stl::enumeration<MovieDataType, std::uint32_t> fileFormat;    // 38
		std::uint32_t                                  pad3C;         // 3C
		GPtr<LoadTaskData>                             loadTaskData;  // 40
	};
	static_assert(sizeof(GFxMovieDataDef) == 0x48);
}
