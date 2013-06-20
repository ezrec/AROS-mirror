/****************************/
/*			    */
/* Typeface v1.24	    */
/* © 1995-2000 David Kinder */
/*			    */
/****************************/

#include <stdlib.h>
#include "typeface.h"
#include "checkbox.h"

#ifdef __AROS__
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct UtilityBase *UtilityBase;
#else
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
#endif
struct Library *AslBase, *BGUIBase, *DiskfontBase, *TextFieldBase;
struct Library *IFFParseBase, *KeymapBase;
struct Screen *Screen;
struct Window *FontWnd, *OldWindowPtr;
struct DrawInfo *ScrDrawInfo;
struct List *CharWndList;
struct MsgPort *WndMsgPort;
struct Image *SizeImage, *LeftImage, *RightImage, *UpImage, *DownImage;
struct TextFont *NewFont, *ScrFont, *FixFont;
struct FontRequester *FontReq, *PrefsFontReq;
struct ScreenModeRequester *PrefsScrReq;
struct Hook *FontWndHook;
struct Character CharBuffer[257];
struct Character ClipChar;
struct IBox CharSize, FontSize;
struct NameInfo ModeIDName;
Object *FontWndObj, *CharObj, *ScrollObj;
Class *CharClass, *EditClass, *SlideClass, *LVClass;
WORD SpaceTable[257], KernTable[257];
IPTR  FirstChar, LastChar, ScaleX, ScaleY, UseTable, ScrWidth, ScrHeight;
char WinTitle[256], FontPath[256], SavePath[256], FontName[256];
char *KeyLabelTable;
BOOL CustomScreen = TRUE, ChangeScreen = FALSE, FlagError;
BOOL NewCustom, NewWindow, MaxWindow = FALSE, ConfirmQuit = FALSE;
BOOL ShowBLine = FALSE, DataChanged, AutoHeight = FALSE;
BOOL InvertShift = FALSE, Toggle = TRUE, QueryWidth = FALSE;

struct TFPreferences Prefs;
struct TextAttr ScrFontAttr, FixFontAttr;
char ScrFontAttrName[MAXFONTNAME], FixFontAttrName[MAXFONTNAME];
struct NewMenu Menus[] = { PROJECT_MENU,FONT_MENU,End };
struct NewMenu CharMenus[] = { PROJECT_MENU,FONT_MENU,CHARACTER_MENU,End };
struct IBox Box = { 0,0,0,0 };
char Version[] = "$VER: "NAME" "VERSION" (21.12.2000)";

struct Window *ParamWnd;
Object *ParamWndObj, *PropCycle, *HeightInt, *WidthInt, *BaseInt, *SmearInt;
Object *NormalCheck, *BoldCheck, *ItalicCheck, *UnderCheck;
Object *ExtCheck, *RevCheck, *AspectCycle, *BlankButton;
/* GetAttr wants IPTR */
IPTR  Proportional, Normal, Bold, Italic, ULine, Extended, Reversed;
/* GetAttr wants IPTR */
IPTR  Width, Aspect;
LONG  Height, Baseline, Smear;
CONST_STRPTR PropLabels[3], AspectLabels[4], SaveLabels[3];

struct Window *PrefsWnd;
Object *PrefsWndObj;
Object *CustButton, *ModeButton, *ModeInfo, *FontButton, *FontInfo, *PubStr;
Object *VecInt, *PixelXInt, *PixelYInt, *PixelCycle, *SizeCheck, *ChrHInt;
Object *SaveCycle, *ChrWCycle, *ChrFontInfo, *ChrFontButton, *MaxCheck;
Object *QuitCheck, *BLineCheck, *AutoCheck, *ShiftCheck, *ToggleCheck;
Object *QueryCheck;
Object *ToolBarOut, *ToolBarIn;
char PrefScrBuffer[256];
ULONG PrefScrModeID,PrefScrDepth;
char PrefScrFontName[MAXFONTNAME];
UWORD PrefScrFontHeight;
char PrefThisPubName[MAXPUBSCREENNAME], PrefDefPubName[MAXPUBSCREENNAME];
extern IPTR PixelBorder;
char PrefFixFontName[MAXFONTNAME];
UWORD PrefFixFontHeight;
char PrefFixBuffer[256];
CONST_STRPTR PrefsPages[6], BorderLabels[4];
struct Hook *TBDisplayHook, *TBResourceHook, *TBCompareHook;

struct Window *AssignWnd;
Object *AssignWndObj, *FontDirStr, *DirReqObj, *SaveReqObj;

struct Window *EditCharWnd;
Object *EditCharWndObj, *EditCharNum;

struct Window *WidthWnd;
struct Character *WidthChar;
struct CharNode *WidthNode;
Object *WidthWndObj, *ChrWidthScroll, *ChrWidthInt, *LeftInd, *RightInd;
char WidthTitle[256];

struct Window *KernWnd;
struct Character *KernChar;
struct CharNode *KernNode;
Object *KernWndObj, *SpaceObj, *KernObj;
char KernTitle[256];

struct Window *SaveWnd;
Object *SaveWndObj, *SaveDirStr, *FontNameStr, *SizeInfo;
Object *FirstNum, *LastNum, *TableCheck;

extern struct Window *ResetKernWnd;
extern Object *ResetKernWndObj, *ResetSpaceInt, *ResetKernInt;

extern struct Window *QueryWnd;
extern Object *QueryWndObj, *QueryWidthNum;

extern struct Window *PreviewWnd;
extern Object *PreviewWndObj;

int main()
{
  Setup();
  MsgLoop();
  return 0;
}

int wbmain()
{
  main();
  return 0;
}

void Setup(void)
{
struct Screen *defscr;
static struct TextAttr topaz8 = { "topaz.font",8,FS_NORMAL,FPF_ROMFONT };
BPTR lock;
int i;
MakeStaticHook(WindowRef, WindowHook);

#ifdef __AROS__
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37);
  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 37);
  UtilityBase = (struct UtilityBase *)OpenLibrary("utility.library", 0);
  
  if (!IntuitionBase || !GfxBase || !UtilityBase)
  {
     Quit();
  }
  
  InitLocale("Typeface.catalog", 0);
#endif

  if (IntuitionBase->LibNode.lib_Version < 37) Quit();
  if ((AslBase = OpenLibrary("asl.library",37)) == NULL)
    MyError(GetString(msgNoAslLib));
  if ((DiskfontBase = OpenLibrary("diskfont.library",36)) == NULL)
    MyError(GetString(msgNoDiskfontLib));
  if ((IFFParseBase = OpenLibrary("iffparse.library",37)) == NULL)
    MyError(GetString(msgNoIFFParseLib));
  if ((KeymapBase = OpenLibrary("keymap.library",37)) == NULL) Quit();
  if ((BGUIBase = OpenLibrary("bgui.library",41)) == NULL)
  {
    if ((BGUIBase = OpenLibrary("PROGDIR:libs/bgui.library",41)) == NULL)
      MyError(GetString(msgNoBguiLib));
  }
#ifdef __AROS__
// FIXME: AROS does not yet have a textfield.gadget
#else
  if ((TextFieldBase = OpenLibrary("gadgets/textfield.gadget",3)) == NULL)
  {
    if ((TextFieldBase = OpenLibrary("PROGDIR:gadgets/textfield.gadget",3))
      == NULL) MyError(GetString(msgNoTextFieldGadget));
  }
#endif
  if ((FontReq = AllocAslRequestTags(ASL_FontRequest,
    ASLFO_TitleText,GetString(msgOpenFont),
    ASLFO_MaxHeight,1000,TAG_DONE)) == NULL) ErrorCode(ASLALLOC);
  if ((PrefsFontReq = AllocAslRequestTags(ASL_FontRequest,
    ASLFO_TitleText,GetString(msgScreenFont),
    ASLFO_MaxHeight,1000,TAG_DONE)) == NULL) ErrorCode(ASLALLOC);
  if (AslBase->lib_Version > 37)
  {
    if ((PrefsScrReq = AllocAslRequestTags(ASL_ScreenModeRequest,
      ASLSM_TitleText,GetString(msgScreenMode),
      ASLSM_DoDepth,TRUE,TAG_DONE)) == NULL) ErrorCode(ASLALLOC);
  }
  if ((CharClass = InitCharGadgClass()) == NULL) Quit();
  if ((EditClass = InitEditGadgClass()) == NULL) Quit();
  if ((SlideClass = InitSlideGadgClass()) == NULL) Quit();
  if ((LVClass = InitFLClass()) == NULL) Quit();
  if (InitCheckClass() == NULL) Quit();
  if (GetNewList(&CharWndList) == NULL) Quit();
  if ((WndMsgPort = CreateMsgPort()) == NULL) ErrorCode(CREATEPORT);

  if ((KeyLabelTable = AllocVec(512,MEMF_CLEAR)) == NULL)
    ErrorCode(ALLOCVEC);
  for (i = 0; i < 256; i++) *(KeyLabelTable+(i*2)) = i;

  strcpy(Prefs.ThisPubName,NAME);
  GetDefaultPubScreen(Prefs.DefPubName);
  if ((defscr = LockPubScreen(Prefs.DefPubName)) == NULL)
    ErrorCode(LOCKSCREEN);
  Prefs.Version = CURRENT_PREFS;
  Prefs.Flags = PREFS_CUSTOMSCREEN|PREFS_PIXELBORDER;
  Prefs.ScrModeID = GetVPModeID(&defscr->ViewPort);
  Prefs.ScrDepth = 2;
  Prefs.VecHeight = 11;
  Prefs.PixelX = 20;
  Prefs.PixelY = 20;
  strcpy(Prefs.ScrFontName,defscr->Font->ta_Name);
  Prefs.ScrFontHeight = defscr->Font->ta_YSize;
  strcpy(Prefs.FixedFontName,
    GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name);
  Prefs.FixedFontHeight = GfxBase->DefaultFont->tf_YSize;
  Prefs.ToolBar = DEFAULT_TBAR;
  Prefs.CharSize = FALSE;
  Prefs.CharWidth = 1;
  Prefs.CharHeight = 8;
  UnlockPubScreen(NULL,defscr);

  if ((FontWndHook = AllocVec(sizeof(struct Hook),MEMF_CLEAR)) == NULL)
    ErrorCode(ALLOCVEC);
  InitHook(FontWndHook, WindowRef, NULL);

  LoadPrefs();
  SetupScreen();
  strcpy(FontPath,"FONTS:");
  if ((lock = Lock("FONTS:",ACCESS_READ)))
  {
    NameFromLock(lock,FontPath,256);
    UnLock(lock);
  }
  strcpy(SavePath,FontPath);
  LoadFont(&topaz8,0);
}

void SetupScreen(void)
{
UWORD pens[] = { ~0 };
struct Screen *pubscr;
BOOL unique_pub_name = TRUE;

  if (CustomScreen)
  {
    strcpy(ScrFontAttrName,Prefs.ScrFontName);
    ScrFontAttr.ta_Name = ScrFontAttrName;
    ScrFontAttr.ta_YSize = Prefs.ScrFontHeight;
    ScrFontAttr.ta_Style = FS_NORMAL;
    ScrFont = OpenDiskFont(&ScrFontAttr);

    if ((pubscr = LockPubScreen(Prefs.ThisPubName)) != NULL)
    {
      UnlockPubScreen(NULL,pubscr);
      unique_pub_name = FALSE;
    }

    if ((Screen = OpenScreenTags(NULL,
      SA_Depth,Prefs.ScrDepth,
      SA_DisplayID,Prefs.ScrModeID,
      unique_pub_name ? SA_PubName : TAG_IGNORE,Prefs.ThisPubName,
      SA_Pens,pens,
      SA_Font,&ScrFontAttr,
      SA_Title,NAME,
      SA_AutoScroll,TRUE,
      TAG_DONE)) == NULL) ErrorCode(OPENSCREEN);
    PubScreenStatus(Screen,0);
  }
  else
  {
    if ((Screen = LockPubScreen(Prefs.DefPubName)) == NULL)
    {
      if ((Screen = LockPubScreen(NULL)) == NULL)
	ErrorCode(LOCKSCREEN);
    }
  }
  GetScreenRatio(Screen);

  if ((ScrDrawInfo = GetScreenDrawInfo(Screen)) == NULL) Quit();
  SizeImage = (struct Image *)CreateSysImage(SIZEIMAGE,ScrDrawInfo);
  LeftImage = (struct Image *)CreateSysImage(LEFTIMAGE,ScrDrawInfo);
  RightImage = (struct Image *)CreateSysImage(RIGHTIMAGE,ScrDrawInfo);
  UpImage = (struct Image *)CreateSysImage(UPIMAGE,ScrDrawInfo);
  DownImage = (struct Image *)CreateSysImage(DOWNIMAGE,ScrDrawInfo);
  Box.Top = Screen->BarHeight+1;
  if (AutoHeight) Prefs.VecHeight = SizeY(11);
  OpenFontWnd();
  ScreenToFront(Screen);
}

HOOKPROTONO(WindowHook, ULONG, struct IntuiMessage *msg)
{
IPTR  height,pos;
struct IBox newp;

  if ((msg->Class == IDCMP_CHANGEWINDOW) && (msg->Code == CWCODE_MOVESIZE))
  {
    GetAttr(WINDOW_Bounds,FontWndObj,(IPTR *)&newp);
    if ((newp.Width != FontSize.Width) || (newp.Height != FontSize.Height))
    {
      height = (FontWnd->Height-FontWnd->BorderTop-FontWnd->BorderBottom-
	(2*SizeY(2)))/GetCharGadgHeight(1,FixFont);
      GetAttr(CG_Pos,CharObj,&pos);
      SetGadgetAttrs((struct Gadget *)ScrollObj,FontWnd,NULL,
	PGA_Top,pos,
	PGA_Total,256/(4<<Prefs.CharWidth),
	PGA_Visible,height,TAG_DONE);
      GetAttr(PGA_Top,ScrollObj,&pos);
      SetGadgetAttrs((struct Gadget *)CharObj,FontWnd,NULL,
	CG_Pos,pos,
	CG_SizeY,height,
	CG_Redraw,TRUE,TAG_DONE);
    }
    GetAttr(WINDOW_Bounds,FontWndObj,(IPTR *)&FontSize);
  }
  return 0;
}

