Scriptname FEC_DeathEffectsPC extends activemagiceffect  
{Plays VFX if killed. Attach to Player magic effect}
 
import game
import debug
import utility
import Sound

import po3_SKSEFunctions
import po3_Events_AME
import FEC_Utility

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							PROPERTIES
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Actor property playerRef auto

;FIRE-CHARRED
Armor property FEC_FireCharBody00 auto  
Armor property FEC_FireCharBody01 auto  
Armor property FEC_FireCharHands auto
Armor property FEC_FireCharFeet auto
Armor property FEC_FireCharHead auto
Armor property FEC_FireCharHeadDecap auto
;FIRE-SKELETON
Armor property FEC_FireSkeleBody00 auto 
Armor property FEC_FireSkeleBody01 auto 
Armor property FEC_FireSkeleHead auto
Armor property FEC_FireSkeleHands auto
Armor property FEC_FireSkeleFeet auto
;DRAIN
Armor property FEC_DrainBody00 auto 
Armor property FEC_DrainBody01 auto   
Armor property FEC_DrainHands auto
Armor property FEC_DrainFeet auto
Armor property FEC_DrainHead auto
Armor property FEC_DrainHeadDecap auto
;SHOCK
Armor property FEC_ShockXRayBody auto
Armor property FEC_ShockXRayHead auto  
;DRAGON-SOUL-ABSORB
Armor property FEC_DefaultSkeleBody00 auto 
Armor property FEC_DefaultSkeleBody01 auto 
Armor property FEC_DefaultSkeleHead auto
Armor property FEC_DefaultSkeleHands auto
Armor property FEC_DefaultSkeleFeet auto
;VAMPIRE
Armor property DLC1VampireSkeletonFXArmor Auto
 
;SUN
Activator property FEC_AshPile auto
;ACID
Activator property FEC_AcidAshPile auto
;VAMPIRE
Activator property DLC1dunHarkonAshPile Auto
Activator property DLC1dunHarkonDeathFXAct Auto

bool isPlayerCreature

;SOUL ABSORB
EffectShader property FEC_FlyingBitsFXS auto  
EffectShader property FEC_FlyingBitsLiteFXS auto  
EffectShader property FEC_SmokeFXS auto 
EffectShader property FEC_FireHeavyFXS auto 
EffectShader property FEC_FireFXS auto 
EffectShader property FEC_BlankFXS auto 
EffectShader property FEC_SoulAbsorbFireFXS auto
EffectShader property FEC_SoulPowerAbsorbFXS auto
;FIRE
EffectShader property FEC_FireCharFXS auto 
EffectShader property FEC_FireCharInstantFXS auto
EffectShader property FEC_SootFXS auto
EffectShader property FEC_FireDisintegrateSkinFXS auto 
EffectShader property FEC_FireDisintegrateSkinLongFXS auto 
EffectShader property FEC_FireDisintegrateFXS auto 
EffectShader property FEC_SteamFXS auto 
EffectShader property FEC_FireSmokeFXS auto 
;ACID
EffectShader property FEC_AcidDisintegrateFXS auto 
;SHOCK
EffectShader property FEC_ShockFXS auto
EffectShader property FEC_ShockExpFXS auto 
EffectShader property FEC_SootInstantFXS auto
;FROST
EffectShader property FEC_FrostFXS auto 
EffectShader property FEC_FrostIceFormFXS auto
;DRAIN/POISON
EffectShader property FEC_PoisonVeinFXS auto 
EffectShader property FEC_DrainSmokeFXS auto 
EffectShader property FEC_DrainDisintegrateSkinFXS auto 
EffectShader property FEC_PoisonWispsFXS auto 
;SUN
EffectShader property DLC1SunFireImpactFXShader auto
EffectShader property DLC1SunFireFXShader auto
EffectShader property DLC1SunFireCloakFXShader auto
EffectShader property FEC_SunDisintegrateFXS auto
;VAMPIRE
EffectShader property DLC1VampireBleedHarkonFireFXS auto
EffectShader property DLC1VampireBleedHarkonDeathFXS auto

