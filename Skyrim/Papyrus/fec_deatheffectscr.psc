Scriptname FEC_DeathEffectsCR extends activemagiceffect  
;animal effects

import game
import debug
import utility
import Sound

import po3_SKSEFunctions ;helper functions
import po3_Events_AME
import FEC_Utility
 
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;							PROPERTIES

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Actor property playerRef auto
Actor victim

Actorbase victimBase

;SUN
Activator property FEC_AshPile auto
;ACID
Activator property FEC_AcidAshPile auto

Art fireRefractionArt
Art genericTriggerArt

bool globalLock = false
bool triggerLock = false

bool allowEffectsWhenDead = true

bool allowFireDamage = false
bool allowShockDamage = false
bool allowCollisionScaling = true

bool isBipedal = false
bool isHumanoid = false
bool isDraugr = false
bool isSkeleton = false
bool isReanimated = false

bool permanentEffect = false
bool charred = false
bool frozen = false
bool frozenShader = false
bool paralyzed = false
bool reanimated = false
bool sleeping = false

bool disintegrateProof = false

bool isVIP = false

;ICE
Container property FEC_FrostContainerCorpseCr auto
Container property FEC_FrostContainerCorpseCrQuad auto
Container property FEC_FrostContainerPouch auto

EffectShader property FEC_FlyingBitsFXS auto  
EffectShader property FEC_FlyingBitsLiteFXS auto  
EffectShader property FEC_SmokeFXS auto 
EffectShader property FEC_FireSmokeFXS auto 
EffectShader property FEC_SteamFXS auto 
EffectShader property FEC_FireFXS auto 
EffectShader property FEC_FireHeavyFXS auto 
EffectShader property FEC_FireCharCrFXS auto
EffectShader property FEC_FireCharFXS auto
EffectShader property FEC_SootFXS auto
EffectShader property FEC_FireDisintegrateFXS auto
;
EffectShader property FEC_FrostFXS auto 
EffectShader property FEC_FrostIceFormFXS auto 
;
EffectShader property FEC_ShockFXS auto 
;SUN
EffectShader property FEC_SunDisintegrateFXS auto
;
EffectShader property FEC_AcidDisintegrateFXS auto 

Explosion property FEC_FrostExplosion auto

float scaleDiff

Faction[] property bipedalCreatureFactions auto

Formlist property FEC_ListFoodRaw auto
Formlist property FEC_ListFoodCooked auto
;	
Formlist property FEC_ListKeywordMagic auto	
Formlist property FEC_ListKeywordMagicFrozen auto
Formlist property FEC_ListKeywordWeapon auto
;
Formlist property DisintegrationMainImmunityList auto

GlobalVariable property FEC_AcidRandomChanceCr auto
GlobalVariable property FEC_FireRandomChanceCr auto
GlobalVariable property FEC_SunRandomChanceCr auto
GlobalVariable property FEC_FrostRandomChanceCr auto
GlobalVariable property FEC_ShockRandomChanceCr auto
GlobalVariable property FEC_PoisonRandomChanceCr auto

GlobalVariable property FEC_FrostModesCR auto
GlobalVariable property FEC_FrostShaderCrToggle auto

GlobalVariable property FEC_CookedFoodToggle auto
GlobalVariable property FEC_PostDeathEffectsCrToggle auto

GlobalVariable property FEC_FireDamageToggle auto
GlobalVariable property FEC_ShockDamageToggle auto

GlobalVariable property FEC_FireModesCr auto

GlobalVariable property FEC_CollisionScaleToggle auto

;FIRE
ImpactDataSet property FEC_FireImpactSet auto
;ICE
ImpactDataSet property FEC_FrostImpactSet auto
ImpactDataSet property FEC_WaterImpactSet auto
;SHOCK
ImpactDataSet property FEC_ShockImpactSet auto

int randomChance
int magicType = -1

int spellLevel = 0;
int projectileType = 0;

;types
int kSUN = 0
int kACID = 1
int kFIRE = 2
int kFROST = 3
int kSHOCK = 4
int kFIRE_FROST = 6
int kFIRE_SHOCK = 7
int kFROST_SHOCK = 10
int kSHOCK_FROST = 11

;modes
int kRANDOM = 0
int kAUTOMATIC = 1
int kCHAR = 2
int kVAPOURISED = 3

int kFREEZE_AUTO = 0
int kFREEZE = 1
int kICEBLOCK = 2

int victimSex = 0

int kRESET_PERM = 0
int kRESET_TEMP = 1
int kRESET_FROZEN_ACTOR = 2
int kRESET_FROZEN_COL = 3

int kEFFECT_Reset = -2
int kEFFECT_None = -1
int kEFFECT_Charred = 0
int kEFFECT_Frozen = 3
int kEFFECT_FrozenShader = 5

Keyword property IsBeastRace auto
;
Keyword property MagicDamageFire auto
Keyword property MagicDamageFrost auto
Keyword property MagicDamageShock auto