void OpenFontWnd(void)
{
ULONG height;

  strcpy(FixFontAttrName,Prefs.FixedFontName);
  FixFontAttr.ta_Name = FixFontAttrName;
  FixFontAttr.ta_YSize = Prefs.FixedFontHeight;
  FixFontAttr.ta_Style = FS_NORMAL;
  FixFont = OpenDiskFont(&FixFontAttr);

  height = Prefs.CharHeight+1;
  do
  {
    height--;
    Box.Height = GetCharGadgHeight(height,FixFont)+(2*SizeY(2))+
      Screen->WBorTop+1+Screen->Font->ta_YSize+
      (Prefs.CharSize ? SizeImage->Height : Screen->WBorBottom);
  } while (Box.Height > Screen->Height);

  SetupMenus(Menus);
  FontWndObj = WindowObject,
    WINDOW_Screen,Screen,
    WINDOW_SharedPort,WndMsgPort,
    WINDOW_IDCMPHook,FontWndHook,
    WINDOW_IDCMPHookBits,IDCMP_CHANGEWINDOW,
    WINDOW_MenuStrip,Menus,
    WINDOW_Bounds,&Box,
    WINDOW_HelpFile,NAME".guide",
    WINDOW_SmartRefresh,TRUE,
    WINDOW_NoBufferRP,TRUE,
    WINDOW_SizeGadget,Prefs.CharSize,
    WINDOW_Title,WinTitle,
    WINDOW_MasterGroup,
      HGroupObject,
	HOffset(SizeX(4)),VOffset(SizeY(2)),Spacing(SizeX(2)),
	StartMember,
	  CharObj = NewObject(CharClass,NULL,
	    CG_Font,FixFont ? FixFont : GfxBase->DefaultFont,
	    CG_SizeX,4<<Prefs.CharWidth,
	    CG_SizeY,height,
	    GA_ID,ID_CHARS,
	  EndObject,
	EndMember,
	StartMember,
	  ScrollObj = PropObject,
	    PGA_Top,0,
	    PGA_Total,256/(4<<Prefs.CharWidth),
	    PGA_Visible,height,
	    PGA_Arrows,TRUE,
	    GA_ID,ID_CHARSCROLL,
	  EndObject,
	  FixWidth(16),
	EndMember,
      EndObject,
    EndObject;
  if (FontWndObj == NULL) ErrorCode(NEWWINDOW);
  if ((FontWnd = WindowOpen(FontWndObj)) == NULL)
  {
    if (Prefs.CharWidth > 0)
    {
      CloseFontWnd();
      Prefs.CharWidth--;
      OpenFontWnd();
    }
    else ErrorCode(OPENWINDOW);
  }
  FontWnd->MaxWidth = FontWnd->Width;
  FontWnd->MaxHeight = GetCharGadgHeight(256/(4<<Prefs.CharWidth),FixFont)+
    FontWnd->BorderTop+FontWnd->BorderBottom+(2*SizeY(2));
  OldWindowPtr = ((struct Process *)FindTask(NULL))->pr_WindowPtr;
  ((struct Process *)FindTask(NULL))->pr_WindowPtr = FontWnd;
  GetAttr(WINDOW_Bounds,FontWndObj,(IPTR *)&CharSize);
  CharSize.Left += (CharSize.Width + Screen->RastPort.Font->tf_YSize);
  CharSize.Width = Screen->Width - CharSize.Left;
  CharSize.Height = Screen->Height - CharSize.Top;
  GetAttr(WINDOW_Bounds,FontWndObj,(IPTR *)&FontSize);
}

void CloseFontWnd(void)
{
  ClrWindowClose(&FontWndObj,&FontWnd);
  ClrDisposeObject(&FontWndObj);
  if (FixFont)
  {
    CloseFont(FixFont);
    FixFont = NULL;
  }
}

void Quit(void)
{
  QuitScreen();
  RemoveList(&CharWndList,TRUE,NULL);
  ClearCurrentFont();
  if (ClipChar.chr_Data) FreeVec(ClipChar.chr_Data);
  if (NewFont) CloseFont(NewFont);
  if (WndMsgPort) DeleteMsgPort(WndMsgPort);
  if (SlideClass) FreeClass(SlideClass);
  if (EditClass) FreeClass(EditClass);
  if (CharClass) FreeClass(CharClass);
  if (LVClass) FreeClass(LVClass);
  FreeCheckClass();
  if (PrefsScrReq) FreeAslRequest(PrefsScrReq);
  if (PrefsFontReq) FreeAslRequest(PrefsFontReq);
  if (FontReq) FreeAslRequest(FontReq);
  if (FontWndHook) FreeVec(FontWndHook);
  if (KeyLabelTable) FreeVec(KeyLabelTable);
  if (BGUIBase) CloseLibrary(BGUIBase);
  if (TextFieldBase) CloseLibrary(TextFieldBase);
  if (IFFParseBase) CloseLibrary(IFFParseBase);
  if (KeymapBase) CloseLibrary(KeymapBase);
  if (DiskfontBase) CloseLibrary(DiskfontBase);
  if (AslBase) CloseLibrary(AslBase);
#ifdef __AROS__
  CleanupLocale();
  
  if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
  if (GfxBase) CloseLibrary((struct Library *)GfxBase);
  if (UtilityBase) CloseLibrary((struct Library *)UtilityBase);
#endif
  exit(0);
}

void QuitScreen(void)
{
  WakeWindows();
  RemoveList(&CharWndList,FALSE,(HOOKFUNC)CloseCharWin);
  if (FontWnd)
    ((struct Process *)FindTask(NULL))->pr_WindowPtr = OldWindowPtr;
  CloseFontWnd();
  CloseEditFontWnd(TRUE);
  ClosePrefsWnd(TRUE);
  CloseAssignWnd(TRUE);
  CloseResetKernWnd(TRUE);
  CloseEditCharWnd(TRUE);
  CloseWidthWnd(TRUE);
  CloseKernWnd(TRUE);
  CloseSaveWnd();
  CloseQueryWidthWnd();
  ClosePreviewWnd(TRUE);
  ClrDisposeObject((Object **)&DirReqObj);
  ClrDisposeObject((Object **)&SaveReqObj);
  ClrDisposeObject((Object **)&SizeImage);
  ClrDisposeObject((Object **)&LeftImage);
  ClrDisposeObject((Object **)&RightImage);
  ClrDisposeObject((Object **)&UpImage);
  ClrDisposeObject((Object **)&DownImage);
  if (ScrDrawInfo)
  {
    FreeScreenDrawInfo(Screen,ScrDrawInfo); ScrDrawInfo = NULL;
  }
  if (Screen)
  {
    if (CustomScreen)
    {
      PubScreenStatus(Screen,PSNF_PRIVATE);
      while (CloseScreen(Screen) == FALSE) WarnVisitors();
      if (ScrFont)
      {
	CloseFont(ScrFont); ScrFont = NULL;
      }
    }
    else UnlockPubScreen(NULL,Screen);
    Screen = NULL;
  }
}

void MsgLoop(void)
{
IPTR  signal,winsig,code,scroll_pos,pressed,old_height,left,first,last;
IPTR  old_bline,shift;
UWORD tb_entry;
SIPTR temp,normal;
LONG  height;
BOOL free_node,redraw = FALSE;
struct CharNode *node,*oldnode;
char *fontstr;
BPTR lock;

  GetAttr(WINDOW_SigMask,FontWndObj,&winsig);
  while (TRUE)
  {
    signal = Wait(winsig|SIGBREAKF_CTRL_C);
    if (signal & SIGBREAKF_CTRL_C) Quit();
    if (signal & winsig)
    {
      while ((code = CheckObjMsg(FontWnd,FontWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	    CheckQuit();
	    break;
	  case ID_CHARSCROLL:
	    GetAttr(CG_Pos,CharObj,&temp);
	    GetAttr(PGA_Top,ScrollObj,&scroll_pos);
	    if (temp != scroll_pos)
	      SetGadgetAttrs((struct Gadget *)CharObj,FontWnd,NULL,
		CG_Pos,scroll_pos,
		CG_Redraw,FALSE,TAG_DONE);
	    break;
	  case ID_CHARS:
	    GetAttr(CG_Pressed,CharObj,&pressed);
	    SetGadgetAttrs((struct Gadget *)CharObj,FontWnd,NULL,
	      CG_Pressed,~0,TAG_DONE);
	    GetAttr(CG_ShiftDown,CharObj,&shift);
	    if (InvertShift) shift = !shift;
	    if (pressed != ~0) CheckOpenCharWin(pressed,shift);
	    break;
	  default:
	    SharedMsgs(code,NULL);
	    break;
	}
      }
      while ((code = CheckObjMsg(ParamWnd,ParamWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	  case ID_CANCEL:
	    CloseEditFontWnd(FALSE);
	    break;
	  case ID_BOLD:
	  case ID_ITALIC:
	  case ID_ULINE:
	  case ID_EXTEND:
	    GetAttr(GA_Selected,BoldCheck,&normal);
	    GetAttr(GA_Selected,ItalicCheck,&temp);
	    normal |= temp;
	    GetAttr(GA_Selected,UnderCheck,&temp);
	    normal |= temp;
	    GetAttr(GA_Selected,ExtCheck,&temp);
	    normal |= temp;
	    SetGadgetAttrs((struct Gadget *)NormalCheck,ParamWnd,NULL,
	      GA_Selected,normal == 0,TAG_DONE);
	    break;
	  case ID_NORMAL:
	    SetGadgetAttrs((struct Gadget *)BoldCheck,ParamWnd,NULL,
	      GA_Selected,FALSE,TAG_DONE);
	    SetGadgetAttrs((struct Gadget *)ItalicCheck,ParamWnd,NULL,
	      GA_Selected,FALSE,TAG_DONE);
	    SetGadgetAttrs((struct Gadget *)UnderCheck,ParamWnd,NULL,
	      GA_Selected,FALSE,TAG_DONE);
	    SetGadgetAttrs((struct Gadget *)ExtCheck,ParamWnd,NULL,
	      GA_Selected,FALSE,TAG_DONE);
	    break;
	  case ID_BLANK:
	    CheckOpenCharWin(256,0);
	    break;
	  case ID_ACCEPT:
	    FlagError = FALSE;
	    GetAttr(GA_Selected,NormalCheck,&Normal);
	    GetAttr(GA_Selected,BoldCheck,&Bold);
	    GetAttr(GA_Selected,ItalicCheck,&Italic);
	    GetAttr(GA_Selected,UnderCheck,&ULine);
	    GetAttr(GA_Selected,ExtCheck,&Extended);
	    GetAttr(GA_Selected,RevCheck,&Reversed);
	    GetAttr(CYC_Active,AspectCycle,&Aspect);

	    GetAttr(STRINGA_LongVal,SmearInt,&temp);
	    PutPositive(ParamWnd,GetString(msgBoldSmear),temp,0,&Smear);

	    GetAttr(STRINGA_LongVal,HeightInt,&temp);
	    old_height = Height;
	    PutPositive(ParamWnd,GetString(msgFontHeight),temp,1,&Height);
	    if (old_height != Height)
	    {
	      redraw = TRUE;
	      NewHeights();
	    }

	    old_bline = Baseline;
	    GetAttr(STRINGA_LongVal,BaseInt,&temp);
	    if (temp >= (LONG)Height)
	    {
	      if (redraw)
	      {
		Baseline = Height-1;
	      }
	      else
	      {
		ShowReq(GetString(msgErrorBaseline),GetString(msgContinue));
		FlagError = TRUE;
	      }
	    }
	    else PutPositive(ParamWnd,GetString(msgFontBaseline),temp,0,
	      &Baseline);
	    if (old_bline != Baseline) redraw = TRUE;

	    GetAttr(CYC_Active,PropCycle,&temp);
	    if (temp != Proportional)
	    {
	      if ((Proportional = temp) == FALSE)
	      {
		if (ShowReq(GetString(msgWarnPropChange),
		  GetString(msgYesNo)) != 0)
		{
		  redraw = TRUE;
		  NewWidths();
		}
	      }
	    }

	    GetAttr(STRINGA_LongVal,WidthInt,&temp);
	    if (temp >= 0)
	    {
	      if (temp != Width)
	      {
		Width = temp;
		if (Proportional == FALSE)
		{
		  redraw = TRUE;
		  NewWidths();
		}
	      }
	    }
	    else
	    {
	      ShowReq(GetString(msgErrorFontWidth),GetString(msgContinue));
	      FlagError = TRUE;
	    }

	    if (FlagError == FALSE) CloseEditFontWnd(FALSE);
	    if (redraw == TRUE)
	    {
	      RedrawAll();
	      redraw = FALSE;
	    }
	    break;
	  default:
	    SharedMsgs(code,NULL);
	    break;
	}
      }
      while ((code = CheckObjMsg(PrefsWnd,PrefsWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	  case ID_CANCEL:
	    ClosePrefsWnd(FALSE);
	    break;
	  case ID_CUSTOM:
	    GetAttr(GA_Selected,CustButton,&temp);
	    SetGadgetAttrs((struct Gadget *)ModeButton,PrefsWnd,NULL,
	      GA_Disabled,(temp != FALSE) ? FALSE : TRUE,TAG_DONE);
	    SetGadgetAttrs((struct Gadget *)FontButton,PrefsWnd,NULL,
	      GA_Disabled,(temp != FALSE) ? FALSE : TRUE,TAG_DONE);
	    SetGadgetAttrs((struct Gadget *)PubStr,PrefsWnd,NULL,
	      STRINGA_TextVal,GetPubName(temp),TAG_DONE);
	    break;
	  case ID_CHOOSEMODE:
	    SleepWindows();
	    if (AslBase->lib_Version > 37)
	    {
	      if (AslRequestTags(PrefsScrReq,
		ASLSM_Window,PrefsWnd,
		ASLSM_InitialHeight,Screen->Height,
		ASLSM_InitialDisplayID,PrefScrModeID,
		ASLSM_InitialDisplayDepth,PrefScrDepth,TAG_DONE))
	      {
		PrefScrModeID = PrefsScrReq->sm_DisplayID;
		PrefScrDepth = PrefsScrReq->sm_DisplayDepth;
		GetDisplayInfoData(NULL,(UBYTE *)&ModeIDName,
		  sizeof(struct NameInfo),DTAG_NAME,PrefScrModeID);
		SetGadgetAttrs((struct Gadget *)ModeInfo,PrefsWnd,NULL,
		  INFO_TextFormat,ModeIDName.Name,TAG_DONE);
	      }
	    }
	    else ShowReq(GetString(msgNeedAslV38),GetString(msgCancel));
	    WakeWindows();
	    break;
	  case ID_CHOOSEFONT:
	    SleepWindows();
	    if (AslRequestTags(PrefsFontReq,
	      ASLFO_Window,PrefsWnd,
	      ASLFO_InitialHeight,Screen->Height,
	      ASLFO_InitialName,PrefScrFontName,
	      ASLFO_InitialSize,PrefScrFontHeight,
	      ASLFO_FixedWidthOnly,FALSE,TAG_DONE))
	    {
	      strcpy(PrefScrFontName,PrefsFontReq->fo_Attr.ta_Name);
	      PrefScrFontHeight = PrefsFontReq->fo_Attr.ta_YSize;
	      WriteFont(PrefScrBuffer,PrefScrFontName,PrefScrFontHeight);
	      SetGadgetAttrs((struct Gadget *)FontInfo,PrefsWnd,NULL,
		INFO_TextFormat,PrefScrBuffer,TAG_DONE);
	    }
	    WakeWindows();
	    break;
	  case ID_CHOOSEFIXFONT:
	    SleepWindows();
	    if (AslRequestTags(PrefsFontReq,
	      ASLFO_Window,PrefsWnd,
	      ASLFO_InitialHeight,Screen->Height,
	      ASLFO_InitialName,PrefFixFontName,
	      ASLFO_InitialSize,PrefFixFontHeight,
	      ASLFO_FixedWidthOnly,TRUE,TAG_DONE))
	    {
	      strcpy(PrefFixFontName,PrefsFontReq->fo_Attr.ta_Name);
	      PrefFixFontHeight = PrefsFontReq->fo_Attr.ta_YSize;
	      WriteFont(PrefFixBuffer,PrefFixFontName,PrefFixFontHeight);
	      SetGadgetAttrs((struct Gadget *)ChrFontInfo,PrefsWnd,NULL,
		INFO_TextFormat,PrefFixBuffer,TAG_DONE);
	    }
	    WakeWindows();
	    break;
	  case ID_PUBNAME:
	    CopyPubName();
	    break;
	  case ID_AUTOHEIGHT:
	    GetAttr(GA_Selected,AutoCheck,&temp);
	    SetGadgetAttrs((struct Gadget *)VecInt,PrefsWnd,NULL,
	      GA_Disabled,temp,TAG_DONE);
	    break;
	  case ID_ACCEPT:
	  case ID_USE:
	    FlagError = FALSE;
	    NewCustom = CustomScreen;
	    NewWindow = FALSE;
	    GetAttr(GA_Selected,CustButton,&temp);

	    if ((temp == 0) != (CustomScreen == 0))
	    {
	      ChangeScreen = TRUE;
	      NewCustom = (CustomScreen == TRUE) ? FALSE : TRUE;
	    }

	    if (temp != FALSE)
	    {
	      if ((PrefScrModeID != Prefs.ScrModeID) ||
		(PrefScrDepth != Prefs.ScrDepth))
	      {
		ChangeScreen = TRUE;
		NewCustom = TRUE;
	      }
	    }
	    Prefs.ScrModeID = PrefScrModeID;
	    Prefs.ScrDepth = PrefScrDepth;

	    if (temp != FALSE)
	    {
	      if ((stricmp(PrefScrFontName,Prefs.ScrFontName) != 0) ||
		(PrefScrFontHeight != Prefs.ScrFontHeight))
	      {
		ChangeScreen = TRUE;
		NewCustom = TRUE;
	      }
	    }
	    strcpy(Prefs.ScrFontName,PrefScrFontName);
	    Prefs.ScrFontHeight = PrefScrFontHeight;

	    CopyPubName();
	    if (temp != FALSE)
	    {
	      if (stricmp(PrefThisPubName,Prefs.ThisPubName) != 0)
	      {
		ChangeScreen = TRUE;
	        NewCustom = TRUE;
	      }
	    }
	    else
	    {
	      if (stricmp(PrefDefPubName,Prefs.DefPubName) != 0)
	      {
		ChangeScreen = TRUE;
	        NewCustom = FALSE;
	      }
	    }
	    strcpy(Prefs.ThisPubName,PrefThisPubName);
	    strcpy(Prefs.DefPubName,PrefDefPubName);

	    GetAttr(STRINGA_LongVal,VecInt,&temp);
	    PutPositive(PrefsWnd,GetString(msgToolbarHeight),temp,10,
	      &(Prefs.VecHeight));
	    Prefs.ToolBar = 0;
	    tb_entry = FirstEntry(ToolBarIn);
	    while (tb_entry)
	    {
	      Prefs.ToolBar |= tb_entry;
	      tb_entry = NextEntry(ToolBarIn,tb_entry);
	    }
	    GetAttr(GA_Selected,AutoCheck,&temp);
	    AutoHeight = temp ? TRUE : FALSE;
	    if (AutoHeight) Prefs.VecHeight = SizeY(11);

	    GetAttr(STRINGA_LongVal,PixelXInt,&temp);
	    PutPositive(PrefsWnd,GetString(msgPixelWidth),temp,1,
	      &(Prefs.PixelX));
	    GetAttr(STRINGA_LongVal,PixelYInt,&temp);
	    PutPositive(PrefsWnd,GetString(msgPixelHeight),temp,1,
	      &(Prefs.PixelY));
	    GetAttr(CYC_Active,PixelCycle,&PixelBorder);
	    GetAttr(GA_Selected,MaxCheck,&temp);
	    MaxWindow = temp ? TRUE : FALSE;
	    GetAttr(GA_Selected,BLineCheck,&temp);
	    ShowBLine = temp ? TRUE : FALSE;
	    GetAttr(GA_Selected,ToggleCheck,&temp);
	    Toggle = temp ? TRUE : FALSE;

	    GetAttr(GA_Selected,ShiftCheck,&temp);
	    InvertShift = temp ? TRUE : FALSE;
	    GetAttr(GA_Selected,SizeCheck,&temp);
	    if (Prefs.CharSize != temp)
	    {
	      Prefs.CharSize = temp;
	      NewWindow = TRUE;
	    }
	    GetAttr(CYC_Active,ChrWCycle,&temp);
	    if (Prefs.CharWidth != temp)
	    {
	      Prefs.CharWidth = temp;
	      NewWindow = TRUE;
	    }
	    GetAttr(STRINGA_LongVal,ChrHInt,&temp);
	    height = 0;
	    PutPositive(PrefsWnd,GetString(msgSelWndWidth),temp,0,&height);
	    if (height > 0)
	    {
	      if (height*(4<<Prefs.CharWidth) > 256)
		height = 256/(4<<Prefs.CharWidth);
	      if (Prefs.CharHeight != height)
	      {
		Prefs.CharHeight = height;
		NewWindow = TRUE;
	      }
	    }
	    if ((stricmp(PrefFixFontName,Prefs.FixedFontName) != 0) ||
	      (PrefFixFontHeight != Prefs.FixedFontHeight)) NewWindow = TRUE;
	    strcpy(Prefs.FixedFontName,PrefFixFontName);
	    Prefs.FixedFontHeight = PrefFixFontHeight;

	    GetAttr(CYC_Active,SaveCycle,&(Prefs.SaveTo));
	    GetAttr(GA_Selected,QuitCheck,&temp);
	    ConfirmQuit = temp ? TRUE : FALSE;
	    GetAttr(GA_Selected,QueryCheck,&temp);
	    QueryWidth = temp ? TRUE : FALSE;

	    if (code == ID_ACCEPT)
	    {
	      if (Prefs.SaveTo == 0)
	      {
		SavePrefs("ENV:"NAME".prefs");
		SavePrefs("ENVARC:"NAME".prefs");
	      }
	      else SavePrefs("PROGDIR:"NAME".prefs");
	    }
	    if (FlagError == FALSE) ClosePrefsWnd(FALSE);
	    break;
	  default:
	    SharedMsgs(code,NULL);
	    break;
	}
      }
      while ((code = CheckObjMsg(AssignWnd,AssignWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	  case ID_CANCEL:
	    CloseAssignWnd(FALSE);
	    break;
	  case ID_ASSIGN:
	  case ID_ASSIGNADD:
	    GetAttr(STRINGA_TextVal,FontDirStr,(IPTR *)&fontstr);
	    strcpy(FontPath,fontstr);
	    SleepWindows();
	    if ((lock = Lock(FontPath,ACCESS_READ)) != 0)
	    {
	      if (code == ID_ASSIGNADD)
		AssignAdd("FONTS",lock);
	      else
		AssignLock("FONTS",lock);
	      strcpy(SavePath,FontPath);
	      FlushAllFonts();
	    }
	    else ShowReq(GetString(msgNoDir),GetString(msgCancel),FontPath);
	    WakeWindows();
	    CloseAssignWnd(FALSE);
	    break;
	  case ID_ASSIGNPATH:
	    SleepWindows();
	    GetAttr(STRINGA_TextVal,FontDirStr,(IPTR *)&fontstr);
	    SetAttrs(DirReqObj,
	      ASLFR_InitialHeight,Screen->Height,
	      ASLFR_InitialDrawer,fontstr,TAG_DONE);
	    if (DoRequest(DirReqObj) == FRQ_OK)
	    {
	      GetAttr(FRQ_Drawer,DirReqObj,(IPTR *)&fontstr);
	      SetGadgetAttrs((struct Gadget *)FontDirStr,AssignWnd,NULL,
		STRINGA_TextVal,fontstr,TAG_DONE);
	    }
	    WakeWindows();
	    break;
	  default:
	    SharedMsgs(code,NULL);
	    break;
	}
      }
      while ((code = CheckObjMsg(ResetKernWnd,ResetKernWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	  case ID_CANCEL:
	    CloseResetKernWnd(FALSE);
	    break;
	  case ID_ACCEPT:
          {
	    SIPTR space,kern;

	    GetAttr(STRINGA_LongVal,ResetSpaceInt,&space);
	    GetAttr(STRINGA_LongVal,ResetKernInt,&kern);
	    CloseResetKernWnd(FALSE);
	    ResetKerning(space,kern);
	    break;
	  }
	  default:
	    SharedMsgs(code,NULL);
	    break;
	}
      }
      while ((code = CheckObjMsg(EditCharWnd,EditCharWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	  case ID_CANCEL:
	    CloseEditCharWnd(FALSE);
	    break;
	  case ID_ACCEPT:
	  case ID_EDITCHARNUM:
	    GetAttr(STRINGA_LongVal,EditCharNum,&temp);
	    if ((temp >= 0) && (temp < 256))
	    {
	      CloseEditCharWnd(FALSE);
	      CheckOpenCharWin(temp,0);
	    }
	    else
	    {
	      DisplayBeep(EditCharWnd->WScreen);
	      ActivateGadget((struct Gadget *)EditCharNum,EditCharWnd,NULL);
	    }
	    break;
	  default:
	    SharedMsgs(code,NULL);
	    break;
	}
      }
      while ((code = CheckObjMsg(QueryWnd,QueryWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case ID_ACCEPT:
	    WakeWindows();
	    CloseQueryWidthWnd();
	    GetAttr(STRINGA_LongVal,QueryWidthNum,&temp);
	    LoadFont(&FontReq->fo_Attr,temp);
	    LoadChosenFont();
	    break;
	}
      }
      while ((code = CheckObjMsg(PreviewWnd,PreviewWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	  case ID_CANCEL:
	    ClosePreviewWnd(FALSE);
	    break;
	  default:
	    PreviewMsgs(code);
	    break;
	}
      }
      while ((code = CheckObjMsg(WidthWnd,WidthWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	  case ID_CANCEL:
	    CloseWidthWnd(FALSE);
	    break;
	  case ID_ACCEPT:
	    GetAttr(STRINGA_LongVal,ChrWidthInt,&temp);
	    GetAttr(PGA_Top,ChrWidthScroll,&left);
	    if (temp >= 0)
	    {
	      if (temp > WidthChar->chr_Width)
		ChangeCharWidth(WidthChar,temp,left,0);
	      else
		ChangeCharWidth(WidthChar,temp,0,left);
	      RedrawEdit(WidthNode);
	      CloseWidthWnd(FALSE);
	    }
	    else ShowReq(GetString(msgErrorFontWidth),
	      GetString(msgContinue));
	    break;
	  case ID_CHARWIDTH:
	    GetAttr(STRINGA_LongVal,ChrWidthInt,&temp);
	    if (temp >= 0)
	    {
	      SetGadgetAttrs((struct Gadget *)ChrWidthScroll,WidthWnd,NULL,
		PGA_Total,MAX(WidthChar->chr_Width,temp),
		PGA_Visible,MIN(WidthChar->chr_Width,temp),TAG_DONE);
	    }
	    else
	    {
	      ShowReq(GetString(msgErrorFontWidth),GetString(msgContinue));
	      SetGadgetAttrs((struct Gadget *)ChrWidthInt,WidthWnd,NULL,
		STRINGA_LongVal,WidthChar->chr_Width,TAG_DONE);
	    }
	    break;
	  default:
	    SharedMsgs(code,WidthNode);
	    break;
	}
      }
      while ((code = CheckObjMsg(KernWnd,KernWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	  case ID_CANCEL:
	    CloseKernWnd(FALSE);
	    break;
	  case ID_ACCEPT:
	    GetAttr(STRINGA_LongVal,SpaceObj,&temp);
	    SpaceTable[KernNode->chln_Number] = temp;
	    GetAttr(STRINGA_LongVal,KernObj,&temp);
	    KernTable[KernNode->chln_Number] = temp;
	    CloseKernWnd(FALSE);
	    break;
	  default:
	    SharedMsgs(code,KernNode);
	    break;
	}
      }
      while ((code = CheckObjMsg(SaveWnd,SaveWndObj)) != WMHI_NOMORE)
      {
	switch (code)
	{
	  case WMHI_CLOSEWINDOW:
	  case ID_CANCEL:
	    CloseSaveWnd();
	    break;
	  case ID_ACCEPT:
	    GetAttr(STRINGA_LongVal,FirstNum,&first);
	    if ((first < 0) || (first > 255))
	      SaveShowReq(GetString(msgFirstRange),GetString(msgCancel));
	    else
	    {
	      GetAttr(STRINGA_LongVal,LastNum,&last);
	      if ((last < 0) || (last > 255))
		SaveShowReq(GetString(msgLastRange),GetString(msgCancel));
	      else
	      {
		if (first > last) SaveShowReq(GetString(msgFirstGreaterLast),
		  GetString(msgCancel));
		else
		{
		  FirstChar = first;
		  LastChar = last;
		  GetAttr(STRINGA_TextVal,SaveDirStr,(IPTR *)&fontstr);
		  strcpy(SavePath,fontstr);
		  GetAttr(STRINGA_TextVal,FontNameStr,(IPTR *)&fontstr);
		  strcpy(FontName,fontstr);
		  GetAttr(GA_Selected,TableCheck,&UseTable);
		  SaveFont(UseTable,FALSE);
		  sprintf(WinTitle,"%s/%ld",FontName,(long)Height);
		  SetAttrs(FontWndObj,WINDOW_Title,WinTitle,TAG_DONE);
		  CloseSaveWnd();
		}
	      }
	    }
	    break;
	  case ID_GETSAVEDIR:
	    GetAttr(STRINGA_TextVal,SaveDirStr,(IPTR *)&fontstr);
	    SetAttrs(SaveReqObj,
	      ASLFR_Window,SaveWnd,
	      ASLFR_InitialHeight,Screen->Height,
	      ASLFR_InitialDrawer,fontstr,TAG_DONE);
	    WindowBusy(SaveWndObj);
	    if (DoRequest(SaveReqObj) == FRQ_OK)
	    {
	      GetAttr(FRQ_Drawer,SaveReqObj,(IPTR *)&fontstr);
	      SetGadgetAttrs((struct Gadget *)SaveDirStr,SaveWnd,NULL,
		STRINGA_TextVal,fontstr,TAG_DONE);
	    }
	    WindowReady(SaveWndObj);
	    break;
	}
      }
      node = (struct CharNode *)CharWndList->lh_Head;
      while (node->chln_Node.ln_Succ)
      {
	free_node = FALSE;
	while ((code =
	  CheckObjMsg(node->chln_Window,node->chln_Object)) != WMHI_NOMORE)
	{
	  switch (code)
	  {
	    case WMHI_CLOSEWINDOW:
	      CloseCharWin(node);
	      free_node = TRUE;
	      break;
	    default:
	      SharedMsgs(code,node);
	      break;
	  }
	}
	oldnode = node;
	node = (struct CharNode *)node->chln_Node.ln_Succ;
	if (free_node)
	{
	  Remove((struct Node *)oldnode);
	  FreeVec(oldnode);
	}
      }
    }
    if ((ChangeScreen) && (PrefsWnd == NULL))
    {
      QuitScreen();
      CustomScreen = NewCustom;
      SetupScreen();
      SetAttrs(FontWndObj,WINDOW_Title,WinTitle,TAG_DONE);
      GetAttr(WINDOW_SigMask,FontWndObj,&winsig);
      ChangeScreen = FALSE;
      NewWindow = FALSE;
    }
    if ((NewWindow) && (PrefsWnd == NULL))
    {
      if (FontWnd)
      {
	((struct Process *)FindTask(NULL))->pr_WindowPtr = OldWindowPtr;
	CloseFontWnd();
	OpenFontWnd();
	NewWindow = FALSE;
	GetAttr(WINDOW_SigMask,FontWndObj,&winsig);
      }
    }
  }
}

ULONG CheckObjMsg(struct Window *wnd, Object *obj)
{
  return ((wnd != NULL) ? HandleEvent(obj) : WMHI_NOMORE);
}

void CheckOpenCharWin(ULONG pressed, ULONG shift)
{
struct CharNode *node;
struct Character *oldchar;
BOOL open = FALSE;

  node = (struct CharNode *)CharWndList->lh_Head;
  while (node->chln_Node.ln_Succ)
  {
    if (pressed == node->chln_Number)
    {
      open = TRUE;
      WindowToFront(node->chln_Window);
      ActivateWindow(node->chln_Window);
    }
    node = (struct CharNode *)node->chln_Node.ln_Succ;
  }
  if (open == FALSE)
  {
    if (shift != 0)
    {
      node = (struct CharNode *)CharWndList->lh_Head;
      if (node->chln_Node.ln_Succ)
      {
	oldchar = node->chln_Character;
	node->chln_Character = CharBuffer+pressed;
	node->chln_Number = pressed;
	SetGadgetAttrs((struct Gadget *)node->chln_EditGadg,
	  node->chln_Window,NULL,
	  EG_CharNode,node,
	  EG_CharStruct,node->chln_Character,TAG_DONE);
	if (pressed > 0)
	  sprintf(node->chln_Title,GetString(msgCharTitle),
	    (char)pressed,pressed,pressed);
	else
	  sprintf(node->chln_Title,GetString(msgZeroTitle));
	RedrawEdit(node);
	SetAttrs(node->chln_Object,WINDOW_Title,node->chln_Title,TAG_DONE);
	WindowToFront(node->chln_Window);
	ActivateWindow(node->chln_Window);
	if (oldchar == WidthChar) OpenWidthWnd(node);
	if (oldchar == KernChar) OpenKernWnd(node);
      }
      else OpenCharWin(pressed,CharSize);
    }
    else OpenCharWin(pressed,CharSize);
  }
}

void SharedMsgs(ULONG code,struct CharNode *node)
{
  switch (code)
  {
    case ID_OPEN:
      SleepWindows();
      if (AslRequestTags(FontReq,
	ASLFO_InitialHeight,Screen->Height,
	ASLFO_Window,FontWnd,TAG_DONE))
      {
	if ((LoadFont(&FontReq->fo_Attr,0)) && (QueryWidth))
	{
	  OpenQueryWidthWnd();
	}
	else
	{
	  WakeWindows();
	  LoadChosenFont();
	}
      }
      else WakeWindows();
      break;
    case ID_SAVE:
      OpenSaveWnd();
      break;
    case ID_CHANGEDIR:
      OpenAssignWnd();
      break;
    case ID_PREVIEW:
      OpenPreviewWnd();
      break;
    case ID_EDITFONT:
      OpenEditFontWnd();
      break;
    case ID_EDITCHAR:
      OpenEditCharWnd();
      break;
    case ID_PREFS:
      OpenPrefsWnd();
      break;
    case ID_ABOUT:
      OpenAboutWnd();
      break;
    case ID_QUIT:
      CheckQuit();
      break;
    case ID_WIDTHL:
      if (node)
      {
	if (node->chln_Character->chr_Width > 0)
	{
	  ChangeCharWidth(node->chln_Character,
	    node->chln_Character->chr_Width-1,0,0);
	  RedrawEdit(node);
	}
      }
      break;
    case ID_WIDTHR:
      if (node)
      {
	ChangeCharWidth(node->chln_Character,
	  node->chln_Character->chr_Width+1,0,0);
	  RedrawEdit(node);
      }
      break;
    case ID_WIDTHC:
      if (node) OpenWidthWnd(node);
      break;
    case ID_KERNING:
      if (node) OpenKernWnd(node);
      break;
    case ID_MOVEL:
      if (node) MoveLeft(node,NULL);
      break;
    case ID_MOVER:
      if (node) MoveRight(node,NULL);
      break;
    case ID_MOVEU:
      if (node) MoveUp(node,NULL);
      break;
    case ID_MOVED:
      if (node) MoveDown(node,NULL);
      break;
    case ID_FLIPH:
      if (node) FlipHoriz(node);
      break;
    case ID_FLIPV:
      if (node) FlipVert(node);
      break;
    case ID_UNDO:
      if (node) PasteChar(node,&(node->chln_UndoChar));
      break;
    case ID_CUT:
      if (node)
      {
	CopyChar(node,&ClipChar);
	EraseChar(node);
      }
      break;
    case ID_COPY:
      if (node) CopyChar(node,&ClipChar);
      break;
    case ID_PASTE:
      if (node) PasteChar(node,&ClipChar);
      break;
    case ID_ERASE:
      if (node) EraseChar(node);
      break;
    case ID_MOVEFL:
      MoveAll(MoveLeft);
      break;
    case ID_MOVEFR:
      MoveAll(MoveRight);
      break;
    case ID_MOVEFU:
      MoveAll(MoveUp);
      break;
    case ID_MOVEFD:
      MoveAll(MoveDown);
      break;
    case ID_RESETKERN:
      OpenResetKernWnd();
      break;
    case ID_CASCADE:
      CascadeWindows();
      break;
    case ID_ZOOMIN:
      if (node) ZoomIn(node);
      break;
    case ID_ZOOMOUT:
      if (node) ZoomOut(node);
      break;
  }
}

void LoadChosenFont(void)
{
  if (ParamWnd)
  {
    SetGadgetAttrs((struct Gadget *)PropCycle,ParamWnd,NULL,
      CYC_Active,Proportional,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)HeightInt,ParamWnd,NULL,
      STRINGA_LongVal,Height,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)WidthInt,ParamWnd,NULL,
      STRINGA_LongVal,Width,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)BaseInt,ParamWnd,NULL,
      STRINGA_LongVal,Baseline,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)SmearInt,ParamWnd,NULL,
      STRINGA_LongVal,Smear,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)NormalCheck,ParamWnd,NULL,
      GA_Selected,Normal,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)BoldCheck,ParamWnd,NULL,
      GA_Selected,Bold,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)ItalicCheck,ParamWnd,NULL,
      GA_Selected,Italic,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)UnderCheck,ParamWnd,NULL,
      GA_Selected,ULine,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)ExtCheck,ParamWnd,NULL,
      GA_Selected,Extended,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)RevCheck,ParamWnd,NULL,
      GA_Selected,Reversed,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)AspectCycle,ParamWnd,NULL,
      CYC_Active,Aspect,TAG_DONE);
  }
  RedrawAll();
}

