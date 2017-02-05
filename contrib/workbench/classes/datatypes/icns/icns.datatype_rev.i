VERSION		EQU	52
REVISION	EQU	3

DATE	MACRO
		dc.b '26.7.2008'
		ENDM

VERS	MACRO
		dc.b 'icns.datatype 52.3'
		ENDM

VSTRING	MACRO
		dc.b 'icns.datatype 52.3 (26.7.2008)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: icns.datatype 52.3 (26.7.2008)',0
		ENDM
