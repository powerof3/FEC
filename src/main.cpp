#include "graphics.h"
#include "version.h"

//VARS

static bool fixSubtitles = false;
static bool spellEdits = false;
static bool updateProperties = false;
static bool cannotFindESP = false;

static std::string modName;
static RE::FormID deathEffectsNPCID;
static RE::FormID deathEffectsPCID;
static RE::FormID fireFXSID;
static RE::FormID fireHeavyFXSID;
static RE::FormID fireDisintegrateFXSID;

RE::SpellItem* deathEffectsAbility = nullptr;
static RE::SpellItem* deathEffectsPCAbility = nullptr;

const RE::TESFile* mod = nullptr;

static std::vector<std::string> sunExclusions;

static std::string pluginPath;

using PEGETVERSION = const char* (*)();

//EFFECT MAINTAINER

class ActorUpdate
{
public:
	static void Hook()
	{
		HookUpdate();
		HookUpdateNoAI();
	}

protected:
	static void HookUpdate()
	{
		REL::Relocation<std::uintptr_t> ActorUpdate{ REL::ID(36357) };

		auto& trampoline = SKSE::GetTrampoline();
		_UpdateAVs = trampoline.write_call<5>(ActorUpdate.address() + 0x1D1, UpdateAVs);

		logger::info("Hooked actor update.");
	}

	static void HookUpdateNoAI()
	{
		REL::Relocation<std::uintptr_t> vtbl{ REL::ID(261397) };  //Character vtbl
		_UpdateNoAI = vtbl.write_vfunc(0x0AE, UpdateNoAI);

		logger::info("Hooked actor update - no AI.");
	}

private:
	static void UpdateAVs(RE::Actor* a_actor)
	{
		_UpdateAVs(a_actor);

		if (a_actor != RE::PlayerCharacter::GetSingleton()) {
			if (ActorHasInactiveBaseSpell(a_actor)) {
				a_actor->AddSpell(deathEffectsAbility);
			}
		}
	}
	static inline REL::Relocation<decltype(UpdateAVs)> _UpdateAVs;


	static void UpdateNoAI(RE::Actor* a_this, float a_delta)
	{
		_UpdateNoAI(a_this, a_delta);

		if (a_this != RE::PlayerCharacter::GetSingleton()) {
			if (ActorHasInactiveBaseSpell(a_this)) {
				a_this->AddSpell(deathEffectsAbility);
			}
		}
	}
	using UpdateNoAI_t = decltype(&RE::Actor::UpdateNoAI);	// 0AE
	static inline REL::Relocation<UpdateNoAI_t> _UpdateNoAI;


	static auto ActorHasInactiveBaseSpell(RE::Actor* a_actor) -> bool
	{
		bool hasSpell = false;

		if (auto base = a_actor->GetActorBase(); base) {
			auto spellList = base->GetOrCreateSpellList();
			if (spellList && spellList->spells) {
				stl::span<RE::SpellItem*> span(spellList->spells, spellList->numSpells);
				for (auto& spell : span) {
					if (spell == deathEffectsAbility) {
						hasSpell = true;
						break;
					}
				}
			}
		}

		if (hasSpell) {
			if (auto activeEffects = a_actor->GetActiveEffectList(); activeEffects) {
				for (auto& ae : *activeEffects) {
					if (ae && ae->spell == deathEffectsAbility) {
						return false;
					}
				}
			}
			return true;
		}

		return false;
	}
};

//PATCHES

