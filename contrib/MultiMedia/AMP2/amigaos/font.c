/*
 *
 * font.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include <proto/diskfont.h>

#if !defined(__AROS__)
#include <powerup/ppcproto/intuition.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/time.h>
#include <powerup/gcclib/powerup_protos.h>
#include <powerup/ppcproto/exec.h>
#include <powerup/ppcinline/graphics.h>
#include <powerup/ppcproto/dos.h>
#include <powerup/ppcproto/asl.h>
#include <powerup/ppcproto/gadtools.h>
#else

#include "aros-inc.h"

#endif

#include <graphics/displayinfo.h>

#include "../refresh/osd.h"

#if !defined(__AROS__)
extern struct Library *GfxBase;
#endif
extern struct Library *DiskfontBase;

struct charDef
{
  WORD charOffset,charBitWidth;
};

struct Character
{
  UWORD chr_Width,chr_Height;
  UBYTE *chr_Data;
};

#define GETBIT(buffer,offset) \
  ((((*((UBYTE *)(buffer)+((offset)>>3)))&(128>>((offset)&7))) != 0) \
  ? 1 : 0)

static ULONG Proportional, Normal, Bold, Italic, ULine, Extended, Reversed;
static ULONG Height, Width, Baseline, Smear, Aspect;
static WORD SpaceTable[257], KernTable[257];
static ULONG FirstChar, LastChar, UseTable;
static struct TextFont *NewFont = NULL;
static struct Character CharBuffer[257];
static struct FontRequester *FontReq = NULL;
static struct TextAttr topaz8 = {"topaz.font", 8, FS_NORMAL, FPF_ROMFONT};

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

BOOL LoadFont(struct TextAttr *font, ULONG width)
{
  ULONG i;
  struct TTextAttr tagfont;
  ULONG tags[3];

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
    printf("failed to load font\n");
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
    if (!UnpackChar(CharBuffer+FirstChar+i,NewFont,i)) {
      printf("out of memory\n");
      return FALSE;
    }
    KernTables(FirstChar+i,i);

    /* copy font stuff to OSD font */
    osd_chars[i+FirstChar].width = ((struct Character *)(CharBuffer+FirstChar+i))->chr_Width;
    osd_chars[i+FirstChar].height = ((struct Character *)(CharBuffer+FirstChar+i))->chr_Height;
    osd_chars[i+FirstChar].data = ((struct Character *)(CharBuffer+FirstChar+i))->chr_Data;
    osd_chars[i+FirstChar].space = SpaceTable[i+FirstChar];
    osd_chars[i+FirstChar].kern = KernTable[i+FirstChar];
  }
  UnpackChar(CharBuffer+256,NewFont,LastChar-FirstChar+1);
  KernTables(256,LastChar-FirstChar+1);
  CloseFont(NewFont); NewFont = NULL;
  return TRUE;
}

static char font_title[] = "Select subtitle font";

int osd_font_init(void)
{
  int use_fallback = 1;

  memset(osd_chars, 0, sizeof(osd_chars));
  memset(SpaceTable, 0, sizeof(SpaceTable));
  memset(KernTable, 0, sizeof(KernTable));
  memset(CharBuffer, 0, sizeof(CharBuffer));

  FontReq = AllocAslRequestTags(ASL_FontRequest, TAG_END);
  if (FontReq == NULL) {
    printf("failed to allocate font requester\n");
  } else {
    if (AslRequestTags(FontReq, ASLFO_TitleText, font_title, ASLFO_MaxHeight, 144, TAG_END)) {
      LoadFont(&FontReq->fo_Attr, 0);
      use_fallback = 0;
    }
    FreeAslRequest(FontReq);
    FontReq = NULL;
  }

  if (use_fallback) {
    LoadFont(&topaz8, 0);
  }

  return 0;
}

void osd_font_exit(void)
{
  ClearCurrentFont();
}