void MyError(CONST_STRPTR message)
{
  ShowReq(message,GetString(msgCancel));
  Quit();
}

LONG ShowReq(CONST_STRPTR text,CONST_STRPTR gadgets,...)
{
va_list va;
LONG req_return;
struct EasyStruct req =
  { sizeof(struct EasyStruct),0,NAME,NULL,NULL };

  SleepWindows();
  req.es_TextFormat = text;
  req.es_GadgetFormat = gadgets;
  va_start(va,gadgets);
#if defined(__AROS__) && defined(__ARM_ARCH__)
  #warning "TODO: fix va_arg usage for ARM"
  req_return = 0;
#else
  req_return = EasyRequestArgs(FontWnd,&req,NULL,va);
#endif
  va_end(va);
  WakeWindows();
  return req_return;
}

void ClrDisposeObject(Object **obj)
{
  if (*obj)
  {
    DisposeObject(*obj);
    *obj = NULL;
  }
}

void ClrWindowClose(Object **obj,struct Window **wnd)
{
  if (*wnd)
  {
    WindowClose(*obj);
    *wnd = NULL;
  }
}

void SleepWindows(void)
{
struct CharNode *node;

  if (FontWnd)
  {
    WindowBusy(FontWndObj);
    if (ParamWnd) WindowBusy(ParamWndObj);
    if (PrefsWnd) WindowBusy(PrefsWndObj);
    if (AssignWnd) WindowBusy(AssignWndObj);
    if (ResetKernWnd) WindowBusy(ResetKernWndObj);
    if (EditCharWnd) WindowBusy(EditCharWndObj);
    if (WidthWnd) WindowBusy(WidthWndObj);
    if (KernWnd) WindowBusy(KernWndObj);
    if (PreviewWnd) WindowBusy(PreviewWndObj);
    node = (struct CharNode *)CharWndList->lh_Head;
    while (node->chln_Node.ln_Succ)
    {
      WindowBusy(node->chln_Object);
      node =(struct CharNode *) node->chln_Node.ln_Succ;
    }
  }
}

