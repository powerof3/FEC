#pragma once

namespace Dependencies
{
	using _PEGETVERSION = const char* (*)();

	std::string CheckErrors();

	inline constexpr auto PapyrusExtender{ "po3_PapyrusExtender"sv };
	inline constexpr auto PapyrusUtil{ "PapyrusUtil"sv };
	inline constexpr auto po3Tweaks{ "po3_Tweaks"sv };

	inline REL::Version to_version(const std::string& a_version)
	{
		const auto splitVer = REX::STR::SPLIT(a_version, ".");

		const auto to_num_safe = [&](std::size_t a_index) -> std::uint16_t {
			if (a_index >= splitVer.size()) {
				return 0;
			}
			return REX::STR::TO_NUM<std::uint16_t>(splitVer[a_index]);
		};

		return REL::Version(to_num_safe(0), to_num_safe(1), to_num_safe(2), to_num_safe(3));
	}

	inline REL::Version requiredPE{ 6, 3, 0 };
}
