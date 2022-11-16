Scriptname FEC_DeathEffectsNPCPerm extends activemagiceffect  
;created by powerofthree on Nexus  
;core effects

import Sound
import Utility

import po3_SKSEFunctions
import po3_Events_AME
import FEC_Utility
 
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							PROPERTIES
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Actor property playerRef auto
Actor victim

ActorBase victimBase

;SUN
Activator property FEC_AshPile auto
;ACID
Activator property FEC_AcidAshPile auto

Armor property ArmorBriarHeart auto
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

Art refractionArt

bool globalLock = false
bool triggerLock = false

bool allowEffectsWhenDead = true
bool allowFireDamage = false
bool allowShockDamage = false
bool allowFrozenShatter = false
bool allowCollisionScaling = false

bool permanentEffect = false
bool aged = false
bool charred = false
bool skeletonized = false
bool frozen = false
bool paralyzed = false
bool drained = false
bool xray = false

bool isReanimated = false

bool reanimated = false
bool resurrected = false
bool sleeping = false

bool disintegrateProof = false

bool isVIP = false

;ICE
Container property FEC_FrostContainerCorpse auto
Container property FEC_FrostContainerSatchel auto
Container property FEC_ShockContainerSatchel auto
Container property FEC_FrostContainerPouch auto
Container property FEC_ShockContainerPouch auto

;DRAIN
ColorForm greyHair

;DRAIN
EffectShader property FEC_FireSmokeFXS auto 
EffectShader property FEC_DrainSmokeFXS auto 
EffectShader property FEC_DrainDisintegrateSkinFXS auto
;FIRE
EffectShader property FEC_FlyingBitsFXS auto  
EffectShader property FEC_FlyingBitsLiteFXS auto  
EffectShader property FEC_SmokeFXS auto 
EffectShader property FEC_FireFXS auto 
EffectShader property FEC_FireHeavyFXS auto 
EffectShader property FEC_FireCharFXS auto
EffectShader property FEC_FireCharInstantFXS auto
EffectShader property FEC_SootFXS auto
;CHARRED
EffectShader property FEC_FireDisintegrateSkinFXS auto
;SKELETON
EffectShader property FEC_FireDisintegrateSkinLongFXS auto
;VAPORISE
EffectShader property FEC_FireDisintegrateFXS auto
;ICE 
EffectShader property FEC_FrostFXS auto  
EffectShader property FEC_FrostIceFormFXS auto  
;SHOCK
EffectShader property FEC_ShockFXS auto 
EffectShader property FEC_ShockExpFXS auto 
EffectShader property FEC_SootInstantFXS auto
EffectShader property FEC_SunDisintegrateFXS auto
;SUN
EffectShader property FEC_AcidDisintegrateFXS auto 
;WATER
EffectShader property FEC_SteamFXS auto 

;ICE
Explosion property FEC_FrostExplosion auto  
;SHOCK
Explosion property FEC_ShockExplosion auto  

float scaleDiff

;ICE
Form [] inventoryArray
Form [] notEquippedArray

Formlist property FEC_ListFoodRaw auto
Formlist property FEC_ListFoodCooked auto
;FEAR
Formlist property FEC_ListHairColor auto
;	
Formlist property FEC_ListKeywordMagic auto	
Formlist property FEC_ListKeywordMagicFrozen auto
Formlist property FEC_ListKeywordWeapon auto
;
Formlist property DisintegrationMainImmunityList auto

;TRIGGER CHANCE
GlobalVariable property FEC_AcidRandomChance auto
GlobalVariable property FEC_DrainRandomChance auto
GlobalVariable property FEC_FireRandomChance auto
GlobalVariable property FEC_FrostRandomChance auto
GlobalVariable property FEC_ShockRandomChance auto
GlobalVariable property FEC_SunRandomChance auto
;OPTIONS
;FIRE
GlobalVariable property FEC_FireModes auto
GlobalVariable property FEC_FireBurnClothingToggle auto
GlobalVariable property FEC_FireRemoveClothingToggle auto
GlobalVariable property FEC_FireUndeadToggle auto
GlobalVariable property FEC_FireDamageToggle auto
;ICE
GlobalVariable property FEC_FrostModes auto
GlobalVariable property FEC_FrostShatterToggle auto
;SHOCK
GlobalVariable property FEC_ShockModes auto
GlobalVariable property FEC_ShockDamageToggle auto
GlobalVariable property FEC_ShockWeaponDropToggle auto
;DRAIN
GlobalVariable property FEC_DrainModes auto
;
GlobalVariable property FEC_CookedFoodToggle auto
GlobalVariable property FEC_PostDeathEffectsNPCToggle auto
GlobalVariable property FEC_DizonaInstalled auto
GlobalVariable property FEC_CollisionScaleToggle auto

;FIRE
ImpactDataSet property FEC_FireImpactSet auto
;SHOCK
ImpactDataSet property FEC_BloodImpactSet auto
ImpactDataSet property FEC_ShockImpactSet auto
;ICE
ImpactDataSet property FEC_FrostImpactSet auto
;WATER
ImpactDataSet property FEC_WaterImpactSet auto

int magicType = -1

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

int randomChance
int randomEffect
int projectileType = -1;
int spellLevel = 0;

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
int kAGE = 2
int kDESSICATE = 3

int kEFFECT_Reset = -2
int kEFFECT_None = -1
int kEFFECT_Charred = 0
int kEFFECT_Skeletonized = 1
int kEFFECT_Drained = 2
int kEFFECT_Frozen = 3

int kEFFECT_Aged = 0
int kEFFECT_XRayed = 1

int kRESET_PERM = 0
int kRESET_TEMP = 1
int kRESET_FROZEN_ACTOR = 2
int kRESET_FROZEN_COL = 3

int[] property slotsToSkip auto

int victimRaceType
int victimSex

;FIRE
Keyword property MagicDamageFire auto
Keyword property ActorTypeUndead auto
;ICE
Keyword property MagicDamageFrost auto
;SHOCK
Keyword property MagicDamageShock auto

;ICE
MiscObject[] property FrostBodyParts auto
;SHOCK
MiscObject[] property ShockBodyParts auto

;ICE
ObjectReference bodyParts
ObjectReference inventoryContainer

Race victimRace
;FIRE/ICE
Race property KhajiitRace auto
Race property KhajiitRaceVampire auto
Race property ArgonianRace auto
Race property ArgonianRaceVampire auto
;AGE
Race property ElderRace auto

Spell property FEC_FireCorpseSpell auto
Spell property FEC_ShockCorpseSpell auto

String victimName
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

VisualEffect property FEC_GenericTriggerEffect auto
VisualEffect property FEC_FireHeatRefractionEffect auto

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;=======================================
;			EMPTY STATE 
;=======================================

Event OnDeathEffectsActiveNPC() ;EMPTY
endEvent
	
Event OnDeathEffectsInactiveNPC() ;EMPTY
endEvent

Event OnEffectDamageChange(bool abChange)
endEvent

Event OnActorReanimateStart(Actor akTarget, Actor akCaster) ;EMPTY
endEvent

Event OnActorReanimateStop(Actor akTarget, Actor akCaster) ;EMPTY
endEvent

Event OnActorResurrected(Actor akTarget, bool abResetInventory) ;EMPTY
endEvent

Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
endEvent

Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)
endEvent

Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
endEvent

;=======================================
;			DEFAULT STATE 
;=======================================
	
