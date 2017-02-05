VERSION		EQU	0
REVISION	EQU	3

DATE	MACRO
		dc.b '28.11.2006'
		ENDM

VERS	MACRO
		dc.b 'pcx.datatype 0.3'
		ENDM

VSTRING	MACRO
		dc.b 'pcx.datatype 0.3 (28.11.2006)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: pcx.datatype 0.3 (28.11.2006)',0
		ENDM
