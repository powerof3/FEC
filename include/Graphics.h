#pragma once

namespace FEC::GRAPHICS
{
	namespace EXTRA
	{
		template <class T, typename D>
		void add_data_if_none(RE::NiAVObject* a_root, std::string_view a_type, D a_data)
		{
			if (const auto data = a_root->GetExtraData<T>(a_type); !data) {
				if (const auto newData = T::Create(a_type, a_data)) {
					a_root->AddExtraData(newData);
				}
			}
		}
	}

	namespace TEXTURE
	{
		void                    sanitize_path(std::string& a_path);
		RE::BSShaderTextureSet* create_textureset(char** a_value);
	}

	namespace SET
	{
		void SkinAlpha(RE::NiAVObject* a_root, float a_alpha, bool a_setData = true);
		void SkinAlpha(RE::NiAVObject* a_root, RE::NiAVObject* a_node, float a_alpha, bool a_setData = true);

		void Toggle(RE::NiAVObject* a_root, const RE::BSFixedString& a_nodeName, bool a_toggle, bool a_setData = true);
		void Toggle(RE::NiAVObject* a_root, RE::NiAVObject* a_node, bool a_toggle, bool a_setData = true);

		void HeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, HeadPart a_type, float a_alpha, bool a_setData = true);
	}

	namespace RESET
	{
		using ResetData = std::tuple<
			RE::NiStringsExtraData*,
			RE::NiIntegerExtraData*,
			RE::NiIntegerExtraData*,
			RE::NiBooleanExtraData*,
			RE::NiStringsExtraData*,
			std::vector<RE::NiIntegerExtraData*>,
			std::vector<RE::NiStringsExtraData*>,
			std::vector<RE::NiStringsExtraData*>,
			std::vector<RE::NiStringsExtraData*>>;

		using ShaderData = std::tuple<RE::BSShaderTextureSet*, Feature, std::uint64_t, RE::NiColor, float>;

		void                       stop_all_skin_shaders(RE::TESObjectREFR* a_ref);
		std::pair<bool, ResetData> get_data(RE::NiAVObject* a_object);

		void Toggle(RE::NiAVObject* a_root, RE::NiStringsExtraData* a_data);
		void SkinAlpha(RE::NiAVObject* a_root, RE::NiBooleanExtraData* a_data);
		void HeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, const std::vector<RE::NiIntegerExtraData*>& a_data);
		void SkinTint(RE::Actor* a_actor, RE::NiAVObject* a_root, RE::NiIntegerExtraData* a_data);
		void HairTint(RE::Actor* a_actor, RE::NiAVObject* a_root, RE::NiIntegerExtraData* a_data);
		void FaceTXST(RE::Actor* a_actor, RE::NiAVObject* a_root, RE::NiStringsExtraData* a_data);
		void ArmorTXST(RE::Actor* a_actor, RE::NiAVObject* a_root, const RE::BSFixedString& a_folderName, const std::vector<RE::NiStringsExtraData*>& a_vec);
		void SkinTXST(RE::Actor* a_actor, RE::NiAVObject* a_root, const std::vector<RE::NiStringsExtraData*>& a_vec);
		void MaterialShader(RE::NiAVObject* a_root, const std::vector<RE::NiStringsExtraData*>& a_vec);
	}

	namespace ARMOR
	{
		namespace ATTACH
		{
			void Install();
		}

		namespace DETACH
		{
			void Install();
		}
	}

	void Install();
}