Auto State Default
	
	Event OnEffectStart(Actor akTarget, Actor akCaster)
		
		victim = akTarget
		
		if victim
												
			;---------------VARIABLES---------------														
			victimBase = victim.GetLeveledActorBase()
			victimRace = victimBase.GetRace()			
			
			; block all effects if target is immune to disintegration
			if victimBase && DisintegrationMainImmunityList.HasForm(victimBase) || victimRace && DisintegrationMainImmunityList.HasForm(victimRace)
				GoToState("EMPTY")
				return
			endif
			
			victimName = victimBase.GetName()					
			greyHair = FEC_ListHairColor.GetAt(0) as ColorForm		
						
			disintegrateProof = IsDisintegrateProof(victim)
			isReanimated = GetActorState(victim) == 4
			refractionArt = GetArtObject(FEC_FireHeatRefractionEffect)			
			;----------------------------------------
																												
			if GetPermanentDeathEffect(victim, kEFFECT_None)
																							
				if GetPermanentDeathEffect(victim, kEFFECT_Reset)
				
					ClearPermEffectSerialization()	
					ClearTempEffectSerialization()

					ResetActor3D_FEC(victim, false)
				
				elseif GetPermanentDeathEffect(victim, kEFFECT_Frozen)
					
					SendFECResetEvent(victim, kRESET_FROZEN_COL, false)
					ResetFrozen()
					
				else
					
					if isReanimated									
						reanimated = true						
						GoToState("Sleeping")					
					else				
						GoToState("Done")						
					endif
					
				endif
				
				return
				
			elseif GetTemporaryDeathEffect(victim, kEFFECT_None)
														
				if isReanimated															
					reanimated = true						
					GoToState("Sleeping")					
				else				
					GoToBuffer()					
				endif
				
				return
				
			else
			
				ResetActor3D_FEC(victim, false)
				GoToBuffer()
				
			endif
						
		endif
			
	endEvent
	
	Event OnDying(Actor akKiller)
	
		if victim
					
			;-----------------------VARIABLES-------------									
			int [] deathEffect = GetCauseOfDeath(victim, 0)
			magicType = deathEffect[0]
			spellLevel = deathEffect[1]
			projectileType = deathEffect[2]			
			
			victimSex = victimBase.GetSex()
			
			isVIP = IsVIP(victim)
			
			if victimRace == ArgonianRace || victimRace == ArgonianRaceVampire
				victimRaceType = 0
			elseif victimRace == KhajiitRace || victimRace == KhajiitRaceVampire
				victimRaceType = 1
			else
				victimRaceType = 2
			endif
			
			randomChance = GenerateRandomInt(0,100)
			;---------------------------------------------
																					
			if magicType == kFIRE && randomChance <= FEC_FireRandomChance.GetValue() as int
															
				if !IsActorInWater(victim) && !IsActorUnderWater(victim)
					BurnEffect()
				else
					SteamEffect()
				endif
							
			elseif magicType == kFIRE_FROST && randomChance <= FEC_FireRandomChance.GetValue() as int	
																		
				SteamEffect(applyFrost = true)																						

			elseif magicType == kFIRE_SHOCK && randomChance <= FEC_FireRandomChance.GetValue() as int
															
				if !IsActorInWater(victim) && !IsActorUnderWater(victim)
					BurnEffect(applyShock = true)
				else
					SteamEffect(applyShock = true)
				endif
				
			elseif magicType == kFROST && randomChance <= FEC_FrostRandomChance.GetValue() as int
																	
				FreezeEffect()									

			elseif magicType ==	kFROST_SHOCK && randomChance <= FEC_FrostRandomChance.GetValue() as int	
																		
				FreezeEffect(applyShock = true)									

			elseif magicType ==	kSHOCK_FROST && randomChance <= FEC_ShockRandomChance.GetValue() as int	
								
				ShockEffect(applyFrost = true)									

			elseif magicType == kSHOCK && randomChance <= FEC_ShockRandomChance.GetValue() as int	
												
				ShockEffect()									

			elseif magicType == kDRAIN && randomChance <= FEC_DrainRandomChance.GetValue() as int	
						
				DrainEffect()
			
			elseif magicType == kDRAIN_FROST && randomChance <= FEC_DrainRandomChance.GetValue() as int	
						
				DrainEffect(applyFrost = true)	

			elseif magicType == kDRAIN_SHOCK && randomChance <= FEC_DrainRandomChance.GetValue() as int	
						
				DrainEffect(applyShock = true)				
		
			elseif magicType == kSUN && !disintegrateProof && !isVIP && randomChance <= FEC_SunRandomChance.GetValue() as int	
								
				GoToState("DoneForReal")
				CreateAshPileEffect(victim, FEC_SunDisintegrateFXS, FEC_AshPile)	
			
			;/elseif magicType == kACID && !disintegrateProof && !isVIP && randomChance <= FEC_AcidRandomChance.GetValue() as int	
								
				GoToState("DoneForReal")
				CreateAshPileEffect(victim, FEC_AcidDisintegrateFXS, FEC_AcidAshPile, 0.5, 1.30)/;
					
			else
											
				GoToBuffer()

			endif
								
		endif
		
	endEvent
	
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
	endEvent	
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY		
	endEvent

	Event OnTrigger(ObjectReference akReference) ;EMPTY
	endEvent

	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
	endEvent
		
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent
		
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent
	
endState

;=======================================
;		EFFECT BUFFER STATE
;=======================================			

State EffectBuffer
		
	Event OnBeginState()
					
		UnregisterComplexEvents()
		
		RegisterForActorReanimateStart(self)
		RegisterForActorResurrected(self)

	endEvent
	
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim	
			reanimated = true					
			UnregisterForActorReanimateStart(self)		
		endif
		
	endEvent
	
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim		
			resurrected = true
			UnregisterForActorResurrected(self)		
		endif
	
	endEvent

	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
	endEvent

	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY			
	endEvent

	Event OnTrigger(ObjectReference akReference) ;EMPTY			
	endEvent
	
	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
	endEvent
		
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent
	
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference) ;EMPTY			
	endEvent
	
endState

;=======================================
;		FIRE BUFFER STATE
;=======================================			

State FireBuffer
		
	Event OnBeginState()
	
		;---------------VARIABLES---------------
		triggerLock = false
						
		allowFireDamage = FEC_FireDamageToggle.GetValue() as bool
		allowCollisionScaling = FEC_CollisionScaleToggle.GetValue() as bool
				
		scaleDiff = GetScaleDiff(victim)
		;---------------------------------------
		
		UnregisterComplexEvents()		
		
		RegisterForModEvent("FEC_EffectDamageChange", "OnEffectDamageChange")		
		RegisterForActorReanimateStart(self)
		RegisterForActorResurrected(self)
					
	endEvent
	
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim
			reanimated = true		
			UnregisterForActorReanimateStart(self)		
		endif
		
	endEvent	
	
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim				
			resurrected = true	
			UnregisterForActorResurrected(self)			
		endif
	
	endEvent
		
	Event OnEffectDamageChange(bool abChange)	
		allowFireDamage = abChange		
	endEvent

	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
	endEvent
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY			
	endEvent
	
	Event OnTrigger(ObjectReference akReference)
	
		if !allowFireDamage || triggerLock
			return
		endif
		
		triggerLock = true
		
		Actor akActor = akReference as Actor
										
		if akActor && !akActor.HasSpell(FEC_FireCorpseSpell)
			FEC_FireCorpseSpell.Cast(victim, akActor)
		endif
		
		triggerLock = false
		
	endEvent
	
	Event OnTriggerLeave(ObjectReference akReference)

		if akReference 
		
			Actor akActor = akReference as Actor
			
			if akActor			
				akActor.DispelSpell(FEC_FireCorpseSpell)				
			endif
			
		endif

	endEvent
			
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
							
		Armor curArmor = akBaseObject as Armor

		if curArmor
			EquipArmorInSlot_Equip(curArmor)										
		endif					
			
	endEvent
	
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference)
								
		Armor curArmor = akBaseObject as Armor
	
		if curArmor		
			EquipArmorInSlot_Unequip(curArmor)					
		endif			
				
	endEvent
	
endState

;=======================================
;		FROZEN BUFFER STATE
;=======================================			

