#include "Settings.h"

RE::SpellItem* deathEffectsAbility;
RE::SpellItem* deathEffectsPCAbility;

RE::TESFile* mod;

Settings* Settings::GetSingleton()
{
	static Settings singleton;
	return std::addressof(singleton);
}

void ReadFormIDEntry(CSimpleIniA& a_ini, RE::FormID& a_formID, const char* a_entry, const char* a_default)
{
	std::string str = a_ini.GetValue("FormIDs", a_entry, a_default);
	a_ini.SetValue("FormIDs", a_entry, str.c_str(), "", true);

	a_formID = string::lexical_cast<RE::FormID>(str, true);
}

void Settings::LoadSettings()
{
	constexpr auto path = L"Data/SKSE/Plugins/po3_FEC.ini";

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	modName = ini.GetValue("FormIDs", "ModName", "FireBurns.esp");
	ini.SetValue("FormIDs", "ModName", modName.c_str(), ";these formIDs must be changed if FireBurns.esp has been merged or ESLified!!\n;lookup the editorID in XEdit and replace the formids (first two load order digits removed)\n", true);

	try {
		ReadFormIDEntry(ini, deathEffectsNPCID, "DeathEffectsNPCAbility", "0x0067FB28");
		ReadFormIDEntry(ini, deathEffectsPCID, "DeathEffectsPCAbility", "0x00675924");
		ReadFormIDEntry(ini, fireFXSID, "FireFXS", "0x000198F3");
		ReadFormIDEntry(ini, fireHeavyFXSID, "FireHeavyFXS", "0x000A5038");
		ReadFormIDEntry(ini, fireDisintegrateFXSID, "FireDisintegrateFXS", "0x0066B722");
	} catch (...) {
		logger::critical("failed to get formIDs from po3_FEC.ini !");
	}

	fixSubtitles = ini.GetBoolValue("Frost", "SubtitleFix", true);
	ini.SetBoolValue("Frost", "SubtitleFix", fixSubtitles, ";fixes subtitles from not fading out when NPCs are frozen", true);

	spellEdits = ini.GetBoolValue("Spell", "SpellDurationEdits", false);
	ini.SetBoolValue("Spell", "SpellDurationEdits", spellEdits, ";gives instant elemental spells a one second duration (so effects can be detected by FEC). Cost is kept the same.", true);

	updateProperties = ini.GetBoolValue("Update", "UpdateScriptProperties", false);
	ini.SetBoolValue("Update", "UpdateScriptProperties", updateProperties, ";updates mgef script properties", true);

	try {
		creatureColors.reserve(4);
		for (auto i = stl::to_underlying(COLOR::kGiant); i < stl::to_underlying(COLOR::kTotal); i++) {
			auto [name, defColor] = GetColorType(i);

			auto color = ini.GetValue("Poison", name, defColor);
			ini.SetValue("Poison", name, color, "", true);

			creatureColors.emplace_back(string::lexical_cast<std::uint32_t>(color));
		}
	} catch (...) {
		logger::critical("failed to get creature colors from po3_FEC.ini !");
	}

	std::string exclusions = ini.GetValue("Sun", "Exclusions", "NONE");
	if (exclusions == "NONE"sv) {
		exclusions = "AoL - Pitiless";
	}
	ini.SetValue("Sun", "Exclusions", exclusions.c_str(), ";Excludes magic effects from dealing sun damage. Additional effect names must be seperated using a comma, eg: SunDmgEffect , SunDmgEffect2", true);

	logger::info("Sun Exclusion effects = {}", exclusions);
	sunExclusions = string::split(exclusions, " , ");

	ini.SaveFile(path);
}

bool Settings::GetFormsFromMod()
{
	LoadSettings();

	if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
		deathEffectsAbility = dataHandler->LookupForm<RE::SpellItem>(deathEffectsNPCID, modName);
		deathEffectsPCAbility = dataHandler->LookupForm<RE::SpellItem>(deathEffectsPCID, modName);

		mod = const_cast<RE::TESFile*>(dataHandler->LookupLoadedModByName(modName));
		if (!mod) {
			mod = const_cast<RE::TESFile*>(dataHandler->LookupLoadedLightModByName(modName));
		}

		if (!deathEffectsAbility || !deathEffectsPCAbility || !mod) {
			cannotFindESP = true;
			return false;
		}

		fireFXS.push_back(dataHandler->LookupForm<RE::TESEffectShader>(fireFXSID, modName));              //Fire
		fireFXS.push_back(dataHandler->LookupForm<RE::TESEffectShader>(fireHeavyFXSID, modName));         //FireHeavy
		fireFXS.push_back(dataHandler->LookupForm<RE::TESEffectShader>(fireDisintegrateFXSID, modName));  //FireDisintegrate

		return true;
	}
	return false;
}

std::tuple<const char*, const char*> Settings::GetColorType(std::uint32_t a_type)
{
	switch (a_type) {
	case kFalmer:
		return std::make_tuple("Falmer", "0x8F7F75");
	case kGiant:
		return std::make_tuple("Giant", "0x786f6A");
	case kHagraven:
		return std::make_tuple("Hagraven", "0x72615B");
	case kRiekling:
		return std::make_tuple("Riekling", "0x374655");
	default:
		return std::make_tuple("", "");
	}
}
