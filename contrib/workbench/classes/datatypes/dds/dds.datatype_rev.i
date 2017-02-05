VERSION		EQU	52
REVISION	EQU	2

DATE	MACRO
		dc.b '19.6.2008'
		ENDM

VERS	MACRO
		dc.b 'dds.datatype 52.2'
		ENDM

VSTRING	MACRO
		dc.b 'dds.datatype 52.2 (19.6.2008)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: dds.datatype 52.2 (19.6.2008)',0
		ENDM
