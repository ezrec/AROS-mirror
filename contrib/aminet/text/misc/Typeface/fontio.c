/************************/
/*			*/
/* Font loading, saving */
/* and previewing.	*/
/*			*/
/************************/

#include "typeface.h"

struct Window *PreviewWnd;
struct ClipboardHandle *Clip, *Undo;
Object *PreviewWndObj, *PreviewStr, *PreviewScroll;
UBYTE *PreviewFont;

struct Window *QueryWnd;
Object *QueryWndObj, *QueryWidthNum;

struct TextFont *tfh;
struct NewMenu PreviewMenus[] = { PROJECT_MENU,FONT_MENU,PREVIEW_MENU,End };

extern struct Screen *Screen;
extern struct MsgPort *WndMsgPort;
extern struct Character CharBuffer[257];
extern struct Window *SaveWnd;
extern struct TextFont *NewFont;
extern Object *SaveWndObj, *FontWndObj;
extern ULONG FirstChar, LastChar;
extern char WinTitle[256];

void OpenPreviewWnd(void)
{
Object *all, *cancel, *update, *clear;

  if (PreviewWnd)
  {
    WindowToFront(PreviewWnd);
    ActivateWindow(PreviewWnd);
    SetPreviewFont();
    SetPreviewScroller();
#ifdef __AROS__
// FIXME: No textfield.gadget in AROS
#else
    ActivateGadget((struct Gadget *)PreviewStr,PreviewWnd,NULL);
#endif
  }
  else
  {
    if (PreviewWndObj == NULL)
    {
      Clip = OpenClipboard(PRIMARY_CLIP);
      Undo = OpenClipboard(1);
      SetupMenus(PreviewMenus);
      PreviewWndObj = WindowObject,
	WINDOW_Screen,Screen,
	WINDOW_SharedPort,WndMsgPort,
	WINDOW_MenuStrip,PreviewMenus,
	WINDOW_Title,GetString(msgPreviewTitle),
	WINDOW_HelpFile,NAME".guide",
	WINDOW_HelpNode,"preview",
	WINDOW_ScaleWidth,70,
	WINDOW_ScaleHeight,70,
	WINDOW_SmartRefresh,TRUE,
	WINDOW_CloseOnEsc,TRUE,
	WINDOW_PreBufferRP,FALSE,
	WINDOW_MasterGroup,
	  VGroupObject,
	    HOffset(SizeX(8)),VOffset(SizeY(4)),Spacing(SizeY(4)),
	    GROUP_BackFill,SHINE_RASTER,
	    StartMember,
	      HGroupObject,
		HOffset(SizeX(8)),VOffset(SizeY(4)),
		ButtonFrame,
		FRM_Recessed,TRUE,
#ifdef __AROS__
// FIXME: No textfield.gadget in AROS
#else
		StartMember,
		  PreviewStr = ExternalObject,
		    EXT_Class,TEXTFIELD_GetClass(),
		    EXT_NoRebuild,TRUE,
		    TEXTFIELD_BlockCursor,TRUE,
		    TEXTFIELD_Border,TEXTFIELD_BORDER_DOUBLEBEVEL,
		    TEXTFIELD_PassCommand,TRUE,
		    TEXTFIELD_NonPrintChars,TRUE,
		    TEXTFIELD_ClipStream,Clip,
		    TEXTFIELD_UndoStream,Undo,
		    GA_ID,ID_PREVIEWSTR,
		  EndObject,
		EndMember,
#endif
		StartMember,
		  PreviewScroll = PropObject,
		    PGA_Top,0,
		    PGA_Total,0,
		    PGA_Visible,0,
		    PGA_Arrows,TRUE,
		    PGA_Freedom,FREEVERT,
		    GA_ID,ID_PREVIEWSCROLL,
		  EndObject,
		  FixWidth(16),
		EndMember,
	      EndObject,
	    EndMember,
	    StartMember,
	      HGroupObject,
		Spacing(SizeX(8)),
		EqualWidth,
		StartMember,
		  update = KeyButton(GetString(gadgUpdate),ID_UPDATE),
		EndMember,
		StartMember,
		  all = KeyButton(GetString(gadgShowAll),ID_ALL),
		EndMember,
        	StartMember,
		  clear = KeyButton(GetString(gadgClear),ID_CLEAR),
		EndMember,
        	StartMember,
		  cancel = KeyButton(GetString(gadgCancel),ID_CANCEL),
		EndMember,
	      EndObject,
	      FixMinHeight,
	    EndMember,
	  EndObject,
	EndObject;
      if (PreviewWndObj == NULL) ErrorCode(NEWWINDOW);
      SetLabelKey(PreviewWndObj,update,gadgUpdate);
      SetLabelKey(PreviewWndObj,all,gadgShowAll);
      SetLabelKey(PreviewWndObj,clear,gadgClear);
      SetLabelKey(PreviewWndObj,cancel,gadgCancel);
    }
    SetPreviewFont();
    if ((PreviewWnd = WindowOpen(PreviewWndObj)) == NULL)
      ErrorCode(OPENWINDOW);
    SetPreviewScroller();
#ifdef __AROS__
// FIXME: No textfield.gadget in AROS
#else
    ActivateGadget((struct Gadget *)PreviewStr,PreviewWnd,NULL);
#endif
  }
}

