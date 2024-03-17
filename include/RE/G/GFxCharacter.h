#pragma once

#include "RE/G/GArray.h"
#include "RE/G/GASObjectInterface.h"
#include "RE/G/GFxCharacterHandle.h"
#include "RE/G/GFxLog.h"
#include "RE/G/GFxPlayerStats.h"
#include "RE/G/GMatrix2D.h"
#include "RE/G/GMatrix3D.h"
#include "RE/G/GPoint.h"
#include "RE/G/GRect.h"
#include "RE/G/GRefCountNTSImpl.h"
#include "RE/G/GRefCountBaseStatImpl.h"
#include "RE/G/GRefCountBaseWeakSupport.h"
#include "RE/G/GRenderer.h"

namespace RE
{
	class GASEnvironment;
	class GASString;
	class GFxASCharacter;
	class GFxEventId;
	class GFxCharacterDef;
	class GFxDisplayContext;
	class GFxFilterDesc;
	class GFxFontManager;
	class GFxMovieDefImpl;
	class GFxMovieRoot;
	class GFxScale9Grid;

	// DisplayObjectBase, DisplayObject in Scaleform 4.1
	class GFxCharacter : 
		public GRefCountBaseWeakSupport<GFxCharacter, GFxStatMovieViews::kGFxStatMV_MovieClip_Mem>,	 // 00
		public GFxLogBase<GFxCharacter>                                                              // 18
	{
	public:

		struct TopMostParams
		{

		};

		enum Flags : uint16_t
		{
			kScale9GridExists = 0x1,
			kTopmostLevel = 0x2,
			kHitTestNegative = 0x4,
			kHitTestPositive = 0x8,
			kHitTest = 0xC,
			kUnloaded = 0x10,
			kJustLoaded = 0x20,
			kMarkedForRemove = 0x40,
			kInteractiveObject = 0x80,
			kScriptableObject = 0x100,
			kDisplayObjContainer = 0x200,
			kSprite = 0x400,
			kMovieClip = 0x800,
			kUnloading = 0x1000,
			kLoaded = 0x2000,
			kVisible = 0x4000,
			kIndirectTransform = 0x8000,
		};

		// add
		virtual void                           UpdateAlphaFlag();											 // 01
		virtual bool                           Has3D() const;												 // 02
		virtual void                           UpdateViewAndPerspective();									 // 03
		virtual bool                           GetVisible() const;											 // 04
		virtual GRenderer::BlendType           GetBlendMode() const;									     // 05
		virtual bool                           GetContinueAnimationFlag() const;							 // 06
		virtual bool                           GetAcceptAnimMoves() const;									 // 07
		virtual void                           SetAcceptAnimMoves(bool);									 // 08
		virtual void                           SetBlendMode(GRenderer::BlendType);							 // 09
		virtual void                           SetName(const GASString&);									 // 0A
		virtual void                           SetOriginalName(const GASString&);							 // 0B
		virtual void                           SetFilters(GArray<GFxFilterDesc, 2, GArrayDefaultPolicy>);	 // 0C
		virtual GRectF                         GetBounds(const GMatrix2D&) const;							 // 0D
		virtual void                           Unk_0E(void);												 // 0E
		virtual void                           Unk_0F(void);												 // 0F
		virtual GRectF                         GetRectBounds(const GMatrix2D&) const;						 // 10
		virtual bool                           PointTestLocal(const GPointF&, std::uint8_t) const;			 // 11
		virtual GFxASCharacter*                GetTopMostMouseEntity(const GPointF&, const TopMostParams&);  // 12
		virtual const GFxScale9Grid*           GetScale9Grid() const;									     // 13
		virtual void                           PropagateScale9GridExists();									 // 14
		virtual GFxMovieRoot*                  GetMovieRoot() const;										 // 15
		virtual GFxCharacterDef*               GetCharacterDef() const = 0;								     // 16
		virtual GFxMovieDefImpl*               GetResourceMovieDef() const;								     // 17
		virtual GFxFontManager*                GetFontManager() const;										 // 18
		virtual GFxASCharacter*                GetLevelMovie(std::int32_t) const;							 // 19
		virtual GFxASCharacter*                GetASRootMovie(bool) const;									 // 1A
		virtual const GASEnvironment*          GetASEnvironment() const;								     // 1B
		virtual GASEnvironment*                GetASEnvironment();											 // 1C
		virtual void                           Display(GFxDisplayContext&);									 // 1D
		virtual void                           Restart();													 // 1E
		virtual void                           AdvanceFrame(bool, float);									 // 1F
		virtual bool                           OnEvent(const GFxEventId&);									 // 20
		virtual void                           OnEventLoad();												 // 21
		virtual void                           OnEventUnload();												 // 22
		virtual bool                           OnUnloading();												 // 23
		virtual void                           DetachChild(GFxASCharacter*);								 // 24
		virtual bool                           OnKeyEvent(const GFxEventId&, std::int32_t*);				 // 25
		virtual bool                           OnCharEvent(std::uint64_t, std::uint32_t);					 // 26
		virtual bool                           OnMouseWheelEvent(std::int32_t);								 // 27
		virtual void                           PropagateMouseEvent(const GFxEventId&);						 // 28
		virtual void                           PropagateKeyEvent(const GFxEventId&, std::int32_t*);			 // 29
		virtual GFxLog*                        GetLog() const;												 // 2A
		virtual bool                           IsVerboseAction() const;										 // 2B
		virtual GASObjectInterface::ObjectType GetObjectType() const;						                 // 2C
		virtual bool                           IsUsedAsMask() const;										 // 2D
		virtual void                           SetStateChangeFlags(std::uint8_t);							 // 2E
		virtual std::uint8_t                   GetStateChangeFlags() const;									 // 2F
		virtual void                           OnRendererChanged();											 // 30

		// members
		GFxResourceID resourceID;   // 20
		std::int32_t depth;         // 24
		std::uint32_t createFrame;  // 28
		float ratio;                // 2C
		GFxASCharacter* parent;     // 30
		GRenderer::Cxform cxform;   // 38
		GMatrix2D matrix;           // 58
		GMatrix3D* matrix3D;        // 70
		GMatrix3D* perspective3D;   // 78
		GMatrix3D* view3D;          // 80
		float perspectiveFOV;       // 88
		std::uint8_t unk8C[8];      // 8C
		std::uint16_t clipDepth;    // 94
		Flags flags;				// 96
	};
	static_assert(sizeof(GFxCharacter) == 0x98);	
}