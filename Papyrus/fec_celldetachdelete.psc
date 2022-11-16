Scriptname FEC_CellDetachDelete extends ObjectReference  

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							PROPERTIES
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Sound property loadSound auto

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Event OnLoad()

	If loadSound

		loadSound.Play(self)

	endif
	
endEvent


Event OnCellDetach()

	Disable()
	Delete()
	
endEvent
