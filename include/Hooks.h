#pragma once

#include "utils/Trampoline.h"

#include "RE/B/BSCoreTypes.h"
#include "RE/H/HUDMarkerManager.h"
#include "RE/N/NiPoint3.h"

namespace HCN
{
	class BSScaleformManager
	{
#if BUILD_SE
		static constinit inline REL::ID LoadMovieId{ 80302 };
#else
		static constinit inline REL::ID LoadMovieId{ 80302 };
#endif
	public:

		static inline REL::Relocation<bool (*)(const RE::BSScaleformManager*, RE::IMenu*,
			RE::GPtr<RE::GFxMovieView>&, const char*, RE::GFxMovieView::ScaleModeType, float)>
			LoadMovie{ LoadMovieId };
	};

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
			RE::BSTArray<RE::UnkValue>*)>
			ProcessQuestMarker{ ProcessQuestMarkerId };

		static inline REL::Relocation<bool (*)(const RE::HUDMarkerManager*)> ProcessLocationMarkers{ ProcessLocationMarkersId };

		static inline REL::Relocation<bool (*)(const RE::HUDMarkerManager*, RE::HUDMarker::ScaleformData*,
			RE::NiPoint3*, const RE::RefHandle&, std::int32_t)>
			UpdateHUDMarker{ UpdateHUDMarkerId };
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

	void PatchGFxMovie(RE::GFxMovieView* a_viewOut, float a_deltaT, std::uint32_t a_frameCatchUpCount);

	bool ProcessQuestHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId,
		RE::TESQuest*& a_quest);

	bool ProcessLocationHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId);

	bool ProcessEnemyHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId);

	bool ProcessPlayerSetHook(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
		RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::int32_t a_markerId);

	static inline void InstallHooks()
	{
		// BSScaleformManager::LoadMovie
		{
			static std::uintptr_t hookedAddress = BSScaleformManager::LoadMovie.address() + 0x39B;

			struct HookCode : Xbyak::CodeGenerator
			{
				HookCode()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&PatchGFxMovie));
					L(retnLabel), dq(hookedAddress + 6);
				}
			};

			utils::WriteBranchTrampoline<6>(hookedAddress, HookCode());
		}

		// HUDMarkerManager::ProcessQuestMarker
		{
			static std::uintptr_t hookedAddress = HUDMarkerManager::ProcessQuestMarker.address() + 0x114;

			struct Hook : Xbyak::CodeGenerator
			{
				Hook()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					sub(rsp, 0x30);
					mov(ptr[rsp + 0x28], r14);	// r14 = TESQuest**
					call(ptr[rip + hookLabel]);
					add(rsp, 0x30);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&ProcessQuestHook));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, Hook());
		}

		// HUDMarkerManager::ProcessLocationMarkers
		{
			static std::uintptr_t hookedAddress = HUDMarkerManager::ProcessLocationMarkers.address() + 0x450;

			struct Hook : Xbyak::CodeGenerator
			{
				Hook()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&ProcessLocationHook));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, Hook());
		}

		// HUDMenu::ProcessMessage (Enemies)
		{
			static std::uintptr_t hookedAddress = HUDMenu::ProcessMessage.address() + 0x15AB;

			struct Hook : Xbyak::CodeGenerator
			{
				Hook()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&ProcessEnemyHook));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, Hook());
		}

		// Compass::SetMarkers (Player-set marker)
		{
			static std::uintptr_t hookedAddress = Compass::SetMarkers.address() + 0x8D;

			struct Hook : Xbyak::CodeGenerator
			{
				Hook()
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&ProcessPlayerSetHook));
					L(retnLabel), dq(hookedAddress + 5);
				}
			};

			utils::WriteBranchTrampoline<5>(hookedAddress, Hook());
		}
	}
}