State FrozenBuffer
		
	Event OnBeginState()
	
		;---------------VARIABLES---------------		
		allowFrozenShatter = FEC_FrostShatterToggle.GetValue() as bool
		;---------------------------------------
		
		UnregisterComplexEvents()
		UnregisterForActorReanimateStop(self)	
		
		RegisterForMagicEffectApplyEx(self, FEC_ListKeywordMagicFrozen, true)
		RegisterForHitEventEx(self, akSourceFilter = FEC_ListKeywordWeapon)	
		
		RegisterForFECReset(self, kRESET_PERM)
		RegisterForFECReset(self, kRESET_FROZEN_ACTOR)
		
		RegisterForActorReanimateStart(self)
		RegisterForActorResurrected(self)
		
		CheckFrozenEffectSerialization()
											
		if !victim.IsAIEnabled()
			FreezeActor(victim, 0, true)
		endif
		
		if victimName == ""
			victimName = victimBase.GetName()
		endif
		victim.SetDisplayName("")
				
		FEC_FrostFXS.Play(victim)
					
	endEvent
	
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
							
		if akActor == victim	
			if aiType == kRESET_PERM				
				ResetFrozen()				
			elseif aiType == kRESET_FROZEN_ACTOR								
				if !abReset3D			
					ResetFrozen()					
				else			
					GoToState("DoneForReal")				
					ShatterActor(true)					
				endif
			endif			
		endif
	
	endEvent
	
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim				
			SendFECResetEvent(victim, kRESET_FROZEN_COL, false)				
			ResetFrozen()		
			UnregisterForActorReanimateStart(self)						
			GoToState("Sleeping")				
		endif
		
	endEvent
		
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim		
			SendFECResetEvent(victim, kRESET_FROZEN_COL, false)				
			ResetFrozen()						
			UnregisterForActorResurrected(self)			
		endif
	
	endEvent
	
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied)
				
		SendFECResetEvent(victim, kRESET_FROZEN_COL, false)				
		ResetFrozen()	
	
	endEvent
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)
						
		SendFECResetEvent(victim, kRESET_FROZEN_COL, allowFrozenShatter)
		
		if allowFrozenShatter																
			GoToState("DoneForReal")				
			Wait(0.1)
			ShatterActor(true)				
		else							
			ResetFrozen()					
		endif
		
	endEvent

	Event OnTrigger(ObjectReference akReference) ;EMPTY
	endEvent

	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY	 
	endEvent

	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent
	
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent	
	
endState

;=======================================
;		SHOCK BUFFER STATE
;=======================================			

State ShockBuffer
		
	Event OnBeginState()
	
		;---------------VARIABLES---------------
		triggerLock = false
		
		allowShockDamage = FEC_ShockDamageToggle.GetValue() as bool	
		allowCollisionScaling = FEC_CollisionScaleToggle.GetValue() as bool
		
		scaleDiff = GetScaleDiff(victim)
		;----------------------------------------
		
		UnregisterComplexEvents()	
		
		RegisterForModEvent("FEC_EffectDamageChange", "OnEffectDamageChange")		
		RegisterForActorReanimateStart(self)
		RegisterForActorResurrected(self)
							
	endEvent
	

	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim		
			reanimated = true		
			UnregisterForActorReanimateStart(self)			
		endif
		
	endEvent
	
	
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim		
			resurrected = true				
			UnregisterForActorResurrected(self)			
		endif
	
	endEvent
		
	Event OnEffectDamageChange(bool abChange)	
		allowShockDamage = abChange		
	endEvent

	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
	endEvent
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY			
	endEvent
	
	Event OnTrigger(ObjectReference akReference)
	
		if !allowShockDamage || triggerLock
			return
		endif
		
		triggerLock = true
		
		Actor akActor = akReference as Actor										
		if akActor && !akActor.HasSpell(FEC_ShockCorpseSpell)
			FEC_ShockCorpseSpell.Cast(victim, akActor)
		endif
		
		triggerLock = false
		
	endEvent
	
	Event OnTriggerLeave(ObjectReference akReference)

		if akReference 	
			Actor target = akReference as Actor			
			if target			
				target.DispelSpell(FEC_ShockCorpseSpell)				
			endif		
		endif

	endEvent
		
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference)
	endEvent
		
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
	endEvent	
	
endState

;=======================================
;		DRAIN BUFFER STATE
;=======================================			

State DrainBuffer
		
	Event OnBeginState()
				
		UnregisterComplexEvents()
		
		RegisterForActorReanimateStart(self)
		RegisterForActorResurrected(self)
			
	endEvent
	
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim		
			reanimated = true		
			UnregisterForActorReanimateStart(self)			
		endif
		
	endEvent
		
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim		
			resurrected = true				
			UnregisterForActorResurrected(self)			
		endif
	
	endEvent
		
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
	endEvent
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY			
	endEvent
	
	Event OnTrigger(ObjectReference akReference) ;EMPTY
	endEvent
	
	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
	endEvent	
		
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
	
		Armor curArmor = akBaseObject as Armor
		if curArmor
			EquipArmorInSlot_Equip(curArmor)										
		endif
	
	endEvent
	
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference)
					
		Armor curArmor = akBaseObject as Armor
		if curArmor		
			EquipArmorInSlot_Unequip(curArmor)					
		endif	
	
	endEvent
	
endState

;=======================================
;			AFTER DEATH
;=======================================
						
State AfterDeath 

	Event OnBeginState()
					
		if resurrected
			FullReset()
			return
		endif
		
		;---------------VARIABLES---------------
		globalLock = false
		sleeping = false
		
		victimSex = victimBase.GetSex()
		
		isVIP = IsVIP(victim)
		
		if victimRace == ArgonianRace || victimRace == ArgonianRaceVampire
			victimRaceType = 0
		elseif victimRace == KhajiitRace || victimRace == KhajiitRaceVampire
			victimRaceType = 1
		else
			victimRaceType = 2
		endif
		;---------------------------------------
						
		UnregisterComplexEvents()
		UnregisterForActorReanimateStop(self)	
			
		RegisterForModEvent("DeathEffectsInactiveNPC", "OnDeathEffectsInactiveNPC")
		RegisterForMagicEffectApplyEx(self, FEC_ListKeywordMagic, true)
		RegisterForFECReset(self, kRESET_TEMP)
		RegisterForActorReanimateStart(self)
		RegisterForActorResurrected(self)
							
		if CheckTempEffectSerialization() && aged
			ReapplyAge()
		endif
							
	endEvent
	
	Event OnDeathEffectsInactiveNPC()	
		sleeping = true
		GoToState("Sleeping")			
	endEvent
		
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
			
		if akActor == victim && aiType == kRESET_TEMP			
			aged = false
			RemoveTemporaryDeathEffect(victim, kEFFECT_Aged)					
			ResetActor3D_FEC(victim, false)							
		endif
			
	endEvent
		
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim					
			FullReset()					
			UnregisterForActorResurrected(self)			
		endif
	
	endEvent
		
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim		
			reanimated = true
			GoToState("Sleeping")			
			UnregisterForActorReanimateStart(self)
		
		endif
	
	endEvent

	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied)
		
		if globalLock
			return
		endif
		
		globalLock = true
					
		if IsValidMagicEffect(akEffect, akCaster, victim)
		
			spellLevel = akEffect.GetSkillLevel()
			randomChance = GenerateRandomInt(0,100)	
				
			if akEffect.HasKeywordString("po3_MagicDamageSun") && !disintegrateProof && !isVIP && randomChance <= FEC_SunRandomChance.GetValue() as int
																										
				GoToState("DoneForReal")
				CreateAshPileEffect(victim, FEC_SunDisintegrateFXS, FEC_AshPile)
				
			;/elseif IsAcidEffect(akEffect) && !disintegrateProof && !isVIP && randomChance <= FEC_AcidRandomChance.GetValue() as int	
								
				GoToState("DoneForReal")
				CreateAshPileEffect(victim, FEC_AcidDisintegrateFXS, FEC_AcidAshPile, 0.5, 1.30)/;
			
			elseif akEffect.HasKeyword(MagicDamageFire) && randomChance <= FEC_FireRandomChance.GetValue() as int
													
				if !IsActorInWater(victim) && !IsActorUnderWater(victim)
					BurnEffect() 
				else
					SteamEffect()
				endif
			
			elseif akEffect.HasKeyword(MagicDamageFrost) && randomChance <= FEC_FrostRandomChance.GetValue() as int
																											
				Projectile akProjectile = akEffect.GetProjectile()
				if akProjectile	
					projectileType = GetProjectileType(akProjectile)
				endif					
				FreezeEffect()
									
			elseif akEffect.HasKeyword(MagicDamageShock) && randomChance <= FEC_ShockRandomChance.GetValue() as int

				ShockEffect()
				
			endif
				
		endif
					
		globalLock = false
			
	endEvent
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)  ;EMPTY		
	endEvent

	Event OnTrigger(ObjectReference akReference) ;EMPTY
	endEvent

	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY	 
	endEvent
		
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
								
		Armor curArmor = akBaseObject as Armor
		if curArmor && aged				
			EquipSkinTextureSet(curArmor.GetSlotMask())																
		endif					
	
	endEvent
	
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference)
								
		Armor curArmor = akBaseObject as Armor
		if curArmor && aged	
			EquipSkinTextureSet(curArmor.GetSlotMask())									
		endif					

	endEvent
		
