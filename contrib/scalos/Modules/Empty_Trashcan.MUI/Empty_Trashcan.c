// Empty_Trashcan.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include <dos/exall.h>
#include <dos/dostags.h>
#include <dos/dosextens.h>
#include <intuition/imageclass.h>
#include <workbench/startup.h>
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <prefs/prefhdr.h>
#include <scalos/scalos.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iffparse.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include "Empty_Trashcan.h"

#define	Empty_Trashcan_NUMBERS
#define	Empty_Trashcan_ARRAY
#define	Empty_Trashcan_CODE
#include STR(SCALOSLOCALE)

//----------------------------------------------------------------------------

#define	VERSION_MAJOR	40
#define	VERSION_MINOR	1

//----------------------------------------------------------------------------

#define	ID_MUIO		MAKE_ID('M','U','I','O')
#define ID_CMTC		MAKE_ID('C','M','T','C')
#define	ID_STTC         MAKE_ID('S','T','T','C')

//----------------------------------------------------------------------------

#define	Application_Return_Ok		1001

//----------------------------------------------------------------------------

#define KeyButtonHelp(name,key,HelpText)\
	TextObject,\
		ButtonFrame,\
		MUIA_Font, MUIV_Font_Button,\
		MUIA_Text_Contents, name,\
		MUIA_Text_PreParse, "\33c",\
		MUIA_Text_HiChar  , key,\
		MUIA_ControlChar  , key,\
		MUIA_InputMode    , MUIV_InputMode_RelVerify,\
		MUIA_Background   , MUII_ButtonBack,\
		MUIA_ShortHelp    , HelpText,\
		MUIA_CycleChain   , TRUE,\
		End

#define CheckMarkHelp(InputMode, Selected, Image,HelpTextID, NumID)\
	ImageObject,\
		ImageButtonFrame,\
		MUIA_InputMode      , InputMode,\
		MUIA_Image_Spec     , Image,\
		MUIA_Image_FreeVert , TRUE,\
		MUIA_Background     , MUII_ButtonBack,\
		MUIA_ShowSelState   , Selected,\
		MUIA_CycleChain     , TRUE,\
		MUIA_ShortHelp	    , GetLocString(HelpTextID), \
		MUIA_ExportID       ,	NumID, \
		End

//----------------------------------------------------------------------------

// Read support

struct DirList
{
	struct MinList list;
	BPTR lock;
	ULONG read;
};

struct DirNode
{
	struct MinNode node;
	STRPTR name;
	struct DirList list;
	struct DirList *parentList;
	ULONG icon;
	ULONG prot;
};

struct FileNode
{
	struct MinNode node;
	STRPTR name;
	ULONG nameLen;
	ULONG icon;
};

//----------------------------------------------------------------------------

#if !defined(__AROS__) && !defined(__amigaos4__)
#include <dos.h>

long _stack = 16384;		// minimum stack size, used by SAS/C startup code
#endif

//----------------------------------------------------------------------------
	
CONST_STRPTR VerString = "$VER: Empty_Trashcan.module 40.1 "__DATE__ " " COMPILER_STRING;

//----------------------------------------------------------------------------

static STRPTR NameOfLock( BPTR lock );
static STRPTR BuildIconName( CONST_STRPTR name );
static BOOL IsDeletable( CONST_STRPTR name, ULONG prot);
static long Unprotect( CONST_STRPTR name, const ULONG icon, ULONG prot);
static BOOL DeleteIcon(CONST_STRPTR name, const ULONG update );
static BOOL DeleteTheFile( CONST_STRPTR name, const ULONG icon, const ULONG update );
static BOOL ReadInDir( struct DirList *list, APTR dirPool );
static BOOL DeleteDir( struct DirList *root, APTR dirPool );
static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static LONG ReportError(LONG Result, CONST_STRPTR Filename, ULONG GadgetMsgId);
static STRPTR GetLocString(ULONG MsgId);
//static void TranslateStringArray(STRPTR *stringArray);
static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT OpenAboutHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT EmptyTrashcanHookFunc(struct Hook *hook, Object *o, Msg msg);
static void DoEmptyTrashcan(struct WBArg *wbarg);
static ULONG ReadDeleteModulePrefs(void);

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase = NULL;
T_LOCALEBASE LocaleBase = NULL;
#ifndef __amigaos4__
T_UTILITYBASE UtilityBase = NULL;
#endif
struct Library *MUIMasterBase = NULL;
struct Library *IconBase = NULL;
struct Library *IFFParseBase = NULL;

