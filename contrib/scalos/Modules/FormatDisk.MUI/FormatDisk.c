// FormatDisk.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/alerts.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/filehandler.h>
#include <resources/filesysres.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/classusr.h>
#include <libraries/mui.h>
#include <mui/NListview_mcc.h>
#include <workbench/startup.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <proto/muimaster.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include "FormatDisk.h"

#define	FormatDiskMUI_NUMBERS
#define	FormatDiskMUI_ARRAY
#define	FormatDiskMUI_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#define	VERSION_MAJOR	40
#define	VERSION_MINOR	1

//----------------------------------------------------------------------------

#define MAX_DEV_NAME	128

struct FileSystemName
	{
	ULONG fsn_DosType;	// DosType
	CONST_STRPTR fsn_Name;
	};

#define KeyButtonHelp(name,key,HelpText)\
	TextObject,\
		ButtonFrame,\
		MUIA_Font         , MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , key,\
		MUIA_ControlChar  , key,\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp    , HelpText,\
		MUIA_CycleChain   , TRUE,\
		End

#define CheckMarkHelp(selected, HelpTextID)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_InputMode        , MUIV_InputMode_Toggle,\
		MUIA_Image_Spec       , MUII_CheckMark,\
		MUIA_Image_FreeVert   , TRUE,\
		MUIA_Selected         , selected,\
		MUIA_Background       , MUII_ButtonBack,\
		MUIA_ShowSelState     , FALSE,\
		MUIA_CycleChain       , TRUE,\
		MUIA_ShortHelp	      , GetLocString(HelpTextID), \
		End

#if defined(MUII_StringBack)
	#define	StringBack	MUIA_Background, MUII_StringBack
#else /* MUII_StringBack */
	#define StringBack	MUIA_Background, MUII_TextBack
#endif /* MUII_StringBack */

//----------------------------------------------------------------------------

static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT OpenAboutHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT SelectDeviceHookFunc(struct Hook *hook, Object *o, Msg msg);
static STRPTR GetLocString(ULONG MsgId);
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev);
static void FillDeviceList(void);
static void BToCString(BSTR BName, STRPTR CName, size_t Length);
static BOOL IsFssmValid(struct FileSysStartupMsg *fssm);
static void SelectDevice(CONST_STRPTR DevName);
static BOOL IsDiskFormatted(CONST_STRPTR DevName);
static void StripTrailingChar(STRPTR String, char CharToRemove);
static ULONG GetDosType(CONST_STRPTR DevName, STRPTR ExecDevName, size_t MaxLength, ULONG *UnitNumber);
static CONST_STRPTR GetFileSystemName(ULONG DosType, STRPTR Buffer, size_t MaxLength);
static STRPTR GetDosTypePrintableString(UBYTE dosType, STRPTR Buffer, size_t MaxLength);
static void GetCapacityString(STRPTR Buffer, size_t MaxLength, const struct InfoData *id);

//----------------------------------------------------------------------------

#if !defined(__amigaos4__) && !defined(__AROS__)
#include <dos.h>

long _stack = 16384;		// minimum stack size, used by SAS/C startup code
#endif

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase = NULL;
T_LOCALEBASE LocaleBase = NULL;
#ifndef __amigaos4__
T_UTILITYBASE UtilityBase = NULL;
#endif
struct Library *MUIMasterBase = NULL;
struct Library *IconBase = NULL;
struct Library *IconobjectBase = NULL;

#ifdef __amigaos4__
struct IntuitionIFace *IIntuition = NULL;
struct LocaleIFace *ILocale = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct IconIFace *IIcon = NULL;
struct IconobjectIFace *IIconobject = NULL;
#endif

static struct Catalog *gb_Catalog;

static struct Hook AboutHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutHookFunc), NULL };
static struct Hook AboutMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIHookFunc), NULL };
static struct Hook SelectDeviceHook = { { NULL, NULL }, HOOKFUNC_DEF(SelectDeviceHookFunc), NULL };

//----------------------------------------------------------------------------