void ClosePreviewWnd(BOOL all)
{
  ClrWindowClose(&PreviewWndObj,&PreviewWnd);
  if (all)
  {
    ClrDisposeObject(&PreviewWndObj);
    if (Clip) { CloseClipboard(Clip); Clip = NULL; }
    if (Undo) { CloseClipboard(Undo); Undo = NULL; }
    if (PreviewFont) { FreeVec(PreviewFont); PreviewFont = NULL; }
  }
}

void SetPreviewFont()
{
ULONG fchar, lchar;
UBYTE *oldfont;

  oldfont = PreviewFont;
  fchar = FirstChar;
  lchar = LastChar;
  FirstChar = 0;
  LastChar = 255;
  PreviewFont = SaveFont(TRUE,TRUE);
  FirstChar = fchar;
  LastChar = lchar;
#ifdef __AROS__
// FIXME: No textfield.gadget in AROS
// FIXME: When we have textfield.gadget, dont forget that PreviewFont is in <disk format>!!
// FIXME: That is: always big endian! And structure format/aligning like on Amiga. Must convert
// FIXME: the font first if we want to memory-preview it in AROS!!!
#else
  if (PreviewWnd)
  {
    SetGadgetAttrs((struct Gadget *)PreviewStr,PreviewWnd,NULL,
      TEXTFIELD_TextFont,PreviewFont ? tfh : NULL,TAG_DONE);
  }
  else
  {
    SetAttrs(PreviewStr,
      TEXTFIELD_TextFont,PreviewFont ? tfh : NULL,TAG_DONE);
  }
#endif
  if (oldfont) FreeVec(oldfont);
}

void PreviewMsgs(ULONG code)
{
IPTR top;

  switch (code)
  {
    case ID_CLEAR:
#ifdef __AROS__
// FIXME: No textfield.gadget in AROS
#else
      SetGadgetAttrs((struct Gadget *)PreviewStr,PreviewWnd,NULL,
	TEXTFIELD_Text,"",TAG_DONE);
      ActivateGadget((struct Gadget *)PreviewStr,PreviewWnd,NULL);
#endif
      break;
    case ID_ALL:
      PreviewAll();
      break;
    case ID_UPDATE:
      SetPreviewFont();
      break;
    case ID_PREVIEWSCROLL:
      GetAttr(PGA_Top,PreviewScroll,&top);
#ifdef __AROS__
// FIXME: No textfield.gadget in AROS
#else
      SetGadgetAttrs((struct Gadget *)PreviewStr,PreviewWnd,NULL,
	TEXTFIELD_Top,top,TAG_DONE);
#endif
      break;
    case ID_PREVIEWSTR:
      SetPreviewScroller();
      break;
    default:
      SharedMsgs(code,NULL);
      break;
  }
}

void SetPreviewScroller()
{
#ifdef __AROS__
// FIXME: No textfield.gadget in AROS
#else
ULONG lines,visible,top;

  GetAttr(TEXTFIELD_Lines,PreviewStr,&lines);
  GetAttr(TEXTFIELD_Visible,PreviewStr,&visible);
  GetAttr(TEXTFIELD_Top,PreviewStr,&top);
  SetGadgetAttrs((struct Gadget *)PreviewScroll,PreviewWnd,NULL,
    PGA_Total,lines,
    PGA_Visible,visible,
    PGA_Top,top,TAG_DONE);
#endif
}