#ifdef __amigaos4__
struct IntuitionIFace * IIntuition = NULL;
struct LocaleIFace *ILocale = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
struct IconIFace *IIcon = NULL;
struct IFFParseIFace *IIFFParse = NULL;
#endif

static struct Catalog *Cat;


static ULONG DeleteModuleUseTrashcan = FALSE;
static char DeleteModuleGlobalTrashcan[512] = "";

static BOOL deleteAllProt;

static struct Hook AboutHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutHookFunc), NULL };
static struct Hook AboutMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIHookFunc), NULL };
static struct Hook EmptyTrashcanHook = { { NULL, NULL }, HOOKFUNC_DEF(EmptyTrashcanHookFunc), NULL };

static struct WBArg *wbarg1 = NULL;

//----------------------------------------------------------------------------

static Object *APP_Main;
static Object *WIN_Main, *WIN_AboutMUI, *WIN_Progress;
static Object *OkButton, *CancelButton;
static Object *AbortButton;
static Object *TextCurrentItem;
static Object *MenuAbout, *MenuAboutMUI, *MenuQuit;

//----------------------------------------------------------------------------

static STRPTR NameOfLock( BPTR lock )
{
	STRPTR n;
	BOOL again;
	ULONG bufSize = 127;
	if ( !lock )
		return NULL;

	do
	{
		again = FALSE;
		if ((n = malloc(bufSize)))
		{
			if ( NameFromLock( lock, n, bufSize-1 ) == DOSFALSE )
			{
				if ( IoErr() == ERROR_LINE_TOO_LONG )
				{
					bufSize += 127;
					again = TRUE;
				}
				free(n);
				n = NULL;
			}
		}
	}	while(again);

	return n;
}

//-------------------------------------

static STRPTR BuildIconName( CONST_STRPTR name )
{
	ULONG namelen = strlen(name);
	STRPTR infoname = malloc(namelen+8);
	if (infoname)
	{
		strcpy(infoname,name);
		strcat(infoname,".info");
	}
	return infoname;
}

//-------------------------------------

static BOOL IsDeletable( CONST_STRPTR name, ULONG prot)
{
	if (( prot & FIBF_DELETE ))
	{
		BOOL del = deleteAllProt;

		if ( !deleteAllProt)
		{
			LONG Result;

			//MUI_RequestA(
			Result = MUI_Request(APP_Main, WIN_Main, 0,
				GetLocString(MSGID_EMPTY_TRASHCAN),
				GetLocString(MSGID_GADGETS_DELETE_DELETEALL_PROTECT_SKIP),
				GetLocString(MSGID_DELETE_PROTECTED),
				name);

			switch (Result)
			{
			case	0:
				break;
			case	1:
				del = TRUE;
				break;
			case	2:
				del = deleteAllProt = TRUE;
				break;
			}
		}
		return del;
	}
	return TRUE;
}

//-------------------------------------

static long Unprotect( CONST_STRPTR name, const ULONG icon, ULONG prot)
{
	if (( prot & FIBF_DELETE ))
	{
		BOOL ok;
		if (SetProtection(name,0) != DOSFALSE )
			ok = TRUE;
		else
		{
			LONG ioerr = IoErr();
			if ( ioerr == ERROR_OBJECT_NOT_FOUND )
				ok = TRUE;
			else
			{
//				ok = FALSE;
				return ReportError(ioerr, name, MSGID_GADGETTEXT_CANCEL);
			}
		}

		if (ok && icon )
		{
			if ( icon )
			{
				STRPTR infoname = BuildIconName(name);
				if (infoname)
				{
					if (SetProtection(infoname,0) == DOSFALSE )
					{
						LONG ioerr = IoErr();
						if ( ioerr == ERROR_OBJECT_NOT_FOUND )
							/* ok = TRUE */;
						else
							return ReportError(ioerr, name, MSGID_GADGETS_SKIP_ABORT);
					}

					free(infoname);
				}
			}
		}
	}
	return 2;
}

//-------------------------------------

static BOOL DeleteIcon(CONST_STRPTR name, const ULONG update )
{
	if (update)
		return DeleteDiskObject((STRPTR) name);
	else
	{
		STRPTR iname = BuildIconName(name);
		if (iname)
		{
			BOOL Result;

			Result = DeleteFile(iname);

			free(iname);

			return Result;
		}
	}

	return FALSE;
}

//-------------------------------------

