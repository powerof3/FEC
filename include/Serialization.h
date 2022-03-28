#pragma once

namespace FEC::Serialization
{
	using EventResult = RE::BSEventNotifyControl;
	using Lock = std::mutex;
	using Locker = std::scoped_lock<Lock>;

	enum : std::uint32_t
	{
		kSerializationVersion = 1,

		kFEC = 'FECM',

		kFECReset = 'FECR',
		kFECEffect = 'FECE'
	};

	struct ActorEffect
	{
		enum class Permanent
		{
			kNone = static_cast<std::underlying_type_t<Permanent>>(-1),
			kCharred = 0,
			kSkeletonized,
			kDrained,
			kFrozen
		};

		enum class Temporary
		{
			kNone = static_cast<std::underlying_type_t<Temporary>>(-1),
			kAged,
			kXRayed,
			kPoisoned,
			kFrightened,
			kBuriedInAsh,
			kSuffocated
		};
	};

	class Manager : public RE::BSTEventSink<RE::TESFormDeleteEvent>
	{
	public:
		template <class T>
		class ActorEffectMap
		{
		public:
			[[nodiscard]] bool contains(RE::FormID a_key) const
			{
				Locker locker(_lock);
				return _map.contains(a_key);
			}
			[[nodiscard]] T find(RE::FormID a_key) const
			{
				Locker locker(_lock);

				const auto it = _map.find(a_key);
				return it != _map.end() ? it->second : static_cast<T>(-1);
			}

			bool assign(RE::FormID a_key, T a_mapped)
			{
				Locker locker(_lock);
				return _map.insert_or_assign(a_key, std::move(a_mapped)).second;
			}
			bool discard(RE::FormID a_key, T a_mapped)
			{
				Locker locker(_lock);
				return _map.erase(a_key, std::move(a_mapped)).second;
			}
			bool discard(RE::FormID a_key)
			{
				Locker locker(_lock);
				return _map.erase(a_key);
			}

			void load(const SKSE::SerializationInterface* a_intfc)
			{
				assert(a_intfc);
				std::size_t numRegs;
				a_intfc->ReadRecordData(numRegs);

				Locker locker(_lock);
				_map.clear();

				RE::FormID formID;
				std::uint32_t effect;

				for (std::size_t i = 0; i < numRegs; i++) {
					a_intfc->ReadRecordData(formID);
					if (!a_intfc->ResolveFormID(formID, formID)) {
						logger::warn("{} : Failed to resolve formID {:X}"sv, i, formID);
						continue;
					}
					a_intfc->ReadRecordData(effect);

					_map.emplace(formID, static_cast<T>(effect));
				}
			}
			bool save(SKSE::SerializationInterface* a_intfc, std::uint32_t a_type, std::uint32_t a_version)
			{
				assert(a_intfc);
				if (!a_intfc->OpenRecord(a_type, a_version)) {
					logger::error("Failed to open record!");
					return false;
				}

				return save(a_intfc);
			}
			bool save(SKSE::SerializationInterface* a_intfc)
			{
				assert(a_intfc);
				Locker locker(_lock);

				const std::size_t numRegs = _map.size();
				if (!a_intfc->WriteRecordData(numRegs)) {
					logger::error("Failed to save number of regs ({})!", numRegs);
					return false;
				}

				for (auto& reg : _map) {
					if (!a_intfc->WriteRecordData(reg.first)) {
						logger::error("Failed to save reg ({}: {})!", reg.first, reg.second);
						return false;
					}
					if (!a_intfc->WriteRecordData(reg.second)) {
						logger::error("Failed to save reg ({}: {})!", reg.first, reg.second);
						return false;
					}
				}

				return true;
			}
			void clear()
			{
				Locker locker(_lock);
				_map.clear();
			}

		private:
			mutable Lock _lock{};
			robin_hood::unordered_flat_map<RE::FormID, T> _map{};
		};

	    static Manager* GetSingleton()
		{
			static Manager singleton;
			return &singleton;
		}

		static void Register()
		{
			if (const auto scripts = RE::ScriptEventSourceHolder::GetSingleton()) {
				scripts->AddEventSink(GetSingleton());
				logger::info("Registered form deletion event handler"sv);
			}
		}

		SKSE::RegistrationMap<const RE::Actor*, std::uint32_t, bool> FECreset{ "OnFECReset"sv };

		void Save(SKSE::SerializationInterface* a_intfc);
		void Load(SKSE::SerializationInterface* a_intfc);
		void Revert(SKSE::SerializationInterface* a_intfc);
		void FormDelete(RE::VMHandle a_handle);

		EventResult ProcessEvent(const RE::TESFormDeleteEvent* a_event, RE::BSTEventSource<RE::TESFormDeleteEvent>*) override;

		ActorEffectMap<ActorEffect::Permanent> permanentEffectMap;
		ActorEffectMap<ActorEffect::Temporary> temporaryEffectMap;

	private:
		Manager() = default;
		Manager(const Manager&) = delete;
		Manager(Manager&&) = delete;
		~Manager() override = default;

		Manager& operator=(const Manager&) = delete;
		Manager& operator=(Manager&&) = delete;
	};

	void SaveCallback(SKSE::SerializationInterface* a_intfc);
	void LoadCallback(SKSE::SerializationInterface* a_intfc);
	void RevertCallback(SKSE::SerializationInterface* a_intfc);
	void FormDeleteCallback(RE::VMHandle a_handle);
}