MagicEffect[] effectArray

MiscObject property FEC_FrostIceShaderObject auto

ObjectReference inventoryContainer 

Race victimRace 

Spell property FEC_FireCorpseSpell auto
Spell property FEC_ShockCorpseSpell auto

String victimName
String frostTXSTIncludePath

TextureSet property FEC_DefaultSkeleTXST auto
TextureSet property FEC_DefaultSkeleBeastTXST auto
TextureSet property FEC_FireCharSkeleTXST auto
TextureSet property FEC_FireSkeleBeastTXST auto

TextureSet property FEC_DefaultDraugrMaleTXST auto
TextureSet property FEC_DefaultDraugrFemaleTXST auto
TextureSet property FEC_FireCharMaleTXST auto
TextureSet property FEC_FireCharFemaleTXST auto

VisualEffect fireRefractionEffect
VisualEffect genericTriggerEffect
VisualEffect property FEC_GenericTriggerEffect auto
VisualEffect property FEC_GenericTriggerEffectCR auto
VisualEffect property FEC_FireHeatRefractionEffectCR auto
VisualEffect property FEC_FireHeatRefractionEffectCRQuad auto

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;=======================================
;			EMPTY STATE 
;=======================================

Event OnDeathEffectsActiveCr()
endEvent

Event OnDeathEffectsInactiveCr()
endEvent

Event OnEffectDamageChange(bool abChange)
endEvent

Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
endEvent

Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
endEvent

Event OnActorReanimateStop(Actor akTarget, Actor akCaster)
endEvent

Event OnActorResurrected(Actor akTarget, bool abResetInventory)
endEvent

Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)
endEvent

Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY	
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
			victimRace = victim.GetRace()

			; block all effects if target is immune to disintegration
			if (victimBase && DisintegrationMainImmunityList.HasForm(victimBase) || victimRace && DisintegrationMainImmunityList.HasForm(victimRace))
				GoToState("EMPTY")
				return
			endif
			
			victimName = victimBase.GetName()
			
			isBipedal = !IsQuadruped(victim)
			isHumanoid = IsCreatureHumanoid()
			
			disintegrateProof = IsDisintegrateProof(victim)
			isReanimated = GetActorState(victim) == 4
											
			isDraugr = victim.IsInFaction(bipedalCreatureFactions[0])
			isSkeleton = victim.IsInFaction(bipedalCreatureFactions[6])
						
			if isDraugr				
				victimSex = victimBase.GetSex()
			endif
					
			;---------------------------------------------			
							
			if GetPermanentDeathEffect(victim, kEFFECT_None)
																				
				if GetPermanentDeathEffect(victim, kEFFECT_Reset)
				
					ClearPermEffectSerialization()	
					ResetActor3D_FEC(victim, false)
				
				elseif GetPermanentDeathEffect(victim, kEFFECT_Frozen)
				
					SendFECResetEvent(victim, kRESET_FROZEN_COL, false)
					ResetFrozen()
										
				else
				
					if isReanimated												
						reanimated = true							
						GoToStateDebug("Sleeping")						
					else					
						GoToStateDebug("Done")							
					endif
					
				endif
				
				return
				
			elseif GetTemporaryDeathEffect(victim, kEFFECT_None)
								
				if isReanimated															
					reanimated = true						
					GoToStateDebug("Sleeping")					
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
		
			;---------------VARIABLES---------------
			int [] deathEffect = GetCauseOfDeath(victim, 0)
			magicType = deathEffect[0]
			spellLevel = deathEffect[1]
			projectileType = deathEffect[2]	

			isVIP = IsVIP(victim)	
			
			randomChance = GenerateRandomInt(0,100)							
			;--------------------------------------------
			
			;FIRE

			if magicType == kFIRE && randomChance <= FEC_FireRandomChanceCr.GetValue() as int

				if !IsActorInWater(victim) && !IsActorUnderWater(victim)
					BurnEffect()
				else
					SteamEffect()
				endif
				
			elseif magicType == kFIRE_FROST && randomChance <= FEC_FireRandomChanceCr.GetValue() as int	
																		
				SteamEffect(applyFrost = true)
				
			elseif magicType == kFIRE_SHOCK && randomChance <= FEC_FireRandomChanceCr.GetValue() as int
															
				if !IsActorInWater(victim) && !IsActorUnderWater(victim)
					BurnEffect(applyShock = true)
				else
					SteamEffect(applyShock = true)
				endif

			elseif magicType == kFROST && randomChance <= FEC_FrostRandomChanceCr.GetValue() as int
																	
				FreezeEffect()

			elseif magicType ==	kFROST_SHOCK && randomChance <= FEC_FrostRandomChanceCr.GetValue() as int	
																		
				FreezeEffect(applyShock = true)
				
			elseif magicType ==	kSHOCK_FROST && randomChance <= FEC_ShockRandomChanceCr.GetValue() as int	
								
				ShockEffect(applyFrost = true)									

			elseif magicType == kSHOCK && randomChance <= FEC_ShockRandomChanceCr.GetValue() as int	
												
				ShockEffect()
				
			elseif magicType == kSUN && !disintegrateProof && !isVIP && randomChance <= FEC_SunRandomChanceCr.GetValue() as int	
								
				GoToStateDebug("DoneForReal")
				CreateAshPileEffect(victim, FEC_SunDisintegrateFXS, FEC_AshPile)	
			
			;/elseif magicType == kACID && !disintegrateProof && !isVIP && randomChance <= FEC_AcidRandomChanceCr.GetValue() as int	
								
				GoToStateDebug("DoneForReal")
				CreateAshPileEffect(victim, FEC_AcidDisintegrateFXS, FEC_AcidAshPile, 0.5, 1.30)/;
				
			else
			
				GoToBuffer()
	
			endif	
		
		endif
				
	endEvent	
	
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY
	endEvent
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked);EMPTY		
	endEvent

	Event OnTrigger(ObjectReference akReference) ;EMPTY
	endEvent

	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
	endEvent	
	
