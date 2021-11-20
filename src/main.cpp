#include "Graphics.h"
#include "Patches.h"

//GLOBAL VARS
RE::SpellItem* deathEffectsAbility;
RE::SpellItem* deathEffectsPCAbility;

RE::TESFile* mod;

// ERROR HANDLING
class RequirementsCheck
{
public:
	using PEGETVERSION = const char* (*)();

	static std::vector<std::string> GetError()
	{
		std::vector<std::string> vec;

		const auto papyrusExtenderHandle = GetModuleHandleA(ver::PapyrusExtender.data());
		const auto papyrusUtilsHandle = GetModuleHandleA(ver::PapyrusUtil.data());

		std::string message;
		std::string info;

		if (papyrusExtenderHandle == nullptr) {
			logger::error("PapyrusExtender SSE plugin not found | error {}", GetLastError());

			info = "Frozen Electrocuted Combustion - Missing Plugin";

			if (papyrusUtilsHandle == nullptr) {
				message = R"(Papyrus Extender [SKSE plugin] is not loaded! Frozen Electrocuted Combustion [FEC] will not work correctly!

If you have installed, and activated Papyrus Extender in your mod manager, make sure you have all requirements installed, including Microsoft Visual C++ Redistributables 2019.

Otherwise, please download and install it.)";
			} else {
				message = R"(Papyrus Extender [SKSE plugin] is not loaded! Frozen Electrocuted Combustion [FEC] will not work correctly!

Please note that Papyrus EXTENDER is NOT Papyrus UTILS, which is a different SKSE plugin.

If you have installed, and activated Papyrus Extender in your mod manager, make sure you have all requirements installed, including Microsoft Visual C++ Redistributables 2019.

Otherwise, please download and install it.)";
			}
		} else {
			const auto peGetVersion = reinterpret_cast<PEGETVERSION>(GetProcAddress(papyrusExtenderHandle, "GetPluginVersion"));

			if (peGetVersion == nullptr) {
				logger::error("Failed version check info from PapyrusExtender | error {} ", GetLastError());

				info = "Frozen Electrocuted Combustion - Outdated Plugin";

				message = R"(PapyrusExtender [SKSE plugin] is severely out of date!

Required PapyrusExtender version : )" +
				          std::string(ver::PE) +
				          " or higher";
			} else {
				const std::string currentPE(peGetVersion());
				const auto compare = compare_version(currentPE);

				if (compare == -1) {
					logger::error("PapyrusExtender SSE plugin version too low");

					info = "Frozen Electrocuted Combustion - Outdated Plugin";

					message = R"(PapyrusExtender [SKSE plugin] is out of date!

Frozen Electrocuted Combustion [FEC] requires version )" +
					          std::string(ver::PE) +

					          R"(

Current PapyrusExtender version : )" +
					          currentPE;

				} else if (compare == 1) {
					logger::info("PapyrusExtender SSE plugin version too high");

					info = "Frozen Electrocuted Combustion - Outdated Mod";

					message = R"(PapyrusExtender [SKSE plugin] is newer than expected. You'd probably want to update Frozen Electrocuted Combustion [FEC].

Required PapyrusExtender version : )" +
					          std::string(ver::PE) +

					          R"(

Current PapyrusExtender version : )" +
					          currentPE;
				}
			}
		}

		if (!message.empty() && !info.empty()) {
			message += R"(


Click Ok to continue, or Cancel to quit the game)";
			vec.push_back(message);
			vec.push_back(info);
		}

		return vec;
	}

private:
	static std::int32_t compare_version(const std::string& a_value)
	{
		std::uint32_t major1 = 0;
		std::uint32_t minor1 = 0;
		std::uint32_t major2 = 0;
		std::uint32_t minor2 = 0;

		sscanf_s(a_value.data(), "%u.%u", &major1, &minor1);
		sscanf_s(ver::PE.data(), "%u.%u", &major2, &minor2);

		if (major1 < major2) {
			return -1;
		}
		if (major1 > major2) {
			return 1;
		}
		if (minor1 < minor2) {
			return -1;
		}
		if (minor1 > minor2) {
			return 1;
		}

		return 0;
	}
};

void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		{
			auto vec = RequirementsCheck::GetError();
			if (!vec.empty() && vec.size() == 2) {
				auto id = WinAPI::MessageBox(nullptr, vec[0].c_str(), vec[1].c_str(), 0x000001);
				if (id == 2) {
					std::_Exit(EXIT_FAILURE);
				}
			}
		}
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		{
			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				mod = const_cast<RE::TESFile*>(dataHandler->LookupLoadedModByName("FEC.esp"));

				deathEffectsAbility = dataHandler->LookupForm<RE::SpellItem>(0x0067FB28, "FEC.esp");
				deathEffectsPCAbility = dataHandler->LookupForm<RE::SpellItem>(0x00675924, "FEC.esp");

				if (!mod || !deathEffectsAbility || !deathEffectsPCAbility) {
					return;
				}
			}

			constexpr auto get_tweaks_fix = []() {
				const auto po3TweaksHandle = GetModuleHandleA(ver::po3Tweaks.data());
				if (po3TweaksHandle == nullptr) {
					return false;
				}

				constexpr auto path = L"Data/SKSE/Plugins/po3_Tweaks.ini";
				CSimpleIniA ini;
				ini.SetUnicode();

				auto rc = ini.LoadFile(path);
				if (rc < 0) {
					return false;
				}

				return ini.GetBoolValue("Fixes", "Cast No-Death-Dispel Spells on Load", false);
			};
			if (!get_tweaks_fix()) {
				MAINTENANCE::Install();
			} else {
				logger::info("powerofthree's Tweaks found, skipping ability maintainer");
			}

			GRAPHICS::Install();

			PATCH::Install();

			DISTRIBUTE::Install();
		}
		break;
	default:
		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= "po3_FEC.log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "FEC Helper plugin";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("loaded");

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(75);

	const auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(OnInit);

	return true;
}
