#pragma once

namespace FEC
{
	using Slot = RE::BIPED_MODEL::BipedObjectSlot;
	using Biped = RE::BIPED_OBJECT;
	using HeadPart = RE::BGSHeadPart::HeadPartType;
	using Feature = RE::BSShaderMaterial::Feature;
	using States = RE::BSGeometry::States;
	using Texture = RE::BSTextureSet::Texture;

	namespace color
	{
		inline constexpr RE::Color falmer{ 0x8F7F75 };
		inline constexpr RE::Color giant{ 0x786f6A };
		inline constexpr RE::Color hagraven{ 0x72615B };
		inline constexpr RE::Color riekling{ 0x374655 };
	}

	namespace extra
	{
		inline constexpr auto TOGGLE{ "PO3_TOGGLE"sv };
		inline constexpr auto SKIN_TINT{ "PO3_SKINTINT"sv };
		inline constexpr auto HAIR_TINT{ "PO3_HAIRTINT"sv };
		inline constexpr auto SKIN_ALPHA{ "PO3_SKINALPHA"sv };
		inline constexpr auto FACE_TXST{ "PO3_FACETXST"sv };
		inline constexpr auto HEADPART{ "PO3_HEADPART"sv };
		inline constexpr auto TXST{ "PO3_TXST"sv };
		inline constexpr auto SKIN_TXST{ "PO3_SKINTXST"sv };
		inline constexpr auto SHADER{ "PO3_SHADER |"sv };
		inline constexpr auto ORIG_SHADER{ "PO3_ORIGINALSHADER"sv };
	}

	namespace faction
	{
		inline constexpr RE::FormID falmer{ 0x0002997E };
		inline constexpr RE::FormID giant{ 0x0004359A };
		inline constexpr RE::FormID hagraven{ 0x0004359E };
		inline constexpr RE::FormID riekling{ 0x0401D9C9 };
	}

	namespace geometry
	{
		inline constexpr auto head{ "fec_head"sv };
		inline constexpr auto headXRay{ "fec_xray_head"sv };
		inline constexpr auto body{ "fec_body"sv };
		inline constexpr auto bodyCharred{ "fec_charred_body"sv };
	}

	namespace keyword
	{
		inline constexpr auto Sun{ "PO3_MagicDamageSun"sv };
		inline constexpr auto Fire{ "MagicDamageFire"sv };
		inline constexpr auto Frost{ "MagicDamageFrost"sv };
		inline constexpr auto Shock{ "MagicDamageShock"sv };
		inline constexpr auto Paralysis{ "MagicParalysis"sv };
	}

	namespace shader
	{
		inline constexpr RE::FormID fireFXShader{ 0x0001B212 };
		inline constexpr RE::FormID DLC1_SunCloakSpellHandFX{ 0x0200A3BD };

		inline constexpr std::array<RE::FormID, 3> FEC_fireFXS = { 0x806, 0x826, 0x82F };
		inline constexpr std::array<RE::FormID, 3> sunHitFXS = { 0x02019C9D, 0x0200A3BB, 0x0200A3BC };
		inline constexpr std::array<RE::FormID, 2> sunHitArt = { 0x0200A3B7, 0x0200A3B8 };
	}

	namespace slot
	{
		inline constexpr std::array fxSlots = { Slot::kModMouth, Slot::kModChestPrimary, Slot::kModPelvisPrimary, Slot::kModLegRight, Slot::kModChestSecondary, Slot::kModArmRight };
		inline constexpr std::array fxBiped = { Biped::kModMouth, Biped::kModChestPrimary, Biped::kModPelvisPrimary, Biped::kModLegRight, Biped::kModChestSecondary, Biped::kModArmRight };
		inline constexpr std::array headparts = { HeadPart::kMisc, HeadPart::kFace, HeadPart::kEyes, HeadPart::kEyebrows };
		inline constexpr std::array headSlots = { Biped::kHair, Biped::kLongHair, Biped::kCirclet };
	}

	namespace str
	{
		inline constexpr auto embersXDPath = R"(EmbersHD\mx_fireatlas02.dds)"sv;
	}

	namespace texture
	{
		inline constexpr std::array types{
			Texture::kDiffuse,
			Texture::kNormal,
			Texture::kEnvironmentMask,
			Texture::kGlowMap,
			Texture::kHeight,
			Texture::kEnvironment,
			Texture::kMultilayer,
			Texture::kBacklightMask,
			Texture::kUnused08
		};
	}

	namespace underwear
	{
		inline constexpr auto male0{ "MaleUnderwear_1"sv };
		inline constexpr auto male1{ "MaleUnderwear"sv };
		inline constexpr auto female{ "FemaleUnderwear"sv };
		inline constexpr auto himboBoxers{ "HIMBO - Boxers"sv };
		inline constexpr auto himboThong{ "HIMBO - Thong"sv };
		inline constexpr auto himboBriefs{ "HIMBO - Briefs"sv };
		inline constexpr auto bra0{ "Bra"sv };
		inline constexpr auto bra1{ "BraBowTies1"sv };
		inline constexpr auto bra2{ "BraBowTies2"sv };
		inline constexpr auto panty0{ "Panty"sv };
		inline constexpr auto panty1{ "PantyBowTies1"sv };
		inline constexpr auto panty2{ "PantyBowTies2"sv };

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
}

namespace ver
{
	inline constexpr auto PapyrusExtender{ "po3_PapyrusExtender"sv };
	inline constexpr auto PapyrusUtil{ "PapyrusUtil"sv };
	inline constexpr auto po3Tweaks{ "po3_Tweaks"sv };

	inline constexpr auto PE{ "5.0"sv };
}
