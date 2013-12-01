#include <string.h>
#include <dos.h>

#include <dos/dostags.h>
#include <intuition/imageclass.h>
#include <intuition/sghooks.h>
#include <workbench/startup.h>

#include <scalos/scalos.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/locale_protos.h>
#include <clib/scalos_protos.h>
#include <clib/utility_protos.h>

#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/locale_pragmas.h>
#include <pragmas/scalos_pragmas.h>
#include <pragmas/utility_pragmas.h>

__stdargs void NewList( struct List *list );
static void renmain(void);
static void wbmain(struct WBStartup *wbs );
STATIC VOID Render(void);

IMPORT struct Library *GadToolsBase;
IMPORT struct Library *IntuitionBase;
IMPORT struct Library *SysBase;
IMPORT struct Library *GfxBase;
IMPORT struct Library *DOSBase;
IMPORT struct Library *LocaleBase;
IMPORT struct Library *UtilityBase;

struct Library *IconBase;
struct Library *ScalosBase;

struct Catalog *Cat;

struct Screen *scr;
struct DrawInfo *drinfo;
BOOL one2two;
APTR visualinfo;
ULONG offx,offy,fonty,sizeheight,uwidth;
ULONG wndwidth,wndheight;

struct Hook editHook;
struct Window *wnd;
struct Gadget *glist;
struct Gadget *nameString;
//struct Gadget *skipButton;

STRPTR nameStr;
STRPTR orgNameStr;

STRPTR parentStr;
BPTR parent;

STRPTR topStr;
STRPTR top2Str;

struct WBArg *wbarg;
ULONG wbarg_act,wbarg_num;
BOOL relabel;

BOOL glistAttached;
BOOL sizeVerify;
BOOL fromWB;

const STATIC STRPTR engLocText[] =
{
	"Enter a new Name for",
	"New _Name:",
	"_OK",
	"_Cancel",
	"_Skip",
	"Rename",
	"Path",
	NULL,
	"Cannot rename because"
};

UWORD ok_key,cancel_key,skip_key,name_key;

STRPTR locText[9];

const STRPTR VerString = "$VER: rename.module 1.0 "__AMIGADATE__;

#define WND_NAME_STRING 1
#define WND_OK_BUTTON 2
#define WND_SKIP_BUTTON 3
#define WND_CANCEL_BUTTON 4

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
__asm __saveds ULONG StringFunc( register __a0 struct Hook stringHook, register __a1 ULONG *Msg, register __a2 struct SGWork *SGW )
{
//	struct StringInfo *sinfo = SGW->StringInfo;
	if( *Msg != SGH_KEY ) return 0;

	SGW->Actions |= SGA_USE;

	switch( SGW->EditOp )
	{
		case	EO_INSERTCHAR:
					{
						switch(SGW->Code)
						{
							case	'/':
							case	':':
										SGW->Actions |= SGA_BEEP;
										SGW->Actions ^= SGA_USE;
										break;
						}
					}
					break;
	}
	return 1L;
}
//-------------------------------------

