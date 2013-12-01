// RexxGetAttrs.c
// $Date$
// $Revision$


#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/rdargs.h>

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>

#include <graphics/text.h>
#include <workbench/workbench.h>
#include <intuition/intuitionbase.h>

#include <rexx/rxslib.h>
#include <rexx/errors.h>

#include <clib/alib_protos.h>

#include <proto/rexxsyslib.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/scalos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>
#include <ctype.h>


#include "wbrexx.h"
#include "Scalos_Helper.h"

#ifdef __amigaos4__
#define SetRexxVar(msg, var, val, len) SetRexxVarFromMsg((var), (val), (msg))
#endif

static ULONG GetWindowCount(void);
static void SetRexxStemVar(struct RexxMsg *Message, const char *Stem, const char *VarName, const char *Value);
static ULONG GetWindowIconCount(struct ScaWindowStruct *swi, enum IconState Mode);
static struct ScaIconNode *GetWindowIconN(struct ScaWindowStruct *swi, enum IconState Mode, ULONG Index);
static struct UData GetAttr_WindowLeft(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowTop(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowMinWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowMinHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowMaxWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowMaxHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowViewLeft(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowViewTop(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_Get_WindowFromName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_Get_ScreenFromWindow(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowScreenName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowScreenWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowScreenHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsAllCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_Get_AllIconENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsLeft(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsTop(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsType(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsStatus(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsSelectedCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowIconsUnselectedCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_Get_SelectedIconENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_Get_UnselectedIconENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_ApplicationVersion(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_ApplicationScreen(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_ApplicationARexx(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_ApplicationLastError(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_ApplicationIconBorder(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_Get_ScreenFont(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_Get_IconFont(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_Get_SystemFont(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_ApplicationFontName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_ApplicationFontWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_ApplicationFontHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_ApplicationFontSize(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowsCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowsEnum(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_WindowsActive(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_KeyCommandsCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_MenuCommandsCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static ULONG GetCount_KeyCommands(struct UData UserData, struct LeafData *lData);
static ULONG GetCount_MenuCommands(struct UData UserData, struct LeafData *lData);
static struct UData GetAttr_MenuCommandENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_KeyCommandENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_KeyCommandName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_KeyCommandKey(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_KeyCommandCommand(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_MenuCommandName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_MenuCommandTitle(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_MenuCommandShortcut(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static struct UData GetAttr_MenuCommandCommand(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index);
static void PutResult(struct LeafData *lData, const char *Value, const char *StemLeaf);
static ULONG GetCount_Windows(struct UData UserData, struct LeafData *lData);
static ULONG GetCount_AllIcon(struct UData UserData, struct LeafData *lData);
static ULONG GetCount_SelectedIcon(struct UData UserData, struct LeafData *lData);
static ULONG GetCount_UnselectedIcon(struct UData UserData, struct LeafData *lData);
static BOOL IsIconNodeOpen(struct ScaIconNode *sIcon);
#if defined(__GNUC__) && !defined(__MORPHOS__) && ! defined(__amigaos4__) && !defined(__AROS__)
static size_t stccpy(char *dest, const char *src, size_t MaxLen);
static char *strupr(char *q);
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) */


// aus wb39.c
extern WORD ScalosPrefs_IconOffsets[4];
extern struct ScaRootList *rList;

extern struct ScalosBase *ScalosBase;
extern T_UTILITYBASE UtilityBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;


// aus WBRExx.c
extern LONG LastError;			// will be returned by APPLICATION.LASTERROR
extern struct List MenuItemList;	// list of added tool menu items
extern struct List KbdCommandList;	// list of keyboard commands


static const char *ScreenName = "Workbench";
static const char *ARexxPortName = "WORKBENCH";

enum	{ ARG_OBJECT, ARG_NAME, ARG_STEM, ARG_VAR };


#define	ENUM	""
#define	IS_ENUM(str)		(0 == strlen(str))


static const struct LeafCmd GAObjects[] =
	{
/* 0*/	{ "WINDOW", 					&GAObjects[49], &GAObjects[1],  GetAttr_Get_WindowFromName },
/* 1*/		{ "LEFT",				&GAObjects[ 2], NULL,		GetAttr_WindowLeft },
/* 2*/		{ "TOP",				&GAObjects[ 3], NULL,		GetAttr_WindowTop },
/* 3*/		{ "WIDTH",				&GAObjects[ 4], NULL,		GetAttr_WindowWidth },
/* 4*/		{ "HEIGHT",				&GAObjects[ 5], NULL,		GetAttr_WindowHeight },
/* 5*/		{ "MIN",				&GAObjects[ 8], &GAObjects[6],	NULL },
/* 6*/			{ "WIDTH",			&GAObjects[ 7], NULL,		GetAttr_WindowMinWidth },
/* 7*/			{ "HEIGHT",				  NULL, NULL,		GetAttr_WindowMinHeight },
/* 8*/		{ "MAX",				&GAObjects[11], &GAObjects[9],	NULL },
/* 9*/			{ "WIDTH",			&GAObjects[10], NULL,		GetAttr_WindowMaxWidth },
/*10*/			{ "HEIGHT",				  NULL, NULL,		GetAttr_WindowMaxHeight },
/*11*/		{ "VIEW",				&GAObjects[14], &GAObjects[12],	NULL },
/*12*/			{ "LEFT",			&GAObjects[13], NULL,		GetAttr_WindowViewLeft },
/*13*/			{ "TOP",				  NULL, NULL,		GetAttr_WindowViewTop },
/*14*/		{ "SCREEN",				&GAObjects[18], &GAObjects[15],	GetAttr_Get_ScreenFromWindow },
/*15*/			{ "NAME",			&GAObjects[16], NULL,		GetAttr_WindowScreenName },
/*16*/			{ "WIDTH",			&GAObjects[17], NULL,		GetAttr_WindowScreenWidth },
/*17*/			{ "HEIGHT",				  NULL, NULL,		GetAttr_WindowScreenHeight },
/*18*/		{ "ICONS",					  NULL, &GAObjects[19],	NULL },
/*19*/			{ "ALL",			&GAObjects[29], &GAObjects[20],	NULL },
/*20*/				{ "COUNT",		&GAObjects[21], NULL,		GetAttr_WindowIconsAllCount },
/*21*/				{ ENUM,				  NULL, &GAObjects[22], GetAttr_Get_AllIconENUM, GetCount_AllIcon },
/*22*/					{ "NAME",	&GAObjects[23], NULL, 		GetAttr_WindowIconsName },
/*23*/					{ "LEFT",	&GAObjects[24], NULL, 		GetAttr_WindowIconsLeft },
/*24*/					{ "TOP",	&GAObjects[25], NULL, 		GetAttr_WindowIconsTop },
/*25*/					{ "WIDTH",	&GAObjects[26], NULL, 		GetAttr_WindowIconsWidth },
/*26*/					{ "HEIGHT",	&GAObjects[27], NULL, 		GetAttr_WindowIconsHeight },
/*27*/					{ "TYPE",	&GAObjects[28], NULL, 		GetAttr_WindowIconsType },
/*28*/					{ "STATUS",		  NULL, NULL, 		GetAttr_WindowIconsStatus },
/*29*/			{ "SELECTED",			&GAObjects[39], &GAObjects[30],	NULL },
/*30*/				{ "COUNT",		&GAObjects[31], NULL,		GetAttr_WindowIconsSelectedCount },
/*31*/				{ ENUM,				  NULL, &GAObjects[32], GetAttr_Get_SelectedIconENUM, GetCount_SelectedIcon },
/*32*/					{ "NAME",	&GAObjects[33], NULL, 		GetAttr_WindowIconsName },
/*33*/					{ "LEFT",	&GAObjects[34], NULL, 		GetAttr_WindowIconsLeft },
/*34*/					{ "TOP",	&GAObjects[35], NULL, 		GetAttr_WindowIconsTop },
/*35*/					{ "WIDTH",	&GAObjects[36], NULL, 		GetAttr_WindowIconsWidth },
/*36*/					{ "HEIGHT",	&GAObjects[37], NULL, 		GetAttr_WindowIconsHeight },
/*37*/					{ "TYPE",	&GAObjects[38], NULL, 		GetAttr_WindowIconsType },
/*38*/					{ "STATUS",		  NULL, NULL, 		GetAttr_WindowIconsStatus },
/*39*/			{ "UNSELECTED",				  NULL, &GAObjects[40],	NULL },
/*40*/				{ "COUNT",		&GAObjects[41], NULL,		GetAttr_WindowIconsUnselectedCount },
/*41*/				{ ENUM,				  NULL, &GAObjects[42], GetAttr_Get_UnselectedIconENUM, GetCount_UnselectedIcon },
/*42*/					{ "NAME",	&GAObjects[43], NULL, 		GetAttr_WindowIconsName },
/*43*/					{ "LEFT",	&GAObjects[44], NULL, 		GetAttr_WindowIconsLeft },
/*44*/					{ "TOP",	&GAObjects[45], NULL, 		GetAttr_WindowIconsTop },
/*45*/					{ "WIDTH",	&GAObjects[46], NULL, 		GetAttr_WindowIconsWidth },
/*46*/					{ "HEIGHT",	&GAObjects[47], NULL, 		GetAttr_WindowIconsHeight },
/*47*/					{ "TYPE",	&GAObjects[48], NULL, 		GetAttr_WindowIconsType },
/*48*/					{ "STATUS",		  NULL, NULL, 		GetAttr_WindowIconsStatus },
/*49*/	{ "APPLICATION",				&GAObjects[71], &GAObjects[50], NULL },
/*50*/		{ "VERSION",				&GAObjects[51], NULL,		GetAttr_ApplicationVersion },
/*51*/		{ "SCREEN",				&GAObjects[52], NULL,		GetAttr_ApplicationScreen },
/*52*/		{ "AREXX",				&GAObjects[53], NULL,		GetAttr_ApplicationARexx },
/*53*/		{ "LASTERROR",				&GAObjects[54], NULL,		GetAttr_ApplicationLastError },
/*54*/		{ "ICONBORDER",				&GAObjects[55], NULL,		GetAttr_ApplicationIconBorder },
/*55*/		{ "FONT",					  NULL, &GAObjects[56],	NULL },
/*56*/			{ "SCREEN",			&GAObjects[61], &GAObjects[57],	GetAttr_Get_ScreenFont },
/*57*/				{ "NAME",		&GAObjects[58], NULL,		GetAttr_ApplicationFontName },
/*58*/				{ "WIDTH",		&GAObjects[59], NULL,		GetAttr_ApplicationFontWidth },
/*59*/				{ "HEIGHT",		&GAObjects[60], NULL,		GetAttr_ApplicationFontHeight },
/*60*/				{ "SIZE",			  NULL, NULL,		GetAttr_ApplicationFontSize },
/*61*/			{ "ICON",			&GAObjects[66], &GAObjects[62],	GetAttr_Get_IconFont },
/*62*/				{ "NAME",		&GAObjects[63], NULL,		GetAttr_ApplicationFontName },
/*63*/				{ "WIDTH",		&GAObjects[64], NULL,		GetAttr_ApplicationFontWidth },
/*64*/				{ "HEIGHT",		&GAObjects[65], NULL,		GetAttr_ApplicationFontHeight },
/*65*/				{ "SIZE",			  NULL, NULL,		GetAttr_ApplicationFontSize },
/*66*/			{ "SYSTEM",				  NULL, &GAObjects[67],	GetAttr_Get_SystemFont },
/*67*/				{ "NAME",		&GAObjects[68], NULL,		GetAttr_ApplicationFontName },
/*68*/				{ "WIDTH",		&GAObjects[69], NULL,		GetAttr_ApplicationFontWidth },
/*69*/				{ "HEIGHT",		&GAObjects[70], NULL,		GetAttr_ApplicationFontHeight },
/*70*/				{ "SIZE",			  NULL, NULL,		GetAttr_ApplicationFontSize },
/*71*/	{ "WINDOWS",					&GAObjects[75], &GAObjects[72], NULL },
/*72*/		{ "COUNT",				&GAObjects[73], NULL, 		GetAttr_WindowsCount },
/*73*/		{ ENUM,					&GAObjects[74], NULL, 		GetAttr_WindowsEnum, GetCount_Windows },
/*74*/		{ "ACTIVE",					  NULL, NULL, 		GetAttr_WindowsActive },

/*75*/	{ "KEYCOMMANDS",				&GAObjects[81], &GAObjects[76], NULL },
/*76*/		{ "COUNT",				&GAObjects[77], NULL, 		GetAttr_KeyCommandsCount },
/*77*/		{ ENUM,						  NULL, &GAObjects[78], GetAttr_KeyCommandENUM, GetCount_KeyCommands },
/*78*/				{ "NAME",		&GAObjects[79], NULL,		GetAttr_KeyCommandName },
/*79*/				{ "KEY",		&GAObjects[80], NULL,		GetAttr_KeyCommandKey },
/*80*/				{ "COMMAND",			  NULL, NULL,		GetAttr_KeyCommandCommand },

/*81*/	{ "MENUCOMMANDS",					  NULL, &GAObjects[82], NULL },
/*82*/		{ "COUNT",				&GAObjects[83], NULL, 		GetAttr_MenuCommandsCount },
/*83*/		{ ENUM,						  NULL, &GAObjects[84], GetAttr_MenuCommandENUM, GetCount_MenuCommands },
/*84*/				{ "NAME",		&GAObjects[85], NULL,		GetAttr_MenuCommandName },
/*85*/				{ "TITLE",		&GAObjects[86], NULL,		GetAttr_MenuCommandTitle },
/*86*/				{ "SHORTCUT",		&GAObjects[87], NULL,		GetAttr_MenuCommandShortcut },
/*87*/				{ "COMMAND",			  NULL, NULL,		GetAttr_MenuCommandCommand },

	};


static void DoLeafCmd(const struct LeafCmd *lc, struct UData UserData, struct LeafData *lData, const char *ObjName, char *StemLeaf)
{
	BOOL NumFound = FALSE;
	long long1;

	d1(kprintf(__FUNC__ "/%ld: lc=<%s>  ObjName=<%s>  ArgPtr=<%s>  StemLeaf=<%s>\n", __LINE__, \
		lc ? lc->lc_Name : "---",\
		ObjName, lData->lres_ArgPtr, StemLeaf);)

	while (lc && RETURN_OK == lData->lres_Result)
		{
		ULONG Index = ~0;

		if (IS_ENUM(lc->lc_Name) /* && 0 == strlen(lData->lres_ArgPtr) */)
			{
			// "WINDOW.0" or "KEYCOMMANDS.4.NAME"

			if (1 == sscanf(ObjName, "%ld", &long1)) 
				NumFound = TRUE;

			Index = long1; 

			d1(kprintf(__FUNC__ "/%ld: lc=<%s>  Index=%08lx\n", __LINE__, lc->lc_Name, Index);)
			}
		if (NumFound || 0 == Stricmp((STRPTR) ObjName, (STRPTR) lc->lc_Name))
			{
			char StemLeafBuff[100];

			d1(kprintf(__FUNC__ "/%ld: lc=<%s>\n", __LINE__, lc->lc_Name);)

			if (IS_ENUM(lc->lc_Name) && ~0 == Index)
				{
				ULONG n, Count = 0;

				if (lc->lc_GetCount)
					Count = (lc->lc_GetCount)(UserData, lData);

				d1(kprintf(__FUNC__ "/%ld: GetCount=%08lx  Count=%ld\n", __LINE__, lc->lc_GetCount, Count);)

				// enumerate all objects
				for (n=0; n < Count && RETURN_OK == lData->lres_Result; n++)
					{
					char NameBuff[20];

					sprintf(NameBuff, "%ld", (long)n);

					if ('.' == *StemLeaf)
						sprintf(StemLeafBuff, "%s%ld", StemLeaf, (long)n);
					else
						sprintf(StemLeafBuff, "%s.%ld", StemLeaf, (long)n);

					DoLeafCmd(lc, UserData, lData, NameBuff, StemLeafBuff);
					}
				return;
				}
			if (lc->lc_Function)
				{
				d1(kprintf(__FUNC__ "/%ld: lc=<%s>  Index=%08lx\n", __LINE__, lc->lc_Name, Index);)

				UserData = (*lc->lc_Function)(UserData, lData, StemLeaf, Index);
				}

			if (0 == strlen(lData->lres_ArgPtr))
				{
				// end of Object name
				// walk through all sub-objects
				const struct LeafCmd *lcs = lc->lc_Sub;

				d1(kprintf(__FUNC__ "/%ld: Sub=%08lx\n", __LINE__, lcs);)

				if (lData->lres_Flags & LRESF_ORIGINALNAME)
					{
					if ((NULL == lcs && lData->lres_Args[ARG_STEM]) ||
						(lcs && NULL == lData->lres_Args[ARG_STEM]) )
						{
						lData->lres_Result = ERROR_REQUIRED_ARG_MISSING;
						d1(kprintf(__FUNC__ "/%ld: ERROR_REQUIRED_ARG_MISSING\n", __LINE__);)
						}
					}

				lData->lres_Flags &= ~LRESF_ORIGINALNAME;

				while (lcs && RETURN_OK == lData->lres_Result)
					{
					d1(kprintf(__FUNC__ "/%ld: Sub=<%s>\n", __LINE__, lcs->lc_Name);)

					strcpy(StemLeafBuff, StemLeaf);
					strcat(StemLeafBuff, ".");
					strcat(StemLeafBuff, lcs->lc_Name);

					DoLeafCmd(lcs, UserData, lData, lcs->lc_Name, StemLeafBuff);

					lcs = lcs->lc_Next;
					}

				d1(kprintf(__FUNC__ "/%ld: Result=%08lx\n", __LINE__, lData->lres_Result);)
				return;
				}
			else
				{
				char ArgName[100];

				lData->lres_ArgPtr = GetNextArgPart(lData->lres_ArgPtr, ArgName, sizeof(ArgName));
				d1(kprintf(__FUNC__ "/%ld: ArgName=<%s>  ArgPtr=<%s>\n", __LINE__, ArgName, lData->lres_ArgPtr);)

				DoLeafCmd(lc->lc_Sub, UserData, lData, ArgName, "");
				return;
				}
			}

		lc = lc->lc_Next;
		}

	lData->lres_Result = ERROR_REQUIRED_ARG_MISSING;

	d1(kprintf(__FUNC__ "/%ld: ERROR_REQUIRED_ARG_MISSING\n", __LINE__);)
}


#if 0
------------------------------------------------------------------------------
GETATTR command

Purpose:

This command will retrieve information from the Workbench database, such the
names of the drawers currently open and the icons currently selected.

Format:

GETATTR [OBJECT] <Object name> [NAME <Item name>][STEM <Name of stem
variable>] [VAR <Variable name>]

Template:

GETATTR OBJECT/A,NAME/K,STEM/K,VAR/K

Parameters:

OBJECT

Name of the database entry to retrieve. For a list of valid entries see
below.

NAME

For some datatabase entries further information is required to identify the
data to retrieve. This is when you will need to provide a name.

STEM

If you request more than one database entry you will need to provide a
variable to store the information in. For an example of its use, see below.

VAR

If you want the queried information to be stored in a specific variable
(other than the RESULT variable), this is where you provive its name.

Attributes:

You can obtain information on the following attributes:

APPLICATION.VERSION

Version number of workbench.library.

APPLICATION.SCREEN

Name of the public screen Workbench uses.

APPLICATION.AREXX

Name of the Workbench ARexx port.

APPLICATION.LASTERROR

Number of the last error caused by the ARexx interface.

APPLICATION.ICONBORDER

Sizes of the icon borders, returned as four numbers separated by blank
spaces, e.g. "4 3 4 3". The four numbers represent the left border width, the
top border height, the right border width and the bottom border height (in
exactly that order).

APPLICATION.FONT.SCREEN.NAME

Name of the Workbench screen font.

APPLICATION.FONT.SCREEN.WIDTH APPLICATION.FONT.SCREEN.HEIGHT

Size of a single character of the Workbench screen font. Please note that
since the font in question may be proportional spaced the width information
may be of little value. To measure the accurate pixel width of a text in
reference to the font, use the .SIZE attribute.

APPLICATION.FONT.SCREEN.SIZE

Size of a text, measured in pixels, in reference to the screen font. The text
to measure must be provided with the NAME parameter of the GETATTR command.

APPLICATION.FONT.ICON.NAME

Name of the Workbench icon font.

APPLICATION.FONT.ICON.WIDTH APPLICATION.FONT.ICON.HEIGHT

Size of a single character of the Workbench icon font. Please note that since
the font in question may be proportional spaced the width information may be
of little value. To measure the accurate pixel width of a text in reference
to the font, use the .SIZE attribute.

APPLICATION.FONT.ICON.SIZE

Size of a text, measured in pixels, in reference to the icon font. The text
to measure must be provided with the NAME parameter of the GETATTR command.

APPLICATION.FONT.SYSTEM.NAME

Name of the system font.

APPLICATION.FONT.SYSTEM.WIDTH
APPLICATION.FONT.SYSTEM.HEIGHT

Size of a single character of the system font.

APPLICATION.FONT.SYSTEM.SIZE

Size of a text, measured in pixels, in reference to the system font. The text
to measure must be provided with the NAME parameter of the GETATTR command.

WINDOWS.COUNT

Number of the drawer windows currently open. This can be 0.

WINDOWS.0 .. WINDOWS.N

Names of the windows currently open.

WINDOWS.ACTIVE

Name of the currently active Workbench window; this will be "" if none of
Workbench`s windows is currently active.

KEYCOMMANDS.COUNT

Number of keyboard commands assigned. This can be 0.

KEYCOMMANDS.0 .. KEYCOMMANDS.N

Information on all the keyboard commands assigned.

KEYCOMMANDS.<n>.NAME

Name of the keyboard command.

KEYCOMMANDS.<n>.KEY

The key combination assigned to this keyboard command.

KEYCOMMANDS.<n>.COMMAND

The ARexx command assigned to this key combination.

MENUCOMMANDS.COUNT

Number of menu commands assigned (through the "MENU ADD .." command). This can
be 0.

MENUCOMMANDS.0 .. MENUCOMMANDS.N

Information on all the menu commands assigned.

MENUCOMMANDS.<n>.NAME

Name of this menu item.

MENUCOMMANDS.<n>.TITLE

Title of this menu item.

MENUCOMMANDS.<n>.SHORTCUT

The keyboard shortcut assigned to this menu item.

MENUCOMMANDS.<n>.COMMAND

The ARexx command assigned to this menu item.

The following attributes require that the name of the window to obtain
information on is provided.

Example:

WINDOW.LEFT

Left edge of the window.

WINDOW.TOP

Top edge of the window.

WINDOW.WIDTH

Width of the window.

WINDOW.HEIGHT

Height of the window.

WINDOW.MIN.WIDTH

Minimum width of the window.

WINDOW.MIN.HEIGHT

Minimum height of the window.

WINDOW.MAX.WIDTH

Maximum width of the window.

WINDOW.MAX.HEIGHT

Maximum height of the window.

WINDOW.VIEW.LEFT

Horizontal offset of the drawer contents; this value corresponds to the
horizontal window scroller position.

WINDOW.VIEW.TOP

Vertical offset of the drawer contents; this value corresponds to the
vertical window scroller position.

WINDOW.SCREEN.NAME

Name of the public screen the window was opened on.

WINDOW.SCREEN.WIDTH WINDOW.SCREEN.HEIGHT

Size of the public screen the window was opened on.

WINDOW.ICONS.ALL.COUNT

Number of the icons displayed in the window. This can be 0.

WINDOW.ICONS.ALL.0 .. WINDOW.ICONS.ALL.N

Information on all the icons displayed in the window:

WINDOW.ICONS.ALL.<n>.NAME

Name of the icon in question.

WINDOW.ICONS.ALL.<n>.LEFT WINDOW.ICONS.ALL.<n>.TOP

Position of the icon in question.

WINDOW.ICONS.ALL.<n>.WIDTH WINDOW.ICONS.ALL.<n>.HEIGHT

Size of the icon in question.

WINDOW.ICONS.ALL.<n>.TYPE

Type of the icon; one of DISK, DRAWER, TOOL, PROJECT,GARBAGE, DEVICE, KICK or
APPICON.

WINDOW.ICONS.ALL.<n>.STATUS

Whether the icon is selected and (if the icon is a drawer-likeobject, such as
a disk, drawer or trashcan icon) whether thecorresponding drawer is currently
open or closed. This attributeis returned in the form of a string, such as
"SELECTED OPEN" which means that the icon is selected and the corresponding
draweris currently open. The other options include "UNSELECTED" and "CLOSED".

WINDOW.ICONS.SELECTED.COUNT

Number of the selected icons displayed in the window. This can be 0.

WINDOW.ICONS.SELECTED.0 .. WINDOW.ICONS.SELECTED.N

Information on all selected the icons in the window:

WINDOW.ICONS.SELECTED.<n>.NAME

Name of the icon in question.

WINDOW.ICONS.SELECTED.<n>.LEFT WINDOW.ICONS.SELECTED.<n>.TOP

Position of the icon in question.

WINDOW.ICONS.SELECTED.<n>.WIDTH WINDOW.ICONS.SELECTED.<n>.HEIGHT

Size of the icon in question.

WINDOW.ICONS.SELECTED.<n>.TYPE

Type of the icon; one of DISK, DRAWER, TOOL, PROJECT,GARBAGE, DEVICE, KICK or
APPICON.

WINDOW.ICONS.SELECTED.<n>.STATUS

Whether the icon is selected and (if the icon is a drawer-like object, such
as a disk, drawer or trashcan icon) whether the corresponding drawer is
currently open or closed. This attribute is returned in the form of a string,
such as "SELECTED OPEN" which means that the icon is selected and the
corresponding drawer is currently open. The other options include "UNSELECTED"
and "CLOSED". Of course, for the WINDOW.ICONS.SELECTED stem the icon status
will always be reported as "SELECTED".

WINDOW.ICONS.UNSELECTED.COUNT

Number of the unselected icons displayed in the window. This can be 0.

WINDOW.ICONS.UNSELECTED.0 .. WINDOW.ICONS.UNSELECTED.N

Information on all selected the icons in the window:

WINDOW.ICONS.UNSELECTED.<n>.NAME

Name of the icon in question.

WINDOW.ICONS.UNSELECTED.<n>.LEFT WINDOW.ICONS.UNSELECTED.<n>.TOP

Position of the icon in question.

WINDOW.ICONS.UNSELECTED.<n>.WIDTH WINDOW.ICONS.UNSELECTED.<n>.HEIGHT

Size of the icon in question.

WINDOW.ICONS.UNSELECTED.<n>.TYPE

Type of the icon; one of DISK, DRAWER, TOOL, PROJECT, GARBAGE, DEVICE, KICK
or APPICON.

WINDOW.ICONS.UNSELECTED.<n>.STATUS

Whether the icon is selected and (if the icon is a drawer-like object, such
as a disk, drawer or trashcan icon) whether the corresponding drawer is
currently open or closed. This attribute is returned in the form of a string,
such as "UNSELECTED OPEN" which means that the icon is selected and the
corresponding drawer is currently open. The other options include "SELECTED"
and "CLOSED". Of course, for the WINDOW.ICONS.UNSELECTED stem the icon status
will always be reported as "UNSELECTED".

Errors:

10 - If the requester information could not be retrieved, you requested more
than one database entry and did not provide a stem variable or if you
provided a stem variable but did not request more than one database entry.
The error code will be placed in the WORKBENCH.LASTERROR variable.

Result:

RESULT - The information retrieved from the database.

Example:

/* Query the Workbench version. */
ADDRESS workbench
OPTIONS RESULTS

GETATTR application.version
SAY result

/* Query the Workbench version and store it in the * variable
`version_number`. */
GETATTR application.version
VAR version_number
SAY version_number

/* Query the names of all currently open windows, * then print them. */
GETATTR windows
STEM window_list

DO i = 0 TO window_list.count-1
SAY window_list.i;
END;

/* Query name, position and size of the first icon * shown in the root
window. */
GETATTR window.icons.all.0
NAME root
STEM root

SAY root.name
SAY root.left
SAY root.top
SAY root.width
SAY root.height
SAY root.type

/* Query the width and height of the root window. */
GETATTR window.width
NAME root
SAY result

GETATTR window.height
NAME root
SAY result

/* Query the length of a text (in pixels) with reference * to the icon font.
*/
GETATTR application.font.icon.size
NAME ,Text to measure`
SAY result

------------------------------------------------------------------------------
#endif
LONG Cmd_GetAttr(APTR UserData, struct RexxMsg *Message,STRPTR *Args)
{
	char ArgName[100];
	struct LeafData lData;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: obj=%08lx  name=%08lx  stem=%08lx  var=%08lx\n", __LINE__,\
		Args[ARG_OBJECT], Args[ARG_NAME], Args[ARG_STEM], Args[ARG_VAR]);)

	if (NULL == Args[ARG_OBJECT])
		{
		d1(kprintf(__FUNC__ "/%ld: ERROR_REQUIRED_ARG_MISSING\n", __LINE__);)
		return(ERROR_REQUIRED_ARG_MISSING);
		}

	d(kprintf(__FUNC__ "/%ld: obj=<%s>\n", __LINE__, Args[ARG_OBJECT] ? Args[ARG_OBJECT] : (unsigned char *) "");)
	d(kprintf(__FUNC__ "/%ld: name=<%s>\n", __LINE__, Args[ARG_NAME] ? Args[ARG_NAME] : (unsigned char *) "");)
	d(kprintf(__FUNC__ "/%ld: stem=<%s>\n", __LINE__, Args[ARG_STEM] ? Args[ARG_STEM] : (unsigned char *) "");)
	d(kprintf(__FUNC__ "/%ld: var=<%s>\n", __LINE__, Args[ARG_VAR] ? Args[ARG_VAR] : (unsigned char *) "");)

	lData.lres_ArgPtr = GetNextArgPart(Args[ARG_OBJECT], ArgName, sizeof(ArgName));
	d(kprintf(__FUNC__ "/%ld: ArgName=<%s>  ArgPtr=<%s>\n", __LINE__, ArgName, lData.lres_ArgPtr);)

	lData.lres_Message = Message;
	lData.lres_Result = RETURN_OK;
	lData.lres_ResultString = NULL;
	lData.lres_Args = Args;
	lData.lres_Flags = LRESF_ORIGINALNAME;

	DoLeafCmd(&GAObjects[0], uEmpty, &lData, ArgName, "");

	if (RETURN_OK == lData.lres_Result)
		{
		if (Args[ARG_VAR])
			{
			strupr(Args[ARG_VAR]);

			if (lData.lres_ResultString)
				SetRexxVar(Message, Args[ARG_VAR], (STRPTR) lData.lres_ResultString, strlen(lData.lres_ResultString));
			else
				SetRexxVar(Message, Args[ARG_VAR], "", 0);

			ReturnRexxMsg(Message, NULL);
			}
		else
			{
			ReturnRexxMsg(Message, (STRPTR) lData.lres_ResultString);
			}
		}


	return lData.lres_Result;
}


static ULONG GetWindowCount(void)
{
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	ULONG Count = 0;

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d(kprintf(__FUNC__ "/%ld: swi=%08lx Lock=%08lx  Flags=%08lx\n", __LINE__, \
				swi, swi->ws_Lock, swi->ws_Flags));

			Count++;
			}

		SCA_UnLockWindowList();
		}

	return Count;
}


static void SetRexxStemVar(struct RexxMsg *Message, const char *Stem, const char *VarName, const char *Value)
{
	char vName[100];

	strcpy(vName, Stem);
	if (*VarName && '.' != *VarName)
		strcat(vName, ".");

	strcat(vName, VarName);

	SetRexxVar(Message, vName, (STRPTR) Value, strlen(Value));
}


static ULONG GetWindowIconCount(struct ScaWindowStruct *swi, enum IconState Mode)
{
	ULONG Count = 0;
	struct ScaWindowTask *wt = swi->ws_WindowTask;

	d(kprintf(__FUNC__ "/%ld: ScaWindowStruct=%08lx  ScaWindowTask=%08lx\n", __LINE__, swi, wt));

	if (wt)
		{
		struct ScaIconNode *icon;
		char wBuffer[256];

		if (swi->ws_Lock)
			NameFromLock(swi->ws_Lock, wBuffer, sizeof(wBuffer));

		ObtainSemaphore(wt->wt_IconSemaphore);

		for (icon=wt->wt_IconList; icon; icon = (struct ScaIconNode *) icon->in_Node.mln_Succ)
			{
			char *Name = NULL;

			GetAttr(IDTA_Text, icon->in_Icon, (APTR) &Name);

			if (Name)
				{
				switch (Mode)
					{
				case icst_All:
					Count++;
					break;
				case icst_Selected:
					if (isIconSelected(icon))
						Count++;
					break;
				case icst_Unselected:
					if (!isIconSelected(icon))
						Count++;
					break;
					}
				}
			}

		ReleaseSemaphore(wt->wt_IconSemaphore);
		}

	return Count;
}


static struct ScaIconNode *GetWindowIconN(struct ScaWindowStruct *swi, enum IconState Mode, ULONG Index)
{
	ULONG Count = 0;
	struct ScaWindowTask *wt = swi->ws_WindowTask;
	struct ScaIconNode *Result = NULL;

	d1(kprintf(__FUNC__ "/%ld: ScaWindowStruct=%08lx  ScaWindowTask=%08lx  Mode=%ld\n", __LINE__, swi, wt, Mode));

	if (wt)
		{
		struct ScaIconNode *icon;

		ObtainSemaphore(wt->wt_IconSemaphore);

		for (icon=wt->wt_IconList; NULL == Result && icon; icon = (struct ScaIconNode *) icon->in_Node.mln_Succ)
			{
			d1(kprintf(__FUNC__ "/%ld: Index=%ld  Count=%ld  icon=%08lx\n", __LINE__, Index, Count, icon));

			switch (Mode)
				{
			case icst_All:
				if (Count++ == Index)
					Result = icon;
				break;
			case icst_Selected:
				if (isIconSelected(icon))
					{
					if (Count++ == Index)
						Result = icon;
					}
				break;
			case icst_Unselected:
				if (!isIconSelected(icon))
					{
					if (Count++ == Index)
						Result = icon;
					}
				break;
				}
			}

		ReleaseSemaphore(wt->wt_IconSemaphore);
		}

	d1(kprintf(__FUNC__ "/%ld: Result=%08lx\n", __LINE__, Result));

	return Result;
}


static struct UData GetAttr_WindowLeft(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi && swi->ws_Window)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_Window->LeftEdge);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowTop(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi && swi->ws_Window)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_Window->TopEdge);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi && swi->ws_Window)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_Window->Width);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
	return uEmpty;
}


static struct UData GetAttr_WindowHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi && swi->ws_Window)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_Window->Height);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
	return uEmpty;
}


static struct UData GetAttr_WindowMinWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi && swi->ws_Window)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_Window->MinWidth);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
	return uEmpty;
}


static struct UData GetAttr_WindowMinHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi && swi->ws_Window)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_Window->MinHeight);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
	return uEmpty;
}


static struct UData GetAttr_WindowMaxWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi && swi->ws_Window)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_Window->MaxWidth);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowMaxHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi && swi->ws_Window)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_Window->MaxHeight);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowViewLeft(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_xoffset);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowViewTop(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi)
		{
		sprintf(lData->lres_ResultBuffer, "%d", swi->ws_yoffset);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_Get_WindowFromName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (NULL == swi)
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
	else
		{
		uEmpty.uda_Data = swi;
		uEmpty.uda_Type = udaType_ScaWindowStruct;
		}

	return uEmpty;
}


static struct UData GetAttr_Get_ScreenFromWindow(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaWindowStruct != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	if (swi && swi->ws_Window)
		{
		uEmpty.uda_Data = swi->ws_Window->WScreen;
		uEmpty.uda_Type = udaType_Screen;
		}

	return uEmpty;
}


static struct UData GetAttr_WindowScreenName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct Screen *scr = (struct Screen *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_Screen != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	strcpy(lData->lres_ResultBuffer, "");

	if (scr)
		{
		struct PubScreenNode *pubscreen;
		struct List *pubList = LockPubScreenList();

		pubscreen = (struct PubScreenNode *) pubList->lh_Head;

		while (pubscreen->psn_Node.ln_Succ != 0)
			{
			if (pubscreen->psn_Screen == scr)
				{
				sprintf(lData->lres_ResultBuffer, "%s", pubscreen->psn_Node.ln_Name);
				break;
				}

			pubscreen = (struct PubScreenNode *) pubscreen->psn_Node.ln_Succ;
			}

		UnlockPubScreenList();
		}

	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_WindowScreenWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct Screen *scr = (struct Screen *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };
	WORD Width = 0;

	if (udaType_Screen != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (scr)
		{
		Width = scr->Width;
		}

	sprintf(lData->lres_ResultBuffer, "%d", Width);
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_WindowScreenHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct Screen *scr = (struct Screen *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };
	WORD Height = 0;

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_Screen != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (scr)
		{
		Height = scr->Height;
		}

	sprintf(lData->lres_ResultBuffer, "%d", Height);
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_WindowIconsAllCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	sprintf(lData->lres_ResultBuffer, "%ld", (long)GetCount_AllIcon(UserData, lData));
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_Get_AllIconENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct ScaIconNode *sIcon = NULL;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (udaType_ScaWindowStruct != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (swi)
		{
		sIcon = GetWindowIconN(swi, icst_All, Index);
		}

	if (NULL == sIcon)
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
	else
		{
		uEmpty.uda_Data = sIcon;
		uEmpty.uda_Type = udaType_ScaIconNode;
		}

	return uEmpty;
}


static struct UData GetAttr_WindowIconsName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaIconNode *sIcon = (struct ScaIconNode *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaIconNode != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (sIcon)
		{
		char *Name = NULL;

		GetAttr(IDTA_Text, sIcon->in_Icon, (APTR) &Name);

		if (Name)
			{
			stccpy(lData->lres_ResultBuffer, Name, sizeof(lData->lres_ResultBuffer));
			PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
			}
		else
			lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowIconsLeft(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaIconNode *sIcon = (struct ScaIconNode *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaIconNode != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (sIcon)
		{
		struct ExtGadget *gg = (struct ExtGadget *) sIcon->in_Icon;

		if (gg)
			{
			sprintf(lData->lres_ResultBuffer, "%d", gg->LeftEdge);
			PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
			}
		else
			lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowIconsTop(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaIconNode *sIcon = (struct ScaIconNode *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaIconNode != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (sIcon)
		{
		struct ExtGadget *gg = (struct ExtGadget *) sIcon->in_Icon;

		if (gg)
			{
			sprintf(lData->lres_ResultBuffer, "%d", gg->TopEdge);
			PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
			}
		else
			lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowIconsWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaIconNode *sIcon = (struct ScaIconNode *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaIconNode != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (sIcon)
		{
		struct ExtGadget *gg = (struct ExtGadget *) sIcon->in_Icon;

		if (gg)
			{
			sprintf(lData->lres_ResultBuffer, "%d", gg->Width);
			PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
			}
		else
			lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowIconsHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaIconNode *sIcon = (struct ScaIconNode *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaIconNode != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (sIcon)
		{
		struct ExtGadget *gg = (struct ExtGadget *) sIcon->in_Icon;

		if (gg)
			{
			sprintf(lData->lres_ResultBuffer, "%d", gg->Height);
			PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
			}
		else
			lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowIconsType(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaIconNode *sIcon = (struct ScaIconNode *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaIconNode != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (sIcon)
		{
		if (sIcon->in_Icon)
			{
			IPTR Type;

			GetAttr(IDTA_Type, sIcon->in_Icon, &Type);

			switch (Type)
				{
			case WBDISK:
				strcpy(lData->lres_ResultBuffer, "DISK");	
				break;
			case WBDRAWER:
				strcpy(lData->lres_ResultBuffer, "DRAWER");
				break;
			case WBTOOL:
				strcpy(lData->lres_ResultBuffer, "TOOL");
				break;
			case WBPROJECT:
				strcpy(lData->lres_ResultBuffer, "PROJECT");
				break;
			case WBGARBAGE:
				strcpy(lData->lres_ResultBuffer, "GARBAGE");
				break;
			case WBDEVICE:
				strcpy(lData->lres_ResultBuffer, "DEVICE");
				break;
			case WBKICK:
				strcpy(lData->lres_ResultBuffer, "KICK");
				break;
			case WBAPPICON:
				strcpy(lData->lres_ResultBuffer, "APPICON");
				break;
			default:
				strcpy(lData->lres_ResultBuffer, "????");
				break;
				}

			PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
			}
		else
			lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowIconsStatus(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaIconNode *sIcon = (struct ScaIconNode *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaIconNode != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (sIcon)
		{
		struct Gadget *gg = (struct Gadget *) sIcon->in_Icon;

		if (gg)
			{
			sprintf(lData->lres_ResultBuffer, "%s %s", 
				(gg->Flags & GFLG_SELECTED) ? "SELECTED" : "UNSELECTED",
				IsIconNodeOpen(sIcon) ? "OPEN" : "CLOSED");
			PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
			}
		else
			lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowIconsSelectedCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	sprintf(lData->lres_ResultBuffer, "%ld", (long)GetCount_SelectedIcon(UserData, lData));
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_WindowIconsUnselectedCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	sprintf(lData->lres_ResultBuffer, "%ld", (long)GetCount_UnselectedIcon(UserData, lData));
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_Get_SelectedIconENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct ScaIconNode *sIcon = NULL;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d1(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>  Index=%ld\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr, Index);)

	if (swi)
		{
		sIcon = GetWindowIconN(swi, icst_Selected, Index);
		}

	if (NULL == sIcon)
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
	else
		{
		uEmpty.uda_Data = sIcon;
		uEmpty.uda_Type = udaType_ScaIconNode;
		}

	return uEmpty;
}


static struct UData GetAttr_Get_UnselectedIconENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	struct ScaIconNode *sIcon = NULL;
	struct UData uEmpty = { NULL, udaType_Nothing };

	if (NULL == swi)
		swi = FindWindowByName(lData->lres_Args[ARG_NAME]);
	else
		{
		if (udaType_ScaWindowStruct != UserData.uda_Type)
			{
			lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
			return uEmpty;
			}
		}

	d1(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>  Index=%ld\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr, Index);)

	if (swi)
		{
		sIcon = GetWindowIconN(swi, icst_Unselected, Index);
		}

	if (NULL == sIcon)
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
	else
		{
		uEmpty.uda_Data = sIcon;
		uEmpty.uda_Type = udaType_ScaIconNode;
		}

	return uEmpty;
}


static struct UData GetAttr_ApplicationVersion(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	sprintf(lData->lres_ResultBuffer, "%d.%d", ScalosBase->scb_LibNode.lib_Version, ScalosBase->scb_LibNode.lib_Revision);
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_ApplicationScreen(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)
	d(kprintf(__FUNC__ "/%ld: rList=%08lx\n", __LINE__, rList);)

	PutResult(lData, ScreenName, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_ApplicationARexx(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	PutResult(lData, ARexxPortName, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_ApplicationLastError(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)
	d(kprintf(__FUNC__ "/%ld: LastError=%ld\n", __LINE__, LastError);)

	sprintf(lData->lres_ResultBuffer, "%ld", (long)LastError);
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_ApplicationIconBorder(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	sprintf(lData->lres_ResultBuffer, "%d %d %d %d",
		ScalosPrefs_IconOffsets[0],
		ScalosPrefs_IconOffsets[1],
		ScalosPrefs_IconOffsets[2],
		ScalosPrefs_IconOffsets[3]);
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_Get_ScreenFont(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct TextFont *font;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	font = rList->rl_internInfos->ii_DrawInfo->dri_Font;

	uEmpty.uda_Data = font;
	uEmpty.uda_Type = udaType_TextFont;

	return uEmpty;
}


static struct UData GetAttr_Get_IconFont(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct TextFont *font = NULL;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (rList)
		{
		GetAttr(SCCA_IconWin_IconFont, rList->rl_internInfos->ii_MainWindowStruct->ws_WindowTask->mt_MainObject, (APTR) &font);
		}

	uEmpty.uda_Data = font;
	uEmpty.uda_Type = udaType_TextFont;

	return uEmpty;
}


static struct UData GetAttr_Get_SystemFont(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct TextFont *font = NULL;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (rList->rl_internInfos->ii_MainWindowStruct->ws_Window)
		font = rList->rl_internInfos->ii_MainWindowStruct->ws_Window->IFont;

	uEmpty.uda_Data = font;
	uEmpty.uda_Type = udaType_TextFont;

	return uEmpty;
}


static struct UData GetAttr_ApplicationFontName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct TextFont *font = (struct TextFont *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_TextFont != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (font)
		{
		sprintf(lData->lres_ResultBuffer, "%s", font->tf_Message.mn_Node.ln_Name);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}

	return uEmpty;
}


static struct UData GetAttr_ApplicationFontWidth(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct TextFont *font = (struct TextFont *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_TextFont != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (font)
		{
		sprintf(lData->lres_ResultBuffer, "%d", font->tf_XSize);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}

	return uEmpty;
}


static struct UData GetAttr_ApplicationFontHeight(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct TextFont *font = (struct TextFont *) UserData.uda_Data;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_TextFont != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (font)
		{
		sprintf(lData->lres_ResultBuffer, "%d", font->tf_YSize);
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}

	return uEmpty;
}


static struct UData GetAttr_ApplicationFontSize(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct TextFont *font = (struct TextFont *) UserData.uda_Data;
	STRPTR Text = lData->lres_Args[ARG_NAME];
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (NULL == Text)
		Text = "";

	if (udaType_TextFont != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}
	if (font)
		{
		struct RastPort rp;

		InitRastPort(&rp);
		SetFont(&rp, font);

		sprintf(lData->lres_ResultBuffer, "%d", TextLength(&rp, Text, strlen(Text)));
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}

	return uEmpty;
}


static struct UData GetAttr_WindowsCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	sprintf(lData->lres_ResultBuffer, "%ld", (long)GetCount_Windows(UserData, lData));
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_WindowsEnum(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	ULONG Count = 0;
	BOOL Found = FALSE;

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>  Index=%ld\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr, Index);)

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; !Found && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d(kprintf(__FUNC__ "/%ld: swi=%08lx Lock=%08lx  Flags=%08lx\n", __LINE__, \
				swi, swi->ws_Lock, swi->ws_Flags));

			if (swi->ws_Lock)
				{
				if (NameFromLock(swi->ws_Lock, lData->lres_ResultBuffer, sizeof(lData->lres_ResultBuffer)))
					{
					}
				else
					{
					strcpy(lData->lres_ResultBuffer, "???");
					}
				}
			else
				{
				strcpy(lData->lres_ResultBuffer, "root");
				}

			if (Count == Index)
				{
				Found = TRUE;
				PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
				break;
				}

			Count++;
			}

		SCA_UnLockWindowList();
		}

	if (!Found)
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_WindowsActive(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	BOOL Found = FALSE;

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>  Index=%ld\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr, Index);)

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; !Found && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d(kprintf(__FUNC__ "/%ld: swi=%08lx Lock=%08lx  Flags=%08lx\n", __LINE__, \
				swi, swi->ws_Lock, swi->ws_Flags));

			if (swi->ws_Lock)
				{
				if (NameFromLock(swi->ws_Lock, lData->lres_ResultBuffer, sizeof(lData->lres_ResultBuffer)))
					{
					}
				else
					{
					strcpy(lData->lres_ResultBuffer, "???");
					}
				}
			else
				{
				strcpy(lData->lres_ResultBuffer, "root");
				}

			if (swi->ws_Window == IntuitionBase->ActiveWindow)
				{
				Found = TRUE;
				PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
				break;
				}
			}

		SCA_UnLockWindowList();
		}

	if (!Found)
		PutResult(lData, "", StemLeaf);

	return uEmpty;
}


static void PutResult(struct LeafData *lData, const char *Value, const char *StemLeaf)
{
	d(kprintf(__FUNC__ "/%ld: Value=<%s>\n", __LINE__, Value);)

	if (lData->lres_Args[ARG_STEM])
		{
		d(kprintf(__FUNC__ "/%ld: Value=<%s>  StemLeaf=<%s>\n", __LINE__, Value, StemLeaf);)

		SetRexxStemVar(lData->lres_Message, lData->lres_Args[ARG_STEM], StemLeaf, Value);
		}
	else
		{
		lData->lres_ResultString = Value;
		}
}


static ULONG GetCount_Windows(struct UData UserData, struct LeafData *lData)
{
	return GetWindowCount();
}


static ULONG GetCount_AllIcon(struct UData UserData, struct LeafData *lData)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	ULONG Count = 0;

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaWindowStruct != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return 0;
		}
	if (swi)
		Count = GetWindowIconCount(swi, icst_All);

	return Count;
}


static ULONG GetCount_SelectedIcon(struct UData UserData, struct LeafData *lData)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	ULONG Count = 0;

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaWindowStruct != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return 0;
		}
	if (swi)
		Count = GetWindowIconCount(swi, icst_Selected);

	return Count;
}


static ULONG GetCount_UnselectedIcon(struct UData UserData, struct LeafData *lData)
{
	struct ScaWindowStruct *swi = (struct ScaWindowStruct *) UserData.uda_Data;
	ULONG Count = 0;

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_ScaWindowStruct != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return 0;
		}
	if (swi)
		Count = GetWindowIconCount(swi, icst_Unselected);

	return Count;
}


static struct UData GetAttr_KeyCommandsCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	sprintf(lData->lres_ResultBuffer, "%ld", (long)GetCount_KeyCommands(UserData, lData));
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static struct UData GetAttr_MenuCommandsCount(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	sprintf(lData->lres_ResultBuffer, "%ld", (long)GetCount_MenuCommands(UserData, lData));
	PutResult(lData, lData->lres_ResultBuffer, StemLeaf);

	return uEmpty;
}


static ULONG GetCount_KeyCommands(struct UData UserData, struct LeafData *lData)
{
	ULONG Count = 0;
	const struct KeyboardCommand *kbc;

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	for (kbc = (struct KeyboardCommand *) KbdCommandList.lh_Head;
		kbc != (struct KeyboardCommand *) &KbdCommandList.lh_Tail;
		kbc = (struct KeyboardCommand *) kbc->kbc_Node.ln_Succ)
		{
		Count++;
		}

	return Count;
}


static ULONG GetCount_MenuCommands(struct UData UserData, struct LeafData *lData)
{
	ULONG Count = 0;
	const struct ToolMenuItem *tmi;

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	for (tmi = (struct ToolMenuItem *) MenuItemList.lh_Head;
		tmi != (struct ToolMenuItem *) &MenuItemList.lh_Tail;
		tmi = (struct ToolMenuItem *) tmi->tmi_Node.ln_Succ)
		{
		Count++;
		}

	d1(kprintf(__FUNC__ "/%ld: Count=%lu\n", __LINE__, Count);)

	return Count;
}


static struct UData GetAttr_MenuCommandENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ToolMenuItem *tmi, *tmiFound;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	tmiFound = NULL;

	for (tmi = (struct ToolMenuItem *) MenuItemList.lh_Head;
		tmi != (struct ToolMenuItem *) &MenuItemList.lh_Tail;
		tmi = (struct ToolMenuItem *) tmi->tmi_Node.ln_Succ)
		{
		if (Index-- == 0)
			{
			tmiFound = tmi;
			break;
			}
		}

	if (tmiFound)
		{
		uEmpty.uda_Data = tmiFound;
		uEmpty.uda_Type = udaType_MenuCommand;
		}
	else
		{
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
		}

	return uEmpty;
}


static struct UData GetAttr_KeyCommandENUM(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct KeyboardCommand *kbc, *kbcFound;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	kbcFound = NULL;

	for (kbc = (struct KeyboardCommand *) KbdCommandList.lh_Head;
		kbc != (struct KeyboardCommand *) &KbdCommandList.lh_Tail;
		kbc = (struct KeyboardCommand *) kbc->kbc_Node.ln_Succ)
		{
		if (Index-- == 0)
			{
			kbcFound = kbc;
			break;
			}
		}

	if (kbcFound)
		{
		d1(kprintf(__FUNC__ "/%ld: kbc=%08lx\n", __LINE__, kbcFound);)

		uEmpty.uda_Data = kbcFound;
		uEmpty.uda_Type = udaType_KeyCommand;
		}
	else
		{
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;
		}

	return uEmpty;
}


static struct UData GetAttr_KeyCommandName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct KeyboardCommand *kbc;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d1(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_KeyCommand != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	kbc = (struct KeyboardCommand *) UserData.uda_Data;

	d1(kprintf(__FUNC__ "/%ld: kbc=%08lx\n", __LINE__, kbc);)

	if (kbc)
		{
		stccpy(lData->lres_ResultBuffer, kbc->kbc_Name, sizeof(lData->lres_ResultBuffer));
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_KeyCommandKey(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct KeyboardCommand *kbc;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d1(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_KeyCommand != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	kbc = (struct KeyboardCommand *) UserData.uda_Data;

	d1(kprintf(__FUNC__ "/%ld: kbc=%08lx\n", __LINE__, kbc);)

	if (kbc)
		{
		stccpy(lData->lres_ResultBuffer, kbc->kbc_Key, sizeof(lData->lres_ResultBuffer));
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_KeyCommandCommand(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct KeyboardCommand *kbc;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d1(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_KeyCommand != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	kbc = (struct KeyboardCommand *) UserData.uda_Data;

	d1(kprintf(__FUNC__ "/%ld: kbc=%08lx\n", __LINE__, kbc);)

	if (kbc)
		{
		stccpy(lData->lres_ResultBuffer, kbc->kbc_Cmd, sizeof(lData->lres_ResultBuffer));
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_MenuCommandName(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ToolMenuItem *tmi;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_MenuCommand != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	tmi = (struct ToolMenuItem *) UserData.uda_Data;

	if (tmi)
		{
		stccpy(lData->lres_ResultBuffer, tmi->tmi_Name, sizeof(lData->lres_ResultBuffer));
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_MenuCommandTitle(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ToolMenuItem *tmi;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_MenuCommand != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	tmi = (struct ToolMenuItem *) UserData.uda_Data;

	if (tmi)
		{
		stccpy(lData->lres_ResultBuffer, tmi->tmi_Title, sizeof(lData->lres_ResultBuffer));
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_MenuCommandShortcut(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ToolMenuItem *tmi;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr);)

	if (udaType_MenuCommand != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	tmi = (struct ToolMenuItem *) UserData.uda_Data;

	if (tmi)
		{
		stccpy(lData->lres_ResultBuffer, tmi->tmi_Shortcut, sizeof(lData->lres_ResultBuffer));
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


static struct UData GetAttr_MenuCommandCommand(struct UData UserData, struct LeafData *lData, const char *StemLeaf, ULONG Index)
{
	struct ToolMenuItem *tmi;
	struct UData uEmpty = { NULL, udaType_Nothing };

	d(kprintf(__FUNC__ "/%ld: UserData=%08lx  ArgPtr=<%s>\n", __LINE__, UserData.uda_Type, lData->lres_ArgPtr));

	if (udaType_MenuCommand != UserData.uda_Type)
		{
		lData->lres_Result = ERROR_OBJECT_WRONG_TYPE;
		return uEmpty;
		}

	tmi = (struct ToolMenuItem *) UserData.uda_Data;

	if (tmi)
		{
		stccpy(lData->lres_ResultBuffer, tmi->tmi_Cmd, sizeof(lData->lres_ResultBuffer));
		PutResult(lData, lData->lres_ResultBuffer, StemLeaf);
		}
	else
		lData->lres_Result = ERROR_OBJECT_NOT_FOUND;

	return uEmpty;
}


// check if <sIcon> is open somewhere as an icon window
static BOOL IsIconNodeOpen(struct ScaIconNode *sIcon)
{
	struct ScaWindowStruct *swi;
	BOOL Result = FALSE;

	if ((BPTR)NULL == sIcon->in_Lock)
		return FALSE;

	swi = GetIconNodeOpenWindow(sIcon->in_Lock, sIcon);

	if (swi)
		{
		SCA_UnLockWindowList();
		Result = TRUE;
		}

	return Result;
}


#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
// Replacement for SAS/C library functions

static size_t stccpy(char *dest, const char *src, size_t MaxLen)
{
	size_t Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}

static char *strupr(char *q)
{
   while (q && *q)
	{
	char c = toupper(*q);
	*(q++) = c;
	}

   return q;
}


#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) */
