/************************/
/*			*/
/* Gadgets for Typeface */
/*			*/
/************************/

#include "typeface.h"

#ifdef __AROS__
extern struct Library *BGUIBase;
#endif

extern struct GfxBase *GfxBase;

/* For any gadget */

void Gadg_DrawBevel(struct RastPort *rp,ULONG x,ULONG y,ULONG w,ULONG h,
  UBYTE pen1,UBYTE pen2)
{
  SetAPen(rp,pen1);
  Move(rp,x+w-2,y);
  Draw(rp,x,y);
  Draw(rp,x,y+h-1);
  Move(rp,1+x,y+h-2);
  Draw(rp,1+x,1+y);

  SetAPen(rp,pen2);
  Move(rp,1+x,y+h-1);
  Draw(rp,x+w-1,y+h-1);
  Draw(rp,x+w-1,y);
  Move(rp,x+w-2,1+y);
  Draw(rp,x+w-2,y+h-2);
}

void Gadg_SetTag(struct TagItem *tag,ULONG id, ULONG data)
{
  tag->ti_Tag = id;
  tag->ti_Data = data;
}

/* Character selection gadget */

#define CG_XOFFSET 6
#define CG_YOFFSET 3

struct CharGadgData
{
  UWORD cg_CharWidth,cg_CharHeight;
  WORD cg_FirstX,cg_FirstY;
  struct TextFont *cg_TextFont;
  ULONG cg_LastPressed,cg_Pos;
  BOOL cg_Pressed,cg_Redraw;
  UWORD cg_SizeX,cg_SizeY;
  ULONG cg_ShiftPressed;
};

SAVEDS ASM ULONG DispatchCharGadg(TF_REGPARAM(a0, Class *, cl), TF_REGPARAM(a2, Object *, o), TF_REGPARAM(a1, Msg, msg));
ULONG CharGadg_NEW(Class *cl,Object *o,struct opSet *ops);
ULONG CharGadg_RENDER(Class *cl,struct Gadget *gadg,struct gpRender *gpr);
ULONG CharGadg_GOACTIVE(Class *cl,struct Gadget *gadg,struct gpInput *gpi);
ULONG CharGadg_HANDLEINPUT(Class *cl,struct Gadget *gadg,
  struct gpInput *gpi);
ULONG CharGadg_GOINACTIVE(Class *cl,struct Gadget *gadg,
  struct gpGoInactive *gpgi);
ULONG CharGadg_SET(Class *cl,struct Gadget *gadg,struct opSet *ops);
ULONG CharGadg_GET(Class *cl,Object *o,struct opGet *opg);
ULONG CharGadg_DIMENSIONS(Class *cl,struct Gadget *gadg,
  struct bmDimensions *dim);
void CharGadg_DrawChar(struct RastPort *rp,struct GadgetInfo *gi,
  BOOL selected,ULONG x,ULONG y,ULONG w,ULONG h,char gchar,BOOL lines);
void CharGadg_DrawCurrent(struct GadgetInfo *gi,struct CharGadgData *cgd,
  struct Gadget *gadg);

Class *InitCharGadgClass(void)
{
Class *super,*cl = NULL;

  if ((super = BGUI_GetClassPtr(BGUI_BASE_GADGET)))
  {
    if ((cl = MakeClass(NULL,NULL,super,sizeof(struct CharGadgData),0)))
      cl->cl_Dispatcher.h_Entry = (HOOKFUNC)DispatchCharGadg;
  }
  return cl;
}

ULONG GetCharGadgHeight(UWORD height,struct TextFont *font)
{
struct TextFont *gadget_font;

  gadget_font = font ? font : GfxBase->DefaultFont;
  return (gadget_font->tf_YSize+(CG_YOFFSET*2))*height;
}

#ifdef __AROS__
AROS_UFH3(ULONG, DispatchCharGadg,
    AROS_UFHA(Class *, cl, A0),
    AROS_UFHA(Object *, o, A2),
    AROS_UFHA(Msg, msg, A1))
#else
SAVEDS ASM ULONG DispatchCharGadg(TF_REGPARAM(a0, Class *, cl), TF_REGPARAM(a2, Object *, o), TF_REGPARAM(a1, Msg, msg))
#endif
{
    AROS_USERFUNC_INIT

ULONG retval;

  switch (msg->MethodID)
  {
    case OM_NEW:
      retval = CharGadg_NEW(cl,o,(struct opSet *)msg);
      break;
    case OM_SET:
      retval = CharGadg_SET(cl,(struct Gadget *)o,(struct opSet *)msg);
      break;
    case OM_GET:
      retval = CharGadg_GET(cl,o,(struct opGet *)msg);
      break;
    case GM_RENDER:
      retval = CharGadg_RENDER(cl,(struct Gadget *)o,(struct gpRender *)msg);
      break;
    case GM_GOACTIVE:
      retval = CharGadg_GOACTIVE(cl,(struct Gadget *)o,
	(struct gpInput *)msg);
      break;
    case GM_HANDLEINPUT:
      retval = CharGadg_HANDLEINPUT(cl,(struct Gadget *)o,
	(struct gpInput *)msg);
      break;
    case GM_GOINACTIVE:
      retval = CharGadg_GOINACTIVE(cl,(struct Gadget *)o,
	(struct gpGoInactive *)msg);
      break;
    case BASE_DIMENSIONS:
      retval = CharGadg_DIMENSIONS(cl,(struct Gadget *)o,
	(struct bmDimensions *)msg);
      break;
    default:
      retval = DoSuperMethodA(cl,o,(Msg)msg);
      break;
  }
  return retval;

    AROS_USERFUNC_EXIT
}

ULONG CharGadg_NEW(Class *cl,Object *o,struct opSet *ops)
{
struct CharGadgData *cgd = NULL;
ULONG retval;

  retval = (ULONG)DoSuperMethodA(cl,o,(Msg)ops);
  if (retval)
  {
    cgd = INST_DATA(cl,retval);

    cgd->cg_TextFont = (struct TextFont *)
      GetTagData(CG_Font,(ULONG)GfxBase->DefaultFont,ops->ops_AttrList);
    cgd->cg_SizeX = GetTagData(CG_SizeX,8,ops->ops_AttrList);
    cgd->cg_SizeY = GetTagData(CG_SizeY,8,ops->ops_AttrList);

    cgd->cg_CharWidth = cgd->cg_TextFont->tf_XSize+(CG_XOFFSET*2);
    cgd->cg_CharHeight = cgd->cg_TextFont->tf_YSize+(CG_YOFFSET*2);
    cgd->cg_Pressed = FALSE;
    cgd->cg_Redraw = TRUE;
    cgd->cg_LastPressed = ~0;
  }
  return retval;
}

