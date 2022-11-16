#include "Patches.h"

namespace FEC
{
	namespace PATCH
	{
		void FrozenDeathSubtitles()
		{
			using Flags = RE::TOPIC_INFO_DATA::TOPIC_INFO_FLAGS;

			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				const auto frostKYWD = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(keyword::Frost);
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
								const auto newNode = new RE::TESConditionItem;

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
				const auto falmerFaction = RE::TESForm::LookupByID<RE::TESFaction>(faction::falmer);
				const auto giantFaction = RE::TESForm::LookupByID<RE::TESFaction>(faction::giant);
				const auto hagravenFaction = RE::TESForm::LookupByID<RE::TESFaction>(faction::hagraven);
				const auto rieklingFaction = RE::TESForm::LookupByID<RE::TESFaction>(faction::riekling);
				const auto thirstRieklingFaction = RE::TESForm::LookupByID<RE::TESFaction>(faction::thirstRiekling);

				std::uint32_t falmerCount = 0;
				std::uint32_t giantCount = 0;
				std::uint32_t hagravenCount = 0;
				std::uint32_t rieklingCount = 0;

				for (const auto& actorbase : dataHandler->GetFormArray<RE::TESNPC>()) {
					if (actorbase && !actorbase->HasApplicableKeywordString("ActorTypeNPC"sv)) {
						if (actorbase->IsInFaction(falmerFaction)) {
							falmerCount++;
							actorbase->bodyTintColor = color::falmer;
						} else if (actorbase->IsInFaction(giantFaction)) {
							giantCount++;
							actorbase->bodyTintColor = color::giant;
						} else if (actorbase->IsInFaction(hagravenFaction)) {
							hagravenCount++;
							actorbase->bodyTintColor = color::hagraven;
						} else if (actorbase->IsInFaction(rieklingFaction) || actorbase->IsInFaction(thirstRieklingFaction)) {
							rieklingCount++;
							actorbase->bodyTintColor = color::riekling;
						}
					}
				}
				logger::info("Finished actorbase bodytint edits on {} falmer, {} giants, {} hagravens and {} rieklings", falmerCount, giantCount, hagravenCount, rieklingCount);
			}
		}

