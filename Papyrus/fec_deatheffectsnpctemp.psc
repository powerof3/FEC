Scriptname FEC_DeathEffectsNPCTemp extends activemagiceffect  
;secondary effects

import game
import debug
import utility

import po3_SKSEFunctions ;helper functions
import po3_Events_AME
import FEC_Utility
 
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							PROPERTIES
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Actor victim

ActorBase victimBase

bool poisoned = false
bool frightened = false
bool air = false
bool soultrapped = false

ColorForm poisonSkin 
ColorForm whiteHair 
ColorForm whiteSkin 

;POISON/SOULTRAP
EffectShader property FEC_PoisonWispsFXS auto 
EffectShader property FEC_PoisonVeinFXS auto 
EffectShader property FEC_SoulTrapSkinFXS auto  

MagicEffect[] effectArray

;FEAR
Formlist property FEC_ListHairColor auto
Formlist property FEC_ListSkinColor auto

;CHANCE
GlobalVariable property FEC_PoisonRandomChance auto
GlobalVariable property FEC_SoulTrapRandomChance auto
GlobalVariable property FEC_FearRandomChance auto
GlobalVariable property FEC_AirRandomChance auto
;
GlobalVariable property FEC_PoisonOpacityColor auto
GlobalVariable property FEC_PoisonOpacityAutoCalc auto
GlobalVariable property FEC_PoisonMode auto

int randomChance
int spellLevel
int projectileType

int magicType = -1
int bleedoutType = -1

int kPOISON = 0
int kFEAR = 1
int kPOISONFEAR = 3

int kEFFECT_Reset = -2
int kEFFECT_None = -1
int kEFFECT_Poisoned = 2
int kEFFECT_Frightened = 3
int kEFFECT_Suffocated = 4

Keyword property IsBeastRace auto
Keyword property ActorTypeGhost auto

Race property OrcRace auto
Race property OrcRaceVampire auto
Race targetRace

;SOULTRAP
TextureSet property EyesMaleHumanBlind auto
TextureSet property EyesMaleOrcBlind auto
TextureSet property SkinEyesFemaleArgonianBlind auto
TextureSet property SkinEyesMaleArgonianBlind auto

;POISON
TextureSet property EyesMaleHumanBrownBloodShot auto

VisualEffect property FEC_AirAbsorbFXEffect auto
VisualEffect property FEC_AirAbsorbFXNoWindEffect auto

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;=======================================
;			EMPTY STATE 
;=======================================

Event OnEnterBleedout()		
endEvent

Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
endEvent

Event OnSoulTrapped(Actor akVictim, Actor akKiller)
endEvent

Event OnActorResurrected(Actor akTarget, bool abResetInventory)
endEvent

;=======================================
;			DEFAULT STATE 
;=======================================