static BOOL DeleteTheFile( CONST_STRPTR name, const ULONG icon, const ULONG update )
{
	BOOL goon = TRUE;
	LONG ioerr;
	BOOL onlyicon = FALSE;


	set(TextCurrentItem, MUIA_Text_Contents, name);

	DoMethod(APP_Main, MUIM_Application_InputBuffered);

	if (DeleteFile(name) == DOSFALSE)
	{
		ioerr = IoErr();
		if ( ioerr == ERROR_OBJECT_NOT_FOUND)
		{
			if (!DeleteIcon(name,update))
			{
				ioerr = IoErr();
				if (ioerr == ERROR_OBJECT_NOT_FOUND)
					ioerr = 0;
			}
			else
				ioerr = 0;
			onlyicon = TRUE;
		}
		else
		{
			if ( ioerr == ERROR_OBJECT_IN_USE )
			{
				if (DeleteFile(name) == DOSFALSE)
					ioerr = IoErr();
				else
					ioerr = 0;
			}
		}
	}
	else
		ioerr = 0;

	if ( ioerr )
	{
		switch( ioerr)
		{
		case	ERROR_DELETE_PROTECTED:
			{
				BOOL del = deleteAllProt;
				if ( !deleteAllProt)
				{
					LONG Result;

					//MUI_RequestA(
					Result = MUI_Request(APP_Main, WIN_Main, 0,
						GetLocString(MSGID_EMPTY_TRASHCAN),
						GetLocString(MSGID_GADGETS_DELETE_DELETEALL_PROTECT_SKIP),
						GetLocString(MSGID_DELETE_PROTECTED),
						name);

					switch (Result)
					{
					case	0:
						del = FALSE;
						break;
					case	1:
						del = TRUE;
						break;
					case	2:
						del = deleteAllProt = TRUE;
						break;
					}
					
				}
				if (del)
				{
					BOOL ok = onlyicon;
					if ( !ok )
						if (SetProtection(name,0) != DOSFALSE )
							ok = TRUE;

					if (ok)//nlyicon || SetProtection(name,NULL) != DOSFALSE )
					{
						BOOL error;

						if ( icon || onlyicon )
						{
							ULONG namelen = strlen(name);
							STRPTR infoname = malloc(namelen+8);
							error = TRUE;
							if (infoname)
							{
								strcpy(infoname,name);
								strcpy(infoname+namelen,".info");

								if (SetProtection(infoname,0) != DOSFALSE )
									error = FALSE;

								free(infoname);
							}
							else
								SetIoErr(ERROR_NO_FREE_STORE);
						}
						else
							 error = FALSE;

						if ( !error )
						{
							if ( onlyicon )
							{
								if (DeleteIcon(name,update))
									return TRUE;
							}
							else
							{
								if (DeleteFile(name) != DOSFALSE )
								{
									if (icon)
									{
										if (DeleteIcon(name,update))
											return TRUE;
									}
									else
										return TRUE;
								}
							}
						}
					}
					ioerr = IoErr();
				}
				else
					break;
			}

		default:
			return (BOOL) ReportError(ioerr, name, MSGID_GADGETS_SKIP_ABORT);
			break;
		}
	}
	else
	{
		if ( icon )
		{
			DeleteIcon(name,update);
		}
	}

	return goon;
}

//-------------------------------------

