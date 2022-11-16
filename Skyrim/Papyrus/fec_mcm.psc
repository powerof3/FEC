Scriptname FEC_MCM extends MCM_ConfigBase
;MCM version 1

import po3_SKSEFunctions ;helper functions
import Game
import Utility
import FEC_Utility

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							PROPERTIES
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Actor property playerRef auto

Keyword property ActorTypeNPC auto

Formlist property FEC_ListSkinColor auto

Spell property FEC_DeathEffectsNPCAb auto

;===================;
;		GLOBAL
;===================;

;CHANCE
GlobalVariable property FEC_GlobalRandomChance auto
;AFTER DEATH
GlobalVariable property FEC_PostDeathEffectsNPCToggle auto
GlobalVariable property FEC_PostDeathEffectsCrToggle auto
;DEATH CAMERA
GlobalVariable property FEC_DeathCameraExtendToggle auto
GlobalVariable property FEC_DeathCameraDuration auto
;HITBOX
GlobalVariable property FEC_CollisionScaleToggle auto

;===================;
;		FIRE
;===================;
GlobalVariable property FEC_FireRandomChance auto
GlobalVariable property FEC_FireRandomChanceCr auto
;PC
GlobalVariable property FEC_FirePCToggle auto
;MODES
GlobalVariable property FEC_FireModes auto
GlobalVariable property FEC_FireModesCr auto
String[] fireModes
String[] fireModesCR
;OPTIONS
GlobalVariable property FEC_FireBurnClothingToggle auto
GlobalVariable property FEC_FireRemoveClothingToggle auto
GlobalVariable property FEC_FireUndeadToggle auto
GlobalVariable property FEC_FireDamageToggle auto
GlobalVariable property FEC_CookedFoodToggle auto
EffectShader[] property FireFXS auto
GlobalVariable property FEC_FireFXParticleCount auto

;===================;
;		FROST
;===================;

;CHANCE
GlobalVariable property FEC_FrostRandomChance auto
GlobalVariable property FEC_FrostRandomChanceCr auto
;PC
GlobalVariable property FEC_FrostPCToggle auto
;MODES
GlobalVariable property FEC_FrostModes auto
GlobalVariable property FEC_FrostModesCR auto
String[] frostModes
String[] frostModesCR
;OPTIONS
GlobalVariable property FEC_FrostShatterToggle auto
GlobalVariable property FEC_FrostShaderCrToggle auto

;===================;
;		SHOCK
;===================;

;CHANCE
GlobalVariable property FEC_ShockRandomChance auto
GlobalVariable property FEC_ShockRandomChanceCr auto
;PC
GlobalVariable property FEC_ShockPCToggle auto
;MODES
GlobalVariable property FEC_ShockModes auto
String[] shockModes
;OPTIONS
GlobalVariable property FEC_ShockDamageToggle auto
GlobalVariable property FEC_ShockWeaponDropToggle auto

;===================;
;		DRAIN
;===================;

;CHANCE
GlobalVariable property FEC_DrainRandomChance auto
;PC
GlobalVariable property FEC_DrainPCToggle auto
;MODE
GlobalVariable property FEC_DrainModes auto
String[] drainModes

;===================;
;		POISON
;===================;

;CHANCE
GlobalVariable property FEC_PoisonRandomChance auto
GlobalVariable property FEC_PoisonRandomChanceCr auto
;PC
GlobalVariable property FEC_PoisonPCToggle auto
;MODE
GlobalVariable property FEC_PoisonMode auto
;COLOR
ColorForm poisonSkin 
GlobalVariable property FEC_PoisonOpacityAutoCalc auto
GlobalVariable property FEC_PoisonOpacityColor auto

;===================;
;		SUN
;===================;

;CHANCE
GlobalVariable property FEC_SunRandomChance auto
GlobalVariable property FEC_SunRandomChanceCr auto
;PC
GlobalVariable property FEC_SunPCToggle auto
;PARTICLE
EffectShader property FEC_SunDisintegrateFXS auto
GlobalVariable property FEC_SunFXParticleCount auto

;===================;
;		FEAR
;===================;

;CHANCE
GlobalVariable property FEC_FearRandomChance auto
GlobalVariable property FEC_FearRandomChanceCr auto

;====================;
;		SOULTRAP
;====================;

;CHANCE
GlobalVariable property FEC_SoulTrapRandomChance auto
GlobalVariable property FEC_SoulTrapRandomChanceCr auto
;PC
GlobalVariable property FEC_SoulAbsorbPCToggle auto

;===================;
;		ACID
;===================;

;CHANCE
GlobalVariable property FEC_AcidRandomChance auto
GlobalVariable property FEC_AcidRandomChanceCr auto
;PC
GlobalVariable property FEC_AcidPCToggle auto

;===================;
;		VAMPIRE
;===================;

;PC
GlobalVariable property FEC_VampireLordPCToggle auto

;===================;
;		AIR
;===================;

;CHANCE
GlobalVariable property FEC_AirRandomChance auto

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Function GetVersion()

	return 1 ; Default version

