#pragma once

#include "utils/Trampoline.h"

#include "RE/B/BSCoreTypes.h"
#include "RE/C/Compass.h"
#include "RE/H/HUDMarkerManager.h"
#include "RE/N/NiPoint3.h"

namespace hooks
{
	bool UpdateQuests(const RE::HUDMarkerManager* a_hudMarkerManager, RE::HUDMarker::ScaleformData* a_markerData,
					  RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame,
					  RE::TESQuestTarget* a_questTargets);

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

	class HUDMarkerManager
	{
		static constexpr REL::RelocationID UpdateQuestsId{ 50826, 51691 };
		static constexpr REL::RelocationID UpdateLocationsId{ 50870, 51744 };
		static constexpr REL::RelocationID AddMarkerId{ 50851, 51728 };

	public:

		static inline REL::Relocation<bool(*)(const RE::HUDMarkerManager*, RE::TESQuest**,
											  RE::BSTArray<RE::TESQuestTarget>*)> UpdateQuests{ UpdateQuestsId };

		static inline REL::Relocation<bool(*)(const RE::HUDMarkerManager*)> UpdateLocations{ UpdateLocationsId };

		static inline REL::Relocation<bool(*)(const RE::HUDMarkerManager*, RE::HUDMarker::ScaleformData*,
											  RE::NiPoint3*, const RE::RefHandle&, std::int32_t)> AddMarker{ AddMarkerId };
	};

	class HUDMenu
	{
		static constexpr REL::RelocationID ProcessMessageId{ 50718, 51612 };

	public:

		static inline REL::Relocation<RE::UI_MESSAGE_RESULTS(*)(const RE::HUDMenu*, RE::UIMessage&)> ProcessMessage{ ProcessMessageId };
	};

	class Compass
	{
		static constexpr REL::RelocationID SetMarkersId{ 50775, 51670 };
		static constexpr REL::RelocationID UpdateId{ 50773, 51668 };

	public:

		static inline REL::Relocation<bool(*)(RE::Compass*)> SetMarkers{ SetMarkersId };
		static inline REL::Relocation<void(*)(RE::Compass*)> Update{ UpdateId };
	};

	static inline void Install()
	{
		// `HUDMarkerManager::UpdateQuests` (call to `HUDMarkerManager::AddMarker`)
		struct UpdateQuestsHook : Hook<5>
		{
			static std::uintptr_t Address() { return HUDMarkerManager::UpdateQuests.address() + REL::VariantOffset{ 0x114, 0x180, 0x114 }.offset(); }

			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator(std::uintptr_t a_hookedAddress)
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					mov(ptr[rsp + 0x28], rbx);	// rbx = TESQuestTarget*
					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdateQuests));
					L(retnLabel), dq(a_hookedAddress + 5);

