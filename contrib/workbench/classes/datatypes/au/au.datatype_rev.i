VERSION		EQU	50
REVISION	EQU	1

DATE	MACRO
		dc.b '27.2.2006'
		ENDM

VERS	MACRO
		dc.b 'au.datatype 50.1'
		ENDM

VSTRING	MACRO
		dc.b 'au.datatype 50.1 (27.2.2006)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: au.datatype 50.1 (27.2.2006)',0
		ENDM
