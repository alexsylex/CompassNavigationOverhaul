#pragma once

#include "AutoTrampoline.h"

#include "RE/B/BSCoreTypes.h"
#include "RE/H/HUDMarkerManager.h"
#include "RE/N/NiPoint3.h"

namespace HCN
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

		static inline REL::Relocation<bool (*)(const RE::HUDMarkerManager*, RE::ScaleformHUDMarkerData*,
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

		static inline REL::Relocation<std::uintptr_t> SetMarkers{ SetMarkersId };
	};

	bool ProcessQuestHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
						  RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId,
						  RE::TESQuest*& a_quest);

	bool ProcessLocationHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
							 RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId);

	bool ProcessEnemyHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
						  RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId);

	bool ProcessPlayerSetHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::ScaleformHUDMarkerData* a_markerData,
							  RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId);

	static inline void InstallHooks()
	{
		// HudMarkerManager::ProcessQuestMarker
		{
			struct Hook : Xbyak::CodeGenerator
			{
				Hook()
				{
					Xbyak::Label hookLabel;

					sub(rsp, 0x38);
					mov(ptr[rsp + 0x28], r14);	// r14 = TESQuest**
					call(ptr[rip + hookLabel]);
					add(rsp, 0x38);
					ret();

					L(hookLabel);
					dq(reinterpret_cast<std::uintptr_t>(&ProcessQuestHook));
				}
			};

			SKSE::AutoTrampoline<5>(HUDMarkerManager::ProcessQuestMarker.address() + 0x114, Hook{});
		}

		// HudMarkerManager::ProcessLocationMarkers
		{
			struct Hook : Xbyak::CodeGenerator
			{
				Hook()
				{
					Xbyak::Label hookLabel;

					jmp(ptr[rip + hookLabel]);

					L(hookLabel);
					dq(reinterpret_cast<std::uintptr_t>(&ProcessLocationHook));
				}
			};

			SKSE::AutoTrampoline<5>(HUDMarkerManager::ProcessLocationMarkers.address() + 0x450, Hook{});
		}

		// HUDMenu::ProcessMessage (Enemies)
		{
			struct Hook : Xbyak::CodeGenerator
			{
				Hook()
				{
					Xbyak::Label hookLabel;

					jmp(ptr[rip + hookLabel]);

					L(hookLabel);
					dq(reinterpret_cast<std::uintptr_t>(&ProcessEnemyHook));
				}
			};

			SKSE::AutoTrampoline<5>(HUDMenu::ProcessMessage.address() + 0x15AB, Hook{});
		}

		// Compass::SetMarkers (Player-set marker)
		{
			struct Hook : Xbyak::CodeGenerator
			{
				Hook()
				{
					Xbyak::Label hookLabel;

					jmp(ptr[rip + hookLabel]);

					L(hookLabel);
					dq(reinterpret_cast<std::uintptr_t>(&ProcessPlayerSetHook));
				}
			};

			SKSE::AutoTrampoline<5>(Compass::SetMarkers.address() + 0x8D, Hook{});
		}
	}
}
