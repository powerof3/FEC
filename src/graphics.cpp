#include "graphics.h"

extern RE::TESFile* mod;
extern RE::SpellItem* deathEffectsAbility;

namespace util
{
	bool is_in_FEC_faction(RE::Actor* a_actor)
	{
		bool result = false;

		a_actor->VisitFactions([&result](RE::TESFaction* a_faction, std::int8_t a_rank) {
			if (a_faction && a_rank > -1) {
				const std::string name(a_faction->GetName());
				if (std::ranges::find(effectDoneFac, name) != effectDoneFac.end()) {
					result = true;
					return true;
				}
			}
			return false;
		});

		return result;
	}

	std::int32_t get_FEC_faction(RE::Actor* a_actor)
	{
		std::int32_t x = -1;

		a_actor->VisitFactions([&x](RE::TESFaction* a_faction, std::int8_t a_rank) {
			if (a_faction && a_rank > -1) {
				const std::string name(a_faction->GetName());
				for (std::uint32_t i = 0; i < effectFac.size(); i++) {
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

	bool has_base_spell(RE::Actor* a_actor, RE::SpellItem* a_spell)
	{
		bool hasSpell = false;

		const auto base = a_actor->GetActorBase();
		const auto spellList = base ? base->GetSpellList() : nullptr;

		if (spellList && spellList->spells && spellList->numSpells > 0) {
			std::span<RE::SpellItem*> span(spellList->spells, spellList->numSpells);
			for (const auto& spell : span) {
				if (spell == a_spell) {
					hasSpell = true;
					break;
				}
			}
		}

		return hasSpell;
	}

	std::optional<std::uint32_t> get_data(RE::NiAVObject* a_object)
	{
		if (!a_object->extra || a_object->extraDataSize == 0) {
			return std::nullopt;
		}

		std::optional<std::uint32_t> result = std::nullopt;

		std::span<RE::NiExtraData*> span(a_object->extra, a_object->extraDataSize);
		for (const auto& extraData : span) {
			if (result.has_value()) {
				break;
			}

			if (extraData && !extraData->name.empty()) {
				switch (string::const_hash(extraData->name)) {
				case string::const_hash("EXTRA_HEAD"sv):
					result = 0;
					break;
				case string::const_hash("EXTRA_BODY"sv):
					result = 1;
					break;
				case string::const_hash("EXTRA_CHARRED_BODY"sv):
					result = 2;
					break;
				default:
					break;
				}
			}
		}

		return result;
	}

	bool has_keyword(RE::Actor* a_actor, std::string_view a_keyword)
	{
		const auto actorbase = a_actor->GetActorBase();
		return actorbase && actorbase->HasKeyword(a_keyword);
	}
}

namespace GRAPHICS
{
	using namespace util;

	void SetSkinAlpha(RE::NiAVObject* a_root, float a_alpha, bool a_setData)
	{
		a_root->UpdateMaterialAlpha(a_alpha, true);

		if (a_setData) {
			if (a_alpha == 1.0f) {
				a_root->RemoveExtraData(EXTRA::SKIN_ALPHA);
			} else {
				add_data_if_none<RE::NiBooleanExtraData>(a_root, EXTRA::SKIN_ALPHA, true);
			}
		}
	}

	void SetSkinAlpha(RE::NiAVObject* a_root, RE::NiAVObject* a_node, float a_alpha, bool a_setData)
	{
		a_node->UpdateMaterialAlpha(a_alpha, true);

		if (a_setData) {
			if (a_alpha == 1.0f) {
				a_root->RemoveExtraData(EXTRA::SKIN_ALPHA);
			} else {
				add_data_if_none<RE::NiBooleanExtraData>(a_root, EXTRA::SKIN_ALPHA, true);
			}
		}
	}

	void ToggleNode(RE::NiAVObject* a_root, const RE::BSFixedString& a_nodeName, const bool a_toggle, bool a_setData)
	{
		if (const auto object = a_root->GetObjectByName(a_nodeName); object) {
			object->CullNode(a_toggle);

			if (a_setData) {
				if (const auto data = a_root->GetExtraData<RE::NiStringsExtraData>(EXTRA::TOGGLE); data) {
					a_toggle ?
                        data->Insert(a_nodeName) :
                        data->Remove(a_nodeName);
				} else if (a_toggle) {
					const std::vector<RE::BSFixedString> vec{ a_nodeName };
					if (const auto newData = RE::NiStringsExtraData::Create(EXTRA::TOGGLE, vec); newData) {
						a_root->AddExtraData(newData);
					}
				}
			}
		}
	}

	void ToggleNode(RE::NiAVObject* a_root, RE::NiAVObject* a_node, bool a_toggle, bool a_setData)
	{
		a_node->CullNode(a_toggle);

		if (a_setData) {
			if (const auto data = a_root->GetExtraData<RE::NiStringsExtraData>(EXTRA::TOGGLE); data) {
				a_toggle ?
                    data->Insert(a_node->name) :
                    data->Remove(a_node->name);
			} else if (a_toggle) {
				const std::vector<RE::BSFixedString> vec{ a_node->name };
				if (const auto newData = RE::NiStringsExtraData::Create(EXTRA::TOGGLE, vec); newData) {
					a_root->AddExtraData(newData);
				}
			}
		}
	}

	void SetHeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, HeadPart a_type, float a_alpha, bool a_setData)
	{
		if (const auto object = a_actor->GetHeadPartObject(a_type); object) {
			object->UpdateMaterialAlpha(a_alpha, false);

			if (a_setData) {
				const auto name = "PO3_HEADPART - " + std::to_string(stl::to_underlying(a_type));
				if (a_alpha == 1.0f) {
					a_root->RemoveExtraData(name);
				} else {
					util::add_data_if_none<RE::NiIntegerExtraData>(a_root, name, stl::to_underlying(a_type));
				}
			}
		}
	}
}

namespace ARMOR
{
	using ShaderType = RE::BSShaderMaterial::Feature;

	using namespace util;

	namespace Attach
	{
		struct ProcessGeometry
		{
			static void thunk(RE::BipedAnim* a_biped, RE::BSGeometry* a_object, RE::BSDismemberSkinInstance* a_dismemberInstance, std::int32_t a_slot, bool a_unk05)
			{
				func(a_biped, a_object, a_dismemberInstance, a_slot, a_unk05);

				if (a_biped && a_object) {
					const auto ref = a_biped->actorRef.get();
					const auto actor = ref ? ref->As<RE::Actor>() : nullptr;

					if (!actor || actor->IsPlayerRef() || !has_keyword(actor, NPC)) {
						return;
					}

					const auto facType = get_FEC_faction(actor);
					if (facType == -1) {
						return;
					}

					const auto root = actor->Get3D(false);
					if (!root) {
						return;
					}

					if (std::ranges::any_of(fxBiped, [&](const auto& bipedSlot) {
							const auto addon = a_biped->objects[bipedSlot].addon;
							return addon && mod->IsFormInMod(addon->formID);
						})) {
						if (!a_object->name.empty()) {
							switch (string::const_hash(a_object->name)) {
							case string::const_hash(UNDERWEAR::male0):
							case string::const_hash(UNDERWEAR::male1):
							case string::const_hash(UNDERWEAR::female):
							case string::const_hash(UNDERWEAR::bra0):
							case string::const_hash(UNDERWEAR::bra1):
							case string::const_hash(UNDERWEAR::bra2):
							case string::const_hash(UNDERWEAR::panty0):
							case string::const_hash(UNDERWEAR::panty1):
							case string::const_hash(UNDERWEAR::panty2):
								{
									if (facType <= 1) {
										GRAPHICS::ToggleNode(root, a_object, true);
									}
								}
								break;
							default:
								{
									if (a_object->HasShaderType(ShaderType::kFaceGenRGBTint)) {
										const auto alpha = facType == 3 ? 0.5f : 0.0f;
										GRAPHICS::SetSkinAlpha(root, a_object, alpha);
									}
								}
								break;
							}
						}
					}
				}
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct ProcessObject
		{
			static void thunk(RE::BipedAnim* a_biped, RE::NiAVObject* a_object, std::int32_t a_slot, bool a_unk04)
			{
				func(a_biped, a_object, a_slot, a_unk04);

				if (a_biped && a_object) {
					const auto ref = a_biped->actorRef.get();
					const auto actor = ref ? ref->As<RE::Actor>() : nullptr;

					if (!actor || actor->IsPlayerRef() || !has_keyword(actor, NPC)) {
						return;
					}

					const auto facType = get_FEC_faction(actor);
					if (facType == -1) {
						return;
					}

					const auto root = actor->Get3D(false);
					if (!root) {
						return;
					}

					if (std::ranges::any_of(fxBiped, [&](const auto& bipedSlot) {
							const auto addon = a_biped->objects[bipedSlot].addon;
							return addon && mod->IsFormInMod(addon->formID);
						})) {
						RE::BSVisit::TraverseScenegraphGeometries(a_object, [&](const RE::BSGeometry* a_geom) -> RE::BSVisit::BSVisitControl {
							if (!a_geom->name.empty()) {
								switch (string::const_hash(a_geom->name)) {
								case string::const_hash(UNDERWEAR::male0):
								case string::const_hash(UNDERWEAR::male1):
								case string::const_hash(UNDERWEAR::female):
								case string::const_hash(UNDERWEAR::bra0):
								case string::const_hash(UNDERWEAR::bra1):
								case string::const_hash(UNDERWEAR::bra2):
								case string::const_hash(UNDERWEAR::panty0):
								case string::const_hash(UNDERWEAR::panty1):
								case string::const_hash(UNDERWEAR::panty2):
									{
										if (facType <= 1) {
											GRAPHICS::ToggleNode(root, a_object, true);
										}
									}
									break;
								default:
									{
										if (a_object->HasShaderType(ShaderType::kFaceGenRGBTint)) {
											const auto alpha = facType == 3 ? 0.5f : 0.0f;
											GRAPHICS::SetSkinAlpha(root, a_object, alpha);
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
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct PerformNPCDismember
		{
			static void thunk(RE::TESNPC* a_npc, RE::Actor* a_actor, RE::NiAVObject* a_node)
			{
				func(a_npc, a_actor, a_node);

				if (a_actor && !a_actor->IsPlayerRef() && has_keyword(a_actor, NPC)) {
					const auto facType = get_FEC_faction(a_actor);
					if (facType == -1) {
						return;
					}

					const auto biped = a_actor->GetCurrentBiped().get();
					const auto root = a_actor->Get3D();
					if (root && biped) {
						const auto addon = biped->objects[Biped::kModMouth].addon;

						if (addon && mod->IsFormInMod(addon->formID)) {
							if (facType == 0 || facType == 1) {
								GRAPHICS::ToggleNode(root, RE::FixedStrings::GetSingleton()->bsFaceGenNiNodeSkinned, true);
							} else if (facType == 2) {
								for (auto& headpart : headparts) {
									GRAPHICS::SetHeadPartAlpha(a_actor, root, headpart, 0.0f);
								}
							} else {
								if (has_keyword(a_actor, IsBeast)) {
									GRAPHICS::SetHeadPartAlpha(a_actor, root, HeadPart::kMisc, 0.0f);
								}
							}
						}
					}
				}
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		void Install()
		{
			REL::Relocation<std::uintptr_t> processAttachedGeometry{ REL::ID(15535), 0x79A };  //armor
			stl::write_thunk_call<ProcessGeometry>(processAttachedGeometry.address());

			REL::Relocation<std::uintptr_t> attachArmorAddon{ REL::ID(15501), 0x1EA };  //armor 2
			stl::write_thunk_call<ProcessObject>(attachArmorAddon.address());

			REL::Relocation<std::uintptr_t> processArmorAttach{ REL::ID(24236), 0x33E };  // head
			stl::write_thunk_call<PerformNPCDismember>(processArmorAttach.address());

			logger::info("Hooked armor attach.");
		}
	}

	namespace Detach
	{
		struct UpdateCollision
		{
			static void thunk(RE::NiAVObject* a_node, bool a_unk02, bool a_unk03)
			{
				func(a_node, a_unk02, a_unk03);

				if (a_node && !a_node->AsFadeNode()) {
					const auto user = a_node->GetUserData();
					const auto actor = user ? user->As<RE::Actor>() : nullptr;

					if (actor && !actor->IsPlayerRef() && has_keyword(actor, NPC)) {
						const auto root = actor->Get3D(false);
						if (!root) {
							return;
						}

						RE::BSVisit::TraverseScenegraphGeometries(a_node, [&](RE::BSGeometry* a_geometry) -> RE::BSVisit::BSVisitControl {
							auto result = RE::BSVisit::BSVisitControl::kContinue;

							const auto val = get_data(a_geometry);
							if (!val.has_value()) {
								return result;
							}

							switch (val.value()) {
							case 0:
								{
									GRAPHICS::ToggleNode(root, RE::FixedStrings::GetSingleton()->bsFaceGenNiNodeSkinned, false, false);
									for (auto& headpart : headparts) {
										GRAPHICS::SetHeadPartAlpha(actor, root, headpart, 1.0f, false);
									}
									result = RE::BSVisit::BSVisitControl::kStop;
								}
								break;
							case 1:
								{
									GRAPHICS::SetSkinAlpha(root, 1.0f, false);
									result = RE::BSVisit::BSVisitControl::kStop;
								}
								break;
							case 2:
								{
									for (auto& name : UNDERWEAR::underwears) {
										GRAPHICS::ToggleNode(root, name, false, false);
									}
									GRAPHICS::SetSkinAlpha(root, 1.0f, false);
									result = RE::BSVisit::BSVisitControl::kStop;
								}
								break;
							default:
								break;
							}

							return result;
						});
					}
				}
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL::ID(15495), 0x1F };  //removeNodeFromScene
			stl::write_thunk_call<UpdateCollision>(target.address());

			logger::info("Hooked armor detach.");
		}
	}
}

namespace RESET
{
	TESResetEventHandler* TESResetEventHandler::GetSingleton()
	{
		static TESResetEventHandler singleton;
		return &singleton;
	}

	RE::BSEventNotifyControl TESResetEventHandler::ProcessEvent(const RE::TESResetEvent* evn, RE::BSTEventSource<RE::TESResetEvent>*)
	{
		if (!evn) {
			return RE::BSEventNotifyControl::kContinue;
		}

		const auto object = evn->object;
		if (!object) {
			return RE::BSEventNotifyControl::kContinue;
		}

		const auto actor = object->As<RE::Actor>();
		if (!actor) {
			return RE::BSEventNotifyControl::kContinue;
		}

		static RE::TESFaction* resetFaction;
		if (!resetFaction) {
			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				for (const auto& faction : dataHandler->GetFormArray<RE::TESFaction>()) {
					if (faction && mod->IsFormInMod(faction->GetFormID())) {
						if (const std::string name(faction->GetName()); name == effectResetFac) {
							resetFaction = faction;
							break;
						}
					}
				}
			}
		}

		if (resetFaction) {
			if (util::is_in_FEC_faction(actor)) {
				if (!actor->IsAIEnabled()) {
					actor->EnableAI(true);
				}
				actor->AddToFaction(resetFaction, 1);
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}
}
