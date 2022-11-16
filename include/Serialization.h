#pragma once

namespace FEC::Serialization
{
	using EventResult = RE::BSEventNotifyControl;
	using Lock = std::shared_mutex;
	using Locker = std::scoped_lock<Lock>;

	enum : std::uint32_t
	{
		kSerializationVersion = 1,
		kFECResetVersion = 2,

		kFEC = 'FECK',

		kFECReset = 'REST',
		kFECPerm = 'PERM',
		kFECTemp = 'TEMP'
	};

	struct ActorEffect
	{
		enum class Permanent
		{
			kReset = static_cast<std::underlying_type_t<Permanent>>(-2),
			kNone = static_cast<std::underlying_type_t<Permanent>>(-1),
			kCharred = 0,
			kSkeletonized,
			kDrained,
			kFrozen
		};

		enum class Temporary
		{
			kReset = static_cast<std::underlying_type_t<Temporary>>(-2),
			kNone = static_cast<std::underlying_type_t<Temporary>>(-1),
			kAged,
			kXRayed,
			kPoisoned,
			kFrightened,
			kSuffocated,
			kFrozenShader
		};
	};

	class Manager :
		public RE::BSTEventSink<RE::TESFormDeleteEvent>,
		public RE::BSTEventSink<RE::TESResetEvent>,
		public RE::BSTEventSink<RE::TESLoadGameEvent>
	{
	public:
		using TempEffectSet = robin_hood::unordered_flat_set<ActorEffect::Temporary>;

		template <class T>
		class ActorEffectMap
		{
		public:
			[[nodiscard]] bool contains(RE::FormID a_key) const
			{
				Locker locker(_lock);
				return _map.contains(a_key);
			}
			[[nodiscard]] std::optional<T> find(RE::FormID a_key) const
			{
				Locker locker(_lock);

				const auto it = _map.find(a_key);
				return it != _map.end() ? std::make_optional(it->second) : std::nullopt;
			}

			bool assign(RE::FormID a_key, ActorEffect::Permanent a_mapped)
			{
				Locker locker(_lock);
				return _map.insert_or_assign(a_key, a_mapped).second;
			}
			bool assign(RE::FormID a_key, ActorEffect::Temporary a_mapped)
			{
				Locker locker(_lock);
				return _map[a_key].insert(a_mapped).second;
			}
			bool reset(RE::FormID a_key, ActorEffect::Temporary a_mapped)
			{
				Locker locker(_lock);

				_map[a_key].clear();
				return _map[a_key].insert(a_mapped).second;
			}

			bool discard(RE::FormID a_key, ActorEffect::Permanent a_mapped)
			{
				Locker locker(_lock);
				return _map.erase(a_key, std::move(a_mapped));
			}
			bool discard(RE::FormID a_key, ActorEffect::Temporary a_mapped)
			{
				Locker locker(_lock);
				return _map[a_key].erase(std::move(a_mapped));
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

				if constexpr (std::is_same_v<T, ActorEffect::Permanent>) {
					RE::FormID formID;
					std::int32_t effect;

					for (std::size_t i = 0; i < numRegs; i++) {
						a_intfc->ReadRecordData(formID);
						if (!a_intfc->ResolveFormID(formID, formID)) {
							logger::warn("{} : Failed to resolve formID {:X}"sv, i, formID);
							continue;
						}
						a_intfc->ReadRecordData(effect);

						_map.emplace(formID, static_cast<T>(effect));
					}

				} else {
					RE::FormID formID;
					std::size_t numEffects;
					std::int32_t effect;

					for (std::size_t i = 0; i < numRegs; i++) {
						a_intfc->ReadRecordData(formID);
						if (!a_intfc->ResolveFormID(formID, formID)) {
							logger::warn("{} : Failed to resolve formID {:X}"sv, i, formID);
							continue;
						}
						a_intfc->ReadRecordData(numEffects);
						for (std::size_t j = 0; j < numEffects; j++) {
							a_intfc->ReadRecordData(effect);
							_map[formID].insert(static_cast<ActorEffect::Temporary>(effect));
						}
					}
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
					logger::error("Failed to save map size ({})!", numRegs);
					return false;
				}

				if constexpr (std::is_same_v<T, ActorEffect::Permanent>) {
					for (auto& [key, mapped] : _map) {
						if (!a_intfc->WriteRecordData(key)) {
							logger::error("	Failed to save key ({:X}: {})!", key, stl::to_underlying(mapped));
							return false;
						}
						if (!a_intfc->WriteRecordData(stl::to_underlying(mapped))) {
							logger::error("	Failed to save value ({:X}: {})!", key, stl::to_underlying(mapped));
							return false;
						}
					}
				} else {
					for (auto& [key, set] : _map) {
						if (!a_intfc->WriteRecordData(key)) {
							logger::error("	Failed to save key ({:X})!", key);
							return false;
						}
						if (!a_intfc->WriteRecordData(set.size())) {
							logger::error("	Failed to save value size ({:X})!", key);
							return false;
						}
						for (auto& mapped : set) {
							if (!a_intfc->WriteRecordData(stl::to_underlying(mapped))) {
								logger::error("	Failed to save reg ({:X} : {})!", key, stl::to_underlying(mapped));
								return false;
							}
						}
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
				scripts->AddEventSink<RE::TESFormDeleteEvent>(GetSingleton());
				scripts->AddEventSink<RE::TESResetEvent>(GetSingleton());
				scripts->AddEventSink<RE::TESLoadGameEvent>(GetSingleton());
				logger::info("Registered form deletion event handler"sv);
			}
		}

		SKSE::RegistrationMap<std::uint32_t, const RE::Actor*, std::uint32_t, bool> FECreset{ "OnFECReset"sv };

		void Save(SKSE::SerializationInterface* a_intfc);
		void Load(SKSE::SerializationInterface* a_intfc);
		void Revert(SKSE::SerializationInterface* a_intfc);
		void FormDelete(RE::VMHandle a_handle);

		EventResult ProcessEvent(const RE::TESFormDeleteEvent* a_event, RE::BSTEventSource<RE::TESFormDeleteEvent>*) override;
		EventResult ProcessEvent(const RE::TESResetEvent* a_event, RE::BSTEventSource<RE::TESResetEvent>*) override;
		EventResult ProcessEvent(const RE::TESLoadGameEvent* a_event, RE::BSTEventSource<RE::TESLoadGameEvent>*) override;

		ActorEffectMap<ActorEffect::Permanent> permanentEffectMap;
		ActorEffectMap<TempEffectSet> temporaryEffectMap;

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