endState

;=======================================
;		EFFECT BUFFER STATE
;=======================================			

State EffectBuffer
		
	Event OnBeginState()
				
		UnregisterComplexEvents()
		RegisterForActorReanimateStart(self)
		
	endEvent
	
	
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim		
			reanimated = true					
			UnregisterForActorReanimateStart(self)			
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
				
	endEvent
		
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim		
			reanimated = true		
			UnregisterForActorReanimateStart(self)			
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
	
endState

;=======================================
;		FROZEN BUFFER STATE
;=======================================			

State FrozenBuffer
		
	Event OnBeginState()
						
		UnregisterComplexEvents()
		UnregisterForActorReanimateStop(self)	
			
		RegisterForMagicEffectApplyEx(self, FEC_ListKeywordMagicFrozen, true)
		RegisterForHitEventEx(self, akSourceFilter = FEC_ListKeywordWeapon)			
		
		RegisterForFECReset(self, 0)	
		RegisterForFECReset(self, 2)					
		RegisterForActorReanimateStart(self)		
										
		if !victim.IsAIEnabled()
			FreezeActor(victim, 0, true)
		endif
		
		if CheckPermEffectSerialization() && frozenShader	
			ReapplyFrozenShader()
		endif
							
		if victimName == ""
			victimName = victimBase.GetName()
		endif
		victim.SetDisplayName("")
		
		if !frozenShader && HasEffectShader(victim, FEC_FrostFXS) == 0
			StopAllShaders(victim)
			Wait(1.0)
			FEC_FrostFXS.Play(victim)
		endif
										
	endEvent
	
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
							
		if akActor == victim && (aiType == kRESET_PERM || aiType == kRESET_FROZEN_ACTOR)									
			ResetFrozen()			
		endif
					
	endEvent
	
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim
		
			UnregisterForActorReanimateStart(self)			
			SendFECResetEvent(victim, kRESET_FROZEN_COL, false)				
			ResetFrozen(true)		
			
		endif
		
	endEvent
	
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied)
							
		SendFECResetEvent(victim, kRESET_FROZEN_COL, false)			
		ResetFrozen()	
	
	endEvent
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)
													
		SendFECResetEvent(victim, kRESET_FROZEN_COL, false)				
		ResetFrozen()
		
	endEvent

	Event OnTrigger(ObjectReference akReference) ;EMPTY
	endEvent

	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
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
		;---------------------------------------
		
		UnregisterComplexEvents()
		
		RegisterForModEvent("FEC_EffectDamageChange", "OnEffectDamageChange")
		RegisterForActorReanimateStart(self)
		
	endEvent
		
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim
		
			reanimated = true		
			UnregisterForActorReanimateStart(self)
			
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
		
			Actor akActor = akReference as Actor			
			if akActor			
				akActor.DispelSpell(FEC_ShockCorpseSpell)				
			endif
			
		endif

	endEvent
	
endState

;=======================================
;			AFTER DEATH
;=======================================				

