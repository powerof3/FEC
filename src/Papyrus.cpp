#include "Papyrus.h"

#include "Graphics.h"
#include "Serialization.h"

namespace FEC::Papyrus
{
	using ActorEffect = Serialization::ActorEffect;

	using deathEffectPair = std::pair<std::uint32_t, RE::EffectSetting*>;                               // [type, mgef]
	using deathEffectMap = std::map<std::uint32_t, std::vector<std::pair<RE::EffectSetting*, float>>>;  // [type, [mgef, mag]]

	struct detail
	{
		static bool process_active_effect(RE::ActiveEffect* a_activeEffect, DEATH::MODE a_mode, deathEffectPair& a_effectPair, deathEffectMap& a_effectMap)
		{
			using FLAG = RE::EffectSetting::EffectSettingData::Flag;
			using AE_FLAG = RE::ActiveEffect::Flag;
			using CAST_TYPE = RE::MagicSystem::CastingType;
			using Archetype = RE::EffectArchetypes::ArchetypeID;

			const auto mgef = a_activeEffect ? a_activeEffect->GetBaseObject() : nullptr;
			if (mgef && mgef->data.flags.all(FLAG::kHostile) && mgef->data.flags.all(FLAG::kDetrimental)) {
				if (a_mode == DEATH::MODE::kPermanent) {
					if (mgef->HasKeyword(keyword::Sun)) {
						a_effectPair = { DEATH::TYPE::PERMANENT::kSun, mgef };  //sun override
						return true;
					}
					/*if (mgef->data.resistVariable == RE::ActorValue::kPoisonResist && mgef->data.castingType == CAST_TYPE::kConcentration) {
                    a_effectPair = { DEATH::TYPE::PERMANENT::kAcid, mgef }; //acid override
                    return true;
                }*/
					if (mgef->HasKeyword(keyword::Fire)) {
						a_effectMap[DEATH::TYPE::kFire].emplace_back(mgef, -a_activeEffect->magnitude);  //flipping the magnitude back to +ve
					} else if (mgef->HasKeyword(keyword::Frost)) {
						a_effectMap[DEATH::TYPE::kFrost].emplace_back(mgef, -a_activeEffect->magnitude);
					} else if (mgef->HasKeyword(keyword::Shock)) {
						a_effectMap[DEATH::TYPE::kShock].emplace_back(mgef, -a_activeEffect->magnitude);
					} else if (mgef->GetArchetype() == Archetype::kAbsorb) {
						a_effectMap[DEATH::TYPE::kDrain].emplace_back(mgef, -a_activeEffect->magnitude);
					}
				} else {
					if (mgef->data.resistVariable == RE::ActorValue::kPoisonResist && mgef->data.castingType != CAST_TYPE::kConcentration) {
						a_effectMap[DEATH::TYPE::kPoison].emplace_back(mgef, -a_activeEffect->magnitude);
					} else if (mgef->GetArchetype() == Archetype::kDemoralize) {
						a_effectMap[DEATH::TYPE::kFear].emplace_back(mgef, -a_activeEffect->magnitude);
					}
				}
			}
			return false;
		}
	};

	std::vector<std::int32_t> GetCauseOfDeath(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_type)
	{
		using FLAG = RE::EffectSetting::EffectSettingData::Flag;
		using AE_FLAG = RE::ActiveEffect::Flag;
		using CAST_TYPE = RE::MagicSystem::CastingType;
		using Archetype = RE::EffectArchetypes::ArchetypeID;

		std::vector<std::int32_t> vec;
		vec.resize(3, -1);

		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return vec;
		}

#ifndef SKYRIMVR
		const auto activeEffects = a_actor->GetActiveEffectList();
		if (!activeEffects) {
			a_vm->TraceForm(a_actor, "has no active effects", a_stackID, Severity::kInfo);
			return vec;
		}
#endif

		deathEffectPair effectPair{ -1, nullptr };
		deathEffectMap effectMap{};

