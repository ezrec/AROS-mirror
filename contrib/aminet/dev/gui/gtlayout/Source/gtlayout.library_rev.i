VERSION		EQU	45
REVISION	EQU	1
DATE	MACRO
		dc.b	'11.7.98'
	ENDM
VERS	MACRO
		dc.b	'gtlayout.library 45.1'
	ENDM
VSTRING	MACRO
		dc.b	'gtlayout.library 45.1 (11.7.98)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: gtlayout.library 45.1 (11.7.98)',0
	ENDM