State AfterDeath 

	Event OnBeginState()
			
		;---------------VARIABLES---------------
		globalLock = false
		sleeping = false
		
		isVIP = IsVIP(victim)
		;----------------------------------------
						
		UnregisterComplexEvents()
		UnregisterForActorReanimateStop(self)		
				
		RegisterForModEvent("DeathEffectsInactiveCR", "OnDeathEffectsInactiveCR")
		RegisterForMagicEffectApplyEx(self, FEC_ListKeywordMagic, true)		
		RegisterForActorReanimateStart(self)
		RegisterForActorResurrected(self)
		
		if CheckPermEffectSerialization()					
			if frozenShader		
				ReapplyFrozenShader()		
			endif
		endif
					
	endEvent

	Event OnDeathEffectsInactiveCR()
	
		sleeping = true
		GoToStateDebug("Sleeping")
			
	endEvent
		
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim		
			FullReset(true)			
			UnregisterForActorResurrected(self)			
		endif
	
	endEvent
		
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim
		
			reanimated = true
			GoToStateDebug("Sleeping")
			
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
			
			if akEffect.HasKeywordString("po3_MagicDamageSun") && !disintegrateProof && !isVIP && randomChance <= FEC_SunRandomChanceCr.GetValue() as int
																							
				GoToStateDebug("DoneForReal")
				CreateAshPileEffect(victim, FEC_SunDisintegrateFXS, FEC_AshPile)
				
			;/elseif IsAcidEffect(akEffect) && !disintegrateProof && !isVIP && randomChance <= FEC_AcidRandomChanceCr.GetValue() as int	
								
				GoToStateDebug("DoneForReal")
				CreateAshPileEffect(victim, FEC_AcidDisintegrateFXS, FEC_AcidAshPile, 0.5, 1.30)/;
				
			elseif akEffect.HasKeyword(MagicDamageFire) && randomChance <= FEC_FireRandomChanceCr.GetValue() as int

				if !IsActorInWater(victim) && !IsActorUnderWater(victim)
					BurnEffect() 			
				else	
					SteamEffect() 
				endif				
			
			elseif akEffect.HasKeyword(MagicDamageFrost) && randomChance <= FEC_FrostRandomChanceCr.GetValue() as int
						
				FreezeEffect()
				
			elseif akEffect.HasKeyword(MagicDamageShock) && randomChance <= FEC_ShockRandomChanceCr.GetValue() as int

				ShockEffect()

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
	
endState
	
;=======================================

;			SLEEPING STATE

;=======================================			

State Sleeping

	Event OnBeginState()
		
		;---------------VARIABLES---------------
		globalLock = true
		triggerLock = true
		;---------------------------------------
		
		UnregisterComplexEvents()
		
		RegisterForModEvent("DeathEffectsActiveCr", "OnDeathEffectsActiveCr")	
		RegisterForFECReset(self, 0)	
		RegisterForActorReanimateStop(self)		
		RegisterForActorResurrected(self)
		
		if CheckPermEffectSerialization()				
			if charred		
				ReapplyCharred()			
			elseif frozenShader		
				ReapplyFrozenShader()		
			endif
		endif
			
	endEvent
	
	Event OnDeathEffectsActiveCr()
		
		if !reanimated
			if permanentEffect
				GoToStateDebug("Done")
			else
				GoToStateDebug("AfterDeath")
			endif
		endif
			
	endEvent
		
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
			
		if akActor == victim && aiType == kRESET_PERM
								
			ClearPermEffectSerialization()							
			if abReset3D
				ResetActor3D_FEC(victim, false)	
			endif			
			GoToBuffer()
			
		endif
			
	endEvent	
	
	Event OnActorReanimateStop(Actor akTarget, Actor akCaster)
		
		if akTarget == victim && !sleeping
			
			reanimated = false				
			if permanentEffect
				GoToStateDebug("Done")
			else
				GoToStateDebug("AfterDeath")
			endif					
			UnregisterForActorReanimateStop(self)
			
		endif
	
	endEvent
		
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim
		
			FullReset(true)			
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
		
endState

;=======================================
;			DONE STATE
;=======================================			

