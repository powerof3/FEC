#pragma once

namespace MAINTENANCE
{
	class TESLoadGameEventHandler final : public RE::BSTEventSink<RE::TESLoadGameEvent>
	{
	public:
		static TESLoadGameEventHandler* GetSingleton();

		RE::BSEventNotifyControl ProcessEvent(const RE::TESLoadGameEvent* evn, RE::BSTEventSource<RE::TESLoadGameEvent>*) override;

	protected:
		TESLoadGameEventHandler() = default;
		TESLoadGameEventHandler(const TESLoadGameEventHandler&) = delete;
		TESLoadGameEventHandler(TESLoadGameEventHandler&&) = delete;
		~TESLoadGameEventHandler() override = default;

		TESLoadGameEventHandler& operator=(const TESLoadGameEventHandler&) = delete;
		TESLoadGameEventHandler& operator=(TESLoadGameEventHandler&&) = delete;
	};

	void Install();
}

namespace PATCH
{
	namespace formid
	{
		inline constexpr RE::FormID falmerFaction = 0x0002997E;
		inline constexpr RE::FormID giantFaction = 0x0004359A;
		inline constexpr RE::FormID hagravenFaction = 0x0004359E;
		inline constexpr RE::FormID rieklingFaction = 0x0401D9C9;

		inline constexpr RE::FormID fireFXShader = 0x0001B212;

		inline constexpr std::array<RE::FormID, 3> FEC_fireFXS = { 0x806, 0x826, 0x82F };
	}

	namespace color
	{
		inline constexpr RE::Color falmer = 0x8F7F75;
		inline constexpr RE::Color giant = 0x786f6A;
		inline constexpr RE::Color hagraven = 0x72615B;
		inline constexpr RE::Color riekling = 0x374655;
	}

	namespace str
	{
		inline constexpr auto embersXDPath = R"(EmbersHD\mx_fireatlas02.dds)"sv;
		inline constexpr auto MagicDamageFrost{ "MagicDamageFrost"sv };
	}

	void Install();
};

namespace DISTRIBUTE
{
	namespace formid
	{
		inline constexpr std::array<RE::FormID, 3> sunHitFXS = { 0x02019C9D, 0x0200A3BB, 0x0200A3BC };
		inline constexpr std::array<RE::FormID, 2> sunHitArt = { 0x0200A3B7, 0x0200A3B8 };

		inline constexpr RE::FormID DLC1_SunCloakSpellHandFX = 0x0200A3BD;
	}

	namespace str
	{
		inline constexpr auto MagicDamageSun{ "PO3_MagicDamageSun"sv };
	}

	void Install();
};