endState

;=======================================
;			SLEEPING STATE
;=======================================

State Sleeping

	Event OnBeginState()
								
		if resurrected
			FullReset()
			return
		endif
		
		;---------------VARIABLES---------------
		globalLock = true
		triggerLock = true
		;---------------------------------------
				
		UnregisterComplexEvents()
		
		RegisterForModEvent("DeathEffectsActiveNPC", "OnDeathEffectsActiveNPC")			
		RegisterForFECReset(self, kRESET_PERM)		
		RegisterForActorReanimateStop(self)		
		RegisterForActorResurrected(self)
			
		if CheckPermEffectSerialization()
			if charred		
				ReapplyCharred()			
			elseif skeletonized		
				ReapplySkeleton()		
			elseif drained		
				ReapplyDrained()
			endif
			SetSkinAlpha(victim, 0.0)
		endif
		
		if CheckTempEffectSerialization() && aged
			ReapplyAge()
		endif
																	
	endEvent
	
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
		
		if akActor == victim && aiType == kRESET_PERM					
			ClearTempEffectSerialization()
			ClearPermEffectSerialization()								
			if abReset3D
				ResetActor3D_FEC(victim)	
			endif						
		endif
			
	endEvent
	
	Event OnDeathEffectsActiveNPC()	
		if !reanimated
			if permanentEffect
				GoToState("Done")
			else
				GoToState("AfterDeath")
			endif
		endif		
	endEvent
	
	Event OnActorReanimateStop(Actor akTarget, Actor akCaster)
		
		if akTarget == victim && !sleeping			
			reanimated = false				
			if permanentEffect
				GoToState("Done")
			else
				GoToState("AfterDeath")
			endif				
			UnregisterForActorReanimateStop(self)			
		endif
	
	endEvent
		
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim		
			FullReset()				
			UnregisterForActorResurrected(self)					
		endif
	
	endEvent
	
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
	endEvent

	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY	
	endEvent	

	Event OnTrigger(ObjectReference akReference) ;EMPTY			
	endEvent
	
	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
	endEvent
		
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
							
		Armor curArmor = akBaseObject as Armor

		if curArmor													
			if charred || skeletonized || drained
				EquipArmorInSlot_Equip(curArmor)
			elseif aged
				EquipSkinTextureSet(curArmor.GetSlotMask())		
			endif							
		endif							
	
	endEvent
	
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference)
								
		Armor curArmor = akBaseObject as Armor			
		if curArmor																								
			if charred || skeletonized || drained
				EquipArmorInSlot_Unequip(curArmor)						
			elseif aged
				EquipSkinTextureSet(curArmor.GetSlotMask())		
			endif			
		endif				

	endEvent
				
endState

;=======================================
;			DONE STATE
;=======================================

State Done 

	Event OnBeginState()
		
		if reanimated		
			GoToState("Sleeping")
			return			
		endif
		if resurrected
			FullReset()
			return
		endif
							
		;---------------VARIABLES---------------
		globalLock = false
		sleeping = false
		
		isVIP = IsVIP(victim)
		;----------------------------------------
			
		UnregisterComplexEvents()
		
		RegisterForModEvent("DeathEffectsInactiveNPC", "OnDeathEffectsInactiveNPC")
		RegisterForMagicEffectApplyEx(self, FEC_ListKeywordMagic, true)		
		RegisterForFECReset(self, kRESET_PERM)		
		RegisterForActorReanimateStart(self)
		RegisterForActorResurrected(self)
		
		if GetTemporaryDeathEffect(victim, kEFFECT_None)
			SendFECResetEvent(victim, kRESET_TEMP, false)
		endif	
		ClearTempEffectSerialization()
		
		if CheckPermEffectSerialization()				
			if charred		
				ReapplyCharred()			
			elseif skeletonized	
				ReapplySkeleton()		
			elseif drained	
				ReapplyDrained()
			endif
			SetSkinAlpha(victim, 0.0)
		endif
														
	endEvent
	
	Event OnDeathEffectsInactiveNPC()
		
		sleeping = true
		GoToState("Sleeping")
		
	endEvent

	Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
				
		if akActor == victim && aiType == kRESET_PERM										
			ClearPermEffectSerialization()						
			if abReset3D
				ResetActor3D_FEC(victim, true)		
			endif				
			GoToBuffer()						
		endif
			
	endEvent
			
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim		
			reanimated = true
			GoToState("Sleeping")			
			UnregisterForActorReanimateStart(self)		
		endif
	
	endEvent
	
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim		
			FullReset()		
			UnregisterForActorResurrected(self)					
		endif
	
	endEvent
		
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied)
			
		if globalLock
			return
		endif
		
		globalLock = true
										
		if IsValidMagicEffect(akEffect, akCaster, victim)
		
			spellLevel = akEffect.GetSkillLevel()
			randomChance = GenerateRandomInt(0,100)
				
			if akEffect.HasKeywordString("po3_MagicDamageSun") && !disintegrateProof && !isVIP && randomChance <= FEC_SunRandomChance.GetValue() as int
						
				GoToState("DoneForReal")
				CreateAshPileEffect(victim, FEC_SunDisintegrateFXS, FEC_AshPile)	

			;/elseif IsAcidEffect(akEffect) && !disintegrateProof && !isVIP && randomChance <= FEC_AcidRandomChance.GetValue() as int	
								
				GoToState("DoneForReal")
				CreateAshPileEffect(victim, FEC_AcidDisintegrateFXS, FEC_AcidAshPile, 0.5, 1.30)/;
			
			elseif akEffect.HasKeyword(MagicDamageFire) && randomChance <= FEC_FireRandomChance.GetValue() as int	
											
				FEC_FrostFXS.Stop(victim)
				FEC_FrostIceFormFXS.Stop(victim)	
					
				if HasArtObject(victim, refractionArt) == 0 && !IsActorInWater(victim) && !IsActorUnderWater(victim)
					if drained				
						BurnDrainEffect()				
					else				
						BurnEffect()					
					endif
				endif
							
			elseif akEffect.HasKeyword(MagicDamageFrost) && (randomChance <= FEC_FrostRandomChance.GetValue() as int) && HasEffectShader(victim, FEC_FrostFXS, true) == 0	

				FEC_FrostFXS.Play(victim)	
			
			elseif akEffect.HasKeyword(MagicDamageShock) && randomChance <= FEC_ShockRandomChance.GetValue() as int && HasEffectShader(victim, FEC_ShockFXS) == 0
	
				if !permanentEffect						
					ShockEffect()						
				else																			
					float time = GenerateRandomFloat(1.0,1.50)
					FEC_GenericTriggerEffect.Play(victim, time)
					FEC_ShockFXS.Play(victim, time)
					if GetPermanentDeathEffect(victim, kEFFECT_Drained)
						FEC_SootFXS.Play(victim)							
					endif							
				endif		
																	
			endif
				
		endif
						
		globalLock = false
			
	endEvent

	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY	
	endEvent	

	Event OnTrigger(ObjectReference akReference) ;EMPTY			
	endEvent
	
	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
	endEvent
		
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
						
		Armor curArmor = akBaseObject as Armor
		if curArmor && permanentEffect
			EquipArmorInSlot_Equip(curArmor)									
		endif
		
	endEvent
	
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference)
						
		Armor curArmor = akBaseObject as Armor
		if curArmor && permanentEffect
			EquipArmorInSlot_Unequip(curArmor)																																			
		endif
		
	endEvent
			
endState

;=======================================
;			DONE FOR REAL STATE
;=======================================

State DoneForReal

	Event OnBeginState()	
	
		UnregisterComplexEvents()
		UnregisterForActorReanimateStart(self)	
		UnregisterForActorReanimateStop(self)			
		UnregisterForActorResurrected(self)		
		
		ClearPermEffectSerialization()	
				
		if GetTemporaryDeathEffect(victim, kEFFECT_None)
			SendFECResetEvent(victim, kRESET_TEMP, false)
		endif		
		ClearTempEffectSerialization()
					
		GoToState("Default")
										
	endEvent
	
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
	endEvent
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY	
	endEvent	
	
	Event OnTrigger(ObjectReference akReference) ;EMPTY
	endEvent
	
	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
	endEvent		
	
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent		
	
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent
		