;FROST
Explosion property FEC_FrostExplosion auto  
;SHOCK
Explosion property FEC_ShockExplosion auto  
;VAMPIRE
Explosion property HarkonDeathExplosion Auto

float property fDefaultDeathCamDuration auto
float fModifiedDeathCamDuration

Formlist property FEC_ListSkinColor auto
Formlist property FEC_ListHairColor auto

GlobalVariable property FEC_AcidPCToggle auto
GlobalVariable property FEC_FirePCToggle auto
GlobalVariable property FEC_SunPCToggle auto

GlobalVariable property FEC_FireModes auto
GlobalVariable property FEC_FireBurnClothingToggle auto
;
GlobalVariable property FEC_SoulAbsorbPCToggle auto
;
GlobalVariable property FEC_FrostPCToggle auto
GlobalVariable property FEC_FrostModes auto
;
GlobalVariable property FEC_ShockPCToggle auto
GlobalVariable property FEC_ShockModes auto
;
GlobalVariable property FEC_PoisonPCToggle auto
GlobalVariable property FEC_PoisonOpacityColor auto
GlobalVariable property FEC_PoisonMode auto
GlobalVariable property FEC_PoisonOpacityAutoCalc auto
;
GlobalVariable property FEC_DrainPCToggle auto
GlobalVariable property FEC_DrainModes auto
;
GlobalVariable property FEC_VampireLordPCToggle auto
;
GlobalVariable property FEC_DeathCameraDuration auto
GlobalVariable property FEC_DeathCameraExtendToggle auto
GlobalVariable property FEC_DizonaInstalled auto

;FIRE
ImpactDataSet property FEC_FireImpactSet auto
;SHOCK
ImpactDataSet property FEC_BloodImpactSet auto
;ICE
ImpactDataSet property FEC_FrostImpactSet auto
;WATER
ImpactDataSet property FEC_WaterImpactSet auto

int randomEffect
int projectileType
int spellLevel
int magicType

int playerRaceType
int playerSex

int[] property slotsToSkip auto

int kSUN = 0
int kACID = 1
int kFIRE = 2
int kFROST = 3
int kSHOCK = 4
int kDRAIN = 5
int kFIRE_FROST = 6
int kFIRE_SHOCK = 7
int kDRAIN_FROST = 8
int kDRAIN_SHOCK = 9
int kFROST_SHOCK = 10
int kSHOCK_FROST = 11

int kPOISON = 0

int kRANDOM = 0
int kAUTOMATIC = 1
;fire
int kCHAR = 2
int kSKELE = 3
int kVAPOURISE = 4
;frost
int kFREEZE = 2
int kICEBLOCK = 3
int kSHATTER = 4
;shock
int kXRAY = 3
int kEXPLODE = 4
;drain
int kAGE = 1
int kDESSICATE = 2

Keyword property MagicDamageFire auto
Keyword property MagicDamageShock auto
Keyword property MagicDamageFrost auto
;
Keyword property ActorTypeCreature auto
Keyword property ActorTypeAnimal auto
Keyword property ActorTypeDragon auto

;Frost
MiscObject[] property FrostBodyParts auto
;SHOCK
MiscObject[] property ShockBodyParts auto

ObjectReference bodyParts

Race property KhajiitRace auto
Race property KhajiitRaceVampire auto
Race property ArgonianRace auto
Race property ArgonianRaceVampire auto

Race property DLC1VampireBeastRace Auto
;
Race playerRace

;DRAGON-SOUL-ABSORB
Sound property NPCDragonDeathSequenceWind auto
Sound property NPCDragonDeathSequenceExplosion auto