static BOOL ReadInDir( struct DirList *list, APTR dirPool )
{
	BOOL retval = TRUE;
	STRPTR lockName;

	if ( !list->lock )
	{
		d1(KPrintF(__FILE__ "/%s/%ld: No Lock\n", __FUNC__, __LINE__));
		return FALSE;
	}

	if ( list->read )
	{
		d1(KPrintF(__FILE__ "/%s/%ld: Already read", __FUNC__, __LINE__));
		return TRUE;
	}

	lockName = NameOfLock(list->lock);
	if ( lockName )
	{
		ULONG lockSize = strlen(lockName);
		APTR filePool = CreatePool( 0,4096,4096);
		if ( filePool )
		{
			struct ExAllControl *eac;
			eac = (struct ExAllControl*)AllocDosObject(DOS_EXALLCONTROL,NULL);
			if ( eac )
			{
				struct ExAllData *ead;
				ead = (struct ExAllData*) malloc(1032);
				if (ead)
				{
					LONG more;
					struct MinList fileList;
					NewList((struct List*)&fileList);

					eac->eac_LastKey = 0;
					eac->eac_MatchString = NULL;
		
					list->read = TRUE;
		
					do
					{
						struct ExAllData *ed = ead;
						more = ExAll( list->lock, ed, 1024, ED_PROTECTION, eac );
						if ((!more) &&  (IoErr() != ERROR_NO_MORE_ENTRIES ))
						{
							retval = FALSE;
							break;
						}
		
						if ( eac->eac_Entries )
						{
							do
							{
								ULONG nlen = strlen(ed->ed_Name);
								ULONG len = nlen+8+lockSize;
								if ( ed->ed_Type > 0 )
								{
									STRPTR name = (STRPTR)AllocPooled(dirPool,len);
									if (name)
									{
										struct DirNode *dir = (struct DirNode*)AllocPooled(dirPool,sizeof(struct DirNode));
										if ( dir )
										{
											strcpy(name,lockName);
											AddPart(name,ed->ed_Name,len);
	
											dir->name = name;
											dir->list.lock = Lock( name, ACCESS_READ);
											dir->list.read = FALSE;
											dir->parentList = list;
											dir->icon = FALSE;
											dir->prot = ed->ed_Prot;
											NewList((struct List*)&dir->list);
				
											AddTail((struct List*)list, (struct Node*)dir);
										}
									}
								}
								else
								{
									struct FileNode *file = (struct FileNode*)AllocPooled(filePool,sizeof(struct FileNode));
									if ( file )
									{
										STRPTR name = (STRPTR)AllocPooled(filePool,len);
										if ( name )
										{
											strcpy(name,lockName);
											AddPart(name,ed->ed_Name,len);

											file->name = name;
											file->nameLen = strlen(name);
											file->icon = FALSE;
											AddTail((struct List*)&fileList, (struct Node*)file);
										}
									}
								}
								ed = ed->ed_Next;
							} while(ed);
						}
					} while(more);

					ExAllEnd( list->lock, ead, 1024, ED_PROTECTION, eac );
					free(ead);

					if ( !IsListEmpty((struct List*)&fileList))
					{
						struct FileNode *node;
						struct DirNode *dnode;

						node = (struct FileNode*)fileList.mlh_Head;
						while(node->node.mln_Succ && retval)
						{
							// Icons aussortieren
							if ( node->nameLen > 4 )
							{
								if ( !Stricmp(".info",node->name+node->nameLen-5))
								{
									struct FileNode *fnode = (struct FileNode*)fileList.mlh_Head;
									BOOL iconfound = FALSE;
									node->name[node->nameLen-5] = 0;  
									while(fnode->node.mln_Succ)
									{
										if ( fnode != node )
										{
											if ( !Stricmp(node->name,fnode->name))
											{
												fnode->icon = iconfound = TRUE;
												break;
											}
										}
										fnode = (struct FileNode*)fnode->node.mln_Succ;
									}

									if ( !iconfound )
									{
										dnode = (struct DirNode*)list->list.mlh_Head;
										while(dnode->node.mln_Succ)
										{
											if ( !Stricmp(node->name,dnode->name))
											{
												dnode->icon = iconfound = TRUE;
												break;
											}

											dnode = (struct DirNode*)dnode->node.mln_Succ;
										}
									}

									if ( iconfound )
									{
										struct FileNode *tfn = node;
										node = (struct FileNode*)node->node.mln_Succ;
										Remove((struct Node*)tfn);
										continue;
									}
								}
							}
							node = (struct FileNode*)node->node.mln_Succ;
						}

						node = (struct FileNode*)fileList.mlh_Head;
						while(node->node.mln_Succ && retval)
						{
							retval = DeleteTheFile(node->name,node->icon,FALSE);
							node = (struct FileNode*)node->node.mln_Succ;
						}

						dnode = (struct DirNode*)list->list.mlh_Head;
						while(dnode->node.mln_Succ)
						{
							struct DirNode *help = dnode;
							dnode = (struct DirNode*)dnode->node.mln_Succ;
							if ( !IsDeletable(help->name,help->prot))
							{
								UnLock(help->list.lock);
								Remove((struct Node*)help);
							}
						}
					}
				}

				FreeDosObject(DOS_EXALLCONTROL,eac);
			}
			DeletePool(filePool);
		}
		free(lockName);
	}
	return retval;
}

//-------------------------------------

