/*********************/
/*		     */
/* Character editing */
/*		     */
/*********************/

#include "typeface.h"

struct Window *ResetKernWnd;
Object *ResetKernWndObj, *ResetSpaceInt, *ResetKernInt;

ULONG PixelBorder = 1;
extern struct TFPreferences Prefs;

struct VectorItem LineArrowLeft[] =
  { {11,10,VIF_SCALE},
    {SHADOWPEN,0,VIF_DRIPEN},
     {2, 5,VIF_MOVE},
     {9, 2,VIF_DRAW},
     {9, 8,VIF_DRAW},
     {2, 5,VIF_DRAW},
     {0, 0,VIF_LASTITEM} };
struct VectorItem LineArrowRight[] =
  { {11,10,VIF_SCALE},
    {SHADOWPEN,0,VIF_DRIPEN},
     {9, 5,VIF_MOVE},
     {2, 2,VIF_DRAW},
     {2, 8,VIF_DRAW},
     {9, 5,VIF_DRAW},
     {0, 0,VIF_LASTITEM} };
struct VectorItem SolidArrowLeft[] =
  { {11,10,VIF_SCALE},
    {SHADOWPEN,0,VIF_AOLDRIPEN},
     {0, 0,VIF_AREASTART},
     {2, 5,VIF_MOVE},
     {9, 2,VIF_DRAW},
     {9, 8,VIF_DRAW},
     {2, 5,VIF_DRAW},
     {0, 0,VIF_AREAEND},
     {0, 0,VIF_LASTITEM} };
struct VectorItem SolidArrowRight[] =
  { {11,10,VIF_SCALE},
    {SHADOWPEN,0,VIF_AOLDRIPEN},
     {0, 0,VIF_AREASTART},
     {9, 5,VIF_MOVE},
     {2, 2,VIF_DRAW},
     {2, 8,VIF_DRAW},
     {9, 5,VIF_DRAW},
     {0, 0,VIF_AREAEND},
     {0, 0,VIF_LASTITEM} };
struct VectorItem SolidArrowUp[] =
  { {22,10,VIF_SCALE},
    {SHADOWPEN,0,VIF_AOLDRIPEN},
     {0, 0,VIF_AREASTART},
    {11, 2,VIF_MOVE},
     {5, 8,VIF_DRAW},
    {17, 8,VIF_DRAW},
    {11, 2,VIF_DRAW},
     {0, 0,VIF_AREAEND},
     {0, 0,VIF_LASTITEM} };
struct VectorItem SolidArrowDown[] =
  { {22,11,VIF_SCALE},
    {SHADOWPEN,0,VIF_AOLDRIPEN},
     {0, 0,VIF_AREASTART},
    {11, 8,VIF_MOVE},
     {5, 2,VIF_DRAW},
    {17, 2,VIF_DRAW},
    {11, 8,VIF_DRAW},
     {0, 0,VIF_AREAEND},
     {0, 0,VIF_LASTITEM} };
struct VectorItem KernImage[] =
  { {22,11,VIF_SCALE},
    {SHINEPEN,0,VIF_AOLDRIPEN},
    {SHINEPEN,0,VIF_DRIPEN},
     {0, 0,VIF_AREASTART},
     {4, 2,VIF_MOVE},
     {4, 8,VIF_DRAW},
    {10, 8,VIF_DRAW},
     {4, 2,VIF_DRAW},
     {0, 0,VIF_AREAEND},
    {SHADOWPEN,0,VIF_AOLDRIPEN},
    {SHADOWPEN,0,VIF_DRIPEN},
     {0, 0,VIF_AREASTART},
    {11, 2,VIF_MOVE},
    {17, 2,VIF_DRAW},
    {17, 8,VIF_DRAW},
    {11, 2,VIF_DRAW},
     {0, 0,VIF_AREAEND},
     {0, 0,VIF_LASTITEM} };
struct VectorItem WidthImage[] =
  { {11,10,VIF_SCALE},
    {SHINEPEN,0,VIF_DRIPEN},
    {SHADOWPEN,0,VIF_AOLDRIPEN},
     {0, 0,VIF_AREASTART},
     {2, 5,VIF_MOVE},
     {9, 2,VIF_DRAW},
     {9, 8,VIF_DRAW},
     {2, 5,VIF_DRAW},
     {0, 0,VIF_AREAEND},
     {0, 0,VIF_LASTITEM} };
struct VectorItem ZoomInImage[] =
  { {22,10,VIF_SCALE},
    {SHADOWPEN,0,VIF_DRIPEN},
     {5, 5,VIF_MOVE},
    {16, 5,VIF_DRAW},
    {10, 2,VIF_MOVE},
    {10, 8,VIF_DRAW},
    {11, 2,VIF_MOVE},
    {11, 8,VIF_DRAW},
     {0, 0,VIF_LASTITEM} };
struct VectorItem ZoomOutImage[] =
  { {22,10,VIF_SCALE},
    {SHADOWPEN,0,VIF_DRIPEN},
     {5, 5,VIF_MOVE},
    {16, 5,VIF_DRAW},
     {0, 0,VIF_LASTITEM} };