;DRAIN
TextureSet property FEC_BlankTXST auto
;
TextureSet property SkinHeadMaleOld auto
TextureSet property SkinHeadFemaleOld auto
;
TextureSet property SkinHeadMaleArgonianOld auto
TextureSet property SkinHeadFemaleArgonianOld auto
;
TextureSet property SkinHeadMaleKhajiitOld auto
TextureSet property SkinHeadFemaleKhajiitOld auto
;
TextureSet property SkinHandMaleOld auto
TextureSet property SkinHandFemaleOld auto
;
TextureSet property SkinBodyMaleOld auto
TextureSet property SkinBodyFemaleOld auto

VisualEffect property FEC_DragonSoulAbsorbPCEffect auto
VisualEffect property FEC_FireHeatRefractionEffect auto

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Event OnEffectStart(Actor playerRef, Actor caster)

	if WaitFor3DLoad(playerRef)
		ResetActor3D(playerRef, "po3_FEC")
	endif
					
endEvent

Event OnPlayerLoadGame()
		
	if WaitFor3DLoad(playerRef)
		ResetActor3D(playerRef, "po3_FEC")
	endif
			
endEvent
  
Event OnDying(Actor akKiller)

	;---------------------VARIABLES----------------------------		
	int [] permanentEffects = GetCauseOfDeath(playerRef, 0)	
	int [] tempEffects = GetCauseOfDeath(playerRef, 1)	
	
	magicType = permanentEffects[0]
	spellLevel = permanentEffects[1]
	projectileType = permanentEffects[2]

	playerRace = playerRef.GetRace()
	playerSex = playerRef.GetActorBase().GetSex()
	
	if playerRace == ArgonianRace || playerRace == ArgonianRaceVampire
		playerRaceType = 0
	elseif playerRace == KhajiitRace || playerRace == KhajiitRaceVampire
		playerRaceType = 1
	else
		playerRaceType = 2
	endif
	
	isPlayerCreature = playerRef.HasKeyword(ActorTypeAnimal) || playerRef.HasKeyword(ActorTypeCreature)	
	
	randomEffect = GenerateRandomInt(1,3)		
	;-----------------------------------------------------------
		
	if akKiller && akKiller.HasKeyword(ActorTypeDragon) && FEC_SoulAbsorbPCToggle.GetValue() as bool
	
		SetupThirdPersonDeath()
		DragonAbsorbEffect(akKiller)			
		return
			
	elseif playerRace == DLC1VampireBeastRace && FEC_VampireLordPCToggle.GetValue() as bool
	
		SetupThirdPersonDeath()		
		VampireLordEffect()
		return
		
	endif
	
	if magicType != -1
		
		SetupThirdPersonDeath()
		
		if magicType == kFIRE && FEC_FirePCToggle.GetValue() as bool
													
			if IsActorInWater(playerRef) || IsActorUnderWater(playerRef)
				SteamEffect()
			else
				BurnEffect()
			endif
					
		elseif magicType == kFIRE_FROST && FEC_FirePCToggle.GetValue() as bool	
																
			SteamEffect(applyFrost = true)																						

		elseif magicType == kFIRE_SHOCK && FEC_FirePCToggle.GetValue() as bool	
													
			if !IsActorInWater(playerRef) && !IsActorUnderWater(playerRef)
				BurnEffect(applyShock = true)
			else
				SteamEffect(applyShock = true)
			endif
		
		elseif magicType == kFROST && FEC_FrostPCToggle.GetValue() as bool
																
			FreezeEffect()									

		elseif magicType ==	kFROST_SHOCK && FEC_ShockPCToggle.GetValue() as bool
																	
			FreezeEffect(applyShock = true)									

		elseif magicType ==	kSHOCK_FROST && FEC_ShockPCToggle.GetValue() as bool
							
			ShockEffect(applyFrost = true)									

		elseif magicType == kSHOCK && FEC_ShockPCToggle.GetValue() as bool	
											
			ShockEffect()									

		elseif magicType == kDRAIN && FEC_DrainPCToggle.GetValue() as bool	
					
			DrainEffect()											

		elseif magicType == kSUN && FEC_SunPCToggle.GetValue() as bool
						
			AshpileEffect(FEC_SunDisintegrateFXS, FEC_AshPile)	

		;/elseif magicType == kACID && FEC_AcidPCToggle.GetValue() as bool
						
			AshpileEffect(FEC_AcidDisintegrateFXS, FEC_AcidAshPile, 0.50)/;	

		endif
	
	else
		
		magicType = tempEffects[0]
		spellLevel = tempEffects[1]
		projectileType = tempEffects[2]
		
		if magicType != -1
						
			SetupThirdPersonDeath()
						
			if magicType == kPOISON && FEC_PoisonPCToggle.GetValue() as bool

				PoisonEffect()				
														
			endif
			
		endif		

	endif

