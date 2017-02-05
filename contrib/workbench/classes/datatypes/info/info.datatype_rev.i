VERSION		EQU	52
REVISION	EQU	2

DATE	MACRO
		dc.b '26.8.2008'
		ENDM

VERS	MACRO
		dc.b 'info.datatype 52.2'
		ENDM

VSTRING	MACRO
		dc.b 'info.datatype 52.2 (26.8.2008)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: info.datatype 52.2 (26.8.2008)',0
		ENDM