static const struct FileSystemName FsList[] =
{
	/* BSD disklabel in MSDOS partitions */
	{FS_BSD_DISKLABEL_MSDOS_SWAP   , "BSD swap"},        /* "BSD\1" */
	{FS_BSD_DISKLABEL_MSDOS_V6     , "BSD version 6"},   /* "BSD\2" */
	{FS_BSD_DISKLABEL_MSDOS_V7     , "BSD version 7"},   /* "BSD\3" */
	{FS_BSD_DISKLABEL_MSDOS_SYSV   , "BSD System V"},    /* "BSD\4" */
	{FS_BSD_DISKLABEL_MSDOS_41     , "4.1 BSD"},         /* "BSD\5" */
	{FS_BSD_DISKLABEL_MSDOS_8TH    , "8th edition BSD"}, /* "BSD\6" */
	{FS_BSD_DISKLABEL_MSDOS_42     , "4.2 BSD"},         /* "BSD\7" */
	{FS_BSD_DISKLABEL_MSDOS_MSDOS  , "BSD MSDOS"},       /* "BSD\8" */
	{FS_BSD_DISKLABEL_MSDOS_44LFS  , "4.4 BSD LFS"},     /* "BSD\9" */
	{FS_BSD_DISKLABEL_MSDOS_UNKNOWN, "BSD unknown"},     /* "BSD\a" */
	{FS_BSD_DISKLABEL_MSDOS_HPFS   , "BSD OS/2 HPFS"},   /* "BSD\b" */
	{FS_BSD_DISKLABEL_MSDOS_ISO9660, "BSD ISO9660"},     /* "BSD\c" */
	{FS_BSD_DISKLABEL_MSDOS_BOOT   , "BSD boot"},        /* "BSD\d" */
	{FS_BSD_DISKLABEL_MSDOS_AFFS   , "BSD Amiga AFFS"},  /* "BSD\e" */
	{FS_BSD_DISKLABEL_MSDOS_HFS    , "BSD Apple HFS"},   /* "BSD\f" */

	/* OSF (alpha) */
	{FS_OFS_LINUX_SWAP  , "OSF BSD/Linux swap"},  /* "OFS\1" */
	{FS_OFS_BSD_V6      , "OSF BSD version 6"},   /* "OFS\2" */
	{FS_OFS_BSD_V7      , "OSF BSD version 7"},   /* "OFS\3" */
	{FS_OFS_BSD_SYSV    , "OSF BSD System V"},    /* "OFS\4" */
	{FS_OFS_BSD_41      , "OSF 4.1 BSD"},         /* "OFS\5" */
	{FS_OFS_BSD_8TH     , "OSF 8th edition BSD"}, /* "OFS\6" */
	{FS_OFS_BSD_42      , "OSF 4.2 BSD"},         /* "OFS\7" */
	{FS_OFS_LINUX_NATIVE, "OSF Linux native"},    /* "OFS\8" */
	{FS_OFS_BSD_44LFS   , "OSF 4.4 BSD LFS"},     /* "OFS\9" */
	{FS_OFS_UNKNOWN     , "OSF unknown"},         /* "OFS\a" */
	{FS_OFS_HPFS        , "OSF OS/2 HPFS"},       /* "OFS\b" */
	{FS_OFS_ISO9660     , "OSF ISO9660"},         /* "OFS\c" */
	{FS_OFS_BOOT        , "OSF boot"},            /* "OFS\d" */
	{FS_OFS_AFFS        , "OSF Amiga AFFS"},      /* "OFS\e" */
	{FS_OFS_HFS         , "OSF Apple HFS"},       /* "OFS\f" */
	{FS_OFS_ADVFS       , "OSF Digital AdvFS"},   /* "OFS\1\0" */

	/* SUN */
	{FS_SUN_EMPTY       , "Sun empty"},        /* "SUN\0" */
	{FS_SUN_BOOT        , "Sun boot"},         /* "SUN\1" */
	{FS_SUN_SUNOS_ROOT  , "SunOS root"},       /* "SUN\2" */
	{FS_SUN_SUNOS_SWAP  , "SunOS swap"},       /* "SUN\3" */
	{FS_SUN_SUNOS_USR   , "SunOS usr"},        /* "SUN\4" */
	{FS_SUN_WHOLE       , "Sun whole disk"},   /* "SUN\5" */
	{FS_SUN_SUNOS_STAND , "SunOS stand"},      /* "SUN\6" */
	{FS_SUN_SUNOS_VAR   , "SunOS var"},        /* "SUN\7" */
	{FS_SUN_SUNOS_HOME  , "SunOS home"},       /* "SUN\8" */
	{FS_SUN_LINUX_MINIX , "Sun Linux minix"},  /* "SUN\8\1" */
	{FS_SUN_LINUX_SWAP  , "Sun Linux swap"},   /* "SUN\8\2" */
	{FS_SUN_LINUX_NATIVE, "Sun Linux native"}, /* "SUN\8\3" */

	/* Amiga */
	{FS_AMIGA_GENERIC_BOOT  , "Amiga generic boot"},         /* "BOOU" */
	{FS_AMIGA_OFS           , "OFS"},                        /* "DOS\0" */
	{FS_AMIGA_FFS           , "FFS"},                        /* "DOS\1" */
	{FS_AMIGA_OFS_INTL      , "OFS Intl."},                  /* "DOS\2" */
	{FS_AMIGA_FFS_INTL      , "FFS Intl."},                  /* "DOS\3" */
	{FS_AMIGA_OFS_DC_INTL   , "OFS DC Intl."},               /* "DOS\4" */
	{FS_AMIGA_FFS_DC_INTL   , "FFS DC Intl."},               /* "DOS\5" */
	{FS_AMIGA_OFS_LNFS      , "OFS LNFS"},                   /* "DOS\6" */
	{FS_AMIGA_FFS_LNFS      , "FFS LNFS"},                   /* "DOS\7" */
	{FS_AMIGA_MUFS_FFS_INTL , "muFS FFS Intl."},             /* "muFS" */
	{FS_AMIGA_MUFS_OFS      , "muFS OFS"},                   /* "muF\0" */
	{FS_AMIGA_MUFS_FFS      , "muFS FFS"},                   /* "muF\1" */
	{FS_AMIGA_MUFS_OFS_INTL , "muFS OFS Intl"},              /* "muF\2" */
	{FS_AMIGA_MUFS_FFS_INTL2, "muFS FFS Intl."},             /* "muF\3", same as muFS */
	{FS_AMIGA_MUFS_OFS_DC   , "muFS OFS DC"},                /* "muF\4" */
	{FS_AMIGA_MUFS_FFS_DC   , "muFS FFS DC"},                /* "muF\5" */
	{FS_AMIGA_LINUX_NATIVE  , "Amiga Linux native"},         /* "LNX\0" */
	{FS_AMIGA_LINUX_EXT2    , "Amiga Linux ext2"},           /* "EXT2" */
	{FS_AMIGA_LINUX_SWAP    , "Amiga Linux swap"},           /* "SWAP" */
	{FS_AMIGA_LINUX_SWAP2   , "Amiga Linux swap"},           /* "SWP\0", same as SWAP */
	{FS_AMIGA_LINUX_MINIX   , "Amiga Linux minix"},          /* "MNX\0" */
	{FS_AMIGA_AMIX_0        , "Amix 0"},                     /* "UNI\0" */
	{FS_AMIGA_AMIX_1        , "Amix 1"},                     /* "UNI\1" */
	{FS_AMIGA_NETBSD_ROOT   , "Amiga NetBSD root"},          /* "NBR\7" */
	{FS_AMIGA_NETBSD_SWAP   , "Amiga NetBSD swap"},          /* "NBS\1" */
	{FS_AMIGA_NETBSD_OTHER  , "Amiga NetBSD other"},         /* "NBU\7" */
	{FS_AMIGA_PFS0          , "PFS 0"},                      /* "PFS\0" */ /* XXX: not sure about the PFS ones.. ask.. */
	{FS_AMIGA_PFS1          , "PFS 1"},                      /* "PFS\1" */ /* XXX: not sure about the PFS ones.. ask.. */
	{FS_AMIGA_PFS2          , "PFS 2"},                      /* "PFS\2" */
	{FS_AMIGA_PDS2          , "PFS 2 SCSIdirect"},           /* "PDS\0" */
	{FS_AMIGA_PFS3          , "PFS 3"},                      /* "PFS\3" */
	{FS_AMIGA_PDS3          , "PFS 3 SCSIdirect"},           /* "PDS\3" */
	{FS_AMIGA_MUPFS         , "PFS Multiuser"},              /* "muPF"  */
	{FS_AMIGA_AFS           , "AFS"},                        /* "AFS\0" */
	{FS_AMIGA_AFS_EXP       , "AFS (experimental)"},         /* "AFS\1" */
	{FS_AMIGA_CDISO         , "CDROM ISO"},                  /* "CD01" can be AmiCDFS, CBM's FS or CDrive in ISO mode */
	{FS_AMIGA_CDHSF         , "CDROM HSF"},                  /* "CD00" HighSierra */
	{FS_AMIGA_CDDA          , "CDROM CDDA"},                 /* "CDDA" audio CD */
	{FS_AMIGA_CDRIVE        , "CDrive or AmiCDFS"},          /* "CDFS" used by AmiCDFS and CDrive, CDrive changes the dostype to ISO, HighSierra or CDDA then */
	{FS_AMIGA_ASIMCDFS      , "AsimCDFS"},                   /* <meaningless> */
	{FS_AMIGA_HFS           , "Macintosh HFS"},              /* "MAC\0" */
	{FS_AMIGA_MSDOS         , "MSDOS disk"},                 /* "MSD\0" */
	{FS_AMIGA_MSDOS_HF      , "MSDOS PC-Task hardfile"},     /* "MSH\0" */
	{FS_AMIGA_BFFS          , "BFFS"},                       /* "BFFS" */
	{FS_AMIGA_SFS           , "SFS"},                        /* "SFS\0 */

	/* Those are special amiga stuff */
	{FS_AMIGA_BAD , "Unreadable disk"},            /* "BAD\0" */
	{FS_AMIGA_NDOS, "Not really dos"},             /* "NDOS" */
	{FS_AMIGA_KICK, "Kickstart disk"},             /* "KICK" */

	/* Atari */
	{FS_ATARI_GEMDOS      , "Atari GEMDOS (<32MB)"}, /* "AGEM" */
	{FS_ATARI_GEMDOSBIG   , "Atari GEMDOS (>32MB)"}, /* "ABGM" */
	{FS_ATARI_LINUX       , "Atari Linux"},          /* LNX */
	{FS_ATARI_LINUX_SWAP  , "Atari Linux swap"},     /* SWP */
	{FS_ATARI_LINUX_MINIX , "Atari Linux minix"},    /* MIX */
	{FS_ATARI_LINUX_MINIX2, "Atari Linux minix"},    /* MNX */
	{FS_ATARI_HFS         , "Atari HFS"},            /* MAC */
	{FS_ATARI_SYSV_UNIX   , "Atari SysV unix"},      /* UNX */
	{FS_ATARI_RAW         , "Atari raw"},            /* RAW */
	{FS_ATARI_EXTENDED    , "Atari extended"},       /* XGM */

	/* Macintosh */
	{FS_MAC_PARTITION_MAP   , "Mac partition map"},      /* "MAC\0" */
	{FS_MAC_MACOS_DRIVER    , "MacOS driver"},           /* "MAC\1" */
	{FS_MAC_MACOS_DRIVER_43 , "MacOS driver 4.3"},       /* "MAC\2" */
	{FS_MAC_MACOS_HFS       , "MacOS HFS"},              /* "MAC\4" */
	{FS_MAC_MACOS_MFS       , "MacOS MFS"},              /* "MAC\5" */
	{FS_MAC_SCRATCH         , "Mac scratch"},            /* "MAC\6" */
	{FS_MAC_PRODOS          , "Mac ProDOS"},             /* "MAC\7" */
	{FS_MAC_FREE            , "Mac free"},               /* "MAC\8" */
	{FS_MAC_LINUX_SWAP      , "Mac Linux swap"},         /* "MAC\9" */
	{FS_MAC_AUX             , "Mac A/UX"},               /* "MAC\a" */
	{FS_MAC_MSDOS           , "Mac MSDOS"},              /* "MAC\b" */
	{FS_MAC_MINIX           , "Mac minix"},              /* "MAC\c" */
	{FS_MAC_AFFS            , "Mac Amiga AFFS"},         /* "MAC\d" */
	{FS_MAC_LINUX_NATIVE    , "Mac Linux native"},       /* "MAC\e" */
	{FS_MAC_NEWWORLD        , "Mac NewWorld bootblock"}, /* "MAC\f" */
	{FS_MAC_MACOS_ATA       , "MacOS ATA driver"},       /* "MAC\1\0" */
	{FS_MAC_MACOS_FW_DRIVER , "MacOS FW driver"},        /* "MAC\1\1" */
	{FS_MAC_MACOS_IOKIT     , "MacOS IOKit"},            /* "MAC\1\2" */
	{FS_MAC_MACOS_PATCHES   , "MacOS patches"},          /* "MAC\1\3" */
	{FS_MAC_MACOSX_BOOT     , "MacOSX bootloader"},      /* "MAC\1\4" */
	{FS_MAC_MACOSX_LOADER   , "MacOSX loader"},          /* "MAC\1\5" */
	{FS_MAC_UFS             , "Mac UFS"},                /* "MAC\1\6" */

	/* Acorn */
	{FS_ACORN_ADFS          , "Acorn ADFS"},                  /* "ARM\1" */
	{FS_ACORN_LINUX_MAP     , "Acorn Linux partitionmap"},    /* "ARM\2" */
	{FS_ACORN_LINUX_EXT2    , "Acorn Linux ext2"},            /* "ARM\3" */
	{FS_ACORN_LINUX_SWAP    , "Acorn Linux swap"},            /* "ARM\4" */
	{FS_ACORN_ADFS_ICS      , "Acorn ADFS (ICS/APDL)"},       /* "ARM\5" */
	{FS_ACORN_LINUX_EXT2_ICS, "Acorn Linux ext2 (ICS/APDL)"}, /* "ARM\6" */
	{FS_ACORN_LINUX_SWAP_ICS, "Acorn Linux swap (ICS/APDL)"}, /* "ARM\7" */

	/* Sinclair QL */
	{FS_SINCLAIR_QL5A, "Sinclair QL 720k"},  /* "QL5A" */
	{FS_SINCLAIR_QL5B, "Sinclair QL 1440k"}, /* "QL5B" */

	/* Spectrum */
	{FS_SPECTRUM_DISCIPLE, "Spectrum Disciple"},   /* "ZXS\0" */
	{FS_SPECTRUM_UNIDOS  , "Spectrum UniDos"},     /* "ZXS\1" */
	{FS_SPECTRUM_SAMDOS  , "Spectrum SamDos"},     /* "ZXS\2" */
	{FS_SPECTRUM_OPUS    , "Spectrum Opus (180k)"}, /* "ZXS\3" */

	/* Archimedes */
	{FS_ARCHIMEDES_D, "Archimedes (D)"},  /* "ARMD" */
	{FS_ARCHIMEDES_E, "Archimedes (E)"}, /* "ARME" */

	/* CP/M */
	{FS_CPM, "CP/M"}, /* "CPM\2" */

	/* C64 */
	{FS_C64, "C64"}, /* "1541" */

	{0, NULL},
};