endFunction

Event OnConfigInit()

	SetupEffectModes()	
	LoadSettings()
	
endEvent

Event OnGameReload()

	parent.OnGameReload()
	
	SetupEffectModes()
	LoadSettings()
	
endEvent

Event OnConfigClose()

	ReapplyPoison()

EndEvent

Event OnSettingChange(String a_ID)

	if a_ID == "iRandomChanceGlobal:Settings"
		SetGlobalRandomChance(GetGlobalInt(FEC_GlobalRandomChance, a_ID))
	elseif a_ID == "bAllowDeathCam:Settings"
		if !GetGlobalBool(FEC_DeathCameraExtendToggle, "bAllowDeathCam:Settings")
			SetGameSettingFloat("fPlayerDeathReloadTime", 5.0)
		endif
	elseif a_ID == "iDeathCamDur:Settings"
		int deathCamDur = GetGlobalInt(FEC_DeathCameraDuration, a_ID)
		SetGameSettingFloat("fPlayerDeathReloadTime", deathCamDur as float)
	elseif a_ID == "bAllowAfterDeathNPC:Settings"
		SendAfterDeathActiveEvent(FEC_PostDeathEffectsNPCToggle, "bAllowAfterDeathNPC:Settings", "NPC")
	elseif a_ID == "bAllowAfterDeathCR:Settings"
		SendAfterDeathActiveEvent(FEC_PostDeathEffectsCRToggle, "bAllowAfterDeathCR:Settings","CR")
	elseif a_ID == "bAllowHitBoxScaling:Settings"
		SetGlobalBool(FEC_CollisionScaleToggle, a_ID)
	elseif a_ID == "bAffectPlayer:Fire"
		SetGlobalBool(FEC_FirePCToggle, a_ID)
	elseif a_ID == "iRandomChanceNPC:Fire"
		SetGlobalInt(FEC_FireRandomChance, a_ID)
	elseif a_ID == "iRandomChanceCR:Fire"
		SetGlobalInt(FEC_FireRandomChanceCR, a_ID)
	elseif a_ID == "bRandom:FireEffects"
		if GetModSettingBool(a_ID)
			FEC_FireModes.SetValue(0.0)
		endif
	elseif SetEffectModeOnChange(FEC_FireModes, a_ID, fireModes)
		return
	elseif a_ID == "iFireParticleCount:FireVisuals"
		int fireParticleCount = GetGlobalInt(FEC_FireFXParticleCount, a_ID)
		int iFireFX = FireFXS.length
		while iFireFX
			iFireFX -= 1
			SetParticleFullCount(FireFXS[iFireFX], fireParticleCount)
			SetParticlePersistentCount(FireFXS[iFireFX], fireParticleCount / 1.2857)
		endWhile
	elseif a_ID == "bAllowVampCombust:FireVisuals"
		SetGlobalBool(FEC_FireUndeadToggle, a_ID)
	elseif SetEffectModeOnChange(FEC_FireModesCr, a_ID, fireModesCR)
		return
	elseif a_ID == "bAllowCookFood:FireImmersion"
		SetGlobalBool(FEC_CookedFoodToggle, a_ID)
	elseif a_ID == "bAllowFireDamage:FireImmersion"
		SendEffectDamageChangeEvent(FEC_FireDamageToggle, a_ID)
	elseif a_ID == "bAllowBurnClothing:FireImmersion"
		SetGlobalBool(FEC_FireBurnClothingToggle, a_ID)
	elseif a_ID == "bAllowRemoveClothing:FireImmersion"
		SetGlobalBool(FEC_FireRemoveClothingToggle, a_ID)
	elseif a_ID == "bAffectPlayer:Frost"
		SetGlobalBool(FEC_FrostPCToggle, a_ID)
	elseif a_ID == "iRandomChanceNPC:Frost"
		FEC_FrostRandomChance.SetValue(GetModSettingInt(a_ID) as Float)
	elseif a_ID == "iRandomChanceCR:Frost"
		FEC_FrostRandomChanceCr.SetValue(GetModSettingInt(a_ID) as Float)
	elseif SetEffectModeOnChange(FEC_FrostModes, a_ID, frostModes)
		return
	elseif SetEffectModeOnChange(FEC_FrostModesCR, a_ID, frostModesCR)
		return
	elseif a_ID == "bAllowFrozenShatter:FrostVisuals"
		SetGlobalBool(FEC_FrostShatterToggle, a_ID)
	elseif a_ID == "bApplyIceShader:FrostVisuals"
		SetGlobalBool(FEC_FrostShaderCrToggle, a_ID)
	elseif a_ID == "bAffectPlayer:Shock"
		SetGlobalBool(FEC_ShockPCToggle, a_ID)
	elseif a_ID == "iRandomChanceNPC:Shock"
		SetGlobalInt(FEC_ShockRandomChance, a_ID)
	elseif a_ID == "iRandomChanceCR:Shock"
		SetGlobalInt(FEC_ShockRandomChanceCr, a_ID)
	elseif SetEffectModeOnChange(FEC_ShockModes, a_ID, shockModes)
		return
	elseif a_ID == "bAllowShockDamage:ShockImmersion"
		SendEffectDamageChangeEvent(FEC_ShockDamageToggle, a_ID)
	elseif a_ID == "bAllowWeaponDrop:ShockImmersion"
		SetGlobalBool(FEC_ShockWeaponDropToggle, a_ID)
	elseif a_ID == "bAffectPlayer:Drain"
		SetGlobalBool(FEC_DrainPCToggle, a_ID)
	elseif a_ID == "iRandomChance:Drain"
		SetGlobalInt(FEC_DrainRandomChance, a_ID)
	elseif SetEffectModeOnChange(FEC_DrainModes, a_ID, drainModes)
		return
	elseif a_ID == "bAffectPlayer:Poison"
		SetGlobalBool(FEC_PoisonPCToggle, a_ID)
	elseif a_ID == "iRandomChanceNPC:Poison"
		SetGlobalInt(FEC_PoisonRandomChance, a_ID)
	elseif a_ID == "iRandomChanceCR:Poison"
		SetGlobalInt(FEC_PoisonRandomChanceCr, a_ID)
	elseif a_ID == "iPoisonColor:Poison"
		UpdatePoisonColor()
	elseif a_ID == "sPoisonColorHex:Poison"
		UpdatePoisonColorHex()
	elseif a_ID == "iBlendModes:PoisonBlendMode"
		SetGlobalInt(FEC_PoisonMode, a_ID)
	elseif a_ID == "bAutoSkinLuminanceDetect:PoisonBlendMode"
		SetGlobalBool(FEC_PoisonOpacityAutoCalc, "bAutoSkinLuminanceDetect:PoisonBlendMode")
	elseif a_ID == "iPoisonIntensity:PoisonBlendMode"
		FEC_PoisonOpacityColor.SetValue((GetModSettingInt(a_ID) / 100) as float)
	elseif a_ID == "iRandomChance:Air"
		SetGlobalInt(FEC_AirRandomChance, a_ID)
	elseif a_ID == "bAffectPlayer:Acid"
		SetGlobalBool(FEC_AcidPCToggle, a_ID)
	elseif a_ID == "iRandomChanceNPC:Acid"
		SetGlobalInt(FEC_AcidRandomChance, a_ID)
	elseif a_ID == "iRandomChanceCR:Acid"
		SetGlobalInt(FEC_AcidRandomChanceCr, a_ID)
	elseif a_ID == "bAffectPlayer:DragonSoulAbsorb"
		SetGlobalBool(FEC_SoulAbsorbPCToggle, a_ID)
	elseif a_ID == "iRandomChanceNPC:Fear"
		SetGlobalInt(FEC_FearRandomChance, a_ID)
	elseif a_ID == "iRandomChanceCR:Fear"
		SetGlobalInt(FEC_FearRandomChanceCr, a_ID)
	elseif a_ID == "iRandomChanceNPC:SoulTrap"
		SetGlobalInt(FEC_SoulTrapRandomChance, a_ID)
	elseif a_ID == "iRandomChanceCR:SoulTrap"
		SetGlobalInt(FEC_SoulTrapRandomChanceCr, a_ID)
	elseif a_ID == "bAffectPlayer:Sun"
		SetGlobalBool(FEC_SunPCToggle, a_ID)
	elseif a_ID == "iRandomChanceNPC:Sun"
		SetGlobalInt(FEC_SunRandomChance, a_ID)
	elseif a_ID == "iRandomChanceCR:Sun"
		SetGlobalInt(FEC_SunRandomChanceCr, a_ID)
	elseif a_ID == "iSunParticleCount:Sun"
		int sunParticleCount = GetGlobalInt(FEC_SunFXParticleCount, a_ID)
		SetParticleFullCount(FEC_SunDisintegrateFXS, sunParticleCount)
		SetParticlePersistentCount(FEC_SunDisintegrateFXS, sunParticleCount / 1.2857)
	elseif a_ID == "bAffectPlayer:VampireLord"
		SetGlobalBool(FEC_VampireLordPCToggle, a_ID)
	endif
	
