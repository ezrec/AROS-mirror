VERSION		EQU	2
REVISION	EQU	2
DATE	MACRO
		dc.b	'25.6.92'
	ENDM
VERS	MACRO
		dc.b	'ShutdownSetup 2.2'
	ENDM
VSTRING	MACRO
		dc.b	'ShutdownSetup 2.2 (25.6.92)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: ShutdownSetup 2.2 (25.6.92)',0
	ENDM
