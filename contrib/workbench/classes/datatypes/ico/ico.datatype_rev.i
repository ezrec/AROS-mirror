VERSION		EQU	52
REVISION	EQU	1

DATE	MACRO
		dc.b '29.7.2008'
		ENDM

VERS	MACRO
		dc.b 'ico.datatype 52.1'
		ENDM

VSTRING	MACRO
		dc.b 'ico.datatype 52.1 (29.7.2008)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: ico.datatype 52.1 (29.7.2008)',0
		ENDM
