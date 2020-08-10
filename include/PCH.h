#pragma once

#include "RE/Skyrim.h"
#include "REL/Relocation.h"
#include "SKSE/SKSE.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>
#include <Windows.h>

#include <spdlog/sinks/basic_file_sink.h>

#ifndef NDEBUG
#include <spdlog/sinks/msvc_sink.h>
#endif

namespace logger = SKSE::log;
using namespace SKSE::util;

#define DLLEXPORT __declspec(dllexport)


namespace FormID
{
	inline constexpr RE::FormID falmerFactionID = 0x0002997E;
	inline constexpr RE::FormID giantFactionID = 0x0004359A;
	inline constexpr RE::FormID hagravenFactionID = 0x0004359E;
	inline constexpr RE::FormID rieklingFactionID = 0x0001D9C9;

	inline constexpr RE::FormID creatureKeywordID = 0x00013795;
	inline constexpr RE::FormID daedraKeywordID = 0x00013797;
	inline constexpr RE::FormID dragonKeywordID = 0x00035D59;

	inline constexpr RE::FormID frostKeywordID = 0x0001CEAE;
	inline constexpr RE::FormID shockKeywordID = 0x0001CEAF;
}


inline std::string as_string(std::string_view v)
{
	return { v.data(), v.size() };
}