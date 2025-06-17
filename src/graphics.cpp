#include "Graphics.h"
#include "Serialization.h"

namespace FEC::GRAPHICS
{
	namespace TEXTURE
	{
		void sanitize_path(std::string& a_path)
		{
			std::ranges::transform(a_path, a_path.begin(),
				[](char c) { return static_cast<char>(std::tolower(c)); });

			a_path = srell::regex_replace(a_path, srell::regex("/+|\\\\+"), "\\");
			a_path = srell::regex_replace(a_path, srell::regex("^\\\\+"), "");
			a_path = srell::regex_replace(a_path, srell::regex(R"(.*?[^\s]textures\\|^textures\\)", srell::regex::icase), "");
		}

		RE::BSShaderTextureSet* create_textureset(char** a_value)
		{
			const auto textureset = RE::BSShaderTextureSet::Create();
			if (textureset) {
				for (const auto type : stl::enum_range(Texture::kDiffuse, Texture::kTotal)) {
					if (!string::is_empty(a_value[type])) {
						textureset->SetTexturePath(type, a_value[type]);
					}
				}
			}
			return textureset;
		}
	}

	ShaderDataOutput::ShaderDataOutput(RE::NiStringsExtraData* a_data)
	{
		hasData = false;

		if (a_data && a_data->value && a_data->size == kShaderTotal) {
			if (const auto new_txst = TEXTURE::create_textureset(a_data->value); new_txst) {
				textureSet = new_txst;
				feature = string::to_num<Feature>(a_data->value[kFeature]);
				flags = string::to_num<std::uint64_t>(a_data->value[kFlag]);
				emissiveColor = RE::NiColor(string::to_num<std::uint32_t>(a_data->value[kColor]));
				emissiveMult = string::to_num<float>(a_data->value[kColorMult]);
				hasData = true;
			}
		}
	}

	bool ShaderDataOutput::Reset(RE::BSGeometry* a_geometry, RE::BSLightingShaderProperty* a_shaderProp, MaterialBase* a_material) const
	{
		if (!hasData) {
			return false;
		}

		if (const auto newMaterial = MaterialBase::CreateMaterial(feature.get()); newMaterial) {
			newMaterial->CopyBaseMembers(a_material);
			newMaterial->ClearTextures();
			newMaterial->OnLoadTextureSet(0, textureSet);

			a_shaderProp->flags = static_cast<RE::BSShaderProperty::EShaderPropertyFlag>(flags);
			a_shaderProp->lastRenderPassState = std::numeric_limits<std::int32_t>::max();
			if (a_shaderProp->flags.all(ShaderFlags::kOwnEmit)) {
				if (!a_shaderProp->emissiveColor) {
					a_shaderProp->emissiveColor = new RE::NiColor();
				}
				*a_shaderProp->emissiveColor = emissiveColor;
			}
			a_shaderProp->emissiveMult = emissiveMult;

			a_shaderProp->SetMaterial(newMaterial, true);
			a_shaderProp->SetupGeometry(a_geometry);
			a_shaderProp->FinishSetupGeometry(a_geometry);

			newMaterial->~BSLightingShaderMaterialBase();
			RE::free(newMaterial);
		}

		return true;
	}

	void ActorApplier::ToggleNode(RE::NiAVObject* a_root, RE::NiAVObject* a_node, bool a_cull, bool a_setData)
	{
		a_node->CullNode(a_cull);

		if (!a_setData) {
			return;
		}

		if (const auto data = a_root->GetExtraData<RE::NiStringsExtraData>(EXTRA::TOGGLE); data) {
			a_cull ?
				data->Insert(a_node->name) :
				data->Remove(a_node->name);
		} else if (a_cull) {
			std::vector<RE::BSFixedString> vec{ a_node->name };
			if (const auto newData = RE::NiStringsExtraData::Create(EXTRA::TOGGLE, vec); newData) {
				a_root->AddExtraData(newData);
			}
		}
	}

	void ActorApplier::ToggleNode(RE::NiAVObject* a_root, const RE::BSFixedString& a_nodeName, bool a_cull, bool a_setData)
	{
		ToggleNode(a_root, a_root->GetObjectByName(a_nodeName), a_cull, a_setData);
	}