struct VectorItem FlipHImage[] =
  { {22,10,VIF_SCALE},
    {SHADOWPEN,0,VIF_DRIPEN},
     {4, 5,VIF_MOVE},
    {16, 5,VIF_DRAW},
     {0, 0,VIF_AREASTART},
     {7, 2,VIF_MOVE},
     {4, 5,VIF_DRAW},
     {7, 8,VIF_DRAW},
     {0, 0,VIF_AREAEND},
    {0, 0,VIF_AREASTART},
    {13, 2,VIF_MOVE},
    {16, 5,VIF_DRAW},
    {13, 8,VIF_DRAW},
     {0, 0,VIF_AREAEND},
    {0, 0,VIF_LASTITEM} };
struct VectorItem FlipVImage[] =
  { {22,10,VIF_SCALE},
    {SHADOWPEN,0,VIF_DRIPEN},
    {11, 2,VIF_MOVE},
    {11, 8,VIF_DRAW},
     {0, 0,VIF_AREASTART},
     {6, 4,VIF_MOVE},
    {11, 2,VIF_DRAW},
    {16, 4,VIF_DRAW},
     {0, 0,VIF_AREAEND},
     {0, 0,VIF_AREASTART},
     {6, 6,VIF_MOVE},
    {11, 8,VIF_DRAW},
    {16, 6,VIF_DRAW},
     {0, 0,VIF_AREAEND},
     {0, 0,VIF_LASTITEM} };

extern ULONG Baseline;
extern struct Character CharBuffer[256];
extern struct List *CharWndList;
extern struct Screen *Screen;
extern struct Image *UpImage, *DownImage;
extern struct Image *SizeImage, *LeftImage, *RightImage;
extern struct MsgPort *WndMsgPort;

struct CharNode *OpenCharWin(ULONG charnum,struct IBox size)
{
extern Class *EditClass;
extern BOOL MaxWindow, ShowBLine, Toggle, DataChanged;
extern struct NewMenu CharMenus[];

struct CharNode *node;
Object *toolbarsub;
ULONG gw,gh,top;
UWORD width,height;
static ULONG scroll2edit[] = { GA_ID,EG_Update,TAG_DONE };

  if ((node = (struct CharNode *)CreateNode(sizeof(struct CharNode),
    CharWndList)) == NULL) Quit();
  if ((node->chln_Title = AllocVec(256,MEMF_CLEAR)) == NULL)
    ErrorCode(ALLOCVEC);
  if (charnum > 0)
  {
    if (charnum == 256)
      sprintf(node->chln_Title,GetString(msgBlankTitle));
    else
      sprintf(node->chln_Title,GetString(msgCharTitle),
      (char)charnum,charnum,charnum);
  }
  else sprintf(node->chln_Title,GetString(msgZeroTitle));
  if ((node->chln_Hook = AllocVec(sizeof(struct Hook),MEMF_CLEAR)) == NULL)
    ErrorCode(ALLOCVEC);
  node->chln_Hook->h_Entry = (HOOKFUNC)CharHook;
  node->chln_Hook->h_Data = node;
  node->chln_Character = CharBuffer+charnum;
  node->chln_Number = charnum;
  width = (Prefs.PixelX*node->chln_Character->chr_Width)+(2*EG_XOFFSET)+
    (2*SizeX(4))+Screen->WBorLeft+UpImage->Width;
  if (width < size.Width) size.Width = width;
  if (MaxWindow) size.Width = width;
  height = (Prefs.PixelY*node->chln_Character->chr_Height)+(2*EG_YOFFSET)+1+
    (2*SizeY(2))+Screen->WBorTop+Screen->Font->ta_YSize+LeftImage->Height;
  CopyChar(node,&(node->chln_UndoChar));

