
	incdir	include:
	include	libraries/ahi_sub.i

TRUE		EQU	1
FALSE		EQU	0

BEG:

*** FORM AHIM
	dc.l	ID_FORM
	dc.l	E-S
S:
	dc.l	ID_AHIM


*** AUDN
DrvName:
	dc.l	ID_AUDN
	dc.l	.e-.s
.s
	dc.b	"void",0
.e
	CNOP	0,2


*** AUDM
ModeA:
	dc.l	ID_AUDM
	dc.l	.e-.s
.s
	dc.l	AHIDB_AudioID,		$001F0001

	dc.l	AHIDB_Volume,		TRUE
	dc.l	AHIDB_Panning,		TRUE
	dc.l	AHIDB_Stereo,		TRUE
	dc.l	AHIDB_HiFi,		TRUE
	dc.l	AHIDB_MultTable,	FALSE

	dc.l	AHIDB_Name,		.name-.s
	dc.l	TAG_DONE
.name	dc.b	"Void:HiFi 16 bit stereo++",0
.e
	CNOP	0,2


*** AUDM
ModeB:
	dc.l	ID_AUDM
	dc.l	.e-.s
.s
	dc.l	AHIDB_AudioID,		$001F0002

	dc.l	AHIDB_Volume,		TRUE
	dc.l	AHIDB_Panning,		FALSE
	dc.l	AHIDB_Stereo,		FALSE
	dc.l	AHIDB_HiFi,		TRUE
	dc.l	AHIDB_MultTable,	FALSE

	dc.l	AHIDB_Name,		.name-.s
	dc.l	TAG_DONE
.name	dc.b	"Void:HiFi 16 bit mono",0
.e
	CNOP	0,2

*** AUDM
ModeC:
	dc.l	ID_AUDM
	dc.l	.e-.s
.s
	dc.l	AHIDB_AudioID,		$001F0003

	dc.l	AHIDB_Volume,		TRUE
	dc.l	AHIDB_Panning,		TRUE
	dc.l	AHIDB_Stereo,		TRUE
	dc.l	AHIDB_HiFi,		FALSE
	dc.l	AHIDB_MultTable,	FALSE

	dc.l	AHIDB_Name,		.name-.s
	dc.l	TAG_DONE
.name	dc.b	"Void:16 bit stereo++",0
.e
	CNOP	0,2


*** AUDM
ModeD:
	dc.l	ID_AUDM
	dc.l	.e-.s
.s
	dc.l	AHIDB_AudioID,		$001F0004

	dc.l	AHIDB_Volume,		TRUE
	dc.l	AHIDB_Panning,		FALSE
	dc.l	AHIDB_Stereo,		FALSE
	dc.l	AHIDB_HiFi,		FALSE
	dc.l	AHIDB_MultTable,	FALSE

	dc.l	AHIDB_Name,		.name-.s
	dc.l	TAG_DONE
.name	dc.b	"Void:16 bit mono",0
.e
	CNOP	0,2

E:
	CNOP	0,2
END:
