Scriptname FEC_FrozenCollisionTrigger extends ObjectReference  

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

GlobalVariable property FEC_FrostShatterToggle auto
GlobalVariable property FEC_CollisionScaleToggle auto

Keyword property MagicDamageFrost auto
Keyword property MagicDamageFire auto
Keyword property MagicSummonUndead auto

Projectile property VoicePushProjectile02 auto
Projectile property VoicePushProjectile03 auto


;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

;=======================================
;			EMPTY STATE 
;=======================================

Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
endEvent

Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied)
endEvent

;=======================================
;			DEFAULT STATE 
;=======================================

Auto State Default

	Event OnCellAttach()
										
		if globalLock
			return
		endif
		
		globalLock = true
		
		if !victim		
			victim = GetClosestActorFromRef(self, true)	
		endif
		
		if WaitFor3DLoad(self)
									
			if victim && WaitFor3DLoad(victim)
				
				SetupFrozenActor()	
									
			endif
			
		endif
		
		globalLock = false
		
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
		
		globalLock = false
		
	endEvent

	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied)
					
		if magicLock
			return
		endif
		
		magicLock = true
		
		if !victim		
			victim = GetClosestActorFromRef(self, true)	
		endif
				
		if victim
							
			if GetEffectArchetypeAsString(akEffect) == "Reanimate"
																															
				ClearActor(false)	

				EquippedArray = AddAllEquippedItemsToArray(victim)	;removing items while AI is disabled causes naked bug
				int iIndex = EquippedArray.Length
				while iIndex
					iIndex -= 1
					RemoveItem(EquippedArray[iIndex], 1, true)  
				endwhile		
				RemoveAllItems(victim, true, true)			
											
				Disable()
				Delete()
				
			elseif akEffect.HasKeyword(MagicDamageFire)
								
				if victim.Is3DLoaded()
					FEC_FrostFXS.Stop(victim)
				endif
											
				ClearActor(false)
				
				EquippedArray = AddAllEquippedItemsToArray(victim)	
				int iIndex = EquippedArray.Length
				while iIndex
					iIndex -= 1
					RemoveItem(EquippedArray[iIndex], 1, true)  
				endwhile		
				RemoveAllItems(victim, true, true)			
																										
				Disable()
				Delete()
						
			elseif akEffect.HasKeyword(MagicDamageFrost)
			
				int projectileType = -1
				Projectile akProjectile = akEffect.GetProjectile()
				if akProjectile	
					projectileType = GetProjectileType(akProjectile)
				endif
							
				if FEC_FrostShatterToggle.GetValue() == 1.0 && projectileType == 1
										
					ClearActor(true)
					GoToState("Shatter")
										
				endif
									
			endif
			
		endif
			
		magicLock = false
			
	endEvent

	Event OnTriggerEnter(ObjectReference akActionRef)
				
		if triggerLock
			return
		endif
		
		triggerLock = true
				
		if victim
			
			Actor akActor = akActionRef as Actor
				
			if akActor && akActor.GetAnimationVariableFloat("Speed") >= 1500.0 || akActionRef.GetBaseObject() == VoicePushProjectile02 || akActionRef.GetBaseObject() == VoicePushProjectile03
						
				if FEC_FrostShatterToggle.GetValue() == 1.0 
								
					ClearActor(true)
					GoToState("Shatter")
					
				else
				
					GoToState("Clear")
					
				endif
					
			endif

		endif
			
		triggerLock = false
		
	endEvent
	
	Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
							
		if victim && !victim.IsAIEnabled()
				
			if victim.IsEquipped(akBaseItem) && akBaseItem.IsPlayable()
		
				victim.RemoveItem(akBaseItem)
					
			endif
			
		endif
	
	endEvent
		
	Event OnCellDetach()
						
		GoToState("Clear")

	endEvent
	
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D) ;Sent by actor to collision, no need to send shatter event here
										
		if akActor == victim
					
			if aiType == 3
			
				if abReset3D			
					GoToState("Shatter")				
				else			
					GoToState("Clear")
				endif
				
			elseif aiType == 0
			
				GoToState("Clear")
				
			endif
			
		endif			
					
	endEvent

endState

;=======================================
;			CLEAR STATE 
;======================================

State Clear

	Event OnBeginState()
																															
		magicLock = true;
		triggerLock = true;
		
		if !victim		
			victim = GetClosestActorFromRef(self, true)		
		endif
		
		ClearActor(false)
		
		EquippedArray = AddAllEquippedItemsToArray(victim)	
		int iIndex = EquippedArray.Length
		while iIndex
			iIndex -= 1
			RemoveItem(EquippedArray[iIndex], 1, true)  
		endwhile			
		RemoveAllItems(victim, true)
				
		Disable()
		Delete()
	
	endEvent

	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY
	endEvent
		
	Event OnTriggerEnter(ObjectReference akActionRef) ;EMPTY
	endEvent	
	
	Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
	endEvent	
	
	Event OnCellDetach() ;EMPTY
	endEvent

	Event OnFECReset(Actor akActor, int aiType, bool abReset3D);EMPTY
	endEvent
	
endState

;=======================================
;			SHATTER STATE 
;=======================================

State Shatter

	Event OnBeginState()
																															
		magicLock = true;
		triggerLock = true;
		
		if !victim		
			victim = GetClosestActorFromRef(self, true)		
		endif
		
		EquippedArray = AddAllEquippedItemsToArray(victim)	
		int iIndex = EquippedArray.Length
		while iIndex
			iIndex -= 1
			RemoveItem(EquippedArray[iIndex], 1, true)  
		endwhile			
		RemoveAllItems(victim, true)
						
		Disable()
		Delete()
	
	endEvent

	Event OnMagicEffectApplyEx(ObjectReference akCaster, MagicEffect akEffect, Form akSource, bool abApplied) ;EMPTY
	endEvent
		
	Event OnTriggerEnter(ObjectReference akActionRef) ;EMPTY
	endEvent	
	
	Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
	endEvent	
	
	Event OnCellDetach() ;EMPTY
	endEvent

	Event OnFECReset(Actor akActor, int aiType, bool abReset3D);EMPTY
	endEvent
	
endState

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;                          FUNCTIONS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Function SetupFrozenActor()

	if !HasNiExtraData(self, "PO3_SCALE")
		float scaleDiff = GetScaleDiff()
		if scaleDiff != 1.0 && FEC_CollisionScaleToggle.GetValue() == 1.0 
			ScaleObject3D(self, "", scaleDiff)
		endif
	endif

	SetDisplayName(victim.GetLeveledActorBase().GetName())			
	
	RegisterForFECReset_Form(self, 0)
	RegisterForFECReset_Form(self, 3)
	
	RegisterForMagicEffectApplyEx(self, MagicDamageFire, true)
	RegisterForMagicEffectApplyEx(self, MagicDamageFrost, true)
	RegisterForMagicEffectApplyEx(self, MagicSummonUndead, true)
	
endFunction

float Function GetScaleDiff()

	float victimHeight = victim.GetHeight()
	if victimHeight == 0.0
		return 1
	endif
	return victimHeight/128.0
	
endFunction

Function ClearActor(bool shatter)

	if victim
		
		victim.SetDisplayName(victim.GetLeveledActorBase().GetName())
		
		SendFECResetEvent(victim, 2, shatter)
		
	endif
	
endFunction