//----------------------------------------------------------------------------

static Object *APP_Main;
static Object *WIN_Main;
static Object *WIN_AboutMUI;
static Object *NListDosDevices;
static Object *StringLabel;
static Object *TextFormatted;
static Object *TextDosType;
static Object *TextDeviceName;
static Object *CancelButton;
static Object *ButtonQuickFormat, *ButtonFormat, *ButtonFormatVerify;
static Object *MenuAbout, *MenuAboutMUI, *MenuQuit;

//----------------------------------------------------------------------------


int main(int argc, char *argv[])
{
	LONG win_opened;
#if 0
	struct WBStartup *WBenchMsg =
		(argc == 0) ? (struct WBStartup *)argv : NULL;

	if (NULL == WBenchMsg)
		{
		return 5;
		}
	if (WBenchMsg->sm_NumArgs < 2)
		{
		return 5;
		}
#endif

	do	{
		ULONG sigs = 0;
		BOOL Run = TRUE;

		if (!OpenLibraries())
			break;

		if (!CheckMCCVersion(MUIC_NListview, 19, 66) )
			break;

		gb_Catalog = OpenCatalogA(NULL, "Scalos/FormatDiskMUI.catalog",NULL);

		APP_Main = ApplicationObject,
			MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
			MUIA_Application_Version,	"$VER: Scalos FormatDisk.module V40.1 (" __DATE__ ") " COMPILER_STRING,
			MUIA_Application_Copyright,	"© The Scalos Team, 2008" CURRENTYEAR,
			MUIA_Application_Author,	"The Scalos Team",
			MUIA_Application_Description,	"Scalos FormatDisk module",
			MUIA_Application_Base,		"SCALOS_FORMATDISK",

			SubWindow, WIN_Main = WindowObject,
				MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
				MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
				MUIA_Window_AppWindow, TRUE,
				MUIA_Window_UseBottomBorderScroller, TRUE, // +jmc+

				WindowContents, VGroup,
					Child, HGroup,
						Child, NListviewObject,
							MUIA_Weight, 10,
							MUIA_NListview_NList, NListDosDevices = NListObject,
								MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
								MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
								End, //NListObject
							End, //NListviewObject

						Child, VGroup,
							GroupFrame,
							Child, ColGroup(2),
								Child, Label1(GetLocString(MSGID_STRING_LABEL)),
								Child, StringLabel = StringObject,
									StringFrame,
									StringBack,
									End, //StringObject

								Child, Label1(GetLocString(MSGID_TEXT_FILESYSTEM)),
								Child, TextDosType = TextObject,
									MUIA_Background, MUII_TextBack,
									TextFrame,
									End, //TextObject

								Child, Label1(GetLocString(MSGID_TEXT_STATUS)),
								Child, TextFormatted = TextObject,
									TextFrame,
									MUIA_Background, MUII_TextBack,
									End, //TextObject
								End, //ColGroup

							Child, HVSpace,

							Child, HGroup,
								Child, VGroup,
									GroupFrame,
									MUIA_Background, MUII_GroupBack,
									MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_COMMON),

									Child, ColGroup(2),
										Child, Label1(GetLocString(MSGID_CHECK_CREATEICONS)),
										Child, CheckMarkHelp(FALSE, MSGID_CHECK_CREATEICONS_SHORTHELP),
										End, //ColGroup
									End, //VGroup

								Child, HVSpace,

								Child, VGroup,
									GroupFrame,
									MUIA_Background, MUII_GroupBack,
									MUIA_FrameTitle, (ULONG) GetLocString(MSGID_GROUP_SPECIFIC),

									Child, ColGroup(2),
										Child, Label1(GetLocString(MSGID_CHECK_CASE_SENSITIVE)),
										Child, CheckMarkHelp(FALSE, MSGID_CHECK_CASE_SENSITIVE_SHORTHELP),

										Child, Label1(GetLocString(MSGID_CHECK_CREATE_RECYCLE)),
										Child, CheckMarkHelp(FALSE, MSGID_CHECK_CREATE_RECYCLE_SHORTHELP),

										Child, Label1(GetLocString(MSGID_CHECK_RECYCLE_VISIBLE)),
										Child, CheckMarkHelp(FALSE, MSGID_CHECK_RECYCLE_VISIBLE_SHORTHELP),
										End, //ColGroup
									End, //VGroup

								End, //HGroup

							Child, HVSpace,

							Child, TextDeviceName = TextObject,
								TextFrame,
								MUIA_Background, MUII_TextBack,
								MUIA_Text_PreParse, "\33c",
								End, //TextObject
							End, //VGroup
						End, //HGroup

					Child, VSpace(1),

					Child, ColGroup(4),
						Child, ButtonQuickFormat = KeyButtonHelp(GetLocString(MSGID_BUTTON_QUICKFORMAT),
									*GetLocString(MSGID_BUTTON_QUICKFORMAT_SHORT), GetLocString(MSGID_BUTTON_QUICKFORMAT_SHORTHELP)),
						Child, ButtonFormat = KeyButtonHelp(GetLocString(MSGID_BUTTON_FORMAT),
									*GetLocString(MSGID_BUTTON_FORMAT_SHORT), GetLocString(MSGID_BUTTON_FORMAT_SHORTHELP)),
						Child, ButtonFormatVerify = KeyButtonHelp(GetLocString(MSGID_BUTTON_FORMAT_VERIFY),
									*GetLocString(MSGID_BUTTON_FORMAT_VERIFY_SHORT), GetLocString(MSGID_BUTTON_FORMAT_VERIFY_SHORTHELP)),
						Child, CancelButton = KeyButtonHelp(GetLocString(MSGID_CANCELBUTTON),
									*GetLocString(MSGID_CANCELBUTTON_SHORT), GetLocString(MSGID_SHORTHELP_CANCELBUTTON)),
						End, //HGroup
				End, //VGroup
			End, //WindowObject

			MUIA_Application_Menustrip, MenustripObject,
				Child, MenuObjectT(GetLocString(MSGID_MENU_PROJECT)),

					Child, MenuAbout = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_ABOUT),
					End,
					Child, MenuAboutMUI = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_ABOUTMUI),
					End,
					Child, MenuitemObject,
						MUIA_Menuitem_Title, -1,
					End,
					Child, MenuQuit = MenuitemObject,
						MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_QUIT),
						MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_PROJECT_QUIT_SHORT),
					End,

				End, //MenuObjectT
			End, //MenuStripObject

		End; //ApplicationObject

		d1(KPrintF( "%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (NULL == APP_Main)
			{
			printf(GetLocString(MSGID_CREATE_APPLICATION_FAILED));
			break;
			}

		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			WIN_Main, 3, MUIM_Set, MUIA_Window_Open, FALSE);
		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutHook);
		DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutMUIHook);
		DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		// Call hook everytime a device is selected
		DoMethod(NListDosDevices, MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime,
			APP_Main, 3, MUIM_CallHook, &SelectDeviceHook, MUIV_TriggerValue);

		// Quit when "Cancel" button is clicked
		DoMethod(CancelButton, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		set(ButtonQuickFormat, MUIA_Disabled, TRUE);
		set(ButtonFormat, MUIA_Disabled, TRUE);
		set(ButtonFormatVerify, MUIA_Disabled, TRUE);

		FillDeviceList();

		set(WIN_Main, MUIA_Window_Open, TRUE);
		get(WIN_Main, MUIA_Window_Open, &win_opened);

		if (!win_opened)
			{
			printf(GetLocString(MSGID_CREATE_MAINWINDOW_FAILED));
			break;
			}

		d1(KPrintF( "%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		while (Run)
			{
			ULONG Action = DoMethod(APP_Main, MUIM_Application_NewInput, &sigs);

			switch (Action)
				{
			case MUIV_Application_ReturnID_Quit:
				Run = FALSE;
				break;
				}

			if (Run && sigs)
				{
				sigs = Wait(sigs | SIGBREAKF_CTRL_C);

				if (sigs & SIGBREAKF_CTRL_C)
					{
					Run = FALSE;
					}
				}
			}

		set(WIN_Main, MUIA_Window_Open, FALSE);
		} while (0);

	if (APP_Main)
		MUI_DisposeObject(APP_Main);
	if(gb_Catalog)
		CloseCatalog(gb_Catalog);
		
	CloseLibraries();

	return 0;
}

//----------------------------------------------------------------------------

static BOOL OpenLibraries(void)
{
	MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN-1);
	if (NULL == MUIMasterBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
		if (NULL == IMUIMaster)
			return FALSE;
		}
#endif


	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			return FALSE;
		}