  node->chln_ToolBar =
    BGUI_NewObject(BGUI_GROUP_GADGET,GROUP_Spacing,SizeX(4),TAG_DONE);
  if (Prefs.ToolBar & (FTBAR_WIDTHL|FTBAR_WIDTHR))
  {
    toolbarsub = BGUI_NewObject(BGUI_GROUP_GADGET,TAG_DONE);
    if (Prefs.ToolBar & FTBAR_WIDTHL)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,LineArrowLeft,
	  ButtonFrame,
	  GA_ID,ID_WIDTHL,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    if (Prefs.ToolBar & FTBAR_WIDTHR)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,LineArrowRight,
	  ButtonFrame,
	  GA_ID,ID_WIDTHR,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    DoMethod(node->chln_ToolBar,GRM_ADDMEMBER,toolbarsub,TAG_DONE);
  }
  if (Prefs.ToolBar & (FTBAR_MOVEL|FTBAR_MOVER|FTBAR_MOVEU|FTBAR_MOVED))
  {
    if (node->chln_ToolBarWidth) node->chln_ToolBarWidth += SizeX(4);
    toolbarsub = BGUI_NewObject(BGUI_GROUP_GADGET,TAG_DONE);
    if (Prefs.ToolBar & FTBAR_MOVEL)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,SolidArrowLeft,
	  ButtonFrame,
	  GA_ID,ID_MOVEL,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    if (Prefs.ToolBar & FTBAR_MOVER)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,SolidArrowRight,
	  ButtonFrame,
	  GA_ID,ID_MOVER,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    if (Prefs.ToolBar & FTBAR_MOVEU)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,SolidArrowUp,
	  ButtonFrame,
	  GA_ID,ID_MOVEU,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    if (Prefs.ToolBar & FTBAR_MOVED)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,SolidArrowDown,
	  ButtonFrame,
	  GA_ID,ID_MOVED,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    DoMethod(node->chln_ToolBar,GRM_ADDMEMBER,toolbarsub,TAG_DONE);
  }
  if (Prefs.ToolBar & (FTBAR_KERN|FTBAR_WIDTH))
  {
    if (node->chln_ToolBarWidth) node->chln_ToolBarWidth += SizeX(4);
    toolbarsub = BGUI_NewObject(BGUI_GROUP_GADGET,TAG_DONE);
    if (Prefs.ToolBar & FTBAR_KERN)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,KernImage,
	  ButtonFrame,
	  GA_ID,ID_KERNING,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    if (Prefs.ToolBar & FTBAR_WIDTH)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,WidthImage,
	  ButtonFrame,
	  GA_ID,ID_WIDTHC,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    DoMethod(node->chln_ToolBar,GRM_ADDMEMBER,toolbarsub,TAG_DONE);
  }
  if (Prefs.ToolBar & (FTBAR_ZOOMIN|FTBAR_ZOOMOUT))
  {
    if (node->chln_ToolBarWidth) node->chln_ToolBarWidth += SizeX(4);
    toolbarsub = BGUI_NewObject(BGUI_GROUP_GADGET,TAG_DONE);
    if (Prefs.ToolBar & FTBAR_ZOOMIN)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,ZoomInImage,
	  ButtonFrame,
	  GA_ID,ID_ZOOMIN,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    if (Prefs.ToolBar & FTBAR_ZOOMOUT)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,ZoomOutImage,
	  ButtonFrame,
	  GA_ID,ID_ZOOMOUT,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    DoMethod(node->chln_ToolBar,GRM_ADDMEMBER,toolbarsub,TAG_DONE);
  }
  if (Prefs.ToolBar & (FTBAR_FLIPH|FTBAR_FLIPV))
  {
    if (node->chln_ToolBarWidth) node->chln_ToolBarWidth += SizeX(4);
    toolbarsub = BGUI_NewObject(BGUI_GROUP_GADGET,TAG_DONE);
    if (Prefs.ToolBar & FTBAR_FLIPH)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,FlipHImage,
	  ButtonFrame,
	  GA_ID,ID_FLIPH,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    if (Prefs.ToolBar & FTBAR_FLIPV)
    {
      DoMethod(toolbarsub,GRM_ADDMEMBER,
	ButtonObject,
	  VIT_VectorArray,FlipVImage,
	  ButtonFrame,
	  GA_ID,ID_FLIPV,
	EndObject,
	FixWidth(22),FixHeight(Prefs.VecHeight),TAG_DONE);
      node->chln_ToolBarWidth += 22;
    }
    DoMethod(node->chln_ToolBar,GRM_ADDMEMBER,toolbarsub,TAG_DONE);
  }

  if (node->chln_ToolBarWidth > 0)
  {
    height += Prefs.VecHeight+SizeY(2);
    node->chln_ToolBarHeight = Prefs.VecHeight+SizeY(2);
  }
  if (height < size.Height) size.Height = height;
  SetupMenus(CharMenus);

