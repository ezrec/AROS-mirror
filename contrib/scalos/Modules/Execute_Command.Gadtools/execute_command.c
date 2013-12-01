#include <string.h>
#include <dos.h>

#include <dos/dostags.h>
#include <intuition/imageclass.h>
#include <workbench/startup.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/wb_protos.h>
#include <clib/asl_protos.h>
#include <clib/locale_protos.h>
#include <clib/utility_protos.h>

#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/wb_pragmas.h>
#include <pragmas/asl_pragmas.h>
#include <pragmas/locale_pragmas.h>
#include <pragmas/utility_pragmas.h>

__stdargs void NewList( struct List *list );
static void ecmain(void);
static void wbmain(struct WBStartup *wbs );

IMPORT struct Library *GadToolsBase;
IMPORT struct Library *IntuitionBase;
IMPORT struct Library *SysBase;
IMPORT struct Library *GfxBase;
IMPORT struct Library *DOSBase;
IMPORT struct Library *LocaleBase;
IMPORT struct Library *UtilityBase;

struct Library *WorkbenchBase;

struct Catalog *Cat;

struct Screen *scr;
struct DrawInfo *drinfo;
APTR visualinfo;
ULONG offx,offy,fonty,sizeheight,uwidth;
ULONG wndwidth,wndheight;

struct Window *wnd;
struct MsgPort *appMsgPort;
struct AppWindow *appWnd;
struct Gadget *glist;
struct Gadget *commandString;
struct Gadget *stackInteger;
STRPTR commandStr;
ULONG stackNum;

BOOL glistAttached;
BOOL sizeVerify;
BOOL fromWB;

const STATIC STRPTR engLocText[] =
{
	"Enter command and its Arguments",
	"_Command",
	"_OK",
	"_Cancel",
	"Execute a file",
	"_Stack",
	"Choose a command",
	"Scalos Output Window"
};

UWORD ok_key, cancel_key, stack_key, command_key, commandbut_key;

STRPTR locText[8];

const STRPTR VerString = "$VER: execute_command.module 1.0 "__AMIGADATE__;

#define WND_COMMAND_STRING 1
#define WND_COMMAND_BUTTON 2
#define WND_OK_BUTTON 3
#define WND_STACK_INTEGER 4
#define WND_CANCEL_BUTTON 5

//-------------------------------------
STATIC VOID SR_CopyFunc(VOID)
{
	__emit(0x16c0); // MOVE.B  D0,(A3)+
}
//-------------------------------------
VOID sprintf( STRPTR buf, const STRPTR fmt,...)
{
	RawDoFmt((const STRPTR)fmt,(STRPTR*)&fmt+1,(void(*)())SR_CopyFunc,buf);
}