endEvent

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;========================================================														
	;[0]				BURN EFFECT                                                                                           
;========================================================

Function BurnEffect(bool applyShock = false)

	;---------------------VARIABLES----------------------------	
	int effectType = GetEffectType(kFIRE)	
	;-----------------------------------------------------------

	if (effectType == kRANDOM && randomEffect == 3) || (effectType == kAUTOMATIC && spellLevel == 100) || effectType == kVAPOURISE
		AshpileEffect(FEC_FireDisintegrateFXS, FEC_AshPile)	
		return				
	endif 
	
	EffectShader fireShader
	if spellLevel >= 75
		fireShader = FEC_FireHeavyFXS
	else
		fireShader = FEC_FireFXS
	endif

	fireShader.Play(playerRef, 4.0)
	FEC_FireHeatRefractionEffect.Play(playerRef, 13.0)
	
	if applyShock	
		FEC_ShockFXS.Play(playerRef, GenerateRandomFloat(6.0, 8.0))		
	endif
	
	playerRef.PlayImpactEffect(FEC_FireImpactSet, "MagicEffectsNode", 0.0, 0.0, -1.0, 128.0, true, true)	
	
	Wait(GenerateRandomFloat(2.0,3.0))
		
	if isPlayerCreature
	
		FEC_FireCharFXS.Play(playerRef)
		
	elseif effectType == kAUTOMATIC
	
		if spellLevel >= 75
					
			SetSkeletonCharred()
						
		else
					
			SetCharred()
				
		endif
		
	elseif effectType == kCHAR || (effectType == kRANDOM && randomEffect == 1)
				
		SetCharred()
		
	elseif effectType == kSKELE || (effectType == kRANDOM && randomEffect == 2)
			
		SetSkeletonCharred()
					
	endif
	
	FEC_FireSmokeFXS.Play(playerRef, 8.0)   ; smoke effects
	
endFunction

;========================================================														
;	[1]				STEAM EFFECT                                                                                            
;========================================================

Function SteamEffect(bool applyFrost = false, bool applyShock = false)	
				
	FEC_SteamFXS.Play(playerRef, 8.0)  ; smoke effect
	
	FEC_FireCharInstantFXS.Play(playerRef)
	
	if applyShock		
		FEC_ShockFXS.Play(playerRef, 7.0)		
	endif
		
	Wait(1.0)
	
	ImpactDataSet impact
	if applyFrost	
		impact = FEC_WaterImpactSet
	else
		impact = FEC_FireImpactSet	
	endif
	
	playerRef.PlayImpactEffect(impact, "MagicEffectsNode", 0.0, 0.0, -1.0, 512.0, true, true)	
					
endFunction

;========================================================													
	;[2]				FREEZE EFFECT                                                                                           
;========================================================

