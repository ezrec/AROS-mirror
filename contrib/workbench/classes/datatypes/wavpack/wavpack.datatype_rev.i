VERSION		EQU	0
REVISION	EQU	1

DATE	MACRO
		dc.b '18.12.2008'
		ENDM

VERS	MACRO
		dc.b 'wavpack.datatype 0.1'
		ENDM

VSTRING	MACRO
		dc.b 'wavpack.datatype 0.1 (18.12.2008)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: wavpack.datatype 0.1 (18.12.2008)',0
		ENDM
