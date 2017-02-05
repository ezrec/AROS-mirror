VERSION		EQU	0
REVISION	EQU	1

DATE	MACRO
		dc.b '5.11.2008'
		ENDM

VERS	MACRO
		dc.b 'amr.datatype 0.1'
		ENDM

VSTRING	MACRO
		dc.b 'amr.datatype 0.1 (5.11.2008)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: amr.datatype 0.1 (5.11.2008)',0
		ENDM