State Done 

	Event OnBeginState()
							
		if reanimated		
			GoToStateDebug("Sleeping")
			return			
		endif
		
		;---------------VARIABLES---------------
		globalLock = false
		sleeping = false
		
		isVIP = IsVIP(victim)
		;---------------------------------------
		
		UnregisterComplexEvents()
		
		RegisterForModEvent("DeathEffectsInactiveCR", "OnDeathEffectsInactiveCR")
		RegisterForMagicEffectApplyEx(self, FEC_ListKeywordMagic, true)		
		RegisterForFECReset(self, 0)		
		RegisterForActorReanimateStart(self)
		RegisterForActorResurrected(self)
		
		if GetTemporaryDeathEffect(victim, kEFFECT_None)
			SendFECResetEvent(victim, kRESET_TEMP, false)
		endif
				
		if CheckPermEffectSerialization()						
			if charred		
				ReapplyCharred()			
			elseif frozenShader		
				ReapplyFrozenShader()		
			endif
		endif
						
	endEvent

	Event OnDeathEffectsInactiveCr()
	
		sleeping = true
		GoToStateDebug("Sleeping")
			
	endEvent
	
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
			
		if akActor == victim && aiType == kRESET_PERM
					
			ClearPermEffectSerialization()	
						
			if abReset3D
				ResetActor3D_FEC(victim, false)	
			endif
			
			GoToBuffer()
			
		endif
			
	endEvent
		
	Event OnActorReanimateStart(Actor akTarget, Actor akCaster)
	
		if akTarget == victim
		
			reanimated = true
			GoToStateDebug("Sleeping")
			
			UnregisterForActorReanimateStart(self)
		
		endif
	
	endEvent
		
	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim
		
			FullReset(true)			
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
			
			if akEffect.HasKeywordString("po3_MagicDamageSun") && !disintegrateProof && !isVIP && randomChance <= FEC_SunRandomChanceCr.GetValue() as int
						
				GoToStateDebug("DoneForReal")
				CreateAshPileEffect(victim, FEC_SunDisintegrateFXS, FEC_AshPile)

			;/elseif IsAcidEffect(akEffect) && !disintegrateProof && !isVIP && randomChance <= FEC_AcidRandomChanceCr.GetValue() as int	
								
				GoToStateDebug("DoneForReal")
				CreateAshPileEffect(victim, FEC_AcidDisintegrateFXS, FEC_AcidAshPile, 0.5, 1.30)/;
			
			elseif akEffect.HasKeyword(MagicDamageFire) && randomChance <= FEC_FireRandomChanceCr.GetValue() as int
										
				FEC_FrostFXS.Stop(victim)
				FEC_FrostIceFormFXS.Stop(victim)

				if !fireRefractionArt				
					if isBipedal 	
						fireRefractionArt = GetArtObject(FEC_FireHeatRefractionEffectCR)
					else
						fireRefractionArt = GetArtObject(FEC_FireHeatRefractionEffectCRQuad)
					endif
				endif								
				
				if HasArtObject(victim, fireRefractionArt) == 0	&& !IsActorInWater(victim) && !IsActorUnderWater(victim)		
					BurnEffect()
				endif
	
			elseif akEffect.HasKeyword(MagicDamageFrost) && randomChance <= (FEC_FrostRandomChanceCr.GetValue() as int) && HasEffectShader(victim, FEC_FrostFXS) == 0	
																										
				FEC_FrostFXS.Play(victim)			
									
			elseif akEffect.HasKeyword(MagicDamageShock) && (randomChance <= FEC_ShockRandomChanceCR.GetValue() as int) && HasEffectShader(victim, FEC_ShockFXS) == 0
	
				ShockEffect()		
								
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
		
		GoToStateDebug("Default")
								
	endEvent
	
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY
	endEvent
	
	Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked) ;EMPTY		
	endEvent

	Event OnTrigger(ObjectReference akReference) ;EMPTY
	endEvent

	Event OnTriggerLeave(ObjectReference akReference) ;EMPTY
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
				
endState
	
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;==========================================================															
;[1]				BURN EFFECT                                                                                           
;==========================================================	

Function BurnEffect(bool applyShock = false)

	GoToStateDebug("FireBuffer")
	
	;---------------------VARIABLES----------------------------
	int effectType = GetEffectType(kFIRE)	
	int randomEffect = GenerateRandomInt(1,2)		
	;----------------------------------------------------------
	
	if !disintegrateProof && !isVIP && ((effectType == kRANDOM && randomEffect == 2) || (effectType == kAUTOMATIC && spellLevel == 100) || effectType == kVAPOURISED)
		GoToStateDebug("DoneForReal")
		CreateAshPileEffect(victim, FEC_FireDisintegrateFXS, FEC_AshPile)	
		return					
	endif
	
	if spellLevel >= 75
		FEC_FireHeavyFXS.Play(victim, GenerateRandomFloat(3.0,5.0))	
	else
		FEC_FireFXS.Play(victim, GenerateRandomFloat(2.0,4.0))	
	endif
	
	if paralyzed
		FreezeActor(victim, 1, false)
	endif

	if applyShock	
		FEC_ShockFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))		
	endif	
	
	if !fireRefractionEffect
		if isBipedal 	
			fireRefractionEffect = FEC_FireHeatRefractionEffectCR
		else
			fireRefractionEffect = FEC_FireHeatRefractionEffectCRQuad
		endif
	endif
	fireRefractionEffect.Play(victim)
	
	if allowCollisionScaling && allowFireDamage
		ScaleTriggerArt(victim, scaleDiff, "FECRefractionFX")
	endif
	
	victim.PlayImpactEffect(FEC_FireImpactSet, "MagicEffectsNode", 0.0, 0.0, -1.0, 128.0, true, true)

	Wait(GenerateRandomFloat(1.00,2.00))
		
	RemoveEffectsNotOfType(victim, 4)
	
	frozenShader = false
	RemoveTemporaryDeathEffect(victim, kEFFECT_FrozenShader)
	
	Wait(GenerateRandomFloat(1.00,2.00))
	
	FEC_FlyingBitsLiteFXS.Play(victim, 8.0)		; playing flying bits 	  		
	FEC_FlyingBitsFXS.Play(victim, 8.0)
	
	if !permanentEffect			
		
		if isHumanoid && HasEffectShader(victim, FEC_FireCharFXS) == 0
		
			FEC_SootFXS.Play(victim)
			FEC_FireCharFXS.Play(victim)
				
		elseif isDraugr
					
								
			Armor targetArmor = GetWornArmor()
			
			if victimSex == 0
			
				Armor beard = victim.GetWornForm(0x00000010) as Armor
				if (beard)
					victim.UnequipItem(beard, true)
				endif						
				ReplaceArmorTextureSet(victim, targetArmor, FEC_DefaultDraugrMaleTXST, FEC_FireCharMaleTXST) 
											
			else

				ToggleChildNode(victim, "Rags61", true)			
				ReplaceArmorTextureSet(victim, targetArmor, FEC_DefaultDraugrFemaleTXST, FEC_FireCharFemaleTXST) 
			
			endif
			
			permanentEffect = true
			charred = true
			AssignPermanentDeathEffect(victim, kEFFECT_Charred)
						
			Armor hair = victim.GetWornForm(0x00000002) as Armor
			if (hair)
				victim.UnequipItem(hair, true)
			endif
			
			ToggleChildNode(victim, "Eye_R", true)
			ToggleChildNode(victim, "Eye_L", true)				
				
			FEC_SootFXS.Play(victim)
							
		elseif isSkeleton
				
			Armor targetArmor = GetWornArmor()			
			if victimRace.HasKeyword(IsBeastRace)	
				ReplaceArmorTextureSet(victim, targetArmor, FEC_DefaultSkeleBeastTXST, FEC_FireSkeleBeastTXST) 			
			else
				ReplaceArmorTextureSet(victim, targetArmor, FEC_DefaultSkeleTXST, FEC_FireCharSkeleTXST) 			
			endif
			
			permanentEffect = true
			charred = true			
			AssignPermanentDeathEffect(victim, kEFFECT_Charred)
						
		else
	
			FEC_FireCharCrFXS.Play(victim)
		
		endif
				
	endif
		
	Wait(GenerateRandomFloat(3.00,4.00))
	
	CookFood()
	
	FEC_FireSmokeFXS.Play(victim, GenerateRandomFloat(8.0, 12.0)); smoke effect
		
	Wait(7.0)
	
	fireRefractionEffect.Stop(victim)
	
	GoToStateDebug("Done")
  	