endEvent

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							UTIL
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Function SetSettingInt(GlobalVariable gVar, String modSetting)

	SetModSettingInt(modSetting, gVar.GetValue() as int)

endFunction

Function SetSettingIntValue(int value, String modSetting)

	SetModSettingInt(modSetting, value)

endFunction

Function SetSettingBool(GlobalVariable gVar, String modSetting)

	SetModSettingBool(modSetting, gVar.GetValue() as bool)

endFunction

Function SetGlobalInt(GlobalVariable gVar, String modSetting)

	gVar.SetValue(GetModSettingInt(modSetting) as float)

endFunction

Function SetGlobalBool(GlobalVariable gVar, String modSetting)

	gVar.SetValue(GetModSettingBool(modSetting) as float)

endFunction

int Function GetGlobalInt(GlobalVariable gVar, String modSettingStr)

	int modSetting = GetModSettingInt(modSettingStr)
	gVar.SetValue(modSetting as float)
	return modSetting

endFunction

bool Function GetGlobalBool(GlobalVariable gVar, String modSettingStr)

	bool modSetting = GetModSettingBool(modSettingStr)
	gVar.SetValue(modSetting as float)
	return modSetting

endFunction

Function SetEffectMode(GlobalVariable gVar, String[] settings)

	int i = 0
	int len = settings.Length	
	while i < len
		if GetModSettingBool(settings[i])
			gVar.SetValue(i as float)
			return
		endif
		i += 1
	endWhile

