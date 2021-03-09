#include "graphics.h"

extern const RE::TESFile* mod;
extern RE::SpellItem* deathEffectsAbility;

namespace
{
	auto IsInFECFaction(RE::Actor* a_actor) -> bool
	{
		bool result = false;

		a_actor->VisitFactions([&result](RE::TESFaction* a_faction, std::int8_t a_rank) {
			if (a_faction && a_rank > -1) {
				const std::string name(a_faction->GetName());
				for (const auto& facName : effectDoneFac) {
					if (facName == name) {
						result = true;
						return true;
					}
				}
			}
			return false;
		});

		return result;
	}


	auto GetFECFaction(RE::Actor* a_actor) -> std::int32_t
	{
		std::int32_t x = -1;

		a_actor->VisitFactions([&x](RE::TESFaction* a_faction, std::int8_t a_rank) {
			if (a_faction && a_rank > -1) {
				const std::string name(a_faction->GetName());
				for (std::int32_t i = 0; i < effectFac.size(); i++) {
					if (name == effectFac[i]) {
						x = i;
						return true;
					}
				}
			}
			return false;
		});

		return x;
	}


	auto constexpr const_hash(const char* input, const std::uint32_t hash = 5381) -> std::uint32_t
	{
		return *input ?
					 const_hash(input + 1, hash * 33 + static_cast<std::uint32_t>(*input)) :
					 hash;
	}


	auto ActorHasBaseSpell(RE::Actor* a_actor, RE::SpellItem* a_spell) -> bool
	{
		bool hasSpell = false;

		if (auto base = a_actor->GetActorBase(); base) {
			const auto spellList = base->GetOrCreateSpellList();
			if (spellList && spellList->spells) {
				stl::span<RE::SpellItem*> span(spellList->spells, spellList->numSpells);
				for (auto& spell : span) {
					if (spell == a_spell) {
						hasSpell = true;
						break;
					}
				}
			}
		}

		return hasSpell;
	}


	auto GetData(RE::NiAVObject* a_object) -> std::optional<std::uint32_t>
	{
		if (!a_object->extra || a_object->extraDataSize == 0) {
			return std::nullopt;
		}

		std::optional<std::uint32_t> result = std::nullopt;

		stl::span<RE::NiExtraData*> span(a_object->extra, a_object->extraDataSize);
		for (auto& extraData : span) {
			if (result.has_value()) {
				break;
			}

			if (extraData && !extraData->name.empty()) {
				switch (const_hash(extraData->name.c_str())) {
				case const_hash("EXTRA_HEAD"):
					result = 0;
					break;
				case const_hash("EXTRA_BODY"):
					result = 1;
					break;
				case const_hash("EXTRA_CHARRED_BODY"):
					result = 2;
					break;
				default:
					break;
				}
			}
		}

		return result;
	}
}


namespace ARMOR
{
	void Graphics::SetSkinAlpha(RE::NiAVObject* a_root, float a_alpha, bool a_setData)
	{
		a_root->UpdateMaterialAlpha(a_alpha, true);

		if (a_setData) {
			if (a_alpha == 1.0f) {
				a_root->RemoveExtraData("PO3_SKINALPHA"sv);
			} else if (const auto data = a_root->GetExtraData<RE::NiBooleanExtraData>("PO3_SKINALPHA"sv); !data) {
				const auto newData = RE::NiBooleanExtraData::Create("PO3_SKINALPHA"sv, true);
				if (newData) {
					a_root->AddExtraData(newData);
				}
			}
		}
	}


	void Graphics::SetSkinAlpha(RE::NiAVObject* a_root, RE::NiAVObject* a_node, float a_alpha, bool a_setData)
	{
		a_node->UpdateMaterialAlpha(a_alpha, true);

		if (a_setData) {
			if (a_alpha == 1.0f) {
				a_root->RemoveExtraData("PO3_SKINALPHA"sv);
			} else if (const auto data = a_root->GetExtraData<RE::NiBooleanExtraData>("PO3_SKINALPHA"sv); !data) {
				const auto newData = RE::NiBooleanExtraData::Create("PO3_SKINALPHA"sv, true);
				if (newData) {
					a_root->AddExtraData(newData);
				}
			}
		}
	}