auto State Default

	Event OnEffectStart(Actor akTarget, Actor akCaster)
	
		victim = akTarget
		
		if victim 
					
			;---------------------VARIABLES----------------------------					
			targetRace = victim.GetRace()
			victimBase = victim.GetLeveledActorBase()

			whiteSkin =  FEC_ListSkinColor.GetAt(0) as ColorForm
			whiteHair = FEC_ListHairColor.GetAt(0) as ColorForm
			poisonSkin = FEC_ListSkinColor.GetAt(1) as ColorForm		
			;-----------------------------------------------------------
			
			if GetTemporaryDeathEffect(victim, kEFFECT_Reset)
			
				FullReset()
															
			elseif GetTemporaryDeathEffect(victim, kEFFECT_Suffocated)

				if HasDeferredKill(victim)				
					victim.EndDeferredKill()
				endif
				RemoveTemporaryDeathEffect(victim, kEFFECT_Suffocated)
				
			endif
			
			if victim.IsDead()			
				if GetTemporaryDeathEffect(victim, kEFFECT_None)			
					GoToState("TempDone")				
				else			
					GoToState("NotDone")					
				endif				
			endif
								
		endif								
		
	endEvent
	
	Event OnEnterBleedout()
	
		if victim		
			int [] bleedoutEffect = GetCauseOfDeath(victim, 1)
			bleedoutType = bleedoutEffect[0]
			
			effectArray = GetActiveEffects(victim, false)						
			if GetAirEffect(effectArray)
				
				randomChance = GenerateRandomInt(0,100)							
				if randomChance <= FEC_AirRandomChance.GetValue() as int
															
					VisualEffect airEffect					
					
					if spellLevel < 50
						Wait(0.6)
						if !victim.IsDead()
							victim.StartDeferredKill()
							AssignTemporaryDeathEffect(victim, kEFFECT_Suffocated)
							victim.SetNoBleedoutRecovery(true)
							airEffect = FEC_AirAbsorbFXNoWindEffect
							FEC_AirAbsorbFXNoWindEffect.Play(victim)
						endif
					else
						Wait(1.0)
						if !victim.IsDead()							
							victim.StartDeferredKill()
							AssignTemporaryDeathEffect(victim, kEFFECT_Suffocated)
							victim.SetNoBleedoutRecovery(true)
							airEffect = FEC_AirAbsorbFXEffect
							FEC_AirAbsorbFXEffect.Play(victim)
						endif
					endif
					victim.SetExpressionPhoneme(11, 1.0)
					
					Wait(5.10)
																			
					if HasDeferredKill(victim)
						RemoveTemporaryDeathEffect(victim, kEFFECT_Suffocated)
						victim.SetNoBleedoutRecovery(false)
						victim.EndDeferredKill()
						victim.Kill()
						airEffect.Stop(victim)
					endif

				endif
				
			endif
		
		endif
	
	endEvent
			
	Event OnDying(Actor akKiller)
	
		if victim
			
			;---------------------VARIABLES----------------------------					
			int [] deathEffect = GetCauseOfDeath(victim, 1)
			magicType = deathEffect[0]
			spellLevel = deathEffect[1]
			projectileType = deathEffect[2]		
			randomChance = GenerateRandomInt(0,100) 			
			;----------------------------------------------------------
									
			FEC_AirAbsorbFXEffect.Stop(victim)
			FEC_AirAbsorbFXNoWindEffect.Stop(victim)
			
			if (magicType == kPOISON || bleedoutType == kPOISON) && randomChance <= FEC_PoisonRandomChance.GetValue() as int

				VeinEffect()				
										
			elseif (magicType == kPOISONFEAR || bleedoutType ==  kPOISONFEAR) && randomChance <= FEC_PoisonRandomChance.GetValue() as int
			
				VeinEffect(true)
								
			elseif (magicType == kFEAR) && randomChance <= FEC_FearRandomChance.GetValue() as int
			
				FearEffect()
				
			else
			
				GoToState("NotDone")
							
			endif
				
		endif	
		
	endEvent
		
endState

;=======================================
;			NOTDONE STATE
;=======================================

State NotDone
	
	Event OnBeginState()
					
		UnregisterForAllModEvents()
		
		soultrapped = IsSoulTrapped(victim)								
		if soultrapped && randomChance <= FEC_SoulTrapRandomChance.GetValue() as int				
			SoulTrapEffect()									
		endif
		
		if !soultrapped
			RegisterForSoulTrapped(self)
		endif
	
	endEvent
		
	Event OnSoulTrapped(Actor akVictim, Actor akKiller)
	
		if akVictim == victim 	
			soultrapped = true		
			if randomChance <= FEC_SoulTrapRandomChance.GetValue() as int					
				SoulTrapEffect()			
			endif					
			UnregisterForSoulTrapped(self)						
		endif
	
	endEvent

	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference) ;EMPTY
	endEvent

	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference) ;EMPTY	
	endEvent
	
endState

;=======================================
;			TEMP DONE STATE
;=======================================

State TempDone

	Event OnBeginState()
	
		UnregisterForAllModEvents()		
		UnregisterForAllFECResets(self)	
		
		RegisterForFECReset(self, 1)
		RegisterForActorResurrected(self)
		
		if CheckTempSerialization()
			if poisoned
				ReapplyPoison()
			elseif frightened			
				ReapplyFear()
			endif
		endif
			
	endEvent
		
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
				
		if akActor == victim && aiType == 1							
			FullReset()				
		endif
			
	endEvent

	Event OnActorResurrected(Actor akTarget, bool abResetInventory)
	
		if akTarget == victim		
			FullReset(true)
			
			UnregisterForActorResurrected(self)
			UnregisterForAllFECResets(self)			
		endif
	
	endEvent	
	
	Event OnObjectEquipped(Form akBaseObject, ObjectReference akReference)
		
		Armor CurArmor = akBaseObject as Armor
		if CurArmor	
			if victim.Is3DLoaded()
				if poisoned							
					BlendColorWithSkinTone(victim, poisonSkin, FEC_PoisonMode.GetValue() as int, FEC_PoisonOpacityAutoCalc.GetValue() as bool, FEC_PoisonOpacityColor.GetValue())					
				elseif frightened				
					BlendColorWithSkinTone(victim, whiteSkin, 6, true, 1.0)					
				endif
			endif				
		endif

	endEvent

	Event OnObjectUnequipped(Form akBaseObject, ObjectReference akReference)
		
		Armor CurArmor = akBaseObject as Armor
		if CurArmor		
			if victim.Is3DLoaded()
				if poisoned 		
					BlendColorWithSkinTone(victim, poisonSkin, FEC_PoisonMode.GetValue() as int, FEC_PoisonOpacityAutoCalc.GetValue() as bool, FEC_PoisonOpacityColor.GetValue()) 				
				elseif frightened				
					BlendColorWithSkinTone(victim, whiteSkin, 6, true, 1.0)				
				endif
			endif				
		endif
						
	endEvent

