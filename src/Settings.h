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

	bool cannotFindESP;

	bool fixSubtitles;
	bool spellEdits;
	bool updateProperties;

	std::vector<RE::Color> creatureColors;
	std::vector<std::string> sunExclusions;
	std::vector<RE::TESEffectShader*> fireFXS;

private:
	Settings() = default;

	std::tuple<const char*, const char*> GetColorType(std::uint32_t a_type);

	RE::FormID deathEffectsNPCID;
	RE::FormID deathEffectsPCID;

	RE::FormID fireFXSID;
	RE::FormID fireHeavyFXSID;
	RE::FormID fireDisintegrateFXSID;

	std::string modName;
};
