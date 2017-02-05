VERSION		EQU	52
REVISION	EQU	1

DATE	MACRO
		dc.b '2.5.2009'
		ENDM

VERS	MACRO
		dc.b 'maud.datatype 52.1'
		ENDM

VSTRING	MACRO
		dc.b 'maud.datatype 52.1 (2.5.2009)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: maud.datatype 52.1 (2.5.2009)',0
		ENDM