Function FreezeEffect(bool applyShock = false)

	;---------------------VARIABLES----------------------------
	int effectType = GetEffectType(kFROST)		
	;-----------------------------------------------------------

	if isPlayerCreature || (effectType == kRANDOM && randomEffect == 2) || effectType == kICEBLOCK
	
		playerRef.PlaceAtMe(FEC_FrostExplosion)
		
		FreezeActor(playerRef, 1, true)
	
		FEC_FrostIceFormFXS.Play(playerRef)
		
		if applyShock		
			FEC_ShockFXS.Play(playerRef, 7.0)			
		endif
		
		playerRef.PlayImpactEffect(FEC_FrostImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 512.0, true, true)	
		
	elseif (effectType == kRANDOM && randomEffect == 1) || (effectType == kAUTOMATIC && projectileType != 1) || effectType == kFREEZE 
	
		playerRef.PlaceAtMe(FEC_FrostExplosion)
				
		FEC_FrostIceFormFXS.Play(playerRef)
		
		playerRef.PlayImpactEffect(FEC_FrostImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 512.0, true, true)	
		
	elseif (effectType == kRANDOM && randomEffect == 3) || (effectType == kAUTOMATIC && projectileType == 1) || effectType == kSHATTER  ; shatter
	
		FEC_FrostFXS.Play(playerRef)	
	
		bodyParts = PlaceBodyParts(playerRef, FrostBodyParts, playerRaceType, playerSex)
				
		WaitFor3DLoad(bodyParts)

		playerRef.PlayImpactEffect(FEC_FrostImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 512.0, true, true)			
		
		playerRef.SetAlpha(0.0)
		
		FEC_FrostFXS.Stop(playerRef)	
		
	endif
	
endFunction
 
;========================================================													
	;[3]				SHOCK EFFECT                                                                                           
;========================================================

Function ShockEffect(bool applyFrost = false)

	;---------------------VARIABLES----------------------------	
	int effectType = GetEffectType(kSHOCK)
	;----------------------------------------------------------

	if isPlayerCreature || (effectType == kRANDOM && randomEffect == 1) || (effectType == kAUTOMATIC && spellLevel < 25 ) || effectType == kCHAR ; fry
	
		FEC_ShockFXS.Play(playerRef)
		
		if applyFrost		
			playerRef.PlaceAtMe(FEC_FrostExplosion)
		endif

		playerRef.PushActorAway(playerRef, 0.0)

		if isPlayerCreature
			FEC_FireCharFXS.Play(playerRef)
		else
			FEC_SootFXS.Play(playerRef)
		endif

		ApplyHavokImpulse()
	
		FEC_SmokeFXS.Play(playerRef, 6.0)   ; smoke effect 
		
		FEC_ShockFXS.Stop(playerRef)
	
	elseif (effectType == kRANDOM && randomEffect == 2) || (effectType == kAUTOMATIC && spellLevel < 75) || effectType == kXRAY ;xray
	
		EquipHead(playerRef, FEC_ShockXRayHead, None, false, false)
		playerRef.EquipItem(FEC_ShockXRayBody)
		
		SetSkinAlpha(playerRef, 0.5)
			
		FEC_ShockExpFXS.Play(playerRef)
		
		ApplyHavokImpulse()
		
		FEC_SootInstantFXS.Play(playerRef)
		FEC_ShockExpFXS.Stop(playerRef)
		
		wait(0.75)
		
		playerRef.UnEquipItem(FEC_ShockXRayBody)
		playerRef.UnequipItem(FEC_ShockXRayHead)
		
		SetSkinAlpha(playerRef, 1.0)
		
		FEC_SmokeFXS.Play(playerRef, 6.0)   ; smoke effect 
	
	elseif (effectType == kRANDOM && randomEffect == 3) || (effectType == kAUTOMATIC && spellLevel >= 75) || effectType == kEXPLODE ; explode

		FEC_ShockExpFXS.Play(playerRef)
		
		if applyFrost
			playerRef.PlaceAtMe(FEC_FrostExplosion)
		endif
		
		bodyParts = PlaceBodyParts(playerRef, ShockBodyParts, playerRaceType, playerSex)
				
		WaitFor3DLoad(bodyParts)	
		
		playerRef.PlaceAtMe(FEC_ShockExplosion)		
		
		playerRef.PlayImpactEffect(FEC_BloodImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 512.0, true, true)		
		
		playerRef.SetAlpha(0.0)
		FEC_ShockExpFXS.Stop(playerRef)		
		
	endif
		