static BOOL DeleteDir( struct DirList *root, APTR dirPool )
{
	struct DirList *cl = root;
	BOOL goon = TRUE;

	d1(kprintf(__FUNC__ "/%ld: root = %08lx  dirPool = %08lx\n", __LINE__, root, dirPool));

	while(cl)
	{
		if (goon)
		{
			if (!ReadInDir(cl,dirPool))
				goon = FALSE;
		}

		if ( !IsListEmpty((struct List*)cl))
		{
			struct DirNode *cn = (struct DirNode*)cl->list.mlh_Head;
			cl = (struct DirList*)&cn->list;
		}
		else
		{
			if (cl != root )
			{
				struct DirNode *cn = (struct DirNode*)(((ULONG*)cl)-3);
				struct DirNode *ncn = (struct DirNode*)cn->node.mln_Succ;

				if ( !ncn->node.mln_Succ )
				{
					do
					{
						if ( cl == root )
							return TRUE;

						cl = cn->parentList;
						if ( !cl )
						{
							d1(KPrintF(__FILE__ "/%s/%ld: Ready...", __FUNC__, __LINE__));
							return TRUE;
						}

						if ( IsListEmpty((struct List*)cl))
						{
							return FALSE;
						}

						ncn = (struct DirNode*)RemHead((struct List*)cl);
						cn = (struct DirNode*)(((ULONG*)cl)-3);

						if (ncn->list.lock)
						{
							UnLock(ncn->list.lock);
							ncn->list.lock = (BPTR)NULL;
						}

						if ( goon )
						{
							Unprotect(ncn->name,ncn->icon,ncn->prot);
							goon = DeleteTheFile(ncn->name,ncn->icon,FALSE);
						}
					} while( IsListEmpty((struct List*)cl));
					if ( !cl )
						break;

					ncn = (struct DirNode*)cl->list.mlh_Head;
				}
				else
				{
					Remove((struct Node*)cn);

					if (cn->list.lock)
					{
						UnLock(cn->list.lock);
						cn->list.lock = (BPTR)NULL;
					}

					if ( goon )
					{
						Unprotect(cn->name,cn->icon,cn->prot);
						goon = DeleteTheFile(cn->name,cn->icon,FALSE);
					}
				}

				cl = (struct DirList*)&ncn->list;
			}
			else 
				return FALSE;
		}
	}

	return FALSE;
}

//-------------------------------------