		void FireShaders()
		{
			const auto defFireFXS = RE::TESForm::LookupByID<RE::TESEffectShader>(shader::fireFXShader);
			if (defFireFXS && defFireFXS->particleShaderTexture.textureName == str::embersXDPath) {
				std::vector<RE::TESEffectShader*> fireFXS;
				fireFXS.reserve(shader::FEC_fireFXS.size());

				for (auto& formID : shader::FEC_fireFXS) {
					fireFXS.emplace_back(RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESEffectShader>(formID, "FEC.esp"));
				}

				for (const auto& shader : fireFXS) {
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

		void Cooking()
		{
			const auto dataHandler = RE::TESDataHandler::GetSingleton();

			const auto listFoodRaw = dataHandler->LookupForm<RE::BGSListForm>(list::rawFood, "FEC.esp");
			const auto listFoodCooked = dataHandler->LookupForm<RE::BGSListForm>(list::cookedFood, "FEC.esp");

			if (dataHandler->LookupModByName("Complete Alchemy & Cooking Overhaul.esp")) {
				for (auto& [rawID, cookedID] : food::caco_map) {
					listFoodRaw->AddForm(RE::TESForm::LookupByEditorID(rawID));
					listFoodCooked->AddForm(RE::TESForm::LookupByEditorID(cookedID));
				}
				logger::info("Applied CACO patch");
			}
			if (dataHandler->LookupModByName("Hunterborn.esp")) {
				for (auto& [rawID, cookedID] : food::hunterborn_map) {
					listFoodRaw->AddForm(RE::TESForm::LookupByEditorID(rawID));
					listFoodCooked->AddForm(RE::TESForm::LookupByEditorID(cookedID));
				}
				logger::info("Applied Hunterborn patch");
			}
		}

		void Install()
		{
			FrozenDeathSubtitles();
			BodyTintColor();
			FireShaders();
			Cooking();
		}
	}

	namespace POST_LOAD_PATCH
	{
		void FireShaders()
		{
			const auto FEC_FireFXParticleCount = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(global::FEC_FireFXParticleCount, "FEC.esp");

			if (!FEC_FireFXParticleCount) {
				return;
			}

			std::vector<RE::TESEffectShader*> fireFXS;
			fireFXS.reserve(shader::FEC_fireFXS.size());

			for (auto& formID : shader::FEC_fireFXS) {
				fireFXS.emplace_back(RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESEffectShader>(formID, "FEC.esp"));
			}

			for (const auto& shader : fireFXS) {
				if (shader) {
					shader->data.particleShaderPersistantParticleCount = FEC_FireFXParticleCount->value / 1.2857f;
					shader->data.particleShaderFullParticleBirthRatio = FEC_FireFXParticleCount->value;
				}
			}
			logger::info("Applied fire shader particle counts");
		}

		void SunShader()
		{
			const auto FEC_SunFXParticleCount = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(global::FEC_SunFXParticleCount, "FEC.esp");
			const auto sunFXS = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESEffectShader>(shader::FEC_sunFXS, "FEC.esp");

			if (sunFXS && FEC_SunFXParticleCount) {
				sunFXS->data.particleShaderPersistantParticleCount = FEC_SunFXParticleCount->value / 1.2857f;
				sunFXS->data.particleShaderFullParticleBirthRatio = FEC_SunFXParticleCount->value;
			}

			logger::info("Applied sun shader particle counts");
		}

		void MiscPatches()
		{
			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler->LookupModByName("DizonaBody.esm")) {
				dataHandler->LookupForm<RE::TESGlobal>(global::FEC_DizonaInstalled, "FEC.esp")->value = 1.0f;
			}
		}

		void Install()
		{
			FireShaders();
			SunShader();
			MiscPatches();
		}
	}

	namespace DISTRIBUTE
	{
		bool CanDeathEffectsBeAdded(RE::TESNPC* a_npc)
		{
			if (a_npc->IsSummonable() || a_npc->HasApplicableKeywordString(keyword::Ghost) || a_npc->IsGhost()) {
				return false;
			}

			if (a_npc->HasApplicableKeywordString(keyword::NPC)) {
			    const auto race = a_npc->GetRace();
                return !(race && (race->IsChildRace() || string::icontains(race->GetFormEditorID(), "Child")));
			}

			if (a_npc->HasApplicableKeywordString(keyword::Creature) || a_npc->HasApplicableKeywordString(keyword::Animal)) {
				return !(a_npc->HasApplicableKeywordString(keyword::Dragon) || a_npc->HasApplicableKeywordString(keyword::Daedra));
			}

			return false;
		}

		void DeathEffect()
		{
			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				for (const auto& actorbase : dataHandler->GetFormArray<RE::TESNPC>()) {
					if (actorbase && !actorbase->IsPlayer() && CanDeathEffectsBeAdded(actorbase)) {
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
				auto sunKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(keyword::Sun);
				if (!sunKeyword) {
					const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSKeyword>();
					sunKeyword = factory ? factory->Create() : nullptr;
					if (sunKeyword) {
						sunKeyword->SetFormEditorID(keyword::Sun.data());

						auto& keywords = dataHandler->GetFormArray<RE::BGSKeyword>();
						keywords.push_back(sunKeyword);
					}
				}

				if (!sunKeyword) {
					return;
				}

				constexpr auto is_sun_effect = [](const RE::EffectSetting* a_mgef) {
					using Flag = RE::EffectSetting::EffectSettingData::Flag;

					const auto hitFXS = a_mgef->data.effectShader;
					bool isSunEffect = hitFXS && std::ranges::find(shader::sunHitFXS, hitFXS->GetFormID()) != shader::sunHitFXS.end();

					if (!isSunEffect) {
						const auto hitArt = a_mgef->data.hitEffectArt;
						isSunEffect = hitArt && std::ranges::find(shader::sunHitArt, hitArt->GetFormID()) != shader::sunHitArt.end();
					}
					if (!isSunEffect) {
						const auto castArt = a_mgef->data.castingArt;
						isSunEffect = castArt && castArt->GetFormID() == shader::DLC1_SunCloakSpellHandFX;
					}

					return isSunEffect && !(a_mgef->data.flags.none(Flag::kHostile) || a_mgef->data.flags.none(Flag::kDetrimental) || a_mgef->data.castingType == RE::MagicSystem::CastingType::kConstantEffect || a_mgef->data.flags.all(Flag::kNoHitEvent));
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
	}
}
