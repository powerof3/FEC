#include "Patches.h"

extern RE::SpellItem* deathEffectsAbility;
extern RE::SpellItem* deathEffectsPCAbility;

extern RE::TESFile* mod;

namespace MAINTENANCE
{
	namespace Ability
	{
		struct detail
		{
			static bool has_inactive_base_spell(RE::Character* a_actor)
			{
				bool hasSpell = false;

				if (const auto base = a_actor->GetActorBase(); base) {
					const auto spellList = base->GetSpellList();
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
						for (const auto& ae : *activeEffects) {
							if (ae && ae->spell == deathEffectsAbility) {
								return false;
							}
						}
					}
					return true;
				}

				return false;
			}

			static void apply_ab(RE::Character* a_this)
			{
				if (!a_this->IsPlayerRef() && has_inactive_base_spell(a_this)) {
					a_this->AddSpell(deathEffectsAbility);
				}
			}
		};

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
			static void thunk(RE::Character* a_this, float a_delta)
			{
				func(a_this, a_delta);

				detail::apply_ab(a_this);
			}
			static inline REL::Relocation<decltype(&thunk)> func;
		};
	}

	TESLoadGameEventHandler* TESLoadGameEventHandler::GetSingleton()
	{
		static TESLoadGameEventHandler singleton;
		return &singleton;
	}

	RE::BSEventNotifyControl TESLoadGameEventHandler::ProcessEvent(const RE::TESLoadGameEvent* evn, RE::BSTEventSource<RE::TESLoadGameEvent>*)
	{
		if (!evn) {
			return RE::BSEventNotifyControl::kContinue;
		}

		if (const auto player = RE::PlayerCharacter::GetSingleton(); player) {
			player->RemoveSpell(deathEffectsPCAbility);
			player->AddSpell(deathEffectsPCAbility);
		}

		if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
			constexpr auto has_base_spell = [](RE::Actor* a_actor, RE::SpellItem* a_spell) {
				bool hasSpell = false;

				const auto base = a_actor->GetActorBase();
				const auto spellList = base ? base->GetSpellList() : nullptr;

				if (spellList && spellList->spells && spellList->numSpells > 0) {
					std::span<RE::SpellItem*> span(spellList->spells, spellList->numSpells);
					for (const auto& spell : span) {
						if (spell == a_spell) {
							hasSpell = true;
							break;
						}
					}
				}

				return hasSpell;
			};

			for (auto& actorHandle : processLists->highActorHandles) {
				auto actor = actorHandle.get();
				if (actor && has_base_spell(actor.get(), deathEffectsAbility)) {
					actor->RemoveSpell(deathEffectsAbility);
					actor->AddSpell(deathEffectsAbility);
				}
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::Offset(0x5FBC50) };
		stl::write_thunk_call<Ability::UpdateAVs>(target.address() + 0x1AA);

		stl::write_vfunc<RE::Character, 0x0AE, Ability::ActorUpdateNoAI>();

		const auto sourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
		if (sourceHolder) {
			sourceHolder->AddEventSink(TESLoadGameEventHandler::GetSingleton());
		}
	}
}

namespace PATCH
{
	void FrozenDeathSubtitles()
	{
		using Flags = RE::TOPIC_INFO_DATA::TOPIC_INFO_FLAGS;

		if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
			const auto frostKYWD = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(str::MagicDamageFrost);
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
					for (const auto& topicInfo : span) {
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

	void BodyTintColor()
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

			for (const auto& actorbase : dataHandler->GetFormArray<RE::TESNPC>()) {
				if (actorbase && !actorbase->HasKeyword("ActorTypeNPC"sv)) {
					if (actorbase->IsInFaction(falmerFaction)) {
						falmerCount++;
						actorbase->bodyTintColor = 0x8F7F75;
					} else if (actorbase->IsInFaction(giantFaction)) {
						giantCount++;
						actorbase->bodyTintColor = 0x786f6A;
					} else if (actorbase->IsInFaction(hagravenFaction)) {
						hagravenCount++;
						actorbase->bodyTintColor = 0x72615B;
					} else if (actorbase->IsInFaction(rieklingFaction)) {
						rieklingCount++;
						actorbase->bodyTintColor = 0x374655;
					}
				}
			}
			logger::info("Finished actorbase bodytint edits on {} falmer, {} giants, {} hagravens and {} rieklings", falmerCount, giantCount, hagravenCount, rieklingCount);
		}
	}