#endif

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 38);
#ifdef __amigaos4__
	if (NULL != LocaleBase)
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
#endif

	IconBase = OpenLibrary("icon.library", 39);
	if (NULL == IconBase)
		return FALSE;
#ifdef __amigaos4__
	else
		{
		IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
		if (NULL == IIcon)
			return FALSE;
		}
#endif

#ifndef __amigaos4__
	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;
#endif

	return TRUE;
}

//----------------------------------------------------------------------------

static void CloseLibraries(void)
{
#ifndef __amigaos4__
	if (UtilityBase)
		{
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
#endif
#ifdef __amigaos4__
	if (ILocale)
		{
		DropInterface((struct Interface *)ILocale);
		ILocale = NULL;
		}
#endif
	if (LocaleBase)
		{
		CloseLibrary((struct Library *)LocaleBase);
		LocaleBase = NULL;
		}
#ifdef __amigaos4__
	if (IIcon)
		{
		DropInterface((struct Interface *)IIcon);
		IIcon = NULL;
		}
#endif
	if (IconBase)
		{
		CloseLibrary(IconBase);
		IconBase = NULL;
		}
#ifdef __amigaos4__
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
#endif
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *)IntuitionBase);
		IntuitionBase = NULL;
		}
#ifdef __amigaos4__
	if (IMUIMaster)
		{
		DropInterface((struct Interface *)IMUIMaster);
		IMUIMaster = NULL;
		}