endState

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;========================================================													
	;[1]				POISON EFFECT                                                                                           
;========================================================

Function VeinEffect(bool applyFear = false)
	
	FEC_PoisonWispsFXS.Play(victim,4.0)
	
	FEC_PoisonVeinFXS.Play(victim)	
	
	BlendColorWithSkinTone(victim, poisonSkin, FEC_PoisonMode.GetValue() as int, FEC_PoisonOpacityAutoCalc.GetValue() as bool, FEC_PoisonOpacityColor.GetValue())
	
	poisoned = true
	AssignTemporaryDeathEffect(victim, kEFFECT_Poisoned)
	
	if applyFear	
		SetHairColor(victim, whiteHair)
		
		frightened = true
		AssignTemporaryDeathEffect(victim, kEFFECT_Frightened)
	endif
	
	GoToState("TempDone")
			
endFunction

Function ReapplyPoison()
	
	if WaitFor3DLoad(victim)
					
		RemoveEffectsNotOfType(victim, 2)

		BlendColorWithSkinTone(victim, poisonSkin, FEC_PoisonMode.GetValue() as int, FEC_PoisonOpacityAutoCalc.GetValue() as bool, FEC_PoisonOpacityColor.GetValue())
		
		if frightened		
			SetHairColor(victim, whiteHair)			
		endif
		
		FEC_PoisonVeinFXS.Play(victim)
		
	endif
					
endFunction
 
;========================================================													
	;[2]				SOULTRAP EFFECT                                                                                           
;========================================================

Function SoulTrapEffect ()

	if targetRace.HasKeyword(IsBeastRace)
	
		if victimBase.GetSex() == 1	
			victim.SetEyeTexture(SkinEyesFemaleArgonianBlind)			
		else		
			victim.SetEyeTexture(SkinEyesMaleArgonianBlind)			
		endif
		
	elseif targetRace == OrcRace || targetRace == OrcRaceVampire	
		victim.SetEyeTexture(EyesMaleOrcBlind)	
	else	
		victim.SetEyeTexture(EyesMaleHumanBlind)	
	endif
	
	FEC_SoulTrapSkinFXS.Play(victim, 7.0)
	
endFunction

;========================================================													
	;[3]				FEAR EFFECT                                                                                           
;========================================================

Function FearEffect()

	SetHairColor(victim, whiteHair)
	BlendColorWithSkinTone(victim, whiteSkin, 6, true, 1.0)
	
	frightened = true	
	AssignTemporaryDeathEffect(victim, kEFFECT_Frightened)
	
	GoToState("TempDone")
						
endFunction

Function ReapplyFear()
	
	if WaitFor3DLoad(victim)
	
		RemoveEffectsNotOfType(victim, 2)
		
		BlendColorWithSkinTone(victim, whiteSkin, 6, true, 1.0)
		SetHairColor(victim, WhiteHair)
		
	endif
					
endFunction

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                         MISC FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Function GetAirEffect(MagicEffect[] SpellArray)

	int i = 0
	int iSpell = SpellArray.Length 

	while  i < iSpell
		MagicEffect effect = SpellArray[i]
		if effect && effect.HasKeywordString("MagicDamageWind")
			spellLevel = effect.GetSkillLevel()			
			return true
		endif
		i += 1
	endwhile
	
	return false

endFunction

bool Function CheckTempSerialization()

	bool result = false
			
	if poisoned
		AssignTemporaryDeathEffect(victim, kEFFECT_Poisoned)
	elseif GetTemporaryDeathEffect(victim, kEFFECT_Poisoned)
		poisoned = true
		result = true
	endif
	
	if frightened
		AssignTemporaryDeathEffect(victim, kEFFECT_Frightened)
	elseif GetTemporaryDeathEffect(victim, kEFFECT_Frightened)
		frightened = true
		result = true
	endif
			
	return result
		
endFunction

Function FullReset(bool goToDefault = false)

	poisoned = false
	frightened = false
	air = false
	soultrapped = false
	
	AssignTemporaryDeathEffect(victim, kEFFECT_None)
	
	if goToDefault
		GoToState("Default")
	endif

endFunction