#define PREVIEW_WIDTH 32

void PreviewAll(void)
{
#ifdef __AROS__
// FIXME: No textfield.gadget in AROS
#else
int i,j;
char listtext[((PREVIEW_WIDTH+1)*(256/PREVIEW_WIDTH))+1];

  for (i = 0; i < 256/PREVIEW_WIDTH; i++)
  {
    for (j = 0; j < PREVIEW_WIDTH; j++)
    {
      switch ((i*PREVIEW_WIDTH)+j)
      {
	case '\0':
	case '\n':
	case 0x0D:
	  listtext[(i*(PREVIEW_WIDTH+1))+j] = ' ';
	  break;
	default:
	  listtext[(i*(PREVIEW_WIDTH+1))+j] = (i*PREVIEW_WIDTH)+j;
	  break;
      }
    }
    listtext[(i*(PREVIEW_WIDTH+1))+PREVIEW_WIDTH] = '\n';
  }
  listtext[((PREVIEW_WIDTH+1)*(256/PREVIEW_WIDTH))] = '\0';
  SetGadgetAttrs((struct Gadget *)PreviewStr,PreviewWnd,NULL,
    TEXTFIELD_Text,listtext,TAG_DONE);
  ActivateGadget((struct Gadget *)PreviewStr,PreviewWnd,NULL);
#endif
}

extern ULONG Width, Height, Baseline, Smear, Aspect;
extern ULONG Proportional, Normal, Bold, Italic, ULine, Extended, Reversed;
extern char FontName[256];
extern WORD SpaceTable[257], KernTable[257];
extern BOOL DataChanged;
extern char SavePath[256];

struct FileDiskFontHeader
{
  UBYTE dfh_DF_ln_Succ[4];
  UBYTE dfh_DF_ln_Pred[4];
  UBYTE dfh_DF_ln_Type;
  BYTE dfh_DF_ln_Pri;
  UBYTE dfh_DF_ln_Name[4];

  UBYTE dfh_FileID[2];
  UBYTE dfh_Revision[2];
  UBYTE dfh_Segment[4];
  UBYTE dfh_Name[MAXFONTNAME];

  UBYTE dfh_TF_tf_Message_mn_Node_ln_Succ[4];
  UBYTE dfh_TF_tf_Message_mn_Node_ln_Pred[4];
  UBYTE dfh_TF_tf_Message_mn_Node_ln_Type;
  BYTE dfh_TF_tf_Message_mn_Node_ln_Pri;
  UBYTE dfh_TF_tf_Message_mn_Node_ln_Name[4];    
  UBYTE dfh_TF_tf_Message_mn_ReplyPort[4];
  UBYTE dfh_TF_tf_Message_mn_Length[2];

  UBYTE dfh_TF_tf_YSize[2];
  UBYTE dfh_TF_tf_Style;
  UBYTE dfh_TF_tf_Flags;
  UBYTE dfh_TF_tf_XSize[2];
  UBYTE dfh_TF_tf_Baseline[2];
  UBYTE dfh_TF_tf_BoldSmear[2];
  UBYTE dfh_TF_tf_Accessors[2];
  UBYTE dfh_TF_tf_LoChar;
  UBYTE dfh_TF_tf_HiChar;
  UBYTE dfh_TF_tf_CharData[4];
  UBYTE dfh_TF_tf_Modulo[2];
  UBYTE dfh_TF_tf_CharLoc[4];
  UBYTE dfh_TF_tf_CharSpace[4];
  UBYTE dfh_TF_tf_CharKern[4];
};

#ifdef __AROS__
#if !AROS_BIG_ENDIAN