endFunction

bool Function SetEffectModeOnChange(GlobalVariable gVar, String a_ID, String[] settings)

	int i = 0
	int len = settings.Length	
	while i < len
		if a_ID == settings[i]
			if GetModSettingBool(settings[i])
				gVar.SetValue(i as float)
			endif
			return true
		endif
		i += 1
	endWhile
	
	return false

endFunction

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Function SetupEffectModes()

	fireModes = new String[5]
	fireModes[0] = "bRandom:FireEffects"
	fireModes[1] = "bAutomatic:FireEffects"
	fireModes[2] = "bChar:FireEffects"
	fireModes[3] = "bSkeletonize:FireEffects"
	fireModes[4] = "bVaporize:FireEffects"

	fireModesCR = new String[4]
	fireModesCR[0] = "bRandomCR:FireEffects"
	fireModesCR[1] = "bAutomaticCR:FireEffects"
	fireModesCR[2] = "bCharCR:FireEffects"
	fireModesCR[3] = "bVaporizeCR:FireEffects"

	frostModes = new String[5]
	frostModes[0] = "bRandom:FrostEffects"
	frostModes[1] = "bAutomatic:FrostEffects"
	frostModes[2] = "bFreeze:FrostEffects"
	frostModes[3] = "bIceBlock:FrostEffects"
	frostModes[4] = "bShatter:FrostEffects"

	frostModesCR = new String[3]
	frostModesCR[0] = "bAutomaticCR:FrostEffects"
	frostModesCR[1] = "bFreezeCR:FrostEffects"
	frostModesCR[2] = "bIceBlockCR:FrostEffects"

	shockModes = new String[5]
	shockModes[0] = "bRandom:ShockEffects"
	shockModes[1] = "bAutomatic:ShockEffects"
	shockModes[2] = "bSinge:ShockEffects"
	shockModes[3] = "bXRay:ShockEffects"
	shockModes[4] = "bExplosive:ShockEffects"

	drainModes = new String[4]
	drainModes[0] = "bRandom:DrainEffects"
	drainModes[1] = "bAutomatic:DrainEffects"
	drainModes[2] = "bAge:DrainEffects"
	drainModes[3] = "bDessicate:DrainEffects"

endFunction

Function SetGlobalRandomChance(int globalChance)

	SetModSettingInt("iRandomChanceNPC:Fire", globalChance)
	SetModSettingInt("iRandomChanceCR:Fire", globalChance)
	SetModSettingInt("iRandomChanceNPC:Frost", globalChance)
	SetModSettingInt("iRandomChanceCR:Frost", globalChance)
	SetModSettingInt("iRandomChanceNPC:Shock", globalChance)
	SetModSettingInt("iRandomChanceCR:Shock", globalChance)
	SetModSettingInt("iRandomChance:Drain", globalChance)
	SetModSettingInt("iRandomChanceNPC:Poison", globalChance)
	SetModSettingInt("iRandomChanceCR:Poison", globalChance)
	SetModSettingInt("iRandomChanceNPC:Acid", globalChance)
	SetModSettingInt("iRandomChanceCR:Acid", globalChance)
	SetModSettingInt("iRandomChance:Air", globalChance)
	SetModSettingInt("iRandomChanceNPC:Fear", globalChance)
	SetModSettingInt("iRandomChanceCR:Fear", globalChance)
	SetModSettingInt("iRandomChanceNPC:SoulTrap", globalChance)
	SetModSettingInt("iRandomChanceCR:SoulTrap", globalChance)
	SetModSettingInt("iRandomChanceNPC:Sun", globalChance)
	SetModSettingInt("iRandomChanceCR:Sun", globalChance)
	
endFunction

Function SendAfterDeathActiveEvent(GlobalVariable gVar, String a_ID, String a_type)

	if GetGlobalBool(gVar, a_ID)
		ModEvent.Send(ModEvent.Create("DeathEffectsActive" + a_type))
	else
		ModEvent.Send(ModEvent.Create("DeathEffectsInactive" + a_type))
	endif	

endFunction

Function SendEffectDamageChangeEvent(GlobalVariable gVar, String modSetting)

	int handle = ModEvent.Create("FEC_EffectDamageChange")
	ModEvent.PushBool(handle, GetGlobalBool(gVar, modSetting))
	ModEvent.Send(handle)

endFunction