void WakeWindows(void)
{
struct CharNode *node;

  if (FontWnd)
  {
    WindowReady(FontWndObj);
    if (ParamWnd) WindowReady(ParamWndObj);
    if (PrefsWnd) WindowReady(PrefsWndObj);
    if (AssignWnd) WindowReady(AssignWndObj);
    if (ResetKernWnd) WindowReady(ResetKernWndObj);
    if (EditCharWnd) WindowReady(EditCharWndObj);
    if (WidthWnd) WindowReady(WidthWndObj);
    if (KernWnd) WindowReady(KernWndObj);
    if (PreviewWnd) WindowReady(PreviewWndObj);
    node = (struct CharNode *)CharWndList->lh_Head;
    while (node->chln_Node.ln_Succ)
    {
      WindowReady(node->chln_Object);
      node = (struct CharNode *)node->chln_Node.ln_Succ;
    }
  }
}

BOOL UnpackChar(struct Character *chr,struct TextFont *font,ULONG i)
{
struct charDef *def;
ULONG j,k,mod,off,width,modj,widthj;
UBYTE *data;

  mod = font->tf_Modulo;
  data = (UBYTE *)font->tf_CharData;
  def = ((struct charDef *)(font->tf_CharLoc))+i;
  off = def->charOffset;
  chr->chr_Width = def->charBitWidth;
  if ((width = chr->chr_Width) > 0)
  {
    if ((chr->chr_Data = AllocVec(chr->chr_Width*chr->chr_Height,MEMF_ANY))
      == NULL) return (FALSE);
    for (j = 0; j < chr->chr_Height; j++)
    {
      modj = mod*j;
      widthj = width*j;
      for (k = 0; k < width; k++)
        *(chr->chr_Data+widthj+k) = GETBIT(data+modj,off+k);
    }
  }
  return (TRUE);
}

void KernTables(ULONG to, ULONG from)
{
  if (NewFont->tf_CharSpace)
    SpaceTable[to] = *((WORD *)(NewFont->tf_CharSpace)+from);
  if (NewFont->tf_CharKern)
    KernTable[to] = *((WORD *)(NewFont->tf_CharKern)+from);
  if ((NewFont->tf_CharSpace) || (NewFont->tf_CharKern))
    UseTable = TRUE;
  else
    UseTable = FALSE;
}

void ClearCurrentFont(void)
{
struct Character *chr;

  for (chr = CharBuffer; chr < CharBuffer+257; chr++)
  {
    if (chr->chr_Data) FreeVec(chr->chr_Data);
    chr->chr_Data = NULL;
  }
}