void FixFontContentsEndianess(struct FontContentsHeader *fch)
{
  WORD i;
    
  for(i = 0; i < fch->fch_NumEntries; i++)
  {
    if(fch->fch_FileID == FCH_ID)
    {
      struct FontContents *fc;

      fc = (struct FontContents *)(fch + 1);
      fc += i;

      fc->fc_YSize = AROS_WORD2BE(fc->fc_YSize);
    }
    else if ((fch->fch_FileID == TFCH_ID) ||
	     (fch->fch_FileID == OFCH_ID))
    {
      struct TFontContents *tfc;

      tfc = (struct TFontContents *)(fch + 1);
      tfc += i;

      if (tfc->tfc_TagCount)
      {
	ULONG *tags;
	WORD t;

	tags = (ULONG *)(&tfc->tfc_FileName[MAXFONTPATH-(tfc->tfc_TagCount*8)]);
	for (t = 0; t < tfc->tfc_TagCount * 2; t++)
	{
	  tags[t] = AROS_LONG2BE(tags[t]);
	}

      }
      tfc->tfc_TagCount = AROS_WORD2BE(tfc->tfc_TagCount);
      tfc->tfc_YSize = AROS_WORD2BE(tfc->tfc_YSize);

    }

  } /* for(i = 0; i < fch->fch_NumEntries; i++) */

  fch->fch_FileID = AROS_WORD2BE(fch->fch_FileID);
  fch->fch_NumEntries = AROS_WORD2BE(fch->fch_NumEntries);
    
}

#endif
#endif

