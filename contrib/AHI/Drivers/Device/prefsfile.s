
	incdir	include:
	include	libraries/ahi_sub.i

TRUE		EQU	1
FALSE		EQU	0

AHIDB_DeviceUnit	EQU	AHIDB_UserBase+0	;Private tag

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
	dc.l	AHIDB_AudioID,		$00030001

	dc.l	AHIDB_DeviceUnit,	0

	dc.l	AHIDB_Volume,		TRUE
	dc.l	AHIDB_Panning,		TRUE
	dc.l	AHIDB_Stereo,		TRUE
	dc.l	AHIDB_HiFi,		TRUE
	dc.l	AHIDB_MultTable,	FALSE

	dc.l	AHIDB_Name,		.name-.s
	dc.l	TAG_DONE
.name	dc.b	"Device:Unit 0",0
.e
	CNOP	0,2

*** AUDM
ModeB:
	dc.l	ID_AUDM
	dc.l	.e-.s
.s
	dc.l	AHIDB_AudioID,		$00030002

	dc.l	AHIDB_DeviceUnit,	1

	dc.l	AHIDB_Volume,		TRUE
	dc.l	AHIDB_Panning,		TRUE
	dc.l	AHIDB_Stereo,		TRUE
	dc.l	AHIDB_HiFi,		TRUE
	dc.l	AHIDB_MultTable,	FALSE

	dc.l	AHIDB_Name,		.name-.s
	dc.l	TAG_DONE
.name	dc.b	"Device:Unit 1",0
.e
	CNOP	0,2


*** AUDM
ModeC:
	dc.l	ID_AUDM
	dc.l	.e-.s
.s
	dc.l	AHIDB_AudioID,		$00030003

	dc.l	AHIDB_DeviceUnit,	2

	dc.l	AHIDB_Volume,		TRUE
	dc.l	AHIDB_Panning,		TRUE
	dc.l	AHIDB_Stereo,		TRUE
	dc.l	AHIDB_HiFi,		TRUE
	dc.l	AHIDB_MultTable,	FALSE

	dc.l	AHIDB_Name,		.name-.s
	dc.l	TAG_DONE
.name	dc.b	"Device:Unit 2",0
.e
	CNOP	0,2


*** AUDM
ModeD:
	dc.l	ID_AUDM
	dc.l	.e-.s
.s
	dc.l	AHIDB_AudioID,		$00030004

	dc.l	AHIDB_DeviceUnit,	3

	dc.l	AHIDB_Volume,		TRUE
	dc.l	AHIDB_Panning,		TRUE
	dc.l	AHIDB_Stereo,		TRUE
	dc.l	AHIDB_HiFi,		TRUE
	dc.l	AHIDB_MultTable,	FALSE

	dc.l	AHIDB_Name,		.name-.s
	dc.l	TAG_DONE
.name	dc.b	"Device:Unit 3",0
.e
	CNOP	0,2

E:
	CNOP	0,2
END:
