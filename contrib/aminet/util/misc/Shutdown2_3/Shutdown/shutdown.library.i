VERSION		EQU	2
REVISION	EQU	3
DATE	MACRO
		dc.b	'23.7.93'
	ENDM
VERS	MACRO
		dc.b	'shutdown.library 2.3'
	ENDM
VSTRING	MACRO
		dc.b	'shutdown.library 2.3 (23.7.93)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: shutdown.library 2.3 (23.7.93)',0
	ENDM
