#include "event.h"

//-----------------------------------------------------------------------
std::unordered_set<RE::Actor*> swimmingActors;

const UInt32 fireKeywordID = 0x0001CEAD;
const UInt32 frostKeywordID = 0x0001CEAE;
const UInt32 shockKeywordID = 0x0001CEAF;

//-----------------------------------------------------------------------

void DebugNotification(const char* a_notification, void* a_arg2, bool a_arg3)
{
	using func_t = function_type_t<decltype(&DebugNotification)>;
	REL::Offset<func_t*> func(RE::Offset::DebugNotification);
	return func(a_notification, a_arg2, a_arg3);
}

//------------------------------------------------------------------------

BSAnimationGraphEventHandler* BSAnimationGraphEventHandler::GetSingleton()
{
	static BSAnimationGraphEventHandler singleton;
	return &singleton;
}

auto BSAnimationGraphEventHandler::ReceiveEvent(RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
-> EventResult
{
	if (!a_event || !a_event->akTarget)
		return EventResult::kContinue;

	RE::Actor * thisActor = skyrim_cast<RE::Actor*>(a_event->akTarget);

	if (!thisActor)
		return EventResult::kContinue;

	if (a_event->animName == "SoundPlay")
	{
		if (!swimmingActors.count(thisActor) && thisActor->IsSwimming())
		{
			swimmingActors.insert(thisActor);
			DebugNotification("started swimming", 0, 1);
		}
	}
	else if (a_event->animName == "SoundPlay.FSTSwimSwim")
	{
		if (!swimmingActors.count(thisActor))
		{
			swimmingActors.insert(thisActor);
			DebugNotification("started swimming", 0, 1);
		}
	}
	else if (a_event->animName == "EnableBumper")
	{
		if (swimmingActors.count(thisActor))
		{
			swimmingActors.erase(thisActor);
			DebugNotification("stopped swimming", 0, 1);
		}
	}

	return EventResult::kContinue;
}

//--------------------------------------------------------------------------------

TESMagicEffectApplyEventHandler* TESMagicEffectApplyEventHandler::GetSingleton()
{
	static TESMagicEffectApplyEventHandler singleton;
	return &singleton;
}

auto TESMagicEffectApplyEventHandler::ReceiveEvent(RE::TESMagicEffectApplyEvent* a_event, RE::BSTEventSource<RE::TESMagicEffectApplyEvent>* a_eventSource)
-> EventResult
{
	if (a_event->target == nullptr)
		return EventResult::kContinue;

	RE::Actor * thisActor = skyrim_cast<RE::Actor*>(a_event->target);

	if (!thisActor || !swimmingActors.count(thisActor))
		return EventResult::kContinue;

	auto thisEffect = skyrim_cast<RE::EffectSetting*>(RE::TESForm::LookupByID(a_event->formID));

	if (!thisEffect || (thisEffect->data.flags & RE::EffectSetting::Data::Flag::kHostile) != RE::EffectSetting::Data::Flag::kHostile)
		return EventResult::kContinue;

	RE::BGSKeyword * magicDamageFire = skyrim_cast<RE::BGSKeyword*>(RE::TESForm::LookupByID(fireKeywordID));
	if (thisEffect->HasKeyword(magicDamageFire))
	{
		return EventResult::kContinue;
	}
	RE::BGSKeyword * magicDamageFrost = skyrim_cast<RE::BGSKeyword*>(RE::TESForm::LookupByID(frostKeywordID));
	if (thisEffect->HasKeyword(magicDamageFrost))
	{
		return EventResult::kContinue;
	}
	RE::BGSKeyword * magicDamageShock = skyrim_cast<RE::BGSKeyword*>(RE::TESForm::LookupByID(shockKeywordID));
	if (thisEffect->HasKeyword(magicDamageShock))
	{
		return EventResult::kContinue;
	}

	return EventResult::kContinue;
}