Function ResetActor3DSingle()
	
	Actor target = GetCurrentConsoleRef() as Actor		
	if target
			
		if target != playerRef
			SendFECResetEvent(target, 0, false) ;perm
			SendFECResetEvent(target, 1, false) ;temp
			SendFECResetEvent(target, 3, false) ;frozenCol
		endif
						
		if ResetActor3D_FEC(target)
			Debug.Notification(target.GetLeveledActorBase().GetName() + " was successfully reset!")
		else
			Debug.Notification(target.GetLeveledActorBase().GetName() + " has no effects applied")
		endif											
	else		
		Debug.Notification("No NPC reference selected!")		
	endif
	
endFunction

Function ResetActor3DAll()

	Debug.Notification("[FEC] Loaded NPC reset started")
	
	Actor[] actorArray = GetActorsByProcessingLevel(0)
	int idx  = actorArray.Length
	
	int count = 0;
	int count01 = 0;

	while idx > 0			
		idx -= 1		
		Actor target = actorArray[idx]					
		if HasActiveSpell(target, FEC_DeathEffectsNPCAb)
			count01 += 1;
			
			SendFECResetEvent(target, 0, false) ;perm
			SendFECResetEvent(target, 1, false) ;temp
			SendFECResetEvent(target, 3, false) ;frozenCol
								
			if ResetActor3D_FEC(target)
				count += 1;					
			endif
												
		endif			
	endwhile
	
	Debug.Notification("[FEC] : " + count01 + " loaded NPCs reset; " + count + " had effects")

endFunction

Function ReapplyPoison()

	;-----------------------VARIABLES------------------------------
	int mode = FEC_PoisonMode.GetValue() as int
	bool opacityMode = FEC_PoisonOpacityAutoCalc.GetValue() as bool
	float opacityColor =  FEC_PoisonOpacityColor.GetValue()
	;--------------------------------------------------------------
	
	if !poisonSkin
		poisonSkin = FEC_ListSkinColor.GetAt(1) as ColorForm
	endif
	
	Actor[] array = GetActorsByProcessingLevel(0)
	int idx  = array.Length	
	while idx > 0	
		idx -= 1	
		Actor target = array[idx]		
		if target && GetTemporaryDeathEffect(target, 2)
			BlendColorWithSkinTone(target, poisonSkin, mode, opacityMode, opacityColor)   
		endif				
	endwhile
	
endFunction

Function UpdatePoisonColor()
	
	if !poisonSkin
		poisonSkin =  FEC_ListSkinColor.GetAt(1) as ColorForm
	endif
	
	int poisonColor = GetModSettingInt("iPoisonColor:Poison")
	poisonSkin.SetColor(poisonColor)
	
	SetModSettingString("sPoisonColorHex:Poison", IntToString(poisonColor, true))

	RefreshMenu() ;not working?
	
endFunction

Function UpdatePoisonColorHex()
	
	if !poisonSkin
		poisonSkin =  FEC_ListSkinColor.GetAt(1) as ColorForm
	endif
	
	int poisonColorHex = StringToInt(GetModSettingString("sPoisonColorHex:Poison"))
	if poisonColorHex != -1
		SetModSettingInt("iPoisonColor:Poison", poisonColorHex )
	
		poisonSkin.SetColor(poisonColorHex )
		
		RefreshMenu()
	endif

endFunction

Function LoadSettings()

;SETTINGS
	int globalRandomChance = GetGlobalInt(FEC_GlobalRandomChance, "iRandomChanceGlobal:Settings")
	if globalRandomChance != 100
		SetGlobalRandomChance(globalRandomChance)
	endif
	
	float deathCamDur
	if !GetGlobalBool(FEC_DeathCameraExtendToggle, "bAllowDeathCam:Settings")
		deathCamDur = 5.0
	else
		deathCamDur = GetModSettingInt("iDeathCamDur:Settings") as float
	endif
	FEC_DeathCameraDuration.SetValue(deathCamDur)
	SetGameSettingFloat("fPlayerDeathReloadTime", deathCamDur)
	
	SendAfterDeathActiveEvent(FEC_PostDeathEffectsNPCToggle, "bAllowAfterDeathNPC:Settings", "NPC")
	SendAfterDeathActiveEvent(FEC_PostDeathEffectsCRToggle, "bAllowAfterDeathCR:Settings","CR")
	
	SetGlobalBool(FEC_CollisionScaleToggle, "bAllowHitBoxScaling:Settings")
	