UBYTE *SaveFont(BOOL tables,BOOL preview)
{
char fontpath[256], filename[256], datestr[LEN_DATSTRING];
ULONG filesize = 0, numchars, bitwidth = 0, i, offbit, hunkstart;
struct DateTime dt;
struct FileDiskFontHeader *dfh;
struct FontContentsHeader *fch;
ULONG *longptr, *relocstart;
UBYTE *buffer, *mptr, *fontdataptr;
struct charDef *fontlocptr;
WORD *fontkernptr;
BPTR fontfile, contfile, lock;

  strcpy(fontpath,SavePath);
  AddPart(fontpath,FontName,256);
  sprintf(filename,"%s/%ld",fontpath,(long)Height);

  DateStamp(&(dt.dat_Stamp));
  dt.dat_Format = FORMAT_CDN;
  dt.dat_Flags = 0;
  dt.dat_StrDay = NULL;
  dt.dat_StrDate = datestr;
  dt.dat_StrTime = NULL;
  DateToStr(&dt);
  while ((mptr = strchr(datestr,'-')) != NULL) *mptr = '.';

  numchars = LastChar-FirstChar+2;
  filesize += 14*4;			/* Hunk structure */
  filesize += sizeof(struct FileDiskFontHeader);
  filesize += 4*4;			/* Relocatable addresses */
  for (i = FirstChar; i < LastChar+1; i++)
    bitwidth += (CharBuffer+i)->chr_Width;
  bitwidth += (CharBuffer+256)->chr_Width;
  if (bitwidth % 16 > 0) bitwidth = ((bitwidth/16)+1)*16;
  filesize += (bitwidth/8)*Height;	/* Character data */
  filesize += numchars*sizeof(struct charDef);
  if (tables)
  {
    filesize += numchars*4;		/* Space and kern tables */
    filesize += 2*4;			/* Table relocs */
  }
  if (filesize % 4 > 0) filesize = ((filesize/4)+1)*4;
  if ((buffer = AllocVec(filesize,MEMF_CLEAR)) != NULL)
  {
    longptr = (ULONG *)buffer;
    *longptr++ = LONG2BE(0x000003F3);		/* Hunk structure */
    *longptr++ = LONG2BE(0x00000000);
    *longptr++ = LONG2BE(0x00000001);
    *longptr++ = LONG2BE(0x00000000);
    *longptr++ = LONG2BE(0x00000000);
    *longptr++ = LONG2BE((filesize - ((tables ? 19 : 17)*4))/4);
    *longptr++ = LONG2BE(0x000003E9);		/* Hunk size (twice) */
    *longptr++ = LONG2BE((filesize - ((tables ? 19 : 17)*4))/4);
    if (preview)
      hunkstart = 0;
    else
      hunkstart = (IPTR)longptr;	/* All relocs relative to here */
    *longptr++ = LONG2BE(0x70FF4E75);		/* MOVEQ #-1,D0 RTS */
    relocstart = (ULONG *)(buffer+filesize-((9+(tables ? 2 : 0))*4));

    dfh = (struct FileDiskFontHeader *)longptr;
    dfh->dfh_DF_ln_Type = NT_FONT;
    *(ULONG *)dfh->dfh_DF_ln_Name = LONG2BE((LONG)((IPTR)dfh->dfh_Name-hunkstart));
    *(UWORD *)dfh->dfh_FileID = WORD2BE(DFH_ID);
    sprintf(dfh->dfh_Name,"$VER: %s%d 39.0 (%s)",FontName,(int)Height,datestr);

    fontdataptr = ((UBYTE *)dfh)+sizeof(struct FileDiskFontHeader);
    fontlocptr = (struct charDef *)(fontdataptr+((bitwidth/8)*Height));
    if ((IPTR)fontlocptr % 2 > 0)
      fontlocptr = (struct charDef *)((((IPTR)fontlocptr/2)+1)*2);
    fontkernptr = (WORD *)((UBYTE *)fontlocptr+
      numchars*sizeof(struct charDef));

#ifdef __AROS__
    tfh = (struct TextFont *)0xBAADC0DE;
#else
    tfh = (struct TextFont *)(&dfh->dfh_TF_tf_Message_mn_Node_ln_Succ);
#endif

    dfh->dfh_TF_tf_Message_mn_Node_ln_Type = NT_FONT;
    *(ULONG *)dfh->dfh_TF_tf_Message_mn_Node_ln_Name = *(ULONG *)dfh->dfh_DF_ln_Name;
    *(UWORD *)dfh->dfh_TF_tf_YSize = WORD2BE(Height);
    if (Normal == FALSE)
    {
      if (Bold) dfh->dfh_TF_tf_Style |= FSF_BOLD;
      if (Italic) dfh->dfh_TF_tf_Style |= FSF_ITALIC;
      if (ULine) dfh->dfh_TF_tf_Style |= FSF_UNDERLINED;
      if (Extended) dfh->dfh_TF_tf_Style |= FSF_EXTENDED;
    }
    else  dfh->dfh_TF_tf_Style= FS_NORMAL;
    dfh->dfh_TF_tf_Flags = FPF_DESIGNED|FPF_DISKFONT;
    if (Proportional) dfh->dfh_TF_tf_Flags |= FPF_PROPORTIONAL;
    if (Reversed) dfh->dfh_TF_tf_Flags |= FPF_REVPATH;
    switch (Aspect)
    {
      case 1:
	dfh->dfh_TF_tf_Flags |= FPF_TALLDOT;
	break;
      case 2:
	dfh->dfh_TF_tf_Flags |= FPF_WIDEDOT;
	break;
    }
    *(UWORD *)dfh->dfh_TF_tf_XSize = WORD2BE(Width);
    *(UWORD *)dfh->dfh_TF_tf_Baseline = WORD2BE(Baseline);
    *(UWORD *)dfh->dfh_TF_tf_BoldSmear = WORD2BE(Smear);
    dfh->dfh_TF_tf_LoChar = FirstChar;
    dfh->dfh_TF_tf_HiChar = LastChar;
    *(ULONG *)dfh->dfh_TF_tf_CharData = LONG2BE((ULONG)((IPTR)fontdataptr-hunkstart));
    *(UWORD *)dfh->dfh_TF_tf_Modulo = WORD2BE(bitwidth/8);
    *(ULONG *)dfh->dfh_TF_tf_CharLoc = LONG2BE((ULONG)((IPTR)fontlocptr-hunkstart));
    if (tables)
    {
      *(ULONG *)dfh->dfh_TF_tf_CharSpace = LONG2BE((LONG)((IPTR)fontkernptr-hunkstart));
      *(ULONG *)dfh->dfh_TF_tf_CharKern = LONG2BE(
	(ULONG)((IPTR)fontkernptr+(numchars*2)-hunkstart));
    }
    for (i = FirstChar, offbit = 0; i < LastChar+1; i++)
    {
      WriteCharData(fontlocptr,fontdataptr,i-FirstChar,i,&offbit,bitwidth/8);

      if (tables)
      {
	*(fontkernptr+(i-FirstChar)) = WORD2BE(SpaceTable[i]);
	*(fontkernptr+(i-FirstChar+numchars)) = WORD2BE(KernTable[i]);
      }
    }

    WriteCharData(fontlocptr,fontdataptr,numchars-1,256,&offbit,bitwidth/8);
    if (tables)
    {
      *(fontkernptr+(numchars-1)) = WORD2BE(SpaceTable[256]);
      *(fontkernptr+(numchars-1+numchars)) = WORD2BE(KernTable[256]);
    }

    *relocstart++ = LONG2BE(0x000003EC);		/* Reloc structure */
    *relocstart++ = LONG2BE(4 + (tables ? 2 : 0));
    *relocstart++ = LONG2BE(0x00000000);
    if (tables)
    {
      *relocstart++ = LONG2BE((IPTR)(&(dfh->dfh_TF_tf_CharKern))-hunkstart);
      *relocstart++ = LONG2BE((IPTR)(&(dfh->dfh_TF_tf_CharSpace))-hunkstart);
    }
    *relocstart++ = LONG2BE((IPTR)(&(dfh->dfh_TF_tf_CharLoc))-hunkstart);
    *relocstart++ = LONG2BE((IPTR)(&(dfh->dfh_TF_tf_CharData))-hunkstart);
    *relocstart++ = LONG2BE((IPTR)(&(dfh->dfh_TF_tf_Message_mn_Node_ln_Name))-hunkstart);
    *relocstart++ = LONG2BE((IPTR)(&(dfh->dfh_DF_ln_Name))-hunkstart);
    *relocstart++ = LONG2BE(0x00000000);
    *relocstart++ = LONG2BE(0x000003F2);

    if (preview) return (buffer);

    WindowBusy(SaveWndObj);
    lock = Lock(fontpath,ACCESS_READ);
    WindowReady(SaveWndObj);
    if (lock == 0)
    {
      if (SaveShowReq(GetString(msgSaveNoDir),GetString(msgSaveNoDirGadgets),
	fontpath))
      {
        WindowBusy(SaveWndObj);
        lock = CreateDir(fontpath);
        WindowReady(SaveWndObj);
	if (lock == 0)
	{
	  SaveShowReq(GetString(msgSaveCreateError),GetString(msgCancel),
	    fontpath);
	  if (buffer) FreeVec(buffer);
	  return 0;
	}
	else UnLock(lock);
      }
      else
      {
	if (buffer) FreeVec(buffer);
	return 0;
      }
    }
    else UnLock(lock);

    WindowBusy(SaveWndObj);
    lock = Lock(filename,ACCESS_READ);
    WindowReady(SaveWndObj);
    if (lock)
    {
      UnLock(lock);
      if (!SaveShowReq(GetString(msgSaveWarnExists),
	GetString(msgSaveWarnExistsGadgets),filename))
      {
	if (buffer) FreeVec(buffer);
	return 0;
      }
    }

    WindowBusy(SaveWndObj);
    fontfile = Open(filename,MODE_NEWFILE);
    WindowReady(SaveWndObj);
    if (fontfile)
    {
      WindowBusy(SaveWndObj);
      Write(fontfile,buffer,filesize);
      Close(fontfile);
      DataChanged = FALSE;

      lock = Lock(SavePath,ACCESS_READ);
      WindowReady(SaveWndObj);
      if (lock != 0)
      {
	sprintf(filename,"%s.font",fontpath);
	WindowBusy(SaveWndObj);
	fch = NewFontContents(lock,FilePart(filename));
	WindowReady(SaveWndObj);
	if (fch != NULL)
	{
	  WindowBusy(SaveWndObj);
	  contfile = Open(filename,MODE_NEWFILE);
	  WindowReady(SaveWndObj);
	  if (contfile != 0)
	  {
	    WindowBusy(SaveWndObj);
#ifdef __AROS__
#if !AROS_BIG_ENDIAN
    	    FixFontContentsEndianess(fch);
#endif
#endif
	    Write(contfile,fch,sizeof(struct FontContentsHeader)+
	      (BE2WORD(fch->fch_NumEntries)*sizeof(struct FontContents)));
	    Close(contfile);
	    WindowReady(SaveWndObj);
	  }
	  else SaveShowReq(GetString(msgSaveWriteHeader),
	    GetString(msgCancel),filename);
	  DisposeFontContents(fch);
	}
	else ShowReq(GetString(msgSaveCreateHeader),GetString(msgCancel));
	UnLock(lock);
      }
    }
    else SaveShowReq(GetString(msgSaveWriteData),GetString(msgCancel),
      filename);
  }
  else SaveShowReq(GetString(msgSaveNoMemory),GetString(msgCancel));
  if (buffer) FreeVec(buffer);
  FlushAllFonts();
  return (NULL);
}