	void FireShaders()
	{
		const auto defFireFXS = RE::TESForm::LookupByID<RE::TESEffectShader>(formid::fireFXShader);
		if (defFireFXS && defFireFXS->particleShaderTexture.textureName == str::embersXDPath) {
			std::vector<RE::TESEffectShader*> fireFXS;
			fireFXS.reserve(formid::FEC_fireFXS.size());
			for (auto& formID : formid::FEC_fireFXS) {
				fireFXS.emplace_back(RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESEffectShader>(formID, "FEC.esp"));
			}
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
			logger::info("Applied EmbersXD patch");
		}
	}

	void Install()
	{
		FrozenDeathSubtitles();
		BodyTintColor();
		FireShaders();
	}
};

namespace DISTRIBUTE
{
	void DeathEffect()
	{
		if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
			constexpr auto can_be_added = [](RE::TESNPC* a_npc) {
				if (a_npc->IsSummonable() || a_npc->HasKeyword("ActorTypeGhost"sv) || a_npc->IsGhost()) {
					return false;
				}

				if (a_npc->HasKeyword("ActorTypeNPC"sv)) {
					const auto race = a_npc->GetRace();
					const std::string raceName = race ? race->GetFormEditorID() : "";
					return raceName.find("Child") == std::string::npos;
				}

				if (a_npc->HasKeyword("ActorTypeCreature"sv) || a_npc->HasKeyword("ActorTypeAnimal"sv)) {
					return !(a_npc->HasKeyword("ActorTypeDragon"sv) || a_npc->HasKeyword("ActorTypeDaedra"sv));
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

	void SunKeyword()
	{
		std::uint32_t count = 0;

		if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
			auto& keywords = dataHandler->GetFormArray<RE::BGSKeyword>();

			RE::BGSKeyword* sunKeyword;
			auto it = std::ranges::find_if(keywords,
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

			constexpr auto is_sun_effect = [](const RE::EffectSetting* a_mgef) {
				using Flag = RE::EffectSetting::EffectSettingData::Flag;

				const auto hitFXS = a_mgef->data.effectShader;
				bool isSunEffect = hitFXS && std::ranges::find(formid::sunHitFXS, hitFXS->GetFormID()) != formid::sunHitFXS.end();

				if (!isSunEffect) {
					const auto hitArt = a_mgef->data.hitEffectArt;
					isSunEffect = hitArt && std::ranges::find(formid::sunHitArt, hitArt->GetFormID()) != formid::sunHitArt.end();
				}
				if (!isSunEffect) {
					const auto castArt = a_mgef->data.castingArt;
					isSunEffect = castArt && castArt->GetFormID() == formid::DLC1_SunCloakSpellHandFX;
				}

				if (isSunEffect) {
					if (a_mgef->data.flags.none(Flag::kHostile) || a_mgef->data.flags.none(Flag::kDetrimental) || a_mgef->data.castingType == RE::MagicSystem::CastingType::kConstantEffect || a_mgef->data.flags.all(Flag::kNoHitEvent)) {
						return false;
					}
				}
				return false;
			};

			for (const auto& mgef : dataHandler->GetFormArray<RE::EffectSetting>()) {
				if (mgef && is_sun_effect(mgef)) {
					mgef->AddKeyword(sunKeyword);
					count++;
				}
			}
		}

		logger::info("Finished sun keyword distribution. {} magic effects processed", count);
	}

	void Install()
	{
		DeathEffect();
		SunKeyword();
	}
};