  node->chln_Object = WindowObject,
    WINDOW_Screen,Screen,
    WINDOW_SharedPort,WndMsgPort,
    WINDOW_MenuStrip,CharMenus,
    WINDOW_Title,node->chln_Title,
    WINDOW_Bounds,&size,
    WINDOW_HelpFile,NAME".guide",
    WINDOW_HelpNode,"char",
    WINDOW_IDCMPHook,node->chln_Hook,
    WINDOW_IDCMPHookBits,IDCMP_CHANGEWINDOW|IDCMP_RAWKEY,
    WINDOW_SizeRight,TRUE,
    WINDOW_SizeBottom,TRUE,
    WINDOW_CloseOnEsc,TRUE,
    WINDOW_MasterGroup,
      node->chln_WinGroup = VGroupObject,
	HOffset(SizeX(4)),VOffset(SizeY(2)),Spacing(SizeY(2)),
	StartMember,
	  node->chln_EditGadg = NewObject(EditClass,NULL,
	    EG_PixelX,Prefs.PixelX,
	    EG_PixelY,Prefs.PixelY,
	    EG_CharNode,node,
	    EG_CharStruct,node->chln_Character,
	    EG_PixelBorder,PixelBorder,
	    EG_ShowBaseline,ShowBLine,
	    EG_Toggle,Toggle,
	    EG_Baseline,Baseline,
	    EG_ChangePtr,&DataChanged,
	    GA_Disabled,FALSE,
	    GA_ID,ID_EDIT,
	  EndObject,
	EndMember,
      EndObject,
    EndObject;
  if (node->chln_Object == NULL)
    ErrorCode(NEWWINDOW);
  node->chln_MinPixelX = Prefs.PixelX;
  node->chln_MinPixelY = Prefs.PixelY;
  node->chln_CurPixelX = node->chln_MinPixelX;
  node->chln_CurPixelY = node->chln_MinPixelY;
  if (node->chln_ToolBarHeight > 0)
  {
    DoMethod(node->chln_WinGroup,GRM_INSERTMEMBER,
      node->chln_ToolBar,NULL,FixMinSize,TAG_DONE);
  }
  if ((node->chln_Window = WindowOpen(node->chln_Object)) == NULL)
    ErrorCode(OPENWINDOW);

  gh = MAX(LeftImage->Height,RightImage->Height);
  gh = MAX(SizeImage->Height,gh);
  gw = MAX(UpImage->Width,DownImage->Width);
  gw = MAX(SizeImage->Width,gw);
  top = Screen->WBorTop+Screen->Font->ta_YSize+1;

  if ((node->chln_HorizGadg = CreatePropGadg(FREEHORIZ,
    GA_Left,3,
    GA_RelBottom,3-gh,
    GA_RelWidth,-5-gw-LeftImage->Width-RightImage->Width,
    GA_Height,gh-4,
    GA_BottomBorder,TRUE,
    GA_ID,GADG_HORIZ,
    ICA_TARGET,node->chln_EditGadg,
    ICA_MAP,scroll2edit,
    PGA_Total,node->chln_Character->chr_Width,
    PGA_Visible,node->chln_Character->chr_Width,TAG_DONE)) == NULL)
      ErrorCode(NEWPROP);
  if ((node->chln_VertGadg = CreatePropGadg(FREEVERT,
    GA_RelRight,5-gw,
    GA_Top,top+1,
    GA_Width,gw-8,
    GA_RelHeight,-top-SizeImage->Height-UpImage->Height-DownImage->Height-2,
    GA_RightBorder,TRUE,
    GA_Previous,node->chln_HorizGadg,
    GA_ID,GADG_VERT,
    ICA_TARGET,node->chln_EditGadg,
    ICA_MAP,scroll2edit,
    PGA_Total,node->chln_Character->chr_Height,
    PGA_Visible,node->chln_Character->chr_Height,TAG_DONE)) == NULL)
      ErrorCode(NEWPROP);

  if ((node->chln_LeftGadg = CreateButtonGadg((Object *)LeftImage,
    GA_RelRight,1-LeftImage->Width-RightImage->Width-gw,
    GA_RelBottom,1-LeftImage->Height,
    GA_BottomBorder,TRUE,
    GA_Previous,node->chln_VertGadg,
    GA_ID,GADG_LEFT,
    ICA_TARGET,node->chln_EditGadg,
    ICA_MAP,scroll2edit,TAG_DONE)) == NULL) ErrorCode(NEWBUTTON);
  if ((node->chln_RightGadg = CreateButtonGadg((Object *)RightImage,
    GA_RelRight,1-RightImage->Width-gw,
    GA_RelBottom,1-RightImage->Height,
    GA_BottomBorder,TRUE,
    GA_Previous,node->chln_LeftGadg,
    GA_ID,GADG_RIGHT,
    ICA_TARGET,node->chln_EditGadg,
    ICA_MAP,scroll2edit,TAG_DONE)) == NULL) ErrorCode(NEWBUTTON);
  if ((node->chln_UpGadg = CreateButtonGadg((Object *)UpImage,
    GA_RelRight,1-UpImage->Width,
    GA_RelBottom,1-UpImage->Height-DownImage->Height-SizeImage->Height,
    GA_RightBorder,TRUE,
    GA_Previous,node->chln_RightGadg,
    GA_ID,GADG_UP,
    ICA_TARGET,node->chln_EditGadg,
    ICA_MAP,scroll2edit,TAG_DONE)) == NULL) ErrorCode(NEWBUTTON);
  if ((node->chln_DownGadg = CreateButtonGadg((Object *)DownImage,
    GA_RelRight,1-DownImage->Width,
    GA_RelBottom,1-DownImage->Height-SizeImage->Height,
    GA_RightBorder,TRUE,
    GA_Previous,node->chln_UpGadg,
    GA_ID,GADG_DOWN,
    ICA_TARGET,node->chln_EditGadg,
    ICA_MAP,scroll2edit,TAG_DONE)) == NULL) ErrorCode(NEWBUTTON);

  AddGList(node->chln_Window,(struct Gadget *)node->chln_HorizGadg,-1,-1,
    NULL);
  SetScrollers(node);
  RefreshGList((struct Gadget *)node->chln_HorizGadg,node->chln_Window,
    NULL,-1);
    
#warning CHECKME: there was no return statement at all

 return node;
}

extern struct Window *WidthWnd, *KernWnd;

void CloseCharWin(struct CharNode *node)
{
extern struct CharNode *WidthNode, *KernNode;

  ClrWindowClose(&(node->chln_Object),&(node->chln_Window));
  if (node->chln_ToolBarHeight > 0) DoMethod(node->chln_WinGroup,
    GRM_REMMEMBER,node->chln_ToolBar,TAG_DONE);
  ClrDisposeObject(&(node->chln_ToolBar));
  ClrDisposeObject(&(node->chln_Object));
  if (node->chln_Title)
  {
    FreeVec(node->chln_Title);
    node->chln_Title = NULL;
  }
  ClrDisposeObject(&(node->chln_HorizGadg));
  ClrDisposeObject(&(node->chln_VertGadg));
  ClrDisposeObject(&(node->chln_LeftGadg));
  ClrDisposeObject(&(node->chln_RightGadg));
  ClrDisposeObject(&(node->chln_UpGadg));
  ClrDisposeObject(&(node->chln_DownGadg));
  if (node->chln_Hook)
  {
    FreeVec(node->chln_Hook);
    node->chln_Hook = NULL;
  }
  if ((WidthWnd) && (WidthNode == node)) CloseWidthWnd(FALSE);
  if ((KernWnd) && (KernNode == node)) CloseKernWnd(FALSE);
  if (node->chln_UndoChar.chr_Data);
  {
    FreeVec(node->chln_UndoChar.chr_Data);
    node->chln_UndoChar.chr_Data = NULL;
  }
}

Object *CreateSysImage(ULONG which,struct DrawInfo *dri)
{
  return (NewObject(NULL,SYSICLASS,
    SYSIA_DrawInfo,dri,
    SYSIA_Which,which,TAG_DONE));
}

Object *CreatePropGadg(ULONG freedom,Tag tag1,...)
{
  return (NewObject(NULL,PROPGCLASS,
    PGA_Freedom,freedom,
    PGA_NewLook,TRUE,
    PGA_Borderless,TRUE,
    TAG_MORE,&tag1));
}

Object *CreateButtonGadg(Object *image,Tag tag1,...)
{
  return (NewObject(NULL,BUTTONGCLASS,
    GA_Image,image,
    TAG_MORE,&tag1));
}

#ifdef __AROS__
AROS_UFH3(void, CharHook,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, o, A2),
    AROS_UFHA(struct IntuiMessage *, msg, A1))