ULONG CharGadg_RENDER(Class *cl,struct Gadget *gadg,struct gpRender *gpr)
{
struct CharGadgData *cgd = INST_DATA(cl,(Object *)gadg);
struct TextFont *old_tf = NULL;
struct RastPort *rp;
ULONG retval;
int i,j;

  retval = DoSuperMethodA(cl,(Object *)gadg,(Msg)gpr);
  if (retval == 0)
    return 0;

  if (cgd->cg_Redraw == FALSE)
    return retval;
  cgd->cg_Redraw = FALSE;

  rp = gpr->gpr_RPort;
  if (rp->Font != cgd->cg_TextFont)
  {
    old_tf = rp->Font;
    SetFont(rp,cgd->cg_TextFont);
  }

  switch (gpr->gpr_Redraw)
  {
    case GREDRAW_REDRAW:
    case GREDRAW_UPDATE:
      for (i = 0; i < cgd->cg_SizeY; i++)
      {
	for (j = 0; j < cgd->cg_SizeX; j++)
	{
	  CharGadg_DrawChar(rp,gpr->gpr_GInfo,FALSE,
	    (j*cgd->cg_CharWidth)+gadg->LeftEdge,
	    (i*cgd->cg_CharHeight)+gadg->TopEdge,
	    cgd->cg_CharWidth,cgd->cg_CharHeight,
	    ((cgd->cg_Pos+i)*cgd->cg_SizeX)+j,
	    gpr->gpr_Redraw == GREDRAW_REDRAW);
	}
      }
      break;
  }

  if (old_tf)
    SetFont(rp,old_tf);
  return retval;
}

ULONG CharGadg_GOACTIVE(Class *cl,struct Gadget *gadg,struct gpInput *gpi)
{
struct CharGadgData *cgd = INST_DATA(cl,(Object *)gadg);
struct GadgetInfo *gi;
ULONG retval;
int i,j;

  retval = DoSuperMethodA(cl,(Object *)gadg,(Msg)gpi);
  cgd->cg_FirstX = -1;
  cgd->cg_FirstY = -1;
  cgd->cg_Pressed = TRUE;
  j = gpi->gpi_Mouse.X / cgd->cg_CharWidth;
  i = gpi->gpi_Mouse.Y / cgd->cg_CharHeight;

  if ((i >= 0) && (i < cgd->cg_SizeY) && (j >= 0) && (j < cgd->cg_SizeX))
  {
    cgd->cg_FirstX = j;
    cgd->cg_FirstY = i;
    gi = gpi->gpi_GInfo;
    CharGadg_DrawCurrent(gi,cgd,gadg);
    retval = GMR_MEACTIVE;
  }
  return retval;
}

ULONG CharGadg_HANDLEINPUT(Class *cl,struct Gadget *gadg,struct gpInput *gpi)
{
struct CharGadgData *cgd = INST_DATA(cl,(Object *)gadg);
struct GadgetInfo *gi = gpi->gpi_GInfo;
struct InputEvent *ie = gpi->gpi_IEvent;
ULONG retval = GMR_MEACTIVE;
int i,j;

  DoSuperMethodA(cl,(Object *)gadg,(Msg)gpi);
  if (gi->gi_Window)
  {
    if ((gi->gi_Window->Flags & WFLG_WINDOWACTIVE) == 0)
      return GMR_NOREUSE;
  }

  while (ie && (retval == GMR_MEACTIVE))
  {
    if (ie->ie_Class == IECLASS_RAWMOUSE)
    {
      if (ie->ie_Code != SELECTUP)
      {
	cgd->cg_ShiftPressed =
	  ie->ie_Qualifier & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT);
	j = gpi->gpi_Mouse.X / cgd->cg_CharWidth;
	i = gpi->gpi_Mouse.Y / cgd->cg_CharHeight;
	if ((i == cgd->cg_FirstY) && (j == cgd->cg_FirstX))
	{
	  if (cgd->cg_Pressed == FALSE)
	  {
	    cgd->cg_Pressed = TRUE;
	    CharGadg_DrawCurrent(gi,cgd,gadg);
	  }
	}
	else
	{
	  if (cgd->cg_Pressed == TRUE)
	  {
	    cgd->cg_Pressed = FALSE;
	    CharGadg_DrawCurrent(gi,cgd,gadg);
	  }
	}
      }
      else
        retval = GMR_NOREUSE|GMR_VERIFY;
    }
    ie = ie->ie_NextEvent;
  }
  return retval;
}

ULONG CharGadg_GOINACTIVE(Class *cl,struct Gadget *gadg,
  struct gpGoInactive *gpgi)
{
struct CharGadgData *cgd = INST_DATA(cl,(Object *)gadg);
struct TextFont *old_tf = NULL;
struct GadgetInfo *gi;
struct RastPort *rp;
struct TagItem inactive;
ULONG retval;

  retval = DoSuperMethodA(cl,(Object *)gadg,(Msg)gpgi);
  if ((cgd->cg_FirstX != -1) && (cgd->cg_FirstY != -1))
  {
    gi = gpgi->gpgi_GInfo;
    if (cgd->cg_Pressed == TRUE)
    {
      if ((rp = ObtainGIRPort(gi)))
      {
	if (rp->Font != cgd->cg_TextFont)
	{
	  old_tf = rp->Font;
	  SetFont(rp,cgd->cg_TextFont);
	}
	CharGadg_DrawChar(rp,gi,FALSE,
	  (cgd->cg_FirstX*cgd->cg_CharWidth)+gadg->LeftEdge,
	  (cgd->cg_FirstY*cgd->cg_CharHeight)+gadg->TopEdge,
	  cgd->cg_CharWidth,cgd->cg_CharHeight,
	  ((cgd->cg_Pos+cgd->cg_FirstY)*cgd->cg_SizeX)+cgd->cg_FirstX,
	  TRUE);
	if (old_tf)
	  SetFont(rp,old_tf);
	ReleaseGIRPort(rp);
      }
      cgd->cg_LastPressed =
	((cgd->cg_Pos+cgd->cg_FirstY)*cgd->cg_SizeX)+cgd->cg_FirstX;
      Gadg_SetTag(&inactive,TAG_DONE,0);
      DoMethod((Object *)gadg,OM_NOTIFY,&inactive,gi,0);
    }
  }
  return retval;
}