#endif
	if (MUIMasterBase)
		{
		CloseLibrary(MUIMasterBase);
		MUIMasterBase = NULL;
		}
}

//----------------------------------------------------------------------------

static STRPTR GetLocString(ULONG MsgId)
{
	struct FormatDiskMUI_LocaleInfo li;

	li.li_Catalog = gb_Catalog;
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR)GetFormatDiskMUIString(&li, MsgId);
}

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	if (NULL == WIN_AboutMUI)
		{
		WIN_AboutMUI = MUI_NewObject(MUIC_Aboutmui,
			MUIA_Window_RefWindow, WIN_Main,
			MUIA_Aboutmui_Application, APP_Main,
			End;
		}

	if (WIN_AboutMUI)
		set(WIN_AboutMUI, MUIA_Window_Open, TRUE);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT OpenAboutHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	MUI_Request(APP_Main, WIN_Main, 0, NULL,
		GetLocString(MSGID_ABOUTREQOK),
		GetLocString(MSGID_ABOUTREQFORMAT),
		VERSION_MAJOR, VERSION_MINOR, COMPILER_STRING, CURRENTYEAR);
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT SelectDeviceHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	LONG *pPos = (LONG *) msg;

	(void) hook;
	(void) o;

	if (*pPos >= 0)
		{
		STRPTR entry;

		DoMethod(NListDosDevices, MUIM_NList_GetEntry, *pPos, &entry);
		d2(KPrintF(__FILE__ "/%s/%ld: *pPos=%ld  entry=<%s>\n", __FUNC__, __LINE__, *pPos, entry));

		if (entry)
			SelectDevice(entry);
		}

}