#else
SAVEDS ASM void CharHook(TF_REGPARAM(a0, struct Hook *, hook), TF_REGPARAM(a2, Object *, o),
  TF_REGPARAM(a1, struct IntuiMessage *, msg))
#endif
{
    AROS_USERFUNC_INIT

struct InputEvent *ievent;
char trans[2];

  switch (msg->Class)
  {
    case IDCMP_CHANGEWINDOW:
      if (msg->Code == CWCODE_MOVESIZE) ForceResizeChar(hook->h_Data);
      break;
    case IDCMP_RAWKEY:
      if ((ievent = AllocVec(sizeof(struct InputEvent),MEMF_CLEAR)))
      {
	ievent->ie_Class = IECLASS_RAWKEY;
	ievent->ie_Code = msg->Code;
	ievent->ie_Qualifier = msg->Qualifier;
	MapRawKey(ievent,trans,1,NULL);
	FreeVec(ievent);
	switch (trans[0])
	{
	  case '+':
	    ZoomIn(hook->h_Data);
	    break;
	  case '-':
	    ZoomOut(hook->h_Data);
	    break;
	}
      }
      break;
  }

    AROS_USERFUNC_EXIT
}

void ForceResizeChar(struct CharNode *node)
{
struct Character *chr;
LONG l,t,w,h;

  chr = node->chln_Character;
  GetAttr(EG_XOffset,node->chln_EditGadg,&l);
  GetAttr(EG_YOffset,node->chln_EditGadg,&t);
  GetAttr(EG_Width,node->chln_EditGadg,&w);
  GetAttr(EG_Height,node->chln_EditGadg,&h);
  if (chr->chr_Width - l < w) l = chr->chr_Width - w;
  if (l < 0) l = 0;
  if (chr->chr_Height - t < h) t = chr->chr_Height - h;
  if (t < 0) t = 0;
  SetGadgetAttrs((struct Gadget *)node->chln_EditGadg,node->chln_Window,
    NULL,EG_XOffset,l,EG_YOffset,t,TAG_DONE);
  SetScrollers(node);
}

void SetScrollers(struct CharNode *node)
{
ULONG l,t,w,h;

  GetAttr(EG_XOffset,node->chln_EditGadg,&l);
  GetAttr(EG_YOffset,node->chln_EditGadg,&t);
  GetAttr(EG_Width,node->chln_EditGadg,&w);
  GetAttr(EG_Height,node->chln_EditGadg,&h);
  SetGadgetAttrs((struct Gadget *)node->chln_HorizGadg,node->chln_Window,
    NULL,PGA_Top,l,PGA_Visible,w,TAG_DONE);
  SetGadgetAttrs((struct Gadget *)node->chln_VertGadg,node->chln_Window,
    NULL,PGA_Top,t,PGA_Visible,h,TAG_DONE);
}