ULONG CharGadg_SET(Class *cl,struct Gadget *gadg,struct opSet *ops)
{
struct CharGadgData *cgd = INST_DATA(cl,(Object *)gadg);
struct TagItem *tags, *tag;
struct RastPort *rp;
ULONG retval;

  retval = DoSuperMethodA(cl,(Object *)gadg,(Msg)ops);
  tags = ops->ops_AttrList;
  if (tags)
  {
    if ((tag = FindTagItem(CG_Pos,tags)))
      cgd->cg_Pos = tag->ti_Data;
    if ((tag = FindTagItem(CG_SizeY,tags)))
      cgd->cg_SizeY = tag->ti_Data;
    if ((tag = FindTagItem(CG_Pressed,tags)))
      cgd->cg_LastPressed = tag->ti_Data;

    if ((tag = FindTagItem(CG_Redraw,tags)))
    {
      if ((rp = ObtainGIRPort(ops->ops_GInfo)))
      {
	cgd->cg_Redraw = TRUE;
	DoMethod((Object *)gadg,GM_RENDER,ops->ops_GInfo,rp,
	  tag->ti_Data ? GREDRAW_REDRAW : GREDRAW_UPDATE);
	ReleaseGIRPort(rp);
      }
    }
  }
  return retval;
}

ULONG CharGadg_GET(Class *cl,Object *o,struct opGet *opg)
{
struct CharGadgData *cgd = INST_DATA(cl,o);
ULONG retval = 0;

  switch (opg->opg_AttrID)
  {
    case CG_Pos:
      *(opg->opg_Storage) = cgd->cg_Pos;
      break;
    case CG_SizeY:
      *(opg->opg_Storage) = cgd->cg_SizeY;
      break;
    case CG_Pressed:
      *(opg->opg_Storage) = cgd->cg_LastPressed;
      break;
    case CG_ShiftDown:
      *(opg->opg_Storage) = cgd->cg_ShiftPressed;
      break;
    default:
      retval = DoSuperMethodA(cl,o,(Msg)opg);
      break;
  }
  return retval;
}

ULONG CharGadg_DIMENSIONS(Class *cl,struct Gadget *gadg,struct bmDimensions *dim)
{
struct CharGadgData *cgd = INST_DATA(cl,gadg);
ULONG retval;

  retval = DoSuperMethodA(cl,(Object *)gadg,(Msg)dim);
  if (retval)
  {
  int mx, my;

    mx = cgd->cg_CharWidth*cgd->cg_SizeX;
    my = cgd->cg_CharHeight*cgd->cg_SizeY;

    dim->bmd_Extent->be_Min.Width  += mx;
    dim->bmd_Extent->be_Min.Height += my;
    dim->bmd_Extent->be_Nom.Width  += mx;
    dim->bmd_Extent->be_Nom.Height += my;
   }
   return retval;
}

void CharGadg_DrawChar(struct RastPort *rp,struct GadgetInfo *gi,
  BOOL selected,ULONG x,ULONG y,ULONG w,ULONG h,char gchar, BOOL lines)
{
struct DrawInfo *dri = gi->gi_DrInfo;
UBYTE pen1,pen2,pen3,pen4;

  if (selected)
  {
    pen1 = dri->dri_Pens[SHADOWPEN];
    pen2 = dri->dri_Pens[SHINEPEN];
    pen3 = dri->dri_Pens[FILLTEXTPEN];
    pen4 = dri->dri_Pens[FILLPEN];
  }
  else
  {
    pen1 = dri->dri_Pens[SHINEPEN];
    pen2 = dri->dri_Pens[SHADOWPEN];
    pen3 = dri->dri_Pens[TEXTPEN];
    pen4 = dri->dri_Pens[BACKGROUNDPEN];
  }

  SetAPen(rp,pen4);
  if (lines)
  {
    RectFill(rp,x,y,x+w-1,y+h-1);
    Gadg_DrawBevel(rp,x,y,w,h,pen1,pen2);
  }
  else
    RectFill(rp,x+2,y+1,x+w-5,y+h-3);

  SetAPen(rp,pen3);
  SetBPen(rp,pen4);
  SetDrMd(rp,JAM2);
  Move(rp,x+CG_XOFFSET,y+CG_YOFFSET+rp->Font->tf_Baseline);
  Text(rp,&gchar,1);
}

void CharGadg_DrawCurrent(struct GadgetInfo *gi,struct CharGadgData *cgd,
  struct Gadget *gadg)
{
struct TextFont *old_tf = NULL;
struct RastPort *rp;

  if ((rp = ObtainGIRPort(gi)))
  {
    if (rp->Font != cgd->cg_TextFont)
    {
      old_tf = rp->Font;
      SetFont(rp,cgd->cg_TextFont);
    }
    CharGadg_DrawChar(rp,gi,cgd->cg_Pressed,
      (cgd->cg_FirstX*cgd->cg_CharWidth)+gadg->LeftEdge,
      (cgd->cg_FirstY*cgd->cg_CharHeight)+gadg->TopEdge,
      cgd->cg_CharWidth,cgd->cg_CharHeight,
      ((cgd->cg_Pos+cgd->cg_FirstY)*cgd->cg_SizeX)+cgd->cg_FirstX,TRUE);
    if (old_tf)
      SetFont(rp,old_tf);
    ReleaseGIRPort(rp);
  }
}

/* Character editing gadget */

struct EditGadgData
{
  UWORD eg_PixelX,eg_PixelY;
  struct Character *eg_Char;
  UWORD eg_XOffset,eg_YOffset;
  struct CharNode *eg_Node;
  UWORD eg_Button, eg_PixelBorder, eg_ShowBaseline;
  ULONG eg_Baseline;
  BOOL *eg_ChangePtr;
  WORD eg_ActiveX, eg_ActiveY;
  UWORD eg_Toggle, eg_FirstPixelState;
};

