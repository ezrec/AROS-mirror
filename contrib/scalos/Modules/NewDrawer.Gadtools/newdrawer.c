// newdrawer.c
// $Date$
// $Revision$


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

#include <dos/dostags.h>
#include <intuition/imageclass.h>
#include <datatypes/iconobject.h>
#include <workbench/startup.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/iconobject.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/locale.h>
#include <proto/utility.h>

#include <defs.h>
#include <Year.h> // +jmc+


#define	d1(x)		;
#define	d2(x)		x;

#ifndef TOGGLE_BOOL
#define TOGGLE_BOOL( x ) ( (x) ? FALSE : TRUE )
#endif

// from debug.lib
extern int kprintf(const char *fmt, ...);

__stdargs void NewList( struct List *list );
static void ndmain(void);
static void wbmain(struct WBStartup *wbs );

IMPORT T_LOCALEBASE LocaleBase;
IMPORT T_UTILITYBASE UtilityBase;
struct Library *IconobjectBase = NULL;

struct Catalog *Cat;

struct Screen *scr;
struct DrawInfo *drinfo;
BOOL one2two;
APTR visualinfo;
ULONG offx,offy,fonty,sizeheight,uwidth;
ULONG wndwidth,wndheight;

struct Window *wnd;
struct Gadget *glist;
struct Gadget *drawerString;
struct Gadget *iconCheckbox;

STRPTR drawerStr;
ULONG iconChecked;

STRPTR parentStr;
STRPTR topStr;

BOOL glistAttached;
BOOL sizeVerify;
BOOL fromWB;

const STATIC STRPTR engLocText[] =
{
	"Enter Name for a a new Drawer in:",
	"New _Drawer:",
	"_OK",
	"_Cancel",
	"Add a new Drawer",
	"Unnamed",
	"Path:",
	"Create _Icon",
	"Can't create drawer"
};

UWORD ok_key, cancel_key, drawer_key, icon_key;

STRPTR locText[9];

const STRPTR VerString = "$VER: newdrawer.module 1.1 "__AMIGADATE__;

#define WND_DRAWER_STRING 1
#define WND_ICON_CHECKBOX 2
#define WND_OK_BUTTON 3
#define WND_CANCEL_BUTTON 4