endFunction
  
;========================================================													
	;[4]				DRAIN EFFECT                                                                                           
;========================================================

Function DrainEffect(bool applyFrost = false, bool applyShock = false)

	;---------------------VARIABLES--------------------------	
	int effectType = GetEffectType(kDRAIN)
	int randomDrainEffect = GenerateRandomInt(1,2)	
	;--------------------------------------------------------
	
	if effectType == kRANDOM && randomDrainEffect == 2 || effectType == kDESSICATE
	
		FEC_DrainSmokeFXS.Play(playerRef)
	
		if applyShock		
			FEC_ShockFXS.Play(playerRef, 6.0)			
		endif
		if applyFrost
			FEC_FrostFXS.Play(playerRef, 6.0)
		endif
	
		Wait(1.0)
		
		EquipHead(playerRef, FEC_DrainHead, FEC_DrainHeadDecap, true, true)	
		EquipUnderSkin(playerRef, 32, FEC_DrainBody00, FEC_DrainBody01)
		EquipUnderSkin(playerRef, 33, FEC_DrainHands)
		EquipUnderSkin(playerRef, 37, FEC_DrainFeet)
			
		FEC_DrainDisintegrateSkinFXS.Play(playerRef)
		
		SetHeadPartAlpha(playerRef, 0, 0.0) ;mouth	
		SetHeadPartAlpha(playerRef, 2, 0.0) ;eyes
		playerRef.SetEyeTexture(FEC_BlankTXST) ;hetrochromic eyes are not affected by SetHDPTAlpha		
		
		SetSkinAlpha(playerRef, 0.0)
	
		Wait(3.30)
	
		FEC_DrainSmokeFXS.Stop(playerRef)
		
	elseif effectType == kRANDOM && randomDrainEffect == 1 || effectType == kAGE
	
		if applyShock					
			FEC_ShockFXS.Play(playerRef, 7.0)
			FEC_SmokeFXS.Play(playerRef, 7.0)			
		endif
		if applyFrost
			FEC_FrostFXS.Play(playerRef, 7.0)
		endif	
		
		if playerRaceType == 0
			ReplaceFaceTextureSet(playerRef, SkinHeadMaleArgonianOld, SkinHeadFemaleArgonianOld, 3)
		elseif playerRaceType == 1
			ReplaceFaceTextureSet(playerRef, SkinHeadMaleKhajiitOld, SkinHeadFemaleKhajiitOld, 3)
		else
			ReplaceFaceTextureSet(playerRef, SkinHeadMaleOld, SkinHeadFemaleOld, 1)			
			if FEC_DizonaInstalled.GetValue() == 0.0
				ReplaceSkinTextureSet(playerRef, SkinBodyMaleOld, SkinBodyFemaleOld, 0x00000004, 1)
				if !playerRef.GetEquippedArmorInSlot(33)
					ReplaceSkinTextureSet(playerRef, SkinHandMaleOld, SkinHandFemaleOld, 0x00000008, 1)
				endif			
				if !playerRef.GetEquippedArmorInSlot(37)
					ReplaceSkinTextureSet(playerRef, SkinBodyMaleOld, SkinBodyFemaleOld, 0x00000080, 1)
				endif
			endif
		endif		
			
		SetHairColor(playerRef, FEC_ListHairColor.GetAt(0) as ColorForm)
		
	endif
	  	
endFunction 
 
;========================================================													
	;[5]				POISON EFFECT                                                                                           
;========================================================

Function PoisonEffect()

	FEC_PoisonWispsFXS.Play(playerRef,4.0)
	
	FEC_PoisonVeinFXS.Play(playerRef)	
	
	BlendColorWithSkinTone(playerRef, FEC_ListSkinColor.GetAt(1) as ColorForm, FEC_PoisonMode.GetValue() as int, FEC_PoisonOpacityAutoCalc.GetValue() as bool, FEC_PoisonOpacityColor.GetValue())
		
