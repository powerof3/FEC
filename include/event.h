#pragma once

#include "main.h"

class BSAnimationGraphEventHandler : public RE::BSTEventSink<RE::BSAnimationGraphEvent>
{
public:
	using EventResult = RE::EventResult;

	static BSAnimationGraphEventHandler* GetSingleton();
	virtual EventResult ReceiveEvent(RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) override;

protected:
	BSAnimationGraphEventHandler() = default;
	BSAnimationGraphEventHandler(const BSAnimationGraphEventHandler&) = delete;
	BSAnimationGraphEventHandler(BSAnimationGraphEventHandler&&) = delete;
	virtual ~BSAnimationGraphEventHandler() = default;

	BSAnimationGraphEventHandler& operator=(const BSAnimationGraphEventHandler&) = delete;
	BSAnimationGraphEventHandler& operator=(BSAnimationGraphEventHandler&&) = delete;
};

//--------------------------------------------------------------------------------

class TESMagicEffectApplyEventHandler : public RE::BSTEventSink<RE::TESMagicEffectApplyEvent>
{
public:
	using EventResult = RE::EventResult;

	static TESMagicEffectApplyEventHandler* GetSingleton();
	virtual EventResult ReceiveEvent(RE::TESMagicEffectApplyEvent* a_event, RE::BSTEventSource<RE::TESMagicEffectApplyEvent>* a_eventSource) override;

protected:
	TESMagicEffectApplyEventHandler() = default;
	TESMagicEffectApplyEventHandler(const TESMagicEffectApplyEventHandler&) = delete;
	TESMagicEffectApplyEventHandler(TESMagicEffectApplyEventHandler&&) = delete;
	virtual ~TESMagicEffectApplyEventHandler() = default;

	TESMagicEffectApplyEventHandler& operator=(const TESMagicEffectApplyEventHandler&) = delete;
	TESMagicEffectApplyEventHandler& operator=(TESMagicEffectApplyEventHandler&&) = delete;
};