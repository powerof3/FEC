#pragma once

#define NOMB

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#pragma warning(push)
#include <SimpleIni.h>
#include <frozen/map.h>
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
	using SKSE::stl::is_in;
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

namespace ver
{
	inline constexpr auto PapyrusExtender{ "po3_PapyrusExtender"sv };
	inline constexpr auto PapyrusUtil{ "PapyrusUtil"sv };
	inline constexpr auto po3Tweaks{ "po3_Tweaks"sv };

	inline constexpr auto PE{ "4.5"sv };
}

#include "Version.h"
