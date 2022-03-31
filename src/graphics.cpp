#include "Graphics.h"
#include "Serialization.h"

extern RE::TESFile* mod;
extern RE::SpellItem* deathEffectsAbility;

namespace FEC::GRAPHICS
{
	namespace TEXTURE
	{
		void sanitize_path(std::string& a_path)
		{
			std::ranges::transform(a_path, a_path.begin(),
				[](char c) { return static_cast<char>(std::tolower(c)); });

			a_path = std::regex_replace(a_path, std::regex("/+|\\\\+"), "\\");
			a_path = std::regex_replace(a_path, std::regex("^\\\\+"), "");
			a_path = std::regex_replace(a_path, std::regex(R"(.*?[^\s]textures\\|^textures\\)", std::regex_constants::icase), "");
		}

		RE::BSShaderTextureSet* create_textureset(char** a_value)
		{
			if (const auto textureset = RE::BSShaderTextureSet::Create(); textureset) {
				for (const auto& type : texture::types) {
					if (!string::is_empty(a_value[type])) {
						textureset->SetTexturePath(type, a_value[type]);
					}
				}
				return textureset;
			}
			return nullptr;
		}
	}

	namespace SET
	{
		void SkinAlpha(RE::NiAVObject* a_root, float a_alpha, bool a_setData)
		{
			a_root->UpdateMaterialAlpha(a_alpha, true);

			if (a_setData) {
				if (a_alpha == 1.0f) {
					a_root->RemoveExtraData(extra::SKIN_ALPHA);
				} else {
					EXTRA::add_data_if_none<RE::NiBooleanExtraData>(a_root, extra::SKIN_ALPHA, true);
				}
			}
		}

		void SkinAlpha(RE::NiAVObject* a_root, RE::NiAVObject* a_node, float a_alpha, bool a_setData)
		{
			a_node->UpdateMaterialAlpha(a_alpha, true);

			if (a_setData) {
				if (a_alpha == 1.0f) {
					a_root->RemoveExtraData(extra::SKIN_ALPHA);
				} else {
					EXTRA::add_data_if_none<RE::NiBooleanExtraData>(a_root, extra::SKIN_ALPHA, true);
				}
			}
		}

		void Toggle(RE::NiAVObject* a_root, const RE::BSFixedString& a_nodeName, const bool a_toggle, bool a_setData)
		{
			if (const auto object = a_root->GetObjectByName(a_nodeName); object) {
				object->CullNode(a_toggle);

				if (a_setData) {
					if (const auto data = a_root->GetExtraData<RE::NiStringsExtraData>(extra::TOGGLE); data) {
						a_toggle ?
							data->Insert(a_nodeName) :
                            data->Remove(a_nodeName);
					} else if (a_toggle) {
						const std::vector<RE::BSFixedString> vec{ a_nodeName };
						if (const auto newData = RE::NiStringsExtraData::Create(extra::TOGGLE, vec); newData) {
							a_root->AddExtraData(newData);
						}
					}
				}
			}
		}

		void Toggle(RE::NiAVObject* a_root, RE::NiAVObject* a_node, bool a_toggle, bool a_setData)
		{
			a_node->CullNode(a_toggle);

			if (a_setData) {
				if (const auto data = a_root->GetExtraData<RE::NiStringsExtraData>(extra::TOGGLE); data) {
					a_toggle ?
						data->Insert(a_node->name) :
                        data->Remove(a_node->name);
				} else if (a_toggle) {
					const std::vector<RE::BSFixedString> vec{ a_node->name };
					if (const auto newData = RE::NiStringsExtraData::Create(extra::TOGGLE, vec); newData) {
						a_root->AddExtraData(newData);
					}
				}
			}
		}