//-------------------------------------
ULONG StrLen( const STRPTR str )
{
	if( !str) return 0;
	return strlen(str);
}
//-------------------------------------
STRPTR StrCopy( const STRPTR str )
{
	STRPTR dst;
	if( !str ) return NULL;
	if( !*str) return NULL;

	dst = (STRPTR)AllocVec(strlen(str)+1,0);
	if(dst) strcpy(dst,str);
	return dst;
}
//-------------------------------------
STRPTR BuildConName( STRPTR wtitle, struct Screen *scr )
{
  const STATIC STRPTR fmt = "CON:/%ld//%ld/%s/AUTO/CLOSE/WAIT";

	STRPTR buf = (STRPTR)AllocVec( strlen(fmt)+strlen(wtitle)+30,0 );
	if( buf )
	{
		LONG height=280;
		LONG top=13;
		if( scr )
		{
			struct Rectangle rect;
			if(QueryOverscan( GetVPModeID( &scr->ViewPort ), &rect, OSCAN_TEXT ))
			{
				height = rect.MaxY - rect.MinY + 1;
			} else height = scr->Height;

			height = height*2/3;
			top = -scr->TopEdge;
			if( top <= scr->BarHeight ) top = scr->BarHeight + 1;
		}
		sprintf( buf, fmt, top, height, wtitle );
	}
	return buf;
}
//-------------------------------------
STRPTR NameOfLock( BPTR lock )
{
	STRPTR n;
	BOOL again;
	ULONG bufSize = 127;
	if( !lock ) return NULL;

	do
	{
		again = FALSE;
		if((n = (STRPTR)AllocVec(bufSize, 0x10000 )))
		{
			if( NameFromLock( lock, n, bufSize-1 ) == DOSFALSE )
			{
				if( IoErr() == ERROR_LINE_TOO_LONG )
				{
					bufSize += 127;
					again = TRUE;
				}
				FreeVec(n);
				n = NULL;
			}
		}
	}	while(again);

	return n;
}
//-------------------------------------
STRPTR WBArg2String( struct WBArg *arg, LONG numArgs )
{
	struct WBArgNode
	{
		struct MinNode node;
		STRPTR dirname;
		STRPTR filename;
	};

	struct MinList list;
	ULONG len=0;
	STRPTR buf;
	UWORD i;

	NewList((struct List*)&list );

	for( i=0; i<numArgs; i++)
	{
		struct WBArgNode *node = (struct WBArgNode*)AllocVec(sizeof(struct WBArgNode),0x10000);
		if(node)
		{
			STRPTR n = NameOfLock(arg[i].wa_Lock);
			node->dirname = n;
			node->filename = arg[i].wa_Name;

			len += StrLen( n )+StrLen(arg[i].wa_Name)+8;
			AddTail((struct List*)&list, (struct Node*)node );
		}
	}

	buf = (STRPTR)AllocVec(len+2,0x10000);
	if(buf)
	{
		struct WBArgNode *node;
		STRPTR bufPtr = buf;

		while(( node = (struct WBArgNode*)RemHead((struct List*)&list)))
		{
			STRPTR n = node->dirname;
			if( !n ) continue;

			if( n[0] ) strcpy(bufPtr,n );
			AddPart(bufPtr,node->filename,len-(bufPtr-buf));

			bufPtr+=strlen(bufPtr);
			if(list.mlh_Head->mln_Succ)
				*bufPtr++=' ';

			FreeVec(n);
			FreeVec(node);
		}
		*bufPtr = 0;
	}
	return buf;
}
//-------------------------------------
STRPTR GetFullPath( const STRPTR drw, const STRPTR file )
{
	ULONG length = StrLen(drw)+StrLen(file)+4;
	STRPTR fp = (STRPTR)AllocVec( length+1,0 );
	if( fp )
	{
		strcpy( fp, drw );

		if( AddPart( fp, file, length ))	return fp;
		FreeVec( fp );
	}
	return NULL;
}
//-------------------------------------
STRPTR RequestAFile(void)
{
	STRPTR file_name = NULL;
	struct Library *AslBase;

	AslBase = OpenLibrary("asl.library",39);
	if( AslBase )
	{
		struct FileRequester *file_req;

		file_req = (struct FileRequester*)AllocAslRequestTags(ASL_FileRequest,
														ASLFR_RejectIcons, TRUE,
														ASLFR_Window,wnd,
														ASLFR_SleepWindow,TRUE,
														ASLFR_TitleText,locText[6],
														TAG_DONE);
		if( file_req )
		{
			if(AslRequest(file_req, NULL))
			{
				file_name = GetFullPath( file_req->fr_Drawer, file_req->fr_File );
			}
			FreeAslRequest(file_req);
		}
		CloseLibrary(AslBase);
	}
	return file_name;
}
//-------------------------------------
ULONG FindUnderscoredToLower(STRPTR text)
{
	ULONG c;
	while(( c = *text++))
	{
		if( c == '_' ) return ToLower(*text);
	}
	return 0;
}
//-------------------------------------

//-------------------------------------
STATIC VOID InitStrings(void)
{
	ULONG i;
	for(i=0; i<8; i++)
	{
		locText[i] = GetCatalogStr( Cat, i, engLocText[i]);
	}

	command_key = FindUnderscoredToLower(locText[1]);
	commandbut_key = ToUpper(command_key);
	ok_key = FindUnderscoredToLower(locText[2]);
	cancel_key = FindUnderscoredToLower(locText[3]);
	stack_key = FindUnderscoredToLower(locText[5]);
}
//-------------------------------------

