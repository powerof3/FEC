#include "Graphics.h"
#include "Papyrus.h"
#include "Patches.h"
#include "Serialization.h"

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


Click Yes to quit the game, or No to continue playing)";
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
			if (const auto vec = RequirementsCheck::GetError(); !vec.empty() && vec.size() == 2) {
				const auto id = WinAPI::MessageBox(nullptr, vec[0].c_str(), vec[1].c_str(), 0x00000004);
				if (id == 2) {
					std::_Exit(EXIT_FAILURE);
				}
			}
		}
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		{
			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				mod = const_cast<RE::TESFile*>(dataHandler->LookupModByName("FEC.esp"));

				if (!mod) {
					logger::error("unable to find FEC.esp");
				}

				deathEffectsAbility = dataHandler->LookupForm<RE::SpellItem>(0x8E7, "FEC.esp");
				deathEffectsPCAbility = dataHandler->LookupForm<RE::SpellItem>(0x8E4, "FEC.esp");

				if (!deathEffectsAbility || !deathEffectsPCAbility) {
					logger::error("unable to find death effect abilities");
					return;
				}
			}

			FEC::GRAPHICS::Install();

			FEC::PATCH::Install();

			FEC::DISTRIBUTE::Install();

			FEC::Serialization::Manager::Register();
		}
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
	case SKSE::MessagingInterface::kNewGame:
		{
			if (mod && deathEffectsAbility && deathEffectsPCAbility) {
				FEC::POST_LOAD_PATCH::Install();
			} else if (const auto consoleLog = RE::ConsoleLog::GetSingleton()) {
				consoleLog->Print("[FEC] FEC.esp is not installed!");
			}
		}
		break;
	default:
		break;
	}
}

#ifdef SKYRIM_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Version::MAJOR);
	v.PluginName("FEC Helper plugin");
	v.AuthorName("powerofthree");
	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();
#else
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "FEC Helper plugin";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver <
#ifndef SKYRIMVR
		SKSE::RUNTIME_1_5_39
#else
		SKSE::RUNTIME_VR_1_4_15
#endif
	) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}
#endif

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%l] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();

	logger::info("loaded");

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(75);

	const auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(OnInit);

	const auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(FEC::Papyrus::Bind);

	const auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID(FEC::Serialization::kFEC);
	serialization->SetSaveCallback(FEC::Serialization::SaveCallback);
	serialization->SetLoadCallback(FEC::Serialization::LoadCallback);
	serialization->SetRevertCallback(FEC::Serialization::RevertCallback);
	serialization->SetFormDeleteCallback(FEC::Serialization::FormDeleteCallback);

	return true;
}