int main(int argc, char *argv[])
{
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

	wbarg1 = &WBenchMsg->sm_ArgList[1];
	do	{
		LONG win_opened = FALSE;
		BOOL Run = TRUE;

		if (!OpenLibraries())
			break;

		if (LocaleBase)
			Cat = OpenCatalogA( NULL, "Scalos/Empty_Trashcan.catalog", NULL);

		ReadDeleteModulePrefs();

		APP_Main = ApplicationObject,
			MUIA_Application_Title,		GetLocString(MSGID_TITLENAME),
			MUIA_Application_Version,	"$VER: Scalos Empty_Trashcan.module V40.2 (" __DATE__ ") " COMPILER_STRING,
			MUIA_Application_Copyright,	"© The Scalos Team, 2005" CURRENTYEAR,
			MUIA_Application_Author,	"The Scalos Team",
			MUIA_Application_Description,	"Scalos Empty_Trashcan module",
			MUIA_Application_Base,		"SCALOS_EMPTY_TRASH",

			SubWindow, WIN_Main = WindowObject,
				MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
//				MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
				MUIA_Window_AppWindow, FALSE,

				MUIA_Window_TopEdge, MUIV_Window_TopEdge_Moused,
				MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Moused,

				MUIA_Window_Width, MUIV_Window_Width_Screen(30), //+jmc+

				WindowContents, VGroup,
					Child, TextObject,
						MUIA_Text_PreParse, MUIX_C,
						MUIA_Text_Contents, GetLocString(MSGID_WARNING_EMPTY_TRASHCAN),
						End, //TextObject

					Child, VGroup,
						MUIA_ShowMe, strlen(DeleteModuleGlobalTrashcan) > 0 && DeleteModuleUseTrashcan,

						Child, TextObject,
							MUIA_Text_PreParse, MUIX_C,
							MUIA_Text_Contents, GetLocString(MSGID_GLOBAL_TRASHCAN),
							End, //TextObject
						Child, TextObject,
							MUIA_Text_PreParse, MUIX_C,
							MUIA_Text_Contents, DeleteModuleGlobalTrashcan,
							End, //TextObject
						End, //VGroup

					Child, HGroup,
						MUIA_Group_SameWidth, TRUE,
						Child, OkButton = KeyButtonHelp(GetLocString(MSGID_OKBUTTON),
									*GetLocString(MSGID_OKBUTTON_SHORT),
                                                                        GetLocString(MSGID_SHORTHELP_OKBUTTON)),
						Child, CancelButton = KeyButtonHelp(GetLocString(MSGID_CANCELBUTTON),
									*GetLocString(MSGID_CANCELBUTTON_SHORT),
                                                                        GetLocString(MSGID_SHORTHELP_CANCELBUTTON)),
						End, //HGroup
					End, //VGroup

				End, //WindowObject

			SubWindow, WIN_Progress = WindowObject,
				MUIA_Window_Title, GetLocString(MSGID_TITLENAME),
//				MUIA_Window_ID,	MAKE_ID('M','A','I','N'),
				MUIA_Window_AppWindow, FALSE,

				MUIA_Window_TopEdge, MUIV_Window_TopEdge_Moused,
				MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Moused,

				MUIA_Window_Width, MUIV_Window_Width_Screen(30), //+jmc+

				WindowContents, VGroup,
					Child, TextObject,
						MUIA_Text_PreParse, MUIX_C,
						MUIA_Text_Contents, GetLocString(MSGID_TEXT_CURRENTITEM),
						MUIA_ShortHelp, GetLocString(MSGID_TEXT_CURRENTITEM_SHORTHELP),
						End, //TextObject

					Child, TextCurrentItem = TextObject,
						MUIA_Text_PreParse, MUIX_C,
						MUIA_Text_Contents, "",
						End, //TextObject

					Child, HGroup,
						MUIA_Group_SameWidth, TRUE,
						Child, AbortButton = KeyButtonHelp(GetLocString(MSGID_ABORTBUTTON),
									*GetLocString(MSGID_ABORTBUTTON_SHORT), GetLocString(MSGID_SHORTHELP_ABORTBUTTON)),
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

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));

		if (NULL == APP_Main)
			{
			printf(GetLocString(MSGID_CREATE_APPLICATION_FAILED));
			break;
			}

		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			WIN_Main, 3, MUIM_Set, MUIA_Window_Open, FALSE);

		DoMethod(WIN_Main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		// If cancel button is pressed, quit
		DoMethod(CancelButton, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		// Ok button is pressed, start emptying the trashcan
		DoMethod(OkButton, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_CallHook, &EmptyTrashcanHook);

		DoMethod(AbortButton, MUIM_Notify, MUIA_Pressed, FALSE,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutHook);

		DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
			APP_Main, 2, MUIM_CallHook, &AboutMUIHook);

		DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
			APP_Main, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

		set(WIN_Main, MUIA_Window_Open, TRUE);
		get(WIN_Main, MUIA_Window_Open, &win_opened);

		if (!win_opened)
			{
			printf(GetLocString(MSGID_CREATE_MAINWINDOW_FAILED));
			break;
			}

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: \n", __LINE__));

		while (Run)
			{
			LONG sigs;
			ULONG Action = DoMethod(APP_Main, MUIM_Application_NewInput, &sigs);

			switch (Action)
				{
			case Application_Return_Ok:
				Run = FALSE;
				break;
			case MUIV_Application_ReturnID_Quit:
				Run = FALSE;
				break;
				}
			}
		} while (0);

	set(WIN_Main, MUIA_Window_Open, FALSE);

	if (APP_Main)
		MUI_DisposeObject(APP_Main);
	if (Cat)
		CloseCatalog(Cat);

	CloseLibraries();

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT EmptyTrashcanHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	BPTR ParentLock = (BPTR)NULL;
	BPTR DirLock = (BPTR)NULL;

	(void) hook,
	(void) o;
	(void) msg;

	// Empty selected trashcan
	DoEmptyTrashcan(wbarg1);

	// Now try to empty global trashcan, if defined
	do	{
		struct WBArg globalTrashcanWBArg;

		if (strlen(DeleteModuleGlobalTrashcan) < 1)
			break;
		if (!DeleteModuleUseTrashcan)
			break;

		globalTrashcanWBArg.wa_Name = FilePart(DeleteModuleGlobalTrashcan);
		if (NULL == globalTrashcanWBArg.wa_Name)
			break;

		DirLock = Lock(DeleteModuleGlobalTrashcan, ACCESS_READ);
		if ((BPTR)NULL == DirLock)
			break;

		ParentLock = globalTrashcanWBArg.wa_Lock = ParentDir(DirLock);
		// NULL is valid for ParentLock

		// Now empty global trashcan
		DoEmptyTrashcan(&globalTrashcanWBArg);
		} while (0);

	if (ParentLock)
		UnLock(ParentLock);
	if (DirLock)
		UnLock(DirLock);
}

