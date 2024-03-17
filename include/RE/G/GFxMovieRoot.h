#pragma once

#include "RE/G/GArrayLH.h"
#include "RE/G/GColor.h"
#include "RE/G/GFxKeyboardState.h"
#include "RE/G/GFxMovieDefImpl.h"
#include "RE/G/GFxMovieView.h"
#include "RE/G/GFxMouseState.h"
#include "RE/G/GMatrix2D.h"
#include "RE/G/GStringHash.h"

namespace RE
{
	class GASIntervalTimer;
	class GFxASCharacter;
	class GFxFontManagerStates;
	class GFxIMECandidateListStyle;
	class GFxLoadQueueEntry;
	class GFxMouseState;
	class GFxSprite;
	class GFxStateBagImpl;

	class GFxIMEImm32Dll
	{
	public:

		void(* ImmGetContext)(...);
		void(* ImmReleaseContext)(...);
		void(* ImmGetCompositionStringA)(...);
		void(* ImmGetCompositionStringW)(...);
		void(* ImmIsUIMessageA)(...);
		void(* ImmIsUIMessageW)(...);
		void(* immGetDefaultIMEWnd)(...);
		std::uint64_t libraryA;
		std::uint32_t lastError;
	};

	class GFxMovieRoot : public GFxMovieView
	{
	public:

		struct ReturnValueHolder;

		struct InvokeAliasInfo
		{
			// Unknown
		};

		struct StickyVarNode
		{
			// Unknown
		};

		enum class Flag
		{
			kNone = 0,
			kViewportSet = 1 << 0,
			kCachedLogFlag = 1 << 1,
			kVerboseAction = 1 << 2,
			kLogRootFilenames = 1 << 3,
			kLogChildFilenames = 1 << 4,
			kLogLongFilenames = 1 << 5,
			kSuppressActionErrors = 1 << 6,
			kNeedMouseUpdate = 1 << 7,
			kLevelClipsChanged = 1 << 8,
			kAdvanceCalled = 1 << 9,
			kHasChanges = 1 << 10,
			kNoInvisibleAdvanceFlag = 1 << 11,
			kSetCursorTypeFuncOverloaded = 1 << 12,
			kContinueAnimation = 1 << 13,
			kStageAutoOrients = 1 << 14,
			kOnEventLoadProgressCalled = 1 << 15,
			kDisableFocusAutoRelByClick = 1 << 16,
			kBackgroundSetByTag = 1 << 17,
			kMovieFocused = 1 << 18,
			kOptimizedAdvanceListInvalid = 1 << 19,
			kPaused = 1 << 20,

			kDisableFocusAutoRelease = 0xC00000,
			kAlwaysEnableFocusArrowKeys = 0x3000000,
			kAlwaysEnableKeyboardPress = 0xC000000,
			kDisableFocusRolloverEvent = 0x30000000,
			kDisableFocusKeys = 0xC0000000
		};

		struct LevelInfo
		{
			int level;			// 00
			GFxSprite* sprite;	// 08
		};
		static_assert(sizeof(LevelInfo) == 0x10);

		struct ActionEntry;

		struct ActionQueueEntry
		{
			ActionEntry* actionRoot;
			ActionEntry* insertEntry;
			ActionEntry* lastEntry;
		};
		static_assert(sizeof(ActionQueueEntry) == 0x18);

		struct ActionQueueType
		{
			ActionQueueEntry entries[7];
			std::int32_t modId;
			ActionEntry* freeEntry;
			std::uint32_t currentSessionId;
			std::uint32_t cfreeEntriesCount;
			std::uint32_t lastSessionId;
			GMemoryHeap* heap;
		};
		static_assert(sizeof(ActionQueueType) == 0xD0);

		~GFxMovieRoot() override;  // 00

