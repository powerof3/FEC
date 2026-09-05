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

		return REL::Version(
			REX::STR::TO_NUM<std::uint16_t>(splitVer[0]),
			REX::STR::TO_NUM<std::uint16_t>(splitVer[1]),
			REX::STR::TO_NUM<std::uint16_t>(splitVer[2]),
			REX::STR::TO_NUM<std::uint16_t>(splitVer[3])
		);
	}

	inline REL::Version requiredPE{ 6, 3, 0 };
}