//-------------------------------------
STATIC ULONG CalcWidth(void)
{
	ULONG wndwidth;
	ULONG newwndwidth = GetTextLength(locText[3])+GetTextLength(locText[4])+GetTextLength(locText[5])+2*offx+50;
	wndwidth = GetTextLength(topStr)+2*offx+8;
	if(newwndwidth>wndwidth)wndwidth=newwndwidth;

	newwndwidth = GetTextLength(top2Str)+2*offx+8;
	if(newwndwidth>wndwidth)wndwidth=newwndwidth;

	if(wndwidth > scr->Width) wndwidth=scr->Width;
	return wndwidth;
}
//-------------------------------------
STATIC VOID RenameTheFile(void)
{
	if(nameStr)
	{
		if( relabel )
		{
			Relabel(parentStr,nameStr);
		}	else
		{
			ULONG len1 = strlen(orgNameStr);
			ULONG len2 = strlen(nameStr);
			STRPTR infoname1 = AllocVec(len1+8,0);
			if(infoname1)
			{
				STRPTR infoname2 = AllocVec(len2+8,0);
				if(infoname2)
				{
					BPTR olddir = CurrentDir(parent);
					struct ScaUpdateIcon_IW updateIcon;

					strcpy(infoname1,orgNameStr);
					strcpy(&infoname1[len1],".info");
					strcpy(infoname2,nameStr);
					strcpy(&infoname2[len2],".info");
	
					Rename(infoname1,infoname2);
					Rename(orgNameStr,nameStr);
					CurrentDir(olddir);
					FreeVec(infoname2);

					updateIcon.ui_iw_Lock = parent;
					updateIcon.ui_iw_Name = orgNameStr;
					SCA_UpdateIcon( WSV_Type_IconWindow, &updateIcon, sizeof(struct ScaUpdateIcon_IW));

					updateIcon.ui_iw_Name = nameStr;
					SCA_UpdateIcon( WSV_Type_IconWindow, &updateIcon, sizeof(struct ScaUpdateIcon_IW));
				}
				FreeVec(infoname1);
			}
		}
	}
}
//-------------------------------------
STATIC VOID CleanRename(void)
{
	if(parent)
	{
		UnLock(parent);
		parent = NULL;
	}
	
	if(nameStr)
	{
		FreeVec(nameStr);
		nameStr = NULL;
	}

	if(orgNameStr)
	{
		FreeVec(orgNameStr);
		orgNameStr = NULL;
	}

	if(parentStr)
	{
		FreeVec(parentStr);
		parentStr = NULL;
	}

	if( topStr )
	{
		FreeVec(topStr);
		topStr=NULL;
	}

	if( top2Str )
	{
		FreeVec(top2Str);
		top2Str=NULL;
	}
}
//-------------------------------------
STATIC BOOL PrepareRename(void)
{
	BOOL retval = FALSE;
	if(wbarg_act < wbarg_num )
	{
		CleanRename();
		relabel = FALSE;
		if((parentStr = NameOfLock(wbarg[wbarg_act].wa_Lock)))
		{
			BOOL drawer;
			if(wbarg[wbarg_act].wa_Name)
			{
				if(*wbarg[wbarg_act].wa_Name) drawer = FALSE;
				else drawer=TRUE;
			}	else drawer=TRUE;

			if( drawer )
			{
				if((parent = ParentDir(wbarg[wbarg_act].wa_Lock)))
				{
					STRPTR drwName = FilePart(parentStr);
					STRPTR parentName = PathPart(parentStr);
					if((nameStr = StrCopy(drwName)))
					{
						if((orgNameStr = StrCopy(drwName)))
						{
							*parentName = 0;
							wbarg_act++;
							retval = TRUE;
						}
					}
				} else
				{
					LONG len=strlen(parentStr);
					if(len)
					{
						if(parentStr[len-1] == ':')
						{
							relabel = TRUE;
							parent=NULL;
							nameStr=StrCopy(parentStr);
							nameStr[len-1]=0;
							orgNameStr=StrCopy(nameStr);
							wbarg_act++;
							retval = TRUE;
						}
					}
				}
			}	else
			{
				if((nameStr = StrCopy(wbarg[wbarg_act].wa_Name)))
				{
					if((orgNameStr = StrCopy(nameStr)))
					{
						if((parent = DupLock(wbarg[wbarg_act].wa_Lock)))
						{
							wbarg_act++;
							retval = TRUE;
						}
					}
				}
			}
		}
	}

	if( retval && parentStr )
	{
		topStr = (STRPTR)AllocVec( strlen(nameStr)+strlen(locText[0])+8,0);
		if(topStr)
		{
			sprintf(topStr,"%s '%s'",locText[0],nameStr);
		}	else retval = FALSE;
		top2Str = (STRPTR)AllocVec( strlen(parentStr)+strlen(locText[6])+8,0);
		if(top2Str)
		{
			sprintf(top2Str,"%s '%s'",locText[6],parentStr);

			if( wnd )
			{
				ULONG newwidth = CalcWidth();
				if(newwidth>wnd->Width)
				{
					ChangeWindowBox(wnd,wnd->LeftEdge,wnd->TopEdge,newwidth,wnd->Height);
					Delay(6);
					WindowLimits(wnd,newwidth,0,~0,wnd->Height);
					wndwidth=wnd->Width;
				}	else
				{
					if( nameString )
					{
						GT_SetGadgetAttrs(nameString,wnd,NULL,
																GTST_String,nameStr,
																TAG_DONE);

						((struct StringInfo*)(nameString->SpecialInfo))->BufferPos = strlen(nameStr);

						ActivateGadget(nameString,wnd,NULL);
					}
				}
				Render();
			}

		}	else retval = FALSE;
	}

	return retval;
}
//-------------------------------------