		void HeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, HeadPart a_type, float a_alpha, bool a_setData)
		{
			if (const auto object = a_actor->GetHeadPartObject(a_type); object) {
				object->UpdateMaterialAlpha(a_alpha, false);

				if (a_setData) {
					const auto name = "PO3_HEADPART - " + std::to_string(stl::to_underlying(a_type));
					if (a_alpha == 1.0f) {
						a_root->RemoveExtraData(name);
					} else {
						EXTRA::add_data_if_none<RE::NiIntegerExtraData>(a_root, name, stl::to_underlying(a_type));
					}
				}
			}
		}
	}

	namespace RESET
	{
		struct detail
		{
			static std::pair<bool, ShaderData> get_original_shaders(RE::NiStringsExtraData* a_data)
			{
				ShaderData shaderData;
				bool result = true;

				if (a_data && a_data->value && a_data->size > 0) {
					auto& [textureSet, feature, flags, emissiveColor, emissiveMult] = shaderData;
					if (const auto new_txst = TEXTURE::create_textureset(a_data->value); new_txst) {
						try {
							textureSet = new_txst;
							feature = string::lexical_cast<Feature>(a_data->value[9]);
							flags = string::lexical_cast<std::uint64_t>(a_data->value[10]);
							emissiveColor = RE::NiColor(
								string::lexical_cast<std::uint32_t>(a_data->value[11]));
							emissiveMult = string::lexical_cast<float>(a_data->value[12]);
						} catch (...) {
							result = false;
						}
					} else {
						result = false;
					}
				}

				return { result, shaderData };
			}

			static void reset_shaderdata(RE::NiAVObject* a_object, std::vector<RE::BSFixedString>& a_geometries)
			{
				using Flag = RE::BSShaderProperty::EShaderPropertyFlag;

				RE::BSVisit::TraverseScenegraphGeometries(a_object, [&](RE::BSGeometry* a_geometry) -> RE::BSVisit::BSVisitControl {
					if (std::ranges::find(a_geometries, a_geometry->name) == a_geometries.end()) {
						return RE::BSVisit::BSVisitControl::kContinue;
					}

					const auto effect = a_geometry->properties[States::kEffect];
					const auto lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect.get());
					if (lightingShader) {
						const auto originalData = lightingShader->GetExtraData<RE::NiStringsExtraData>(extra::ORIG_SHADER);
						if (!originalData) {
							return RE::BSVisit::BSVisitControl::kContinue;
						}

						const auto material = static_cast<RE::BSLightingShaderMaterialBase*>(lightingShader->material);
						if (material) {
							auto [result, shaderData] = get_original_shaders(originalData);
							if (!result) {
								logger::warn("unable to get original shader values for {}", a_geometry->name);
								return RE::BSVisit::BSVisitControl::kContinue;
							}

							auto& [textureSet, feature, flags, emissiveColor, emissiveMult] = shaderData;

							if (const auto newMaterial = RE::BSLightingShaderMaterialBase::CreateMaterial(feature); newMaterial) {
								newMaterial->CopyBaseMembers(material);
								newMaterial->ClearTextures();
								newMaterial->OnLoadTextureSet(0, textureSet);

								lightingShader->flags = static_cast<RE::BSShaderProperty::EShaderPropertyFlag>(flags);
								lightingShader->lastRenderPassState = std::numeric_limits<std::int32_t>::max();
								if (lightingShader->flags.all(Flag::kOwnEmit)) {
									if (!lightingShader->emissiveColor) {
										lightingShader->emissiveColor = new RE::NiColor();
									}
									lightingShader->emissiveColor->red = emissiveColor.red;
									lightingShader->emissiveColor->green = emissiveColor.green;
									lightingShader->emissiveColor->blue = emissiveColor.blue;
								}
								lightingShader->emissiveMult = emissiveMult;

								lightingShader->SetMaterial(newMaterial, true);
								lightingShader->SetupGeometry(a_geometry);
								lightingShader->FinishSetupGeometry(a_geometry);

								newMaterial->~BSLightingShaderMaterialBase();
								RE::free(newMaterial);

								lightingShader->RemoveExtraData(originalData->GetName());
								a_geometry->RemoveExtraData(originalData->GetName());
							}
						}
					}

					return RE::BSVisit::BSVisitControl::kContinue;
				});
			}

			static void reset_textureset(RE::NiAVObject* a_object, RE::BSShaderTextureSet* a_txst, bool a_doOnlySkin, const std::string& a_folder)
			{
				using Texture = RE::BSTextureSet::Texture;

				RE::BSVisit::TraverseScenegraphGeometries(a_object, [&](RE::BSGeometry* a_geometry) -> RE::BSVisit::BSVisitControl {
					const auto effect = a_geometry->properties[States::kEffect];
					const auto lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect.get());
					if (lightingShader) {
						const auto material = static_cast<RE::BSLightingShaderMaterialBase*>(lightingShader->material);
						if (material) {
							if (!a_doOnlySkin) {
								if (const auto textureSet = material->textureSet; textureSet && !a_folder.empty()) {
									std::string sourcePath{ textureSet->GetTexturePath(Texture::kDiffuse) };
									if (TEXTURE::sanitize_path(sourcePath); sourcePath.find(a_folder) == std::string::npos) {
										return RE::BSVisit::BSVisitControl::kContinue;
									}
								}
							} else {
								if (const auto feature = material->GetFeature(); feature != Feature::kFaceGenRGBTint && feature != Feature::kFaceGen) {
									return RE::BSVisit::BSVisitControl::kContinue;
								}
							}

							if (const auto newMaterial = static_cast<RE::BSLightingShaderMaterialBase*>(material->Create()); newMaterial) {
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
		};

		void stop_all_skin_shaders(RE::TESObjectREFR* a_ref)
		{
			using Flags = RE::EffectShaderData::Flags;

			if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
				const auto handle = a_ref->CreateRefHandle();
				processLists->GetShaderEffects([&](RE::ShaderReferenceEffect& a_shaderEffect) {
					if (a_shaderEffect.target == handle) {
						if (const auto effectData = a_shaderEffect.effectData; effectData &&
																			   effectData->data.flags.all(Flags::kSkinOnly) &&
																			   !effectData->holesTexture.textureName.empty()) {
							a_shaderEffect.finished = true;
						}
					}
					return true;
				});
			}
		}

		std::pair<bool, ResetData> get_data(RE::NiAVObject* a_object)
		{
			ResetData resetData;
			bool success = false;

			if (!a_object->extra || a_object->extraDataSize == 0) {
				return { success, resetData };
			}

			auto& [toggle, tintSkin, tintHair, alphaSkin, TXSTFace, alphaHDPT, TXST, TXSTSkin, shader] = resetData;

			const std::span span(a_object->extra, a_object->extraDataSize);
			for (const auto& extraData : span) {
				if (!extraData) {
					continue;
				}
				if (const auto name = extraData->GetName(); !name.empty()) {
					switch (string::const_hash(name)) {
					case string::const_hash(extra::TOGGLE):
						{
							toggle = static_cast<RE::NiStringsExtraData*>(extraData);
							success = true;
						}
						break;
					case string::const_hash(extra::SKIN_TINT):
						{
							tintSkin = static_cast<RE::NiIntegerExtraData*>(extraData);
							success = true;
						}
						break;
					case string::const_hash(extra::HAIR_TINT):
						{
							tintHair = static_cast<RE::NiIntegerExtraData*>(extraData);
							success = true;
						}
						break;
					case string::const_hash(extra::SKIN_ALPHA):
						{
							alphaSkin = static_cast<RE::NiBooleanExtraData*>(extraData);
							success = true;
						}
						break;
					case string::const_hash(extra::FACE_TXST):
						{
							TXSTFace = static_cast<RE::NiStringsExtraData*>(extraData);
							success = true;
						}
						break;
					default:
						if (string::icontains(name, extra::HEADPART)) {
							alphaHDPT.emplace_back(static_cast<RE::NiIntegerExtraData*>(extraData));
							success = true;

						} else if (string::icontains(name, extra::TXST)) {
							TXST.emplace_back(static_cast<RE::NiStringsExtraData*>(extraData));
							success = true;

						} else if (string::icontains(name, extra::SKIN_TXST)) {
							TXSTSkin.emplace_back(static_cast<RE::NiStringsExtraData*>(extraData));
							success = true;

						} else if (string::icontains(name, extra::SHADER)) {
							shader.emplace_back(static_cast<RE::NiStringsExtraData*>(extraData));
							success = true;
						}
						break;
					}
				}
			}

			return { success, resetData };
		}

		void Toggle(RE::NiAVObject* a_root, RE::NiStringsExtraData* a_data)
		{
			if (a_data && a_data->value && a_data->size > 0) {
				std::span<char*> span(a_data->value, a_data->size);
				for (const auto& string : span) {
					if (!string::is_empty(string)) {
						if (const auto object = a_root->GetObjectByName(string); object) {
							object->CullNode(false);
						}
					}
				}
				a_root->RemoveExtraData(a_data->GetName());
			}
		}

		void SkinTint(RE::Actor* a_actor, RE::NiAVObject* a_root, RE::NiIntegerExtraData* a_data)
		{
			if (a_data) {
				const auto actorbase = a_actor->GetActorBase();
				const auto facePart = actorbase ? actorbase->GetCurrentHeadPartByType(HeadPart::kFace) : nullptr;
				const auto faceNode = a_actor->GetFaceNodeSkinned();

				if (faceNode && facePart) {
					if (const auto faceGen = RE::BSFaceGenManager::GetSingleton(); faceGen) {
						faceGen->PrepareHeadPartForShaders(faceNode, facePart, actorbase);
					}
					a_root->UpdateBodyTint(actorbase->bodyTintColor);
					a_root->RemoveExtraData(a_data->GetName());
				}
			}
		}

		void HairTint(RE::Actor* a_actor, RE::NiAVObject* a_root, RE::NiIntegerExtraData* a_data)
		{
			if (a_data) {
				const auto actorBase = a_actor->GetActorBase();
				const auto headData = actorBase ? actorBase->headRelatedData : nullptr;
				const auto colorForm = headData ? headData->hairColor : nullptr;

				if (colorForm) {
					a_root->UpdateHairColor(colorForm->color);

					if (const auto& biped = a_actor->GetCurrentBiped(); biped) {
						for (auto& slot : slot::headSlots) {
							const auto node = biped->objects[slot].partClone;
							if (node && node->HasShaderType(RE::BSShaderMaterial::Feature::kHairTint)) {
								node->UpdateHairColor(colorForm->color);
							}
						}
					}

					a_root->RemoveExtraData(a_data->GetName());
				}
			}
		}

		void SkinAlpha(RE::NiAVObject* a_root, RE::NiBooleanExtraData* a_data)
		{
			if (a_data) {
				a_root->UpdateMaterialAlpha(1.0f, true);
				a_root->RemoveExtraData(a_data->GetName());
			}
		}

		void HeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, const std::vector<RE::NiIntegerExtraData*>& a_data)
		{
			for (auto& data : a_data) {
				if (data) {
					if (const auto object = a_actor->GetHeadPartObject(static_cast<HeadPart>(data->value)); object) {
						object->UpdateMaterialAlpha(1.0f, false);
						a_root->RemoveExtraData(data->GetName());
					}
				}
			}
		}

		void FaceTXST(RE::Actor* a_actor, RE::NiAVObject* a_root, RE::NiStringsExtraData* a_data)
		{
			if (a_data && a_data->value) {
				const auto textureset = TEXTURE::create_textureset(a_data->value);
				const auto faceObject = a_actor->GetHeadPartObject(HeadPart::kFace);
				if (textureset && faceObject) {
					detail::reset_textureset(faceObject, textureset, true, std::string());
					a_root->RemoveExtraData(a_data->GetName());
				}
			}
		}

		void ArmorTXST(RE::Actor* a_actor, RE::NiAVObject* a_root, const RE::BSFixedString& a_folderName, const std::vector<RE::NiStringsExtraData*>& a_vec)
		{
			if (a_vec.empty()) {
				return;
			}

			std::string folder{ a_folderName };
			TEXTURE::sanitize_path(folder);

			for (auto& data : a_vec) {
				if (data && data->value && data->size > 0) {
					RE::FormID formID = 0;
					if (std::string armorID{ data->value[data->size - 1] }; !armorID.empty()) {
						try {
							formID = string::lexical_cast<RE::FormID>(armorID, true);
						} catch (...) {
							continue;
						}
					}
					auto armor = a_actor->GetWornArmor(formID);
					if (!armor) {
						armor = a_actor->GetSkin();
					}
					if (armor) {
						const auto textureset = TEXTURE::create_textureset(data->value);
						const auto arma = armor->GetArmorAddon(a_actor->GetRace());
						if (textureset && arma) {
							a_actor->VisitArmorAddon(armor, arma, [&](bool, RE::NiAVObject& a_obj) -> bool {
								detail::reset_textureset(&a_obj, textureset, false, folder);
								return true;
							});
							a_root->RemoveExtraData(data->GetName());
						}
					}
				}
			}
		}

		void SkinTXST(RE::Actor* a_actor, RE::NiAVObject* a_root, const std::vector<RE::NiStringsExtraData*>& a_vec)
		{
			using Slot = RE::BGSBipedObjectForm::BipedObjectSlot;

			if (a_vec.empty()) {
				return;
			}

			for (auto& data : a_vec) {
				if (data && data->value && data->size > 0) {
					auto slot = Slot::kNone;
					if (std::string slotMaskstr{ data->value[data->size - 1] }; !slotMaskstr.empty()) {
						try {
							slot = string::lexical_cast<Slot>(slotMaskstr);
						} catch (...) {
							continue;
						}
					}
					if (const auto skinArmor = a_actor->GetSkin(slot); skinArmor) {
						const auto textureset = TEXTURE::create_textureset(data->value);
						const auto skinArma = skinArmor->GetArmorAddonByMask(a_actor->GetRace(), slot);
						if (textureset && skinArma) {
							a_actor->VisitArmorAddon(skinArmor, skinArma, [&](bool, RE::NiAVObject& a_obj) -> bool {
								detail::reset_textureset(&a_obj, textureset, true, std::string());
								return true;
							});
							a_root->RemoveExtraData(data->GetName());
						}
					}
				}
			}
		}

		void MaterialShader(RE::NiAVObject* a_root, const std::vector<RE::NiStringsExtraData*>& a_vec)
		{
			if (a_vec.empty()) {
				return;
			}

			for (auto& data : a_vec) {
				if (data && data->value && data->size > 0) {
					std::vector<RE::BSFixedString> vec({ data->value, data->value + data->size });
					detail::reset_shaderdata(a_root, vec);

					a_root->RemoveExtraData(data->GetName());
				}
			}
		}
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
				return std::ranges::any_of(slot::fxBiped, [&](const auto& bipedSlot) {
					const auto addon = a_biped->objects[bipedSlot].addon;
					return addon && mod->IsFormInMod(addon->formID);
				});
			}

			static void attach(RE::BipedAnim* a_biped, RE::NiAVObject* a_object)
			{
				if (a_biped && a_object) {
					const auto ref = a_biped->actorRef.get();
					const auto actor = ref ? ref->As<RE::Actor>() : nullptr;

					if (!actor || actor->IsPlayerRef() || !actor->HasKeywordString("ActorTypeNPC"sv)) {
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
							if (const auto name = a_object->name; !name.empty()) {
								if (std::ranges::find(underwear::underwears, name.c_str()) != underwear::underwears.end() && (*permType == PermEffect::kCharred || *permType == PermEffect::kSkeletonized)) {
									SET::Toggle(root, a_object, true);
								} else if (a_object->HasShaderType(Feature::kFaceGenRGBTint)) {
									SET::SkinAlpha(root, a_object, 0.0f);
								}
							}
						} else if (tempType && !tempType->empty()) {
							if (const auto name = a_object->name; !name.empty()) {
								if (a_object->HasShaderType(Feature::kFaceGenRGBTint) && tempType->contains(TempEffect::kXRayed)) {
									SET::SkinAlpha(root, a_object, 0.5f);
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

				std::optional<std::uint32_t> result = std::nullopt;

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

			struct ProcessObject
			{
				static void thunk(RE::BipedAnim* a_biped, RE::NiAVObject* a_object, std::int32_t a_slot, bool a_unk04)
				{
					func(a_biped, a_object, a_slot, a_unk04);

					detail::attach(a_biped, a_object);
				}
				static inline REL::Relocation<decltype(thunk)> func;
			};

			struct PerformNPCDismember
			{
				static void thunk(RE::TESNPC* a_npc, RE::Actor* a_actor, RE::NiAVObject* a_node)
				{
					func(a_npc, a_actor, a_node);

					if (a_actor && !a_actor->IsPlayerRef() && a_actor->HasKeywordString("ActorTypeNPC"sv)) {
						const auto permType = Manager::GetSingleton()->permanentEffectMap.find(a_actor->GetFormID());
						if (permType && *permType != PermEffect::kNone) {
							return;
						}

						const auto& biped = a_actor->GetCurrentBiped();
						if (const auto root = a_actor->Get3D(); root && biped) {
							const auto addon = biped->objects[Biped::kModMouth].addon;

							if (addon && mod->IsFormInMod(addon->formID)) {
								if (*permType == PermEffect::kCharred || *permType == PermEffect::kSkeletonized) {
									SET::Toggle(root, RE::FixedStrings::GetSingleton()->bsFaceGenNiNodeSkinned, true);
								} else if (*permType == PermEffect::kDrained) {
									for (auto& headpart : slot::headparts) {
										SET::HeadPartAlpha(a_actor, root, headpart, 0.0f);
									}
								} else {
									if (a_actor->HasKeywordString("IsBeastRace"sv)) {
										SET::HeadPartAlpha(a_actor, root, HeadPart::kMisc, 0.0f);
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
				REL::Relocation<std::uintptr_t> processAttachedGeometry{ RELOCATION_ID(15535, 15712), OFFSET(0x79A, 0x72F) };  //armor
				stl::write_thunk_call<ProcessGeometry>(processAttachedGeometry.address());

				REL::Relocation<std::uintptr_t> attachArmorAddon{ RELOCATION_ID(15501, 15678), 0x1EA };  //armor 2
				stl::write_thunk_call<ProcessObject>(attachArmorAddon.address());

				REL::Relocation<std::uintptr_t> processArmorAttach{ RELOCATION_ID(24236, 24740), OFFSET(0x33E, 0x562) };  // head
				stl::write_thunk_call<PerformNPCDismember>(processArmorAttach.address());

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

						if (actor && !actor->IsPlayerRef() && actor->HasKeywordString("ActorTypeNPC"sv)) {
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
										SET::Toggle(root, RE::FixedStrings::GetSingleton()->bsFaceGenNiNodeSkinned, false, false);
										for (auto& headpart : slot::headparts) {
											SET::HeadPartAlpha(actor, root, headpart, 1.0f, false);
										}
										result = RE::BSVisit::BSVisitControl::kStop;
									}
									break;
								case 1:
									{
										SET::SkinAlpha(root, 1.0f, false);
										result = RE::BSVisit::BSVisitControl::kStop;
									}
									break;
								case 2:
									{
										for (auto& name : underwear::underwears) {
											SET::Toggle(root, name, false, false);
										}
										SET::SkinAlpha(root, 1.0f, false);
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
