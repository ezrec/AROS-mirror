VERSION		EQU	0
REVISION	EQU	2

DATE	MACRO
		dc.b '29.7.2008'
		ENDM

VERS	MACRO
		dc.b 'jpeg2000.datatype 0.2'
		ENDM

VSTRING	MACRO
		dc.b 'jpeg2000.datatype 0.2 (29.7.2008)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: jpeg2000.datatype 0.2 (29.7.2008)',0
		ENDM
