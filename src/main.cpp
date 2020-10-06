#include "version.h"


std::int32_t actorCount = 0;

bool fixSubtitles = false;
bool spellEdits = false;
bool updateProperties = false;
bool cannotFindESP = false;

RE::BGSKeyword* shockKYWD = nullptr;

RE::SpellItem* deathEffectsAbility = nullptr;
RE::SpellItem* deathEffectsPCAbility = nullptr;

std::string pluginPath;

//------------------------------------------------------------------------------------------------------

typedef const char* (*PEGETVERSION)();

//-------------------------------------------------------------------------------------------------------

bool ReadINI()
{
	pluginPath = SKSE::GetPluginConfigPath("po3_FEC");

	CSimpleIniA ini;
	SI_Error rc = ini.LoadFile(pluginPath.c_str());

	if (rc < 0) {
		logger::error("Can't load 'po3_FEC.ini'");
		return false;
	}

	ini.SetUnicode();

	fixSubtitles = ini.GetBoolValue("Frost", "SubtitleFix", true);
	spellEdits = ini.GetBoolValue("Spell", "SpellDurationEdits", false);
	updateProperties = ini.GetBoolValue("Update", "UpdateScriptProperties", false);

	return true;
}

//----------------------------------------------------------------------------------------

bool GetFormsFromMod()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (dataHandler) {
		deathEffectsAbility = dataHandler->LookupForm<RE::SpellItem>(0x067FB28, "FireBurns.esp");
		if (!deathEffectsAbility) {
			cannotFindESP = true;
			return false;
		}
		deathEffectsPCAbility = dataHandler->LookupForm<RE::SpellItem>(0x00675924, "FireBurns.esp");

		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------

bool ActorHasInactiveBaseSpell(RE::Actor* a_actor, RE::SpellItem* a_spell)
{
	bool hasSpell = false;

	auto base = a_actor->GetActorBase();
	if (base) {
		auto spellList = base->GetOrCreateSpellList();
		if (spellList && spellList->spells) {
			nonstd::span<RE::SpellItem*> span(spellList->spells, spellList->numSpells);
			for (auto& spell : span) {
				if (spell && spell == a_spell) {
					hasSpell = true;
					break;
				}
			}
		}
	}

	if (hasSpell) {
		auto activeEffects = a_actor->GetActiveEffectList();
		if (activeEffects) {
			for (auto& ae : *activeEffects) {
				if (ae && ae->spell == a_spell) {
					return false;
				}
			}
			return true;
		}
	}

	return false;
}


bool ActorHasBaseSpell(RE::Actor* a_actor, RE::SpellItem* a_spell)
{
	bool hasSpell = false;

	auto base = a_actor->GetActorBase();
	if (base) {
		auto spellList = base->GetOrCreateSpellList();
		if (spellList && spellList->spells) {
			nonstd::span<RE::SpellItem*> span(spellList->spells, spellList->numSpells);
			for (auto& spell : span) {
				if (spell && spell == a_spell) {
					hasSpell = true;
					break;
				}
			}
		}
	}

	if (hasSpell) {
		auto activeEffects = a_actor->GetActiveEffectList();
		if (activeEffects) {
			for (auto& ae : *activeEffects) {
				if (ae && ae->spell == a_spell) {
					return true;
				}
			}
			return false;
		}
	}

	return false;
}


namespace ProcessTasks
{
	void ApplySpell()
	{
		RE::func2E9950();

		auto processLists = RE::ProcessLists::GetSingleton();
		if (processLists) {
			auto numHighActors = processLists->numberHighActors;
			if (numHighActors == actorCount) {
				return;
			}
			actorCount = numHighActors;
			for (auto& handle : processLists->highActorHandles) {
				auto actorPtr = handle.get();
				auto actor = actorPtr.get();
				if (actor && actor->IsDead() && ActorHasInactiveBaseSpell(actor, deathEffectsAbility)) {
					actor->AddSpell(deathEffectsAbility);
				}
			}
		}
	}

	void Apply()
	{
		auto& trampoline = SKSE::GetTrampoline();
		REL::Relocation<std::uintptr_t> ProcessTasks_Hook{ RE::Offset::Main::ProcessTasks, 0x04 };
		trampoline.write_call<5>(ProcessTasks_Hook.address(), &ApplySpell);

		logger::info("Hooked process task job.");
	}
}

//----------------------------------------------------------------------------------------

void DoFrozenDeathSubtitlesFix()
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

						if ((topicInfo && topicInfo->data.flags.none(Flags::kNoLIPFile))) {
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

void DoSpellEdits()
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
							if (baseEffect && (baseEffect->HasKeywordString(MagicDamageFrost.data()) || baseEffect->HasKeywordString(MagicDamageShock.data())) && baseEffect->data.castingType == CastingType::kFireAndForget && baseEffect->data.flags.all(Flags::kHostile)) {
								logger::info("	{}", baseEffect->GetName());
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


void DoBodyTintColorEdits()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (dataHandler) {
		logger::info("Starting actorbase bodytint edits");

		CSimpleIniA ini;
		ini.LoadFile(pluginPath.c_str());
		ini.SetUnicode();

		RE::Color falmerColor = ini.GetLongValue("Poison", "Falmer", 0x7f8081);
		std::uint32_t falmerCount = 0;

		RE::Color giantColor = ini.GetLongValue("Poison", "Giant", 0x7f8081);
		std::uint32_t giantCount = 0;

		RE::Color hagravenColor = ini.GetLongValue("Poison", "Hagraven", 0x7f8081);
		std::uint32_t hagravenCount = 0;

		RE::Color rieklingColor = ini.GetLongValue("Poison", "Riekling", 0x7f8081);
		std::uint32_t rieklingCount = 0;

		auto falmerFaction = RE::TESForm::LookupByID<RE::TESFaction>(FormID::falmerFactionID);
		auto giantFaction = RE::TESForm::LookupByID<RE::TESFaction>(FormID::giantFactionID);
		auto hagravenFaction = RE::TESForm::LookupByID<RE::TESFaction>(FormID::hagravenFactionID);
		auto rieklingFaction = RE::TESForm::LookupByID<RE::TESFaction>(FormID::rieklingFactionID);

		for (const auto& actorbase : dataHandler->GetFormArray<RE::TESNPC>()) {
			if (actorbase && !actorbase->HasKeyword("ActorTypeNPC")) {
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


void DoEmbersXDPatch()
{
	std::vector<RE::TESEffectShader*> fireFXS;
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (dataHandler) {
		fireFXS.push_back(dataHandler->LookupForm<RE::TESEffectShader>(0x000198F3, "FireBurns.esp"));
		fireFXS.push_back(dataHandler->LookupForm<RE::TESEffectShader>(0x000A5038, "FireBurns.esp"));
	}

	auto defFireFXS = RE::TESForm::LookupByID<RE::TESEffectShader>(FormID::fireFXShaderID);
	if (defFireFXS && defFireFXS->particleShaderTexture.textureName == embersXDPath.data()) {
		logger::info("Applying EmbersXD patch");
		for (auto& shader : fireFXS) {
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


void DoSunKeywordDistribution()
{
	logger::info("Applying global sun keyword to sun magic effects");
	std::uint32_t count = 0;

	auto sunKeyword = RE::BGSKeyword::CreateKeyword("PO3_MagicDamageSun");
	if (sunKeyword) {
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			auto& keywords = dataHandler->GetFormArray<RE::BGSKeyword>();
			if (std::find(keywords.begin(), keywords.end(), sunKeyword) == keywords.end()) {  //adding to datahandler array because HasKeywordString pulls the string from there
				keywords.push_back(sunKeyword);
			}
			for (const auto& mgef : dataHandler->GetFormArray<RE::EffectSetting>()) {
				if (mgef) {
					auto hitFXS = mgef->data.effectShader;
					auto castArt = mgef->data.castingArt;
					auto hitArt = mgef->data.hitEffectArt;
					if (mgef->HasKeywordString("MAG_MagicDamageSun") || mgef->HasKeywordString("ORD_Res_Sun_Keyword") || hitFXS && std::find(sunHitFXS.begin(), sunHitFXS.end(), hitFXS->GetFormID()) != sunHitFXS.end() || hitArt && std::find(sunHitArt.begin(), sunHitArt.end(), hitArt->GetFormID()) != sunHitArt.end() || castArt && castArt->GetFormID() == FormID::DLC1_SunCloakSpellHandFX) {
						logger::info("	{}", mgef->GetName());
						mgef->AddKeyword(sunKeyword);
						count++;
					}
				}
			}
		}
	}

	logger::info("Finished sun keyword distribution. {} magic effects processed", count);
}


bool CanBeAddedTo(RE::TESNPC* a_actorbase)
{
	if (a_actorbase->HasKeyword("ActorTypeNPC")) {
		if (!a_actorbase->HasKeyword("ActorTypeGhost")) {
			return true;
		}
	}
	if (a_actorbase->HasKeyword("ActorTypeAnimal")) {
		return true;
	}
	if (a_actorbase->HasKeyword("ActorTypeCreature")) {
		if (!a_actorbase->HasKeyword("ActorTypeDragon") && !a_actorbase->HasKeyword("ActorTypeDaedra")) {
			return true;
		}
	}
	return false;
}


void DoSpellDistribution()
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

//----------------------------------------------------------------------------------------

class TESLoadGameEventHandler : public RE::BSTEventSink<RE::TESLoadGameEvent>
{
public:
	static TESLoadGameEventHandler* GetSingleton()
	{
		static TESLoadGameEventHandler singleton;
		return &singleton;
	}

	virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESLoadGameEvent* evn, RE::BSTEventSource<RE::TESLoadGameEvent>* a_eventSource) override
	{
		if (!evn) {
			return RE::BSEventNotifyControl::kContinue;
		}

		if (cannotFindESP) {
			RE::ShowMessageBox(R"(Frozen Electrocuted Combustion [FEC] - Missing formIDs!

This can happen if you merged or eslified FireBurns.esp. Effects will not be applied to NPCs.)");

			return RE::BSEventNotifyControl::kStop;
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		if (player) {
			bool hasSpell = player->HasSpell(deathEffectsPCAbility);
			if (updateProperties) {
				player->RemoveSpell(deathEffectsPCAbility);
				player->AddSpell(deathEffectsPCAbility);
			} else {
				if (!hasSpell) {
					player->AddSpell(deathEffectsPCAbility);
				}
			}
		}

		auto singleton = RE::ProcessLists::GetSingleton();
		for (auto& handle : singleton->highActorHandles) {
			auto actorPtr = handle.get();
			auto actor = actorPtr.get();
			if (actor && ActorHasBaseSpell(actor, deathEffectsAbility)) {
				actor->RemoveSpell(deathEffectsAbility);
				actor->AddSpell(deathEffectsAbility);
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

protected:
	TESLoadGameEventHandler() = default;
	TESLoadGameEventHandler(const TESLoadGameEventHandler&) = delete;
	TESLoadGameEventHandler(TESLoadGameEventHandler&&) = delete;
	virtual ~TESLoadGameEventHandler() = default;

	TESLoadGameEventHandler& operator=(const TESLoadGameEventHandler&) = delete;
	TESLoadGameEventHandler& operator=(TESLoadGameEventHandler&&) = delete;
};

//----------------------------------------------------------------------------------------

std::int32_t compareVersion(const std::string& a_value)
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


void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		{
			HMODULE getPapyrusExtenderDLL = GetModuleHandle("po3_PapyrusExtender");

			if (getPapyrusExtenderDLL == NULL) {
				logger::info("PapyrusExtender SSE plugin not found | error  %d | displaying warning message", GetLastError());

				std::string message = R"(Frozen Electrocuted Combustion [FEC] is missing a required SKSE plugin - Papyrus Extender SSE - and will not run correctly!

Download the plugin from its Skyrim SSE nexus page : https://www.nexusmods.com/skyrimspecialedition/mods/22854)";

				HMODULE getPapyrusUtilsDLL = GetModuleHandle("PapyrusUtil");

				if (getPapyrusUtilsDLL != NULL) {
					message = R"(Frozen Electrocuted Combustion [FEC] is missing a required SKSE plugin - PapyrusExtender SSE - and will not run correctly!

Download the plugin from its Skyrim SSE nexus page : https://www.nexusmods.com/skyrimspecialedition/mods/22854

Please note that PapyrusEXTENDER is NOT PapyrusUTILS, which is a different utility SKSE plugin.)";
				}

				MessageBox(NULL, message.c_str(), "Frozen Electrocuted Combustion - Missing Plugin", MB_OK | MB_ICONWARNING);
			} else {
				PEGETVERSION peGetVersion = (PEGETVERSION)GetProcAddress(getPapyrusExtenderDLL, "GetPluginVersion");

				if (peGetVersion == NULL) {
					logger::info("Failed version check info from PapyrusExtender | error  %d ", GetLastError());

					std::string message = R"(PapyrusExtender [SKSE plugin] is severely out of date!

Frozen Electrocuted Combustion [FEC] will not run correctly.

Required PapyrusExtender version : )" + as_string(PE_VER) +
										  " or higher";

					MessageBox(NULL, message.c_str(), "Frozen Electrocuted Combustion - Outdated Plugin", MB_OK | MB_ICONWARNING);
				} else {
					std::string currentPE = peGetVersion();
					auto compare = compareVersion(currentPE);

					if (compare == -1) {
						logger::info("PapyrusExtender SSE plugin version too low | displaying warning message");

						std::string message = R"(PapyrusExtender [SKSE plugin] is out of date!

Frozen Electrocuted Combustion [FEC] requires version )" +
											  as_string(PE_VER) +

											  R"(

Current PapyrusExtender version : )" + currentPE;

						MessageBox(NULL, message.c_str(), "Frozen Electrocuted Combustion - Outdated Plugin", MB_OK | MB_ICONWARNING);
					} else if (compare == 1) {
						logger::info("PapyrusExtender SSE plugin version too high | displaying warning message");

						std::string message = R"(PapyrusExtender [SKSE plugin] is newer than expected. You'd probably want to update Frozen Electrocuted Combustion [FEC].

Required PapyrusExtender version : )" + as_string(PE_VER) +

											  R"(

Current PapyrusExtender version : )" + currentPE;

						MessageBox(NULL, message.c_str(), "Frozen Electrocuted Combustion - Outdated Mod", MB_OK | MB_ICONWARNING);
					}
				}
			}
		}
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		{
			if (GetFormsFromMod()) {
				ProcessTasks::Apply();
				DoFrozenDeathSubtitlesFix();
				DoSpellDistribution();
				DoSpellEdits();
				DoBodyTintColorEdits();
				DoEmbersXDPatch();
				DoSunKeywordDistribution();
			} else {
				logger::info("FireBurns.esp not found, aborting...");
			}
			auto sourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
			if (sourceHolder) {
				sourceHolder->AddEventSink(TESLoadGameEventHandler::GetSingleton());
				logger::info("Registered game load event handler.");
			}
		}
		break;
	}
}

//----------------------------------------------------------------------------------------

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
		spdlog::set_default_logger(log);
		spdlog::set_pattern("[%H:%M:%S:%e:%f] %v");

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

		SKSE::Init(a_skse);
		SKSE::AllocTrampoline(1 << 4);

		ReadINI();

		auto messaging = SKSE::GetMessagingInterface();
		if (!messaging->RegisterListener("SKSE", OnInit)) {
			logger::critical("Failed to register messaging listener!\n");
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