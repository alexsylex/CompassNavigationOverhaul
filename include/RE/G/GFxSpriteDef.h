#pragma once

#include "RE/G/GArrayLH.h"
#include "RE/G/GFxTimelineDef.h"
#include "RE/G/GStringHash.h"

namespace RE
{
	class GFxCharacter;
	class GFxMovieDataDef;
	class GFxScale9Grid;

	class GFxTimelineIODef : public GFxTimelineDef
	{
	public:
		// add
		virtual void SetLoadingPlaylistFrame(const Frame& a_frame) = 0;	      // 0E
		virtual void AddFrameName(const GString& a_name, GFxLog* a_log) = 0;  // 0F
	};
	static_assert(sizeof(GFxTimelineIODef) == 0x20);

	class GFxSpriteDef : public GFxTimelineIODef
	{
	public:
		~GFxSpriteDef() override;  // 00

		// override (GFxResource)
		std::uint32_t GetResourceTypeCode() const override;  // 02

		// override (GFxCharacterDef)
		bool          DefPointTestLocal(const GPointF& a_pt, bool a_testShape, const GFxCharacter* a_character) const override;              // 05
		std::uint32_t GetVersion() const override;                                                                                           // 08
		GFxCharacter* CreateCharacter(GFxASCharacter* a_asCharacter, GFxResourceID a_resourceID, GFxMovieDefImpl* a_movieDefImpl) override;	 // 09

		// override (GFxTimelineDef)
		std::uint32_t	GetFrameCount() const override;                                                                        // 0A
		const Frame		GetPlayList(std::int32_t a_frameNumber) const override;												   // 0B
		bool			GetInitActions(Frame* a_frame, std::int32_t a_frameNumber) const override;							   // 0C - { return false; }
		bool			GetLabeledFrame(const char* a_label, std::uint32_t* a_frameNumber, bool a_translateNumbers) override;  // 0D

		// override (GFxTimelineIODef)
		void SetLoadingPlaylistFrame(const Frame& a_frame) override;       // 0E
		void AddFrameName(const GString& a_name, GFxLog* a_log) override;  // 0F

		// add
		virtual float            GetWidth() const;      // 10 - { return 1.0f; }
		virtual float            GetHeight() const;     // 11 - { return 1.0f; }
		virtual float            GeFrameRate() const;   // 12
		virtual GRectF           GetFrameRect() const;  // 13
		virtual std::int32_t     GetLoadingFrame();     // 14
		virtual std::uint32_t    GetSWFFlags();         // 15
		virtual std::uint32_t    GetTagCount() const;	// 16 - { return 0; }
		virtual std::uint64_t    GetFileBytes() const;  // 17
		virtual const char*      GetFileURL();          // 18

		// members
		GFxMovieDataDef*          movieData;     // 20
		GStringHash<std::int32_t> frameLabels;   // 28
		std::int32_t              frameCount;    // 30
		std::int32_t              frameLoading;  // 34
		GArrayLH<Frame>           playlist;      // 38
		GFxScale9Grid*            scale9Grid;	 // 50
		std::uint8_t              buttonState;   // 58 - 0x1 = up, 0x2 = down, 0x4 = over
		std::uint8_t              pad59;         // 59
		std::uint16_t             pad5A;         // 5A
		std::uint32_t             pad5C;         // 5C
	};
	static_assert(sizeof(GFxSpriteDef) == 0x60);
}