;FIRE
	SetGlobalBool(FEC_FirePCToggle, "bAffectPlayer:Fire")
	SetGlobalInt(FEC_FireRandomChance, "iRandomChanceNPC:Fire")
	SetGlobalInt(FEC_FireRandomChanceCr, "iRandomChanceCR:Fire")	
	
	SetEffectMode(FEC_FireModes, fireModes)
	
	int fireParticleCount = GetGlobalInt(FEC_FireFXParticleCount, "iFireParticleCount:FireVisuals")
	int iFireFX = FireFXS.length
	while iFireFX
		iFireFX -= 1
		SetParticleFullCount(FireFXS[iFireFX], fireParticleCount)
		SetParticlePersistentCount(FireFXS[iFireFX], fireParticleCount / 1.2857)
	endWhile
	SetGlobalBool(FEC_FireUndeadToggle, "bAllowVampCombust:FireVisuals")
	
	SetEffectMode(FEC_FireModesCr, fireModesCr)
	
	SetGlobalBool(FEC_CookedFoodToggle, "bAllowCookFood:FireImmersion")
	
	SendEffectDamageChangeEvent(FEC_FireDamageToggle, "bAllowFireDamage:FireImmersion")
	
	SetGlobalBool(FEC_FireBurnClothingToggle, "bAllowBurnClothing:FireImmersion")
	SetGlobalBool(FEC_FireRemoveClothingToggle, "bAllowRemoveClothing:FireImmersion")

;FROST
	SetGlobalBool(FEC_FrostPCToggle, "bAffectPlayer:Frost")
	SetGlobalInt(FEC_FrostRandomChance, "iRandomChanceNPC:Frost")
	SetGlobalInt(FEC_FrostRandomChanceCr, "iRandomChanceCR:Frost")
	
	SetEffectMode(FEC_FrostModes, frostModes)
	SetEffectMode(FEC_FrostModesCR, frostModesCR)
	
	SetGlobalBool(FEC_FrostShatterToggle,"bAllowFrozenShatter:FrostVisuals")
	SetGlobalBool(FEC_FrostShaderCrToggle,"bApplyIceShader:FrostVisuals")
	
;SHOCK	
	SetGlobalBool(FEC_ShockPCToggle, "bAffectPlayer:Shock")
	SetGlobalInt(FEC_ShockRandomChance,"iRandomChanceNPC:Shock")
	SetGlobalInt(FEC_ShockRandomChanceCr,"iRandomChanceCR:Shock")
	
	SetEffectMode(FEC_ShockModes, shockModes)
	
	SendEffectDamageChangeEvent(FEC_ShockDamageToggle, "bAllowShockDamage:ShockImmersion")
	
	SetGlobalBool(FEC_ShockWeaponDropToggle,"bAllowWeaponDrop:ShockImmersion")
	
;DRAIN	
	SetGlobalBool(FEC_DrainPCToggle, "bAffectPlayer:Drain")
	SetGlobalInt(FEC_DrainRandomChance,"iRandomChance:Drain")
		
	SetEffectMode(FEC_DrainModes, drainModes)
	
;POSION	
	SetGlobalBool(FEC_PoisonPCToggle, "bAffectPlayer:Poison")
	SetGlobalInt(FEC_PoisonRandomChance,"iRandomChanceNPC:Poison")
	SetGlobalInt(FEC_PoisonRandomChanceCr,"iRandomChanceCR:Poison")
	
	UpdatePoisonColor()
	UpdatePoisonColorHex()
	
	SetGlobalInt(FEC_PoisonMode, "iBlendModes:PoisonBlendMode")
	SetGlobalBool(FEC_PoisonOpacityAutoCalc, "bAutoSkinLuminanceDetect:PoisonBlendMode")
	FEC_PoisonOpacityColor.SetValue((GetModSettingInt("iPoisonIntensity:PoisonBlendMode") / 100) as float)

;MISC	
	SetGlobalInt(FEC_AirRandomChance,"iRandomChance:Air")
	
	SetGlobalBool(FEC_AcidPCToggle,"bAffectPlayer:Acid")
	SetGlobalInt(FEC_AcidRandomChance,"iRandomChanceNPC:Acid")
	SetGlobalInt(FEC_AcidRandomChanceCr,"iRandomChanceCR:Acid")
	
	SetGlobalBool(FEC_SoulAbsorbPCToggle,"bAffectPlayer:DragonSoulAbsorb")
	
	SetGlobalInt(FEC_FearRandomChance,"iRandomChanceNPC:Fear")
	SetGlobalInt(FEC_FearRandomChanceCr,"iRandomChanceCR:Fear")
	
	SetGlobalInt(FEC_SoulTrapRandomChance,"iRandomChanceNPC:SoulTrap")
	SetGlobalInt(FEC_SoulTrapRandomChanceCr,"iRandomChanceCR:SoulTrap")
	
	SetGlobalBool(FEC_SunPCToggle,"bAffectPlayer:Sun")
	SetGlobalInt(FEC_SunRandomChance,"iRandomChanceNPC:Sun")
	SetGlobalInt(FEC_SunRandomChanceCr,"iRandomChanceCR:Sun")
	
	int sunParticleCount = GetGlobalInt(FEC_SunFXParticleCount, "iSunParticleCount:Sun")
	SetParticleFullCount(FEC_SunDisintegrateFXS, sunParticleCount)
	SetParticlePersistentCount(FEC_SunDisintegrateFXS, sunParticleCount / 1.2857)
	
	SetGlobalBool(FEC_VampireLordPCToggle,"bAffectPlayer:VampireLord")
		
endFunction

Function SaveSettings()