endFunction
 
;==========================================================															
;[2]				STEAM EFFECT                                                                                            
;==========================================================	

Function SteamEffect(bool applyFrost = false, bool applyShock = false)	
		
	GoToStateDebug("EffectBuffer")
	
	FEC_SteamFXS.Play(victim, GenerateRandomFloat(8.0,10.0))  ; smoke effect	
	FEC_FireCharCrFXS.Play(victim)
	
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
	
	victim.PlayImpactEffect(Impact, "MagicEffectsNode", 0.0, 0.0, -1.0, 512.0, true, true)			
	
	CookFood()

	GoToBuffer()
					
endFunction
	
;==========================================================														
;[3]				FREEZE EFFECT                                                                                           
;==========================================================

Function FreezeEffect(bool applyShock = false)

	GoToStateDebug("EffectBuffer")
	
	;---------------VARIABLES---------------	
	int effectType = GetEffectType(kFROST)
	;---------------------------------------
	
	victim.PlaceAtMe(FEC_FrostExplosion)
	
	victim.PlayImpactEffect(FEC_FrostImpactSet, "MagicEffectsNode", 0.0, 0.0, -1.0, 512.0, true, true)		
	
	if isVIP || isReanimated || effectType == kICEBLOCK
					
		IceBlock(applyShock)
	
	elseif effectType == kFREEZE_AUTO
	
		if isBipedal
									
			FreezeActor(victim, 0, true)
			
			TryApplyIceShader(false)
			
			KillNoWait(victim)

			StopAllShaders(victim)
			Wait(1.0)
			FEC_FrostFXS.Play(victim)			
														
			inventoryContainer = victim.PlaceAtMe(FEC_FrostContainerCorpseCr)
			if WaitFor3DLoad(inventoryContainer)			
				inventoryContainer.SplineTranslateToRefNode(victim, "MagicEffectsNode",1.0,10000.0)
				inventoryContainer.SetActorOwner(playerRef.GetActorBase())	
				
				victim.RemoveAllItems(inventoryContainer, abRemoveQuestItems = true)
			endif
					
			if applyShock		
				FEC_ShockFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))			
			endif
			
			permanentEffect = true
			frozen = true
			AssignPermanentDeathEffect(victim, kEFFECT_Frozen)
					
			GoToStateDebug("FrozenBuffer")
			
		else
					
			IceBlock(applyShock)
			
		endif	
		
	elseif effectType == kFREEZE
									
		FreezeActor(victim, 0, true)
										
		TryApplyIceShader(false)
		
		KillNoWait(victim)

		StopAllShaders(victim)
		Wait(1.0)
		FEC_FrostFXS.Play(victim)		
		
		string nodeName
		if isBipedal
			inventoryContainer = victim.PlaceAtMe(FEC_FrostContainerCorpseCr)
			nodeName = "NPC Pelvis [Pelv]"
		else
			inventoryContainer = victim.PlaceAtMe(FEC_FrostContainerCorpseCrQuad)
			nodeName = "MagicEffectsNode"
		endif
		
		if WaitFor3DLoad(inventoryContainer)		
			inventoryContainer.SplineTranslateToRefNode(victim, nodeName, 1.0,10000.0)		
			inventoryContainer.SetActorOwner(playerRef.GetActorBase())
			
			victim.RemoveAllItems(inventoryContainer, abRemoveQuestItems = true)
		endif
		
		if applyShock		
			FEC_ShockFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))			
		endif
		
		permanentEffect = true
		frozen = true
		AssignPermanentDeathEffect(victim, kEFFECT_Frozen)
		
		GoToStateDebug("FrozenBuffer")
				
	endif
	
