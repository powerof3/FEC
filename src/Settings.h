#pragma once

enum COLOR : std::uint32_t
{
	kFalmer = 0,
	kGiant = 1,
	kHagraven = 2,
	kRiekling = 3,
	kTotal = 4
};

class Settings
{
public:
	static Settings* GetSingleton();

	void LoadSettings();
	bool GetFormsFromMod();

	bool cannotFindESP{ false };

	bool fixSubtitles{ false };
	bool spellEdits{ false };
	bool updateProperties{ false };

	std::vector<RE::Color> creatureColors;
	std::vector<std::string> sunExclusions;
	std::vector<RE::TESEffectShader*> fireFXS;

private:
	Settings() = default;

	static std::tuple<const char*, const char*> GetColorType(std::uint32_t a_type);

	RE::FormID deathEffectsNPCID{ 0 };
	RE::FormID deathEffectsPCID{ 0 };

	RE::FormID fireFXSID{ 0 };
	RE::FormID fireHeavyFXSID{ 0 };
	RE::FormID fireDisintegrateFXSID{ 0 };

	std::string modName;
};