SAVEDS ASM ULONG DispatchEditGadg(TF_REGPARAM(a0, Class *, cl), TF_REGPARAM(a2, Object *, o), TF_REGPARAM(a1, Msg, msg));
ULONG EditGadg_NEW(Class *cl,Object *o,struct opSet *ops);
ULONG EditGadg_GET(Class *cl,Object *o,struct opGet *opg);
ULONG EditGadg_SET(Class *cl,struct Gadget *gadg,struct opSet *ops);
ULONG EditGadg_RENDER(Class *cl,struct Gadget *gadg,struct gpRender *gpr);
ULONG EditGadg_GOACTIVE(Class *cl,struct Gadget *gadg,struct gpInput *gpi);
ULONG EditGadg_HANDLEINPUT(Class *cl,struct Gadget *gadg,
  struct gpInput *gpi);
ULONG EditGadg_DIMENSIONS(Class *cl,struct Gadget *gadg,
  struct bmDimensions *dim);
void EditGadg_TogglePixel(struct Gadget *gadg,struct gpInput *gpi,
  struct EditGadgData *egd,WORD i,WORD j,BOOL first);

Class *InitEditGadgClass(void)
{
Class *super,*cl = NULL;

  if ((super = BGUI_GetClassPtr(BGUI_BASE_GADGET)))
  {
    if ((cl = MakeClass(NULL,NULL,super,sizeof(struct EditGadgData),0)))
      cl->cl_Dispatcher.h_Entry = (HOOKFUNC)DispatchEditGadg;
  }
  return cl;
}

void EditGadg_GetSize(struct EditGadgData *egd,UWORD w,UWORD h,
  UWORD *ew_ptr, UWORD *eh_ptr)
{
struct Character *chr;

  *ew_ptr = (w-(2*EG_XOFFSET))/(egd->eg_PixelX);
  *eh_ptr = (h-(2*EG_YOFFSET))/(egd->eg_PixelY);
  if ((chr = egd->eg_Char))
  {
    if (*ew_ptr > chr->chr_Width) *ew_ptr = chr->chr_Width;
    if (*eh_ptr > chr->chr_Height) *eh_ptr = chr->chr_Height;
  }
}

void EditGadg_Draw(Object *o, struct GadgetInfo *gi,ULONG draw)
{
struct RastPort *rp;

  if ((rp = ObtainGIRPort(gi)))
  {
    DoMethod(o,GM_RENDER,gi,rp,draw);
    ReleaseGIRPort(rp);
  }
}

#ifdef __AROS__
AROS_UFH3(ULONG, DispatchEditGadg,
    AROS_UFHA(Class *, cl, A0),
    AROS_UFHA(Object *, o, A2),
    AROS_UFHA(Msg, msg, A1))
#else
SAVEDS ASM ULONG DispatchEditGadg(TF_REGPARAM(a0, Class *, cl), TF_REGPARAM(a2, Object *, o), TF_REGPARAM(a1, Msg, msg))
#endif
{
    AROS_USERFUNC_INIT

ULONG retval;

  switch (msg->MethodID)
  {
    case OM_NEW:
      retval = EditGadg_NEW(cl,o,(struct opSet *)msg);
      break;
    case OM_GET:
      retval = EditGadg_GET(cl,o,(struct opGet *)msg);
      break;
    case OM_SET:
    case OM_UPDATE:
      retval = EditGadg_SET(cl,(struct Gadget *)o,(struct opSet *)msg);
      break;
    case GM_RENDER:
      retval = EditGadg_RENDER(cl,(struct Gadget *)o,(struct gpRender *)msg);
      break;
    case GM_GOACTIVE:
      retval = EditGadg_GOACTIVE(cl,(struct Gadget *)o,
	(struct gpInput *)msg);
      break;
    case GM_HANDLEINPUT:
      retval = EditGadg_HANDLEINPUT(cl,(struct Gadget *)o,
	(struct gpInput *)msg);
      break;
    case BASE_DIMENSIONS:
      retval = EditGadg_DIMENSIONS(cl,(struct Gadget *)o,
	(struct bmDimensions *)msg);
      break;
    default:
      retval = DoSuperMethodA(cl,o,(Msg)msg);
      break;
  }
  return retval;

    AROS_USERFUNC_EXIT
}

ULONG EditGadg_NEW(Class *cl,Object *o,struct opSet *ops)
{
struct EditGadgData *egd;
ULONG retval = 0;

  retval = (ULONG)DoSuperMethodA(cl,o,(Msg)ops);
  if (retval)
  {
    egd = INST_DATA(cl,retval);

    egd->eg_PixelX = GetTagData(EG_PixelX,32,ops->ops_AttrList);
    egd->eg_PixelY = GetTagData(EG_PixelY,16,ops->ops_AttrList);
    egd->eg_Char = (struct Character *)
      GetTagData(EG_CharStruct,0,ops->ops_AttrList);
    egd->eg_XOffset = 0;
    egd->eg_YOffset = 0;
    egd->eg_Node = (struct CharNode *)
      GetTagData(EG_CharNode,0,ops->ops_AttrList);
    egd->eg_Button = 0;
    egd->eg_PixelBorder = GetTagData(EG_PixelBorder,1,ops->ops_AttrList);
    egd->eg_ShowBaseline = GetTagData(EG_ShowBaseline,0,ops->ops_AttrList);
    egd->eg_Toggle = GetTagData(EG_Toggle,0,ops->ops_AttrList);
    egd->eg_Baseline = GetTagData(EG_Baseline,0,ops->ops_AttrList);
    egd->eg_ChangePtr = (BOOL *)GetTagData(EG_ChangePtr,0,ops->ops_AttrList);
  }
  return retval;
}

