Scriptname FEC_CellDetachContainer extends ObjectReference  

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;							EVENTS
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Event OnCellDetach()

	if !po3_sksefunctions.IsQuestItem(self)
	
		RemoveAllItems()
		Disable()
		MarkForDelete()
		
	endif

endEvent