ULONG SaveShowReq(CONST_STRPTR text,CONST_STRPTR gadgets,...)
{
va_list va;
struct EasyStruct req =
  { sizeof(struct EasyStruct),0,NAME,NULL,NULL };
ULONG retcode;

  if (SaveWnd == NULL) return (0);
  WindowBusy(SaveWndObj);
  req.es_TextFormat = text;
  req.es_GadgetFormat = gadgets;
  va_start(va,gadgets);
#if defined(__AROS__) && defined(__ARM_ARCH__)
  #warning "TODO: fix va_arg usage for ARM"
  retcode = 0;
#else
  retcode = EasyRequestArgs(SaveWnd,&req,NULL,va);
#endif
  va_end(va);
  WindowReady(SaveWndObj);
  return (retcode);
}

void WriteCharData(struct charDef *cd, UBYTE *fontdataptr, ULONG dest,
  ULONG src, ULONG *off, ULONG mod)
{
struct Character *chr;
UBYTE *data;
ULONG i,j,width,widthi,modi,offset;

  chr = CharBuffer+src;
  data = (chr->chr_Data == NULL) ?
    (CharBuffer+256)->chr_Data : chr->chr_Data;
  width = chr->chr_Width;
  offset = *off;
  for (i = 0; i < chr->chr_Height; i++)
  {
    modi = mod*i;
    widthi = width*i;
    for (j = 0; j < width; j++)
    {
      if (*(data+widthi+j) == 1)
	*(fontdataptr+modi+((offset+j)>>3)) |= 128>>((offset+j)&7);
    }
  }
#ifdef __AROS__
  ((WORD *)(cd+dest))[0] = WORD2BE(offset);
  ((WORD *)(cd+dest))[1] = WORD2BE(width);
#else
  (cd+dest)->charOffset = offset;
  (cd+dest)->charBitWidth = width;
#endif
  *off += width;
}