//-------------------------------------
ULONG StrLen( const STRPTR str )
{
	if( !str)
		 return 0;
	return strlen(str);
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
		if((n = (STRPTR) calloc(bufSize, 1)))
		{
			if( NameFromLock( lock, n, bufSize-1 ) == DOSFALSE )
			{
				if( IoErr() == ERROR_LINE_TOO_LONG )
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
STRPTR GetFullPath( const STRPTR drw, const STRPTR file )
{
	ULONG length = StrLen(drw)+StrLen(file)+4;
	STRPTR fp = (STRPTR) malloc( length+1 );
	if( fp )
	{
		strcpy( fp, drw );

		if( AddPart( fp, file, length ))	return fp;
		free( fp );
	}
	return NULL;
}
//-------------------------------------
ULONG GetTextLength( const STRPTR str )
{
	return TextLength(&scr->RastPort,str,strlen(str));
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
BOOL ToggleCheckbox( struct Window *wnd, struct Gadget *g, BOOL oldState )
{
	BOOL newState = TOGGLE_BOOL(oldState);
	GT_SetGadgetAttrs( g, wnd, NULL,
			GTCB_Checked, newState,
			TAG_DONE );
	return newState;
}
//-------------------------------------

//-------------------------------------
STATIC VOID InitStrings(void)
{
	ULONG i;
	for(i=0; i<9; i++)
	{
		locText[i] = GetCatalogStr( Cat, i, engLocText[i]);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: locText[%ld]=<%s>\n", __LINE__, i, locText[i]));
	}

	drawer_key = FindUnderscoredToLower(locText[1]);
	ok_key = FindUnderscoredToLower(locText[2]);
	cancel_key = FindUnderscoredToLower(locText[3]);
	icon_key = FindUnderscoredToLower(locText[7]);
}
//-------------------------------------

//-------------------------------------
STATIC VOID GetDefaultDrawer(void)
{
	ULONG num = 1;
	STRPTR buf;	
	drawerStr = (STRPTR) calloc(strlen(locText[5])+20, 1);
	if(drawerStr)
	{
		strcpy(drawerStr,locText[5]);
		buf = drawerStr+strlen(locText[5]);
		while(num<1000000)
		{
			BPTR lock;
			sprintf(buf,"%ld",num);
			if(( lock = Lock(drawerStr,ACCESS_READ)))
			{
				UnLock(lock);
			}	else return;
			num++;
		}
		free(drawerStr);
		drawerStr=NULL;
	}
}
//-------------------------------------
STATIC VOID GetDrawer(void)
{
	STRPTR drwStr;

	if(GT_GetGadgetAttrs(drawerString,wnd,NULL,
			GTST_String, &drwStr,
			TAG_DONE ))
	{
		if( drawerStr ) 
			free(drawerStr);
		drawerStr=strdup(drwStr);
	}
}
//-------------------------------------
STATIC VOID GetIcon(void)
{
	ULONG ichk=0;
	if(GT_GetGadgetAttrs(iconCheckbox,wnd,NULL,
			GTCB_Checked, &ichk,
			TAG_DONE ))
	{
		iconChecked=ichk;
	}
}
//-------------------------------------
STATIC BOOL SetupScreen(void)
{
	if((scr = LockPubScreen(NULL)))
	{
		if((drinfo = GetScreenDrawInfo(scr)))
		{
			if((visualinfo = GetVisualInfoA(scr,NULL)))
			{
				UWORD x,y;
				Object *o = (Object*)NewObject( NULL, SYSICLASS,
					SYSIA_DrawInfo,drinfo,
					SYSIA_Which, SIZEIMAGE,
					TAG_DONE);
				if(o)
				{
					if(!GetAttr( IA_Height,o,&sizeheight)) 
						sizeheight = 11;
					DisposeObject(o);
				}
				else 
					sizeheight = 11;
				uwidth = TextLength(&scr->RastPort,"_",1);

				x = drinfo->dri_Resolution.X;
				y = drinfo->dri_Resolution.Y;
				if( y/x >= 2 )
				{
					one2two = TRUE;
				}	else one2two = FALSE;

				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: parentStr=%08lx\n", __LINE__, parentStr));
				if(parentStr)
				{
					topStr = (STRPTR) malloc( strlen(parentStr)+strlen(locText[0])+8 );
					if(topStr)
					{
						sprintf(topStr,"%s \'%s\'",locText[0],parentStr);
						d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: locText[0]=<%s>\n", __LINE__, locText[0]));
						d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: parentStr=<%s>\n", __LINE__, parentStr));
						d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: topStr=<%s>\n", __LINE__, topStr));
					}
				}

				return TRUE;
			}
		}
	}
	return FALSE;
}
//-------------------------------------
STATIC VOID FreeScreen(void)
{
	if( visualinfo ) 
		FreeVisualInfo(visualinfo);
	if( drinfo ) 
		FreeScreenDrawInfo(scr,drinfo);
	if( scr ) 
		UnlockPubScreen(NULL,scr);
}
//-------------------------------------
STATIC VOID Render(void)
{
	Move(wnd->RPort,offx+4,offy+drinfo->dri_Font->tf_Baseline+4);
	SetABPenDrMd(wnd->RPort,drinfo->dri_Pens[TEXTPEN],0,JAM1);

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: topStr=%08lx\n", __LINE__, topStr));

	if( topStr ) 
		Text(wnd->RPort,topStr,strlen(topStr));
	else
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
		WORD createIconWidth = TextLength(&scr->RastPort,locText[7],strlen(locText[7]))-uwidth;
		WORD usableWidth = wndwidth - 2*offx-8;
		WORD cbw = (fonty+6)*13*(one2two+1)/11;
		WORD createIconLeft = wndwidth-offx-4-cbw-createIconWidth-8;
		BOOL setCursor;

		ng.ng_VisualInfo = visualinfo;
		ng.ng_TextAttr = NULL;
		ng.ng_GadgetText = locText[1];
		ng.ng_GadgetID = WND_DRAWER_STRING;
		ng.ng_Flags = NULL;
		ng.ng_UserData = NULL;
		ng.ng_LeftEdge = offx+13+TextLength(&scr->RastPort,ng.ng_GadgetText,strlen(ng.ng_GadgetText))-uwidth;
		ng.ng_TopEdge = offy+fonty+7;
		ng.ng_Width = createIconLeft - ng.ng_LeftEdge-6;
		ng.ng_Height = fonty+6;

		if( !drawerString && drawerStr ) setCursor = TRUE;
		else setCursor = FALSE;

		g = drawerString = CreateGadget( STRING_KIND, g, &ng,
					GTST_String, drawerStr?drawerStr:(STRPTR)"",
					GT_Underscore,'_',
					TAG_DONE);

		if( setCursor )
			((struct StringInfo*)(drawerString->SpecialInfo))->BufferPos = strlen(drawerStr);


		ng.ng_LeftEdge = wndwidth - cbw-4-offx;
		ng.ng_Width = cbw;
		ng.ng_GadgetID = WND_ICON_CHECKBOX;
		ng.ng_GadgetText = locText[7];

		g = iconCheckbox = CreateGadget( CHECKBOX_KIND, g, &ng,
					GTCB_Scaled, TRUE,
					GTCB_Checked, iconChecked,
					GT_Underscore,'_',
					TAG_DONE );

		ng.ng_TopEdge += ng.ng_Height+4;
		ng.ng_Width = usableWidth/2-3;
		ng.ng_GadgetText = locText[2];
		ng.ng_LeftEdge = offx+4;
		ng.ng_GadgetID = WND_OK_BUTTON;

		g = CreateGadget( BUTTON_KIND, g, &ng,
					GT_Underscore,'_',
					TAG_DONE);

		ng.ng_GadgetText = locText[3];
		ng.ng_LeftEdge = wndwidth - ng.ng_Width-4-offx;
		ng.ng_GadgetID = WND_CANCEL_BUTTON;

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
	if(wnd) CloseWindow(wnd);
	if(glist) FreeGadgets(glist);
	if(topStr)
	{
		free(topStr);
		topStr = NULL;
	}
}
//-------------------------------------
STATIC BOOL SetupGUI(void)
{
	WORD cbw;// = (fonty+6)*13*(one2two+1)/11;
	ULONG newwndwidth;// = GetTextLength(locText[1])+GetTextLength(locText[7])+cbw+GetTextLength(locText[5])+2*offx+46;

	fonty = drinfo->dri_Font->tf_YSize;
	offx = scr->WBorLeft;
	offy = scr->WBorTop + fonty + 1;

	cbw = (fonty+6)*13*(one2two+1)/11;
	newwndwidth = GetTextLength(locText[1])+GetTextLength(locText[7])+cbw+GetTextLength(locText[5])+2*offx+46;

	wndwidth = GetTextLength(topStr);
	if(newwndwidth>wndwidth)wndwidth=newwndwidth;
	if(wndwidth > scr->Width) wndwidth=scr->Width;

	wndheight = offy + 3*fonty + sizeheight + 26;

	GetDefaultDrawer();

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
//			WA_MaxHeight,-1,
			TAG_DONE );
		if( wnd )
		{
			glistAttached = TRUE;
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
			if( drawerStr ) 
				free(drawerStr);
			drawerStr = NULL;
			break;

		case	IDCMP_VANILLAKEY:
			if( code == 10 || code == ok_key) retVal = TRUE;
			else if( code == 27 || code == cancel_key )
			{
				if( drawerStr ) 
					free(drawerStr);
				drawerStr = NULL;
				retVal = TRUE;
			}
			else if( code == drawer_key ) ActivateGadget(drawerString,wnd,NULL);
			else if( code == icon_key ) iconChecked = ToggleCheckbox(wnd,iconCheckbox,iconChecked);
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
				ActivateGadget( drawerString, wnd, NULL);
			}	else 
			{
				glistAttached = FALSE;
				retVal = TRUE;
			}
			break;

		case	IDCMP_GADGETUP:
			switch( ((struct Gadget*)iaddress)->GadgetID )
			{
			case	WND_DRAWER_STRING:
				retVal = TRUE;
				GetDrawer();
				break;

			case	WND_ICON_CHECKBOX:
				GetIcon();
				break;

			case	WND_CANCEL_BUTTON:
				retVal = TRUE;
				if( drawerStr ) 
					free(drawerStr);
				drawerStr = NULL;
				break;

			case	WND_OK_BUTTON:
				retVal = TRUE;
				GetDrawer();
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

//-------------------------------------
static void ndmain(void)
{
	if(LocaleBase) Cat = OpenCatalogA( NULL, "Scalos/Scalos_NewDrawer.catalog",NULL);

	if ((IconobjectBase = OpenLibrary(ICONOBJECTNAME, 39)))
	{
		BPTR lock = Lock("ENV:Scalos/NoSizeVerify",ACCESS_READ);
		if(lock)
		{
			sizeVerify = FALSE;
			UnLock(lock);
		}	else sizeVerify = TRUE;

		InitStrings();

		iconChecked = TRUE;
	
		if(SetupScreen())
		{
			if( SetupGUI())
			{
				BOOL ready = FALSE;
				ULONG wndsig = 1UL<<wnd->UserPort->mp_SigBit;
	
				ActivateGadget(drawerString,wnd,NULL);
	
				while( ready == FALSE )
				{
					ULONG signal = Wait(wndsig|4096);
	
					if( signal & 4096 ) ready = TRUE;
					if( signal & wndsig ) ready = HandleWnd();
				}
				FreeGUI();
	
				if( drawerStr )
				{
					BPTR lock = CreateDir(drawerStr);
					if( lock )
					{
						if( iconChecked )
						{
							Object *IconObj;

							IconObj	= GetDefIconObject(WBDRAWER, NULL);
							if (IconObj)
								{
								PutIconObjectTags(IconObj, drawerStr,
									TAG_END);
								DisposeIconObject(IconObj);
								}
						}
						UnLock(lock);
					}
					free(drawerStr);
					drawerStr = NULL;
				}
			}
			FreeScreen();
		}
		CloseLibrary(IconobjectBase);
	}
	if(Cat) CloseCatalog(Cat);
}
//-------------------------------------
main()
{
	if (WBenchMsg)
		wbmain(WBenchMsg);
}
//-------------------------------------
static void wbmain(struct WBStartup *wbs )
{
//	BPTR olddir;
//	BPTR lock;

	if( wbs->sm_NumArgs > 1 )
	{
		if((parentStr = NameOfLock(wbs->sm_ArgList[1].wa_Lock)))
		{
			BPTR olddir = CurrentDir(wbs->sm_ArgList[1].wa_Lock);
			ndmain();
			free(parentStr);
			CurrentDir(olddir);
		}
/*		lock = NULL;
		commandStr = strdup(wbs->sm_ArgList[1].wa_Name);
		olddir = CurrentDir(wbs->sm_ArgList[1].wa_Lock);
	}	else
	{
		lock = Lock("RAM:",ACCESS_READ);
		olddir = CurrentDir(lock);*/
	}
//	main();
//	CurrentDir(olddir);

//	if(lock) UnLock(lock);
}
//-------------------------------------
