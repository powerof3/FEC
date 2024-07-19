Scriptname FEC_FrozenCollisionCreatureTrigger extends ObjectReference  
;creatures

import PO3_SKSEFunctions
import PO3_Events_Form

import FEC_Utility

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							PROPERTIES
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Actor victim

bool magicLock = false
bool triggerLock = false
bool globalLock = false

EffectShader property FEC_FrostFXS auto  

Form [] EquippedArray

GlobalVariable property FEC_CollisionScaleToggle auto

Keyword property MagicDamageFire auto
Keyword property MagicSummonUndead auto

Projectile property VoicePushProjectile02 auto
Projectile property VoicePushProjectile03 auto

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;=======================================
;			EMPTY STATE 
;=======================================

Event OnFECReset(Actor akActor, int aiType, bool abReset3D) ;EMPTY
endEvent

Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied)
endEvent

;=======================================
;			DEFAULT STATE 
;=======================================

auto State Default

	Event OnCellAttach()
												
		if globalLock
			return
		endif
		
		globalLock = true
		
		if WaitFor3DLoad(self)
		
			if !victim				
				victim = GetClosestActorFromRef(self, true)				
			endif
			
			if victim && WaitFor3DLoad(victim)
				
				SetupFrozenActor()
								
			endif
			
		endif
		
		globalLock =  false
	
	endEvent
	
	Event OnLoad()
							
		if globalLock
			return
		endif
		
		globalLock = true
		
		if !victim			
			victim = GetClosestActorFromRef(self, true)			
		endif
		
		if victim && WaitFor3DLoad(victim)
			
			SetupFrozenActor()
							
		endif

		globalLock =  false
	
	endEvent
		
	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied)
			
		if magicLock
			return
		endif
		
		magicLock = true
		
		if GetEffectArchetypeAsString(akEffect) == "Reanimate" || akEffect.HasKeyword(MagicDamageFire)
		
			GoToState("Clear")
										
		endif
			
		magicLock = false
			
	endEvent

	Event OnTriggerEnter(ObjectReference akActionRef)
	
		if triggerLock
			return
		endif
		
		triggerLock = true
			
		Actor akActor = akActionRef as Actor
			
		if akActor && akActor.GetAnimationVariableFloat("Speed") >= 1500.0 || akActionRef.GetBaseObject() == VoicePushProjectile02 || akActionRef.GetBaseObject() == VoicePushProjectile03
								
			GoToState("Clear")
				
		endif
										
		triggerLock = false
		
	endEvent	
		
	Event OnCellDetach()
					
		GoToState("Clear")
				
	endEvent
		
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D) ;Sent by actor to collision, no need to send shatter event here
										
		if akActor == victim && (aiType == 0 || aiType == 3)
										
			GoToState("Clear")
				
		endif
	
	endEvent

endState

;=======================================

;			CLEAR STATE 

;=======================================

State Clear

	Event OnBeginState()
						
		if victim.Is3DLoaded()
			FEC_FrostFXS.Stop(victim)
		endif
																											
		ClearActor()
	
	endEvent

	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY
	endEvent
		
	Event OnTriggerEnter(ObjectReference akActionRef) ;EMPTY
	endEvent	
		
	Event OnCellDetach() ;EMPTY
	endEvent

	Event OnFECReset(Actor akActor, int aiType, bool abReset3D) ;EMPTY
	endEvent
	
endState
	
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;                          FUNCTIONS

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Function ClearActor()

	magicLock = true
	triggerLock = true
								
	if !victim		
		victim = GetClosestActorFromRef(self, true)		
	endif
				
	victim.SetDisplayName(victim.GetLeveledActorBase().GetName())
		
	SendFECResetEvent(victim, 2, false)
	
	RemoveAllItems(victim, true, true)
						
	Disable()
	Delete()
	
endFunction

Function SetupFrozenActor()

	if !HasNiExtraData(self, "PO3_SCALE")
		float scaleDiff = GetScaleDiff(victim)
		if scaleDiff != 1.0 && FEC_CollisionScaleToggle.GetValue() == 1.0 
			ScaleObject3D(self, "", scaleDiff)
		endif
	endif			
	
	SetDisplayName(victim.GetLeveledActorBase().GetName())
	
	RegisterForFECReset_Form(self, 0)
	RegisterForFECReset_Form(self, 3)
	
	RegisterForMagicEffectApplyEx(self, MagicDamageFire, true)
	RegisterForMagicEffectApplyEx(self, MagicSummonUndead, true)
	
endFunction