		const auto deathMode = static_cast<DEATH::MODE>(a_type);

#ifndef SKYRIMVR
		for (const auto& activeEffect : *activeEffects) {
			if (detail::process_active_effect(activeEffect, deathMode, effectPair, effectMap)) {
				break;
			}
		}
#else
		//VR work around because GetActiveEffectList does not return a BSSimpleList like in SSE
		a_actor->VisitActiveEffects([&](RE::ActiveEffect* activeEffect) -> RE::BSContainer::ForEachResult {
			if (detail::process_active_effect(activeEffect, deathMode, effectPair, effectMap)) {
				return RE::BSContainer::ForEachResult::kStop;
			}
			return RE::BSContainer::ForEachResult::kContinue;
		});
#endif

		if (effectPair.first == DEATH::TYPE::kNone && !effectMap.empty()) {
			constexpr auto mag_cmp = [](const auto& a_lhs, const auto& a_rhs) {
				return numeric::definitely_less_than(a_lhs.second, a_rhs.second);
			};

			if (effectMap.size() == 1) {
				const auto& type = effectMap.begin()->first;
				auto [effect, effectMag] = *std::ranges::max_element(effectMap.begin()->second, mag_cmp);
				effectPair = { type, effect };
			} else {
				if (a_type != 0) {
					bool poison = !effectMap[DEATH::TYPE::kPoison].empty();
					bool fear = !effectMap[DEATH::TYPE::kFear].empty();

					if (poison) {
						auto& poisonVec = effectMap[DEATH::TYPE::kPoison];
						auto poisonEffect = *std::ranges::max_element(poisonVec, mag_cmp);

						effectPair = { DEATH::TYPE::kPoison, poisonEffect.first };
						if (fear) {
							effectPair.first = DEATH::TYPE::kPoisonFear;
						}
					} else if (fear) {
						auto& fearVec = effectMap[DEATH::TYPE::kFear];
						auto poisonEffect = *std::ranges::max_element(fearVec, mag_cmp);

						effectPair = { DEATH::TYPE::kFear, poisonEffect.first };
					}
				} else {
					bool fire = !effectMap[DEATH::TYPE::kFire].empty();
					bool frost = !effectMap[DEATH::TYPE::kFrost].empty();
					bool shock = !effectMap[DEATH::TYPE::kShock].empty();
					bool drain = !effectMap[DEATH::TYPE::kDrain].empty();

					if (fire) {
						auto& fireVec = effectMap[DEATH::TYPE::kFire];
						auto fireEffect = *std::ranges::max_element(fireVec, mag_cmp);

						effectPair = { DEATH::TYPE::kFire, fireEffect.first };
						if (frost) {
							effectPair.first = DEATH::TYPE::kFireFrost;
						} else if (shock) {
							effectPair.first = DEATH::TYPE::kFireShock;
						}
					} else if (drain) {
						auto& drainVec = effectMap[DEATH::TYPE::kDrain];
						auto drainEffect = *std::ranges::max_element(drainVec, mag_cmp);

						effectPair = { DEATH::TYPE::kDrain, drainEffect.first };
						if (shock) {
							effectPair.first = DEATH::TYPE::kDrainShock;
							auto& shockVec = effectMap[DEATH::TYPE::kShock];
							auto shockEffect = *std::ranges::max_element(shockVec, mag_cmp);

							if (numeric::definitely_less_than(drainEffect.second, shockEffect.second)) {
								effectPair.second = shockEffect.first;
							}
						} else if (frost) {
							effectPair.first = DEATH::TYPE::kDrainFrost;
							auto& frostVec = effectMap[DEATH::TYPE::kFrost];
							auto frostEffect = *std::ranges::max_element(frostVec, mag_cmp);

							if (numeric::definitely_less_than(drainEffect.second, frostEffect.second)) {
								effectPair.second = frostEffect.first;
							}
						}
					} else if (frost) {
						auto& frostVec = effectMap[DEATH::TYPE::kFrost];
						auto frostEffect = *std::ranges::max_element(frostVec, mag_cmp);

						effectPair = { DEATH::TYPE::kFrost, frostEffect.first };
						if (shock) {
							auto& shockVec = effectMap[DEATH::TYPE::kShock];
							auto shockEffect = *std::ranges::max_element(shockVec, mag_cmp);

							if (numeric::definitely_less_than(frostEffect.second, shockEffect.second)) {
								effectPair = { DEATH::TYPE::kShockFrost, shockEffect.first };
							} else {
								effectPair.first = { DEATH::TYPE::kFrostShock };
							}
						}
					} else if (shock) {
						auto& shockVec = effectMap[DEATH::TYPE::kShock];
						auto shockEffect = *std::ranges::max_element(shockVec, mag_cmp);

						effectPair = { DEATH::TYPE::kShock, shockEffect.first };
					}
				}
			}
		}