void OpenEditFontWnd(void)
{
Object *accept, *cancel;

  if (ParamWnd)
  {
    WindowToFront(ParamWnd);
    ActivateWindow(ParamWnd);
  }
  else
  {
    if (ParamWndObj == NULL)
    {
      PropLabels[0] = GetString(msgFontFixed);
      PropLabels[1] = GetString(msgFontProp);
      AspectLabels[0] = GetString(msgAspectNormal);
      AspectLabels[1] = GetString(msgAspectThin);
      AspectLabels[2] = GetString(msgAspectWide);
      SetupMenus(Menus);

      ParamWndObj = WindowObject,
	WINDOW_Screen,Screen,
	WINDOW_SharedPort,WndMsgPort,
	WINDOW_MenuStrip,Menus,
	WINDOW_Title,GetString(msgFontParamsTitle),
	WINDOW_HelpFile,NAME".guide",
	WINDOW_HelpNode,"params",
	WINDOW_SmartRefresh,TRUE,
	WINDOW_SizeGadget,FALSE,
	WINDOW_CloseOnEsc,TRUE,
	WINDOW_MasterGroup,
	  VGroupObject,
	    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
	    GROUP_BackFill,SHINE_RASTER,
	    StartMember,
	      VGroupObject,
		HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		ButtonFrame,
		FRM_Recessed,TRUE,
		StartMember,
		  PropCycle = KeyCycle(GetString(gadgFontType),PropLabels,
		    Proportional,ID_PROPORTIONAL),
		EndMember,
		StartMember,
		  HGroupObject,
		    Spacing(SizeX(8)),
		    StartMember,
		      VGroupObject,
			Spacing(SizeY(2)),
			StartMember,
			  HeightInt = TabKeyInteger(
			    GetString(gadgFontHeight),Height,5,ID_HEIGHT),
			EndMember,
			StartMember,
			  BaseInt = TabKeyInteger(
			    GetString(gadgFontBaseline),Baseline,5,
			    ID_BASELINE),
			EndMember,
		      EndObject,
		    EndMember,
		    StartMember,
		      VGroupObject,
			Spacing(SizeY(2)),
			StartMember,
			  WidthInt = TabKeyInteger(GetString(gadgFontWidth),
			    Width,5,ID_WIDTH),
			EndMember,
			StartMember,
			  SmearInt = TabKeyInteger(GetString(gadgBoldSmear),
			    Smear,5,ID_SMEAR),
			EndMember,
		      EndObject,
		    EndMember,
		  EndObject,
		EndMember,
	      EndObject,
	    EndMember,
	    StartMember,
	      VGroupObject,
		HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		ButtonFrame,
		FRM_Recessed,TRUE,
		StartMember,
		  HGroupObject,
		    Spacing(SizeX(8)),
		    StartMember,
		      VGroupObject,
			Spacing(SizeY(2)),
			StartMember,
			  NormalCheck =
			    KeyCheckBox(GetString(gadgFontNormal),Normal,
			      ID_NORMAL),
			EndMember,
			StartMember,
			  ItalicCheck =
			    KeyCheckBox(GetString(gadgFontItalic),Italic,
			      ID_ITALIC),
			EndMember,
		      EndObject,
		    EndMember,
		    StartMember,
		      VGroupObject,
			Spacing(SizeY(2)),
			StartMember,
			  BoldCheck = KeyCheckBox(GetString(gadgFontBold),
			    Bold,ID_BOLD),
			EndMember,
			StartMember,
			  UnderCheck =
			    KeyCheckBox(GetString(gadgFontUnderline),ULine,
			      ID_ULINE),
			EndMember,
		      EndObject,
		    EndMember,
		    StartMember,
		      VGroupObject,
			Spacing(SizeY(2)),
			StartMember,
			  ExtCheck =
			    KeyCheckBox(GetString(gadgFontExtended),Extended,
			      ID_EXTEND),
			EndMember,
			StartMember,
			  RevCheck =
			    KeyCheckBox(GetString(gadgFontReversed),Reversed,
			      ID_REVERSE),
			EndMember,
		      EndObject,
		    EndMember,
		  EndObject,
		EndMember,
		StartMember,
		  HGroupObject,
		    Spacing(SizeX(8)),
		    StartMember,
		      AspectCycle = KeyPopCycle(GetString(gadgFontAspect),
			AspectLabels,Aspect,ID_ASPECT),
		    EndMember,
		    StartMember,
		      BlankButton = KeyButton(GetString(gadgFontBlank),
			ID_BLANK),
		      Weight(20),
		    EndMember,
		  EndObject,
		EndMember,
	      EndObject,
	    EndMember,
	    StartMember,
	      HGroupObject,
		Spacing(SizeX(8)),
		EqualWidth,
		StartMember,
		  accept = KeyButton(GetString(gadgAccept),ID_ACCEPT),
		EndMember,
        	StartMember,
		  cancel = KeyButton(GetString(gadgCancel),ID_CANCEL),
		EndMember,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	  EndObject,
	EndObject;
      if (ParamWndObj == NULL) ErrorCode(NEWWINDOW);
      SetLabelKey(ParamWndObj,accept,gadgAccept);
      SetLabelKey(ParamWndObj,cancel,gadgCancel);
      SetLabelKey(ParamWndObj,PropCycle,gadgFontType);
      SetLabelKey(ParamWndObj,HeightInt,gadgFontHeight);
      SetLabelKey(ParamWndObj,WidthInt,gadgFontWidth);
      SetLabelKey(ParamWndObj,BaseInt,gadgFontBaseline);
      SetLabelKey(ParamWndObj,SmearInt,gadgBoldSmear);
      SetLabelKey(ParamWndObj,NormalCheck,gadgFontNormal);
      SetLabelKey(ParamWndObj,BoldCheck,gadgFontBold);
      SetLabelKey(ParamWndObj,ItalicCheck,gadgFontItalic);
      SetLabelKey(ParamWndObj,UnderCheck,gadgFontUnderline);
      SetLabelKey(ParamWndObj,ExtCheck,gadgFontExtended);
      SetLabelKey(ParamWndObj,RevCheck,gadgFontReversed);
      SetLabelKey(ParamWndObj,AspectCycle,gadgFontAspect);
      SetLabelKey(ParamWndObj,BlankButton,gadgFontBlank);
      DoMethod(ParamWndObj,WM_TABCYCLE_ORDER,
	HeightInt,WidthInt,BaseInt,SmearInt,NULL);
    }

    SetAttrs(PropCycle,CYC_Active,Proportional,TAG_DONE);
    SetAttrs(HeightInt,STRINGA_LongVal,Height,TAG_DONE);
    SetAttrs(BaseInt,STRINGA_LongVal,Baseline,TAG_DONE);
    SetAttrs(WidthInt,STRINGA_LongVal,Width,TAG_DONE);
    SetAttrs(SmearInt,STRINGA_LongVal,Smear,TAG_DONE);
    SetAttrs(NormalCheck,GA_Selected,Normal,TAG_DONE);
    SetAttrs(ItalicCheck,GA_Selected,Italic,TAG_DONE);
    SetAttrs(BoldCheck,GA_Selected,Bold,TAG_DONE);
    SetAttrs(UnderCheck,GA_Selected,ULine,TAG_DONE);
    SetAttrs(ExtCheck,GA_Selected,Extended,TAG_DONE);
    SetAttrs(RevCheck,GA_Selected,Reversed,TAG_DONE);
    SetAttrs(AspectCycle,CYC_Active,Aspect,TAG_DONE);
    if ((ParamWnd = WindowOpen(ParamWndObj)) == NULL) ErrorCode(OPENWINDOW);
  }
}

void CloseEditFontWnd(BOOL obj)
{
  ClrWindowClose(&ParamWndObj,&ParamWnd);
  if (obj) ClrDisposeObject(&ParamWndObj);
}

HOOKPROTONHNO(TBCompareFunc, LONG, struct lvCompare *lvc)
{
  if (lvc->lvc_EntryA < lvc->lvc_EntryB) return -1;
  if (lvc->lvc_EntryA > lvc->lvc_EntryB) return 1;
  return 0;
}

HOOKPROTONHNO(TBResourceFunc, LONG, struct lvResource * lvr)
{
  if (lvr->lvr_Command == LVRC_MAKE) return (IPTR)lvr->lvr_Entry;
  return 0;
}

HOOKPROTONHNO(TBDisplayFunc, CONST_STRPTR, struct lvRender *lvr)
{
  switch ((IPTR)lvr->lvr_Entry)
  {
    case FTBAR_WIDTHL:
      return GetString(prefsDecWidth);
    case FTBAR_WIDTHR:
      return GetString(prefsIncWidth);
    case FTBAR_MOVEL:
      return GetString(prefsMoveLeft);
    case FTBAR_MOVER:
      return GetString(prefsMoveRight);
    case FTBAR_MOVEU:
      return GetString(prefsMoveUp);
    case FTBAR_MOVED:
      return GetString(prefsMoveDown);
    case FTBAR_KERN:
      return GetString(prefsKerning);
    case FTBAR_WIDTH:
      return GetString(prefsWidthWin);
    case FTBAR_ZOOMIN:
      return GetString(prefsZoomIn);
    case FTBAR_ZOOMOUT:
      return GetString(prefsZoomOut);
    case FTBAR_FLIPH:
      return GetString(prefsFlipHoriz);
    case FTBAR_FLIPV:
      return GetString(prefsFlipVert);
    default:
      return "";
  }
}

