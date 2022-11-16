Scriptname FEC_DeathEffectsCRTemp extends activemagiceffect  
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

Actor victim

Actorbase victimBase

bool isHumanoid = false

bool poisoned = false
bool frightened = false
bool soultrapped = false

ColorForm poisonSkin 
ColorForm WhiteHair 
ColorForm whiteSkin 
;
EffectShader property FEC_PoisonWispsFXS auto 
EffectShader property FEC_PoisonVeinFXS auto 
EffectShader property FEC_SoulTrapFXS auto 
EffectShader property FEC_SoulTrapSkinFXS auto   

Faction[] property BipedalCreatureFactions auto

Formlist property FEC_ListHairColor auto
Formlist property FEC_ListSkinColor auto

GlobalVariable property FEC_PoisonRandomChanceCr auto
GlobalVariable property FEC_SoulTrapRandomChanceCr auto
GlobalVariable property FEC_FearRandomChanceCr auto
;
GlobalVariable property FEC_PoisonOpacityColor auto
GlobalVariable property FEC_PoisonMode auto
GlobalVariable property FEC_PoisonOpacityAutoCalc auto

int randomChance

int magicType = -1
int spellLevel
int projectileType

int kPOISON = 0
int kFEAR = 1
int kPOISONFEAR = 2

int kEFFECT_Reset = -2
int kEFFECT_None = -1
int kEFFECT_Poisoned = 2
int kEFFECT_Frightened = 3

int kRESET_PERM = 0
int kRESET_TEMP = 1
int kRESET_FROZEN_ACTOR = 2
int kRESET_FROZEN_COL = 3

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;=======================================
;			EMPTY STATE 
;=======================================

Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
endEvent

Event OnActorResurrected(Actor akTarget, bool abResetInventory)
endEvent

Event OnSoulTrapped(Actor akVictim, Actor akKiller)
endEvent	

;=======================================
;			DEFAULT STATE 
;=======================================

Auto State Default

	Event OnEffectStart(Actor akTarget, Actor akCaster)
	
		victim = akTarget
		
		if victim 
										
			;---------------------VARIABLES----------------------------				
			victimBase = victim.GetLeveledActorBase()					
			poisonSkin = FEC_ListSkinColor.GetAt(1) as ColorForm
			isHumanoid = IsCreatureHumanoid()			
			;----------------------------------------------------------
			
			if GetTemporaryDeathEffect(victim, kEFFECT_Reset)			
				FullReset()				
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

	Event OnDying(Actor akKiller)

		if victim
					
			;---------------------VARIABLES----------------------------		
			int [] deathEffect = GetCauseOfDeath(victim, 1)
			magicType = deathEffect[0]
			spellLevel = deathEffect[1]
			projectileType = deathEffect[2]			
			randomChance = GenerateRandomInt(0,100)					
			;-----------------------------------------------------------	
						
			if 	magicType == kPOISON && randomChance <= FEC_PoisonRandomChanceCr.GetValue() as int
	
				VeinEffect()
				
			elseif (magicType == kPOISONFEAR) && randomChance <= FEC_FearRandomChanceCr.GetValue() as int
					
				VeinEffect(true)
				
			elseif (magicType == kFEAR) && randomChance <= FEC_FearRandomChanceCr.GetValue() as int && isHumanoid
					
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
		if soultrapped && randomChance <= FEC_SoulTrapRandomChanceCr.GetValue() as int							
			SoulTrapEffect()			
		endif
		
		if !soultrapped
			RegisterForSoulTrapped(self)
		endif
	
	endEvent
		
	Event OnSoulTrapped(Actor akVictim, Actor akKiller)
	
		if akVictim == victim 
		
			soultrapped = true
			
			if randomChance <= FEC_SoulTrapRandomChanceCr.GetValue() as int				
				SoulTrapEffect()				
			endif

			UnregisterForSoulTrapped(self)
						
		endif
	
	endEvent
	
endState

;=======================================
;			DONE STATE
;=======================================

State TempDone

	Event OnBeginState()
	
		UnregisterForAllModEvents()		
		UnregisterForAllFECResets(self)
		
		RegisterForFECReset(self, kRESET_TEMP)		
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
		
		if akActor == victim && aiType == kRESET_TEMP			
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

endState

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;========================================================													
	;[1]				POISON EFFECT                                                                                           
;========================================================

Function VeinEffect(bool applyFear = false)

	FEC_PoisonWispsFXS.Play(victim,4.0)
	
	if isHumanoid
		
		FEC_PoisonVeinFXS.Play(victim)
		
		BlendColorWithSkinTone(victim, poisonSkin, FEC_PoisonMode.GetValue() as int, FEC_PoisonOpacityAutoCalc.GetValue() as bool, FEC_PoisonOpacityColor.GetValue())
		
		if applyFear		
			SetHairColor(victim, WhiteHair)
			
			frightened = true
			AssignTemporaryDeathEffect(victim, kEFFECT_Frightened)
		endif
			
		poisoned = true
		AssignTemporaryDeathEffect(victim, kEFFECT_Poisoned)
	
		GoToState("TempDone")
		
	else
	
		GoToState("NotDone")
					
	endif
			
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

Function SoulTrapEffect()

	if isHumanoid
		FEC_SoulTrapSkinFXS.Play(victim, 7.0)	
	else	
		FEC_SoulTrapFXS.Play(victim, 7.0)			
	endif

endFunction
 
;========================================================													
	;[3]				FEAR EFFECT                                                                                           
;========================================================

Function FearEffect()
	
	SetHairColor(victim, WhiteHair)
	BlendColorWithSkinTone(victim, whiteSkin, 6, true, 1.0)
		
	frightened = true
	AssignTemporaryDeathEffect(victim, kEFFECT_Frightened)
	
	GoToState("TempDone")		
							
endFunction


Function ReapplyFear()

	if WaitFor3DLoad(victim)
	
		RemoveEffectsNotOfType(victim, 2)
		
		SetHairColor(victim, WhiteHair)
		BlendColorWithSkinTone(victim, whiteSkin, 6, true, 1.0)
		
	endif
					
endFunction

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                         MISC FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Function IsCreatureHumanoid()

	int i = 0
	int iFaction = BipedalCreatureFactions.Length 
	Faction akFaction

	while  i < iFaction
		akFaction = (BipedalCreatureFactions[i])
		if victim.IsInFaction(akFaction)
			return true
		endif
		i += 1
	endwhile
	
	return false
	
endFunction

bool Function CheckTempSerialization()


	if poisoned
		AssignTemporaryDeathEffect(victim, kEFFECT_Poisoned)	
	elseif GetTemporaryDeathEffect(victim, kEFFECT_Poisoned)	
		poisoned = true
	endif
	
	if frightened
		AssignTemporaryDeathEffect(victim, kEFFECT_Frightened)
	elseif GetTemporaryDeathEffect(victim, kEFFECT_Poisoned)
		frightened = true
	endif
	
	return poisoned || frightened
	
endFunction

Function FullReset(bool goToDefault = false)
		
	poisoned = false
	RemoveTemporaryDeathEffect(victim, kEFFECT_Poisoned)	
		
	frightened = false
	RemoveTemporaryDeathEffect(victim, kEFFECT_Frightened)	
	
	soultrapped = false
	
	if goToDefault	
		GoToState("Default")
	else
		GoToState("NotDone")
	endif

endFunction