					ready();
				}
			};

			UpdateQuestsHook(std::uintptr_t a_hookedAddress) :
				Hook{ a_hookedAddress, HookCodeGenerator{ a_hookedAddress } }
			{}
		};

		// `HUDMarkerManager::UpdateLocations` (calls to `TESObjectREFR::GetWorldspace`)
		struct AllowedToShowMapMarkerHook : Hook<5>
		{
			static std::uintptr_t Address1() { return HUDMarkerManager::UpdateLocations.address() + REL::VariantOffset{ 0x139, 0x13C, 0x139 }.offset(); }
			static std::uintptr_t Address2() { return HUDMarkerManager::UpdateLocations.address() + REL::VariantOffset{ 0x21C, 0x24B, 0x21C }.offset(); }

			AllowedToShowMapMarkerHook(std::uintptr_t a_hookedAddress) :
				Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&AllowedToShowMapMarker) }
			{}
		};

		// `HUDMarkerManager::UpdateLocations` (call to `HUDMarkerManager::AddMarker` for locations)
		struct UpdateLocationsHook : Hook<5>
		{
			static std::uintptr_t Address() { return HUDMarkerManager::UpdateLocations.address() + REL::VariantOffset{ 0x450, 0x473, 0x450 }.offset(); }

			UpdateLocationsHook(std::uintptr_t a_hookedAddress) :
				Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&UpdateLocations) }
			{}
		};

		// `HUDMenu::ProcessMessage` (call to `HUDMarkerManager::AddMarker` for enemies)
		struct UpdateEnemiesHook : Hook<5>
		{
			// Offset changed from 1.6.640 -> 1.6.1130, so we gotta be more granular with the versions
			static std::uintptr_t Address()
			{
				REL::Version version = REL::Module::get().version();
				std::uintptr_t offsetAE = version < SKSE::RUNTIME_SSE_1_6_1130 ? 0x1695 : 0x1735;

				return HUDMenu::ProcessMessage.address() + REL::VariantOffset{ 0x15AB, offsetAE, 0x15AB }.offset();
			}

			UpdateEnemiesHook(std::uintptr_t a_hookedAddress) :
				Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&UpdateEnemies) }
			{}
		};

		// `Compass::SetMarkers` (call to `HUDMarkerManager::AddMarker` for Player-set marker)
		struct UpdatePlayerSetMarkerHook : Hook<5>
		{
			// In AE `Compass::SetMarkers` is inlined in `Compass::Update`
			static std::uintptr_t Address() { return REL::Module::IsAE() ? Compass::Update.address() + 0xAE : Compass::SetMarkers.address() + 0x8D; }

			UpdatePlayerSetMarkerHook(std::uintptr_t a_hookedAddress) :
				Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&UpdatePlayerSetMarker) }
			{}
		};

		// `Compass::SetMarkers` (call to movie->Invoke("SetCompassMarkers", ...))
		struct SetCompassMarkersHook : Hook<5>
		{
			// In AE `Compass::SetMarkers` is inlined in `Compass::Update`
			static std::uintptr_t Address() { return REL::Module::IsAE() ? Compass::Update.address() + 0x128 : Compass::SetMarkers.address() + 0x10F; }

			SetCompassMarkersHook(std::uintptr_t a_hookedAddress) :
				Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&SetCompassMarkers) }
			{}
		};

		UpdateQuestsHook updateQuestsHook{ UpdateQuestsHook::Address() };
		AllowedToShowMapMarkerHook allowedToShowMapMarkerHook[2] = { AllowedToShowMapMarkerHook::Address1(), AllowedToShowMapMarkerHook::Address2() };
		UpdateLocationsHook updateLocationsHook{ UpdateLocationsHook::Address() };
		UpdateEnemiesHook updateEnemiesHook{ UpdateEnemiesHook::Address() };
		UpdatePlayerSetMarkerHook updatePlayerSetMarkerHook{ UpdatePlayerSetMarkerHook::Address() };
		SetCompassMarkersHook setCompassMarkersHook{ SetCompassMarkersHook::Address() };
		
		// The destination of the hook for `AllowedToShowMapMarker` is the same,
		// so we need to allocate memory for it only once
		static DefaultTrampoline defaultTrampoline{ updateQuestsHook.getSize() + allowedToShowMapMarkerHook->getSize() +
													updateLocationsHook.getSize() + updateEnemiesHook.getSize() +
													updatePlayerSetMarkerHook.getSize() + setCompassMarkersHook.getSize() };
		
		defaultTrampoline.write_branch(updateQuestsHook);
		defaultTrampoline.write_call(allowedToShowMapMarkerHook[0]);
		defaultTrampoline.write_call(allowedToShowMapMarkerHook[1]);
		defaultTrampoline.write_call(updateLocationsHook);
		defaultTrampoline.write_call(updateEnemiesHook);
		defaultTrampoline.write_call(updatePlayerSetMarkerHook);
		defaultTrampoline.write_call(setCompassMarkersHook);
	}

	namespace compat
	{
		class MapMarkerFramework
		{
		public:

			static RE::GFxMovieDef* GetCompassMovieDef();

			static inline void Install(SKSE::WinAPI::HMODULE a_moduleHandle)
			{
				std::uintptr_t getCompassMovieDefHookAddress = SigScanner::FindPattern
				<
					"4C 8B F1 "	// mov     r14, rcx
					"48 8B 02 "	// mov     rax, [rdx]
					"48 8B CA "	// mov     rcx, rdx <- We want this address (offset 6)
					"FF 50 08 "	// call    qword ptr [rax+8]
					"?? 8B ?? "	// mov     ??, rax
					"48 85 C0"	// test    rax, rax
				>(a_moduleHandle) + 6;

				// `ImportManager::SetupHUDMenu` call to a_movieView->GetMovieDef()
				struct GetCompassMovieDefHook : Hook<6>
				{
					GetCompassMovieDefHook(std::uintptr_t a_hookedAddress) :
						Hook{ a_hookedAddress, reinterpret_cast<std::uintptr_t>(&GetCompassMovieDef) }
					{}
				};

				GetCompassMovieDefHook getCompassMovieDefHook{ getCompassMovieDefHookAddress };

				static CustomTrampoline mapMarkerFrameworkTrampoline{ "MapMarkerFramework Trampoline", a_moduleHandle,
																	  getCompassMovieDefHook.getSize() };

				mapMarkerFrameworkTrampoline.write_call(getCompassMovieDefHook);
			}

			static inline const SKSE::PluginInfo* pluginInfo = nullptr;
			static inline RE::GFxMovieDef* compassMovieDef = nullptr;
		};
	}
}
