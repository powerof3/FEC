#pragma once

namespace FEC::GRAPHICS
{
	namespace TEXTURE
	{
		void                    sanitize_path(std::string& a_path);
		RE::BSShaderTextureSet* create_textureset(char** a_value);
	}

	struct ShaderDataOutput
	{
		enum TYPE
		{
			kDiffuse = 0,
			kTextureTotal = 9,

			kFeature = kTextureTotal,
			kFlag,
			kColor,
			kColorMult,

			kShaderTotal
		};

		ShaderDataOutput() = default;
		ShaderDataOutput(RE::NiStringsExtraData* a_data);

		bool Reset(RE::BSGeometry* a_geometry, RE::BSLightingShaderProperty* a_shaderProp, MaterialBase* a_material) const;

		RE::BSShaderTextureSet* textureSet;
		REX::Enum<Feature>      feature;
		std::uint64_t           flags;
		RE::NiColor             emissiveColor;
		float                   emissiveMult;
		bool                    hasData;
	};

	struct ActorApplier
	{
		static void ToggleNode(RE::NiAVObject* a_root, RE::NiAVObject* a_node, bool a_cull, bool a_setData = false);
		static void ToggleNode(RE::NiAVObject* a_root, const RE::BSFixedString& a_nodeN, bool a_cull, bool a_setData = false);
		static void HeadPartAlpha(RE::Actor* a_actor, RE::NiAVObject* a_root, HeadPartType a_type, float a_alpha, bool a_setData = false);
		static void SkinAlpha(RE::NiAVObject* a_root, float a_alpha, bool a_setData = false);
		static void SkinAlpha(RE::NiAVObject* a_root, RE::NiAVObject* a_node, float a_alpha, bool a_setData = false);
	};

	struct ActorResetter
	{
		ActorResetter() = default;
		ActorResetter(RE::Actor* a_actor, RE::NiAVObject* a_object, const RE::BSFixedString& a_folderName = {});

		bool ResetEffectsNotOfType(EFFECT::TYPE a_effectType) const;

	private:
		static void stop_all_skin_shaders(RE::TESObjectREFR* a_ref);
		static void reset_textureset(RE::NiAVObject* a_object, RE::BSShaderTextureSet* a_txst, bool a_doOnlySkin, const std::string& a_folder = {});
		static void reset_shaderdata(RE::NiAVObject* a_object, std::vector<RE::BSFixedString>& a_geometries);

		void ResetToggle() const;
		void ResetSkinAlpha() const;
		void ResetHeadPartAlpha() const;
		void ResetSkinTint() const;
		void ResetHairTint() const;
		void ResetFaceTXST() const;
		void ResetArmorTXST() const;
		void ResetSkinTXST() const;
		void ResetMaterialShader() const;

		// members
		RE::NiPointer<RE::Actor>             actor{};
		RE::NiPointer<RE::NiAVObject>        root{};
		std::string                          folderName{};
		RE::NiStringsExtraData*              toggle{};
		RE::NiIntegerExtraData*              tintSkin{};
		RE::NiIntegerExtraData*              tintHair{};
		RE::NiBooleanExtraData*              alphaSkin{};
		RE::NiStringsExtraData*              txstFace{};
		std::vector<RE::NiIntegerExtraData*> alphaHDPT{};
		std::vector<RE::NiStringsExtraData*> txst{};
		std::vector<RE::NiStringsExtraData*> txstSkin{};
		std::vector<RE::NiStringsExtraData*> shaders{};
		bool                                 hasData{ false };
	};

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
