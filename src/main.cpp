#include "Dependencies.h"
#include "Graphics.h"
#include "Papyrus.h"
#include "Patches.h"
#include "Serialization.h"

//GLOBAL VARS
RE::SpellItem* deathEffectsPCAbility{};

void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		{
			const auto consoleLog = RE::ConsoleLog::GetSingleton();

			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				FEC::mod = const_cast<RE::TESFile*>(dataHandler->LookupModByName("FEC.esp"));

				if (!FEC::mod) {
					logger::error("unable to find FEC.esp");
				}

				FEC::deathEffectsAbility = dataHandler->LookupForm<RE::SpellItem>(0x8E7, "FEC.esp");
				deathEffectsPCAbility = dataHandler->LookupForm<RE::SpellItem>(0x8E4, "FEC.esp");

				if (!FEC::deathEffectsAbility || !deathEffectsPCAbility) {
					logger::error("unable to find death effect abilities");

					if (consoleLog) {
						consoleLog->Print("[FEC] FEC.esp is not loaded! Disabling FEC helper plugin\n");
					}

					return;
				}
			}

			if (const auto error = Dependencies::CheckErrors(); !error.empty()) {
				if (consoleLog) {
					consoleLog->Print(error.c_str());
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
			if (FEC::mod && FEC::deathEffectsAbility && deathEffectsPCAbility) {
				FEC::POST_LOAD_PATCH::Install();
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
#	ifndef SKYRIMVR
		SKSE::RUNTIME_1_5_39
#	else
		SKSE::RUNTIME_VR_1_4_15
#	endif
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

	logger::info("Game version : {}", a_skse->RuntimeVersion().string());

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
