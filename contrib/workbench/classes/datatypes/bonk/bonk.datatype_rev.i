VERSION		EQU	0
REVISION	EQU	2

DATE	MACRO
		dc.b '18.12.2008'
		ENDM

VERS	MACRO
		dc.b 'bonk.datatype 0.2'
		ENDM

VSTRING	MACRO
		dc.b 'bonk.datatype 0.2 (18.12.2008)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: bonk.datatype 0.2 (18.12.2008)',0
		ENDM