endState

;=======================================
;			EMPTY STATE
;=======================================

State EMPTY

	Event OnBeginState()											
	endEvent
	
	Event OnDying(Actor akKiller)
	endEvent
	
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
	endEvent	
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY		
	endEvent

	Event OnTrigger(ObjectReference akReference) ;EMPTY
	endEvent

	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
	endEvent
		
	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent
		
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent
		
endState

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;						EFFECT FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;========================================================														
;	[1]				BURN EFFECT                                                                                           
;========================================================

Function BurnEffect(bool applyShock = false)

	GoToState("FireBuffer")
	
	;---------------VARIABLES---------------
	int effectType = GetEffectType(kFIRE)
	randomEffect = GenerateRandomInt(1,3)	
	bool vampMode = FEC_FireUndeadToggle.GetValue() as bool
	;---------------------------------------
		
	if !disintegrateProof && !isVIP && ((effectType == kRANDOM && randomEffect == 3) || (effectType == kAUTOMATIC && spellLevel == 100) || effectType == kVAPOURISE || \
		(victim.HasKeyword(ActorTypeUndead) && vampMode))
		GoToState("DoneForReal")
		CreateAshPileEffect(victim, FEC_FireDisintegrateFXS, FEC_AshPile)	
		return				
	endif 
	
	if spellLevel >= 75
		FEC_FireHeavyFXS.Play(victim, GenerateRandomFloat(4.0,5.0))	
	else
		FEC_FireFXS.Play(victim, GenerateRandomFloat(3.0,5.0))	
	endif
	
	if paralyzed
		FreezeActor(victim, 1, false)
	endif
	
	if applyShock	
		FEC_ShockFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))		
	endif
		
	FEC_FireHeatRefractionEffect.Play(victim)	
	if allowCollisionScaling && allowFireDamage
		ScaleTriggerArt(victim, scaleDiff, "FECRefractionFX")
	endif
	
	victim.PlayImpactEffect(FEC_FireImpactSet, "MagicEffectsNode", 0.0, 0.0, -1.0, 128.0, true, true)
	
	if !charred && !skeletonized
					
		if effectType == kAUTOMATIC
		
			if spellLevel >= 75									
				SetSkeletonNew()						
			else																
				SetCharredNew()				
			endif
			
		elseif effectType == kCHAR || (effectType == kRANDOM && randomEffect == 1)
					
			SetCharredNew()
						
		elseif effectType == kSKELE || (effectType == kRANDOM && randomEffect == 2)
								
			SetSkeletonNew()	
			
		endif
		
		CookFood()
					
	else
		
		Wait(GenerateRandomFloat(2.50, 3.50))
		
	endif
											
	FEC_FireSmokeFXS.Play(victim, GenerateRandomFloat(8.0, 12.0)); smoke effect
	
	Wait(7.0)
	
	FEC_FireHeatRefractionEffect.Stop(victim)
	
	GoToState("Done")
	
endFunction

;========================================================														
;	[2]				STEAM EFFECT                                                                                            
;========================================================

Function SteamEffect(bool applyFrost = false, bool applyShock = false)	
			
	GoToState("EffectBuffer")
	
	FEC_SteamFXS.Play(victim, GenerateRandomFloat(8.0,10.0))  ; smoke effect
	
	FEC_FireCharInstantFXS.Play(victim)
	
	if applyShock		
		FEC_ShockFXS.Play(victim, GenerateRandomFloat(6.0,8.0))		
	endif
		
	Wait(1.0)
	
	ImpactDataSet impact;
	if applyFrost	
		impact = FEC_WaterImpactSet
	else
		impact = FEC_FireImpactSet	
	endif
	
	victim.PlayImpactEffect(impact, "MagicEffectsNode", 0.0, 0.0, -1.0, 512.0, true, true)	
		
	CookFood()
			
	GoToBuffer()
					
endFunction

;========================================================													
;	[3]				FREEZE EFFECT                                                                                           
;========================================================

Function FreezeEffect(bool applyShock = false)

	;---------------VARIABLES---------------
	int effectType = GetEffectType(kFROST)
	randomEffect = GenerateRandomInt(1,3)
	;---------------------------------------
	
	if isVIP || isReanimated || (effectType == kRANDOM && randomEffect == 2) || effectType == kICEBLOCK ;ice block
			
		GoToState("EffectBuffer")
		
		if !victim.HasKeywordString("ImmuneParalysis")	
			FreezeActor(victim, 1, true)
			paralyzed = true
		endif
		
		victim.PlaceAtMe(FEC_FrostExplosion)
		
		StopAllShaders(victim)
		
		Wait(0.1)
		
		FEC_FrostIceFormFXS.Play(victim)

		if applyShock		
			FEC_ShockFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))			
		endif
		
		victim.PlayImpactEffect(FEC_FrostImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 512.0, true, true)	
				
		GoToState("Done")
			
	elseif (effectType == kRANDOM && randomEffect == 1) || (effectType == kAUTOMATIC && projectileType != 1) || effectType == kFREEZE  ;freeze
															
		GoToState("EffectBuffer")		
		
		FreezeActor(victim, 0, true)
							
		victim.PlaceAtMe(FEC_FrostExplosion)
		
		StopAllShaders(victim)	
		FEC_FrostFXS.Play(victim)
																																											
		KillNoWait(victim)
		
		inventoryContainer = victim.PlaceAtMe(FEC_FrostContainerCorpse)						
		if inventoryContainer
						
			if WaitFor3DLoad(inventoryContainer)
				inventoryContainer.SplineTranslateToRefNode(victim, "NPC Pelvis [Pelv]",1.0,10000.0)
			endif
		
			inventoryArray = AddAllItemsToArray(victim, true, false, false)
			notEquippedArray = AddAllEquippedItemsToArray(victim)		
			int iIndex = inventoryArray.Length
			while iIndex
				iIndex -= 1
				victim.RemoveItem(inventoryArray[iIndex], 1, true, inventoryContainer)  
			endwhile		
			int iIndex2 = notEquippedArray.Length
			while iIndex2
				iIndex2 -= 1
				inventoryContainer.AddItem(notEquippedArray[iIndex2], 1)  
			endwhile
			inventoryContainer.SetActorOwner(playerRef.GetActorBase())	
			
		endif
		
		victim.PlayImpactEffect(FEC_FrostImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 512.0, true, true)
		
		if applyShock		
			FEC_ShockFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))			
		endif	
		
		permanentEffect = true
		frozen = true
		AssignPermanentDeathEffect(victim, kEFFECT_Frozen)
						
		GoToState("FrozenBuffer")
		
	elseif (effectType == kRANDOM && randomEffect == 3) || (effectType == kAUTOMATIC && projectileType == 1) || effectType == kSHATTER ;shatter
			
		GoToState("DoneForReal")
		
		FreezeActor(victim, 0, true)
		FEC_FrostFXS.Play(victim)
			
		Wait(GenerateRandomFloat(0.20,0.40))
		
		ShatterActor(false)
									
	endif
			
endFunction
 
;========================================================													
;	[4]				SHOCK EFFECT                                                                                         
;========================================================