//-------------------------------------
STATIC VOID GetCommand(void)
{
	STRPTR cmdStr;

	if(GT_GetGadgetAttrs(commandString,wnd,NULL,
								GTST_String, &cmdStr,
								TAG_DONE ))
	{
		if( commandStr ) FreeVec(commandStr);
		commandStr=StrCopy(cmdStr);
	}
}
//-------------------------------------
STATIC VOID GetStack(void)
{
	GT_GetGadgetAttrs(stackInteger,wnd,NULL,
								GTIN_Number,&stackNum,
								TAG_DONE );

	if( stackNum < 4096 )
	{
		stackNum = 4096;
		GT_SetGadgetAttrs(stackInteger,wnd,NULL,
								GTIN_Number,stackNum,
								TAG_DONE );

	}
}
//-------------------------------------
STATIC VOID SetCommand(void)
{
	GT_SetGadgetAttrs(commandString,wnd,NULL,
								GTST_String, commandStr?commandStr:(STRPTR)"",
								TAG_DONE );
}
//-------------------------------------

//-------------------------------------
STATIC VOID CommandButton(void)
{
	STRPTR cstr = RequestAFile();
	if( cstr )
	{
		if(commandStr)FreeVec(commandStr);
		commandStr = cstr;
		SetCommand();
		((struct StringInfo*)(commandString->SpecialInfo))->BufferPos = strlen(cstr);
		ActivateGadget(commandString, wnd, NULL );
	}
}
//-------------------------------------