//----------------------------------------------------------------------------

IPTR mui_getv(APTR obj, ULONG attr)
{
    IPTR v;
    GetAttr(attr, obj, &v);
    return (v);
}

//----------------------------------------------------------------------------

//  Checks if a certain version of a MCC is available
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev)
{
	BOOL flush = TRUE;

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: %s ", __LINE__, name);)

	while (1)
		{
		ULONG ver;
		ULONG rev;
		struct Library *base;
		char libname[256];

		// First we attempt to acquire the version and revision through MUI
		Object *obj = MUI_NewObject((STRPTR) name, TAG_DONE);
		if (obj)
			{
			get(obj, MUIA_Version, &ver);
			get(obj, MUIA_Revision, &rev);

			MUI_DisposeObject(obj);

			if(ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: v%ld.%ld found through MUIA_Version/Revision\n", __LINE__, ver, rev);)
				return TRUE;
				}
			}

		// If we did't get the version we wanted, let's try to open the
		// libraries ourselves and see what happens...
		stccpy(libname, "PROGDIR:mui", sizeof(libname));
		AddPart(libname, name, sizeof(libname));

		if ((base = OpenLibrary(&libname[8], 0)) || (base = OpenLibrary(&libname[0], 0)))
			{
			UWORD OpenCnt = base->lib_OpenCnt;

			ver = base->lib_Version;
			rev = base->lib_Revision;

			CloseLibrary(base);

			// we add some additional check here so that eventual broken .mcc also have
			// a chance to pass this test (i.e. Toolbar.mcc is broken)
			if (ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: v%ld.%ld found through OpenLibrary()\n", __LINE__, ver, rev);)
				return TRUE;
				}

			if (OpenCnt > 1)
				{
				if (MUI_Request(NULL, NULL, 0L,
					(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
                                        (STRPTR) GetLocString(MSGID_STARTUP_MCC_IN_USE),
					name, minver, minrev, ver, rev))
					{
					flush = TRUE;
					}
				else
					break;
				}

			// Attempt to flush the library if open count is 0 or because the
			// user wants to retry (meaning there's a chance that it's 0 now)

			if (flush)
				{
				struct Library *result;
				extern struct ExecBase *SysBase;

				Forbid();
				if ((result = (struct Library *) FindName(&SysBase->LibList, name)))
					RemLibrary(result);
				Permit();
				flush = FALSE;
				}
			else
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: couldn`t find minimum required version.\n", __LINE__);)

				// We're out of luck - open count is 0, we've tried to flush
				// and still haven't got the version we want
				if (MUI_Request(NULL, NULL, 0L,
					(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
                                        (STRPTR) GetLocString(MSGID_STARTUP_OLD_MCC),
					name, minver, minrev, ver, rev))
					{
					flush = TRUE;
					}
				else
					break;
				}
			}
		else
			{
			// No MCC at all - no need to attempt flush
			flush = FALSE;
			if (!MUI_Request(NULL, NULL, 0L,
				(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
				(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
                                (STRPTR) GetLocString(MSGID_STARTUP_MCC_NOT_FOUND),
				name, minver, minrev))
				{
				break;
				}
			}
		}

	return FALSE;
}

//----------------------------------------------------------------------------

static void FillDeviceList(void)
{
	ULONG DosListFlags = LDF_DEVICES | LDF_READ;
	struct DosList *dl;

	dl = LockDosList(DosListFlags);

	while ((dl = NextDosEntry(dl, DosListFlags)))
		{
		struct FileSysStartupMsg *fssm = BADDR(dl->dol_misc.dol_handler.dol_Startup);

		if (dl->dol_Task && IsFssmValid(fssm))
			{
			char DeviceName[MAX_DEV_NAME];

			BToCString(dl->dol_Name, DeviceName, sizeof(DeviceName) - 1);
			strcat(DeviceName, ":");

			d1(KPrintF( "%s/%s/%ld: dl=%08lx  dol_Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, dl, dl->dol_Name, DeviceName));

			if (IsFileSystem(DeviceName))
				{
				DoMethod(NListDosDevices,
					MUIM_NList_InsertSingle,
					DeviceName,
					MUIV_NList_Insert_Sorted);
				}
			}
		}

	UnLockDosList(DosListFlags);
}

//----------------------------------------------------------------------------

static void BToCString(BSTR BName, STRPTR CName, size_t Length)
{
	const UBYTE *bName = BADDR((const UBYTE *) BName);
	size_t BLength = bName[0];

	bName++;	// skip length

	while (Length > 1 && BLength)
		{
		*CName++ = *bName++;
		Length--;
		BLength--;
		}
	*CName = '\0';
}

//----------------------------------------------------------------------------

/*
 * Checks if a device has a valid fssm.
 */
static BOOL IsFssmValid(struct FileSysStartupMsg *fssm)
{
	const struct DosEnvec *env;

	if (!(TypeOfMem(fssm) & MEMF_PUBLIC))
		return FALSE;

	if (!(TypeOfMem(BADDR(fssm->fssm_Device)) & MEMF_PUBLIC))
		return FALSE;

	env = BADDR(fssm->fssm_Environ);
	if (!(TypeOfMem((APTR) env) & MEMF_PUBLIC))
		return FALSE;

	if (env->de_TableSize > 2 * sizeof(struct DosEnvec))
		return FALSE;
#if 0
					if (!((UBYTE *)myfssm)[0] || !((UBYTE *)devname)[0])
					{
						return (TRUE);
					}
#endif

	return TRUE;
}

//----------------------------------------------------------------------------

static void SelectDevice(CONST_STRPTR DevName)
{
	struct FileInfoBlock *fib = NULL;
	struct InfoData *id = NULL;
	BPTR dLock = 0;
	struct Process *myProc = (struct Process *) FindTask(NULL);
	APTR oldWindowPtr = myProc->pr_WindowPtr;

	myProc->pr_WindowPtr = (APTR) ~0;

	do	{
		char FsName[MAX_DEV_NAME];
		char ExecDevName[108];
		char DevInfoString[128];
		char CapacityString[40];
		ULONG UnitNumber = 0;
		ULONG DosType;

		fib = AllocDosObject(DOS_FIB, NULL);
		d2(KPrintF( "%s/%s/%ld: fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));
		if (NULL == fib)
			break;

		id = malloc(sizeof(struct InfoData));
		d2(KPrintF( "%s/%s/%ld: id=%08lx\n", __FILE__, __FUNC__, __LINE__, id));
		if (NULL == id)
			break;

		dLock = Lock(DevName, ACCESS_READ);
		d2(KPrintF( "%s/%s/%ld: dLock=%08lx\n", __FILE__, __FUNC__, __LINE__, dLock));
		if (0 == dLock)
			break;

		if (!Info(dLock, id))
			break;

		d2(KPrintF( "%s/%s/%ld: Info() Ok, id_DiskType=%08lx\n", __FILE__, __FUNC__, __LINE__, id->id_DiskType));

		if (ID_NO_DISK_PRESENT == id->id_DiskType)
			break;

		if (Examine(dLock, fib))
			{
			d2(KPrintF( "%s/%s/%ld: Examine() OK  <%s>\n", __FILE__, __FUNC__, __LINE__, fib->fib_FileName));
			setstring(StringLabel, fib->fib_FileName);
			}
		else
			{
			d2(KPrintF( "%s/%s/%ld: Examine() failed  <%s>\n", __FILE__, __FUNC__, __LINE__, fib->fib_FileName));
			setstring(StringLabel, GetLocString(MSGID_LABEL_RELABEL_ME));
			}

		set(TextFormatted, MUIA_Text_Contents,
			GetLocString(IsDiskFormatted(DevName) ? MSGID_DISK_FORMATTED : MSGID_DISK_UNFORMATTED));

		DosType = GetDosType(DevName, ExecDevName, sizeof(ExecDevName), &UnitNumber);
		d2(KPrintF( "%s/%s/%ld: DosType=%08lx\n", __FILE__, __FUNC__, __LINE__, DosType));

		set(TextDosType, MUIA_Text_Contents, GetFileSystemName(DosType, FsName, sizeof(FsName)));

		GetCapacityString(CapacityString, sizeof(CapacityString), id);

		snprintf(DevInfoString, sizeof(DevInfoString),
			GetLocString(MSGID_TEXT_DEVICENAME), ExecDevName, UnitNumber, CapacityString);

		set(TextDeviceName, MUIA_Text_Contents, DevInfoString);

		if (ID_WRITE_PROTECTED == id->id_DiskState)
			{
			set(ButtonQuickFormat, MUIA_Disabled, TRUE);
			set(ButtonFormat, MUIA_Disabled, TRUE);
			set(ButtonFormatVerify, MUIA_Disabled, TRUE);
			}
		else
			{
			set(ButtonQuickFormat, MUIA_Disabled, FALSE);
			set(ButtonFormat, MUIA_Disabled, FALSE);
			set(ButtonFormatVerify, MUIA_Disabled, FALSE);
			}
		} while (0);

	if (dLock)
		UnLock(dLock);
	if (id)
		free(id);
	if (fib)
		FreeDosObject(DOS_FIB, fib);

	myProc->pr_WindowPtr = oldWindowPtr;
}

//----------------------------------------------------------------------------

static BOOL IsDiskFormatted(CONST_STRPTR DevName)
{
	char ClonedDeviceName[MAX_DEV_NAME];
	ULONG DosListFlags = LDF_DEVICES | LDF_READ;
	struct DosList *dl;
	struct MsgPort *mp = NULL;
	BOOL IsFormatted = FALSE;

	stccpy(ClonedDeviceName, DevName, sizeof(ClonedDeviceName));
	StripTrailingChar(ClonedDeviceName, ':');

	d2(KPrintF( "%s/%s/%ld: ClonedDeviceName=<%s>\n", __FILE__, __FUNC__, __LINE__, ClonedDeviceName));

	dl = LockDosList(DosListFlags);
	if ((dl = FindDosEntry(dl, ClonedDeviceName, LDF_DEVICES)))
		{
		mp = dl->dol_Task;
		d2(KPrintF( "%s/%s/%ld: dl=%08lx  mp=%08lx  dol_Task=%08lx\n", __FILE__, __FUNC__, __LINE__, dl, mp));
		}
	UnLockDosList(DosListFlags);

	if (mp)
		{
		DosListFlags = LDF_VOLUMES | LDF_READ;

		dl = LockDosList(DosListFlags);
		while ((dl = NextDosEntry(dl, LDF_VOLUMES)))
			{
			d2(KPrintF( "%s/%s/%ld: dl=%08lx  mp=%08lx  dol_Task=%08lx\n", __FILE__, __FUNC__, __LINE__, dl, mp, dl->dol_Task));
			if (mp == dl->dol_Task)
				{
				IsFormatted = TRUE;
				break;
				}
			}
		UnLockDosList(DosListFlags);
		}

	return IsFormatted;
}

//----------------------------------------------------------------------------

static void StripTrailingChar(STRPTR String, char CharToRemove)
{
	size_t Len = strlen(String);

	if (Len > 0 && CharToRemove == String[Len - 1])
		String[Len - 1] = '\0';
}

//----------------------------------------------------------------------------

static ULONG GetDosType(CONST_STRPTR DevName, STRPTR ExecDevName, size_t MaxLength, ULONG *UnitNumber)
{
	char ClonedDeviceName[MAX_DEV_NAME];
	ULONG DosListFlags = LDF_DEVICES | LDF_READ;
	struct DosList *dl;
	ULONG DosType = 0;

	stccpy(ClonedDeviceName, DevName, sizeof(ClonedDeviceName));
	StripTrailingChar(ClonedDeviceName, ':');

	d2(KPrintF( "%s/%s/%ld: ClonedDeviceName=<%s>\n", __FILE__, __FUNC__, __LINE__, ClonedDeviceName));

	dl = LockDosList(DosListFlags);
	if ((dl = FindDosEntry(dl, ClonedDeviceName, LDF_DEVICES)))
		{
		struct FileSysStartupMsg *fssm = BADDR(dl->dol_misc.dol_handler.dol_Startup);

		d2(KPrintF( "%s/%s/%ld: dl=%08lx  fssm=%08lx\n", __FILE__, __FUNC__, __LINE__, dl, fssm));

		if (dl->dol_Task && IsFssmValid(fssm))
			{
			struct DosEnvec *env = BADDR(fssm->fssm_Environ);

			DosType = env->de_DosType;

			BToCString(fssm->fssm_Device, ExecDevName, MaxLength);
			*UnitNumber = fssm->fssm_Unit;
			}
		}
	UnLockDosList(DosListFlags);

	return DosType;
}

//----------------------------------------------------------------------------

static CONST_STRPTR GetFileSystemName(ULONG DosType, STRPTR Buffer, size_t MaxLength)
{
	const struct FileSystemName *fsn;
	CONST_STRPTR FsName = NULL;

	for (fsn = FsList; fsn->fsn_Name; fsn++)
		{
		if (fsn->fsn_DosType == DosType)
			{
			stccpy(Buffer, fsn->fsn_Name, MaxLength);
			FsName = Buffer;
			break;
			}
		}

	if (NULL == FsName)
		{
		char DosListString[4][10];

		snprintf(Buffer, MaxLength,
			GetLocString(MSGID_FSNAME_UNKNOWN),
			GetDosTypePrintableString((UBYTE) (DosType >> 24), DosListString[0], sizeof(DosListString[0])),
			GetDosTypePrintableString((UBYTE) ((DosType >> 16) & 0xff), DosListString[1], sizeof(DosListString[1])),
			GetDosTypePrintableString((UBYTE) ((DosType >> 8) & 0xff), DosListString[2], sizeof(DosListString[2])),
			GetDosTypePrintableString((UBYTE) (DosType & 0xff), DosListString[3], sizeof(DosListString[3]))
			);
		}

	return Buffer;
}

//----------------------------------------------------------------------------

static STRPTR GetDosTypePrintableString(UBYTE dosType, STRPTR Buffer, size_t MaxLength)
{
	if (isprint(dosType))
		snprintf(Buffer, MaxLength, "%c", dosType);
	else
		snprintf(Buffer, MaxLength, "\\%x", (unsigned int) dosType);

	return Buffer;
}

//----------------------------------------------------------------------------

static void GetCapacityString(STRPTR Buffer, size_t MaxLength, const struct InfoData *id)
{
	ULONG BlocksPerKByte = 1024 / id->id_BytesPerBlock;
	ULONG KBytes, MBytes, GBytes;

	if (BlocksPerKByte)
		{
		// Block size <= 1024
		KBytes = id->id_NumBlocks / BlocksPerKByte;
		}
	else
		{
		// Block size > 1024
		ULONG KBytesPerBlock = id->id_BytesPerBlock / 1024;

		KBytes = id->id_NumBlocks * KBytesPerBlock;
		}

	MBytes = KBytes / 1024;
	GBytes = MBytes / 1024;

	if (GBytes > 2)
		{
		if (GBytes > 20)
			snprintf(Buffer, MaxLength, "%lu %s", (unsigned long)GBytes, GetLocString(MSGID_GBYTENAME));
		else
			snprintf(Buffer, MaxLength, "%lu.%lu %s", (unsigned long)GBytes, (unsigned long)((MBytes / 100) % 10), GetLocString(MSGID_GBYTENAME));
		}
	else if (MBytes > 2)
		{
		if (MBytes > 20)
			snprintf(Buffer, MaxLength, "%lu %s", (unsigned long)MBytes, GetLocString(MSGID_MBYTENAME));
		else
			snprintf(Buffer, MaxLength, "%lu.%lu %s", (unsigned long)MBytes, (unsigned long)((KBytes / 100) % 10), GetLocString(MSGID_MBYTENAME));
		}
	else
		snprintf(Buffer, MaxLength, "%lu %s", (unsigned long)KBytes, GetLocString(MSGID_KBYTENAME));
}

//----------------------------------------------------------------------------


