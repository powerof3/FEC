Scriptname FEC_Utility Hidden 

import po3_SKSEFunctions

;----------------------------------------------------------------------------------------------------------
;NATIVE
;----------------------------------------------------------------------------------------------------------
	
	;FEC function
	;returns effect type, effect skill level, and projectile type, of the highest magnitude effect present on the actor
	;permanent - SUN, ACID, FIRE, FROST, SHOCK, DRAIN
	;temporary - POISON, FEAR
	int[] Function GetCauseOfDeath(Actor akActor, int type) global native
	
	;0 - charred/skeleton
	;1 - drained
	;2 - poisoned/frightened
	;3-  aged
	;4 - charred creature
	;5 - frozen
	Function RemoveEffectsNotOfType(Actor akActor, int aiEffectType) global native
	
	; 0 - permanent
	; 1 - temporary
	; 2 - frozenActor
	; 3 - frozenCol
	Function SendFECResetEvent(Actor akActor, int aiType, bool abReset3D) global native
	
	Function VaporizeUnderwear(Actor akVictim) global native
	
	bool Function GetPermanentDeathEffect(Actor akActor, int aiEffectType) global native
	bool Function GetTemporaryDeathEffect(Actor akActor, int aiEffectType) global native
	
	Function AssignPermanentDeathEffect(Actor akActor, int aiEffectType) global native
	Function AssignTemporaryDeathEffect(Actor akActor, int aiEffectType) global native
	Function RemoveTemporaryDeathEffect(Actor akActor, int aiEffectType) global native
		
	Function RegisterForFECReset(ActiveMagicEffect akActiveEffect, int aiType) global native	
	Function UnregisterForFECReset(ActiveMagicEffect akActiveEffect, int aiType) global native
	Function UnregisterForAllFECResets(ActiveMagicEffect akActiveEffect) global native
	
	Function RegisterForFECReset_Form(Form akForm, int aiType) global native
	Function UnregisterForFECReset_Form(Form akForm, int aiType) global native
	Function UnregisterForAllFECResets_Form(Form akForm) global native			
		
	Event OnFECReset(Actor akActor, int aiType, bool abReset3D)
	EndEvent

;----------------------------------------------------------------------------------------------------------
;EFFECT
;----------------------------------------------------------------------------------------------------------

Function CookMeat(Actor akVictim, FormList akRawList, FormList akCookedList) global

	int iR = 0
	int iC = 0
	int foodCount
	
	while (iR < akRawList.GetSize()) && (iC < akCookedList.GetSize())
		foodCount = akVictim.GetItemCount(akRawList.GetAt(iR))
		if foodCount > 0
			akVictim.RemoveItem(akRawList.GetAt(iR), foodCount)
			akVictim.AddItem(akCookedList.GetAt(iC), foodCount)
		endif
		iR += 1
		iC += 1
	endwhile
	
endFunction

Function CreateAshPileEffect(Actor akVictim, EffectShader disintegrateFXS, Activator akAshPile, float startTime = 1.25, float endTime = 1.65) global
	
	StopAllShaders(akVictim)
	
	Utility.Wait(0.1)
	
	akVictim.SetCriticalStage(akVictim.CritStage_DisintegrateStart)
	
	disintegrateFXS.Play(akVictim)

	Utility.Wait(startTime)	

	akVictim.AttachAshPile(akAshPile)

	Utility.Wait(endTime)

	disintegrateFXS.Stop(akVictim)
	
	akVictim.SetAlpha (0.0,true)
		
	akVictim.SetCriticalStage(akVictim.CritStage_DisintegrateEnd)		
		
endFunction

Function EquipHead(Actor akVictim, Armor Head, Armor DecapHead, bool equipDecap, bool toggleGore) global

	if !IsLimbGone(akVictim, 1) 
		akVictim.EquipItem(Head)
	else
		if equipDecap
			akVictim.EquipItem(DecapHead)
		endif
		if toggleGore
			ToggleChildNode(akVictim, "NeckGore", true)
			ToggleChildNode(akVictim, "HeadGore", true)
		endif
	endif
	