void OpenPrefsWnd(void)
{
Object *save, *use, *cancel, *select, *page;
static STRPTR width_labels[] = { "4","8","16","32",NULL };
static ULONG map[] = { CYC_Active,PAGE_Active,TAG_DONE };

/*
extern struct VectorItem LineArrowLeft[], LineArrowRight[];
extern struct VectorItem SolidArrowLeft[], SolidArrowRight[];
extern struct VectorItem SolidArrowUp[], SolidArrowDown[];
extern struct VectorItem KernImage[], WidthImage[];
extern struct VectorItem ZoomInImage[], ZoomOutImage[];
*/
  if (PrefsWnd)
  {
    WindowToFront(PrefsWnd);
    ActivateWindow(PrefsWnd);
  }
  else
  {
    if (PrefsWndObj == NULL)
    {
      MakeStaticHook(TBCompareRef, TBCompareFunc);
      MakeStaticHook(TBResourceRef, TBResourceFunc);
      MakeStaticHook(TBDisplayRef, TBDisplayFunc);
      PrefsPages[0] = GetString(msgPrefsScreen);
      PrefsPages[1] = GetString(msgPrefsToolBar);
      PrefsPages[2] = GetString(msgPrefsEdit);
      PrefsPages[3] = GetString(msgPrefsSelect);
      PrefsPages[4] = GetString(msgPrefsMisc);
      BorderLabels[0] = GetString(msgBorderNo);
      BorderLabels[1] = GetString(msgBorderYes);
      BorderLabels[2] = GetString(msgBorderGrid);
      SaveLabels[0] = GetString(msgPrefsEnv);
      SaveLabels[1] = GetString(msgPrefsProg);
      SetupMenus(Menus);
      if ((TBDisplayHook = AllocVec(sizeof(struct Hook),MEMF_CLEAR)) == NULL) ErrorCode(ALLOCVEC);
      InitHook(TBDisplayHook, TBDisplayRef, NULL);
      if ((TBResourceHook = AllocVec(sizeof(struct Hook),MEMF_CLEAR)) == NULL) ErrorCode(ALLOCVEC);
      InitHook(TBResourceHook, TBResourceRef, NULL);
      TBResourceHook->h_Entry = (HOOKFUNC)TBResourceFunc;
      if ((TBCompareHook = AllocVec(sizeof(struct Hook),MEMF_CLEAR)) == NULL) ErrorCode(ALLOCVEC);
      TBCompareHook->h_Entry = (HOOKFUNC)TBCompareFunc;
      InitHook(TBCompareHook, TBCompareRef, NULL);

      PrefsWndObj = WindowObject,
	WINDOW_Screen,Screen,
	WINDOW_SharedPort,WndMsgPort,
	WINDOW_MenuStrip,Menus,
	WINDOW_Title,GetString(msgPrefsTitle),
	WINDOW_HelpFile,NAME".guide",
	WINDOW_HelpNode,"prefs",
	WINDOW_ToolTicks,10,
	WINDOW_SmartRefresh,TRUE,
	WINDOW_SizeGadget,FALSE,
	WINDOW_CloseOnEsc,TRUE,
	WINDOW_MasterGroup,
	  VGroupObject,
	    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
	    GROUP_BackFill,SHINE_RASTER,
            StartMember,
	      select = CycleObject,
		ButtonFrame,
		CYC_Labels,PrefsPages,
		CYC_Popup,TRUE,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	    StartMember,
	      page = PageObject,

		PageMember,
		  VGroupObject,
		    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		    ButtonFrame,
		    FRM_Recessed,TRUE,
		    VarSpace(50),
		    StartMember,
		      HGroupObject,
			VarSpace(50),
			StartMember,
			  CustButton = KeyCheckBox(GetString(gadgCustomScr),
			    CustomScreen,ID_CUSTOM),
			EndMember,
			VarSpace(50),
		      EndObject,
		    EndMember,
		    StartMember,
		      HGroupObject,
			VarSpace(0),
			FixMinHeight,
			EqualHeight,
			StartMember,
			  ModeInfo = InfoObject,
			    UScoreLabel(GetString(gadgMode),'_'),
			    ButtonFrame,
			    FRM_Flags,FRF_RECESSED,
			    INFO_MinLines,1,
			    INFO_HorizOffset,3,
			    INFO_VertOffset,3,
			  EndObject,
			  FixMinHeight,
			EndMember,
			StartMember,
			  ModeButton = ButtonObject,
			    PopUp,
			    ButtonFrame,
			    GA_ID,ID_CHOOSEMODE,
			  EndObject,
			  FixMinWidth,
			EndMember,
		      EndObject,
		      FixMinHeight,
		    EndMember,
		    StartMember,
		      HGroupObject,
			VarSpace(0),
			FixMinHeight,
			EqualHeight,
			StartMember,
			  FontInfo = InfoObject,
			    UScoreLabel(GetString(gadgScreenFont),'_'),
			    ButtonFrame,
			    FRM_Flags,FRF_RECESSED,
			    INFO_MinLines,1,
			    INFO_HorizOffset,3,
			    INFO_VertOffset,3,
			  EndObject,
			  FixMinHeight,
			EndMember,
			StartMember,
			  FontButton = ButtonObject,
			    PopUp,
			    ButtonFrame,
			    GA_ID,ID_CHOOSEFONT,
			  EndObject,
			  FixMinWidth,
			EndMember,
		      EndObject,
		      FixMinHeight,
		    EndMember,
		    StartMember,
		      PubStr = StringObject,
			UScoreLabel(GetString(gadgPubScreen),'_'),
			RidgeFrame,
			STRINGA_MaxChars,MAXPUBSCREENNAME,
			GA_ID,ID_PUBNAME,
		      EndObject,
		      NoAlign,
		      FixMinHeight,
		    EndMember,
		    VarSpace(50),
		  EndObject,

		PageMember,
		  VGroupObject,
		    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		    ButtonFrame,
		    FRM_Recessed,TRUE,
		    StartMember,
		      HGroupObject,
			StartMember,
			  HGroupObject,
			    Spacing(SizeX(4)),
			    StartMember,
			      ToolBarOut = NewObject(LVClass,NULL,
				FL_SortDrops,1,
				BT_DragObject,1,
				BT_DropObject,1,
				LISTV_MultiSelect,1,
				LISTV_DisplayHook,TBDisplayHook,
				LISTV_ResourceHook,TBResourceHook,
				LISTV_CompareHook,TBCompareHook,
				BT_ToolTip,GetString(gadgToolBarOut),
				TAG_DONE),
			    EndMember,
			    StartMember,
			      ToolBarIn = NewObject(LVClass,NULL,
				FL_SortDrops,1,
				BT_DragObject,1,
				BT_DropObject,1,
				LISTV_MultiSelect,1,
				LISTV_DisplayHook,TBDisplayHook,
				LISTV_ResourceHook,TBResourceHook,
				LISTV_CompareHook,TBCompareHook,
				BT_ToolTip,GetString(gadgToolBarIn),
				TAG_DONE),
			    EndMember,
			  EndObject,
			EndMember,
		      EndObject,
		    EndMember,
		    StartMember,
		      HGroupObject,
			Spacing(SizeX(8)),
			VarSpace(100),
			StartMember,
			  VecInt = StringObject,
			    RidgeFrame,
			    UScoreLabel(GetString(gadgToolBarHeight),'_'),
			    STRINGA_LongVal,0,
			    STRINGA_MaxChars,5,
			    STRINGA_MinCharsVisible,5,
			    GA_ID,ID_VECHEIGHT,
			  EndObject,
			EndMember,
			StartMember,
			  VGroupObject,
			    VarSpace(60),
			    StartMember,
			      AutoCheck = KeyCheckBox(GetString(gadgAuto),
				FALSE,ID_AUTOHEIGHT),
			    EndMember,
			    VarSpace(50),
			  EndObject,
			EndMember,
			VarSpace(100),
		      EndObject,
		      FixMinHeight,
		    EndMember,
		  EndObject,

		PageMember,
		  VGroupObject,
		    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		    ButtonFrame,
		    FRM_Recessed,TRUE,
		    StartMember,
		      VGroupObject,
			HOffset(SizeX(8)),VOffset(SizeY(4)),
			Spacing(SizeY(2)),
			NeXTFrame, 
			FrameTitle(GetString(msgPixels)),
			StartMember,
			  HGroupObject,
			    Spacing(SizeX(8)),
			    EqualWidth,
			    StartMember,
			      PixelXInt = TabKeyInteger(
				GetString(gadgPixelWidth),Prefs.PixelX,3,
				ID_PIXELWIDTH),
			      FixMinHeight,
			    EndMember,
			    StartMember,
			      PixelYInt = TabKeyInteger(
				GetString(gadgPixelHeight),Prefs.PixelY,3,
				ID_PIXELHEIGHT),
			      FixMinHeight,
			    EndMember,
			  EndObject,
			  NoAlign,
			EndMember,
			StartMember,
			  HGroupObject,
			    Spacing(SizeX(8)),
			    StartMember,
			      PixelCycle = CycleObject,
				UScoreLabel(GetString(gadgPixelBorder),'_'),
				ButtonFrame,
				CYC_Labels,BorderLabels,
				CYC_Popup,TRUE,
				GA_ID,ID_PIXELBORDER,
			      EndObject,
			    EndMember,
			    StartMember,
			      VGroupObject,
				VarSpace(60),
				StartMember,
				  BLineCheck = KeyCheckBox(GetString(gadgShowBLine),FALSE,ID_SHOWBASELINE),
				EndMember,
				VarSpace(50),
			      EndObject,
			    EndMember,
			  EndObject,
			  NoAlign,
			EndMember,
		      EndObject,
		    EndMember,
		    StartMember,
		      HGroupObject,
			Spacing(SizeX(8)),
			VarSpace(50),
			StartMember,
			  MaxCheck = KeyCheckBox(GetString(gadgMaxWindow),FALSE,ID_MAXWIN),
			EndMember,
			StartMember,
			  ToggleCheck = KeyCheckBox(GetString(gadgToggle),TRUE,ID_TOGGLE),
			EndMember,
			VarSpace(50),
		      EndObject,
		    EndMember,
		  EndObject,

		PageMember,
		  VGroupObject,
		    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		    ButtonFrame,
		    FRM_Recessed,TRUE,
		    VarSpace(50),
		    StartMember,
		      HGroupObject,
			Spacing(SizeX(8)),
			VarSpace(50),
			StartMember,
			  SizeCheck = KeyCheckBox(GetString(gadgSizeWin),
			    FALSE,ID_CHARSIZE),
			EndMember,
			StartMember,
			  ShiftCheck = KeyCheckBox(GetString(gadgShift),
			    FALSE,ID_INVERTSHIFT),
			EndMember,
			VarSpace(50),
		      EndObject,
		    EndMember,
		    StartMember,
		      HGroupObject,
			Spacing(SizeX(8)),
			StartMember,
			  ChrWCycle = CycleObject,
			    UScoreLabel(GetString(gadgWidthCycle),'_'),
			    ButtonFrame,
			    CYC_Labels,width_labels,
			    CYC_Popup,TRUE,
			    GA_ID,ID_CHARWIDE,
			  EndObject,
			EndMember,
			StartMember,
			  ChrHInt = KeyInteger(GetString(gadgSelWinHeight),
			    0,3,ID_CHARHIGH),
			EndMember,
		      EndObject,
		    EndMember,
		    StartMember,
		      HGroupObject,
			FixMinHeight,
			EqualHeight,
			StartMember,
			  ChrFontInfo = InfoObject,
			    UScoreLabel(GetString(gadgSelWinFont),'_'),
			    ButtonFrame,
			    FRM_Flags,FRF_RECESSED,
			    INFO_MinLines,1,
			    INFO_HorizOffset,3,
			    INFO_VertOffset,3,
			  EndObject,
			  FixMinHeight,
			EndMember,
			StartMember,
			  ChrFontButton = ButtonObject,
			    PopUp,
			    ButtonFrame,
			    GA_ID,ID_CHOOSEFIXFONT,
			  EndObject,
			  FixMinWidth,
			EndMember,
		      EndObject,
		      FixMinHeight,
		    EndMember,
		    VarSpace(50),
		  EndObject,

		PageMember,
		  VGroupObject,
		    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		    ButtonFrame,
		    FRM_Recessed,TRUE,
		    VarSpace(50),
		    StartMember,
		      HGroupObject,
			HOffset(SizeX(8)),VOffset(SizeY(4)),
			Spacing(SizeX(4)),
			NeXTFrame, 
			FrameTitle(GetString(msgSavePrefsTo)),
			Place(PLACE_ABOVE),
			StartMember,
			  SaveCycle = CycleObject,
			    ButtonFrame,
			    CYC_Labels,SaveLabels,
			    CYC_Active,Prefs.SaveTo,
			    CYC_Popup,TRUE,
			    GA_ID,ID_SAVETO,
			  EndObject,
			EndMember,
		      EndObject,
		    EndMember,
		    StartMember,
		      HGroupObject,
			VarSpace(50),
			StartMember,
			  VGroupObject,
			    Spacing(SizeY(2)),
			    StartMember,
			      QuitCheck = KeyCheckBox(GetString(gadgQuit),FALSE,ID_CONFIRMQUIT),
			    EndMember,
			    StartMember,
			      QueryCheck = KeyCheckBox(GetString(gadgQueryWidth),FALSE,ID_QUERYWIDTH),
			    EndMember,
			  EndObject,
			EndMember,
			VarSpace(50),
		      EndObject,
		    EndMember,
		    VarSpace(50),
		  EndObject,

	      EndObject,
	    EndMember,
	    StartMember,
	      HGroupObject,
		Spacing(SizeX(8)),
		EqualWidth,
		StartMember,
		  save = KeyButton(GetString(gadgPrefsSave),ID_ACCEPT),
		EndMember,
		StartMember,
		  use = KeyButton(GetString(gadgPrefsUse),ID_USE),
		EndMember,
        	StartMember,
		  cancel = KeyButton(GetString(gadgCancel),ID_CANCEL),
		EndMember,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	  EndObject,
	EndObject;
      if (PrefsWndObj == NULL) ErrorCode(NEWWINDOW);

      SetLabelKey(PrefsWndObj,save,gadgPrefsSave);
      SetLabelKey(PrefsWndObj,use,gadgPrefsUse);
      SetLabelKey(PrefsWndObj,cancel,gadgCancel);

      SetLabelKey(PrefsWndObj,CustButton,gadgCustomScr);
      SetLabelKey(PrefsWndObj,ModeButton,gadgMode);
      SetLabelKey(PrefsWndObj,FontButton,gadgScreenFont);
      SetLabelKey(PrefsWndObj,PubStr,gadgPubScreen);

      SetLabelKey(PrefsWndObj,VecInt,gadgToolBarHeight);
      SetLabelKey(PrefsWndObj,AutoCheck,gadgAuto);

      SetLabelKey(PrefsWndObj,PixelXInt,gadgPixelWidth);
      SetLabelKey(PrefsWndObj,PixelYInt,gadgPixelHeight);
      SetLabelKey(PrefsWndObj,PixelCycle,gadgPixelBorder);
      SetLabelKey(PrefsWndObj,MaxCheck,gadgMaxWindow);
      SetLabelKey(PrefsWndObj,BLineCheck,gadgShowBLine);
      SetLabelKey(PrefsWndObj,ToggleCheck,gadgToggle);

      SetLabelKey(PrefsWndObj,SizeCheck,gadgSizeWin);
      SetLabelKey(PrefsWndObj,ShiftCheck,gadgShift);
      SetLabelKey(PrefsWndObj,ChrWCycle,gadgWidthCycle);
      SetLabelKey(PrefsWndObj,ChrHInt,gadgSelWinHeight);
      SetLabelKey(PrefsWndObj,ChrFontButton,gadgSelWinFont);

      GadgetKey(PrefsWndObj,SaveCycle,"v");
      SetLabelKey(PrefsWndObj,QuitCheck,gadgQuit);
      SetLabelKey(PrefsWndObj,QueryCheck,gadgQueryWidth);
      GadgetKey(PrefsWndObj,select,"\\");

      DoMethod(PrefsWndObj,WM_TABCYCLE_ORDER,PixelXInt,PixelYInt,NULL);
      AddMap(select,page,map);
    }

    SetAttrs(CustButton,GA_Selected,CustomScreen,TAG_DONE);
    SetAttrs(ModeButton,
      GA_Disabled,(CustomScreen != FALSE) ? FALSE : TRUE,TAG_DONE);
    SetAttrs(FontButton,
      GA_Disabled,(CustomScreen != FALSE) ? FALSE : TRUE,TAG_DONE);
    PrefScrModeID = Prefs.ScrModeID;
    PrefScrDepth = Prefs.ScrDepth;
    GetDisplayInfoData(NULL,(UBYTE *)&ModeIDName,sizeof(struct NameInfo),
      DTAG_NAME,PrefScrModeID);
    SetAttrs(ModeInfo,INFO_TextFormat,ModeIDName.Name,TAG_DONE);
    strcpy(PrefScrFontName,Prefs.ScrFontName);
    PrefScrFontHeight = Prefs.ScrFontHeight;
    WriteFont(PrefScrBuffer,Prefs.ScrFontName,Prefs.ScrFontHeight);
    SetAttrs(FontInfo,INFO_TextFormat,PrefScrBuffer,TAG_DONE);
    strcpy(PrefThisPubName,Prefs.ThisPubName);
    strcpy(PrefDefPubName,Prefs.DefPubName);
    SetAttrs(PubStr,STRINGA_TextVal,GetPubName(CustomScreen),TAG_DONE);

    ClearList(NULL,ToolBarOut);
    ClearList(NULL,ToolBarIn);
    AddToolBarList(FTBAR_WIDTHL);
    AddToolBarList(FTBAR_WIDTHR);
    AddToolBarList(FTBAR_MOVEL);
    AddToolBarList(FTBAR_MOVER);
    AddToolBarList(FTBAR_MOVEU);
    AddToolBarList(FTBAR_MOVED);
    AddToolBarList(FTBAR_KERN);
    AddToolBarList(FTBAR_WIDTH);
    AddToolBarList(FTBAR_ZOOMIN);
    AddToolBarList(FTBAR_ZOOMOUT);
    AddToolBarList(FTBAR_FLIPH);
    AddToolBarList(FTBAR_FLIPV);
    SetAttrs(ToolBarIn,FL_AcceptDrop,ToolBarOut,TAG_DONE);
    SetAttrs(ToolBarOut,FL_AcceptDrop,ToolBarIn,TAG_DONE);
    SetAttrs(AutoCheck,GA_Selected,AutoHeight,TAG_DONE);
    SetAttrs(VecInt,
      STRINGA_LongVal,Prefs.VecHeight,
      GA_Disabled,AutoHeight,TAG_DONE);

    SetAttrs(PixelXInt,STRINGA_LongVal,Prefs.PixelX,TAG_DONE);
    SetAttrs(PixelYInt,STRINGA_LongVal,Prefs.PixelY,TAG_DONE);
    SetAttrs(PixelCycle,CYC_Active,PixelBorder,TAG_DONE);
    SetAttrs(MaxCheck,GA_Selected,MaxWindow,TAG_DONE);
    SetAttrs(BLineCheck,GA_Selected,ShowBLine,TAG_DONE);
    SetAttrs(ToggleCheck,GA_Selected,Toggle,TAG_DONE);

    SetAttrs(SizeCheck,GA_Selected,Prefs.CharSize,TAG_DONE);
    SetAttrs(ShiftCheck,GA_Selected,InvertShift,TAG_DONE);
    SetAttrs(ChrWCycle,CYC_Active,(IPTR)Prefs.CharWidth,TAG_DONE);
    SetAttrs(ChrHInt,STRINGA_LongVal,Prefs.CharHeight,TAG_DONE);
    strcpy(PrefFixFontName,Prefs.FixedFontName);
    PrefFixFontHeight = Prefs.FixedFontHeight;
    WriteFont(PrefFixBuffer,Prefs.FixedFontName,Prefs.FixedFontHeight);
    SetAttrs(ChrFontInfo,INFO_TextFormat,PrefFixBuffer,TAG_DONE);

    SetAttrs(SaveCycle,CYC_Active,Prefs.SaveTo,TAG_DONE);
    SetAttrs(QuitCheck,GA_Selected,ConfirmQuit,TAG_DONE);
    SetAttrs(QueryCheck,GA_Selected,QueryWidth,TAG_DONE);
    if ((PrefsWnd = WindowOpen(PrefsWndObj)) == NULL) ErrorCode(OPENWINDOW);
  }
}

void ClosePrefsWnd(BOOL obj)
{
  ClrWindowClose(&PrefsWndObj,&PrefsWnd);
  if (obj)
  {
    ClrDisposeObject(&PrefsWndObj);
    FreeVec(TBDisplayHook);
    TBDisplayHook = 0;
    FreeVec(TBResourceHook);
    TBResourceHook = 0;
    FreeVec(TBCompareHook);
    TBCompareHook = 0;
  }
}