void RedrawEdit(struct CharNode *node)
{
  SetGadgetAttrs((struct Gadget *)node->chln_HorizGadg,node->chln_Window,
    NULL,PGA_Total,node->chln_Character->chr_Width,TAG_DONE);
  SetGadgetAttrs((struct Gadget *)node->chln_VertGadg,node->chln_Window,
    NULL,PGA_Total,node->chln_Character->chr_Height,TAG_DONE);
  SetGadgetAttrs((struct Gadget *)node->chln_EditGadg,node->chln_Window,
    NULL,
    EG_Baseline,Baseline,
    EG_Update,GADG_NONE,TAG_DONE);
  ForceResizeChar(node);
}

void MoveLeft(struct CharNode *node,struct Character *chr)
{
ULONG i,j;

  if (node != NULL) chr = node->chln_Character;
  if ((chr->chr_Data) && (chr->chr_Width > 0))
  {
    for (j = 0; j < chr->chr_Height; j++)
    {
      for (i = 0; i < chr->chr_Width-1; i++) DATA(i,j) = DATA(i+1,j);
      DATA(chr->chr_Width-1,j) = 0;
    }
    if (node != NULL) RedrawEdit(node);
  }
}

void MoveRight(struct CharNode *node,struct Character *chr)
{
ULONG i,j;

  if (node != NULL) chr = node->chln_Character;
  if ((chr->chr_Data) && (chr->chr_Width > 0))
  {
    for (j = 0; j < chr->chr_Height; j++)
    {
      for (i = chr->chr_Width-1; i > 0; i--) DATA(i,j) = DATA(i-1,j);
      DATA(0,j) = 0;
    }
    if (node != NULL) RedrawEdit(node);
  }
}

void MoveUp(struct CharNode *node,struct Character *chr)
{
ULONG i,j;

  if (node != NULL) chr = node->chln_Character;
  if ((chr->chr_Data) && (chr->chr_Height > 0))
  {
    for (i = 0; i < chr->chr_Width; i++)
    {
      for (j = 0; j < chr->chr_Height-1; j++) DATA(i,j) = DATA(i,j+1);
      DATA(i,chr->chr_Height-1) = 0;
    }
    if (node != NULL) RedrawEdit(node);
  }
}

void MoveDown(struct CharNode *node,struct Character *chr)
{
ULONG i,j;

  if (node != NULL) chr = node->chln_Character;
  if ((chr->chr_Data) && (chr->chr_Height > 0))
  {
    for (i = 0; i < chr->chr_Width; i++)
    {
      for (j = chr->chr_Height-1; j > 0; j--) DATA(i,j) = DATA(i,j-1);
      DATA(i,0) = 0;
    }
    if (node != NULL) RedrawEdit(node);
  }
}

void MoveAll(void (*MoveFunction)())
{
struct Character *chr;
ULONG i;

  for (i = 0; i < 257; i++)
  {
    chr = CharBuffer+i;
    MoveFunction(NULL,chr);
  }
  RedrawAll();
}

void RedrawAll()
{
struct CharNode *node;

  node = (struct CharNode *)CharWndList->lh_Head;
  while (node->chln_Node.ln_Succ)
  {
    RedrawEdit(node);
    CheckWidthKern(node->chln_Character);
    node = (struct CharNode *)node->chln_Node.ln_Succ;
  }
}

void ZoomIn(struct CharNode *node)
{
ULONG x,y,max_x,max_y;

  GetAttr(EG_PixelX,node->chln_EditGadg,&x);
  GetAttr(EG_PixelY,node->chln_EditGadg,&y);

  max_x = node->chln_Window->Width-(2*EG_XOFFSET)-(2*SizeX(4))-
    Screen->WBorLeft-UpImage->Width;
  max_y = node->chln_Window->Height-(2*EG_YOFFSET)-1-(2*SizeY(2))-
    Screen->WBorTop-Screen->Font->ta_YSize-LeftImage->Height-
    node->chln_ToolBarHeight;

  if ((x*4 <= max_x) && (y*4 <= max_y))
  {
    SetGadgetAttrs((struct Gadget *)node->chln_EditGadg,node->chln_Window,
      NULL,
      EG_PixelX,x*2,
      EG_PixelY,y*2,
      EG_Update,GADG_NONE,TAG_DONE);
    SetScrollers(node);
    SetMinWindowSize(node,x);
  }
  else DisplayBeep(node->chln_Window->WScreen);
}

void ZoomOut(struct CharNode *node)
{
ULONG x,y;

  GetAttr(EG_PixelX,node->chln_EditGadg,&x);
  GetAttr(EG_PixelY,node->chln_EditGadg,&y);

  if ((x/2 >= 2) && (y/2 >= 2))
  {
    SetGadgetAttrs((struct Gadget *)node->chln_EditGadg,node->chln_Window,
      NULL,
      EG_PixelX,x/2,
      EG_PixelY,y/2,
      EG_Update,GADG_NONE,TAG_DONE);
    SetScrollers(node);
    SetMinWindowSize(node,x);
  }
  else DisplayBeep(node->chln_Window->WScreen);
}