void OpenQueryWidthWnd(void)
{
Object *load, *info;
IPTR  args[2];
extern char FontName[256];
extern ULONG Height;

  if (QueryWndObj == NULL)
  {
    QueryWndObj = WindowObject,
      WINDOW_Screen,Screen,
      WINDOW_SharedPort,WndMsgPort,
      WINDOW_Title,GetString(msgQueryWidthTitle),
      WINDOW_HelpFile,NAME".guide",
      WINDOW_HelpNode,"open",
      WINDOW_ScaleWidth,10,
      WINDOW_SmartRefresh,TRUE,
      WINDOW_SizeGadget,FALSE,
      WINDOW_CloseGadget,FALSE,
      WINDOW_RMBTrap,TRUE,
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
		  Spacing(SizeX(8)),
		  StartMember,
		    info = InfoObject,
		      ButtonFrame,
		      FRM_Flags,FRF_RECESSED,
		      Label(GetString(msgQueryFont)),
		      INFO_TextFormat,"%s/%ld",
		      INFO_Args,args,
		      INFO_MinLines,1,
		      INFO_HorizOffset,6,
		      INFO_VertOffset,3,
		      INFO_FixTextWidth,TRUE,
		    EndObject,
		  EndMember,
		  StartMember,
		    QueryWidthNum = StringObject,
		      RidgeFrame,
		      UScoreLabel(GetString(gadgQueryNewWidth),'_'),
		      STRINGA_LongVal,Width,
		      STRINGA_IntegerMin,0,
		      STRINGA_IntegerMax,65535,
		      STRINGA_MaxChars,5,
		      STRINGA_MinCharsVisible,4,
		      GA_ID,ID_QUERYWIDTHNUM,
		    EndObject,
		    Weight(40),
		  EndMember,
		EndObject,
	      EndMember,
	    EndObject,
	  EndMember,
	  StartMember,
	    load = KeyButton(GetString(gadgLoad),ID_ACCEPT),
	    FixMinHeight,
	  EndMember,
	EndObject,
      EndObject;
    if (QueryWndObj == NULL) ErrorCode(NEWWINDOW);
    SetLabelKey(QueryWndObj,QueryWidthNum,gadgQueryNewWidth);
    SetLabelKey(QueryWndObj,load,gadgLoad);
    SetAttrs(QueryWidthNum,STRINGA_BufferPos,1);
    if (Width > 10) SetAttrs(QueryWidthNum,STRINGA_BufferPos,2);
    if (Width > 100) SetAttrs(QueryWidthNum,STRINGA_BufferPos,3);
  }
  args[0] = (IPTR)FontName;
  args[1] = Height;
  if ((QueryWnd = WindowOpen(QueryWndObj)) == NULL) ErrorCode(OPENWINDOW);
  ActivateGadget((struct Gadget *)QueryWidthNum,QueryWnd,NULL);
}

