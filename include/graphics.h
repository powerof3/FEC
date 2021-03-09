#pragma once

namespace
{
	inline constexpr std::array<std::string_view, 4> effectFac = {
		"FEC - Charred"sv,
		"FEC - Skeletonized"sv,
		"FEC - Drained"sv,
		"FEC - XRay"sv
	};

	inline constexpr std::array<std::string_view, 4> effectDoneFac = {
		"FEC - Effects Done"sv,
		"FEC - Temp Effects Done"sv
	};

	inline auto constexpr NPC("ActorTypeNPC");
	inline auto constexpr IsBeast("IsBeastRace");
}


namespace Geometry
{
	inline constexpr const char* head = "fec_head";
	inline constexpr const char* xrayHead = "fec_xray_head";
	inline constexpr const char* body = "fec_body";
	inline constexpr const char* bodyCharred = "fec_charred_body";
}


namespace Underwear
{
	inline constexpr const char* male0 = "MaleUnderwear_1";
	inline constexpr const char* male1 = "MaleUnderwear";
	inline constexpr const char* female = "FemaleUnderwear";
	inline constexpr const char* bra0 = "Bra";
	inline constexpr const char* bra1 = "BraBowTies1";
	inline constexpr const char* bra2 = "BraBowTies2";
	inline constexpr const char* panty0 = "Panty";
	inline constexpr const char* panty1 = "PantyBowTies1";
	inline constexpr const char* panty2 = "PantyBowTies2";
}


namespace ARMOR
{
	using Slot = RE::BIPED_MODEL::BipedObjectSlot;
	using Biped = RE::BIPED_OBJECT;
	using HeadPart = RE::BGSHeadPart::HeadPartType;

	inline constexpr std::array<Slot, 6> fxSlots = { Slot::kModMouth, Slot::kModChestPrimary, Slot::kModPelvisPrimary, Slot::kModLegRight, Slot::kModChestSecondary, Slot::kModArmRight };

	inline constexpr std::array<Biped, 6> fxBiped = { Biped::kModMouth, Biped::kModChestPrimary, Biped::kModPelvisPrimary, Biped::kModLegRight, Biped::kModChestSecondary, Biped::kModArmRight };

	inline constexpr std::array<HeadPart, 4> headparts = { HeadPart::kMisc, HeadPart::kFace, HeadPart::kEyes, HeadPart::kEyebrows };

	inline constexpr std::array<std::string_view, 8> underwear = {
		"MaleUnderwear_1"sv,
		"MaleUnderwear"sv
		"FemaleUnderwear"sv,
		"Bra"sv,
		"BraBowTies1"sv,
		"BraBowTies2"sv,
		"Panty"sv,
		"PantyBowTies1"sv,
		"PantyBowTies2"sv
	};

	class Graphics
	{
	public:
		static void SetSkinAlpha(RE::NiAVObject* a_root, float a_alpha, bool a_setData = true);

		static void SetSkinAlpha(RE::NiAVObject* a_root, RE::NiAVObject* a_node, float a_alpha, bool a_setData = true);

		static void ToggleNode(RE::NiAVObject* a_root, const RE::BSFixedString& a_nodeName, bool a_toggle, bool a_setData = true);

		static void ToggleNode(RE::NiAVObject* a_root, RE::NiAVObject* a_node, bool a_toggle, bool a_setData = true);

		static void SetHeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, HeadPart a_type, float a_alpha, bool a_setData = true);
	};


	class Attach : Graphics
	{
	public:
		static void Hook();

	private:
		static void ProcessGeometry(RE::BipedAnim* a_biped, RE::BSGeometry* a_object, RE::BSDismemberSkinInstance* a_dismemberInstance, std::int32_t a_slot, bool a_unk05);
		static inline REL::Relocation<decltype(ProcessGeometry)> _ProcessGeometry;

		static void ProcessObject(RE::BipedAnim* a_biped, RE::NiAVObject* a_object, std::int32_t a_slot, bool a_unk04);
		static inline REL::Relocation<decltype(ProcessObject)> _ProcessObject;

		static void PerformNPCDismember(RE::TESNPC* a_npc, RE::Actor* a_actor, RE::NiAVObject* a_node);
		static inline REL::Relocation<decltype(PerformNPCDismember)> _PerformNPCDismember;
	};


	class Detach : Graphics
	{
	public:
		static void Hook();

	private:
		static void UpdateCollision(RE::NiAVObject* a_node, bool a_unk02, bool a_unk03);
		static inline REL::Relocation<decltype(UpdateCollision)> _UpdateCollision;

		static void ResetActor(RE::NiAVObject* a_object, RE::Actor* a_actor);
	};
}

namespace RESET
{
	inline auto constexpr effectResetFac = "FEC - Reset"sv;

	class TESResetEventHandler : public RE::BSTEventSink<RE::TESResetEvent>
	{
	public:
		static TESResetEventHandler* GetSingleton();

		RE::BSEventNotifyControl ProcessEvent(const RE::TESResetEvent* evn, RE::BSTEventSource<RE::TESResetEvent>* a_eventSource) override;

	protected:
		TESResetEventHandler() = default;
		TESResetEventHandler(const TESResetEventHandler&) = delete;
		TESResetEventHandler(TESResetEventHandler&&) = delete;
		virtual ~TESResetEventHandler() = default;

		TESResetEventHandler& operator=(const TESResetEventHandler&) = delete;
		TESResetEventHandler& operator=(TESResetEventHandler&&) = delete;
	};
}