	void ActorApplier::HeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, HeadPartType a_type, float a_alpha, bool a_setData)
	{
		if (const auto object = a_actor->GetHeadPartObject(a_type); object) {
			object->UpdateMaterialAlpha(a_alpha, false);

			if (!a_setData) {
				return;
			}

			const auto name = "PO3_HEADPART - " + std::to_string(std::to_underlying(a_type));
			if (a_alpha == 1.0f) {
				a_root->RemoveExtraData(name);
			} else {
				EXTRA::Add<RE::NiIntegerExtraData>(a_root, name, std::to_underlying(a_type));
			}
		}
	}

	void ActorApplier::SkinAlpha(RE::NiAVObject* a_root, float a_alpha, bool a_setData)
	{
		SkinAlpha(a_root, a_root, a_alpha, a_setData);
	}

	void ActorApplier::SkinAlpha(RE::NiAVObject* a_root, RE::NiAVObject* a_node, float a_alpha, bool a_setData)
	{
		a_node->UpdateMaterialAlpha(a_alpha, true);

		if (!a_setData) {
			return;
		}

		if (a_alpha == 1.0f) {
			a_root->RemoveExtraData(EXTRA::SKIN_ALPHA);
		} else {
			EXTRA::Add<RE::NiBooleanExtraData>(a_root, EXTRA::SKIN_ALPHA, true);
		}
	}

	ActorResetter::ActorResetter(RE::Actor* a_actor, RE::NiAVObject* a_object, const RE::BSFixedString& a_folderName) :
		actor(a_actor),
		root(a_object),
		folderName(a_folderName)
	{
		if (!a_object || !a_object->extra || a_object->extraDataSize == 0) {
			hasData = false;
			return;
		}

		std::span<RE::NiExtraData*> span(a_object->extra, a_object->extraDataSize);
		for (const auto& extraData : span) {
			if (!extraData) {
				continue;
			}
			if (const auto name = extraData->GetName(); !name.empty()) {
				switch (string::const_hash(name)) {
				case string::const_hash(EXTRA::TOGGLE):
					{
						toggle = static_cast<RE::NiStringsExtraData*>(extraData);
						hasData = true;
					}
					break;
				case string::const_hash(EXTRA::SKIN_TINT):
					{
						tintSkin = static_cast<RE::NiIntegerExtraData*>(extraData);
						hasData = true;
					}
					break;
				case string::const_hash(EXTRA::HAIR_TINT):
					{
						tintHair = static_cast<RE::NiIntegerExtraData*>(extraData);
						hasData = true;
					}
					break;
				case string::const_hash(EXTRA::SKIN_ALPHA):
					{
						alphaSkin = static_cast<RE::NiBooleanExtraData*>(extraData);
						hasData = true;
					}
					break;
				case string::const_hash(EXTRA::FACE_TXST):
					{
						txstFace = static_cast<RE::NiStringsExtraData*>(extraData);
						hasData = true;
					}
					break;
				default:
					if (string::icontains(name, EXTRA::HEADPART)) {
						alphaHDPT.emplace_back(static_cast<RE::NiIntegerExtraData*>(extraData));
						hasData = true;

					} else if (string::icontains(name, EXTRA::TXST)) {
						txst.emplace_back(static_cast<RE::NiStringsExtraData*>(extraData));
						hasData = true;

					} else if (string::icontains(name, EXTRA::SKIN_TXST)) {
						txstSkin.emplace_back(static_cast<RE::NiStringsExtraData*>(extraData));
						hasData = true;

					} else if (string::icontains(name, EXTRA::SHADER)) {
						shaders.emplace_back(static_cast<RE::NiStringsExtraData*>(extraData));
						hasData = true;
					}
					break;
				}
			}
		}

		if (!folderName.empty()) {
			TEXTURE::sanitize_path(folderName);
		}
	}

	bool ActorResetter::ResetEffectsNotOfType(EFFECT::TYPE a_effectType) const
	{
		if (!actor || !root || !hasData) {
			return false;
		}

		switch (a_effectType) {
		case EFFECT::TYPE::kCharred:
			{
				ResetHeadPartAlpha();
				ResetSkinTint();
				ResetHairTint();
				ResetFaceTXST();
				ResetSkinTXST();
			}
			break;
		case EFFECT::TYPE::kDrained:
			{
				ResetToggle();
				ResetSkinTint();
				ResetHairTint();
				ResetFaceTXST();
				ResetSkinTXST();
			}
			break;
		case EFFECT::TYPE::kPoisoned:
			{
				ResetToggle();
				ResetSkinAlpha();
				ResetHeadPartAlpha();
				ResetFaceTXST();
				ResetSkinTXST();

				if (!actor->IsPlayerRef()) {
					stop_all_skin_shaders(actor.get());
				}
			}
			break;
		case EFFECT::TYPE::kAged:
			{
				ResetToggle();
				ResetSkinAlpha();
				ResetHeadPartAlpha();

				if (!actor->IsPlayerRef()) {
					stop_all_skin_shaders(actor.get());
				}
			}
			break;
		case EFFECT::TYPE::kCharredCreature:
			{
				ResetMaterialShader();
			}
			break;
		default:
			break;
		}

		return true;
	}

	void ActorResetter::ResetToggle() const
	{
		if (toggle && toggle->value && toggle->size > 0) {
			std::span<char*> span(toggle->value, toggle->size);
			for (const auto& string : span) {
				if (!string::is_empty(string)) {
					if (const auto object = root->GetObjectByName(string); object) {
						object->CullNode(false);
					}
				}
			}
			root->RemoveExtraData(toggle->GetName());
		}
	}

	void ActorResetter::ResetSkinTint() const
	{
		if (tintSkin) {
			const auto actorbase = actor->GetActorBase();
			const auto facePart = actorbase ? actorbase->GetCurrentHeadPartByType(HeadPartType::kFace) : nullptr;
			const auto faceNode = actor->GetFaceNodeSkinned();

			if (faceNode && facePart) {
				if (const auto faceGen = RE::BSFaceGenManager::GetSingleton(); faceGen) {
					faceGen->PrepareHeadPartForShaders(faceNode, facePart, actorbase);
				}
				root->UpdateBodyTint(actorbase->bodyTintColor);
				root->RemoveExtraData(tintSkin->GetName());
			}
		}
	}

	void ActorResetter::ResetHairTint() const
	{
		if (tintHair) {
			const auto actorBase = actor->GetActorBase();
			const auto headData = actorBase ? actorBase->headRelatedData : nullptr;
			const auto colorForm = headData ? headData->hairColor : nullptr;

			if (colorForm) {
				root->UpdateHairColor(colorForm->color);

				if (const auto& biped = actor->GetCurrentBiped(); biped) {
					for (auto& slot : SLOT::headSlots) {
						const auto& node = biped->objects[slot].partClone;
						if (node && node->HasShaderType(RE::BSShaderMaterial::Feature::kHairTint)) {
							node->UpdateHairColor(colorForm->color);
						}
					}
				}

				root->RemoveExtraData(tintHair->GetName());
			}
		}
	}

	void ActorResetter::ResetSkinAlpha() const
	{
		if (alphaSkin) {
			root->UpdateMaterialAlpha(1.0f, true);
			root->RemoveExtraData(alphaSkin->GetName());
		}
	}

	void ActorResetter::ResetHeadPartAlpha() const
	{
		if (alphaHDPT.empty()) {
			return;
		}

		for (auto& data : alphaHDPT) {
			if (data) {
				if (const auto object = actor->GetHeadPartObject(static_cast<HeadPartType>(data->value)); object) {
					object->UpdateMaterialAlpha(1.0f, false);
					root->RemoveExtraData(data->GetName());
				}
			}
		}
	}

	void ActorResetter::ResetFaceTXST() const
	{
		if (txstFace && txstFace->value) {
			const auto textureset = TEXTURE::create_textureset(txstFace->value);
			const auto faceObject = actor->GetHeadPartObject(HeadPartType::kFace);
			if (textureset && faceObject) {
				reset_textureset(faceObject, textureset, true);
				root->RemoveExtraData(txstFace->GetName());
			}
		}
	}

	void ActorResetter::ResetArmorTXST() const
	{
		if (txst.empty() || folderName.empty()) {
			return;
		}

		for (auto& data : txst) {
			if (data && data->value && data->size > 0) {
				RE::FormID formID = 0;
				if (std::string armorID{ data->value[data->size - 1] }; !armorID.empty()) {
					try {
						formID = string::to_num<RE::FormID>(armorID, true);
					} catch (...) {
						continue;
					}
				}
				auto armor = actor->GetWornArmor(formID);
				if (!armor) {
					armor = actor->GetSkin();
				}
				if (armor) {
					const auto textureset = TEXTURE::create_textureset(data->value);
					const auto arma = armor->GetArmorAddon(actor->GetRace());
					if (textureset && arma) {
						actor->VisitArmorAddon(armor, arma, [&](bool, RE::NiAVObject& a_obj) -> bool {
							reset_textureset(&a_obj, textureset, false, folderName);
							return true;
						});
						root->RemoveExtraData(data->GetName());
					}
				}
			}
		}
	}

	void ActorResetter::ResetSkinTXST() const
	{
		if (txstSkin.empty()) {
			return;
		}

		for (auto& data : txstSkin) {
			if (data && data->value && data->size > 0) {
				auto slot = Slot::kNone;
				if (std::string slotMaskstr{ data->value[data->size - 1] }; !slotMaskstr.empty()) {
					try {
						slot = string::to_num<Slot>(slotMaskstr);
					} catch (...) {
						continue;
					}
				}
				if (const auto skinArmor = actor->GetSkin(slot); skinArmor) {
					const auto textureset = TEXTURE::create_textureset(data->value);
					const auto skinArma = skinArmor->GetArmorAddonByMask(actor->GetRace(), slot);
					if (textureset && skinArma) {
						actor->VisitArmorAddon(skinArmor, skinArma, [&](bool, RE::NiAVObject& a_obj) -> bool {
							reset_textureset(&a_obj, textureset, true);
							return true;
						});
						root->RemoveExtraData(data->GetName());
					}
				}
			}
		}
	}

	void ActorResetter::ResetMaterialShader() const
	{
		if (shaders.empty()) {
			return;
		}

		for (auto& data : shaders) {
			if (data && data->value && data->size > 0) {
				std::vector<RE::BSFixedString> vec({ data->value, data->value + data->size });
				reset_shaderdata(root.get(), vec);
				root->RemoveExtraData(data->GetName());
			}
		}
	}

	void ActorResetter::stop_all_skin_shaders(RE::TESObjectREFR* a_ref)
	{
		using Flags = RE::EffectShaderData::Flags;

		if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
			const auto handle = a_ref->CreateRefHandle();
			processLists->ForEachShaderEffect([&](RE::ShaderReferenceEffect* a_shaderEffect) {
				if (a_shaderEffect->target == handle) {
					if (const auto effectData = a_shaderEffect->effectData; effectData &&
																			effectData->data.flags.all(Flags::kSkinOnly) &&
																			!effectData->holesTexture.textureName.empty()) {
						a_shaderEffect->finished = true;
					}
				}
				return RE::BSContainer::ForEachResult::kContinue;
			});
		}
	}

	void ActorResetter::reset_textureset(RE::NiAVObject* a_object, RE::BSShaderTextureSet* a_txst, bool a_doOnlySkin, const std::string& a_folder)
	{
		RE::BSVisit::TraverseScenegraphGeometries(a_object, [&](RE::BSGeometry* a_geometry) -> RE::BSVisit::BSVisitControl {
			const auto& effect = a_geometry->properties[States::kEffect];
			const auto  lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect.get());
			if (lightingShader) {
				const auto material = static_cast<MaterialBase*>(lightingShader->material);
				if (material) {
					if (!a_doOnlySkin) {
						if (const auto textureSet = material->textureSet; textureSet && !a_folder.empty()) {
							std::string sourcePath{ textureSet->GetTexturePath(Texture::kDiffuse) };
							if (TEXTURE::sanitize_path(sourcePath); !sourcePath.contains(a_folder)) {
								return RE::BSVisit::BSVisitControl::kContinue;
							}
						}
					} else {
						if (const auto feature = material->GetFeature(); feature != Feature::kFaceGenRGBTint && feature != Feature::kFaceGen) {
							return RE::BSVisit::BSVisitControl::kContinue;
						}
					}

					if (const auto newMaterial = static_cast<MaterialBase*>(material->Create()); newMaterial) {
						newMaterial->CopyMembers(material);

						newMaterial->ClearTextures();
						newMaterial->OnLoadTextureSet(0, a_txst);

						lightingShader->SetMaterial(newMaterial, true);

						lightingShader->SetupGeometry(a_geometry);
						lightingShader->FinishSetupGeometry(a_geometry);

						newMaterial->~BSLightingShaderMaterialBase();
						RE::free(newMaterial);
					}
				}
			}
			return RE::BSVisit::BSVisitControl::kContinue;
		});
	}

	void ActorResetter::reset_shaderdata(RE::NiAVObject* a_object, std::vector<RE::BSFixedString>& a_geometries)
	{
		RE::BSVisit::TraverseScenegraphGeometries(a_object, [&](RE::BSGeometry* a_geometry) -> RE::BSVisit::BSVisitControl {
			if (std::ranges::find(a_geometries, a_geometry->name) == a_geometries.end()) {
				return RE::BSVisit::BSVisitControl::kContinue;
			}

			const auto& effect = a_geometry->properties[States::kEffect];
			const auto  lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect.get());

			if (lightingShader) {
				const auto originalData = lightingShader->GetExtraData<RE::NiStringsExtraData>(EXTRA::ORIG_SHADER);
				if (!originalData) {
					return RE::BSVisit::BSVisitControl::kContinue;
				}

				if (const auto material = static_cast<MaterialBase*>(lightingShader->material)) {
					auto shaderData = ShaderDataOutput(originalData);
					if (!shaderData.Reset(a_geometry, lightingShader, material)) {
						logger::warn("unable to get original shader values for {}", a_geometry->name.c_str());
					}
					lightingShader->RemoveExtraData(originalData->GetName());
					a_geometry->RemoveExtraData(originalData->GetName());
				}
			}

			return RE::BSVisit::BSVisitControl::kContinue;
		});
	}

	namespace ARMOR
	{
		using namespace Serialization;

		using PermEffect = ActorEffect::Permanent;
		using TempEffect = ActorEffect::Temporary;

		struct detail
		{
			static bool has_FEC_addon(RE::BipedAnim* a_biped)
			{
				return std::ranges::any_of(SLOT::fxBiped, [&](const auto& bipedSlot) {
					const auto addon = a_biped->objects[bipedSlot].addon;
					return addon && mod->IsFormInMod(addon->formID);
				});
			}

			static void attach(RE::BipedAnim* a_biped, RE::NiAVObject* a_object)
			{
				if (a_biped && a_object) {
					const auto ref = a_biped->actorRef.get();
					const auto actor = ref ? ref->As<RE::Actor>() : nullptr;

					if (!actor || actor->IsPlayerRef() || !actor->HasKeywordString(KEYWORD::NPC)) {
						return;
					}

					const auto root = actor->Get3D(false);
					if (!root) {
						return;
					}

					if (has_FEC_addon(a_biped)) {
						const auto permType = Manager::GetSingleton()->permanentEffectMap.find(actor->GetFormID());
						const auto tempType = Manager::GetSingleton()->temporaryEffectMap.find(actor->GetFormID());

						if (permType && *permType != PermEffect::kNone) {
							if (*permType == PermEffect::kCharred || *permType == PermEffect::kSkeletonized) {
								if (const auto& name = a_object->name; !name.empty() && std::ranges::find(UNDERWEAR::underwears, name.c_str()) != UNDERWEAR::underwears.end()) {
									ActorApplier::ToggleNode(root, a_object, true);
								}
							}
							if (a_object->HasShaderType(Feature::kFaceGenRGBTint)) {
								ActorApplier::SkinAlpha(root, a_object, 0.0f);
							}
						} else if (tempType && !tempType->empty()) {
							if (const auto name = a_object->name; !name.empty()) {
								if (a_object->HasShaderType(Feature::kFaceGenRGBTint) && tempType->contains(TempEffect::kXRayed)) {
									ActorApplier::SkinAlpha(root, a_object, 0.5f);
								}
							}
						}
					}
				}
			}

			static std::optional<std::uint32_t> get_data(const RE::NiAVObject* a_object)
			{
				if (!a_object->extra || a_object->extraDataSize == 0) {
					return std::nullopt;
				}

				std::optional<std::uint32_t> result{ std::nullopt };

				std::span<RE::NiExtraData*> span(a_object->extra, a_object->extraDataSize);
				for (const auto& extraData : span) {
					if (result) {
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
		};

		namespace ATTACH
		{
			struct ProcessGeometry
			{
				static void thunk(RE::BipedAnim* a_biped, RE::BSGeometry* a_object, RE::BSDismemberSkinInstance* a_dismemberInstance, std::int32_t a_slot, bool a_unk05)
				{
					func(a_biped, a_object, a_dismemberInstance, a_slot, a_unk05);

					detail::attach(a_biped, a_object);
				}
				static inline REL::Relocation<decltype(thunk)> func;
			};

			struct HideShowBufferedSkin
			{
				static void thunk(RE::BipedAnim* a_biped, RE::NiAVObject* a_object, RE::BIPED_OBJECT a_slot, bool a_unk04)
				{
					func(a_biped, a_object, a_slot, a_unk04);

					detail::attach(a_biped, a_object);
				}
				static inline REL::Relocation<decltype(thunk)> func;
			};

			struct UpdateHeadAndHair
			{
				static void thunk(RE::TESNPC* a_npc, RE::Actor* a_actor, RE::NiAVObject* a_node)
				{
					func(a_npc, a_actor, a_node);

					if (a_actor && !a_actor->IsPlayerRef() && a_actor->HasKeywordString(KEYWORD::NPC)) {
						const auto permType = Manager::GetSingleton()->permanentEffectMap.find(a_actor->GetFormID());
						if (!permType || *permType == PermEffect::kNone) {
							return;
						}

						const auto& biped = a_actor->GetCurrentBiped();
						if (const auto root = a_actor->Get3D(); root && biped) {
							const auto addon = biped->objects[Biped::kModMouth].addon;

							if (addon && mod->IsFormInMod(addon->formID)) {
								if (*permType == PermEffect::kCharred || *permType == PermEffect::kSkeletonized) {
									ActorApplier::ToggleNode(root, RE::FixedStrings::GetSingleton()->bsFaceGenNiNodeSkinned, true);
								} else if (*permType == PermEffect::kDrained) {
									for (auto& headpart : SLOT::headparts) {
										ActorApplier::HeadPartAlpha(a_actor, root, headpart, 0.0f);
									}
								} else {
									if (a_actor->HasKeywordString("IsBeastRace"sv)) {
										ActorApplier::HeadPartAlpha(a_actor, root, HeadPartType::kMisc, 0.0f);
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
				REL::Relocation<std::uintptr_t> processAttachedGeometry{ RELOCATION_ID(15535, 15712), OFFSET_3(0x79A, 0x72F, 0x7D4) };  //armor
				stl::write_thunk_call<ProcessGeometry>(processAttachedGeometry.address());

				REL::Relocation<std::uintptr_t> loadBipedParts{ RELOCATION_ID(15501, 15678), OFFSET_3(0x1EA, 0x1EA, 0x1E7) };  //armor 2
				stl::write_thunk_call<HideShowBufferedSkin>(loadBipedParts.address());

				REL::Relocation<std::uintptr_t> replaceRefModel{ RELOCATION_ID(24236, 24740), OFFSET(0x33E, 0x562) };  // head
				stl::write_thunk_call<UpdateHeadAndHair>(replaceRefModel.address());

				logger::info("Hooked armor attach.");
			}
		}

		namespace DETACH
		{
			struct UpdateCollision
			{
				static void thunk(RE::NiAVObject* a_node, bool a_unk02, bool a_unk03)
				{
					func(a_node, a_unk02, a_unk03);

					if (a_node && !a_node->AsFadeNode()) {
						const auto user = a_node->GetUserData();
						const auto actor = user ? user->As<RE::Actor>() : nullptr;

						if (actor && !actor->IsPlayerRef() && actor->HasKeywordString(KEYWORD::NPC)) {
							const auto root = actor->Get3D(false);
							if (!root) {
								return;
							}

							RE::BSVisit::TraverseScenegraphGeometries(a_node, [&](RE::BSGeometry* a_geometry) -> RE::BSVisit::BSVisitControl {
								auto result = RE::BSVisit::BSVisitControl::kContinue;

								const auto val = detail::get_data(a_geometry);
								if (!val) {
									return result;
								}

								switch (*val) {
								case 0:
									{
										ActorApplier::ToggleNode(root, RE::FixedStrings::GetSingleton()->bsFaceGenNiNodeSkinned, false, false);
										for (auto& headpart : SLOT::headparts) {
											ActorApplier::HeadPartAlpha(actor, root, headpart, 1.0f, false);
										}
										result = RE::BSVisit::BSVisitControl::kStop;
									}
									break;
								case 1:
									{
										ActorApplier::SkinAlpha(root, 1.0f, false);
										result = RE::BSVisit::BSVisitControl::kStop;
									}
									break;
								case 2:
									{
										for (auto& name : UNDERWEAR::underwears) {
											ActorApplier::ToggleNode(root, name, false, false);
										}
										ActorApplier::SkinAlpha(root, 1.0f, false);
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
				REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(15495, 15660), 0x1F };  //removeNodeFromScene
				stl::write_thunk_call<UpdateCollision>(target.address());

				logger::info("Hooked armor detach.");
			}
		}
	}

	void Install()
	{
		ARMOR::ATTACH::Install();
		ARMOR::DETACH::Install();
	}
}