endFunction
 
;========================================================													
	;[6]				DRAGON ABSORB EFFECT                                                                                           
;========================================================

Function DragonAbsorbEffect(Actor akKiller)

	FEC_SoulAbsorbFireFXS.Play(playerRef)		; main fire effect I

	Wait(2.0)	

	FEC_FlyingBitsLiteFXS.Play(playerRef, 10.0)		; playing flying bits 	  		
	FEC_FlyingBitsFXS.Play(playerRef, 10.0) 		

	Wait(3.0)
			
	SetSkeletonDefault()			

	Wait(2.0)
	
	;----------------------------- STARTING ABSORB SEQUENCE --------------------------

	NPCDragonDeathSequenceWind.play(playerRef) 						;Sounds for when the wispy particles being to fly at the player.	
	NPCDragonDeathSequenceExplosion.play(playerRef) 

	FEC_DragonSoulAbsorbPCEffect.Play(playerRef, 8.0, akKiller)	;display dragon absorb effect art. One part on dragon one part on player.	
	FEC_SoulPowerAbsorbFXS.Play(playerRef)

	;-------------------------------------- SMOKEY EFFECTS ----------------------------

	Wait(0.8)

	FEC_SoulAbsorbFireFXS.Stop(playerRef)		; main fire effect I

	Wait(4.0)     

 	FEC_SoulPowerAbsorbFXS.Stop(playerRef) 

	FEC_SmokeFXS.Play(playerRef, 5.0)   ; smoke effects
	
endFunction

;========================================================													
	;[7]				VAMPIRE LORD EFFECT                                                                                           
;========================================================