Function ShockEffect(bool applyFrost = false)

	;---------------VARIABLES---------------
	int effectType = GetEffectType(kSHOCK)
	randomEffect = GenerateRandomInt(1,3)
	bool allowWeaponDrop = FEC_ShockWeaponDropToggle.GetValue() as bool	
	;---------------------------------------
	
	if (effectType == kRANDOM && randomEffect == 1) || (effectType == kAUTOMATIC && spellLevel < 25 ) || effectType == kCHAR ; fry
		
		GoToState("ShockBuffer")
			
		FEC_ShockFXS.Play(victim)

		if applyFrost		
			victim.PlaceAtMe(FEC_FrostExplosion)
		endif
		
		FEC_GenericTriggerEffect.Play(victim)
		if allowCollisionScaling && allowShockDamage
			ScaleTriggerArt(victim, scaleDiff, "FECHitBoxGeneric")
		endif
		
		victim.PlayImpactEffect(FEC_ShockImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 128.0, true, true)
				
		victim.PushActoraway(victim, 0.0)
		
		if allowWeaponDrop
			if victim.GetEquippedWeapon() != None
				ObjectReference droppedWeapon = victim.DropObject(victim.GetEquippedWeapon())
				droppedWeapon.SetActorOwner(playerRef.GetActorBase())
			endif		
			if victim.GetEquippedShield() != None
				ObjectReference droppedShield = victim.DropObject(victim.GetEquippedShield())	
				droppedShield.SetActorOwner(playerRef.GetActorBase())
			endif
		endif
		
		if applyFrost				
			FEC_FrostFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))			
		else
			FEC_SootFXS.Play(victim)
			FEC_FireCharFXS.Play(victim)			
		endif
					
		ApplyNormalisedHavokImpulse()
			
		CookFood()
				
		FEC_SmokeFXS.Play(victim, 7.0)   ; smoke effect 
	
		FEC_SootFXS.Play(victim)
		FEC_FireCharFXS.Play(victim)
		
		FEC_GenericTriggerEffect.Stop(victim)
		FEC_ShockFXS.Stop(victim) ; stop spark shader
				
		GoToBuffer()
		
	elseif disintegrateProof || (effectType == kRANDOM && randomEffect == 2) || (effectType == kAUTOMATIC && spellLevel < 75) || effectType == kXRAY ; xray
		
		GoToState("ShockBuffer")
					
		xray = true
		AssignTemporaryDeathEffect(victim, kEFFECT_XRayed)
				
		wait(0.1)
		
		EquipHead(victim, FEC_ShockXRayHead, None, false, false)
		victim.EquipItem(FEC_ShockXRayBody)
					
		FEC_ShockExpFXS.Play(victim)
		
		if applyFrost
			victim.PlaceAtMe(FEC_FrostExplosion)
		endif
		
		FEC_GenericTriggerEffect.Play(victim)
		if allowCollisionScaling && allowShockDamage
			ScaleTriggerArt(victim, scaleDiff, "FECHitBoxGeneric")
		endif
			
		victim.PlayImpactEffect(FEC_ShockImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 128.0, true, true)
				
		victim.PushActoraway(victim, 0.0)
		
		if allowWeaponDrop
			if victim.GetEquippedWeapon() != None
				ObjectReference droppedWeapon = victim.DropObject(victim.GetEquippedWeapon())
				droppedWeapon.SetActorOwner(playerRef.GetActorBase())
			endif		
			if victim.GetEquippedShield() != None
				ObjectReference droppedShield = victim.DropObject(victim.GetEquippedShield())	
				droppedShield.SetActorOwner(playerRef.GetActorBase())
			endif
		endif
		
		if applyFrost		
			FEC_FrostFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))			
		endif
						
		ApplyNormalisedHavokImpulse()
		
		FEC_SootInstantFXS.Play(victim)		
		FEC_GenericTriggerEffect.Stop(victim)
		
		FEC_ShockExpFXS.Stop(victim)
		
		Wait(0.75)
		
		victim.UnequipItem(FEC_ShockXRayHead)
		victim.UnequipItem(FEC_ShockXRayBody)
		
		xray = false		
		AssignTemporaryDeathEffect(victim, kEFFECT_None)
				
		CookFood()
			
		FEC_SmokeFXS.Play(victim, 7.0)   ; smoke effect 
				
		GoToBuffer()
		  	
	elseif (effectType == kRANDOM && randomEffect == 3) || (effectType == kAUTOMATIC && spellLevel >= 75) || effectType == kEXPLODE ; explode
		
		GoToState("DoneForReal")
		
		FEC_ShockExpFXS.Play(victim)
		
		Wait(GenerateRandomFloat(0.15,0.30))
		
		bodyParts = PlaceBodyParts(victim, ShockBodyParts, victimRaceType, victimSex)		
		if WaitFor3DLoad(bodyParts)
			SetupBodyPartGeometry(bodyParts, victim)
		endif
		
		victim.PlaceAtMe(FEC_ShockExplosion)				
					
		victim.SetCriticalStage(victim.CritStage_DisintegrateStart)

		victim.SetAlpha(0.0)
		
		if victimName == ""
			victimName = victimBase.GetName()
		endif
		PlaceContainer(FEC_ShockContainerPouch, FEC_ShockContainerSatchel)
		
		victim.PlayImpactEffect(FEC_BloodImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 512.0, true, true)		
				
		victim.SetCriticalStage(victim.CritStage_DisintegrateEnd)
								
	endif
	
endFunction

;========================================================												
;	[5]				DRAIN EFFECT                                                                                              
;========================================================

Function DrainEffect(bool applyFrost = false, bool applyShock = false)
	
	;---------------VARIABLES---------------	
	int effectType = GetEffectType(kDRAIN)
	randomEffect = GenerateRandomInt(1,2)		
	;---------------------------------------
	
	if victimRace == ElderRace || effectType == kAUTOMATIC && spellLevel >= 50 || effectType == kRANDOM && randomEffect == 2 || effectType == kDESSICATE
		
		GoToState("DrainBuffer")
		
		FEC_DrainSmokeFXS.Play(victim, GenerateRandomFloat(4.0, 6.0))
		
		if applyShock		
			FEC_ShockFXS.Play(victim, GenerateRandomFloat(5.0, 7.0))				
		endif
		if applyFrost
			FEC_FrostFXS.Play(victim, GenerateRandomFloat(5.0, 7.0))
		endif

		Wait(GenerateRandomFloat(1.50,2.50))
				
		EquipHead(victim, FEC_DrainHead, FEC_DrainHeadDecap, true, true)			
		EquipUnderSkin(victim, 32, FEC_DrainBody00, FEC_DrainBody01)
		EquipUnderSkin(victim, 33, FEC_DrainHands)
		EquipUnderSkin(victim, 37, FEC_DrainFeet)
				
		permanentEffect = true
		drained = true		
		AssignPermanentDeathEffect(victim, kEFFECT_Drained)
				
		FEC_DrainDisintegrateSkinFXS.Play(victim)
		
		SetHeadPartAlpha(victim, 0, 0.0);mouth		
		SetHeadPartAlpha(victim, 2, 0.0);eyes
		SetHeadPartAlpha(victim, 6, 0.0);eyebrows
		ToggleChildNode(victim, "Skinned Decal Node", true)

		SetHairColor(victim, greyHair)
			
		Wait(4.50)
	
		SetSkinAlpha(victim, 0.0)
		
		GoToState("Done")
				
	elseif effectType == kRANDOM && randomEffect == 1 || effectType == kAUTOMATIC && spellLevel && spellLevel < 50 || effectType == kAGE
	
		GoToState("EffectBuffer")

		if applyShock					
			float time = GenerateRandomFloat(6.0, 8.0)
			FEC_ShockFXS.Play(victim, time)
			FEC_SmokeFXS.Play(victim, time)			
		endif
		if applyFrost
			FEC_FrostFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))
		endif		
			
		AgeVictim()
		
		aged = true		
		AssignTemporaryDeathEffect(victim, kEFFECT_Aged)
		
		GoToBuffer()
							
	endif
		  	
endFunction
 
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;						EFFECT HELPER FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;========================================================														
;	[1]				BURN EFFECT                                                                                           
;========================================================

Function SetCharredNew()
			
	Wait(GenerateRandomFloat(1.50,2.50))
					
	ToggleChildNode(victim, "Skinned Decal Node", true)

	EquipHead(victim, FEC_FireCharHead, FEC_FireCharHeadDecap, true, true)	
	EquipUnderSkin(victim, 32, FEC_FireCharBody00, FEC_FireCharBody01)
	EquipUnderSkin(victim, 33, FEC_FireCharHands)
	EquipUnderSkin(victim, 37, FEC_FireCharFeet)

	Wait(1.0)

	permanentEffect = true
	charred = true
	aged = false

	AssignPermanentDeathEffect(victim, kEFFECT_Charred)
	AssignTemporaryDeathEffect(victim, kEFFECT_None)

	FEC_SootFXS.Play(victim)
	FEC_FireDisintegrateSkinFXS.Play(victim)
						
	FEC_FlyingBitsLiteFXS.Play(victim, 8.0)	; playing flying bits 			
	FEC_FlyingBitsFXS.Play(victim, 6.0) 
					
	Wait(GenerateRandomFloat(2.0,3.0))
			
	BurnSkinAndClothing(true)
							
endFunction