class PATCH
{
public:
	static void FrozenDeathSubtitles()
	{
		if (fixSubtitles) {
			auto dataHandler = RE::TESDataHandler::GetSingleton();
			if (dataHandler) {
				using Flags = RE::TOPIC_INFO_DATA::TOPIC_INFO_FLAGS;
				auto frostKYWD = RE::TESForm::LookupByID<RE::BGSKeyword>(FormID::frostKeywordID);

				std::uint32_t topicCount = 0;

				for (const auto& topic : dataHandler->GetFormArray<RE::TESTopic>()) {
					if (topic && topic->data.subtype == RE::DIALOGUE_DATA::Subtype::kDeath) {
						auto count = topic->numTopicInfos;

						for (std::uint32_t i = 0; i < count; i++) {
							auto topicInfo = topic->topicInfos[i];

							if (topicInfo && topicInfo->data.flags.none(Flags::kNoLIPFile)) {
								auto newNode = new RE::TESConditionItem;

								newNode->next = nullptr;
								newNode->data.comparisonValue.f = 0.0f;
								newNode->data.functionData.function = RE::FUNCTION_DATA::FunctionID::kHasMagicEffectKeyword;
								newNode->data.functionData.params[0] = frostKYWD;

								if (topicInfo->objConditions.head == nullptr) {
									topicInfo->objConditions.head = newNode;
								} else {
									newNode->next = topicInfo->objConditions.head;
									topicInfo->objConditions.head = newNode;
								}
								topicCount++;
							}
						}
					}
				}
				logger::info("Patched {} death dialogues from showing up on frozen NPCs.", topicCount);
			}
		} else {
			logger::info("Subtitle fix is off.");
		}
	}

	static void SpellDuration()
	{
		using Flags = RE::EffectSetting::EffectSettingData::Flag;
		using CastingType = RE::MagicSystem::CastingType;

		if (spellEdits) {
			auto dataHandler = RE::TESDataHandler::GetSingleton();
			if (dataHandler) {
				std::uint32_t count = 0;

				logger::info("Starting spell edits");
				for (const auto& spell : dataHandler->GetFormArray<RE::SpellItem>()) {
					if (spell) {
						for (auto& effect : spell->effects) {
							if (effect && effect->effectItem.duration == 0.0) {
								auto baseEffect = effect->baseEffect;
								if (baseEffect && (baseEffect->HasKeywordString(STR::MagicDamageFrost) || baseEffect->HasKeywordString(STR::MagicDamageShock)) && baseEffect->data.castingType == CastingType::kFireAndForget && baseEffect->data.flags.all(Flags::kHostile)) {
									auto cost = effect->cost;
									baseEffect->data.flags.reset(Flags::kNoDuration);
									effect->effectItem.duration = 1;
									effect->cost = cost;
									count++;
								}
							}
						}
					}
				}
				logger::info("Finished spell edits. {} magic effects processed", count);
			}
		} else {
			logger::info("Spell edits disabled");
		}
	}

	static void BodyTintColor()
	{
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			logger::info("Starting actorbase bodytint edits");

			CSimpleIniA ini;
			ini.LoadFile(pluginPath.c_str());
			ini.SetUnicode();

			const RE::Color falmerColor = ini.GetLongValue("Poison", "Falmer", 0x7f8081);
			std::uint32_t falmerCount = 0;

			const RE::Color giantColor = ini.GetLongValue("Poison", "Giant", 0x7f8081);
			std::uint32_t giantCount = 0;

			const RE::Color hagravenColor = ini.GetLongValue("Poison", "Hagraven", 0x7f8081);
			std::uint32_t hagravenCount = 0;

			const RE::Color rieklingColor = ini.GetLongValue("Poison", "Riekling", 0x7f8081);
			std::uint32_t rieklingCount = 0;

			const auto falmerFaction = RE::TESForm::LookupByID<RE::TESFaction>(FormID::falmerFactionID);
			const auto giantFaction = RE::TESForm::LookupByID<RE::TESFaction>(FormID::giantFactionID);
			const auto hagravenFaction = RE::TESForm::LookupByID<RE::TESFaction>(FormID::hagravenFactionID);
			const auto rieklingFaction = RE::TESForm::LookupByID<RE::TESFaction>(FormID::rieklingFactionID);

			for (const auto& actorbase : dataHandler->GetFormArray<RE::TESNPC>()) {
				if (actorbase && !actorbase->HasKeyword("ActorTypeNPC"sv)) {
					if (actorbase->IsInFaction(falmerFaction)) {
						falmerCount++;
						actorbase->bodyTintColor = falmerColor;
					} else if (actorbase->IsInFaction(giantFaction)) {
						giantCount++;
						actorbase->bodyTintColor = giantColor;
					} else if (actorbase->IsInFaction(hagravenFaction)) {
						hagravenCount++;
						actorbase->bodyTintColor = hagravenColor;
					} else if (actorbase->IsInFaction(rieklingFaction)) {
						rieklingCount++;
						actorbase->bodyTintColor = rieklingColor;
					}
				}
			}
			logger::info("Finished actorbase bodytint edits on {} falmer, {} giants, {} hagravens and {} rieklings", falmerCount, giantCount, hagravenCount, rieklingCount);
		}
	}

	static void EmbersXD()
	{
		std::vector<RE::TESEffectShader*> fireFXS;
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			fireFXS.push_back(dataHandler->LookupForm<RE::TESEffectShader>(fireFXSID, modName));			  //Fire
			fireFXS.push_back(dataHandler->LookupForm<RE::TESEffectShader>(fireHeavyFXSID, modName));		  //FireHeavy
			fireFXS.push_back(dataHandler->LookupForm<RE::TESEffectShader>(fireDisintegrateFXSID, modName));  //FireDisintegrate
		}

		auto defFireFXS = RE::TESForm::LookupByID<RE::TESEffectShader>(FormID::fireFXShaderID);
		if (defFireFXS && defFireFXS->particleShaderTexture.textureName == STR::embersXDPath.data()) {
			logger::info("Applying EmbersXD patch");
			for (auto& shader : fireFXS) {
				if (shader) {
					shader->data.flags.reset(RE::EffectShaderData::Flags::kParticleGreyscaleColor);
					shader->particleShaderTexture.textureName = defFireFXS->particleShaderTexture.textureName;
					shader->data.colorScale = defFireFXS->data.colorScale;
					shader->data.colorKey2 = defFireFXS->data.colorKey2;
					shader->data.colorKey3 = defFireFXS->data.colorKey3;
					shader->data.particleShaderAnimatedLoopStartFrame = defFireFXS->data.particleShaderAnimatedLoopStartFrame;
					shader->data.particleShaderAnimatedFrameCount = defFireFXS->data.particleShaderAnimatedFrameCount;
				}
			}
		}
	}
};