		if (effectPair.first != DEATH::TYPE::kNone) {
			auto& [value, mgef] = effectPair;
			if (mgef) {
				vec[0] = value;
				vec[1] = mgef->GetMinimumSkillLevel();
				if (const auto projectile = mgef->data.projectileBase; projectile) {
					vec[2] = projectile->GetType();
				}
			}
		}

		return vec;
	}

	void RemoveEffectsNotOfType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type)
	{
		using namespace GRAPHICS;

		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		const auto root = a_actor->Get3D(false);
		if (!root) {
			a_vm->TraceForm(a_actor, "has no 3D", a_stackID, Severity::kInfo);
			return;
		}

		bool result = false;
		RESET::ResetData resetData{};

		std::tie(result, resetData) = RESET::get_data(root);
		if (!result) {
			return;
		}

		auto type = static_cast<EFFECT::TYPE>(a_type);

		SKSE::GetTaskInterface()->AddTask([a_actor, type, root, resetData]() {
			auto& [toggleData, skinTintData, hairTintData, alphaSkinData, txstFaceData, headpartAlphaVec, txstVec, txstSkinVec, shaderVec] = resetData;

			switch (type) {
			case EFFECT::TYPE::kCharred:
				{
					RESET::HeadPartAlpha(a_actor, root, headpartAlphaVec);
					RESET::SkinTint(a_actor, root, skinTintData);
					RESET::HairTint(a_actor, root, hairTintData);
					RESET::FaceTXST(a_actor, root, txstFaceData);
					RESET::SkinTXST(a_actor, root, txstSkinVec);
				}
				break;
			case EFFECT::TYPE::kDrained:
				{
					RESET::Toggle(root, toggleData);
					RESET::SkinTint(a_actor, root, skinTintData);
					RESET::HairTint(a_actor, root, hairTintData);
					RESET::FaceTXST(a_actor, root, txstFaceData);
					RESET::SkinTXST(a_actor, root, txstSkinVec);
				}
				break;
			case EFFECT::TYPE::kPoisoned:
				{
					RESET::Toggle(root, toggleData);
					RESET::SkinAlpha(root, alphaSkinData);
					RESET::HeadPartAlpha(a_actor, root, headpartAlphaVec);
					RESET::FaceTXST(a_actor, root, txstFaceData);
					RESET::SkinTXST(a_actor, root, txstSkinVec);

					if (!a_actor->IsPlayerRef()) {
						RESET::stop_all_skin_shaders(a_actor);
					}
				}
				break;
			case EFFECT::TYPE::kAged:
				{
					RESET::Toggle(root, toggleData);
					RESET::SkinAlpha(root, alphaSkinData);
					RESET::HeadPartAlpha(a_actor, root, headpartAlphaVec);

					if (!a_actor->IsPlayerRef()) {
						RESET::stop_all_skin_shaders(a_actor);
					}
				}
				break;
			case EFFECT::TYPE::kCharredCreature:
				{
					RESET::MaterialShader(root, shaderVec);
				}
				break;
			default:
				break;
			}
		});
	}

	bool GetPermanentDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return false;
		}

		const auto manager = Serialization::Manager::GetSingleton();
		const auto effectType = static_cast<ActorEffect::Permanent>(a_type);

		if (effectType == ActorEffect::Permanent::kNone) {
			return manager->permanentEffectMap.contains(a_actor->GetFormID());
		}

		return manager->permanentEffectMap.find(a_actor->GetFormID()) == effectType;
	}

	bool GetTemporaryDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return false;
		}

		const auto manager = Serialization::Manager::GetSingleton();
		const auto effectType = static_cast<ActorEffect::Temporary>(a_type);

		if (effectType == ActorEffect::Temporary::kNone) {
			return manager->temporaryEffectMap.contains(a_actor->GetFormID());
		}

		const auto set = manager->temporaryEffectMap.find(a_actor->GetFormID());
		return set && set->contains(effectType);
	}

	void AssignPermanentDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		const auto manager = Serialization::Manager::GetSingleton();
		const auto effectType = static_cast<ActorEffect::Permanent>(a_type);

		if (effectType == ActorEffect::Permanent::kNone) {
			manager->permanentEffectMap.discard(a_actor->GetFormID());
		} else {
			manager->permanentEffectMap.assign(a_actor->GetFormID(), effectType);
		}
	}

	void AssignTemporaryDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		const auto manager = Serialization::Manager::GetSingleton();
		const auto effectType = static_cast<ActorEffect::Temporary>(a_type);

		if (effectType == ActorEffect::Temporary::kNone) {
			manager->temporaryEffectMap.discard(a_actor->GetFormID());
		} else {
			manager->temporaryEffectMap.assign(a_actor->GetFormID(), effectType);
		}
	}

	void RemoveTemporaryDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		const auto manager = Serialization::Manager::GetSingleton();
		const auto effectType = static_cast<ActorEffect::Temporary>(a_type);

		if (effectType == ActorEffect::Temporary::kNone) {
			manager->temporaryEffectMap.discard(a_actor->GetFormID());
		} else {
			manager->temporaryEffectMap.discard(a_actor->GetFormID(), effectType);
		}
	}

	void RegisterForFECReset(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::ActiveEffect* a_activeEffect, std::uint32_t a_type)
	{
		if (!a_activeEffect) {
			a_vm->TraceStack("Active Effect is None", a_stackID);
			return;
		}

		auto& regs = Serialization::Manager::GetSingleton()->FECreset;
		regs.Register(a_activeEffect, a_type);
	}

	void RegisterForFECReset_Form(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form, std::uint32_t a_type)
	{
		if (!a_form) {
			a_vm->TraceStack("Active Effect is None", a_stackID);
			return;
		}

		auto& regs = Serialization::Manager::GetSingleton()->FECreset;
		regs.Register(a_form, a_type);
	}

	void SendFECResetEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor, std::uint32_t a_type, bool a_reset)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		Serialization::Manager::GetSingleton()->FECreset.QueueEvent(a_type, a_actor, a_type, a_reset);
	}

	void UnregisterForFECReset(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::ActiveEffect* a_activeEffect, std::uint32_t a_type)
	{
		if (!a_activeEffect) {
			a_vm->TraceStack("Active Effect is None", a_stackID);
			return;
		}

		auto& regs = Serialization::Manager::GetSingleton()->FECreset;
		regs.Unregister(a_activeEffect, a_type);
	}

	void UnregisterForAllFECResets(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::ActiveEffect* a_activeEffect)
	{
		if (!a_activeEffect) {
			a_vm->TraceStack("Active Effect is None", a_stackID);
			return;
		}

		auto& regs = Serialization::Manager::GetSingleton()->FECreset;
		regs.UnregisterAll(a_activeEffect);
	}

	void VaporizeUnderwear(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (!a_actor) {
			a_vm->TraceStack("Actor is None", a_stackID);
			return;
		}

		const auto root = a_actor->Get3D(false);
		if (!root) {
			a_vm->TraceForm(a_actor, "has no 3D", a_stackID, Severity::kInfo);
			return;
		}

		SKSE::GetTaskInterface()->AddTask([a_actor, root]() {
			for (auto& underwear : underwear::underwears) {
				if (const auto object = root->GetObjectByName(underwear)) {
					GRAPHICS::SET::Toggle(root, object, true);
				}
			}
		});
	}

	bool Bind(VM* a_vm)
	{
		if (!a_vm) {
			logger::critical("couldn't get VM State"sv);
			return false;
		}

		auto constexpr obj = "FEC_Utility"sv;

		BIND(GetCauseOfDeath);
		BIND(RemoveEffectsNotOfType);
		BIND(SendFECResetEvent, true);

		BIND(GetPermanentDeathEffect, true);
		BIND(GetTemporaryDeathEffect, true);

		BIND(AssignPermanentDeathEffect, true);
		BIND(AssignTemporaryDeathEffect, true);

		BIND(RemoveTemporaryDeathEffect, true);

		BIND(RegisterForFECReset, true);
		BIND(RegisterForFECReset_Form, true);

		BIND(UnregisterForFECReset, true);
		BIND(UnregisterForAllFECResets, true);

		BIND(VaporizeUnderwear);

		logger::info("Registered FEC functions"sv);

		return true;
	}
}