Function SetSkeletonNew()
	
	Wait(GenerateRandomFloat(2.0,3.0))
	
	ToggleChildNode(victim, "Skinned Decal Node", true)
	
	EquipHead(victim,FEC_FireSkeleHead, None, false, true)			
	EquipUnderSkin(victim, 32, FEC_FireSkeleBody00, FEC_FireSkeleBody01)
	EquipUnderSkin(victim, 33, FEC_FireSkeleHands)
	EquipUnderSkin(victim, 37, FEC_FireSkeleFeet)
	
	Wait(1.0)
		
	permanentEffect = true
	skeletonized = true
	aged = false
	
	AssignPermanentDeathEffect(victim, kEFFECT_Skeletonized)
	AssignTemporaryDeathEffect(victim, kEFFECT_None)
		
	FEC_SootFXS.Play(victim)
	FEC_FireDisintegrateSkinLongFXS.Play(victim)	
							
	FEC_FlyingBitsLiteFXS.Play(victim, 8.0)	; playing flying bits 	  		
	FEC_FlyingBitsFXS.Play(victim, 6.0) 
			
	Wait(GenerateRandomFloat(3.0,3.5))
		
	BurnSkinAndClothing(true)
			
endFunction

Function BurnDrainEffect()

	GoToState("FireBuffer")
	
	if spellLevel >= 75
		FEC_FireHeavyFXS.Play(victim, GenerateRandomFloat(3.0,5.0))	
	else
		FEC_FireFXS.Play(victim, GenerateRandomFloat(2.0,4.0))	
	endif 
		
	FEC_FireHeatRefractionEffect.Play(victim, 13.0)
	if allowCollisionScaling && allowFireDamage
		ScaleTriggerArt(victim, scaleDiff, "FECRefractionFX")
	endif
				
	victim.PlayImpactEffect(FEC_FireImpactSet, "MagicEffectsNode", 0.0, 0.0, -1.0, 128.0, true, true)
	
	Wait(GenerateRandomFloat(2.0,3.0))
		
	EquipHead(victim,FEC_FireCharHead, FEC_FireCharHeadDecap, true, true)
	
	EquipUnderSkin(victim, 32, FEC_FireCharBody00, FEC_FireCharBody01)
	EquipUnderSkin(victim, 33, FEC_FireCharHands)
	EquipUnderSkin(victim, 37, FEC_FireCharFeet)	
	
	drained = false
	charred = true
	AssignPermanentDeathEffect(victim, kEFFECT_Charred)
				
	Wait(GenerateRandomFloat(1.00,2.00))
	
	RemoveEffectsNotOfType(victim, 0)
	
	BurnSkinAndClothing(false)
						
	FEC_SootFXS.Play(victim)	
	FEC_FireSmokeFXS.Play(victim, 7.0)   ; smoke effect
		
endFunction

Function BurnSkinAndClothing(bool setSkinInvs)

	ToggleChildNode(victim, "BSFaceGenNiNodeSkinned", true)
	VaporizeUnderwear(victim)
	
	bool unequipClothing = FEC_FireBurnClothingToggle.GetValue() as bool
	bool removeClothing = FEC_FireRemoveClothingToggle.GetValue() as bool
	if (unequipClothing	|| removeClothing) && !victim.IsEquipped(ArmorBriarHeart)
		Wait(GenerateRandomFloat(1.00,1.50))
		if unequipClothing
			UnequipAllOfType(victim, 2, slotsToSkip)
		else
			RemoveArmorOfType(victim, 2, slotsToSkip, true)
		endif
		Utility.Wait(0.1)
		VaporizeUnderwear(victim)
	endif
	
	if setSkinInvs
		SetSkinAlpha(victim, 0.0)
	endif
	
endFunction

Function ReapplyCharred()

	if WaitFor3DLoad(victim)
							
		RemoveEffectsNotOfType(victim, 0)
					
		EquipHead(victim, FEC_FireCharHead, FEC_FireCharHeadDecap, true, true)	
		
		EquipUnderSkin(victim, 32, FEC_FireCharBody00, FEC_FireCharBody01)
		EquipUnderSkin(victim, 33, FEC_FireCharHands)
		EquipUnderSkin(victim, 37, FEC_FireCharFeet)	
		
		ToggleHairWigs(victim, true)
		
		VaporizeUnderwear(victim)
		
		if (HasEffectShader(victim, FEC_SootInstantFXS, true) == 0)
			FEC_SootInstantFXS.Play(victim)
		endif		
		
	endif
	
endFunction

Function ReapplySkeleton()

	if WaitFor3DLoad(victim) 
								
		RemoveEffectsNotOfType(victim, 0)											
		
		EquipHead(victim, FEC_FireSkeleHead, None, false, true)				
		EquipUnderSkin(victim, 32, FEC_FireSkeleBody00, FEC_FireSkeleBody01)
		EquipUnderSkin(victim, 33, FEC_FireSkeleHands)
		EquipUnderSkin(victim, 37, FEC_FireSkeleFeet)
		
		ToggleHairWigs(victim, true)

		VaporizeUnderwear(victim)		
		
	endif

endFunction

;========================================================														
;	[2]				FROST EFFECT                                                                                           
;========================================================

Function ShatterActor(bool shatterFrozen)

	if disintegrateProof
		if shatterFrozen
			ResetFrozen()
		else
			FreezeActor(victim, 0, false)
		endif
		return
	endif
	
	bodyParts = PlaceBodyParts(victim, FrostBodyParts, victimRaceType, victimSex)
	if WaitFor3DLoad(bodyParts)
		Wait(0.25)
	endif
	
	if shatterFrozen	
		ResetFrozen()	
	else	
		FreezeActor(victim, 0, false)	
	endif
					
	victim.SetCriticalStage(victim.CritStage_DisintegrateStart)
		
	victim.SetAlpha(0.0)
	
	PlaceContainer(FEC_FrostContainerPouch, FEC_FrostContainerSatchel)
					
	victim.PlayImpactEffect(FEC_FrostImpactSet, "NPC R Foot [Rft]", 0.0, 0.0, -1.0, 512.0, true, true)	
	
	victim.SetCriticalStage(victim.CritStage_DisintegrateEnd)

endFunction

Function ResetFrozen()

	FreezeActor(victim, 0, false)
	
	if victim.GetDisplayName() == ""
		if victimName == ""
			victimName = victimBase.GetName()
		endif	
		victim.SetDisplayName(victimName)
	endif

	frozen = false	
	permanentEffect = false
	
	AssignPermanentDeathEffect(victim, kEFFECT_Frozen)
	
	GoToBuffer()
	
endFunction


;========================================================														
;	[2]				DRAIN EFFECT                                                                                         
;========================================================

Function AgeVictim()

	if victimRaceType == 0
		ReplaceFaceTextureSet(victim, SkinHeadMaleArgonianOld, SkinHeadFemaleArgonianOld, 3)
	elseif victimRaceType == 1
		ReplaceFaceTextureSet(victim, SkinHeadMaleKhajiitOld, SkinHeadFemaleKhajiitOld, 3)
	else
		ReplaceFaceTextureSet(victim, SkinHeadMaleOld, SkinHeadFemaleOld, 1)			
		if FEC_DizonaInstalled.GetValue() == 0.0
			ReplaceSkinTextureSet(victim, SkinBodyMaleOld, SkinBodyFemaleOld, 0x00000004, 1)
			if !victim.GetEquippedArmorInSlot(33)
				ReplaceSkinTextureSet(victim, SkinHandMaleOld, SkinHandFemaleOld, 0x00000008, 1)
			endif			
			if !victim.GetEquippedArmorInSlot(37)
				ReplaceSkinTextureSet(victim, SkinBodyMaleOld, SkinBodyFemaleOld, 0x00000080, 1)
			endif
		endif
	endif
	
	SetHairColor(victim, GreyHair)
	
endFunction

Function ReapplyDrained()

	if WaitFor3DLoad(victim)
							
		RemoveEffectsNotOfType(victim, 1)
				
		EquipHead(victim, FEC_DrainHead, FEC_DrainHeadDecap, true, true)	
		
		EquipUnderSkin(victim, 32, FEC_DrainBody00, FEC_DrainBody01)
		EquipUnderSkin(victim, 33, FEC_DrainHands)
		EquipUnderSkin(victim, 37, FEC_DrainFeet)
		
		SetHairColor(victim, greyHair)
									
	endif
	
endFunction