//-------------------------------------
STATIC BOOL SetupScreen(void)
{
	if((scr = LockPubScreen(NULL)))
	{
		if((drinfo = GetScreenDrawInfo(scr)))
		{
			if((visualinfo = GetVisualInfoA(scr,NULL)))
			{
				Object *o = (Object*)NewObject( NULL, SYSICLASS,
															SYSIA_DrawInfo,drinfo,
															SYSIA_Which, SIZEIMAGE,
															TAG_DONE);
				if(o)
				{
					if(!GetAttr( IA_Height,o,&sizeheight)) sizeheight = 11;
					DisposeObject(o);
				}	else sizeheight = 11;
				uwidth = TextLength(&scr->RastPort,"_",1);
				return TRUE;
			}
		}
	}
	return FALSE;
}
//-------------------------------------
STATIC VOID FreeScreen(void)
{
	if( visualinfo ) FreeVisualInfo(visualinfo);
	if( drinfo ) FreeScreenDrawInfo(scr,drinfo);
	if( scr ) UnlockPubScreen(NULL,scr);
}
//-------------------------------------
STATIC VOID Render(void)
{
	Move(wnd->RPort,offx+4,offy+drinfo->dri_Font->tf_Baseline+4);
	SetABPenDrMd(wnd->RPort,drinfo->dri_Pens[TEXTPEN],0,JAM1);
	Text(wnd->RPort,locText[0],strlen(locText[0]));
}
//-------------------------------------
STATIC BOOL CreateGadgets(void)
{
	struct Gadget *g;

	if(glist)
	{
		FreeGadgets(glist);
		glist = NULL;
	}

	g = CreateContext( &glist );
	if( g )
	{
		struct NewGadget ng;
		WORD commandButtonWidth = TextLength(&scr->RastPort,"«",1)+6;
		WORD usableWidth = wndwidth-2*offx-24-TextLength(&scr->RastPort,locText[5],strlen(locText[5]))-uwidth-10;
		BOOL setCursor;
		UWORD cancelLeft;

		ng.ng_VisualInfo = visualinfo;
		ng.ng_TextAttr = NULL;
		ng.ng_GadgetText = locText[1];
		ng.ng_GadgetID = WND_COMMAND_STRING;
		ng.ng_Flags = NULL;
		ng.ng_UserData = NULL;
		ng.ng_LeftEdge = offx+13+TextLength(&scr->RastPort,ng.ng_GadgetText,strlen(ng.ng_GadgetText))-uwidth;
		ng.ng_TopEdge = offy+fonty+7;
		ng.ng_Width = wndwidth-ng.ng_LeftEdge-offx-4-commandButtonWidth;
		ng.ng_Height = fonty+6;

		if( !commandString && commandStr ) setCursor = TRUE;
		else setCursor = FALSE;

		g = commandString = CreateGadget( STRING_KIND, g, &ng,
														GTST_String, commandStr?commandStr:(STRPTR)"",
														GT_Underscore,'_',
														TAG_DONE);

		if( setCursor )
			((struct StringInfo*)(commandString->SpecialInfo))->BufferPos = strlen(commandStr);


		ng.ng_LeftEdge += ng.ng_Width;
		ng.ng_Width = commandButtonWidth;
		ng.ng_GadgetID = WND_COMMAND_BUTTON;
		ng.ng_GadgetText = "«";

		g = CreateGadgetA( BUTTON_KIND, g, &ng, NULL );

		ng.ng_TopEdge += ng.ng_Height+4;
		ng.ng_Width = usableWidth/3;
		ng.ng_GadgetText = locText[2];
		ng.ng_LeftEdge = offx+4;
		ng.ng_GadgetID = WND_OK_BUTTON;

		g = CreateGadget( BUTTON_KIND, g, &ng,
														GT_Underscore,'_',
														TAG_DONE);

		cancelLeft = wndwidth-offx-4-ng.ng_Width;

		ng.ng_LeftEdge += ng.ng_Width + TextLength(&scr->RastPort,locText[5],5)-uwidth+20;
		ng.ng_Width = cancelLeft - ng.ng_LeftEdge - 10;
		ng.ng_GadgetText = locText[5];
		ng.ng_GadgetID = WND_STACK_INTEGER;

		stackInteger = g = CreateGadget( INTEGER_KIND, g, &ng,
														GT_Underscore,'_',
														GTIN_Number, stackNum,
														TAG_DONE);

		ng.ng_GadgetText = locText[3];
		ng.ng_LeftEdge = cancelLeft;
		ng.ng_GadgetID = WND_CANCEL_BUTTON;
		ng.ng_Width = usableWidth/3;

		g = CreateGadget( BUTTON_KIND, g, &ng,
														GT_Underscore,'_',
														TAG_DONE);

		if(g) return TRUE;

		FreeGadgets(glist);
		glist=NULL;
	}
	return FALSE;
}
//-------------------------------------
STATIC VOID FreeGUI(void)
{
	if(appWnd) RemoveAppWindow(appWnd);
	if(appMsgPort) DeleteMsgPort(appMsgPort);
	if(wnd) CloseWindow(wnd);
	if(glist) FreeGadgets(glist);
}
//-------------------------------------
STATIC BOOL SetupGUI(void)
{
	fonty = drinfo->dri_Font->tf_YSize;
	offx = scr->WBorLeft;
	offy = scr->WBorTop + fonty + 1;

	wndwidth = TextLength(&scr->RastPort,locText[0],strlen(locText[0]))+TextLength(&scr->RastPort,locText[1],strlen(locText[1]))+TextLength(&scr->RastPort,"«",1)+6;
	wndheight = offy + 3*fonty + sizeheight + 26;

	if( CreateGadgets())
	{
		ULONG idcmp = IDCMP_CLOSEWINDOW|IDCMP_NEWSIZE|BUTTONIDCMP|IDCMP_VANILLAKEY|STRINGIDCMP;
		if( sizeVerify ) idcmp |= IDCMP_SIZEVERIFY;

		wnd = OpenWindowTags( NULL,
								WA_Left,0,
								WA_Top,offy,
								WA_Width, wndwidth,
								WA_Height, wndheight,
								WA_IDCMP, idcmp,
								WA_DragBar,TRUE,
								WA_SizeGadget, TRUE,
								WA_DepthGadget,TRUE,
								WA_CloseGadget, TRUE,
								WA_Title,locText[4],
								WA_RMBTrap, TRUE,
								WA_Gadgets, glist,
								WA_SizeBBottom,TRUE,
								WA_MaxWidth,-1,
								WA_Activate, TRUE,
								WA_PubScreen,scr,
								WA_NoCareRefresh,TRUE,
//								WA_MaxHeight,-1,
								TAG_DONE );
		if( wnd )
		{
			glistAttached = TRUE;
			if( WorkbenchBase )
			{
				if((appMsgPort = CreateMsgPort()))
				{
					appWnd = AddAppWindowA(1,NULL,wnd,appMsgPort,NULL);
				}
			}

			SetFont(wnd->RPort,drinfo->dri_Font);
			Render();
			return TRUE;
		}
		glistAttached = FALSE;
	}
	FreeGUI();
	return FALSE;
}
//-------------------------------------
STATIC BOOL HandleWnd(void)
{
	struct IntuiMessage *imsg;
	BOOL retVal = FALSE;

	while((imsg = GT_GetIMsg(wnd->UserPort)))
	{
		ULONG cl = imsg->Class;
		UWORD code = imsg->Code;
		APTR iaddress = imsg->IAddress;

		if( cl == IDCMP_SIZEVERIFY )
		{
			if(glistAttached) RemoveGList( wnd, glist, -1 );
			glistAttached = FALSE;
			GT_ReplyIMsg(imsg);
			continue;
		}

		GT_ReplyIMsg(imsg);

		switch( cl )
		{
			case	IDCMP_CLOSEWINDOW:
						retVal = TRUE;
						if( commandStr ) FreeVec(commandStr);
						commandStr = NULL;
						break;

			case	IDCMP_VANILLAKEY:
						if( code == 10 || code == ok_key) retVal = TRUE;
						else if( code == 27 || code == cancel_key)
						{
							retVal = TRUE;
							if( commandStr ) FreeVec(commandStr);
							commandStr = NULL;
						}
						else if( code == command_key ) ActivateGadget(commandString,wnd,NULL);
						else if( code == stack_key ) ActivateGadget(stackInteger,wnd,NULL);
						else if( code == commandbut_key ) CommandButton();
						break;

			case	IDCMP_NEWSIZE:
						if( !sizeVerify && glistAttached)
						{
							if( wndwidth == wnd->Width || wndheight != wnd->Height ) break;
						}
						if(glistAttached) RemoveGList( wnd, glist, -1 );
						if( wnd->Width < wndwidth ) RefreshWindowFrame(wnd);
						wndwidth = wnd->Width;
						wndheight = wnd->Height;
						EraseRect( wnd->RPort, wnd->BorderLeft, wnd->BorderTop+fonty+6,
																		wnd->Width-wnd->BorderRight-1, wnd->Height-wnd->BorderBottom-1 );
						if(CreateGadgets())
						{
							AddGList( wnd, glist, -1,-1, NULL );
							RefreshGList( glist, wnd, NULL, -1 );
							glistAttached = TRUE;
							ActivateGadget( commandString, wnd, NULL);
						}	else 
						{
							glistAttached = FALSE;
							retVal = TRUE;
						}
						break;

			case	IDCMP_GADGETUP:
						switch( ((struct Gadget*)iaddress)->GadgetID )
						{
							case	WND_COMMAND_STRING:
										retVal = TRUE;
										GetCommand();
										break;

							case	WND_COMMAND_BUTTON:
										CommandButton();
										break;

							case	WND_CANCEL_BUTTON:
										retVal = TRUE;
										if( commandStr ) FreeVec(commandStr);
										commandStr = NULL;
										break;

							case	WND_OK_BUTTON:
										retVal = TRUE;
										GetCommand();
										break;

							case	WND_STACK_INTEGER:
										GetStack();
										break;

							default:
										break;
						}
						break;
		}
	}
	return retVal;
}
//-------------------------------------
STATIC VOID HandleApp(void)
{
	struct AppMessage *amsg;
	BOOL commandSet = FALSE;
	while(( amsg = (struct AppMessage*)GetMsg(appMsgPort)))
	{
		if( amsg->am_Type == AMTYPE_APPWINDOW )
		{
			STRPTR str = WBArg2String(amsg->am_ArgList, amsg->am_NumArgs ); 
			if( str )
			{
				if(commandStr) FreeVec(commandStr);
				commandStr = str;
				SetCommand();
				((struct StringInfo*)(commandString->SpecialInfo))->BufferPos = strlen(str);
				commandSet = TRUE;
			}
		}
		ReplyMsg((struct Message*)amsg);
	}
	if(commandSet)
	{
		ActivateWindow(wnd);
		ActivateGadget(commandString,wnd,NULL);
	}
}
//-------------------------------------