Function VampireLordEffect()

	playerRef.EquipItem(DLC1VampireSkeletonFXArmor)
		
	;playerRef.SetSubGraphFloatVariable("fdampRate", 1.0)	;;speeds up fade rate (max 1 min .1
	;playerRef.SetSubGraphFloatVariable("ftoggleBlend", 0.0)	;;blends between two anims default 0 (0 = there 1 = gone)
		
	ObjectReference deathFX = playerRef.PlaceAtMe(DLC1dunHarkonDeathFXAct)
	deathFX.PlayAnimation("PlayAnim01")
	
	playerRef.PlaceAtMe(HarkonDeathExplosion)
	
	playerRef.PlaySubGraphAnimation("TransitionAnim")
	DLC1VampireBleedHarkonDeathFXS.Play(playerRef,0.1)

	playerRef.SetSubGraphFloatVariable("fdampRate", 0.005) ;;speeds up fade rate (max 1 min .1
	playerRef.SetSubGraphFloatVariable("ftoggleBlend", 1.3) ;;blends between two anims default 0 (0 = there 1 = gone)
	
	Wait(1.0)
	DLC1VampireBleedHarkonFireFXS.Play(playerRef,0.5)
		
	Wait(1.5)		
	playerRef.PlaySubGraphAnimation("TransitionAnim")	
	playerRef.AttachAshPile(DLC1dunHarkonAshPile)	
		
	Wait(3.0)	
	playerRef.SetAlpha(0.0)

endFunction

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;						FIRE FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Function SetSkeletonCharred()

	EquipHead(playerRef, FEC_FireSkeleHead, None, false, true)		
	EquipUnderSkin(playerRef, 32, FEC_FireSkeleBody00, FEC_FireSkeleBody01)
	EquipUnderSkin(playerRef, 33, FEC_FireSkeleHands)
	EquipUnderSkin(playerRef, 37, FEC_FireSkeleFeet)
	
	FEC_FireDisintegrateSkinLongFXS.Play(playerRef)	
	
	FEC_FlyingBitsLiteFXS.Play(playerRef, 8.0)	  		
	FEC_FlyingBitsFXS.Play(playerRef, 6.0) 	
			
	Wait(GenerateRandomFloat(2.00,3.00))
	
	BurnSkinAndClothing(true)
		
endFunction

Function SetSkeletonDefault()
	
	EquipHead(playerRef, FEC_DefaultSkeleHead, None, false, true)		
	EquipUnderSkin(playerRef, 32, FEC_DefaultSkeleBody00, FEC_DefaultSkeleBody01)
	EquipUnderSkin(playerRef, 33, FEC_DefaultSkeleHands)
	EquipUnderSkin(playerRef, 37, FEC_DefaultSkeleFeet)
	
	FEC_FireDisintegrateSkinLongFXS.Play(playerRef)	
	
	FEC_FlyingBitsLiteFXS.Play(playerRef, 8.0)	  		
	FEC_FlyingBitsFXS.Play(playerRef, 6.0) 	
					
	Wait(GenerateRandomFloat(2.00,3.00))
	
	BurnSkinAndClothing(false)
	
endFunction

Function SetCharred()
	
	EquipHead(playerRef, FEC_FireCharHead, FEC_FireCharHeadDecap, true, true)		
	EquipUnderSkin(playerRef, 32, FEC_FireCharBody00, FEC_FireCharBody01)
	EquipUnderSkin(playerRef, 33, FEC_FireCharHands)
	EquipUnderSkin(playerRef, 37, FEC_FireCharFeet)
	
	FEC_FireDisintegrateSkinFXS.Play(playerRef)
	
	FEC_FlyingBitsLiteFXS.Play(playerRef, 8.0)	  		
	FEC_FlyingBitsFXS.Play(playerRef, 6.0) 	
		
	Wait(GenerateRandomFloat(2.00,3.00))
	
	BurnSkinAndClothing(true)
				
endFunction 

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                         MISC FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Function GetEffectType(int type)

	if type == kFIRE ; FIRE			
		return FEC_FireModes.GetValue() as int				
	elseif type == kFROST ; FROST
		return FEC_FrostModes.GetValue() as int		
	elseif type == kSHOCK ; SHOCK	
		return FEC_ShockModes.GetValue() as int	
	elseif type == kDRAIN ; DRAIN	
		return FEC_DrainModes.GetValue() as int		
	endif
	
	return 1 ;AUTO
	
endFunction	

Function SetupThirdPersonDeath()

	StopAllShaders(playerRef)
	Wait(0.1)
	if FEC_DeathCameraExtendToggle.GetValue() as bool		
		SetGameSettingFloat("fPlayerDeathReloadTime", FEC_DeathCameraDuration.GetValue())		
	endif
	
endFunction

Function AshpileEffect(EffectShader disintegrateFXS, Activator ashPile, float startTime = 1.25, float endTime = 1.65)
		
	DisintegrateFXS.Play(playerRef)

	Wait(startTime)	

	playerRef.AttachAshPile(ashPile)

	Wait(endTime)

	playerRef.SetAlpha(0.0)
	
	DisintegrateFXS.Stop(playerRef)	
	
endFunction

Function BurnSkinAndClothing(bool burnClothing)

	ToggleChildNode(playerRef, "BSFaceGenNiNodeSkinned", true)
	VaporizeUnderwear(playerRef)
	
	if burnClothing && FEC_FireBurnClothingToggle.GetValue() == 1.0	
		Wait(GenerateRandomFloat(1.00,1.50))
		UnequipAllOfType(playerRef, 2, slotsToSkip)	
	endif
	
	SetSkinAlpha(playerRef, 0.0)

endFunction


Function ApplyHavokImpulse()

	int randomTime = GenerateRandomInt(25, 35)	
	
	int startTime = 0;
	while startTime < randomTime
		playerRef.ApplyHavokImpulse(0.0, 0.0, 1.0, (GenerateRandomInt(50, 160)))
		Wait(GenerateRandomFloat(0.25,0.50))
		startTime += 1
	endwhile

endFunction
