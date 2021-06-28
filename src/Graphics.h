#pragma once

namespace util
{
	bool has_base_spell(RE::Actor* a_actor, RE::SpellItem* a_spell);

	template <class T, typename D>
	void add_data_if_none(RE::NiAVObject* a_root, std::string_view a_type, D a_data)
	{
		if (const auto data = a_root->GetExtraData<T>(a_type); !data) {
			const auto newData = T::Create(a_type, a_data);
			if (newData) {
				a_root->AddExtraData(newData);
			}
		}
	}

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

	inline auto constexpr NPC = "ActorTypeNPC"sv;
	inline auto constexpr IsBeast = "IsBeastRace"sv;

	namespace EXTRA
	{
		inline constexpr auto TOGGLE = "PO3_TOGGLE"sv;
		inline constexpr auto SKIN_TINT = "PO3_SKINTINT"sv;
		inline constexpr auto HAIR_TINT = "PO3_HAIRTINT"sv;
		inline constexpr auto SKIN_ALPHA = "PO3_SKINALPHA"sv;
		inline constexpr auto FACE_TXST = "PO3_FACETXST"sv;
		inline constexpr auto HEADPART = "PO3_HEADPART"sv;
		inline constexpr auto TXST = "PO3_TXST"sv;
		inline constexpr auto SKIN_TXST = "PO3_SKINTXST"sv;
	}

	namespace GEO
	{
		inline constexpr auto head = "fec_head"sv;
		inline constexpr auto xrayHead = "fec_xray_head"sv;
		inline constexpr auto body = "fec_body"sv;
		inline constexpr auto bodyCharred = "fec_charred_body"sv;
	}

	namespace UNDERWEAR
	{
		inline constexpr auto male0 = "MaleUnderwear_1"sv;
		inline constexpr auto male1 = "MaleUnderwear"sv;
		inline constexpr auto female = "FemaleUnderwear"sv;
		inline constexpr auto bra0 = "Bra"sv;
		inline constexpr auto bra1 = "BraBowTies1"sv;
		inline constexpr auto bra2 = "BraBowTies2"sv;
		inline constexpr auto panty0 = "Panty"sv;
		inline constexpr auto panty1 = "PantyBowTies1"sv;
		inline constexpr auto panty2 = "PantyBowTies2"sv;

		inline constexpr std::array<std::string_view, 8> underwears{
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
	}
}

namespace GRAPHICS
{
	using HeadPart = RE::BGSHeadPart::HeadPartType;

	void SetSkinAlpha(RE::NiAVObject* a_root, float a_alpha, bool a_setData = true);

	void SetSkinAlpha(RE::NiAVObject* a_root, RE::NiAVObject* a_node, float a_alpha, bool a_setData = true);

	void ToggleNode(RE::NiAVObject* a_root, const RE::BSFixedString& a_nodeName, bool a_toggle, bool a_setData = true);

	void ToggleNode(RE::NiAVObject* a_root, RE::NiAVObject* a_node, bool a_toggle, bool a_setData = true);

	void SetHeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, HeadPart a_type, float a_alpha, bool a_setData = true);
}

namespace ARMOR
{
	using Slot = RE::BIPED_MODEL::BipedObjectSlot;
	using Biped = RE::BIPED_OBJECT;
	using HeadPart = RE::BGSHeadPart::HeadPartType;

	inline constexpr std::array<Slot, 6> fxSlots = { Slot::kModMouth, Slot::kModChestPrimary, Slot::kModPelvisPrimary, Slot::kModLegRight, Slot::kModChestSecondary, Slot::kModArmRight };

	inline constexpr std::array<Biped, 6> fxBiped = { Biped::kModMouth, Biped::kModChestPrimary, Biped::kModPelvisPrimary, Biped::kModLegRight, Biped::kModChestSecondary, Biped::kModArmRight };

	inline constexpr std::array<HeadPart, 4> headparts = { HeadPart::kMisc, HeadPart::kFace, HeadPart::kEyes, HeadPart::kEyebrows };

	namespace Attach
	{
		void Install();
	}

	namespace Detach
	{
		void Install();
	}
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