void OpenAssignWnd(void)
{
Object *assign, *assignadd, *cancel;

  if (AssignWnd)
  {
    WindowToFront(AssignWnd);
    ActivateWindow(AssignWnd);
  }
  else
  {
    if (AssignWndObj == NULL)
    {
      SetupMenus(Menus);
      AssignWndObj = WindowObject,
	WINDOW_Screen,Screen,
	WINDOW_SharedPort,WndMsgPort,
	WINDOW_MenuStrip,Menus,
	WINDOW_Title,GetString(msgFontDirTitle),
	WINDOW_HelpFile,NAME".guide",
	WINDOW_HelpNode,"dir",
	WINDOW_ScaleWidth,26,
	WINDOW_SmartRefresh,TRUE,
	WINDOW_SizeGadget,FALSE,
	WINDOW_CloseOnEsc,TRUE,
	WINDOW_MasterGroup,
	  VGroupObject,
	    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
	    GROUP_BackFill,SHINE_RASTER,
	    StartMember,
	      HGroupObject,
		HOffset(SizeX(8)),VOffset(SizeY(4)),
		EqualHeight,
		ButtonFrame,
		FRM_Recessed,TRUE,
		StartMember,
		  FontDirStr = StringObject,
		    RidgeFrame,
		    STRINGA_TextVal,FontPath,
		    STRINGA_MaxChars,255,
		    GA_ID,ID_ASSIGNSTR,
		  EndObject,
		EndMember,
		StartMember,
		  ButtonObject,
		    GetPath,
		    ButtonFrame,
		    GA_ID,ID_ASSIGNPATH,
		  EndObject,
		  FixMinWidth,
		EndMember,
	      EndObject,
	    EndMember,
	    StartMember,
	      HGroupObject,
		Spacing(SizeX(8)),
		StartMember,
		  assign = KeyButton(GetString(gadgAssign),ID_ASSIGN),
		EndMember,
        	StartMember,
		  assignadd = KeyButton(GetString(gadgAssignAdd),
		    ID_ASSIGNADD),
		EndMember,
        	StartMember,
		  cancel = KeyButton(GetString(gadgCancel),ID_CANCEL),
		EndMember,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	  EndObject,
	EndObject;
      if (AssignWndObj == NULL) ErrorCode(NEWWINDOW);
      SetLabelKey(AssignWndObj,assign,gadgAssign);
      SetLabelKey(AssignWndObj,assignadd,gadgAssignAdd);
      SetLabelKey(AssignWndObj,cancel,gadgCancel);

      DirReqObj = FileReqObject,
	ASLFR_TitleText,GetString(msgFontDirTitle),
	ASLFR_DrawersOnly,TRUE,
	ASLFR_Window,FontWnd,
      EndObject;
      if (DirReqObj == NULL) ErrorCode(NEWFILE);
    }

    SetAttrs(FontDirStr,STRINGA_TextVal,FontPath,TAG_DONE);
    if ((AssignWnd = WindowOpen(AssignWndObj)) == NULL)
      ErrorCode(OPENWINDOW);
  }
}

void CloseAssignWnd(BOOL obj)
{
  ClrWindowClose(&AssignWndObj,&AssignWnd);
  if (obj) ClrDisposeObject(&AssignWndObj);
}

void OpenEditCharWnd(void)
{
Object *accept, *cancel;

  if (EditCharWnd)
  {
    WindowToFront(EditCharWnd);
    ActivateWindow(EditCharWnd);
  }
  else
  {
    if (EditCharWndObj == NULL)
    {
      SetupMenus(Menus);
      EditCharWndObj = WindowObject,
	WINDOW_Screen,Screen,
	WINDOW_SharedPort,WndMsgPort,
	WINDOW_MenuStrip,Menus,
	WINDOW_Title,GetString(msgEditCharTitle),
	WINDOW_HelpFile,NAME".guide",
	WINDOW_HelpNode,"editchar",
	WINDOW_SmartRefresh,TRUE,
	WINDOW_SizeGadget,FALSE,
	WINDOW_CloseOnEsc,TRUE,
	WINDOW_MasterGroup,
	  VGroupObject,
	    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
	    GROUP_BackFill,SHINE_RASTER,
	    StartMember,
	      HGroupObject,
		HOffset(SizeX(8)),VOffset(SizeY(4)),
		EqualHeight,
		ButtonFrame,
		FRM_Recessed,TRUE,
		StartMember,
		  EditCharNum = StringObject,
		    RidgeFrame,
		    Label(GetString(gadgCharNum)),
		    STRINGA_LongVal,0,
		    STRINGA_IntegerMin,0,
		    STRINGA_IntegerMax,255,
		    STRINGA_MaxChars,3,
		    STRINGA_MinCharsVisible,4,
		    GA_ID,ID_EDITCHARNUM,
		  EndObject,
		EndMember,
	      EndObject,
	    EndMember,
	    StartMember,
	      HGroupObject,
		Spacing(SizeX(8)),
		StartMember,
		  accept = KeyButton(GetString(gadgAccept),ID_ACCEPT),
		EndMember,
        	StartMember,
		  cancel = KeyButton(GetString(gadgCancel),ID_CANCEL),
		EndMember,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	  EndObject,
	EndObject;
      if (EditCharWndObj == NULL) ErrorCode(NEWWINDOW);
      SetLabelKey(EditCharWndObj,accept,gadgAccept);
      SetLabelKey(EditCharWndObj,cancel,gadgCancel);
      SetAttrs(EditCharNum,STRINGA_BufferPos,1);
    }

    if ((EditCharWnd = WindowOpen(EditCharWndObj)) == NULL)
      ErrorCode(OPENWINDOW);
  }
  ActivateGadget((struct Gadget *)EditCharNum,EditCharWnd,NULL);
}

void CloseEditCharWnd(BOOL obj)
{
  ClrWindowClose(&EditCharWndObj,&EditCharWnd);
  if (obj) ClrDisposeObject(&EditCharWndObj);
}

void OpenWidthWnd(struct CharNode *node)
{
Object *accept, *cancel;
static ULONG lmap[] = { PGA_Top,INDIC_Level,TAG_END };
static ULONG rmap[] = { SCRL_Right,INDIC_Level,TAG_END };

  if (node->chln_Number < 256)
    sprintf(WidthTitle,GetString(msgWidthTitle),
      node->chln_Number,node->chln_Character->chr_Width);
  else
    sprintf(WidthTitle,GetString(msgWidthTitleBlank),
      node->chln_Character->chr_Width);
  WidthNode = node;
  WidthChar = node->chln_Character;

  if (WidthWnd)
  {
    SetGadgetAttrs((struct Gadget *)ChrWidthScroll,WidthWnd,NULL,
      PGA_Top,0,
      PGA_Total,1,
      PGA_Visible,1,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)ChrWidthInt,WidthWnd,NULL,
      STRINGA_LongVal,WidthChar->chr_Width,TAG_DONE);
    SetAttrs(WidthWndObj,WINDOW_Title,WidthTitle,TAG_DONE);
    WindowToFront(WidthWnd);
    ActivateWindow(WidthWnd);
  }
  else
  {
    if (WidthWndObj == NULL)
    {
      SetupMenus(CharMenus);
      WidthWndObj = WindowObject,
	WINDOW_Screen,Screen,
	WINDOW_SharedPort,WndMsgPort,
	WINDOW_MenuStrip,CharMenus,
	WINDOW_HelpFile,NAME".guide",
	WINDOW_HelpNode,"width",
	WINDOW_SmartRefresh,TRUE,
	WINDOW_SizeGadget,FALSE,
	WINDOW_CloseOnEsc,TRUE,
	WINDOW_MasterGroup,
	  VGroupObject,
	    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
	    GROUP_BackFill,SHINE_RASTER,
	    StartMember,
	      VGroupObject,
		HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		ButtonFrame,
		FRM_Recessed,TRUE,
		StartMember,
		  ChrWidthInt = KeyInteger(GetString(gadgNewWidth),
		    WidthChar->chr_Width,5,ID_CHARWIDTH),
		EndMember,
		StartMember,
		  HGroupObject,
		    Spacing(SizeX(8)),
		    StartMember,
		      LeftInd = IndicatorObject,
			INDIC_Min,0,
			INDIC_Max,999,
			INDIC_FormatString,GetString(msgLeftOffset),
		      EndObject,
		    EndMember,
		    StartMember,
		      RightInd = IndicatorObject,
			INDIC_Min,0,
			INDIC_Max,999,
			INDIC_FormatString,GetString(msgRightOffset),
		      EndObject,
		    EndMember,
		  EndObject,
		EndMember,
		StartMember,
		  ChrWidthScroll = NewObject(SlideClass,NULL,
		    PGA_Freedom,FREEHORIZ,
		    PGA_Arrows,FALSE,
		    GA_ID,ID_WIDTHPOS,
		  EndObject,
		EndMember,
	      EndObject,
	    EndMember,
	    StartMember,
	      HGroupObject,
		Spacing(SizeX(8)),
		StartMember,
		  accept = KeyButton(GetString(gadgAccept),ID_ACCEPT),
		EndMember,
        	StartMember,
		  cancel = KeyButton(GetString(gadgCancel),ID_CANCEL),
		EndMember,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	  EndObject,
	EndObject;
      if (WidthWndObj == NULL) ErrorCode(NEWWINDOW);
      SetLabelKey(WidthWndObj,accept,gadgAccept);
      SetLabelKey(WidthWndObj,cancel,gadgCancel);
      SetLabelKey(WidthWndObj,ChrWidthInt,gadgNewWidth);
      AddMap(ChrWidthScroll,LeftInd,lmap);
      AddMap(ChrWidthScroll,RightInd,rmap);
    }

    SetAttrs(ChrWidthScroll,
      PGA_Top,0,
      PGA_Total,1,
      PGA_Visible,1,TAG_DONE);
    SetAttrs(ChrWidthInt,STRINGA_LongVal,WidthChar->chr_Width,TAG_DONE);
    SetAttrs(WidthWndObj,WINDOW_Title,WidthTitle,TAG_DONE);
    if ((WidthWnd = WindowOpen(WidthWndObj)) == NULL) ErrorCode(OPENWINDOW);
  }
}

void CloseWidthWnd(BOOL obj)
{
  ClrWindowClose(&WidthWndObj,&WidthWnd);
  if (obj) ClrDisposeObject(&WidthWndObj);
  WidthNode = NULL; WidthChar = NULL;
}

void OpenKernWnd(struct CharNode *node)
{
Object *accept, *cancel;

  if (node->chln_Number < 256)
    sprintf(KernTitle,GetString(msgKernTitle),node->chln_Number);
  else
    sprintf(KernTitle,GetString(msgKernTitleBlank));
  KernNode = node;
  KernChar = node->chln_Character;

  if (KernWnd)
  {
    SetAttrs(KernWndObj,WINDOW_Title,KernTitle,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)SpaceObj,KernWnd,NULL,
      STRINGA_LongVal,SpaceTable[node->chln_Number],TAG_DONE);
    SetGadgetAttrs((struct Gadget *)KernObj,KernWnd,NULL,
      STRINGA_LongVal,KernTable[node->chln_Number],TAG_DONE);
    WindowToFront(KernWnd);
    ActivateWindow(KernWnd);
  }
  else
  {
    if (KernWndObj == NULL)
    {
      SetupMenus(CharMenus);
      KernWndObj = WindowObject,
	WINDOW_Screen,Screen,
	WINDOW_SharedPort,WndMsgPort,
	WINDOW_MenuStrip,CharMenus,
	WINDOW_HelpFile,NAME".guide",
	WINDOW_HelpNode,"kern",
	WINDOW_ScaleWidth,11,
	WINDOW_SmartRefresh,TRUE,
	WINDOW_SizeGadget,FALSE,
	WINDOW_CloseOnEsc,TRUE,
	WINDOW_MasterGroup,
	  VGroupObject,
	    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
	    GROUP_BackFill,SHINE_RASTER,
	    StartMember,
	      VGroupObject,
		HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		ButtonFrame,
		FRM_Recessed,TRUE,
		StartMember,
		  SpaceObj = TabKeyInteger(GetString(gadgSpacing),0,5,
		    ID_SPACE),
		EndMember,
		StartMember,
		  KernObj = TabKeyInteger(GetString(gadgKerning),0,5,
		    ID_KERN),
		EndMember,
	      EndObject,
	    EndMember,
	    StartMember,
	      HGroupObject,
		Spacing(SizeX(8)),
		StartMember,
		  accept = KeyButton(GetString(gadgAccept),ID_ACCEPT),
		EndMember,
        	StartMember,
		  cancel = KeyButton(GetString(gadgCancel),ID_CANCEL),
		EndMember,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	  EndObject,
	EndObject;
      if (KernWndObj == NULL) ErrorCode(NEWWINDOW);
      SetLabelKey(KernWndObj,accept,gadgAccept);
      SetLabelKey(KernWndObj,cancel,gadgCancel);
      SetLabelKey(KernWndObj,SpaceObj,gadgSpacing);
      SetLabelKey(KernWndObj,KernObj,gadgKerning);
      DoMethod(KernWndObj,WM_TABCYCLE_ORDER,SpaceObj,KernObj,NULL);
    }

    SetAttrs(KernWndObj,WINDOW_Title,KernTitle,TAG_DONE);
    SetAttrs(SpaceObj,
      STRINGA_LongVal,SpaceTable[node->chln_Number],TAG_DONE);
    SetAttrs(KernObj,STRINGA_LongVal,KernTable[node->chln_Number],TAG_DONE);
    if ((KernWnd = WindowOpen(KernWndObj)) == NULL) ErrorCode(OPENWINDOW);
  }
}

void CloseKernWnd(BOOL obj)
{
  ClrWindowClose(&KernWndObj,&KernWnd);
  if (obj) ClrDisposeObject(&KernWndObj);
  KernNode = NULL; KernChar = NULL;
}

