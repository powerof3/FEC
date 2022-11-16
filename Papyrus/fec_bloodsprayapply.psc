Scriptname FEC_BloodSprayApply extends activemagiceffect  

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							PROPERTIES
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Spell[] property bloodSpray auto

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Event OnEffectStart(Actor target, Actor caster)

	bloodSpray[po3_SKSEFunctions.GenerateRandomInt(0, bloodSpray.length)].Cast(target)
	
endEvent
 