//-------------------------------------
STATIC VOID InitStrings(void)
{
	ULONG i;
	for(i=0; i<9; i++)
	{
		locText[i] = GetCatalogStr( Cat, i, engLocText[i]);
	}

	name_key = FindUnderscoredToLower(locText[1]);
	ok_key = FindUnderscoredToLower(locText[2]);
	cancel_key = FindUnderscoredToLower(locText[3]);
	skip_key = FindUnderscoredToLower(locText[4]);
}
//-------------------------------------

//-------------------------------------
STATIC VOID GetName(void)
{
	STRPTR nmeStr;

	if(GT_GetGadgetAttrs(nameString,wnd,NULL,
		GTST_String, &nmeStr,
		TAG_DONE ))
	{
		if( nameStr ) FreeVec(nameStr);
		nameStr=StrCopy(nmeStr);
	}
}
//-------------------------------------
//STATIC void SetName(void)
//{
//	  GT_SetGadgetAttrs(nameString,wnd,NULL,
//		  GTST_String, nameStr?nameStr:(STRPTR)"",
//		  TAG_DONE );
//}
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
					if(!GetAttr( IA_Height,o,&sizeheight)) sizeheight = 11;
					DisposeObject(o);
				}	else sizeheight = 11;
				uwidth = TextLength(&scr->RastPort,"_",1);

				x = drinfo->dri_Resolution.X;
				y = drinfo->dri_Resolution.Y;
				if( y/x >= 2 )
				{
					one2two = TRUE;
				}	else one2two = FALSE;

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
	UWORD x1,x2;
	Move(wnd->RPort,offx+4,offy+drinfo->dri_Font->tf_Baseline+4);
	SetABPenDrMd(wnd->RPort,drinfo->dri_Pens[TEXTPEN],drinfo->dri_Pens[BACKGROUNDPEN],JAM2);
	Text(wnd->RPort,topStr,strlen(topStr));

	x1=wnd->RPort->cp_x;
	x2=wnd->Width-wnd->BorderRight-1;
	if( x1 <= x2 )
	{
		EraseRect(wnd->RPort,x1,offy+4,x2,offy+4+fonty);
	}

	Move(wnd->RPort,offx+4,offy+drinfo->dri_Font->tf_Baseline+8+fonty);

	x1=wnd->RPort->cp_x;
	if( x1 <= x2 )
	{
		EraseRect(wnd->RPort,x1,offy+8+fonty,x2,offy+8+2*fonty);
	}

	Text(wnd->RPort,top2Str,strlen(top2Str));

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
		ULONG usableWidth = wndwidth - 2*offx-8;
		ULONG cancelLeft;
		BOOL setCursor;

		ng.ng_VisualInfo = visualinfo;
		ng.ng_TextAttr = NULL;
		ng.ng_GadgetText = locText[1];
		ng.ng_GadgetID = WND_NAME_STRING;
		ng.ng_Flags = NULL;
		ng.ng_UserData = NULL;
		ng.ng_LeftEdge = offx+13+TextLength(&scr->RastPort,ng.ng_GadgetText,strlen(ng.ng_GadgetText))-uwidth;
		ng.ng_TopEdge = offy+2*fonty+10;
		ng.ng_Width = wndwidth-ng.ng_LeftEdge-offx-4;
		ng.ng_Height = fonty+6;

		if( !nameString && nameStr ) setCursor = TRUE;
		else setCursor = FALSE;

		g = nameString = CreateGadget( STRING_KIND, g, &ng,
														GTST_String, nameStr?nameStr:(STRPTR)"",
														GTST_EditHook, &editHook,
														GT_Underscore,'_',
														TAG_DONE);

		if( setCursor )
			((struct StringInfo*)(nameString->SpecialInfo))->BufferPos = strlen(nameStr);

		ng.ng_TopEdge += ng.ng_Height+4;
		ng.ng_Width = usableWidth/3-2;
		ng.ng_GadgetText = locText[2];
		ng.ng_LeftEdge = offx+4;
		ng.ng_GadgetID = WND_OK_BUTTON;

		g = CreateGadget( BUTTON_KIND, g, &ng,
														GT_Underscore,'_',
														TAG_DONE);

		cancelLeft = wndwidth - ng.ng_Width-4-offx;

		if(wbarg_num>1)
		{
			ng.ng_GadgetText = locText[4];
			ng.ng_LeftEdge += ng.ng_Width+2;
			ng.ng_Width = cancelLeft - 2 - ng.ng_LeftEdge;
			ng.ng_GadgetID = WND_SKIP_BUTTON;

			g = CreateGadget( BUTTON_KIND, g, &ng,
														GT_Underscore,'_',
														TAG_DONE);
		}

		ng.ng_GadgetText = locText[3];
		ng.ng_LeftEdge = cancelLeft;
		ng.ng_GadgetID = WND_CANCEL_BUTTON;
		ng.ng_Width = usableWidth/3-2;

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
}
//-------------------------------------
STATIC BOOL SetupGUI(void)
{
//	WORD cbw = (fonty+6)*13*(one2two+1)/11;

	fonty = drinfo->dri_Font->tf_YSize;
	offx = scr->WBorLeft;
	offy = scr->WBorTop + fonty + 1;

	wndwidth = CalcWidth();
	wndheight = offy + 4*fonty + sizeheight + 29;

	editHook.h_Entry = (HOOKFUNC)StringFunc;

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
								WA_Title,locText[5],
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
						if( nameStr ) FreeVec(nameStr);
						nameStr = NULL;
						break;

			case	IDCMP_VANILLAKEY:
						if( code == 10 || code == ok_key )
						{
							GetName();
							RenameTheFile();
							if(!PrepareRename()) retVal = TRUE;
						}
						else if( code == 27 || code == cancel_key)
						{
							if( nameStr ) FreeVec(nameStr);
							nameStr = NULL;
							retVal = TRUE;
						}
						else if( code == skip_key )
						{
							if(!PrepareRename()) retVal = TRUE;
						}
						else if( code == name_key ) ActivateGadget(nameString,wnd,NULL);
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
						EraseRect( wnd->RPort, wnd->BorderLeft, wnd->BorderTop+2*fonty+9,
																		wnd->Width-wnd->BorderRight-1, wnd->Height-wnd->BorderBottom-1 );
						if(CreateGadgets())
						{
							AddGList( wnd, glist, -1,-1, NULL );
							RefreshGList( glist, wnd, NULL, -1 );
							glistAttached = TRUE;
							ActivateGadget( nameString, wnd, NULL);
						}	else 
						{
							glistAttached = FALSE;
							retVal = TRUE;
						}
						break;

			case	IDCMP_GADGETUP:
						switch( ((struct Gadget*)iaddress)->GadgetID )
						{
							case	WND_NAME_STRING:
										GetName();
										RenameTheFile();
										if(!PrepareRename()) retVal = TRUE;
										break;

							case	WND_CANCEL_BUTTON:
										retVal = TRUE;
										if( nameStr ) FreeVec(nameStr);
										nameStr = NULL;
										break;

							case	WND_SKIP_BUTTON:
										if(!PrepareRename()) retVal = TRUE;
										break;

							case	WND_OK_BUTTON:
										GetName();
										RenameTheFile();
										if(!PrepareRename()) retVal = TRUE;
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
static void renmain(void)
{
	if(LocaleBase) Cat = OpenCatalogA( NULL, "Scalos/Scalos_Rename.catalog",NULL);

	if((IconBase = OpenLibrary("icon.library",37)))
	{
		BPTR lock = Lock("ENV:Scalos/NoSizeVerify",ACCESS_READ);
		ScalosBase = OpenLibrary("scalos.library",0);
		if(lock)
		{
			sizeVerify = FALSE;
			UnLock(lock);
		}	else sizeVerify = TRUE;

		InitStrings();

		if(PrepareRename())
		{
			if(SetupScreen())
			{
				if( SetupGUI())
				{
					BOOL ready = FALSE;
					ULONG wndsig = 1UL<<wnd->UserPort->mp_SigBit;
		
					ActivateGadget(nameString,wnd,NULL);
	
					while( ready == FALSE )
					{
						ULONG signal = Wait(wndsig|4096);
		
						if( signal & 4096 ) ready = TRUE;
						if( signal & wndsig ) ready = HandleWnd();
					}
					FreeGUI();
				}
				FreeScreen();
			}
		}
		if(ScalosBase) CloseLibrary(ScalosBase);
		CloseLibrary(IconBase);
	}
	if(Cat) CloseCatalog(Cat);
}
//-------------------------------------
main()
{
	wbmain(WBenchMsg);
}
//-------------------------------------
static void wbmain(struct WBStartup *wbs )
{
	if( wbs->sm_NumArgs > 1 )
	{
		wbarg_num = wbs->sm_NumArgs-1;
		wbarg = wbs->sm_ArgList+1;

		renmain();

		CleanRename();
	}
}
//-------------------------------------