;SETTINGS
	SetSettingInt(FEC_GlobalRandomChance, "iRandomChanceGlobal:Settings")
	
	SetSettingBool(FEC_DeathCameraExtendToggle, "bAllowDeathCam:Settings")
	SetSettingInt(FEC_DeathCameraDuration, "iDeathCamDur:Settings")
	
	SetSettingBool(FEC_PostDeathEffectsNPCToggle, "bAllowAfterDeathNPC:Settings")
	SetSettingBool(FEC_PostDeathEffectsCRToggle, "bAllowAfterDeathCR:Settings")
	
	SetSettingBool(FEC_CollisionScaleToggle, "bAllowHitBoxScaling:Settings")
	
;FIRE
	SetSettingBool(FEC_FirePCToggle, "bAffectPlayer:Fire")
	SetSettingInt(FEC_FireRandomChance, "iRandomChanceNPC:Fire")
	SetSettingInt(FEC_FireRandomChanceCr, "iRandomChanceCR:Fire")	
	
	SetEffectMode(FEC_FireModes, fireModes)
	
	SetSettingInt(FEC_FireFXParticleCount, "iFireParticleCount:FireVisuals")
	SetSettingBool(FEC_FireUndeadToggle, "bAllowVampCombust:FireVisuals")
	
	SetEffectMode(FEC_FireModesCr, fireModesCr)
	
	SetSettingBool(FEC_CookedFoodToggle, "bAllowCookFood:FireImmersion")	
	SetSettingBool(FEC_FireDamageToggle, "bAllowFireDamage:FireImmersion")	
	SetSettingBool(FEC_FireBurnClothingToggle, "bAllowBurnClothing:FireImmersion")
	SetSettingBool(FEC_FireBurnClothingToggle, "bAllowBurnClothing:FireImmersion")

;FROST
	SetSettingBool(FEC_FrostPCToggle, "bAffectPlayer:Frost")
	SetSettingInt(FEC_FrostRandomChance, "iRandomChanceNPC:Frost")
	SetSettingInt(FEC_FrostRandomChanceCr, "iRandomChanceCR:Frost")
	
	SetEffectMode(FEC_FrostModes, frostModes)
	SetEffectMode(FEC_FrostModesCR, frostModesCR)
	
	SetSettingBool(FEC_FrostShatterToggle,"bAllowFrozenShatter:FrostVisuals")
	SetSettingBool(FEC_FrostShaderCrToggle,"bApplyIceShader:FrostVisuals")
	
;SHOCK	
	SetSettingBool(FEC_ShockPCToggle, "bAffectPlayer:Shock")
	SetSettingInt(FEC_ShockRandomChance,"iRandomChanceNPC:Shock")
	SetSettingInt(FEC_ShockRandomChanceCr,"iRandomChanceCR:Shock")
	
	SetEffectMode(FEC_ShockModes, shockModes)
	
	SetSettingBool(FEC_ShockDamageToggle, "bAllowShockDamage:ShockImmersion")	
	SetSettingBool(FEC_ShockWeaponDropToggle,"bAllowWeaponDrop:ShockImmersion")
	
;DRAIN	
	SetSettingBool(FEC_DrainPCToggle, "bAffectPlayer:Drain")
	SetSettingInt(FEC_DrainRandomChance,"iRandomChance:Drain")
		
	SetEffectMode(FEC_DrainModes, drainModes)
	
;POSION	
	SetSettingBool(FEC_PoisonPCToggle, "bAffectPlayer:Poison")
	SetSettingInt(FEC_PoisonRandomChance,"iRandomChanceNPC:Poison")
	SetSettingInt(FEC_PoisonRandomChanceCr,"iRandomChanceCR:Poison")
	
	if poisonSkin	
		int poisonColor = poisonSkin.GetColor()
		SetSettingIntValue(poisonColor, "iPoisonColor:Poison")
		SetModSettingString("sPoisonColorHex:Poison", IntToString(poisonColor, true))
	endif
		
	SetSettingInt(FEC_PoisonMode, "iBlendModes:PoisonBlendMode")
	SetSettingBool(FEC_PoisonOpacityAutoCalc, "bAutoSkinLuminanceDetect:PoisonBlendMode")
	SetSettingIntValue((FEC_PoisonOpacityColor.GetValue() * 100) as int, "iPoisonIntensity:PoisonBlendMode")

