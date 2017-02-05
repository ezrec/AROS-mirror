VERSION		EQU	0
REVISION	EQU	2

DATE	MACRO
		dc.b '28.11.2006'
		ENDM

VERS	MACRO
		dc.b 'targa.datatype 0.2'
		ENDM

VSTRING	MACRO
		dc.b 'targa.datatype 0.2 (28.11.2006)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: targa.datatype 0.2 (28.11.2006)',0
		ENDM