void CloseQueryWidthWnd(void)
{
  ClrWindowClose(&QueryWndObj,&QueryWnd);
  ClrDisposeObject(&QueryWndObj);
}

BOOL LoadFont(struct TextAttr *font, ULONG width)
{
ULONG i;
char *endptr;
struct TTextAttr tagfont;
ULONG tags[3];

  strcpy(FontName,font->ta_Name);
  if ((endptr = strstr(FontName,".font"))) *endptr = 0;

  if (width > 0)
  {
    CopyMem(font,&tagfont,sizeof(struct TextAttr));
    tagfont.tta_Flags &= ~FPF_DESIGNED;
    tagfont.tta_Style |= FSF_TAGGED;
    tagfont.tta_Tags = (struct TagItem *)tags;
    tags[0] = TA_DeviceDPI;
    tags[1] = (width<<16)+tagfont.tta_YSize;
    tags[2] = TAG_DONE;
    NewFont = OpenDiskFont((struct TextAttr *)&tagfont);
  }
  else NewFont = OpenDiskFont(font);

  if (NewFont == NULL)
  {
    ShowReq(GetString(msgNoOpenFont),GetString(msgContinue),FontName,
      (ULONG)font->ta_YSize);
    return FALSE;
  }
  ClearCurrentFont();
  Height = NewFont->tf_YSize;
  Width = NewFont->tf_XSize;
  Baseline = NewFont->tf_Baseline;
  Smear = NewFont->tf_BoldSmear;
  FirstChar = NewFont->tf_LoChar;
  LastChar = NewFont->tf_HiChar;
  Proportional = ((NewFont->tf_Flags & FPF_PROPORTIONAL) != 0);
  Bold = ((NewFont->tf_Style & FSF_BOLD) != 0);
  Italic = ((NewFont->tf_Style & FSF_ITALIC) != 0);
  ULine = ((NewFont->tf_Style & FSF_UNDERLINED) != 0);
  Extended = ((NewFont->tf_Style & FSF_EXTENDED) != 0);
  Normal = ((Bold | Italic | ULine | Extended) == 0);
  Reversed = ((NewFont->tf_Flags & FPF_REVPATH) != 0);
  Aspect = 0;
  if (NewFont->tf_Flags & FPF_TALLDOT) Aspect = 1;
  if (NewFont->tf_Flags & FPF_WIDEDOT) Aspect = 2;

  for (i = 0; i < 257; i++)
  {
    (CharBuffer+i)->chr_Width = (((struct charDef *)(NewFont->tf_CharLoc))
      +LastChar-FirstChar+1)->charBitWidth;
    (CharBuffer+i)->chr_Height = NewFont->tf_YSize;
    SpaceTable[i] = NewFont->tf_XSize;
    KernTable[i] = 0;
    KernTables(i,LastChar-FirstChar+1);
  }
  for (i = 0; i < LastChar-FirstChar+1; i++)
  {
    if (!UnpackChar(CharBuffer+FirstChar+i,NewFont,i)) ErrorCode(ALLOCVEC);
    KernTables(FirstChar+i,i);
  }
  UnpackChar(CharBuffer+256,NewFont,LastChar-FirstChar+1);
  KernTables(256,LastChar-FirstChar+1);
  sprintf(WinTitle,"%s/%ld",FontName,(long)font->ta_YSize);
  SetAttrs(FontWndObj,WINDOW_Title,WinTitle,TAG_DONE);
  CloseFont(NewFont); NewFont = NULL;
  DataChanged = FALSE;
  return TRUE;
}
