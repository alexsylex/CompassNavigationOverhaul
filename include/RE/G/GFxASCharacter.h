#pragma once

#include "RE/G/GASObjectInterface.h"
#include "RE/G/GFxCharacter.h"

namespace RE
{
	class GASMovieClipObject;
	class GFxMovieDef;

	// InteractiveObject in Scaleform 4.1
	class GFxASCharacter :
		public GFxCharacter,	   // 000
		public GASObjectInterface  // 098
	{
	public:
		virtual ~GFxASCharacter();	// 00

		// override (GFxCharacter)
		void UpdateAlphaFlag() override;										  // 01
		bool GetVisible() const override;										  // 04
		GRenderer::BlendType GetBlendMode() const override;						  // 05
		bool GetContinueAnimationFlag() const override;							  // 06
		bool GetAcceptAnimMoves() const override;								  // 07
		void SetAcceptAnimMoves(bool) override;									  // 08
		void SetBlendMode(GRenderer::BlendType) override;						  // 09
		void SetName(const GASString&) override;								  // 0A
		void SetOriginalName(const GASString&) override;						  // 0B
		void SetFilters(GArray<GFxFilterDesc, 2, GArrayDefaultPolicy>) override;  // 0C
		void OnEventUnload() override;											  // 22
		bool OnUnloading() override;											  // 23
		GASObjectInterface::ObjectType GetObjectType() const override;			  // 2C
		bool IsUsedAsMask() const override;										  // 2D
		void SetStateChangeFlags(std::uint8_t) override;						  // 2E
		std::uint8_t GetStateChangeFlags() const override;						  // 2F
		void OnRendererChanged() override;										  // 30

		// override (GASObjectInterface)
		GASObjectInterface::ObjectType GetObjectType2() const override;													// 02
		bool SetMember(GASEnvironment*, GASString const&, GASValue const&, GASPropFlags const&) override;				// 03
		bool GetMember(GASEnvironment*, const GASString&, GASValue*) override;											// 04
		bool FindMember(GASStringContext*, const GASString&, GASMember*) override;										// 05
		bool DeleteMember(GASStringContext*, const GASString&) override;												// 06
		bool SetMemberFlags(GASStringContext*, const GASString&, std::uint8_t) override;								// 07
		void VisitMembers(GASStringContext*, MemberVisitor*, std::uint32_t, const GASObjectInterface*) const override;	// 08
		bool HasMember(GASStringContext*, const GASString&, bool) override;												// 09
		bool SetMemberRaw(GASStringContext*, const GASString&, const GASValue&, const GASPropFlags&) override;			// 0A
		bool GetMemberRaw(GASStringContext*, const GASString&, GASValue*) override;										// 0B
		void Set__proto__(GASStringContext* a_stringContext, GASObject* a_object) override;								// 0D
		bool InstanceOf(GASEnvironment*, const GASObject*, bool) const override;										// 12
		bool Watch(GASStringContext*, const GASString&, const GASFunctionRef&, const GASValue&) override;				// 13
		bool Unwatch(GASStringContext*, const GASString&) override;														// 14

		// add
		virtual void Unk_31(void);					 // 31
		virtual void Unk_32(void);					 // 32
		virtual void Unk_33(void);					 // 33
		virtual void Unk_34(void);					 // 34
		virtual void Unk_35(void);					 // 35 - pure
		virtual void Unk_36(void);					 // 36
		virtual void Unk_37(void);					 // 37
		virtual void Unk_38(void);					 // 38
		virtual void Unk_39(void);					 // 39
		virtual void Unk_3A(void);					 // 3A
		virtual void Unk_3B(void);					 // 3B
		virtual void Unk_3C(void);					 // 3C
		virtual void Unk_3D(void);					 // 3D
		virtual void Unk_3E(void);					 // 3E
		virtual void Unk_3F(void);					 // 3F
		virtual void Unk_40(void);					 // 40
		virtual GASMovieClipObject* GetMovieClip();	 // 41 - { return nullptr; }
		virtual void Unk_42(void);					 // 42
		virtual void Unk_43(void);					 // 43
		virtual void Unk_44(void);					 // 44
		virtual void Unk_45(void);					 // 45
		virtual void Unk_46(void);					 // 46
		virtual void Unk_47(void);					 // 47
		virtual void Unk_48(void);					 // 48
		virtual void Unk_49(void);					 // 49
		virtual void Unk_4A(void);					 // 4A
		virtual void Unk_4B(void);					 // 4B
		virtual void Unk_4C(void);					 // 4C
		virtual void Unk_4D(void);					 // 4D
		virtual void Unk_4E(void);					 // 4E
		virtual void Unk_4F(void);					 // 4F
		virtual void Unk_50(void);					 // 50
		virtual void Unk_51(void);					 // 51
		virtual void Unk_52(void);					 // 52
		virtual void Unk_53(void);					 // 53
		virtual void Unk_54(void);					 // 54
		virtual void Unk_55(void);					 // 55
		virtual void Unk_56(void);					 // 56
		virtual void Unk_57(void);					 // 57

		// members
		std::uint64_t unk0B0;	// 0B0
		std::uint64_t unk0B8;	// 0B8
		std::uint64_t unk0C0;	// 0C0
		GFxMovieDef* movieDef;	// 0C8
		std::uint64_t unk0D0;	// 0D0
		std::uint64_t unk0D8;	// 0D0
		std::uint64_t unk0E0;	// 0D0
		std::uint64_t unk0E8;	// 0D0
		std::uint64_t unk0F0;	// 0D0
		std::uint64_t unk0F8;	// 0D0
		std::uint64_t unk100;	// 0D0
		std::uint64_t unk118;	// 0D0
	};
	static_assert(sizeof(GFxASCharacter) == 0x110);
}