//DISTRIBUTION

class DISTRIBUTE
{
public:
	static void DeathEffect()
	{
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			for (const auto& actorbase : dataHandler->GetFormArray<RE::TESNPC>()) {
				if (actorbase && !actorbase->IsPlayer() && CanBeAddedTo(actorbase)) {
					auto actorEffects = actorbase->GetOrCreateSpellList();
					if (actorEffects) {
						actorEffects->AddSpell(deathEffectsAbility);
					}
				}
			}
		}
	}


	static void SunKeyword()
	{
		logger::info("Applying global sun keyword to sun magic effects");
		std::uint32_t count = 0;


		auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			auto& keywords = dataHandler->GetFormArray<RE::BGSKeyword>();

			RE::BGSKeyword* sunKeyword = nullptr;
			auto it = std::find_if(keywords.begin(), keywords.end(),
				[&](const auto& kywd) -> bool { return kywd && kywd->formEditorID == "PO3_MagicDamageSun"sv; });
			if (it != keywords.end()) {
				sunKeyword = *it;
			} else {
				sunKeyword = RE::BGSKeyword::CreateKeyword("PO3_MagicDamageSun"sv);
				if (sunKeyword) {
					keywords.push_back(sunKeyword);
				}		
			}

			if (sunKeyword) {
				for (const auto& mgef : dataHandler->GetFormArray<RE::EffectSetting>()) {
					if (mgef && CanSunKeywordBeAdded(mgef)) {
						mgef->AddKeyword(sunKeyword);
						count++;
					}
				}
			}
		}
		logger::info("Finished sun keyword distribution. {} magic effects processed", count);
	}