endFunction	

;==========================================================													
;[4]				SHOCK EFFECT                                                                                         
;==========================================================	

Function ShockEffect(bool applyFrost = false)

	GoToStateDebug("ShockBuffer")
	
	FEC_ShockFXS.Play(victim)
	if applyFrost		
		victim.PlaceAtMe(FEC_FrostExplosion)
	endif
	
	if !genericTriggerEffect
		if isBipedal 	
			genericTriggerEffect = FEC_GenericTriggerEffect				
		else
			genericTriggerEffect = FEC_GenericTriggerEffectCR
		endif
	endif
	genericTriggerEffect.Play(victim)
	
	if allowCollisionScaling && allowShockDamage
		ScaleTriggerArt(victim, scaleDiff, "FECHitBoxGeneric")
	endif
		
 	victim.PlayImpactEffect(FEC_ShockImpactSet, "MagicEffectsNode", 0.0, 0.0, -1.0, 512.0, true, true)	
	
	victim.PushActoraway(victim, 0.0)
	
	if isHumanoid	
		FEC_SootFXS.Play(victim)
		FEC_FireCharFXS.Play(victim)		
	elseif isDraugr || isSkeleton	
		FEC_SootFXS.Play(victim)			
	else	
		FEC_FireCharCrFXS.Play(victim) 		
	endif

	float normalisedMass = 1 - (victim.GetMass() - 50) / (2000 - 50)	
	int randomTime = GenerateRandomInt(10, 20)	
	
	int time = 0	
	while time < randomTime
		if victim.Is3DLoaded()
			victim.ApplyHavokImpulse(0.0, 0.0, 1.0, normalisedMass * GenerateRandomInt(50, 100))
		endif
		Wait(GenerateRandomFloat(0.50,1.0))
		time += 1
	endwhile
	
	CookFood()
	
	FEC_SmokeFXS.Play(victim, 6.0)   ; smoke effect 
	
	FEC_ShockFXS.Stop(victim)
	genericTriggerEffect.Stop(victim)
	
	GoToStateDebug("Done")
  	
endFunction

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                         FROST FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Function ApplyMLPShader()

	if isDraugr
		if victimSex == 0
			frostTXSTIncludePath = "Draugr.dds"
		else
			frostTXSTIncludePath = "FemaleDraugr.dds"
		endif
	endif
	
	ObjectReference shader = victim.placeAtMe(FEC_FrostIceShaderObject)
	if WaitFor3DLoad(shader)		
		SetShaderType(victim, shader, frostTXSTIncludePath, 11, 1, abNoWeapons = true, abNoAlphaProperty = true);
		Wait(0.1)
		shader.Disable()
		shader.Delete()
	endif

endFunction

Function TryApplyIceShader(bool setPermanent)

	if FEC_FrostShaderCrToggle.GetValue() == 1.0
		ApplyMLPShader()
		if HasNiExtraData(victim, "PO3_SHADER | 11") 
			if setPermanent
				permanentEffect = true
			endif
			frozenShader = true
			AssignTemporaryDeathEffect(victim, kEFFECT_FrozenShader)
		endif
	endif

endFunction
	
Function IceBlock(bool applyShock = false)
	
	TryApplyIceShader(true)
	
	if !victim.HasKeywordString("ImmuneParalysis")
		FreezeActor(victim, 1, true)
		paralyzed = true
	endif
	
	FEC_FrostIceFormFXS.Play(victim)
	
	if applyShock	
		FEC_ShockFXS.Play(victim, GenerateRandomFloat(6.0, 8.0))		
	endif	
	
	GoToStateDebug("Done")
			
endFunction

Function ResetFrozen(bool goToSleep = false)

	FreezeActor(victim, 0, false)				
	if victim.GetDisplayName() == ""
		if victimName == ""
			victimName = victimBase.GetName()
		endif	
		victim.SetDisplayName(victimName)
	endif

	permanentEffect = false
	frozen = false	
	AssignPermanentDeathEffect(victim, kEFFECT_None)
	
	inventoryContainer = None
	
	if goToSleep
		GoToStateDebug("Sleeping")
	else
		GoToBuffer()
	endif
	
