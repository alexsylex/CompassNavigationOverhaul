#pragma once

#include "utils/Trampoline.h"

#include "RE/B/BSCoreTypes.h"
#include "RE/C/Compass.h"
#include "RE/H/HUDMarkerManager.h"
#include "RE/N/NiPoint3.h"

namespace hooks
{
	class HUDMarkerManager
	{
#if BUILD_SE
		static constinit inline REL::ID ProcessQuestMarkerId{ 50826 };
		static constinit inline REL::ID ProcessLocationMarkersId{ 50870 };
		static constinit inline REL::ID UpdateHUDMarkerId{ 50851 };
#else
		static constinit inline REL::ID ProcessQuestMarkerId{ 0 };
		static constinit inline REL::ID ProcessLocationMarkerId{ 0 };
		static constinit inline REL::ID UpdateHUDMarkerId{ 0 };
#endif
	public:

		static inline REL::Relocation<bool (*)(const RE::HUDMarkerManager*, RE::TESQuest**,
											   RE::BSTArray<RE::UnkValue>*)> ProcessQuestMarker{ ProcessQuestMarkerId };

		static inline REL::Relocation<bool (*)(const RE::HUDMarkerManager*)> ProcessLocationMarkers{ ProcessLocationMarkersId };

		static inline REL::Relocation<bool (*)(const RE::HUDMarkerManager*, RE::HUDMarker::ScaleformData*,
											   RE::NiPoint3*, const RE::RefHandle&, std::int32_t)> UpdateHUDMarker{ UpdateHUDMarkerId };
	};

	class HUDMenu
	{
#if BUILD_SE
		static constinit inline REL::ID ProcessMessageId{ 50718 };
#else
		static constinit inline REL::ID ProcessMessageId{ 0 };
#endif
	public:

		static inline REL::Relocation<RE::UI_MESSAGE_RESULTS (*)(const RE::HUDMenu*, RE::UIMessage&)> ProcessMessage{ ProcessMessageId };
	};

	class Compass
	{
#if BUILD_SE
		static constinit inline REL::ID SetMarkersId{ 50775 };
#else
		static constinit inline REL::ID SetMarkersId{ 0 };
#endif
	public:

		static inline REL::Relocation<bool (*)(RE::Compass*)> SetMarkers{ SetMarkersId };
	};

	bool UpdateQuests(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
							RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame,
							RE::TESQuest*& a_quest);

	RE::TESWorldSpace* AllowedToShowMapMarker(const RE::TESObjectREFR* a_marker);

	bool UpdateLocations(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
							   RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame);

	bool UpdateEnemies(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
							RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame);

	bool UpdatePlayerSetMarker(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
								RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame);

	bool SetCompassMarkers(RE::GFxValue::ObjectInterface* a_objectInterface, void* a_data, 
						   RE::GFxValue* a_result, const char* a_name, const RE::GFxValue* a_args, 
						   std::uint32_t a_numArgs, bool a_isDObj);

	static inline void Install()
	{
		// HUDMarkerManager::ProcessQuestMarker (call to HUDMarkerManager::AddMarker(...))
		{
			static std::uintptr_t hookedAddress = HUDMarkerManager::ProcessQuestMarker.address() + 0x114;

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					mov(ptr[rsp + 0x28], r14);	// r14 = TESQuest**
					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdateQuests));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator());
		}

		// HUDMarkerManager::ProcessLocationMarkers (calls to TESObjectREFR::GetWorldspace())
		{
			static std::uintptr_t hookedAddress1 = HUDMarkerManager::ProcessLocationMarkers.address() + 0x139;
			static std::uintptr_t hookedAddress2 = HUDMarkerManager::ProcessLocationMarkers.address() + 0x21C;

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator(std::uintptr_t a_hookAddress)
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&AllowedToShowMapMarker));
					L(retnLabel), dq(a_hookAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress1, HookCodeGenerator(hookedAddress1));
			utils::WriteBranchTrampoline<5>(hookedAddress2, HookCodeGenerator(hookedAddress2));
		}

		// HUDMarkerManager::ProcessLocationMarkers (call to HUDMarkerManager::AddMarker(...))
		{
			static std::uintptr_t hookedAddress = HUDMarkerManager::ProcessLocationMarkers.address() + 0x450;

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdateLocations));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator());
		}

		// HUDMenu::ProcessMessage (Enemies)
		{
			static std::uintptr_t hookedAddress = HUDMenu::ProcessMessage.address() + 0x15AB;

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdateEnemies));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator());
		}

		// Compass::SetMarkers (Player-set marker)
		{
			static std::uintptr_t hookedAddress = Compass::SetMarkers.address() + 0x8D;

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdatePlayerSetMarker));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator());
		}

		// Compass::SetMarkers (Invoke AS2 "SetCompassMarkers")
		{
			static std::uintptr_t hookedAddress = Compass::SetMarkers.address() + 0x10F;

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&SetCompassMarkers));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, HookCodeGenerator());
		}
	}
}
