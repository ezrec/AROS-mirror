#ifndef SCALOS_PREFERENCES_H
#define SCALOS_PREFERENCES_H
/*
**	$VER: preferences.h 39.2 (28.5.97)
**
**      preferences.library include
**
**	(C) Copyright 1996-1997 ALiENDESiGN
**	All Rights Reserved
*/

struct ScalosPrefsStruct {
	UWORD		ps_Size;
};

// -- Scalos specific Prefs:

// ------------------ ScalosPrefs -------------------------

// Name: "Scalos" or "ScalosPrefs"
// ID: "MAIN"

#define SCP_IconOffsets				0x80000001	// struct [8]
#define SCP_IconNormFrame			0x80000002	// word
#define SCP_IconSelFrame			0x80000003	// word
#define SCP_IconTextMode			0x80000004	// word
#define SCP_IconSecLine				0x80000005	// word
#define SCP_IconTextSkip			0x80000006	// word

#define SCP_BobsType					0x80000010	// byte	image or image+text
#define SCP_BobsMethod				0x80000011	// byte	system or custom
#define SCP_BobsTranspMode			0x80000012	// byte	solid or transparent
#define SCP_BobsTranspType			0x80000013	// byte	ghosted or real transparent
#define SCP_BobsTransp				0x80000014	// long	when transparent

#define SCP_ScreenTitle				0x80000020	// string
#define SCP_RootWinTitle			0x80000021	// string
#define SCP_WindowTitle				0x80000022	// string
#define SCP_Separator				0x80000023	// byte
#define SCP_TitleRefresh			0x80000024	// byte

#define SCP_PathsDefIcons			0x80000025	// string
#define SCP_PathsDiskCopy			0x80000026	// string
#define SCP_PathsWBStartup			0x80000027	// string
#define SCP_PathsHome				0x80000028	// string

#define SCP_MiscAutoRemove			0x80000029	// byte
#define SCP_MiscClickTransp		0x8000002a	// byte
#define SCP_MiscHardEmulation		0x8000002b	// byte
#define SCP_MiscUseExAll			0x8000002c	// byte
#define SCP_MiscWindowType			0x8000002d	// byte
#define SCP_MiscDoWaitDelay		0x8000002e	// byte
#define SCP_MiscDiskiconsRefresh	0x8000002f	// byte	delay
#define SCP_MiscMenuCurrentDir	0x80000030	// byte

#define SCP_NewIconsTransparent	0x80000031	// byte
#define SCP_NewIconsPrecision		0x80000032	// long

#define SCP_TextModeFont			0x80000033	// string
#define SCP_TextModeDateFormat	0x80000034	// byte

#define SCP_PlugInList				0x80000035	// list


#endif /* SCALOS_PREFERENCES_H */