private:
	static auto CanBeAddedTo(RE::TESNPC* a_actorbase) -> bool
	{
		if (a_actorbase->HasKeyword("ActorTypeNPC"sv)) {
			if (const auto race = a_actorbase->GetRace(); race) {
				const std::string raceName(race->GetName());
				if (raceName.find("Child") != std::string::npos) {
					return false;
				}
			}
			if (!a_actorbase->HasKeyword("ActorTypeGhost"sv)) {
				return true;
			}
		}
		if (a_actorbase->HasKeyword("ActorTypeAnimal"sv)) {
			return true;
		}
		if (a_actorbase->HasKeyword("ActorTypeCreature"sv)) {
			if (!a_actorbase->HasKeyword("ActorTypeDragon"sv) && !a_actorbase->HasKeyword("ActorTypeDaedra"sv)) {
				return true;
			}
		}
		return false;
	}

	static auto CanSunKeywordBeAdded(RE::EffectSetting* mgef) -> bool
	{
		using namespace FormID;
		using namespace RE::MagicSystem;
		using Flag = RE::EffectSetting::EffectSettingData::Flag;

		const auto hitFXS = mgef->data.effectShader;
		const auto castArt = mgef->data.castingArt;
		const auto hitArt = mgef->data.hitEffectArt;

		if (hitFXS && std::find(sunHitFXS.begin(), sunHitFXS.end(), hitFXS->GetFormID()) != sunHitFXS.end() || hitArt && std::find(sunHitArt.begin(), sunHitArt.end(), hitArt->GetFormID()) != sunHitArt.end() || castArt && castArt->GetFormID() == DLC1_SunCloakSpellHandFX) {
			
			if (mgef->data.flags.none(Flag::kHostile) || mgef->data.flags.none(Flag::kDetrimental) || mgef->data.castingType == CastingType::kConstantEffect || mgef->data.flags.all(Flag::kNoHitEvent)) {
				return false;
			}

			const std::string name(mgef->GetName());
			if (std::find(sunExclusions.begin(), sunExclusions.end(), name) != sunExclusions.end()) {
				return false;
			}

			return true;
		}

		return false;
	}
};


//LOAD GAME

class TESLoadGameEventHandler : public RE::BSTEventSink<RE::TESLoadGameEvent>
{
public:
	static auto GetSingleton() -> TESLoadGameEventHandler*
	{
		static TESLoadGameEventHandler singleton;
		return &singleton;
	}

	auto ProcessEvent(const RE::TESLoadGameEvent* evn, RE::BSTEventSource<RE::TESLoadGameEvent>* a_eventSource) -> RE::BSEventNotifyControl override
	{
		if (!evn) {
			return RE::BSEventNotifyControl::kContinue;
		}

		if (cannotFindESP) {
			RE::ShowMessageBox(R"(Frozen Electrocuted Combustion [FEC] - Missing formIDs! Effects will not be applied!

If you have renamed/merged/ESLfied FireBurns.esp, make sure to input the new formids in po3_FEC.ini)");
			cannotFindESP = false;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		if (player) {
			if (!player->HasSpell(deathEffectsPCAbility)) {
				player->AddSpell(deathEffectsPCAbility);
			}
		}

		if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
			for (auto& actorHandle : processLists->highActorHandles) {
				auto actorPtr = actorHandle.get();
				auto actor = actorPtr.get();
				if (actor && ActorHasBaseSpell(actor, deathEffectsAbility)) {
					actor->RemoveSpell(deathEffectsAbility);
					actor->AddSpell(deathEffectsAbility);
				}
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

protected:
	TESLoadGameEventHandler() = default;
	TESLoadGameEventHandler(const TESLoadGameEventHandler&) = delete;
	TESLoadGameEventHandler(TESLoadGameEventHandler&&) = delete;
	virtual ~TESLoadGameEventHandler() = default;

	auto operator=(const TESLoadGameEventHandler&) -> TESLoadGameEventHandler& = delete;
	auto operator=(TESLoadGameEventHandler&&) -> TESLoadGameEventHandler& = delete;

private:
	static auto ActorHasBaseSpell(RE::Actor* a_actor, RE::SpellItem* a_spell) -> bool
	{
		auto base = a_actor->GetActorBase();
		if (base) {
			const auto spellList = base->GetOrCreateSpellList();
			if (spellList && spellList->spells) {
				stl::span<RE::SpellItem*> span(spellList->spells, spellList->numSpells);
				for (auto& spell : span) {
					if (spell == a_spell) {
						return true;
					}
				}
			}
		}

		return false;
	}
};


// ERROR HANDLING

class SanityCheck
{
public:
	static auto GetError() -> std::vector<std::string>
	{
		std::vector<std::string> vec;

		const auto papyrusExtenderHandle = GetModuleHandle(PapyrusExtender.data());
		const auto papyrusUtilsHandle = GetModuleHandle(PapyrusUtil.data());

		std::string message;
		std::string info;

		if (papyrusExtenderHandle == nullptr) {
			logger::error("PapyrusExtender SSE plugin not found | error {}", GetLastError());

			info = "Frozen Electrocuted Combustion - Missing Plugin";

			if (papyrusUtilsHandle == nullptr) {
				message = R"(Frozen Electrocuted Combustion [FEC] is missing Papyrus Extender [SKSE plugin])";
			} else {
				message = R"(Frozen Electrocuted Combustion [FEC] is missing Papyrus Extender [SKSE plugin]

Please note that Papyrus EXTENDER is NOT Papyrus UTILS, which is a different SKSE plugin.)";
			}
		} else {
			const auto peGetVersion = reinterpret_cast<PEGETVERSION>(GetProcAddress(papyrusExtenderHandle, "GetPluginVersion"));

			if (peGetVersion == nullptr) {
				logger::error("Failed version check info from PapyrusExtender | error {} ", GetLastError());

				info = "Frozen Electrocuted Combustion - Outdated Plugin";

				message = R"(PapyrusExtender [SKSE plugin] is severely out of date!

Required PapyrusExtender version : )" +
						  STR::as_string(PE_VER) +
						  " or higher";
			} else {
				std::string currentPE(peGetVersion());
				auto compare = compareVersion(currentPE);

				if (compare == -1) {
					logger::error("PapyrusExtender SSE plugin version too low");

					info = "Frozen Electrocuted Combustion - Outdated Plugin";

					message = R"(PapyrusExtender [SKSE plugin] is out of date!

Frozen Electrocuted Combustion [FEC] requires version )" +
							  STR::as_string(PE_VER) +

							  R"(

Current PapyrusExtender version : )" +
							  currentPE;

				} else if (compare == 1) {
					logger::info("PapyrusExtender SSE plugin version too high");

					info = "Frozen Electrocuted Combustion - Outdated Mod";

					message = R"(PapyrusExtender [SKSE plugin] is newer than expected. You'd probably want to update Frozen Electrocuted Combustion [FEC].

Required PapyrusExtender version : )" +
							  STR::as_string(PE_VER) +

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
	static auto compareVersion(const std::string& a_value) -> std::int32_t
	{
		std::uint32_t major1 = 0, minor1 = 0;
		std::uint32_t major2 = 0, minor2 = 0;

		sscanf_s(a_value.data(), "%u.%u", &major1, &minor1);
		sscanf_s(PE_VER.data(), "%u.%u", &major2, &minor2);

		if (major1 < major2)
			return -1;
		if (major1 > major2)
			return 1;
		if (minor1 < minor2)
			return -1;
		if (minor1 > minor2)
			return 1;

		return 0;
	}
};


//INI AND FORM HANDLING

class INI
{
public:
	static auto Read() -> bool
	{
		pluginPath = SKSE::GetPluginConfigPath("po3_FEC");

		CSimpleIniA ini;

		auto rc = ini.LoadFile(pluginPath.c_str());
		if (rc < 0) {
			logger::error("Can't load 'po3_FEC.ini'");
			return false;
		}

		ini.SetUnicode();

		fixSubtitles = ini.GetBoolValue("Frost", "SubtitleFix", true);
		spellEdits = ini.GetBoolValue("Spell", "SpellDurationEdits", false);
		updateProperties = ini.GetBoolValue("Update", "UpdateScriptProperties", false);

		modName = ini.GetValue("FormIDs", "ModName", "FireBurns.esp");
		deathEffectsNPCID = ini.GetLongValue("FormIDs", "DeathEffectsNPCAbility", 0x0067FB28);
		deathEffectsPCID = ini.GetLongValue("FormIDs", "DeathEffectsPCAbility", 0x00675924);
		fireFXSID = ini.GetLongValue("FormIDs", "FireFXS", 0x000198F3);
		fireHeavyFXSID = ini.GetLongValue("FormIDs", "FireHeavyFXS", 0x000A5038);
		fireDisintegrateFXSID = ini.GetLongValue("FormIDs", "FireDisintegrateFXS", 0x0066B722);

		std::string exclusions(ini.GetValue("Sun", "Exclusions", "NONE"));
		if (exclusions == "NONE") {
			exclusions = "AoL - Pitiless";
			ini.SetValue("Sun", "Exclusions", exclusions.c_str(), ";Excludes magic effects from dealing sun damage. Additional effect names must be seperated using a comma, eg: SunDmgEffect , SunDmgEffect2");
			ini.SaveFile(pluginPath.c_str());
		}
		logger::info("Sun Exclusion effects = {}", exclusions);

		sunExclusions = SKSE::UTIL::STRING::split(exclusions, " , ");

		return true;
	}

	static auto GetFormsFromMod() -> bool
	{
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			deathEffectsAbility = dataHandler->LookupForm<RE::SpellItem>(deathEffectsNPCID, modName);
			if (!deathEffectsAbility) {
				cannotFindESP = true;
				return false;
			}
			deathEffectsPCAbility = dataHandler->LookupForm<RE::SpellItem>(deathEffectsPCID, modName);
			if (!deathEffectsPCAbility) {
				cannotFindESP = true;
				return false;
			}
			mod = dataHandler->LookupLoadedModByName(modName);
			if (!mod) {
				cannotFindESP = true;
				return false;
			}
			return true;
		}
		return false;
	}
};


void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		{
			auto vec = SanityCheck::GetError();
			if (!vec.empty() && vec.size() == 2) {
				auto id = MessageBoxA(nullptr, vec[0].c_str(), vec[1].c_str(), MB_OKCANCEL);
				if (id == 2) {
					std::_Exit(EXIT_FAILURE);
				}
			}
		}
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		{
			if (INI::GetFormsFromMod()) {
				ActorUpdate::Hook();
				ARMOR::Attach::Hook();
				ARMOR::Detach::Hook();
				PATCH::FrozenDeathSubtitles();
				PATCH::SpellDuration();
				PATCH::BodyTintColor();
				PATCH::EmbersXD();
				DISTRIBUTE::DeathEffect();
				DISTRIBUTE::SunKeyword();

				if (auto sourceHolder = RE::ScriptEventSourceHolder::GetSingleton(); sourceHolder) {
					sourceHolder->AddEventSink(RESET::TESResetEventHandler::GetSingleton());
					logger::info("Registered reset event handler.");
				}
			} else {
				logger::info("FEC esp not found, aborting...");
			}
			if (auto sourceHolder = RE::ScriptEventSourceHolder::GetSingleton(); sourceHolder) {
				sourceHolder->AddEventSink(TESLoadGameEventHandler::GetSingleton());
				logger::info("Registered game load event handler.");
			}
		}
		break;
	default:
		break;
	}
}