;MISC	
	SetSettingInt(FEC_AirRandomChance,"iRandomChance:Air")
	
	SetSettingBool(FEC_AcidPCToggle,"bAffectPlayer:Acid")
	SetSettingInt(FEC_AcidRandomChance,"iRandomChanceNPC:Acid")
	SetSettingInt(FEC_AcidRandomChanceCr,"iRandomChanceCR:Acid")
	
	SetSettingBool(FEC_SoulAbsorbPCToggle,"bAffectPlayer:DragonSoulAbsorb")
	
	SetSettingInt(FEC_FearRandomChance,"iRandomChanceNPC:Fear")
	SetSettingInt(FEC_FearRandomChanceCr,"iRandomChanceCR:Fear")
	
	SetSettingInt(FEC_SoulTrapRandomChance,"iRandomChanceNPC:SoulTrap")
	SetSettingInt(FEC_SoulTrapRandomChanceCr,"iRandomChanceCR:SoulTrap")
	
	SetSettingBool(FEC_SunPCToggle,"bAffectPlayer:Sun")
	SetSettingInt(FEC_SunRandomChance,"iRandomChanceNPC:Sun")
	SetSettingInt(FEC_SunRandomChanceCr,"iRandomChanceCR:Sun")	
	SetSettingInt(FEC_SunFXParticleCount, "iSunParticleCount:Sun")
	
	SetSettingBool(FEC_VampireLordPCToggle,"bAffectPlayer:VampireLord")
	
	Debug.Notification("[FEC] MCM settings saved!")
		
endFunction

Function ResetSettings()

	SetModSettingInt("iRandomChanceGlobal:Settings", 100)
	SetGlobalRandomChance(100)
	
	SetModSettingBool("bAllowDeathCam:Settings", true)
	SetModSettingInt("iDeathCamDur:Settings", 10)
	SetModSettingBool("bAllowAfterDeathNPC:Settings", true)
	SetModSettingBool("bAllowAfterDeathNPC:Settings", true)
	SetModSettingBool("bAllowHitBoxScaling:Settings", true)
	
	SetModSettingBool("bAffectPlayer:Fire", true)
	SetModSettingBool("bRandom:FireEffects", false)
	SetModSettingBool("bAutomatic:FireEffects", true)
	SetModSettingBool("bChar:FireEffects", false)
	SetModSettingBool("bSkeletonize:FireEffects", false)
	SetModSettingBool("bVaporize:FireEffects", false)	
	SetModSettingInt("iFireParticleCount:FireVisuals", 100)	
	SetModSettingBool("bAllowVampCombust:FireVisuals", true)
	SetModSettingBool("bRandomCR:FireEffects", false)
	SetModSettingBool("bAutomaticCR:FireEffects", true)
	SetModSettingBool("bCharCR:FireEffects", false)
	SetModSettingBool("bVaporizeCR:FireEffects", false)
	SetModSettingBool("bAllowCookFood:FireImmersion", true)
	SetModSettingBool("bAllowFireDamage:FireImmersion", true)
	SetModSettingBool("bAllowBurnClothing:FireImmersion", true)
	SetModSettingBool("bAllowRemoveClothing:FireImmersion", false)
	
	SetModSettingBool("bAffectPlayer:Frost", true)
	SetModSettingBool("bRandom:FrostEffects", false)
	SetModSettingBool("bAutomatic:FrostEffects", true)
	SetModSettingBool("bFreeze:FrostEffects", false)
	SetModSettingBool("bShatter:FrostEffects", false)
	SetModSettingBool("bIceBlock:FrostEffects", false)
	SetModSettingBool("bAutomaticCR:FrostEffects", true)
	SetModSettingBool("bFreezeCR:FrostEffects", false)
	SetModSettingBool("bIceBlockCR:FrostEffects", false)
	SetModSettingBool("bAllowFrozenShatter:FrostVisuals", true)
	SetModSettingBool("bApplyIceShader:FrostVisuals", true)
	
	SetModSettingBool("bAffectPlayer:Shock", true)
	SetModSettingBool("bRandom:ShockEffects", false)
	SetModSettingBool("bAutomatic:ShockEffects", true)
	SetModSettingBool("bSinge:ShockEffects", false)
	SetModSettingBool("bXRay:ShockEffects", false)
	SetModSettingBool("bExplosive:ShockEffects", false)
	SetModSettingBool("bAllowShockDamage:ShockImmersion", true)
	SetModSettingBool("bAllowWeaponDrop:ShockImmersion", false)
	
	SetModSettingBool("bAffectPlayer:Drain", true)
	SetModSettingBool("bRandom:DrainEffects", true)
	SetModSettingBool("bAutomatic:DrainEffects", false)
	SetModSettingBool("bAge:DrainEffects", false)
	SetModSettingBool("bDessicate:DrainEffects", false)
	
	SetModSettingBool("bAffectPlayer:Poison", true)
	SetModSettingInt("iPoisonColor:Poison", 8323582)
	SetModSettingString("sPoisonColorHex:Poison", "0x7F01FE")
	SetModSettingInt("iBlendModes:PoisonBlendMode", 6)
	SetModSettingBool("bAutoSkinLuminanceDetect:PoisonBlendMode", true)
	SetModSettingInt("iPoisonIntensity:PoisonBlendMode", 80)
	
	SetModSettingBool("bAffectPlayer:Acid", true)
	SetModSettingBool("bAffectPlayer:DragonSoulAbsorb", true)
	SetModSettingBool("bAffectPlayer:Sun", true)
	SetModSettingInt("iSunParticleCount:Sun", 100)
	SetModSettingBool("bAffectPlayer:VampireLord", true)
	
	LoadSettings()
	
	Debug.Notification("[FEC] MCM settings reset!")
	
endFunction