void OpenSaveWnd(void)
{
Object *save, *cancel;
ULONG satan = 666;

  if (SaveWnd)
  {
    WindowToFront(SaveWnd);
    ActivateWindow(SaveWnd);
  }
  else
  {
    if (SaveWndObj == NULL)
    {
      SaveWndObj = WindowObject,
	WINDOW_Screen,Screen,
	WINDOW_SharedPort,WndMsgPort,
	WINDOW_Title,GetString(msgSaveTitle),
	WINDOW_HelpFile,NAME".guide",
	WINDOW_HelpNode,"save",
	WINDOW_SmartRefresh,TRUE,
	WINDOW_SizeGadget,FALSE,
	WINDOW_RMBTrap,TRUE,
	WINDOW_CloseOnEsc,TRUE,
	WINDOW_MasterGroup,
	  VGroupObject,
	    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
	    GROUP_BackFill,SHINE_RASTER,
	    StartMember,
	      VGroupObject,
		HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(2)),
		ButtonFrame,
		FRM_Recessed,TRUE,
		StartMember,
		  HGroupObject,
		    VarSpace(0),
		    StartMember,
		      SaveDirStr = StringObject,
			RidgeFrame,
			UScoreLabel(GetString(gadgFontDir),'_'),
			STRINGA_MaxChars,255,
			GA_TabCycle,TRUE,
			GA_ID,ID_SAVEDIR,
		      EndObject,
		    EndMember,
		    StartMember,
		      ButtonObject,
			GetPath,
			ButtonFrame,
			GA_ID,ID_GETSAVEDIR,
		      EndObject,
		      FixMinWidth,
		    EndMember,
		  EndObject,
		  NoAlign,
		EndMember,
		StartMember,
		  HGroupObject,
		    Spacing(SizeX(8)),
		    StartMember,
		      FontNameStr = StringObject,
			RidgeFrame,
			UScoreLabel(GetString(gadgFontName),'_'),
			STRINGA_MaxChars,255,
			GA_TabCycle,TRUE,
			GA_ID,ID_FONTNAME,
		      EndObject,
		    EndMember,
		    StartMember,
		      SizeInfo = InfoObject,
			ButtonFrame,
			FRM_Flags,FRF_RECESSED,
			Label(GetString(msgSize)),
			INFO_TextFormat,"\33c%.2ld",
			INFO_Args,&satan,
			INFO_MinLines,1,
			INFO_HorizOffset,6,
			INFO_VertOffset,3,
			INFO_FixTextWidth,TRUE,
		      EndObject,
		      Weight(10),
		    EndMember,
		  EndObject,
		EndMember,
		StartMember,
		  HGroupObject,
		    Spacing(SizeX(8)),
		    StartMember,
		      FirstNum = StringObject,
			RidgeFrame,
			UScoreLabel(GetString(gadgFirst),'_'),
			STRINGA_LongVal,0,
			STRINGA_MaxChars,3,
			STRINGA_MinCharsVisible,4,
			GA_TabCycle,TRUE,
			GA_ID,ID_FIRST,
		      EndObject,
		    EndMember,
		    StartMember,
		      LastNum = StringObject,
			RidgeFrame,
			UScoreLabel(GetString(gadgLast),'_'),
			STRINGA_LongVal,0,
			STRINGA_MaxChars,3,
			STRINGA_MinCharsVisible,4,
			GA_TabCycle,TRUE,
			GA_ID,ID_LAST,
		      EndObject,
		    EndMember,
		    StartMember,
		      VGroupObject,
			VarSpace(60),
			StartMember,
			  TableCheck = KeyCheckBox(GetString(gadgKernInfo),
			    FALSE,ID_KERNTABLE),
			EndMember,
			VarSpace(50),
		      EndObject,
		    EndMember,
		  EndObject,
		EndMember,
	      EndObject,
	    EndMember,
	    StartMember,
	      HGroupObject,
		Spacing(SizeX(8)),
		StartMember,
		  save = KeyButton(GetString(gadgSave),ID_ACCEPT),
		EndMember,
        	StartMember,
		  cancel = KeyButton(GetString(gadgCancel),ID_CANCEL),
		EndMember,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	  EndObject,
	EndObject;
      if (SaveWndObj == NULL) ErrorCode(NEWWINDOW);
      SetLabelKey(SaveWndObj,save,gadgSave);
      SetLabelKey(SaveWndObj,cancel,gadgCancel);
      SetLabelKey(SaveWndObj,SaveDirStr,gadgFontDir);
      SetLabelKey(SaveWndObj,FontNameStr,gadgFontName);
      SetLabelKey(SaveWndObj,FirstNum,gadgFirst);
      SetLabelKey(SaveWndObj,LastNum,gadgLast);
      SetLabelKey(SaveWndObj,TableCheck,gadgKernInfo);
      DoMethod(SaveWndObj,WM_TABCYCLE_ORDER,
	SaveDirStr,FontNameStr,FirstNum,LastNum,NULL);

      SaveReqObj = FileReqObject,
	ASLFR_TitleText,GetString(msgFontDirTitle),
	ASLFR_DrawersOnly,TRUE,
      EndObject;
      if (SaveReqObj == NULL) ErrorCode(NEWFILE);
    }

    SleepWindows();
    SetAttrs(SaveDirStr,STRINGA_TextVal,SavePath,TAG_DONE);
    SetAttrs(FontNameStr,STRINGA_TextVal,FontName,TAG_DONE);
    SetAttrs(FirstNum,STRINGA_LongVal,FirstChar,TAG_DONE);
    SetAttrs(LastNum,STRINGA_LongVal,LastChar,TAG_DONE);
    SetAttrs(SizeInfo,INFO_Args,&Height,TAG_DONE);
    SetAttrs(TableCheck,
      GA_Disabled,Proportional == TRUE,
      GA_Selected,(Proportional == TRUE) || (UseTable == TRUE),TAG_DONE);
    if ((SaveWnd = WindowOpen(SaveWndObj)) == NULL) ErrorCode(OPENWINDOW);
  }
}

void CloseSaveWnd(void)
{
  WakeWindows();
  ClrWindowClose(&SaveWndObj,&SaveWnd);
  ClrDisposeObject(&SaveWndObj);
}

void OpenAboutWnd(void)
{
static struct EasyStruct req =
  { sizeof(struct EasyStruct),0,NAME,
    NAME" "VERSION" © 1995-2000 David Kinder\n\n"
    "%s\n\n%s%s: %ld Kb\n%s: %ld Kb",NULL };

  SleepWindows();
  req.es_GadgetFormat = GetString(msgContinue);
  EasyRequest(FontWnd,&req,NULL,
#ifdef __AROS__
    "<no textfield.gadget in AROS>",
#else
    TEXTFIELD_GetCopyright(),
#endif
    GetString(msgTranslator),
    GetString(msgFreeChip),AvailMem(MEMF_CHIP)/1024,
    GetString(msgFreeFast),AvailMem(MEMF_FAST)/1024);
  WakeWindows();
}

void PutPositive(struct Window *wnd, CONST_STRPTR name, LONG value, LONG min,
  LONG *dest)
{
  if (value > min)
  {
    *dest = value;
  }
  else
  {
    ShowReq(GetString(msgPutPositive),GetString(msgContinue),name,min);
    FlagError = TRUE;
  }
}

void NewHeights(void)
{
struct Character *chr;
ULONG i;
UBYTE *new_char;

  for (i = 0; i < 257; i++)
  {
    chr = CharBuffer+i;
    if (chr->chr_Data)
    {
      if ((new_char = AllocVec(chr->chr_Width*Height,MEMF_CLEAR)) == NULL)
	ErrorCode(ALLOCVEC);
      CopyMem(chr->chr_Data,new_char,
	MIN(chr->chr_Height,Height)*chr->chr_Width);
      FreeVec(chr->chr_Data);
      chr->chr_Data = new_char;
    }
    chr->chr_Height = Height;
  }
  sprintf(WinTitle,"%s/%ld",FontName,(long)Height);
  SetAttrs(FontWndObj,WINDOW_Title,WinTitle,TAG_DONE);
}

void NewWidths(void)
{
struct Character *chr;
ULONG i;

  for (i = 0; i < 257; i++)
  {
    chr = CharBuffer+i;
    if (chr->chr_Data) ChangeCharWidth(chr,Width,0,0);
    chr->chr_Width = Width;
  }
}

void ChangeCharWidth(struct Character *chr, ULONG width, ULONG newoffset,
  ULONG oldoffset)
{
ULONG j;
UBYTE *data = NULL;

  if ((width > 0) && (chr->chr_Data))
  {
    if ((data = AllocVec(width*Height,MEMF_CLEAR)) == NULL)
      ErrorCode(ALLOCVEC);
    for (j = 0; j < Height; j++)
      CopyMem(chr->chr_Data+(j*chr->chr_Width)+oldoffset,
      data+(j*width)+newoffset,MIN(chr->chr_Width,width));
  }
  if (chr->chr_Data) FreeVec(chr->chr_Data);
  if (Reversed == FALSE)
    SpaceTable[chr-CharBuffer] += (width-chr->chr_Width);
  chr->chr_Data = data;
  chr->chr_Width = width;
  CheckWidthKern(chr);
}

void CheckWidthKern(struct Character *chr)
{
  if (chr == WidthChar)
  {
    if (WidthNode->chln_Number < 256)
      sprintf(WidthTitle,GetString(msgWidthTitle),
        WidthNode->chln_Number,WidthChar->chr_Width);
    else
      sprintf(WidthTitle,GetString(msgWidthTitleBlank),
	WidthChar->chr_Width);
    SetGadgetAttrs((struct Gadget *)ChrWidthScroll,WidthWnd,NULL,
      PGA_Top,0,
      PGA_Total,1,
      PGA_Visible,1,TAG_DONE);
    SetGadgetAttrs((struct Gadget *)ChrWidthInt,WidthWnd,NULL,
      STRINGA_LongVal,WidthChar->chr_Width,TAG_DONE);
    SetAttrs(WidthWndObj,WINDOW_Title,WidthTitle,TAG_DONE);
  }
  if (chr == KernChar)
    SetGadgetAttrs((struct Gadget *)SpaceObj,KernWnd,NULL,
      STRINGA_LongVal,SpaceTable[KernNode->chln_Number],TAG_DONE);
}

void SavePrefs(char *filename)
{
BPTR file;

  SleepWindows();
  Prefs.Flags = 0;
  if (NewCustom) Prefs.Flags |= PREFS_CUSTOMSCREEN;
  if (MaxWindow) Prefs.Flags |= PREFS_MAXWINDOW;
  if (ConfirmQuit) Prefs.Flags |= PREFS_CONFIRMQUIT;
  if (QueryWidth) Prefs.Flags |= PREFS_QUERYWIDTH;
  if (ShowBLine) Prefs.Flags |= PREFS_SHOWBASELINE;
  if (AutoHeight) Prefs.Flags |= PREFS_AUTOTBARHEIGHT;
  if (InvertShift) Prefs.Flags |= PREFS_INVERTSHIFT;
  if (Toggle == 0) Prefs.Flags |= PREFS_NOTOGGLE;
  switch (PixelBorder)
  {
    case 1:
      Prefs.Flags |= PREFS_PIXELBORDER;
      break;
    case 2:
      Prefs.Flags |= PREFS_PIXELGRID;
      break;
  }
  if ((file = Open(filename,MODE_NEWFILE)))
  {
    Write(file,&Prefs,sizeof(struct TFPreferences));
    Close(file);
  }
  else ShowReq(GetString(msgPrefsSaveError),GetString(msgContinue),filename);
  WakeWindows();
}

void LoadPrefs(void)
{
BPTR file;

  if ((file = Open("ENV:"NAME".prefs",MODE_OLDFILE)) == 0)
    file = Open("PROGDIR:"NAME".prefs",MODE_OLDFILE);
  if (file)
  {
    Read(file,&Prefs,sizeof(struct TFPreferences));
    Close(file);
    if (Prefs.Version >= 1)
    {
      CustomScreen = (Prefs.Flags & PREFS_CUSTOMSCREEN) ? TRUE : FALSE;
      MaxWindow = (Prefs.Flags & PREFS_MAXWINDOW) ? TRUE : FALSE;
      ConfirmQuit = (Prefs.Flags & PREFS_CONFIRMQUIT) ? TRUE : FALSE;
      QueryWidth = (Prefs.Flags & PREFS_QUERYWIDTH) ? TRUE : FALSE;
      ShowBLine = (Prefs.Flags & PREFS_SHOWBASELINE) ? TRUE : FALSE;
      AutoHeight = (Prefs.Flags & PREFS_AUTOTBARHEIGHT) ? TRUE : FALSE;
      InvertShift = (Prefs.Flags & PREFS_INVERTSHIFT) ? TRUE : FALSE;
      Toggle = (Prefs.Flags & PREFS_NOTOGGLE) ? FALSE : TRUE;
      PixelBorder = 0;
      if (Prefs.Flags & PREFS_PIXELBORDER) PixelBorder = 1;
      if (Prefs.Flags & PREFS_PIXELGRID) PixelBorder = 2;
    }
    Prefs.Version = CURRENT_PREFS;
  }
}

void FlushAllFonts(void)
{
UBYTE *free;

  if ((free = AllocMem(~0,MEMF_ANY)) != NULL) FreeMem(free,~0);
}

void GetScreenRatio(struct Screen *scr)
{
IPTR  vti[4];
struct ViewPortExtra *vpe;

  ScaleX = 1000;
  ScaleY = 1000;
  ScrWidth = scr->Width;
  ScrHeight = scr->Height;
  if (scr->ViewPort.ColorMap)
  {
    vti[0] = VTAG_VIEWPORTEXTRA_GET;
    vti[1] = 0;
    vti[2] = VTAG_END_CM;
    vti[3] = 0;
    if (VideoControl(scr->ViewPort.ColorMap,(struct TagItem *)vti) == 0)
    {
      vpe = (struct ViewPortExtra *)vti[1];
      ScrWidth = vpe->DisplayClip.MaxX - vpe->DisplayClip.MinX + 1;
      ScrHeight = vpe->DisplayClip.MaxY - vpe->DisplayClip.MinY + 1;
      ScaleX = MAX((ScrWidth*1000)/640,1000);
      ScaleY = MAX((ScrHeight*1000)/256,1000);
    }
  }
}

LONG SizeX(LONG x)
{
  return ((x*ScaleX)/1000);
}

LONG SizeY(LONG y)
{
  return ((y*ScaleY)/1000);
}

void WriteFont(char *buffer, char *name, UWORD height)
{
char *endptr;

  strcpy(buffer,name);
  if ((endptr = strstr(buffer,".font"))) *endptr = 0;
  sprintf(buffer+strlen(buffer),"/%d",height);
}

char *GetPubName(BOOL screen)
{
  return (screen ? PrefThisPubName : PrefDefPubName);
}

void CopyPubName(void)
{
IPTR  custom;
char *name;

  GetAttr(GA_Selected,CustButton,&custom);
  GetAttr(STRINGA_TextVal,PubStr,(IPTR *)&name);
  strcpy(custom ? PrefThisPubName : PrefDefPubName,name);
}

void WarnVisitors(void)
{
struct bguiRequest *visitors;

  if ((visitors = AllocVec(sizeof(struct bguiRequest),MEMF_CLEAR)))
  {
    visitors->br_Flags = BREQF_FAST_KEYS;
    visitors->br_Title = NAME;
    visitors->br_GadgetFormat = GetString(msgContinue);
    visitors->br_TextFormat = GetString(msgVisitors);
    visitors->br_ReqPos = POS_TOPLEFT;
    visitors->br_Screen = Screen;
    BGUI_Request(NULL,visitors);
    FreeVec(visitors);
  }
}

void CheckQuit(void)
{
  if (ConfirmQuit == FALSE) Quit();
  if (ShowReq(GetString(msgCheckQuit),GetString(msgQuitCancel),
    DataChanged ? GetString(msgCharsChanged) : (STRPTR)"") == 1) Quit();
}

void AddToolBarList(UWORD entry)
{
  AddEntry(NULL,(Prefs.ToolBar & entry) ? ToolBarIn : ToolBarOut,entry,LVAP_SORTED);
}

void CascadeWindows(void)
{
struct CharNode *node;
struct IBox bounds;
WORD x,y;

  x = CharSize.Left;
  y = CharSize.Top;
  node = (struct CharNode *)CharWndList->lh_Head;
  while (node->chln_Node.ln_Succ)
  {
    GetAttr(WINDOW_Bounds,node->chln_Object,(IPTR *)&bounds);
    if (y+bounds.Height > node->chln_Window->WScreen->Height)
    {
      x += node->chln_Window->RPort->TxWidth*3;
      y = CharSize.Top;
    }
    bounds.Left = x;
    bounds.Top = y;
    SetAttrs(node->chln_Object,WINDOW_Bounds,&bounds,TAG_DONE);
    WindowToFront(node->chln_Window);
    ActivateWindow(node->chln_Window);
    x += node->chln_Window->BorderLeft;
    y += node->chln_Window->BorderTop;
    node = (struct CharNode *)node->chln_Node.ln_Succ;
  }
}