extern "C" DLLEXPORT bool APIENTRY SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	try {
		auto path = logger::log_directory().value() / "po3_FEC.log";
		auto log = spdlog::basic_logger_mt("global log", path.string(), true);
		log->flush_on(spdlog::level::info);

#ifndef NDEBUG
		log->set_level(spdlog::level::debug);
		log->sinks().push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#else
		log->set_level(spdlog::level::info);

#endif
		set_default_logger(log);
		spdlog::set_pattern("[%H:%M:%S] %v");

		logger::info("po3_FEC v{}", FEC_VERSION_VERSTRING);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "powerofthree's FEC Helper plugin";
		a_info->version = FEC_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			logger::critical("Loaded in editor, marking as incompatible");
			return false;
		}

		const auto ver = a_skse->RuntimeVersion();
		if (ver < SKSE::RUNTIME_1_5_39) {
			logger::critical("Unsupported runtime version {}", ver.string());
			return false;
		}
	} catch (const std::exception& e) {
		logger::critical(e.what());
		return false;
	} catch (...) {
		logger::critical("caught unknown exception");
		return false;
	}

	return true;
}


extern "C" DLLEXPORT bool APIENTRY SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	try {
		logger::info("po3_FEC SSE loaded");

		Init(a_skse);
		SKSE::AllocTrampoline(75);

		INI::Read();

		auto messaging = SKSE::GetMessagingInterface();
		if (!messaging->RegisterListener("SKSE", OnInit)) {
			return false;
		}
	} catch (const std::exception& e) {
		logger::critical(e.what());
		return false;
	} catch (...) {
		logger::critical("caught unknown exception");
		return false;
	}

	return true;
}