endFunction

Function EquipUnderSkin(Actor akVictim, Int aiSlot, Armor akEquip1, Armor akEquip2 = None) global

	Armor akCheck = akVictim.GetEquippedArmorInSlot(aiSlot)
	if !akCheck
		if akEquip2
			akVictim.EquipItem(akEquip2)
		else
			akVictim.EquipItem(akEquip1)
		endif
	elseif HasSkin(akVictim, akCheck)
		akVictim.EquipItem(akEquip1)
	endif
	
endFunction

float Function GetScaleDiff(Actor akVictim) global

	float victimHeight = akVictim.GetHeight()
	if victimHeight == 0.0
		return 1.0
	endif
	return victimHeight/128.0
	
endFunction

bool Function GetFrostEffect(MagicEffect[] mgefs, int spellLevel, int projectileType) global

	int i = 0
	int iLength = mgefs.Length 

	while  i < iLength
		MagicEffect mgef = mgefs[i]
		if mgef && mgef.IsEffectFlagSet(0x00000001) && mgef.HasKeywordString("MagicDamageFrost")
			spellLevel = mgef.GetSkillLevel()
			Projectile akProjectile = mgef.GetProjectile()
			if akProjectile	
				projectileType = GetProjectileType(akProjectile)
			endif				
			return true
		endif
		i += 1
	endwhile
	
	return false

endFunction

bool Function IsAcidEffect(MagicEffect akEffect) global

	return akEffect.GetResistance() == "PoisonResist" && akEffect.GetCastingType() == 2
	
endFunction

bool Function IsDisintegrateProof(Actor akVictim) global

	return akVictim.HasKeywordString("MagicNoDistintegrate") || akVictim.HasKeywordString("ActorTypeGhost")

endFunction

bool Function IsValidMagicEffect(MagicEffect akEffect, ObjectReference akCaster, Actor akVictim) global

	if (!akEffect || !akCaster || !akVictim)
		return false;
	endif
	
	return akEffect.IsEffectFlagSet(0x00000001) && akEffect.IsEffectFlagSet(0x00000004) && akEffect.GetCastingType() != 0 && EvaluateConditionList(akEffect, akCaster, akVictim)

endFunction

ObjectReference Function PlaceBodyParts(Actor akVictim, MiscObject[] akParts, int aiType, int aiSex) global

	if aiType == 0
		if aiSex == 0
			return akVictim.PlaceAtMe(akParts[0])	
		else
			return akVictim.PlaceAtMe(akParts[1])	
		endif
	elseif aiType == 1
		if aiSex == 0
			return akVictim.PlaceAtMe(akParts[2])	
		else
			return akVictim.PlaceAtMe(akParts[3])
		endif
	else
		if aiSex == 0
			return akVictim.PlaceAtMe(akParts[4])	
		else
			return akVictim.PlaceAtMe(akParts[5])	
		endif
	endif
	
	return None
	
endFunction

bool Function ResetActor3D_FEC(Actor akVictim, bool abRemoveArmor = true) global
	

	if abRemoveArmor				
		utility.Wait(0.10) ;wait for any inventory resets
		RemoveAllModItems(akVictim, "FEC.esp", false)
	endif
	if WaitFor3DLoad(akVictim)
		return ResetActor3D(akVictim, "po3_FEC")
	endif
	
	return false
	
endFunction

Function ScaleTriggerArt(Actor akVictim, float scaleDiff, string nodeName) global

	if scaleDiff != 1.0 && akVictim.Is3DLoaded()
		int count = 0
		while !NetImmerse.HasNode(akVictim, nodeName, false) && count < 50
			Utility.Wait(0.1)
			count += 1
		endwhile
		ScaleObject3D(akVictim, nodeName, scaleDiff)
	endif
	
endFunction

bool Function WaitFor3DLoad(ObjectReference akRef, int timeOut = 50) global

	if !akRef
		return false
	endif
	
	int i = 0
	while !akRef.Is3DLoaded() && i < timeOut
		Utility.Wait(0.1)
		i += 1
	endwhile

	return akRef.Is3DLoaded()

endFunction