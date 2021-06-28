#include "Graphics.h"
#include "Settings.h"

//VARS
extern RE::SpellItem* deathEffectsAbility;
extern RE::SpellItem* deathEffectsPCAbility;

extern RE::TESFile* mod;

//EFFECT MAINTAIN
namespace AbMaintain
{
	namespace detail
	{
		bool has_inactive_base_spell(RE::Character* a_actor)
		{
			bool hasSpell = false;

			if (auto base = a_actor->GetActorBase(); base) {
				auto spellList = base->GetSpellList();
				if (spellList && spellList->spells && spellList->numSpells > 0) {
					std::span<RE::SpellItem*> span(spellList->spells, spellList->numSpells);
					for (const auto& spell : span) {
						if (spell == deathEffectsAbility) {
							hasSpell = true;
							break;
						}
					}
				}
			}

			if (hasSpell) {
				const auto activeEffects = a_actor->GetActiveEffectList();
				if (activeEffects) {
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

		void apply_ab(RE::Character* a_this)
		{
			if (!a_this->IsPlayerRef() && has_inactive_base_spell(a_this)) {
				a_this->AddSpell(deathEffectsAbility);
			}
		}
	}

	struct UpdateAVs
	{
		static void thunk(RE::Character* a_this)
		{
			func(a_this);

			detail::apply_ab(a_this);
		}
		static inline REL::Relocation<decltype(&thunk)> func;
	};

	struct ActorUpdateNoAI
	{
		static void thunk(RE::Character* a_this)
		{
			func(a_this);

			detail::apply_ab(a_this);
		}
		static inline REL::Relocation<decltype(&thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(36357), 0x1D1 };
		stl::write_thunk_call<UpdateAVs>(target.address());

		stl::write_vfunc<RE::Character, 0x0AE, ActorUpdateNoAI>();
	}
}

//PATCHES
class PATCH
{
public:
	static void FrozenDeathSubtitles()
	{
		using Flags = RE::TOPIC_INFO_DATA::TOPIC_INFO_FLAGS;

		if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
			const auto frostKYWD = RE::TESForm::LookupByID<RE::BGSKeyword>(formid::frostKeyword);
			if (!frostKYWD) {
				return;
			}

			std::uint32_t topicCount = 0;

			for (const auto& topic : dataHandler->GetFormArray<RE::TESTopic>()) {
				if (topic &&
					topic->topicInfos &&
					topic->numTopicInfos > 0 &&
					topic->data.subtype == RE::DIALOGUE_DATA::Subtype::kDeath) {
					std::span<RE::TESTopicInfo*> span(topic->topicInfos, topic->numTopicInfos);
					for (auto& topicInfo : span) {
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
	}

	static void SpellDuration()
	{
		using Flags = RE::EffectSetting::EffectSettingData::Flag;
		using CastingType = RE::MagicSystem::CastingType;

		if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
			std::uint32_t count = 0;

			const auto is_instant = [](RE::EffectSetting* a_effect) {
				return a_effect && a_effect->HasKeywordString(str::MagicDamageFrost) && a_effect->data.castingType == CastingType::kFireAndForget && a_effect->data.flags.all(Flags::kHostile) && a_effect->data.flags.all(Flags::kDetrimental);
			};

			for (const auto& spell : dataHandler->GetFormArray<RE::SpellItem>()) {
				if (spell) {
					for (auto& effect : spell->effects) {
						if (effect && effect->effectItem.duration == 0) {
							const auto baseEffect = effect->baseEffect;
							if (is_instant(baseEffect)) {
								baseEffect->data.flags.reset(Flags::kNoDuration);
								effect->effectItem.duration = 1;
								count++;
							}
						}
					}
				}
			}
			logger::info("Finished spell edits. {} magic effects processed", count);
		}
	}

	static void BodyTintColor()
	{
		if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
			const auto falmerFaction = RE::TESForm::LookupByID<RE::TESFaction>(formid::falmerFaction);
			const auto giantFaction = RE::TESForm::LookupByID<RE::TESFaction>(formid::giantFaction);
			const auto hagravenFaction = RE::TESForm::LookupByID<RE::TESFaction>(formid::hagravenFaction);
			const auto rieklingFaction = RE::TESForm::LookupByID<RE::TESFaction>(formid::rieklingFaction);

			std::uint32_t falmerCount = 0;
			std::uint32_t giantCount = 0;
			std::uint32_t hagravenCount = 0;
			std::uint32_t rieklingCount = 0;

			auto settings = Settings::GetSingleton();
			auto const colors = settings->creatureColors;

			for (const auto& actorbase : dataHandler->GetFormArray<RE::TESNPC>()) {
				if (actorbase && !actorbase->HasKeyword("ActorTypeNPC"sv)) {
					if (actorbase->IsInFaction(falmerFaction)) {
						falmerCount++;
						actorbase->bodyTintColor = colors[COLOR::kFalmer];
					} else if (actorbase->IsInFaction(giantFaction)) {
						giantCount++;
						actorbase->bodyTintColor = colors[COLOR::kGiant];
					} else if (actorbase->IsInFaction(hagravenFaction)) {
						hagravenCount++;
						actorbase->bodyTintColor = colors[COLOR::kHagraven];
					} else if (actorbase->IsInFaction(rieklingFaction)) {
						rieklingCount++;
						actorbase->bodyTintColor = colors[COLOR::kRiekling];
					}
				}
			}
			logger::info("Finished actorbase bodytint edits on {} falmer, {} giants, {} hagravens and {} rieklings", falmerCount, giantCount, hagravenCount, rieklingCount);
		}
	}

	static void FirePatch()
	{
		const auto defFireFXS = RE::TESForm::LookupByID<RE::TESEffectShader>(formid::fireFXShader);
		if (defFireFXS && defFireFXS->particleShaderTexture.textureName == str::embersXDPath) {
			logger::info("Applying EmbersXD patch");
			auto settings = Settings::GetSingleton();
			for (auto& shader : settings->fireFXS) {
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
		if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
			const auto can_be_added = [](RE::TESNPC* a_npc) {
				if (a_npc->IsSummonable() || a_npc->HasKeyword("ActorTypeGhost"sv) || a_npc->IsGhost()) {
					return false;
				}

				if (a_npc->HasKeyword("ActorTypeNPC"sv)) {
					const auto race = a_npc->GetRace();
					if (const std::string raceName(race ? race->GetFormEditorID() : ""); raceName.find("Child") != std::string::npos) {
						return false;
					}
					return true;
				}
				if (a_npc->HasKeyword("ActorTypeCreature"sv) || a_npc->HasKeyword("ActorTypeAnimal"sv)) {
					if (a_npc->HasKeyword("ActorTypeDragon"sv) || a_npc->HasKeyword("ActorTypeDaedra"sv)) {
						return false;
					}					
					return true;
				}
				
				return false;
			};

			for (const auto& actorbase : dataHandler->GetFormArray<RE::TESNPC>()) {
				if (actorbase && !actorbase->IsPlayer() && can_be_added(actorbase)) {
					if (const auto actorEffects = actorbase->GetSpellList(); actorEffects) {
						actorEffects->AddSpell(deathEffectsAbility);
					}
				}
			}
		}
	}

	static void SunKeyword()
	{
		std::uint32_t count = 0;

		if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
			auto& keywords = dataHandler->GetFormArray<RE::BGSKeyword>();

			RE::BGSKeyword* sunKeyword = nullptr;
			auto it = std::find_if(keywords.begin(), keywords.end(),
				[&](const auto& kywd) -> bool { return kywd && kywd->formEditorID == str::MagicDamageSun; });
			if (it != keywords.end()) {
				sunKeyword = *it;
			} else {
				const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSKeyword>();
				sunKeyword = factory ? factory->Create() : nullptr;
				if (sunKeyword) {
					sunKeyword->formEditorID = str::MagicDamageSun;
					keywords.push_back(sunKeyword);
				}
			}

			if (!sunKeyword) {
				return;
			}

			const auto is_sun_effect = [](RE::EffectSetting* a_mgef, const std::vector<std::string>& a_exclusions) {
				using namespace formid;
				using Flag = RE::EffectSetting::EffectSettingData::Flag;

				const auto hitFXS = a_mgef->data.effectShader;
				bool isSunEffect = hitFXS && std::ranges::find(sunHitFXS, hitFXS->GetFormID()) != sunHitFXS.end();

				if (!isSunEffect) {
					const auto hitArt = a_mgef->data.hitEffectArt;
					isSunEffect = hitArt && std::ranges::find(sunHitArt, hitArt->GetFormID()) != sunHitArt.end();
				}
				if (!isSunEffect) {
					const auto castArt = a_mgef->data.castingArt;
					isSunEffect = castArt && castArt->GetFormID() == DLC1_SunCloakSpellHandFX;
				}

				if (isSunEffect) {
					if (a_mgef->data.flags.none(Flag::kHostile) || a_mgef->data.flags.none(Flag::kDetrimental) || a_mgef->data.castingType == RE::MagicSystem::CastingType::kConstantEffect || a_mgef->data.flags.all(Flag::kNoHitEvent)) {
						return false;
					}
					const std::string name(a_mgef->GetName());
					if (std::ranges::find(a_exclusions, name) != a_exclusions.end()) {
						return false;
					}
					return true;
				}
				return false;
			};

			auto settings = Settings::GetSingleton();
			for (const auto& mgef : dataHandler->GetFormArray<RE::EffectSetting>()) {
				if (mgef && is_sun_effect(mgef, settings->sunExclusions)) {
					mgef->AddKeyword(sunKeyword);
					count++;
				}
			}
		}

		logger::info("Finished sun keyword distribution. {} magic effects processed", count);
	}
};

//LOAD GAME
class TESLoadGameEventHandler : public RE::BSTEventSink<RE::TESLoadGameEvent>
{
public:
	static TESLoadGameEventHandler* GetSingleton()
	{
		static TESLoadGameEventHandler singleton;
		return &singleton;
	}

	RE::BSEventNotifyControl ProcessEvent(const RE::TESLoadGameEvent* evn, RE::BSTEventSource<RE::TESLoadGameEvent>*) override
	{
		if (!evn) {
			return RE::BSEventNotifyControl::kContinue;
		}

		static auto settings = Settings::GetSingleton();
		if (settings->cannotFindESP) {
			RE::DebugMessageBox(R"(Frozen Electrocuted Combustion [FEC] - Missing formIDs! Effects will not be applied!

If you have renamed/merged/ESLfied FireBurns.esp, make sure to input the new formids in po3_FEC.ini)");
			settings->cannotFindESP = false;
		}

		if (const auto player = RE::PlayerCharacter::GetSingleton(); player) {
			player->RemoveSpell(deathEffectsPCAbility);
			player->AddSpell(deathEffectsPCAbility);
		}

		if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
			for (auto& actorHandle : processLists->highActorHandles) {
				auto actor = actorHandle.get();
				if (actor && util::has_base_spell(actor.get(), deathEffectsAbility)) {
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
};

// ERROR HANDLING
class SanityCheck
{
public:
	using PEGETVERSION = const char* (*)();

	static auto GetError() -> std::vector<std::string>
	{
		std::vector<std::string> vec;

		const auto papyrusExtenderHandle = GetModuleHandle(ver::PapyrusExtender.data());
		const auto papyrusUtilsHandle = GetModuleHandle(ver::PapyrusUtil.data());

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
				          std::string(ver::PE) +
				          " or higher";
			} else {
				std::string currentPE(peGetVersion());
				auto compare = compareVersion(currentPE);

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
	static auto compareVersion(const std::string& a_value) -> std::int32_t
	{
		std::uint32_t major1 = 0, minor1 = 0;
		std::uint32_t major2 = 0, minor2 = 0;

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
			auto vec = SanityCheck::GetError();
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
			auto settings = Settings::GetSingleton();
			if (settings->GetFormsFromMod()) {
				const auto get_tweaks_fix = []() {
					const auto po3TweaksHandle = GetModuleHandle(ver::po3Tweaks.data());
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
					AbMaintain::Install();
				} else {
					logger::info("powerofthree's tweaks mod found, skipping ability maintainer");
				}

				ARMOR::Attach::Install();
				ARMOR::Detach::Install();

				if (settings->fixSubtitles) {
					PATCH::FrozenDeathSubtitles();
				} else {
					logger::info("subtitle fix disabled");
				}

				/*if (settings->spellEdits) {
					PATCH::SpellDuration();
				} else {
					logger::info("spell edits disabled");
				}*/ 

				PATCH::BodyTintColor();
				PATCH::FirePatch();
				DISTRIBUTE::DeathEffect();
				DISTRIBUTE::SunKeyword();

				if (const auto sourceHolder = RE::ScriptEventSourceHolder::GetSingleton(); sourceHolder) {
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
	spdlog::set_pattern("[%H:%M:%S] [%l] %v"s);

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
