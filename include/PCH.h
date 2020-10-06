#pragma once

#include "RE/Skyrim.h"
#include "REL/Relocation.h"
#include "SKSE/SKSE.h"

#include <nonstd/span.hpp>
#include <SimpleIni.h>
#include <frozen/string.h>
#include <frozen/set.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace logger = SKSE::log;
using namespace SKSE::util;

#define DLLEXPORT __declspec(dllexport)

namespace FormID
{
	// factions
	inline constexpr RE::FormID falmerFactionID = 0x0002997E;
	inline constexpr RE::FormID giantFactionID = 0x0004359A;
	inline constexpr RE::FormID hagravenFactionID = 0x0004359E;
	inline constexpr RE::FormID rieklingFactionID = 0x0301D9C9;

	// effect keywords
	inline constexpr RE::FormID frostKeywordID = 0x0001CEAE;

	// fire shader
	inline constexpr RE::FormID fireFXShaderID = 0x0001B212;
	
	inline constexpr RE::FormID DLC1_SunCloakSpellHandFX = 0x0200A3BD;
}

inline constexpr frozen::set<RE::FormID, 3> sunHitFXS = { 0x02019C9D, 0x0200A3BB, 0x0200A3BC };
inline constexpr frozen::set<RE::FormID, 2> sunHitArt = { 0x0200A3B7, 0x0200A3B8 };

inline constexpr frozen::string embersXDPath = R"(EmbersHD\mx_fireatlas02.dds)";
inline constexpr frozen::string MagicDamageFrost = "MagicDamageFrost";
inline constexpr frozen::string MagicDamageShock = "MagicDamageShock";

inline std::string as_string(std::string_view v)
{
	return { v.data(), v.size() };
}