Function ReapplyAge()

	if WaitFor3DLoad(victim) && !HasNiExtraData(victim, "FEC_SKINTXST - ")
		RemoveEffectsNotOfType(victim, 3)						
		AgeVictim()		
	endif
				
endFunction

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                         REAPPLY AND RESET FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Function FullReset()

	;------VARIABLES---------------
	globalLock = false
	triggerLock = false
	
	reanimated = false
	resurrected = false
	sleeping = false
	;-------------------------------
		
	ClearPermEffectSerialization()	
	ClearTempEffectSerialization()		
	
	if frozen
		FreezeActor(victim, 0, false)
		frozen = false
	endif
	if paralyzed
		FreezeActor(victim, 1, false)
		paralyzed = false
	endif
	
	inventoryContainer = None
	BodyParts = None	
	
	ResetActor3D_FEC(victim, true)
	
	UnregisterForAllFECResets(self)
	
	GoToState("Default")
		
endFunction

bool Function CheckPermEffectSerialization()

	bool result = false
				
	if charred
		AssignPermanentDeathEffect(victim, kEFFECT_Charred)
	elseif GetPermanentDeathEffect(victim, kEFFECT_Charred)
		charred = true
		result = true
	endif

	if skeletonized
		AssignPermanentDeathEffect(victim, kEFFECT_Skeletonized)
	elseif GetPermanentDeathEffect(victim, kEFFECT_Skeletonized)
		skeletonized = true
		result = true
	endif

	if drained
		AssignPermanentDeathEffect(victim, kEFFECT_Drained)
	elseif GetPermanentDeathEffect(victim, kEFFECT_Drained)
		drained = true
		result = true
	endif
	
	if result
		permanentEffect = true
	endif
	
	return permanentEffect || charred || skeletonized || drained
	
endFunction

bool Function CheckTempEffectSerialization()

	if aged
		AssignTemporaryDeathEffect(victim, kEFFECT_Aged)
	elseif GetTemporaryDeathEffect(victim, kEFFECT_Aged)
		aged = true
	endif
	
	return aged
	
endFunction

bool Function CheckFrozenEffectSerialization()

	if frozen
		AssignPermanentDeathEffect(victim, kEFFECT_Frozen)
	elseif GetPermanentDeathEffect(victim, kEFFECT_Frozen)
		frozen = true
	endif

	return frozen
	
endFunction

Function ClearPermEffectSerialization()

	permanentEffect = false
	charred = false					
	skeletonized = false	
	drained = false
	
	AssignPermanentDeathEffect(victim, kEFFECT_None)
	
endFunction

Function ClearTempEffectSerialization()

	aged = false	
	xray = false
	
	AssignTemporaryDeathEffect(victim, kEFFECT_None)
	
endFunction

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                       EFFECT SELECT FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Function GetEffectType(int type)

	if type == kFIRE 			
		return FEC_FireModes.GetValue() as int				
	elseif type == kFROST 
		return FEC_FrostModes.GetValue() as int		
	elseif type == kSHOCK
		return FEC_ShockModes.GetValue() as int	
	elseif type == kDRAIN
		return FEC_DrainModes.GetValue() as int		
	endif
	
	return 1 ;AUTO
	
endFunction	

Function GoToBuffer()

	if !victim.IsDead()	
		GoToState("Default")
		return		
	endif
	
	if !FEC_PostDeathEffectsNPCToggle.GetValue() as bool	
		sleeping = true
		GoToState("Sleeping")		
	else		
		GoToState("AfterDeath")						
	endif

endFunction	

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                         MISC FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Function EquipArmorInSlot_Unequip(Armor curArmor)

	if IsFormInMod(curArmor, "FEC.esp")
		return
	endif
	
	int slotMask = curArmor.GetSlotMask()
	
	if Math.LogicalAnd(slotMask, 0x00000001) || Math.LogicalAnd(slotMask, 0x00000002) ;head/hair	
		if charred
			EquipHead(victim, FEC_FireCharHead, FEC_FireCharHeadDecap, true, true)		
		elseif skeletonized
			EquipHead(victim,FEC_FireSkeleHead, None, false, true)	
			ToggleChildNode(victim, "BSFaceGenNiNodeSkinned", true)
		elseif drained
			EquipHead(victim, FEC_DrainHead, FEC_DrainHeadDecap, true, true)
		endif
	elseif Math.LogicalAnd(slotMask, 0x00000008);hands				
		if charred
			victim.EquipItem(FEC_FireCharHands)
		elseif skeletonized
			victim.EquipItem(FEC_FireSkeleHands)
		elseif drained
			victim.EquipItem(FEC_DrainHands)
		endif
	elseif Math.LogicalAnd(slotMask, 0x00000080);feet				
		if charred
			victim.EquipItem(FEC_FireCharFeet)
		elseif skeletonized
			victim.EquipItem(FEC_FireSkeleFeet)
		elseif drained
			victim.EquipItem(FEC_DrainFeet)
		endif
	elseif Math.LogicalAnd(slotMask, 0x00000004)							
		if charred
			victim.EquipItem(FEC_FireCharBody01)
			victim.RemoveItem(FEC_FireCharBody00)
		elseif skeletonized
			victim.EquipItem(FEC_FireSkeleBody01)
			victim.RemoveItem(FEC_FireSkeleBody00)
		elseif drained
			victim.EquipItem(FEC_DrainBody01)
			victim.RemoveItem(FEC_DrainBody00)
		endif
	endif
			
endFunction

Function EquipArmorInSlot_Equip(Armor curArmor)
							
	if victim.Is3DLoaded() && !IsFormInMod(curArmor, "FEC.esp")
					
		if charred
			ReapplyCharred()	
		elseif skeletonized
			ReapplySkeleton()	
		elseif drained
			ReapplyDrained()	
		endif
		
	endif
		
endFunction

Function EquipSkinTextureSet(int slotMask)

	if victim.Is3DLoaded()
		if Math.LogicalAnd(slotMask, 0x00000008) ;hands			
			ReplaceSkinTextureSet(victim, SkinHandMaleOld, SkinHandFemaleOld, 0x00000008, 1)
		elseif Math.LogicalAnd(slotMask, 0x00000080)
			ReplaceSkinTextureSet(victim, SkinBodyMaleOld, SkinBodyFemaleOld, 0x00000080, 1)
		elseif Math.LogicalAnd(slotMask, 0x00000004)	
			ReplaceSkinTextureSet(victim, SkinBodyMaleOld, SkinBodyFemaleOld, 0x00000004, 1)
		endif
	endif
	
endFunction

Function PlaceContainer(Container akPouch, Container akSatchel)

	ObjectReference inventory
	
	int numItems = victim.GetNumItems()
	if numItems > 0
		if numItems <=  4		
			inventory = victim.placeAtMe(akPouch)
			inventory.SetDisplayName(victimName + "'s Pouch", true)
		else
			inventory = victim.placeAtMe(akSatchel)
			inventory.SetDisplayName(victimName + "'s Satchel", true)
		endif		
		inventory.SetActorOwner(PlayerRef.GetActorBase())		
		victim.RemoveAllItems(inventory, abRemoveQuestItems = true)
	endif
	
endFunction

Function ApplyNormalisedHavokImpulse()
				
	float normalisedMass = 1 - (victim.GetMass() - 50) / (200 - 50)	
	int randomTime = GenerateRandomInt(15, 25)	
	
	int time = 0	
	while time < randomTime
		if victim.Is3DLoaded()
			victim.ApplyHavokImpulse(0.0, 0.0, 1.0, normalisedMass * GenerateRandomInt(100, 150))
		endif
		Wait(GenerateRandomFloat(0.50,1.0))
		time += 1
	endwhile
	
endFunction

Function CookFood()

	if FEC_CookedFoodToggle.GetValue() as bool
		CookMeat(victim, FEC_ListFoodRaw, FEC_ListFoodCooked)
	endif
	
endFunction

Function GoToStateDebug(String aState)

	if GetState() != aState
		debug.trace(victimName + "[" + IntToString(victim.GetFormID(), true) + "] : " + GetState() + "->" + aState)
	endif
	GoToState(aState)

endFunction

Function UnregisterComplexEvents()

	if self
		UnregisterForAllModEvents()
		UnregisterForAllMagicEffectApplyEx(self)
		UnregisterForAllHitEventsEx(self)
		UnregisterForAllFECResets(self)
	endif

endFunction