	void Graphics::ToggleNode(RE::NiAVObject* a_root, const RE::BSFixedString& a_nodeName, const bool a_toggle, bool a_setData)
	{
		auto object = a_root->GetObjectByName(a_nodeName);
		if (object) {
			object->ToggleNode(a_toggle);

			if (a_setData) {
				if (auto data = a_root->GetExtraData<RE::NiStringsExtraData>("PO3_TOGGLE"sv); data) {
					a_toggle ? data->Insert(a_nodeName) : data->Remove(a_nodeName);
				} else if (a_toggle) {
					std::vector<RE::BSFixedString> vec;
					vec.push_back(a_nodeName);
					if (const auto newData = RE::NiStringsExtraData::Create("PO3_TOGGLE"sv, vec); newData) {
						a_root->AddExtraData(newData);
					}
				}
			}
		}
	}


	void Graphics::ToggleNode(RE::NiAVObject* a_root, RE::NiAVObject* a_node, bool a_toggle, bool a_setData)
	{
		a_node->ToggleNode(a_toggle);

		if (a_setData) {
			if (auto data = a_root->GetExtraData<RE::NiStringsExtraData>("PO3_TOGGLE"sv); data) {
				a_toggle ? data->Insert(a_node->name) : data->Remove(a_node->name);
			} else if (a_toggle) {
				std::vector<RE::BSFixedString> vec;
				vec.push_back(a_node->name);
				if (const auto newData = RE::NiStringsExtraData::Create("PO3_TOGGLE"sv, vec); newData) {
					a_root->AddExtraData(newData);
				}
			}
		}
	}