ULONG EditGadg_RENDER(Class *cl,struct Gadget *gadg,struct gpRender *gpr)
{
struct RastPort *rp;
struct EditGadgData *egd = INST_DATA(cl,(Object *)gadg);
struct DrawInfo *dri = gpr->gpr_GInfo->gi_DrInfo;
struct Character *chr;
UBYTE pen1,pen2,pen3,pen4;
UWORD ew,eh,i,j,x,y,w,h,p;
ULONG retval = 0;

  retval = DoSuperMethodA(cl,(Object *)gadg,(Msg)gpr);
  if (retval == 0)
    return 0;

  rp = gpr->gpr_RPort;
  switch (gpr->gpr_Redraw)
  {
    case GREDRAW_REDRAW:
    case GREDRAW_UPDATE:
      if ((chr = egd->eg_Char) == NULL)
	return retval;

      pen1 = dri->dri_Pens[SHINEPEN];
      pen2 = dri->dri_Pens[SHADOWPEN];
      pen3 = dri->dri_Pens[BACKGROUNDPEN];
      pen4 = dri->dri_Pens[FILLPEN];

      SetAfPt(rp,NULL,0);
      x = gadg->LeftEdge;
      y = gadg->TopEdge;
      w = gadg->Width;
      h = gadg->Height;

      if (gpr->gpr_Redraw == GREDRAW_REDRAW)
      {
	SetAPen(rp,pen3);
	RectFill(rp,x,y,x+w-1,y+h-1);
      }

      EditGadg_GetSize(egd,w,h,&ew,&eh);
      w = (ew*(egd->eg_PixelX))+(2*EG_XOFFSET);
      h = (eh*(egd->eg_PixelY))+(2*EG_YOFFSET);
      p = (egd->eg_PixelBorder != 0) ? 2 : 1;
      Gadg_DrawBevel(rp,x,y,w,h,pen1,pen2);

      if (egd->eg_PixelBorder > 0)
      {
	SetAPen(rp,egd->eg_PixelBorder == 2 ? pen4 : pen3);
	for (j = 0; j < eh-1; j++)
	{
	  Move(rp,x+EG_XOFFSET,((j+1)*egd->eg_PixelY)+y+EG_YOFFSET-1);
	  Draw(rp,x+w-EG_XOFFSET-1,((j+1)*egd->eg_PixelY)+y+EG_YOFFSET-1);
	}
	SetAPen(rp,pen3);
	Move(rp,x+EG_XOFFSET,(eh*egd->eg_PixelY)+y+EG_YOFFSET-1);
	Draw(rp,x+w-EG_XOFFSET-1,(eh*egd->eg_PixelY)+y+EG_YOFFSET-1);
      }

      if ((egd->eg_PixelBorder == 2) && (ew > 0))
      {
	SetAPen(rp,pen4);
	for (i = 0; i < ew-1; i++)
	{
	  Move(rp,((i+1)*egd->eg_PixelX)+x+EG_XOFFSET-1,y+EG_YOFFSET);
	  Draw(rp,((i+1)*egd->eg_PixelX)+x+EG_XOFFSET-1,y+h-EG_YOFFSET-2);
	}
      }

      if (chr->chr_Data)
      {
	for (i = 0; i < ew; i++)
	{
	  for (j = 0; j < eh; j++)
	  {
	    SetAPen(rp,(*(chr->chr_Data+((j+egd->eg_YOffset)*chr->chr_Width)+
	      i+egd->eg_XOffset) != 0) ? pen2 : pen3);
	    RectFill(rp,(i*egd->eg_PixelX)+x+EG_XOFFSET,
	      (j*egd->eg_PixelY)+y+EG_YOFFSET,
	      ((i+1)*egd->eg_PixelX)+x+EG_XOFFSET-p,
	      ((j+1)*egd->eg_PixelY)+y+EG_YOFFSET-p);
	  }
	}
      }

      if (egd->eg_ShowBaseline)
      {
	if (egd->eg_Baseline-egd->eg_YOffset < eh)
	{
	  SetAPen(rp,dri->dri_Pens[SHINEPEN]);
	  Move(rp,x+EG_XOFFSET,((egd->eg_Baseline-egd->eg_YOffset+1)*
	    egd->eg_PixelY)+y+EG_YOFFSET-1);
	  Draw(rp,x+w-EG_XOFFSET-1,((egd->eg_Baseline-egd->eg_YOffset+1)*
	    egd->eg_PixelY)+y+EG_YOFFSET-1);
	}
      }
      break;
  }
  return retval;
}

ULONG EditGadg_GET(Class *cl,Object *o,struct opGet *opg)
{
struct EditGadgData *egd = INST_DATA(cl,o);
UWORD ew,eh;
ULONG retval = 0;

  switch (opg->opg_AttrID)
  {
    case EG_Width:
      EditGadg_GetSize(egd,((struct Gadget *)o)->Width,
	((struct Gadget *)o)->Height,&ew,&eh);
      *(opg->opg_Storage) = ew;
      break;
    case EG_Height:
      EditGadg_GetSize(egd,((struct Gadget *)o)->Width,
	((struct Gadget *)o)->Height,&ew,&eh);
      *(opg->opg_Storage) = eh;
      break;
    case EG_XOffset:
      *(opg->opg_Storage) = egd->eg_XOffset;
      break;
    case EG_YOffset:
      *(opg->opg_Storage) = egd->eg_YOffset;
      break;
    case EG_PixelX:
      *(opg->opg_Storage) = egd->eg_PixelX;
      break;
    case EG_PixelY:
      *(opg->opg_Storage) = egd->eg_PixelY;
      break;
    default:
      retval = DoSuperMethodA(cl,o,(Msg)opg);
      break;
  }
  return retval;
}

