#pragma once

namespace Dependencies
{
	struct Version
	{
		Version(std::uint32_t a_major, std::uint32_t a_minor, std::uint32_t a_patch) :
			major(a_major),
			minor(a_minor),
			patch(a_patch)
		{}

		explicit Version(const std::string& a_version)
		{
			const auto splitVer = string::split(a_version, ".");

			major = string::to_num<std::uint32_t>(splitVer[0]);
			minor = string::to_num<std::uint32_t>(splitVer[1]);
			patch = string::to_num<std::uint32_t>(splitVer[2]);
		}

		bool operator<(const Version& a_rhs) const
		{
			return major < a_rhs.major || minor < a_rhs.minor;
		}

		std::uint32_t major{ 0 };
		std::uint32_t minor{ 0 };
		std::uint32_t patch{ 0 };
	};

	using _PEGETVERSION = const char* (*)();

	std::string CheckErrors();
	
	inline constexpr auto PapyrusExtender{ "po3_PapyrusExtender"sv };
	inline constexpr auto PapyrusUtil{ "PapyrusUtil"sv };
	inline constexpr auto po3Tweaks{ "po3_Tweaks"sv };

	inline Version requiredPE{ 5, 4, 0 };
}

template <>
struct fmt::formatter<Dependencies::Version>
{
	template <class ParseContext>
	constexpr auto parse(ParseContext& a_ctx)
	{
		return a_ctx.begin();
	}

	template <class FormatContext>
	auto format(const Dependencies::Version& a_version, FormatContext& a_ctx)
	{
		return fmt::format_to(a_ctx.out(), "{}.{}.{}", a_version.major, a_version.minor, a_version.patch);
	}
};