endFunction

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                         REAPPLY FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Function ReapplyCharred()
		
	if WaitFor3DLoad(victim)

		if isDraugr
	
			Armor targetArmor = GetWornArmor()			
			Armor hair = victim.GetWornForm(0x00000002) as Armor
			if (hair)
				victim.UnequipItem(hair, true)
			endif
							
			if victimSex == 0
			
				Armor beard = victim.GetWornForm(0x00000010) as Armor
				if (beard)
					victim.UnequipItem(beard, true)
				endif				
				ReplaceArmorTextureSet(victim, targetArmor, FEC_DefaultDraugrMaleTXST, FEC_FireCharMaleTXST) 
											
			else

				ToggleChildNode(victim, "Rags61", true)			
				ReplaceArmorTextureSet(victim, targetArmor, FEC_DefaultDraugrFemaleTXST, FEC_FireCharFemaleTXST) 
			
			endif
			
			ToggleChildNode(victim, "Eye_R", true)
			ToggleChildNode(victim, "Eye_L", true)	
		
		elseif isSkeleton
	
			Armor targetArmor = GetWornArmor()			
			if victimRace.HasKeyword(IsBeastRace)
				ReplaceArmorTextureSet(victim, targetArmor, FEC_DefaultSkeleBeastTXST, FEC_FireSkeleBeastTXST) 	
			else
				ReplaceArmorTextureSet(victim, targetArmor, FEC_DefaultSkeleTXST, FEC_FireCharSkeleTXST) 
			endif

		endif
	
		if HasEffectShader(victim, FEC_SootFXS) == 0
			FEC_SootFXS.Play(victim)
		endif
		
	endif
	
endFunction

Function ReapplyFrozenShader()
	
	if WaitFor3DLoad(victim)
	
		ApplyMLPShader()
		
	endif

endFunction

bool Function CheckPermEffectSerialization()

	bool result = false
				
	if charred
		AssignPermanentDeathEffect(victim, kEFFECT_Charred)
	elseif GetPermanentDeathEffect(victim, kEFFECT_Charred)
		charred = true
		result = true
	endif
	
	if frozenShader
		AssignTemporaryDeathEffect(victim, kEFFECT_FrozenShader)
	elseif GetTemporaryDeathEffect(victim, kEFFECT_FrozenShader)
		frozenShader = true
		result = true
	endif
	
	if result
		permanentEffect = true
	endif
		
	return permanentEffect || charred || frozenShader
	
endFunction

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                         MISC FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Function GetEffectType(int type)

	if type == kFIRE ; FIRE			
		return FEC_FireModesCr.GetValue() as int						
	elseif type == kFROST ; FROST			
		return FEC_FrostModesCR.GetValue() as int		
	;elseif type == 2 ; SHOCK
		;
	;elseif type == 3 ; DRAIN
		;
	endif
	
	return 1 ;AUTO
	
endFunction	

Function FullReset(bool goToDefault = false)

	globalLock = false
	triggerLock = false
	
	permanentEffect = false
	charred = false
	AssignPermanentDeathEffect(victim ,kEFFECT_None)
	
	if frozen
		FreezeActor(victim, 0, false)
		frozen = false
	endif
	if paralyzed
		FreezeActor(victim, 1, false)
		paralyzed = false
	endif
		
	frozen = false
	frozenShader = false	
	RemoveTemporaryDeathEffect(victim ,kEFFECT_FrozenShader)

	if !goToDefault
		GoToBuffer()
	else
		ResetActor3D_FEC(victim, false)	
		UnregisterForAllFECResets(self)
		GoToStateDebug("Default")
	endif

endFunction

Function ClearPermEffectSerialization()

	permanentEffect = false
	charred = false
	frozen = false
	AssignPermanentDeathEffect(victim,kEFFECT_None)
	
	frozenShader = false
	RemoveTemporaryDeathEffect(victim,kEFFECT_FrozenShader)
	
endFunction

Function GoToBuffer()

	if !victim.IsDead()
		GoToStateDebug("Default")
		return		
	endif
	
	if FEC_PostDeathEffectsCrToggle.GetValue() as bool	
		GoToStateDebug("AfterDeath")
	else
		sleeping = true
		GoToStateDebug("Sleeping")
	endif
	
endFunction

bool Function IsCreatureHumanoid()

	int i = 1 ;exclude draugr
	int iFaction = 6
	Faction akFaction

	while  i < iFaction
		akFaction = bipedalCreatureFactions[i]
		if victim.IsInFaction(akFaction)
			return true
		endif
		i += 1
	endwhile
	
	return false
	
endFunction

Armor Function GetWornArmor()

	Armor targetArmor = victim.GetEquippedArmorInSlot(32)
	if !targetArmor && isSkeleton
		targetArmor = victim.GetEquippedArmorInSlot(30) ;BODY
	endif
	if !targetArmor
		targetArmor = victimBase.GetSkin()
	endif
	if !targetArmor
		targetArmor = victimRace.GetSkin()
	endif
	
	return targetArmor
		
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