ULONG EditGadg_SET(Class *cl,struct Gadget *gadg,struct opSet *ops)
{
struct TagItem *tags, *tag;
ULONG retval,pos;
UWORD ew,eh;
struct EditGadgData *egd = INST_DATA(cl,(Object *)gadg);

  retval = DoSuperMethodA(cl,(Object *)gadg,(Msg)ops);
  EditGadg_GetSize(egd,gadg->Width,gadg->Height,&ew,&eh);
  tags = ops->ops_AttrList;
  if (tags)
  {
    if ((tag = FindTagItem(EG_Baseline,tags)))
      egd->eg_Baseline = tag->ti_Data;
    if ((tag = FindTagItem(EG_CharNode,tags)))
      egd->eg_Node = (struct CharNode *)(tag->ti_Data);
    if ((tag = FindTagItem(EG_CharStruct,tags)))
      egd->eg_Char = (struct Character *)(tag->ti_Data);
    if ((tag = FindTagItem(EG_PixelX,tags)))
      egd->eg_PixelX = tag->ti_Data;
    if ((tag = FindTagItem(EG_PixelY,tags)))
      egd->eg_PixelY = tag->ti_Data;
    if ((tag = FindTagItem(EG_Update,tags)))
    {
      switch (tag->ti_Data)
      {
	case GADG_HORIZ:
	  if (egd->eg_Node)
	  {
	    GetAttr(PGA_Top,egd->eg_Node->chln_HorizGadg,&pos);
	    if (egd->eg_XOffset != pos)
	    {
	      egd->eg_XOffset = pos;
	      EditGadg_Draw((Object *)gadg,ops->ops_GInfo,GREDRAW_UPDATE);
	    }
	  }
	  break;
	case GADG_VERT:
	  if (egd->eg_Node)
	  {
	    GetAttr(PGA_Top,egd->eg_Node->chln_VertGadg,&pos);
	    if (egd->eg_YOffset != pos)
	    {
	      egd->eg_YOffset = pos;
	      EditGadg_Draw((Object *)gadg,ops->ops_GInfo,GREDRAW_UPDATE);
	    }
	  }
	  break;
	case GADG_LEFT:
	  if (egd->eg_XOffset > 0)
	  {
	    if ((egd->eg_Button = 1-egd->eg_Button) == 0)
	    {
	      egd->eg_XOffset--;
	      EditGadg_Draw((Object *)gadg,ops->ops_GInfo,GREDRAW_UPDATE);
	      SetGadgetAttrs((struct Gadget *)egd->eg_Node->chln_HorizGadg,
		egd->eg_Node->chln_Window,NULL,PGA_Top,egd->eg_XOffset,
		TAG_DONE);
	    }
	  }
	  break;
	case GADG_RIGHT:
	  if (egd->eg_XOffset + ew < egd->eg_Char->chr_Width)
	  {
	    if ((egd->eg_Button = 1-egd->eg_Button) == 0)
	    {
	      egd->eg_XOffset++;
	      EditGadg_Draw((Object *)gadg,ops->ops_GInfo,GREDRAW_UPDATE);
	      SetGadgetAttrs((struct Gadget *)egd->eg_Node->chln_HorizGadg,
		egd->eg_Node->chln_Window,NULL,PGA_Top,egd->eg_XOffset,
		TAG_DONE);
	    }
	  }
	  break;
	case GADG_UP:
	  if (egd->eg_YOffset > 0)
	  {
	    if ((egd->eg_Button = 1-egd->eg_Button) == 0)
	    {
	      egd->eg_YOffset--;
	      EditGadg_Draw((Object *)gadg,ops->ops_GInfo,GREDRAW_UPDATE);
	      SetGadgetAttrs((struct Gadget *)egd->eg_Node->chln_VertGadg,
		egd->eg_Node->chln_Window,NULL,PGA_Top,egd->eg_YOffset,
		TAG_DONE);
	    }
	  }
	  break;
	case GADG_DOWN:
	  if (egd->eg_YOffset + eh < egd->eg_Char->chr_Height)
	  {
	    if ((egd->eg_Button = 1-egd->eg_Button) == 0)
	    {
	      egd->eg_YOffset++;
	      EditGadg_Draw((Object *)gadg,ops->ops_GInfo,GREDRAW_UPDATE);
	      SetGadgetAttrs((struct Gadget *)egd->eg_Node->chln_VertGadg,
		egd->eg_Node->chln_Window,NULL,PGA_Top,egd->eg_YOffset,
		TAG_DONE);
	    }
	  }
	  break;
	case GADG_NONE:
	  egd->eg_XOffset = 0;
	  egd->eg_YOffset = 0;
	  EditGadg_Draw((Object *)gadg,ops->ops_GInfo,GREDRAW_REDRAW);
	  break;
      }
    }
    if ((tag = FindTagItem(EG_XOffset,tags)))
    {
      if (egd->eg_XOffset != tag->ti_Data)
      {
	egd->eg_XOffset = tag->ti_Data;
	EditGadg_Draw((Object *)gadg,ops->ops_GInfo,GREDRAW_UPDATE);
      }
    }
    if ((tag = FindTagItem(EG_YOffset,tags)))
    {
      if (egd->eg_YOffset != tag->ti_Data)
      {
	egd->eg_YOffset = tag->ti_Data;
	EditGadg_Draw((Object *)gadg,ops->ops_GInfo,GREDRAW_UPDATE);
      }
    }
  }
  return retval;
}

ULONG EditGadg_GOACTIVE(Class *cl,struct Gadget *gadg,struct gpInput *gpi)
{
struct EditGadgData *egd = INST_DATA(cl,(Object *)gadg);
/*struct DrawInfo *dri = gpi->gpi_GInfo->gi_DrInfo;*/
WORD i,j;
UWORD ew,eh;
ULONG retval;

  retval = DoSuperMethodA(cl,(Object *)gadg,(Msg)gpi);
  EditGadg_GetSize(egd,gadg->Width,gadg->Height,&ew,&eh);
  i = (gpi->gpi_Mouse.X - EG_XOFFSET) / egd->eg_PixelX;
  j = (gpi->gpi_Mouse.Y - EG_YOFFSET) / egd->eg_PixelY;
  if ((i >= 0) && (i < ew) && (j >= 0) && (j < eh))
  {
    EditGadg_TogglePixel(gadg,gpi,egd,i,j,TRUE);
    retval = GMR_MEACTIVE;
  }
  else
    retval = GMR_NOREUSE;
  return retval;
}

ULONG EditGadg_HANDLEINPUT(Class *cl,struct Gadget *gadg,
  struct gpInput *gpi)
{
struct GadgetInfo *gi = gpi->gpi_GInfo;
struct InputEvent *ie = gpi->gpi_IEvent;
struct EditGadgData *egd = INST_DATA(cl,(Object *)gadg);
/*struct DrawInfo *dri = gpi->gpi_GInfo->gi_DrInfo;*/
ULONG retval = GMR_MEACTIVE;
WORD i,j;
UWORD ew,eh;

