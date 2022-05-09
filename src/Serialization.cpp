#include "Serialization.h"
#include "Patches.h"

extern RE::SpellItem* deathEffectsAbility;
extern RE::SpellItem* deathEffectsPCAbility;

namespace FEC::Serialization
{
	std::string DecodeTypeCode(std::uint32_t a_typeCode)
	{
		constexpr std::size_t SIZE = sizeof(std::uint32_t);

		std::string sig;
		sig.resize(SIZE);
		const char* iter = reinterpret_cast<char*>(&a_typeCode);
		for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
			sig[j] = iter[i];
		}

		return sig;
	}

	void Manager::Save(SKSE::SerializationInterface* a_intfc)
	{
		FECreset.Save(a_intfc, kFECReset, kSerializationVersion);

		permanentEffectMap.save(a_intfc, kFECPerm, kSerializationVersion);
		temporaryEffectMap.save(a_intfc, kFECTemp, kSerializationVersion);

		logger::info("Finished saving data"sv);
	}
	void Manager::Load(SKSE::SerializationInterface* a_intfc)
	{
		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (version != kSerializationVersion) {
				logger::critical("Loaded data is out of date! Read ({}), expected ({}) for type code ({})", version, kSerializationVersion, DecodeTypeCode(type));
				continue;
			}
			switch (type) {
			case kFECReset:
				FECreset.Load(a_intfc);
				break;
			case kFECPerm:
				permanentEffectMap.load(a_intfc);
				break;
			case kFECTemp:
				temporaryEffectMap.load(a_intfc);
				break;
			default:
				break;
			}
		}
		logger::info("Finished loading data"sv);
	}
	void Manager::Revert(SKSE::SerializationInterface* a_intfc)
	{
		FECreset.Revert(a_intfc);

		permanentEffectMap.clear();
		temporaryEffectMap.clear();

		logger::info("Finished reverting data"sv);
	}
	void Manager::FormDelete(RE::VMHandle a_handle)
	{
		FECreset.UnregisterAll(a_handle);
	}

	EventResult Manager::ProcessEvent(const RE::TESFormDeleteEvent* a_event, RE::BSTEventSource<RE::TESFormDeleteEvent>*)
	{
		if (a_event && a_event->formID != 0) {
			const auto formID = a_event->formID;

			permanentEffectMap.discard(formID);
			temporaryEffectMap.discard(formID);
		}

		return EventResult::kContinue;
	}

	EventResult Manager::ProcessEvent(const RE::TESResetEvent* a_event, RE::BSTEventSource<RE::TESResetEvent>*)
	{
		if (!a_event || !a_event->object) {
			return EventResult::kContinue;
		}

		const auto actor = a_event->object->As<RE::Actor>();
		if (!actor) {
			return EventResult::kContinue;
		}

		if (permanentEffectMap.contains(actor->formID)) {
			if (!actor->IsAIEnabled()) {
				actor->EnableAI(true);
			}
			if (!actor->IsDead()) {
				permanentEffectMap.assign(actor->formID, ActorEffect::Permanent::kReset);
			}
		}
		if (temporaryEffectMap.contains(actor->formID) && !actor->IsDead()) {
			temporaryEffectMap.reset(actor->formID, ActorEffect::Temporary::kReset);
		}

		return EventResult::kContinue;
	}

	EventResult Manager::ProcessEvent(const RE::TESLoadGameEvent* a_event, RE::BSTEventSource<RE::TESLoadGameEvent>*)
	{
		if (!a_event) {
			return EventResult::kContinue;
		}

		if (const auto player = RE::PlayerCharacter::GetSingleton(); player) {
			player->RemoveSpell(deathEffectsPCAbility);
			player->AddSpell(deathEffectsPCAbility);
		}

		if (const auto processLists = RE::ProcessLists::GetSingleton()) {
			constexpr auto has_base_spell = [](const RE::TESSpellList::SpellData* a_spellList, RE::SpellItem* a_spell) {
				if (a_spellList->spells && a_spellList->numSpells > 0) {
					std::span span(a_spellList->spells, a_spellList->numSpells);
					return std::ranges::find(span, a_spell) != span.end();
				}
				return false;
			};

			for (auto& actorHandle : processLists->highActorHandles) {
				const auto actor = actorHandle.get();
				const auto actorbase = actor ? actor->GetActorBase() : nullptr;
				const auto spellList = actorbase ? actorbase->GetSpellList() : nullptr;

				if (actor && actorbase && spellList) {
					if (has_base_spell(spellList, deathEffectsAbility)) {
						actor->RemoveSpell(deathEffectsAbility);
						actor->AddSpell(deathEffectsAbility);
					} else if (DISTRIBUTE::CanDeathEffectsBeAdded(actorbase)) {
						spellList->AddSpell(deathEffectsAbility);
					}
				}
			}
		}

		return EventResult::kContinue;
	}

	void SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		Manager::GetSingleton()->Save(a_intfc);
	}
	void LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		Manager::GetSingleton()->Load(a_intfc);
	}
	void RevertCallback(SKSE::SerializationInterface* a_intfc)
	{
		Manager::GetSingleton()->Revert(a_intfc);
	}
	void FormDeleteCallback(RE::VMHandle a_handle)
	{
		Manager::GetSingleton()->FormDelete(a_handle);
	}
}
