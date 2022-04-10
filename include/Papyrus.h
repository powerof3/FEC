#pragma once

namespace FEC
{
	namespace DEATH
	{
		enum class MODE : std::uint32_t
		{
			kPermanent,
			kTemporary
		};

		struct TYPE
		{
			enum PERMANENT : std::uint32_t
			{
				kNone = static_cast<std::underlying_type_t<PERMANENT>>(-1),
				kSun = 0,
				kAcid,
				kFire,
				kFrost,
				kShock,
				kDrain,
				kFireFrost,
				kFireShock,
				kDrainFrost,
				kDrainShock,
				kFrostShock,
				kShockFrost
			};

			enum TEMPORARY : std::uint32_t
			{
				kPoison = 0,
				kFear,
				kPoisonFear
			};
		};
	}

	namespace EFFECT
	{
		enum class TYPE : std::uint32_t
		{
			kNone = static_cast<std::underlying_type_t<TYPE>>(-1),
			kCharred = 0,
			kSkeletonized = kCharred,
			kDrained,
			kPoisoned,
			kFrightened = kPoisoned,
			kAged,
			kCharredCreature,
			kFrozenCreature
		};
	}

	namespace Papyrus
	{
		std::vector<std::int32_t> GetCauseOfDeath(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::uint32_t a_type);
		void RemoveEffectsNotOfType(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type);

		bool GetPermanentDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type);
		bool GetTemporaryDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type);

		void AssignPermanentDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type);
		void AssignTemporaryDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type);

		void RemoveTemporaryDeathEffect(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor, std::int32_t a_type);

		void RegisterForFECReset(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::ActiveEffect* a_activeEffect, std::uint32_t a_type);
		void RegisterForFECReset_Form(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form, std::uint32_t a_type);

		void SendFECResetEvent(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::Actor* a_actor, std::uint32_t a_type, bool a_reset);

		void UnregisterForFECReset(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::ActiveEffect* a_activeEffect, std::uint32_t a_type);
		void UnregisterForFECReset_Form(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form, std::uint32_t a_type);

		void UnregisterForAllFECResets(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::ActiveEffect* a_activeEffect);
		void UnregisterForAllFECResets_Form(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, const RE::TESForm* a_form);

		void VaporizeUnderwear(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::Actor* a_actor);

		bool Bind(VM* a_vm);
	}
}
