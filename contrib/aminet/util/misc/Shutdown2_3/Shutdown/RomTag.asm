		INCLUDE "exec/types.i"
		INCLUDE	"exec/nodes.i"
		INCLUDE "exec/resident.i"

		INCLUDE	"shutdown.library.i"

		XREF	_LibInitTab
		XREF	_LibName
		XREF	_LibID

		SECTION	text,CODE

		CNOP	4

		MOVEQ	#0,D0		; word
		RTS			; word

InitDesc:	DC.W	RTC_MATCHWORD
		DC.L	InitDesc
		DC.L	EndCode
		DC.B	RTF_AUTOINIT	; yet auto-init
		DC.B	VERSION		; version
		DC.B	NT_LIBRARY
		DC.B	0		; priority
		DC.L	_LibName
		DC.L	_LibID
		DC.L	_LibInitTab	; ptr to init table
EndCode:
		END
