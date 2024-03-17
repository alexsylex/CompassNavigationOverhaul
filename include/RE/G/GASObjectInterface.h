#pragma once

#include "RE/G/GASString.h"

namespace RE
{
	class GASObject;
	class GASValue;
	class GASMember;
	class GASPropFlags;
	class GASFunctionRef;
	class GASFunctionObject;
	class GASEnvironment;

	class GASObjectInterface
	{
	public:

		enum ObjectType : std::uint8_t
		{
			kUnknown,

			// This type is for non-scriptable characters; it is not ObjectInterface.
			kBaseCharacter,

			// These need to be grouped in range for IsASCharacter().
			kSprite,
			kButton,
			kTextField,
			kVideo,

			kASObject,
			kArray,
			kString,
			kNumber,
			kBoolean,
			kMovieClipObject,
			kButtonASObject,
			kTextFieldASObject,
			kVideoASObject,
			kMatrix,
			kPoint,
			kRectangle,
			kColorTransform,
			kCapabilities,
			kTransform,
			kColor,
			kKey,
			kFunction,
			kStage,
			kMovieClipLoader,
			kBitmapData,
			kLoadVars,
			kXML,
			kXMLNode,
			kTextFormat,
			kStyleSheet,
			kSound,
			kNetConnection,
			kNetStream,
			kDate,
			kAsBroadcaster,
			kBitmapFilter,
			kDropShadowFilter,
			kGlowFilter,
			kBlurFilter,
			kBevelFilter,
			kColorMatrixFilter,
			kTextSnapshot,
			kSharedObject
		};

		struct MemberVisitor
		{
			virtual ~MemberVisitor() {}

			virtual void Visit(const GASString& name, const GASValue& val, std::uint8_t flags) = 0;
		};

		struct UserDataHolder;

		virtual ~GASObjectInterface();  // 00

		// add
		virtual char const*    GetTextValue(GASEnvironment*) const;                                                                  // 01
		virtual ObjectType     GetObjectType2() const;                                                                               // 02
		virtual bool           SetMember(GASEnvironment*, GASString const&, GASValue const&, GASPropFlags const&);                   // 03
		virtual bool           GetMember(GASEnvironment*, const GASString&, GASValue*) = 0;                                          // 04
		virtual bool           FindMember(GASStringContext*, const GASString&, GASMember*) = 0;                                      // 05
		virtual bool           DeleteMember(GASStringContext*, const GASString&) = 0;                                                // 06
		virtual bool           SetMemberFlags(GASStringContext*, const GASString&, std::uint8_t) = 0;                                // 07
		virtual void           VisitMembers(GASStringContext*, MemberVisitor*, std::uint32_t, const GASObjectInterface*) const = 0;  // 08
		virtual bool           HasMember(GASStringContext*, const GASString&, bool) = 0;                                             // 09
		virtual bool           SetMemberRaw(GASStringContext*, const GASString&, const GASValue&, const GASPropFlags&) = 0;          // 0A
		virtual bool           GetMemberRaw(GASStringContext*, const GASString&, GASValue*) = 0;                                     // 0B
		virtual GASFunctionRef ToFunction();                                                                                         // 0C
		virtual void           Set__proto__(GASStringContext*, GASObject*) = 0;                                                      // 0D
		virtual GASFunctionRef Get__constructor__(GASStringContext*);                                                                // 0E
		virtual bool           IsSuper() const;                                                                                      // 0F
		virtual bool           IsBuiltinPrototype() const;                                                                           // 10
		virtual void           AddInterface(GASStringContext*, int, GASFunctionObject*);                                             // 11
		virtual bool           InstanceOf(GASEnvironment*, const GASObject*, bool) const;                                            // 12
		virtual bool           Watch(GASStringContext*, const GASString&, const GASFunctionRef&, const GASValue&);                   // 13
		virtual bool           Unwatch(GASStringContext*, const GASString&);                                                         // 14

		// members
		UserDataHolder* userDataHolder;  // 08
		GASObject*      proto;           // 10
	};
	static_assert(sizeof(GASObjectInterface) == 0x18);

}