//-------------------------------------
static void ecmain(void)
{
	BPTR lock;
	WorkbenchBase = OpenLibrary("workbench.library",37);
	if(LocaleBase)Cat = OpenCatalogA( NULL, "Scalos/Scalos_ECommand.catalog",NULL);

	lock = Lock("ENV:Scalos/NoSizeVerify",ACCESS_READ);
	if(lock)
	{
		sizeVerify = FALSE;
		UnLock(lock);
//		lock = NULL;
	}	else sizeVerify = TRUE;

	InitStrings();

	if(SetupScreen())
	{
		stackNum = 4096;
		if(!commandStr)
		{
			STRPTR buffer = (STRPTR)AllocVec(516,0);
			if(buffer)
			{
				if(GetVar("Scalos_last_cmd",buffer,512,GVF_GLOBAL_ONLY)!=-1)
				{
					commandStr = StrCopy(buffer);
				}
				FreeVec(buffer);
			}
		}

		if( SetupGUI())
		{
			BOOL ready = FALSE;
			ULONG wndsig = 1UL<<wnd->UserPort->mp_SigBit;
			ULONG appsig;

			if( appMsgPort ) appsig = 1UL<<appMsgPort->mp_SigBit;
			else appsig = 0UL;

			ActivateGadget(commandString,wnd,NULL);

			while( ready == FALSE )
			{
				ULONG signal = Wait(wndsig|appsig|4096);

				if( signal & 4096 ) ready = TRUE;
				if( signal & wndsig ) ready = HandleWnd();
				if( signal & appsig ) HandleApp();
			}
			FreeGUI();

			if( commandStr )
			{
				STRPTR conName = BuildConName(locText[7],scr);
				if( conName )
				{
					SystemTags( commandStr,
							SYS_Asynch, TRUE,
							SYS_Output, Open(conName,MODE_OLDFILE),
							SYS_Input, NULL,
							SYS_UserShell, TRUE,
							stackNum>=4096?NP_StackSize:TAG_IGNORE,stackNum,
							TAG_DONE);

					FreeVec(conName);
				}
				SetVar("Scalos_last_cmd",commandStr,strlen(commandStr),GVF_GLOBAL_ONLY);
				FreeVec(commandStr);
				commandStr = NULL;
			}
		}
		FreeScreen();
	}

	if(Cat) CloseCatalog(Cat);
	if(WorkbenchBase)CloseLibrary(WorkbenchBase);
}
//-------------------------------------
main()
{
	wbmain(WBenchMsg);
}
//-------------------------------------
static void wbmain(struct WBStartup *wbs )
{
	BPTR olddir;
	BPTR lock;

	if( wbs->sm_NumArgs > 1 )
	{
		lock = NULL;
		commandStr = StrCopy(wbs->sm_ArgList[1].wa_Name);
		olddir = CurrentDir(wbs->sm_ArgList[1].wa_Lock);
	}	else
	{
		lock = Lock("RAM:",ACCESS_READ);
		olddir = CurrentDir(lock);
	}
	ecmain();
	CurrentDir(olddir);

	if(lock) UnLock(lock);
}
//-------------------------------------
