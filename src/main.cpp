#include "Dependencies.h"
#include "Graphics.h"
#include "Papyrus.h"
#include "Patches.h"
#include "Serialization.h"

void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		{
			const auto consoleLog = RE::ConsoleLog::GetSingleton();

			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				FEC::mod = const_cast<RE::TESFile*>(dataHandler->LookupModByName("FEC.esp"));

				if (!FEC::mod) {
					REX::ERROR("unable to find FEC.esp");
					if (consoleLog) {
						consoleLog->Print("[FEC] FEC.esp is not loaded! Disabling FEC helper plugin\n");
					}
					return;
				}

				FEC::deathEffectsAbility = dataHandler->LookupForm<RE::SpellItem>(0x8E7, "FEC.esp");
				FEC::deathEffectsPCAbility = dataHandler->LookupForm<RE::SpellItem>(0x8E4, "FEC.esp");

				if (!FEC::deathEffectsAbility || !FEC::deathEffectsPCAbility) {
					REX::ERROR("unable to find death effect abilities");
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
			if (FEC::mod && FEC::deathEffectsAbility && FEC::deathEffectsPCAbility) {
				FEC::POST_LOAD_PATCH::Install();
			}
		}
		break;
	default:
		break;
	}
}

#ifdef SKYRIM_SUPPORT_AE
SKSE_PLUGIN_VERSION = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(REL::Version{ Version::MAJOR, Version::MINOR, Version::PATCH });
	v.PluginName("FEC Helper plugin");
	v.AuthorName("powerofthree");
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });

	if constexpr (SKSE::RUNTIME_SSE_LATEST < Runtime::MIN_ADDRESS_LIBRARY_V5) {
		v.MinimumRequiredXSEVersion(REL::Version{ 2, 2, 5 });
	} else {
		v.MinimumRequiredXSEVersion(REL::Version{ 2, 3, 0 });
	}

	return v;
}();
#else
SKSE_PLUGIN_QUERY(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "FEC Helper plugin";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		REX::CRITICAL("Loaded in editor, marking as incompatible");
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver
#	ifndef SKYRIMVR
		< SKSE::RUNTIME_SSE_1_5_39
#	else
		> SKSE::RUNTIME_VR_1_4_15
#	endif
	) {
		REX::CRITICAL("Unsupported runtime version {}", ver.string());
		return false;
	}

	return true;
}
#endif

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse, { .log = true,
						   .logName = Version::PROJECT.data(),
						   .trampoline = true,
						   .trampolineSize = 128 });

	Runtime::version = a_skse->RuntimeVersion();

	REX::INFO("Game version : {} (built for {})", Runtime::version, SKSE::RUNTIME_SSE_LATEST);

#ifdef SKYRIM_SUPPORT_AE
	if constexpr (SKSE::RUNTIME_SSE_LATEST < Runtime::MIN_ADDRESS_LIBRARY_V5) {
		if (Runtime::version >= Runtime::MIN_ADDRESS_LIBRARY_V5) {
			REX::FAIL(
				"You are using a newer version of Skyrim than this version of {0} supports.\n"
				"Install the correct version of {0} for your game version.\n"
				"Runtime: {1}\n"
				"Supported: 1.6.1170 (Steam) / 1.6.1179 (GOG)",
				Version::PROJECT, Runtime::version);
		}
	}
#endif

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