		// override (GFxMovieView)
		GFxMovieDef* GetMovieDef() const override;																																						   // 01 - Obtains the movie definition that created this instance.
		std::uint32_t GetCurrentFrame() const override;																																					   // 02 - Obtains the currently active frame of the movie, starting with 0.
		bool HasLooped() const override;																																								   // 03 - Determines whether the movie clip has looped past its last frame.
		void GotoFrame(std::uint32_t a_frameNumber) override;																																			   // 04 - Moves a playhead to a specified frame.
		bool GotoLabeledFrame(const char* a_label, std::int32_t a_offset = 0) override;																													   // 05 - Moves a playhead to a specified frame identified by a label.
		void SetPlayState(PlayState a_state) override;																																					   // 06 - Changes playback state of the movie, allowing animation to be paused and resumed.
		PlayState GetPlayState() const override;																																						   // 07 - Obtains the play state of the movie clip, currently either Playing or Stopped.
		void SetVisible(bool a_visible) override;																																						   // 08 - Sets the visibility state of a movie clip.
		bool GetVisible() const override;																																								   // 09 - Obtains the visibility state of a movie clip.
		bool IsAvailable(const char* a_pathToVar) const override;																																		   // 0A - Checks for availability of a field, method, or nested clip.
		void CreateString(GFxValue* a_value, const char* a_string) override;																															   // 0B - Creates strings that are managed by ActionScript runtime.
		void CreateStringW(GFxValue* a_value, const wchar_t* a_string) override;																														   // 0C - Creates wide character strings that are managed by ActionScript runtime.
		void CreateObject(GFxValue* a_value, const char* a_className = 0, const GFxValue* a_args = 0, std::uint32_t a_numArgs = 0) override;															   // 0D - Creates an instance of ActionScript Object.
		void CreateArray(GFxValue* a_value) override;																																					   // 0E - Creates an instance of an ActionScript Array object.
		void CreateFunction(GFxValue* a_value, GFxFunctionHandler* a_fc, void* a_userData = 0) override;																								   // 0F - Creates a function object that wraps a C++ function.
		bool SetVariable(const char* a_pathToVar, const GFxValue& a_value, SetVarType a_setType = SetVarType::kSticky) override;																		   // 10 - Sets a variable identified by a path to a new value, specified in UTF-8 encoding.
		bool GetVariable(GFxValue* a_val, const char* a_pathToVar) const override;																														   // 11 - Obtains the value of an ActionScript variable based on its path.
		bool SetVariableArray(SetArrayType a_type, const char* a_pathToVar, std::uint32_t a_index, const void* a_data, std::uint32_t a_count, SetVarType a_setType = SetVarType::kSticky) const override;  // 12 - Sets array elements in specified range to data items of specified type.
		bool SetVariableArraySize(const char* a_pathToVar, std::uint32_t a_count, SetVarType a_setType = SetVarType::kSticky) override;																	   // 13 - Sets the size of the array.
		std::uint32_t GetVariableArraySize(const char* a_pathToVar) override;																															   // 14 - Returns the size of array buffer necessary for GetVariableArray.
		bool GetVariableArray(SetArrayType a_type, const char* a_pathToVar, std::uint32_t a_index, void* a_data, std::uint32_t a_count) override;														   // 15 - Populates a buffer with results from an ActionScript array.
		bool Invoke(const char* a_methodName, GFxValue* a_result, const GFxValue* a_args, std::uint32_t a_numArgs) override;																			   // 16 - Calls an ActionScript method on the movie clip.
		bool InvokeFmt(const char* a_methodName, GFxValue* a_result, const char* a_argFmt, ...) override;																								   // 17 - Calls an ActionScript method on the movie clip.
		bool InvokeArgs(const char* a_methodName, GFxValue* a_result, const char* a_argFmt, std::va_list a_args) override;																				   // 18 - Invokes an ActionScript method on the movie clip using a format string followed by a variable argument list.
		void SetViewport(const GViewport& a_viewDesc) override;																																			   // 19 - Sets the render-target surface viewport to which the movie is scaled.
		void GetViewport(GViewport* a_viewDesc) const override;																																			   // 1A - Obtains the render-target surface viewport to which the movie is scaled.
		void SetViewScaleMode(ScaleModeType a_type) override;																																			   // 1B - Sets the current scale mode for the movie.
		ScaleModeType GetViewScaleMode() const override;																																				   // 1C - Obtains the current scale mode for the movie.
		void SetViewAlignment(AlignType a_type) override;																																				   // 1D - Sets the current alignment mode for the movie.
		AlignType GetViewAlignment() const override;																																					   // 1E - Gets the current alignment mode for the movie.
		GRectF GetVisibleFrameRect() const override;																																					   // 1F - Obtains the currently visible rectangle.
		void SetPerspective3D(const GMatrix3D& a_projMatIn) override;																																	   // 20
		void SetView3D(const GMatrix3D& a_viewMatIn) override;																																			   // 21
		GRectF GetSafeRect() const override;																																							   // 22
		void SetSafeRect(const GRectF& a_rect) override;																																				   // 23
		void Restart() override;																																										   // 24
		float Advance(float a_deltaT, std::uint32_t a_frameCatchUpCount = 2) override;																													   // 25
		void Display() override;																																										   // 26
		void DisplayPrePass() override;																																									   // 27
		void SetPause(bool a_pause) override;																																							   // 28
		bool IsPaused() const override;																																									   // 29
		void SetBackgroundColor(const GColor a_bgColor) override;																																		   // 2A
		void SetBackgroundAlpha(float a_alpha) override;																																				   // 2B
		float GetBackgroundAlpha() const override;																																						   // 2C
		HEResult HandleEvent(const GFxEvent& a_event) override;																																			   // 2D
		void GetMouseState(std::uint32_t a_mouseIndex, float* a_x, float* a_y, std::uint32_t* a_buttons) override;																						   // 2E
		void NotifyMouseState(float a_x, float a_y, std::uint32_t a_buttons, std::uint32_t a_mouseIndex = 0) override;																					   // 2F
		bool HitTest(float a_x, float a_y, HitTestType a_testCond = HitTestType::kShapes, std::uint32_t a_controllerIdx = 0) override;																	   // 30
		bool HitTest3D(GPoint3F* a_ptOut, float a_x, float a_y, std::uint32_t a_controllerIdx = 0) override;																							   // 31
		void SetExternalInterfaceRetVal(const GFxValue& a_val) override;																																   // 32
		void* GetUserData() const override;																																								   // 33
		void SetUserData(void* a_data) override;																																						   // 34
		bool AttachDisplayCallback(const char* a_pathToObject, void (*a_callback)(void* a_user), void* a_user) override;																				   // 35
		bool IsMovieFocused() const override;																																							   // 36
		bool GetDirtyFlag(bool a_doReset = true) override;																																				   // 37
		void SetMouseCursorCount(std::uint32_t a_count) override;																																		   // 38
		std::uint32_t GetMouseCursorCount() const override;																																				   // 39
		void SetControllerCount(std::uint32_t a_count) override;																																		   // 3A
		std::uint32_t GetControllerCount() const override;																																				   // 3B
		void GetStats(GStatBag* a_bag, bool a_reset = true) override;																																	   // 3C
		GMemoryHeap* GetHeap() const override;																																							   // 3D
		void ForceCollectGarbage() override;																																							   // 3E
		GPointF TranslateToScreen(const GPointF& a_p, void* a_userMatrix) override;																														   // 3F
		GRectF TranslateToScreen(const GRectF& a_p, void* a_userMatrix) override;																														   // 40
		bool TranslateLocalToScreen(const char* a_pathToCharacter, const GPointF& a_pt, GPointF* a_presPt, void* a_userMatrix) override;																   // 41
		bool SetControllerFocusGroup(std::uint32_t a_controllerIdx, std::uint32_t a_focusGroupIndex) override;																							   // 42
		std::uint32_t GetControllerFocusGroup(std::uint32_t a_controllerIdx) const override;																											   // 43
		GFxMovieDef::MemoryContext* GetMemoryContext() const override;																																	   // 44
		void Release() override;																																										   // 45

