#pragma once

namespace ARMOR
{
	using Slot = RE::BIPED_MODEL::BipedObjectSlot;
	using Biped = RE::BIPED_OBJECT;
	using HeadPart = RE::BGSHeadPart::HeadPartType;
	using ShaderType = RE::BSShaderMaterial::Feature;

	inline constexpr std::array<Slot, 6> fxSlots = { Slot::kModMouth, Slot::kModChestPrimary, Slot::kModPelvisPrimary, Slot::kModLegRight, Slot::kModChestSecondary, Slot::kModArmRight };
	inline constexpr std::array<Biped, 6> fxBiped = { Biped::kModMouth, Biped::kModChestPrimary, Biped::kModPelvisPrimary, Biped::kModLegRight, Biped::kModChestSecondary, Biped::kModArmRight };
	inline constexpr std::array<HeadPart, 4> headparts = { HeadPart::kMisc, HeadPart::kFace, HeadPart::kEyes, HeadPart::kEyebrows };

	namespace factions
	{
		inline constexpr std::array<std::string_view, 4> effects = {
			"FEC - Charred"sv,
			"FEC - Skeletonized"sv,
			"FEC - Drained"sv,
			"FEC - XRay"sv
		};
	}
	
	namespace extra
	{
		inline constexpr auto TOGGLE = "PO3_TOGGLE"sv;
		inline constexpr auto SKIN_TINT = "PO3_SKINTINT"sv;
		inline constexpr auto HAIR_TINT = "PO3_HAIRTINT"sv;
		inline constexpr auto SKIN_ALPHA = "PO3_SKINALPHA"sv;
		inline constexpr auto FACE_TXST = "PO3_FACETXST"sv;
		inline constexpr auto HEADPART = "PO3_HEADPART"sv;
		inline constexpr auto TXST = "PO3_TXST"sv;
		inline constexpr auto SKIN_TXST = "PO3_SKINTXST"sv;

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
	}

	namespace geometry
	{
		inline constexpr auto head = "fec_head"sv;
		inline constexpr auto headXRay = "fec_xray_head"sv;
		inline constexpr auto body = "fec_body"sv;
		inline constexpr auto bodyCharred = "fec_charred_body"sv;
	}

	namespace underwear
	{
		inline constexpr auto male0 = "MaleUnderwear_1"sv;
		inline constexpr auto male1 = "MaleUnderwear"sv;
		inline constexpr auto female = "FemaleUnderwear"sv;
		inline constexpr auto himboBoxers = "HIMBO - Boxers"sv;
		inline constexpr auto himboThong = "HIMBO - Thong"sv;
		inline constexpr auto himboBriefs = "HIMBO - Briefs"sv;
		inline constexpr auto bra0 = "Bra"sv;
		inline constexpr auto bra1 = "BraBowTies1"sv;
		inline constexpr auto bra2 = "BraBowTies2"sv;
		inline constexpr auto panty0 = "Panty"sv;
		inline constexpr auto panty1 = "PantyBowTies1"sv;
		inline constexpr auto panty2 = "PantyBowTies2"sv;

		inline constexpr std::array<std::string_view, 13> underwears{
			male0,
			male1,
			female,
			himboBoxers,
			himboThong,
			himboBriefs,
			bra0,
			bra1,
			bra2,
			panty0,
			panty1,
			panty2
		};
	}

	namespace ATTACH
	{
		void Install();
	}

	namespace DETACH
	{
		void Install();
	}
}

namespace RESET
{
	namespace factions
	{
		inline auto constexpr effectReset = "FEC - Reset"sv;
		
		inline constexpr std::array<std::string_view, 2> effectDone = {
			"FEC - Effects Done"sv,
			"FEC - Temp Effects Done"sv
		};
	}

	class TESResetEventHandler final : public RE::BSTEventSink<RE::TESResetEvent>
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

namespace GRAPHICS
{
	void Install();
}