  DoSuperMethodA(cl,(Object *)gadg,(Msg)gpi);
  if (gi->gi_Window)
  {
    if ((gi->gi_Window->Flags & WFLG_WINDOWACTIVE) == 0)
      return GMR_NOREUSE;
  }

  while (ie && (retval == GMR_MEACTIVE))
  {
    if (ie->ie_Class == IECLASS_RAWMOUSE)
    {
      if (ie->ie_Code != SELECTUP)
      {
	EditGadg_GetSize(egd,gadg->Width,gadg->Height,&ew,&eh);
	i = (gpi->gpi_Mouse.X - EG_XOFFSET) / egd->eg_PixelX;
	j = (gpi->gpi_Mouse.Y - EG_YOFFSET) / egd->eg_PixelY;
	if ((i >= 0) && (i < ew) && (j >= 0) && (j < eh))
	{
	  if ((i != egd->eg_ActiveX) || (j != egd->eg_ActiveY))
	    EditGadg_TogglePixel(gadg,gpi,egd,i,j,FALSE);
	}
	else retval = GMR_NOREUSE;
      }
      else retval = GMR_NOREUSE;
    }
    ie = ie->ie_NextEvent;
  }
  return retval;
}

ULONG EditGadg_DIMENSIONS(Class *cl,struct Gadget *gadg,struct bmDimensions *dim)
{
struct EditGadgData *egd = INST_DATA(cl,gadg);
ULONG retval;

  retval = DoSuperMethodA(cl,(Object *)gadg,(Msg)dim);
  if (retval)
  {
  int mx, my;

    mx = (2*egd->eg_PixelX)+(2*EG_XOFFSET);
    my = (2*egd->eg_PixelY)+(2*EG_YOFFSET);

    dim->bmd_Extent->be_Min.Width  += mx;
    dim->bmd_Extent->be_Min.Height += my;
    dim->bmd_Extent->be_Nom.Width  += mx;
    dim->bmd_Extent->be_Nom.Height += my;
   }
   return retval;
}


void EditGadg_TogglePixel(struct Gadget *gadg,struct gpInput *gpi,
  struct EditGadgData *egd,WORD i,WORD j,BOOL first)
{
struct DrawInfo *dri = gpi->gpi_GInfo->gi_DrInfo;
struct RastPort *rp;
struct Character *chr;
UBYTE *pixel;
UBYTE pen1,pen2;
UWORD px,py;

  egd->eg_ActiveX = i;
  egd->eg_ActiveY = j;
  if ((chr = egd->eg_Char) == NULL) return;
  if (chr->chr_Data == NULL)
  {
    if ((chr->chr_Data = AllocVec(chr->chr_Width*chr->chr_Height,
      MEMF_CLEAR)) == NULL) return;
  }
  pixel =
    chr->chr_Data+((j+egd->eg_YOffset)*chr->chr_Width)+i+egd->eg_XOffset;
  if ((rp = ObtainGIRPort(gpi->gpi_GInfo)))
  {
    pen1 = dri->dri_Pens[SHADOWPEN];
    pen2 = dri->dri_Pens[BACKGROUNDPEN];
    if (egd->eg_PixelBorder == 0)
    {
      px = 1;
      py = 1;
      if (egd->eg_ShowBaseline)
	if (j+egd->eg_YOffset == egd->eg_Baseline) py++;
    }
    else
    {
      px = 2;
      py = 2;
    }
    SetAfPt(rp,NULL,0);
    if (first) egd->eg_FirstPixelState = 1-(*pixel);
    *pixel = egd->eg_Toggle ? 1-(*pixel) : egd->eg_FirstPixelState;
    SetAPen(rp,*pixel == 1 ? pen1 : pen2);
    RectFill(rp,(i*egd->eg_PixelX)+gadg->LeftEdge+EG_XOFFSET,
      (j*egd->eg_PixelY)+gadg->TopEdge+EG_YOFFSET,
      ((i+1)*egd->eg_PixelX)+gadg->LeftEdge+EG_XOFFSET-px,
      ((j+1)*egd->eg_PixelY)+gadg->TopEdge+EG_YOFFSET-py);
    ReleaseGIRPort(rp);
    *(egd->eg_ChangePtr) = TRUE;
  }
}

/* Slider gadget class */

struct SlideGadgData
{
  ULONG sg_Total,sg_Visible;
};

SAVEDS ASM ULONG DispatchSlideGadg(TF_REGPARAM(a0, Class *, cl), TF_REGPARAM(a2, Object *, o), TF_REGPARAM(a1, Msg, msg));
ULONG SlideGadg_NOTIFY(Class *cl,Object *o,struct opUpdate *opu);
ULONG SlideGadg_NEW(Class *cl,Object *o,struct opSet *ops);
ULONG SlideGadg_SET(Class *cl,Object *o,struct opSet *ops);

Class *InitSlideGadgClass(void)
{
Class *super, *cl = NULL;

  if ((super = BGUI_GetClassPtr(BGUI_PROP_GADGET)))
  {
    cl = MakeClass(NULL,NULL,super,0,0);
    if (cl)
      cl->cl_Dispatcher.h_Entry = (HOOKFUNC)DispatchSlideGadg;
  }
  return cl;
}

#ifdef __AROS__
AROS_UFH3(ULONG, DispatchSlideGadg,
    AROS_UFHA(Class *, cl, A0),
    AROS_UFHA(Object *, o, A2),
    AROS_UFHA(Msg, msg, A1))
#else
SAVEDS ASM ULONG DispatchSlideGadg(TF_REGPARAM(a0, Class *, cl), TF_REGPARAM(a2, Object *, o), TF_REGPARAM(a1, Msg, msg))
#endif
{
    AROS_USERFUNC_INIT

ULONG retval;

  switch (msg->MethodID)
  {
    case OM_NEW:
      retval = SlideGadg_NEW(cl,o,(struct opSet *)msg);
      break;
    case OM_SET:
    case OM_UPDATE:
      retval = SlideGadg_SET(cl,o,(struct opSet *)msg);
      break;
    case OM_NOTIFY:
      retval = SlideGadg_NOTIFY(cl,o,(struct opUpdate *)msg);
      break;
    default:
      retval = DoSuperMethodA(cl,o,(Msg)msg);
      break;
  }
  return retval;

    AROS_USERFUNC_EXIT
}

