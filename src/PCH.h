#pragma once

#define NOMB

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#pragma warning(push)
#include <frozen/map.h>
#include <SimpleIni.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <xbyak/xbyak.h>
#pragma warning(pop)

#define DLLEXPORT __declspec(dllexport)

namespace logger = SKSE::log;
namespace numeric = SKSE::stl::numeric;
namespace string = SKSE::stl::string;
namespace WinAPI = SKSE::WinAPI;

using namespace std::literals;

namespace stl
{
	using SKSE::stl::adjust_pointer;
	using SKSE::stl::is;
	using SKSE::stl::to_underlying;

	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}

	template <class F, std::size_t idx, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[0] };
		T::func = vtbl.write_vfunc(idx, T::thunk);
	}
}

namespace formid
{
	inline constexpr RE::FormID falmerFaction = 0x0002997E;
	inline constexpr RE::FormID giantFaction = 0x0004359A;
	inline constexpr RE::FormID hagravenFaction = 0x0004359E;
	inline constexpr RE::FormID rieklingFaction = 0x0301D9C9;

	inline constexpr RE::FormID frostKeyword = 0x0001CEAE;

	inline constexpr RE::FormID fireFXShader = 0x0001B212;
	inline constexpr RE::FormID DLC1_SunCloakSpellHandFX = 0x0200A3BD;

	inline constexpr std::array<RE::FormID, 3> sunHitFXS = { 0x02019C9D, 0x0200A3BB, 0x0200A3BC };
	inline constexpr std::array<RE::FormID, 2> sunHitArt = { 0x0200A3B7, 0x0200A3B8 };
}

namespace str
{
	inline constexpr std::string_view embersXDPath = R"(EmbersHD\mx_fireatlas02.dds)";

	inline constexpr auto MagicDamageFrost{ "MagicDamageFrost"sv };
	inline constexpr auto MagicDamageSun{ "PO3_MagicDamageSun"sv };
}

namespace ver
{
	inline constexpr auto PapyrusExtender{ "po3_PapyrusExtender"sv };
	inline constexpr auto PapyrusUtil{ "PapyrusUtil"sv };
	inline constexpr auto po3Tweaks{ "po3_Tweaks"sv };

	inline constexpr auto PE{ "4.4"sv };
}

#include "Version.h"