void SetMinWindowSize(struct CharNode *node, ULONG oldx)
{
ULONG x,y;

  GetAttr(EG_PixelX,node->chln_EditGadg,&x);
  GetAttr(EG_PixelY,node->chln_EditGadg,&y);

  if (2*(oldx+EG_XOFFSET) < node->chln_ToolBarWidth)
  {
    if (2*(x+EG_XOFFSET) > node->chln_ToolBarWidth)
    {
      node->chln_Window->MinWidth -= node->chln_ToolBarWidth;
      node->chln_Window->MinWidth += 2*(x+EG_XOFFSET);
    }
  }
  else
  {
    if (2*(x+EG_XOFFSET) < node->chln_ToolBarWidth)
    {
      node->chln_Window->MinWidth += node->chln_ToolBarWidth;
      node->chln_Window->MinWidth -= 2*(node->chln_CurPixelX+EG_XOFFSET);
    }
    else
    {
      node->chln_Window->MinWidth += 2*MAX(x,node->chln_MinPixelX);
      node->chln_Window->MinWidth -= 2*node->chln_CurPixelX;
    }
  }
  node->chln_CurPixelX = MAX(x,node->chln_MinPixelX);

  node->chln_Window->MinHeight += 2*MAX(y,node->chln_MinPixelY);
  node->chln_Window->MinHeight -= 2*node->chln_CurPixelY;
  node->chln_CurPixelY = MAX(y,node->chln_MinPixelY);
}

void CopyChar(struct CharNode *node,struct Character *copyto)
{
struct Character *chr;

  chr = node->chln_Character;
  if ((chr->chr_Width > 0) && (chr->chr_Data))
  {
    if (copyto->chr_Data) FreeVec(copyto->chr_Data);
    copyto->chr_Data = NULL;
    copyto->chr_Width = chr->chr_Width;
    copyto->chr_Height = chr->chr_Height;
    if ((copyto->chr_Data = AllocVec(chr->chr_Width*chr->chr_Height,
      MEMF_CLEAR)) == NULL) ErrorCode(ALLOCVEC);
    CopyMem(chr->chr_Data,copyto->chr_Data,chr->chr_Width*chr->chr_Height);
  }
}

void PasteChar(struct CharNode *node,struct Character *pastefrom)
{
struct Character *chr;
ULONG i;
extern ULONG Proportional;

  chr = node->chln_Character;
  if (pastefrom->chr_Data)
  {
    if (chr->chr_Data) FreeVec(chr->chr_Data);
    chr->chr_Data = NULL;
    if (Proportional) chr->chr_Width = pastefrom->chr_Width;
    if (chr->chr_Width > 0)
    {
      if ((chr->chr_Data = AllocVec(chr->chr_Width*chr->chr_Height,
	MEMF_CLEAR)) == NULL) ErrorCode(ALLOCVEC);
      for (i = 0; i < MIN(chr->chr_Height,pastefrom->chr_Height); i++)
	CopyMem(pastefrom->chr_Data+(i*pastefrom->chr_Width),
	chr->chr_Data+(i*chr->chr_Width),
	MIN(chr->chr_Width,pastefrom->chr_Width));
    }
    RedrawEdit(node);
  }
}

void EraseChar(struct CharNode *node)
{
struct Character *chr;
int i;

  chr = node->chln_Character;
  if ((chr->chr_Width > 0) && (chr->chr_Data))
  {
    for (i = 0; i < chr->chr_Width * chr->chr_Height; i++)
      *(chr->chr_Data + i) = 0;
    RedrawEdit(node);
  }
}

void SwapBytes(UBYTE *ptr,int i,int j)
{
UBYTE temp;

  temp = *(ptr+i);
  *(ptr+i) = *(ptr+j);
  *(ptr+j) = temp;
}

void FlipHoriz(struct CharNode *node)
{
struct Character *chr;
int i,j;

  chr = node->chln_Character;
  if ((chr->chr_Width > 0) && (chr->chr_Data))
  {
    for (i = 0; i < chr->chr_Height; i++)
    {
      for (j = 0; j < chr->chr_Width / 2; j++)
	SwapBytes(chr->chr_Data,(i*chr->chr_Width)+j,(i*chr->chr_Width)+chr->chr_Width-j-1);
    }
    RedrawEdit(node);
  }
}

void FlipVert(struct CharNode *node)
{
struct Character *chr;
int i,j;

  chr = node->chln_Character;
  if ((chr->chr_Width > 0) && (chr->chr_Data))
  {
    for (j = 0; j < chr->chr_Width; j++)
    {
      for (i = 0; i < chr->chr_Height / 2; i++)
	SwapBytes(chr->chr_Data,(i*chr->chr_Width)+j,((chr->chr_Height-i-1)*chr->chr_Width)+j);
    }
    RedrawEdit(node);
  }
}