//----------------------------------------------------------------------------

static void DoEmptyTrashcan(struct WBArg *wbarg)
{
	BPTR OldDir = (BPTR)NULL;
	BPTR fLock = (BPTR)NULL;
	struct FileInfoBlock *fib = NULL;
	APTR dirPool = NULL;
	STRPTR name = NULL;
	LONG win_opened = FALSE;

	do	{
		struct DirList list;

		// Close initial window
		set(WIN_Main, MUIA_Window_Open, FALSE);

		// Open progress display window
		set(WIN_Progress, MUIA_Window_Open, TRUE);
		get(WIN_Progress, MUIA_Window_Open, &win_opened);

		if (!win_opened)
			{
			printf(GetLocString(MSGID_CREATE_MAINWINDOW_FAILED));
			break;
			}

		d1(kprintf(__FUNC__ "/%ld: \n", __LINE__));

		OldDir = CurrentDir(wbarg->wa_Lock);

		if (wbarg->wa_Name)
			{
			d1(kprintf(__FUNC__ "/%ld: wa_Name = <%s>\n", __LINE__, wbarg->wa_Name));
			fLock = Lock(wbarg->wa_Name, ACCESS_READ);
			}
		else
			{
			fLock = Lock("", ACCESS_READ);
			}

		if ((BPTR)NULL == fLock)
			break;

		fib = AllocDosObject(DOS_FIB, NULL);
		if (NULL == fib)
			break;

		if (!Examine(fLock, fib))
			break;

		if (fib->fib_DirEntryType < 0)
			break;
			
		dirPool = CreatePool(MEMF_PUBLIC, 4096, 4096);
		if (NULL == dirPool)
			break;

		d1(kprintf(__FUNC__ "/%ld: dirPool = %08lx\n", __LINE__, dirPool));

		name = NameOfLock(wbarg->wa_Lock);
		d1(kprintf(__FUNC__ "/%ld: name = %08lx\n", __LINE__, name));
		if (NULL == name)
			break;

		NewList((struct List*)&list);
					
		list.lock = fLock;
		list.read = FALSE;

		DeleteDir(&list, dirPool);

		// Close progress window
		set(WIN_Progress, MUIA_Window_Open, FALSE);
		} while (0);

	if (name)
		free(name);
	if (dirPool)
		DeletePool(dirPool);
	if (fib)
		FreeDosObject(DOS_FIB, fib);
	if (fLock)
		UnLock(fLock);
	if (OldDir)
		CurrentDir(OldDir);

	if (Cat)
		CloseCatalog( Cat );

	DoMethod(APP_Main, MUIM_Application_ReturnID, Application_Return_Ok);
}

//-------------------------------------

static BOOL OpenLibraries(void)
{
	MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN-1);
	if (NULL == MUIMasterBase)
		return FALSE;
#ifdef __amigaos4__
	IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "Main", 1, NULL);
	if (NULL == IMUIMaster)
		return FALSE;
#endif

	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		return FALSE;
#ifdef __amigaos4__
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "Main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
#endif

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 38);
	if (NULL == LocaleBase)
		return FALSE;
#ifdef __amigaos4__
	ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "Main", 1, NULL);
	if (NULL == ILocale)
		return FALSE;
#endif

#ifndef __amigaos4__
	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;
#endif

	IconBase = OpenLibrary("icon.library", 37);
	if (NULL == IconBase)
		return FALSE;
#ifdef __amigaos4__
	IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "Main", 1, NULL);
	if (NULL == IIcon)
		return FALSE;
#endif

	IFFParseBase = OpenLibrary("iffparse.library", 37);
	if (NULL == IFFParseBase)
		return FALSE;
#ifdef __amigaos4__
	IIFFParse = (struct IFFParseIFace *)GetInterface((struct Library *)IFFParseBase, "Main", 1, NULL);
	if (NULL == IIFFParse)
		return FALSE;
#endif

	return TRUE;
}

//----------------------------------------------------------------------------

