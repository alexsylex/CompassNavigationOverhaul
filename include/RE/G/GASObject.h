#pragma once

#include "RE/G/GASObjectInterface.h"
#include "RE/G/GASRefCountBase.h"

namespace RE
{
	class GASObject :
		public GASRefCountBase<GASObject>,	// 00
		public GASObjectInterface			// 20
	{
	public:
		// override (GASRefCountBase)
		void Unk_00(void) override;	 // 00
		void Unk_01(void) override;	 // 01

		~GASObject() override;	// 02

		// override (GASObjectInterface)
		char const* GetTextValue(GASEnvironment*) const override;														// 01
		ObjectType GetObjectType2() const override;																		// 02
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
		virtual void Unk_03(void);	// 03
		virtual void Unk_04(void);	// 04
		virtual void Unk_05(void);	// 05
		virtual void Unk_06(void);	// 06
		virtual void Unk_07(void);	// 07
		virtual void Unk_08(void);	// 08

		// members
		std::uint64_t unk38;  // 38 - properties
		std::uint64_t unk40;  // 40 - __resolve handler
		std::uint64_t unk48;  // 48
		std::uint64_t unk50;  // 50
		std::uint64_t unk58;  // 58
		std::uint64_t unk60;  // 60
	};
	static_assert(sizeof(GASObject) == 0x68);
}