		// add
		virtual void SetPerspectiveFOV(float a_perspectiveFOV);				// 46
		virtual void SetVerboseAction(bool a_enableVerboseAction);			// 47
		virtual void SetActionErrorsSuppress(bool a_suppressActionErrors);	// 48

		// members
		std::uint64_t unk0018;							  // 0018
		GFxValue::ObjectInterface* objectInterface;		  // 0020
		void* refCountCollector;						  // 0028
		std::uint32_t unk0030;							  // 0030
		std::uint32_t unk0034;							  // 0034
		GMemoryHeap* heap;								  // 0038
		GArrayLH<LevelInfo> movieLevels;				  // 0040
		GFxSprite* sprite;								  // 0058
		GFxMovieDefImpl* movieDef;						  // 0060
		std::uint64_t unk0068;							  // 0068
		std::uint64_t unk0070;							  // 0070
		GViewport viewport;								  // 0078
		//float pixelScale;								  // 00AC
		float viewScaleX;								  // 00B0
		float viewScaleY;								  // 00B4
		float viewOffsetX;								  // 00B8
		float viewOffsetY;								  // 00BC
		ScaleModeType viewScaleMode;					  // 00C0
		AlignType viewAlignment;						  // 00C4
		GRectF visibleFrameRect;						  // 00C8
		GRectF safeRect;								  // 00D8
		GMatrix2D viewportMatrix;						  // 00E8
		GMatrix3D* perspective3D;						  // 0100
		GMatrix3D* view3D;								  // 0108
		std::uint64_t unk0110[(0x0138 - 0x0110) >> 3];	  // 0110
		float perspFOV;									  // 0138
		std::uint32_t unk013C;							  // 013C
		GFxStateBagImpl* stateBag;						  // 0140
		GFxLog* cachedLog;								  // 0148
		std::uint64_t unk0150[(0x0170 - 0x0150) >> 3];	  // 0150
		GFxFontManagerStates* fontManagerStates;		  // 0170
		std::uint64_t unk0178;							  // 0178
		std::uint64_t timeElapsedUsec;					  // 0180
		float timeRemainder;							  // 0188
		float frameTime;								  // 018C
		std::uint32_t forceFrameCatchUp;				  // 0190
		std::uint32_t unk0194;							  // 0194
		std::uint64_t unk0198[(0x09A0 - 0x0198) >> 3];	  // 0198
		GColor backgroundColor;							  // 09A0
		std::uint32_t unk09A4;							  // 09A4
		std::uint64_t unk09A8[(0x0A68 - 0x09A8) >> 3];	  // 09A8
		std::uint32_t mouseCursorCount;					  // 0A68
		std::uint32_t controllerCount;					  // 0A6C
		void* userData;									  // 0A70
		void* unk0A78;									  // 0A78
		GFxKeyboardState keyboardStates[4];				  // 0A80
		ReturnValueHolder* retValHolder;				  // 24A0
		std::uint64_t unk24A8[(0x24D0 - 0x24A8) >> 3];	  // 24A8
		GStringHash<InvokeAliasInfo> invokeAliases;		  // 24D0
		std::uint64_t unk24D8[(0x2508 - 0x24D8) >> 3];	  // 25D8
		GStringHash<StickyVarNode*> stickyVariables;	  // 24A8
		ActionQueueType actionQueue;					  // 2510
		stl::enumeration<Flag, std::uint32_t> flags;	  // 25E0
		std::uint32_t unk25E4;							  // 25E4
		std::uint64_t unk25E8;							  // 25E8
		std::uint64_t unk25F0;							  // 25F0
		std::uint64_t unk25F8;							  // 25F8
		GArrayLH<GFxSprite*> topmostLevelCharacters;	  // 2600
		std::uint64_t pauseTicksMs;						  // 2618
		std::uint64_t startTicksMs;						  // 2620
		GArrayLH<GASIntervalTimer*> intervalTimers;		  // 2628
		std::int32_t lastIntervalTimerId;				  // 2640
		std::uint32_t unk2644;							  // 2644
		std::uint64_t unk2648[(0x2B48 - 0x2648) >> 3];	  // 2648
		std::uint32_t focusGroup;						  // 2B48
		std::uint8_t controllerGroups[16];				  // 2B4C
		std::uint32_t unk2B5C;							  // 2B5C
		std::uint64_t unk2B60;							  // 2B60
		std::uint64_t unk2B68;							  // 2B68
		GFxASCharacter* unloadListHead;					  // 2B70
		GFxIMECandidateListStyle* imeCandidateListStyle;  // 2B78
		GFxIMEImm32Dll imm32Dll;						  // 2B80
		GFxLoadQueueEntry* loadQueueHead;				  // 2BC8
		std::uint32_t lastLoadQueueEntryCount;			  // 2BD0
		std::uint32_t unk2BD4;							  // 2BD4
		std::uint64_t unk2BD8;							  // 2BD8
		std::uint64_t unk2BE0;							  // 2BE0
		std::uint64_t unk2BE8;							  // 2BE8
	};
	static_assert(sizeof(GFxMovieRoot) == 0x2BF0);
}