ULONG SlideGadg_NEW(Class *cl,Object *o,struct opSet *ops)
{
struct SlideGadgData *sgd = NULL;
ULONG retval;

  retval = (ULONG)DoSuperMethodA(cl,o,(Msg)ops);
  if (retval)
  {
    sgd = INST_DATA(cl,retval);

    sgd->sg_Total = GetTagData(PGA_Total,0,ops->ops_AttrList);
    sgd->sg_Visible = GetTagData(PGA_Visible,0,ops->ops_AttrList);
  }
  return retval;
}

ULONG SlideGadg_SET(Class *cl,Object *o,struct opSet *ops)
{
struct SlideGadgData *sgd = INST_DATA(cl,o);
struct GadgetInfo *gi = ops->ops_GInfo;
struct TagItem *tags, *tag;
struct TagItem inactive;
ULONG retval;

  retval = DoSuperMethodA(cl,o,(Msg)ops);
  tags = ops->ops_AttrList;
  if (tags)
  {
    if ((tag = FindTagItem(PGA_Total,tags))) sgd->sg_Total = tag->ti_Data;
    if ((tag = FindTagItem(PGA_Visible,tags))) sgd->sg_Visible = tag->ti_Data;
    Gadg_SetTag(&inactive,TAG_DONE,0);
    DoMethod(o,OM_NOTIFY,&inactive,gi,0);
  }
  return retval;
}

ULONG SlideGadg_NOTIFY(Class *cl,Object *o,struct opUpdate *opu)
{
struct SlideGadgData *sgd = INST_DATA(cl,o);
struct TagItem tags[3];
ULONG pos = 0;

  GetAttr(PGA_Top,o,&pos);
  Gadg_SetTag(&tags[0],GA_ID,((struct Gadget *)o)->GadgetID);
  Gadg_SetTag(&tags[1],SCRL_Right,sgd->sg_Total-sgd->sg_Visible-pos);
  if (opu->opu_AttrList == NULL)
    Gadg_SetTag(&tags[2],TAG_DONE,0);
  else
    Gadg_SetTag(&tags[2],TAG_MORE,(ULONG)opu->opu_AttrList);
  return DoSuperMethod(cl,o,OM_NOTIFY,tags,opu->opu_GInfo,opu->opu_Flags);
}

/* This is a listview subclass used for preferences. */

typedef struct
{
  Object *fld_Accept;
  BOOL fld_SortDrops;
} FLD;

#define SET(x)	 ((struct opSet *)x)
#define QUERY(x) ((struct bmDragPoint *)x)
#define DROP(x)	 ((struct bmDropped *)x)

void SetFLAttr(FLD *fld,struct TagItem *attr)
{
struct TagItem *tag,*tstate = attr;

  while ((tag = NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
      case FL_AcceptDrop:
	fld->fld_Accept = (Object *)tag->ti_Data;
	break;
      case FL_SortDrops:
	fld->fld_SortDrops = tag->ti_Data;
	break;
    }
  }
}

#ifdef __AROS__
AROS_UFH3(ULONG, DispatchFL,
    AROS_UFHA(Class *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
#else
SAVEDS ASM ULONG DispatchFL(TF_REGPARAM(a0, Class *, cl), TF_REGPARAM(a2, Object *, obj), TF_REGPARAM(a1, Msg, msg))
#endif
{
    AROS_USERFUNC_INIT

FLD *fld;
APTR entry;
struct IBox *ib;
ULONG rc,spot;

  switch (msg->MethodID)
  {
    case OM_NEW:
      if ((rc = DoSuperMethodA(cl,obj,msg)))
      {
	fld = (FLD *)INST_DATA(cl,rc);
	fld->fld_Accept = NULL;
	fld->fld_SortDrops = FALSE;
	SetFLAttr(fld,SET(msg)->ops_AttrList);
      }
      break;
    case OM_SET:
      rc = DoSuperMethodA(cl,obj,msg);
      fld = (FLD *)INST_DATA(cl,obj);
      SetFLAttr(fld,SET(msg)->ops_AttrList);
      break;
    case BASE_DRAGQUERY:
      if (QUERY(msg)->bmdp_Source == obj)
	return DoSuperMethodA(cl,obj,msg);

      fld = (FLD *)INST_DATA(cl,obj);
      if (QUERY(msg)->bmdp_Source == fld->fld_Accept)
      {
	GetAttr(LISTV_ViewBounds,obj,(ULONG *)&ib);
	if (QUERY(msg)->bmdp_Mouse.X < ib->Width)
	  return BQR_ACCEPT;
      }
      rc = BQR_REJECT;
      break;
    case BASE_DROPPED:
      if (DROP(msg)->bmd_Source == obj)
	return DoSuperMethodA(cl,obj,msg);

      fld = (FLD *)INST_DATA(cl,obj);
      GetAttr( LISTV_DropSpot,obj,&spot);
      while ((entry = (APTR)FirstSelected(DROP(msg)->bmd_Source)))
      {
	if (fld->fld_SortDrops == FALSE)
	  DoMethod(obj,LVM_INSERTSINGLE,NULL,spot,entry,LVASF_SELECT);
	else
	  DoMethod(obj,LVM_ADDSINGLE,NULL,entry,LVAP_SORTED,LVASF_SELECT);
	DoMethod(DROP(msg)->bmd_Source,LVM_REMENTRY,NULL,entry);
      }
      BGUI_DoGadgetMethod(DROP(msg)->bmd_Source,
	DROP(msg)->bmd_SourceWin,
	DROP(msg)->bmd_SourceReq,
	LVM_REFRESH,NULL);
      rc = 1;
      break;
    default:
      rc = DoSuperMethodA(cl,obj,msg);
      break;
  }
  return rc;

    AROS_USERFUNC_EXIT
}

Class *InitFLClass(void)
{
Class *super,*cl = NULL;

  if ((super = BGUI_GetClassPtr(BGUI_LISTVIEW_GADGET)))
  {
    if ((cl = MakeClass(NULL,NULL,super,sizeof(FLD),0)))
      cl->cl_Dispatcher.h_Entry = (HOOKFUNC)DispatchFL;
  }
  return cl;
}