	void Graphics::SetHeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, HeadPart a_type, float a_alpha, bool a_setData)
	{
		if (auto object = a_actor->GetHeadPartObject(a_type); object) {
			object->UpdateMaterialAlpha(a_alpha, false);

			if (a_setData) {
				const auto name = "PO3_HEADPART - " + std::to_string(to_underlying(a_type));
				if (a_alpha == 1.0f) {
					a_root->RemoveExtraData(name);
				} else {
					if (const auto data = a_root->GetExtraData<RE::NiIntegerExtraData>(name); !data) {
						if (const auto newData = RE::NiIntegerExtraData::Create(name, to_underlying(a_type)); newData) {
							a_root->AddExtraData(newData);
						}
					}
				}
			}
		}
	}


	void Attach::Hook()
	{
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<std::uintptr_t> ProcessAttachedGeometry{ REL::ID(15535) };
		_ProcessGeometry = trampoline.write_call<5>(ProcessAttachedGeometry.address() + 0x79A, ProcessGeometry);  //armor

		REL::Relocation<std::uintptr_t> AttachArmorAddon{ REL::ID(15501) };
		_ProcessObject = trampoline.write_call<5>(AttachArmorAddon.address() + 0x1EA, ProcessObject);  //armor2

		REL::Relocation<std::uintptr_t> ProcessArmorAttach{ REL::ID(24236) };
		_PerformNPCDismember = trampoline.write_call<5>(ProcessArmorAttach.address() + 0x33E, PerformNPCDismember);	 //head

		logger::info("Hooked armor attach.");
	}


	void Attach::ProcessGeometry(RE::BipedAnim* a_biped, RE::BSGeometry* a_object, RE::BSDismemberSkinInstance* a_dismemberInstance, std::int32_t a_slot, bool a_unk05)
	{
		using ShaderType = RE::BSShaderMaterial::Feature;

		_ProcessGeometry(a_biped, a_object, a_dismemberInstance, a_slot, a_unk05);

		if (a_biped && a_object) {
			const auto refPtr = a_biped->actorRef.get();
			const auto ref = refPtr.get();

			if (ref) {
				const auto actor = ref->As<RE::Actor>();
				if (!actor || actor->IsPlayerRef() || !actor->HasKeyword(NPC)) {
					return;
				}

				const auto facType = GetFECFaction(actor);
				if (facType == -1) {
					return;
				}

				const auto root = actor->Get3D(false);
				if (!root) {
					return;
				}

				if (std::any_of(fxBiped.begin(), fxBiped.end(), [&](const auto& bipedSlot) {
						auto biped = a_biped->objects[bipedSlot];
						auto addon = biped.addon;

						return addon && mod->IsFormInMod(addon->formID);
					})) {
					if (!a_object->name.empty()) {
						switch (const_hash(a_object->name.c_str())) {
						case const_hash(Underwear::male0):
						case const_hash(Underwear::male1):
						case const_hash(Underwear::female):
						case const_hash(Underwear::bra0):
						case const_hash(Underwear::bra1):
						case const_hash(Underwear::bra2):
						case const_hash(Underwear::panty0):
						case const_hash(Underwear::panty1):
						case const_hash(Underwear::panty2):
							{
								if (facType <= 1) {
									ToggleNode(root, a_object, true);
								}
							}
							break;
						default:
							{
								if (a_object->HasShaderType(ShaderType::kFaceGenRGBTint)) {
									const auto alpha = facType == 3 ? 0.5f : 0.0f;
									SetSkinAlpha(root, a_object, alpha);
								}
							}
							break;
						}
					}
				}
			}
		}
	}


	void Attach::ProcessObject(RE::BipedAnim* a_biped, RE::NiAVObject* a_object, std::int32_t a_slot, bool a_unk04)
	{
		using ShaderType = RE::BSShaderMaterial::Feature;

		_ProcessObject(a_biped, a_object, a_slot, a_unk04);

		if (a_biped && a_object) {
			const auto refPtr = a_biped->actorRef.get();
			const auto ref = refPtr.get();

			if (ref) {
				const auto actor = ref->As<RE::Actor>();
				if (!actor || actor->IsPlayerRef() || !actor->HasKeyword(NPC)) {
					return;
				}

				const auto facType = GetFECFaction(actor);
				if (facType == -1) {
					return;
				}

				const auto root = actor->Get3D(false);
				if (!root) {
					return;
				}

				if (std::any_of(fxBiped.begin(), fxBiped.end(), [&](const auto& bipedSlot) {
						auto biped = a_biped->objects[bipedSlot];
						auto addon = biped.addon;

						return addon && mod->IsFormInMod(addon->formID);
					})) {
					RE::BSVisit::TraverseScenegraphGeometries(a_object, [&](RE::BSGeometry* a_geom) -> RE::BSVisit::BSVisitControl {
						if (!a_geom->name.empty()) {
							switch (const_hash(a_geom->name.c_str())) {
							case const_hash(Underwear::male0):
							case const_hash(Underwear::male1):
							case const_hash(Underwear::female):
							case const_hash(Underwear::bra0):
							case const_hash(Underwear::bra1):
							case const_hash(Underwear::bra2):
							case const_hash(Underwear::panty0):
							case const_hash(Underwear::panty1):
							case const_hash(Underwear::panty2):
								{
									if (facType <= 1) {
										ToggleNode(root, a_object, true);
									}
								}
								break;
							default:
								{
									if (a_object->HasShaderType(ShaderType::kFaceGenRGBTint)) {
										const auto alpha = facType == 3 ? 0.5f : 0.0f;
										SetSkinAlpha(root, a_object, alpha);
									}
								}
								break;
							}
						}
						return RE::BSVisit::BSVisitControl::kContinue;
					});
				}
			}
		}
	}


	void Attach::PerformNPCDismember(RE::TESNPC* a_npc, RE::Actor* a_actor, RE::NiAVObject* a_node)
	{
		_PerformNPCDismember(a_npc, a_actor, a_node);

		if (a_actor && !a_actor->IsPlayerRef() && a_actor->HasKeyword(NPC)) {
			const auto facType = GetFECFaction(a_actor);
			if (facType == -1) {
				return;
			}
			const auto biped = a_actor->GetCurrentBiped().get();
			const auto root = a_actor->Get3D();
			if (root && biped) {
				const auto bipedObject = biped->objects[Biped::kModMouth];
				const auto addon = bipedObject.addon;

				if (addon && mod->IsFormInMod(addon->formID)) {
					if (facType == 0 || facType == 1) {
						ToggleNode(root, RE::FixedStrings::GetSingleton()->bsFaceGenNiNodeSkinned, true);
					} else if (facType == 2) {
						for (auto& headpart : headparts) {
							SetHeadPartAlpha(a_actor, root, headpart, 0.0f);
						}
					} else {
						if (a_actor->HasKeyword(IsBeast)) {
							SetHeadPartAlpha(a_actor, root, HeadPart::kMisc, 0.0f);
						}
					}
				}
			}
		}
	}


	void Detach::Hook()
	{
		REL::Relocation<std::uintptr_t> RemoveNodeFromScene{ REL::ID(15495) };

		auto& trampoline = SKSE::GetTrampoline();
		_UpdateCollision = trampoline.write_call<5>(RemoveNodeFromScene.address() + 0x1F, UpdateCollision);

		logger::info("Hooked armor detach.");
	}


	void Detach::ResetActor(RE::NiAVObject* a_object, RE::Actor* a_actor)
	{
		const auto root = a_actor->Get3D(false);
		if (!root) {
			return;
		}

		RE::BSVisit::TraverseScenegraphGeometries(a_object, [&](RE::BSGeometry* a_geom) -> RE::BSVisit::BSVisitControl {
			auto result = RE::BSVisit::BSVisitControl::kContinue;

			auto val = GetData(a_geom);
			if (!val.has_value()) {
				return result;
			}

			switch (val.value()) {
			case 0:
				{
					ToggleNode(root, RE::FixedStrings::GetSingleton()->bsFaceGenNiNodeSkinned, false, false);
					for (auto& headpart : headparts) {
						SetHeadPartAlpha(a_actor, root, headpart, 1.0f, false);
					}
					result = RE::BSVisit::BSVisitControl::kStop;
				}
				break;
			case 1:
				{
					SetSkinAlpha(root, 1.0f, false);
					result = RE::BSVisit::BSVisitControl::kStop;
				}
				break;
			case 2:
				{
					for (auto& name : underwear) {
						ToggleNode(root, name, false, false);
					}
					SetSkinAlpha(root, 1.0f, false);
					result = RE::BSVisit::BSVisitControl::kStop;
				}
				break;
			default:
				break;
			}

			return result;
		});
	}


	void Detach::UpdateCollision(RE::NiAVObject* a_node, bool a_unk02, bool a_unk03)
	{
		_UpdateCollision(a_node, a_unk02, a_unk03);

		if (a_node && !a_node->AsFadeNode()) {
			if (auto user = a_node->GetUserData(); user) {
				const auto actor = user->As<RE::Actor>();
				if (actor && !actor->IsPlayerRef() && actor->HasKeyword(NPC)) {
					ResetActor(a_node, actor);
				}
			}
		}
	}
}


namespace RESET
{
	auto TESResetEventHandler::GetSingleton() -> TESResetEventHandler*
	{
		static TESResetEventHandler singleton;
		return &singleton;
	}


	auto TESResetEventHandler::ProcessEvent(const RE::TESResetEvent* evn, RE::BSTEventSource<RE::TESResetEvent>* /*a_eventSource*/) -> RE::BSEventNotifyControl
	{
		if (!evn) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto object = evn->object.get();
		if (!object) {
			return RE::BSEventNotifyControl::kContinue;
		}

		auto actor = object->As<RE::Actor>();
		if (!actor) {
			return RE::BSEventNotifyControl::kContinue;
		}

		if (IsInFECFaction(actor)) {
			static RE::TESFaction* resetFaction;
			if (!resetFaction) {
				if (auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
					for (auto& faction : dataHandler->GetFormArray<RE::TESFaction>()) {
						if (faction) {
							if (std::string name(faction->GetName()); name == effectResetFac) {
								resetFaction = faction;
								break;
							}
						}
					}
				}
			}
			if (resetFaction) {
				if (!actor->IsAIEnabled()) {
					actor->EnableAI(true);
				}
				actor->AddToFaction(resetFaction, 1);
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}
}