#include "Serialization.h"

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

		permanentEffectMap.save(a_intfc, kFECEffect, kSerializationVersion);
		temporaryEffectMap.save(a_intfc, kFECEffect, kSerializationVersion);

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
			case kFECEffect:
				{
					permanentEffectMap.load(a_intfc);
					temporaryEffectMap.load(a_intfc);
				}
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