void OpenResetKernWnd(void)
{
Object *reset, *cancel;
char *warn;
extern struct NewMenu Menus[];

  if (ResetKernWnd)
  {
    WindowToFront(ResetKernWnd);
    ActivateWindow(ResetKernWnd);
  }
  else
  {
    if (ResetKernWndObj == NULL)
    {
      SetupMenus(Menus);
      warn = GetString(infoWarnReset);
      ResetKernWndObj = WindowObject,
	WINDOW_Screen,Screen,
	WINDOW_SharedPort,WndMsgPort,
	WINDOW_MenuStrip,Menus,
	WINDOW_Title,GetString(msgResetTitle),
	WINDOW_HelpFile,NAME".guide",
	WINDOW_HelpNode,"resetkern",
	WINDOW_SmartRefresh,TRUE,
	WINDOW_SizeGadget,FALSE,
	WINDOW_CloseOnEsc,TRUE,
	WINDOW_MasterGroup,
	  VGroupObject,
	    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
	    GROUP_BackFill,SHINE_RASTER,
	    StartMember,
	      VGroupObject,
		HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
		ButtonFrame,
		FRM_Recessed,TRUE,
		StartMember,
		  HGroupObject,
		    VarSpace(50),
		    StartMember,
		      VGroupObject,
			Spacing(SizeY(2)),
			StartMember,
			  ResetSpaceInt = StringObject,
			    RidgeFrame,
			    UScoreLabel(GetString(gadgSpacing),'_'),
			    STRINGA_LongVal,0,
			    STRINGA_MaxChars,5,
			    STRINGA_MinCharsVisible,5,
			    GA_TabCycle,TRUE,
			  EndObject,
			EndMember,
		        StartMember,
			  ResetKernInt = StringObject,
			    RidgeFrame,
			    UScoreLabel(GetString(gadgKerning),'_'),
			    STRINGA_LongVal,0,
			    STRINGA_MaxChars,5,
			    STRINGA_MinCharsVisible,5,
			    GA_TabCycle,TRUE,
			  EndObject,
			EndMember,
		      EndObject,
		    EndMember,
		    StartMember,
		      VGroupObject,
			StartMember,
			  InfoObject,
			    FRM_Type,FRTYPE_NONE,
			    INFO_MinLines,1,
			    INFO_HorizOffset,6,
			    INFO_VertOffset,4,
			    INFO_TextFormat,GetString(infoPlusWidth),
			    INFO_FixTextWidth,TRUE,
			  EndObject,
			EndMember,
			VarSpace(1000),
		      EndObject,
		    EndMember,
		    VarSpace(50),
		  EndObject,
		EndMember,
		StartMember,
		  InfoObject,
		    FRM_Type,FRTYPE_NONE,
		    INFO_MinLines,3,
		    INFO_HorizOffset,0,
		    INFO_VertOffset,0,
		    INFO_TextFormat,"\33c%s",
		    INFO_Args,&warn,
		    INFO_FixTextWidth,TRUE,
		  EndObject,
		EndMember,
	      EndObject,
	    EndMember,
	    StartMember,
	      HGroupObject,
		Spacing(SizeX(8)),
		StartMember,
		  reset = KeyButton(GetString(gadgReset),ID_ACCEPT),
		EndMember,
        	StartMember,
		  cancel = KeyButton(GetString(gadgCancel),ID_CANCEL),
		EndMember,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	  EndObject,
	EndObject;
      if (ResetKernWndObj == NULL) ErrorCode(NEWWINDOW);
      SetLabelKey(ResetKernWndObj,ResetSpaceInt,gadgSpacing);
      SetLabelKey(ResetKernWndObj,ResetKernInt,gadgKerning);
      SetLabelKey(ResetKernWndObj,reset,gadgReset);
      SetLabelKey(ResetKernWndObj,cancel,gadgCancel);
      DoMethod(ResetKernWndObj,WM_TABCYCLE_ORDER,
	ResetSpaceInt,ResetKernInt,NULL);
    }

    if ((ResetKernWnd = WindowOpen(ResetKernWndObj)) == NULL)
      ErrorCode(OPENWINDOW);
  }
}

void CloseResetKernWnd(BOOL obj)
{
  ClrWindowClose(&ResetKernWndObj,&ResetKernWnd);
  if (obj) ClrDisposeObject(&ResetKernWndObj);
}

void ResetKerning(WORD space,WORD kern)
{
extern WORD SpaceTable[], KernTable[];
extern struct Character *KernChar;
extern Object *KernObj;
struct CharNode *node;
int i;

  for (i = 0; i < 257; i++)
  {
    SpaceTable[i] = (CharBuffer+i)->chr_Width + space;
    KernTable[i] = kern;
  }
  node = (struct CharNode *)CharWndList->lh_Head;
  while (node->chln_Node.ln_Succ)
  {
    CheckWidthKern(node->chln_Character);
    if (node->chln_Character == KernChar)
      SetGadgetAttrs((struct Gadget *)KernObj,KernWnd,NULL,
      STRINGA_LongVal,KernTable[node->chln_Number],TAG_DONE);
    node = (struct CharNode *)node->chln_Node.ln_Succ;
  }
  RedrawAll();
}