static void CloseLibraries(void)
{
#ifdef __amigaos4__
	if (IIFFParse)
		{
		DropInterface((struct Interface *)IIFFParse);
		IIFFParse = NULL;
		}
#endif
	if (IFFParseBase)
		{
		CloseLibrary(IFFParseBase);
		IFFParseBase = NULL;
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

static LONG ReportError(LONG IoResult, CONST_STRPTR Filename, ULONG GadgetMsgId)
{
	char FaultText[256];
	LONG Result;

	Fault(IoResult, "", FaultText, sizeof(FaultText));

	Result = MUI_Request(APP_Main, WIN_Main, 0,
		GetLocString(MSGID_EMPTY_TRASHCAN),
		GetLocString(GadgetMsgId),
		GetLocString(MSGID_FAILED_TO_DELETE),
		Filename, FaultText);

	return Result;
}

//----------------------------------------------------------------------------

static STRPTR GetLocString(ULONG MsgId)
{
	struct Empty_Trashcan_LocaleInfo li;

	li.li_Catalog = Cat;
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return (STRPTR)GetEmpty_TrashcanString(&li, MsgId);
}

/*
static void TranslateStringArray(STRPTR *stringArray)
{
	while (*stringArray)
		{
		*stringArray = GetLocString((ULONG) *stringArray);
		stringArray++;
		}
}
*/
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
	MUI_Request(APP_Main, WIN_Main, 0,
		GetLocString(MSGID_EMPTY_TRASHCAN),
		GetLocString(MSGID_ABOUTREQOK),
		GetLocString(MSGID_ABOUTREQFORMAT),
		VERSION_MAJOR, VERSION_MINOR, COMPILER_STRING, CURRENTYEAR);
}

//----------------------------------------------------------------------------

static ULONG ReadDeleteModulePrefs(void)
{
	LONG Result;
	struct IFFHandle *iff;
	BOOL iffOpened = FALSE;
	ULONG *MuioChunk = NULL;

	do	{
		iff = AllocIFF();
		if (NULL == iff)
			{
			Result = IoErr();
			break;
			}

		InitIFFasDOS(iff);

		iff->iff_Stream = (IPTR)Open("ENV:mui/DELETE.MODULE.cfg", MODE_OLDFILE);
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: iff_Stream=%08lx\n", __LINE__, iff->iff_Stream));

		if (0 == iff->iff_Stream)
			{
			Result = IoErr();
			break;
			}

		Result = OpenIFF(iff, IFFF_READ);
		if (RETURN_OK != Result)
			break;

		iffOpened = TRUE;

		Result = StopChunk(iff, ID_PREF, ID_MUIO);
		if (RETURN_OK != Result)
			break;

		while (RETURN_OK == Result)
			{
			struct ContextNode *cn;

			Result = ParseIFF(iff, IFFPARSE_SCAN);
			if (RETURN_OK != Result)
				break;

			cn = CurrentChunk(iff);
			if (NULL == cn)
				continue;

			if (ID_MUIO == cn->cn_ID)
				{
				LONG Actual;
				LONG len = cn->cn_Size;
				const ULONG *lptr;

				d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: iff_Stream=%08lx\n", __LINE__, cn->cn_ID, cn->cn_Size));

				MuioChunk = malloc(cn->cn_Size);
				if (NULL == MuioChunk)
					break;

				Actual = ReadChunkBytes(iff, MuioChunk, cn->cn_Size);
				if (Actual < 0)
					break;

				lptr = MuioChunk;
				while (len > 0)
					{
					size_t EntryLen = lptr[1];
					size_t slen;

					d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: len=%ld  ID=%08lx  EntryLen=%lu\n", __LINE__, len, lptr[0], EntryLen));

					switch (lptr[0])
						{
					case ID_CMTC:	// Use Trashcan
						DeleteModuleUseTrashcan = lptr[2];
						d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: DeleteModuleUseTrashcan=%ld\n", __LINE__, DeleteModuleUseTrashcan));
						break;
					case ID_STTC:	// Global trashcan location
						slen = EntryLen;
						if (slen >= sizeof(DeleteModuleGlobalTrashcan))
							slen = sizeof(DeleteModuleGlobalTrashcan) - 1;
						memcpy(DeleteModuleGlobalTrashcan, (char *) &lptr[2], slen);
						d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: DeleteModuleGlobalTrashcan=<%s>\n", __LINE__, DeleteModuleGlobalTrashcan));
						break;
					default:
						break;
						}

					len -= 2 * sizeof(ULONG) + EntryLen;
					lptr = (ULONG *) (((UBYTE *) &lptr[2]) + EntryLen);
					}


				}
			}
		} while (0);

	if (iff)
		{
		if (iffOpened)
			CloseIFF(iff);

		if (iff->iff_Stream)
			Close((BPTR)iff->iff_Stream);

		FreeIFF(iff);
		}
	if (MuioChunk)
		free(MuioChunk);

	return (ULONG) Result;
}

//----------------------------------------------------------------------------


