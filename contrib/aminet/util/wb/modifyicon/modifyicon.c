#define NAME         "ModifyIcon"
#define VERSION      "1"
#define REVISION     "1"
#define DISTRIBUTION "(GPL) "
#define DATE	     "24.02.2002"
#define AUTHOR       "by Dirk Stöcker <stoecker@epost.de>"

/* Programmheader

	Name:		ModifyIcon
	Author:		SDI
	Distribution:	PD
	Description:	modifies Amiga icons

 1.0   24.02.02 : first version, handles all files totally itself
 1.1   24.02.02 : fixed bug which could trash tooltypes and default tool
*/
//#define DEBUG /* turn debugging on */
//#define CHECK   /* turn on data checks, this will report any unknown type */
                  /* of file and disable writing of unsecure files. */

/* ToDo:
- add RemapImage function -->
  WORD dest[256];
  first pass  --> each used palette entry is set to 1
  second pass --> each entry equal 1 is set to the first entry with
                  this color-palette.
  --> Each color ist transformed to the number in this list
  --> NumColors is reduced to required value
  (transparency color is treated different!)
- SortColors --> Sorts the colors according to their values
  - transparency color is always first
  - use same field as RemapImage
- Show Icon information
- Cleanup/Optimize
  - ga_Width/ga_Height an Images anpassen
  - ToolTypes optimieren
  - Image structure reduzieren. BitMap:Ungenutzte Bits loeschen.
- Zusammenlegung der Paletten bei OS3.5-Icons
  - Depth nicht erhoehen
  - Gesamtgroesse der Datei muss kleiner werden
    (2 komprimierte Paletten groesser als 1 zusammengelegte komprimierte)
  - Transparentcolor zu einer zusammenfassen (auch bei verschiedenen
    Farbwerten)
- FixOldIcon - prepares images/width/... to fit old icon requirements
- FixNewIcon - prepares images/width/... to fit NewIcon requirements
- FixO35Icon - prepares images/width/... to fit OS3.5 icon requirements
- Options of ProcessIcon
ICON/A,REFICON,VIEW/S,OPT=OPTIMIZE/S,US=UNSNAP/S,REMAP/S,SHOWTYPE/S,
NI2CI/S,MWB2CI/S,ROM2CI/S,II2CI/S,MAKECI/S,SBP=SETBITPLANES/N,
SDT=SETDEFTOOL,SIT=SETICONTYPE,SSS=SETSTACKSIZE/N,ST1=SETTRANSPARENT1/N,
ST2=SETTRANSPARENT2/N,SXP=SETXPOS/N,SYP=SETYPOS/N,KCI=KILLCOLORICON/S,
KDD=KILLDRAWERDATA/S,KDT=KILLDEFTOOL/S,KII=KILLICONIMAGE/S,KNI=KILLNEWICON/S,
KSS=KILLSTACKSIZE/S,KTT=KILLTOOLTYPES/S,CCI=COPYCOLORICON/S,
CDD=COPYDRAWERDATA/S,CDT=COPYDEFTOOL/S,CII=COPYICONIMAGE/S,
CIT=COPYICONTYPE/S,CNI=COPYNEWICON/S,CSS=COPYSTACKSIZE/S,CTT=COPYTOOLTYPES/S
    STRPTR      icon;
    STRPTR      reficon;
    ULONG       view;
    ULONG       optimize;
    ULONG       unsnap;
    ULONG       remap;
    ULONG	showtype;
    ULONG       ni2ci;
    ULONG       mwb2ci;
    ULONG       rom2ci;
    ULONG       ii2ci;
    ULONG       makeci;
    ULONG *     setbitplanes;
    STRPTR      setdeftool;
    STRPTR      seticontype;
    ULONG *     setstacksize;
    LONG *	settransparent1;
    LONG *      settransparent2;
    ULONG *     setxpos;
    ULONG *     setypos;
    ULONG       killcoloricon;
    ULONG       killdrawerdata;
    ULONG       killdeftool;
    ULONG       killiconimage;
    ULONG       killnewicon;
    ULONG       killstacksize;
    ULONG       killtooltypes;
    ULONG       copycoloricon;
    ULONG       copydrawerdata;
    ULONG       copydeftool;
    ULONG       copyiconimage;
    ULONG       copyicontype;
    ULONG       copynewicon;
    ULONG       copystacksize;
    ULONG       copytooltypes;
*/

/* The icon-processing and conversion functions are portable. Only the
   interface is Amiga design.
   It is possibly dangerous to use this tool, as it relies on undocumented
   system structures, but the changes from OS2.0 to OS3.5 broke the older
   tools also. So this tool has better control over the structures without
   decreased reliability ;-)
   NOTE: This is mainly a study, what can be done. So if you would like to
   have other options or do additions, feel free to contact me. If you
   have problems with result or input files, please send them for tests.
   This tool may handle certain files wrong, as it is based on structure
   observation and not on documentation.
*/

#define MAXBUFFERSIZE        100000
#define MAXICONSIZE          256
#define MAXDEPTH             8
#define MAXCOLORS            (1<<MAXDEPTH)
			     /* note, this must be in sync with MAXDEPTH, or there will be problems */
#define MAXCOLORSNI          257 /* addition by 1 to support some strange NewIcons */
#define MAXTOOLTYPES         256
#define NOPOSITION           0x80000000

#define	ICONTYPE_DISK        1
#define	ICONTYPE_DRAWER      2
#define	ICONTYPE_TOOL        3
#define	ICONTYPE_PROJECT     4
#define	ICONTYPE_GARBAGE     5
#define	ICONTYPE_DEVICE      6 /* should never appear */
#define	ICONTYPE_KICK	     7
#define ICONYTPE_APPICON     8 /* should never appear */

enum IconMode { ICONMODE_COMPLEMENT, ICONMODE_BACKFILL, ICONMODE_IMAGE };
enum OSType { OSTYPE_13, OSTYPE_20, OSTYPE_35 };
enum PaletteMode { PALETTE_DEFAULT13, PALETTE_DEFAULT20, PALETTE_MAGICWB, PALETTE_ROM, PALETTE_AUTO };
enum DecodeType { DECODE_PALETTE, DECODE_IMAGE };

struct IconPalette {
  unsigned char      Red;
  unsigned char      Green;
  unsigned char      Blue;
};

struct IconImage {
  long               Depth;
  long               NumColors;
  long               Width;
  long               Height;
  /* this is either chunky or planar - depending on Depth == 0 or not */
  unsigned char      ImageData[MAXICONSIZE*MAXICONSIZE*MAXDEPTH/8];
  struct IconPalette Palette[MAXCOLORS];
  long               Transparency; /* Transparent color or -1 */
#ifdef CHECK
  long               Unknown;
#endif
};

struct DrawerInfo {
  long               XPosition;
  long               YPosition;
  long               LeftEdge;
  long               TopEdge;
  long               Width;
  long               Height;
  long               ViewMode;		/* which values? */
  long               Flags;		/* which values? */
#ifdef CHECK
  long               Unknown;
#endif
};

struct IconRepresentation {
  long               Buffersize;
  unsigned char      Buffer[MAXBUFFERSIZE];
  long               Width;
  long               Height;
  long               XPosition;
  long               YPosition;
  long               StackSize;
  enum IconMode      IconMode;
  enum OSType        OSType;
  char               Type;
  char               AspectRatio;
  char               OS35Flags;
  char *             DefaultTool;
  char *             ToolTypes[MAXTOOLTYPES];
  struct IconImage   OldImage[2];
  struct IconImage   OldImageChunky[2];
  struct IconImage   NewIconImage[2];
  struct IconImage   NewImage[2];
  struct DrawerInfo  Drawer;
#ifdef CHECK
  long               Unknown;
#endif
};
#ifdef CHECK
#define IsUnknown(ico) ((ico)->Unknown+(ico)->OldImage[0].Unknown+(ico)->OldImage[1].Unknown+ \
                        (ico)->NewIconImage[0].Unknown + (ico)->NewIconImage[1].Unknown + \
                        (ico)->NewImage[0].Unknown + (ico)->NewImage[1].Unknown + \
                        (ico)->Drawer.Unknown+(ico)->OldImageChunky[0].Unknown+ \
                        (ico)->OldImageChunky[1].Unknown)
#endif

#define sqr(a)  ((a)*(a))

static char *ReadIcon(struct IconRepresentation *ico);
static char *ReadOldImage(unsigned char *buffer, struct IconImage *im);
static char *ReadDrawerData(unsigned char *buffer, struct DrawerInfo *drw);
static char *ProcessNewIcon(struct IconRepresentation *ico);
static char *ReadNewStyleIcon(unsigned char *buffer, struct IconRepresentation *ico);
static char *ProcessNewIconImage(unsigned char **tt, struct IconImage *im, unsigned char num);
static void RemoveToolType(unsigned char **tt);
static void SetOldPalette(struct IconRepresentation *ico, enum PaletteMode mode);
static char *DecodeNewIcon(unsigned char **tt, struct IconImage *im, enum DecodeType type, unsigned char num);
static void Chunky2Planar(struct IconImage *from, struct IconImage *to);
static void SetPalette(struct IconImage *im, const struct IconPalette *pal, long num);
static int mystrcmp(char *a, char *b);
static char mytolower(char a);
static int mystricmp(char *a, char *b);
static char *DecodeNew(unsigned char *buffer, long numbytes, long bits, struct IconImage *im, enum DecodeType type,
       long mode);

static long CreateIcon(struct IconRepresentation *ico, unsigned char *buffer);
static long CreateRLE(unsigned char *buf, unsigned long depth,
unsigned char *dtype, unsigned char *dsize, unsigned long size,
unsigned char *src);
static long CreateNew(struct IconRepresentation *ico, unsigned char *buf);
static void CreateNewIcon(unsigned char *buf, unsigned char num, long *size,
long *numtt, struct IconImage *im);

static void ReduceNumColors(struct IconImage *im);
static char *OptimizeAndFix(struct IconRepresentation *ico);

/****************************************************************************/
/*     The Amiga specific interface                                         */
/****************************************************************************/

#ifdef AMIGA
#include <exec/memory.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#define PARAM   "FROM/A,TO/K,PAL=PALETTE/K,VIEW/S,SAVE/S"
#define version "$VER: " NAME " " VERSION "." REVISION " (" DATE ") " DISTRIBUTION AUTHOR

struct Args {
  STRPTR from;
  STRPTR to;
  STRPTR palette;
  ULONG  view;
  ULONG  save;

  /* preparsed args */
  enum PaletteMode palettemode;
};

#ifdef DEBUG
struct DosLibrary *DOSBase;
#endif

static char *ViewImages(struct IconRepresentation *ico);
static void ViewOneImage(struct GfxBase *GfxBase, LONG *pens, struct IconImage *im,
            LONG x, LONG y, struct Window *win);
static int  CheckArgs(struct Args *args);
static void MakeName(char *buf, char *name);

#ifdef __AROS__
ULONG main(void)
#else
ULONG start(void)
#endif
{
  ULONG ret = RETURN_FAIL;

#ifndef __AROS__
#ifndef DEBUG
  struct DosLibrary *DOSBase;
#endif
  struct ExecBase * SysBase = (*((struct ExecBase **) 4));

  { /* test for WB and reply startup-message */
    struct Process *task;
    if(!(task = (struct Process *) FindTask(0))->pr_CLI)
    {
      WaitPort(&task->pr_MsgPort);
      Forbid();
      ReplyMsg(GetMsg(&task->pr_MsgPort));
      return RETURN_FAIL;
    }
  }

  if((DOSBase = (struct DosLibrary *) OpenLibrary("dos.library", 37)))
  {
#endif
    struct RDArgs *rda;
    LONG i;
    struct Args args;
    struct IconRepresentation *ico;

    args.from = version; /* is deleted by optimizer */
    /* clear Args structure */
    for(i = 0; i < sizeof(struct Args)/4; ++i)
      ((ULONG *)&args)[i] = 0;

    if((ico = (struct IconRepresentation *)
    AllocMem(sizeof(struct IconRepresentation), MEMF_CLEAR)))
    {
      if((rda = ReadArgs(PARAM, (LONG *) &args, 0)))
      {
        if(CheckArgs(&args))
        {
          BPTR file;
          char *text;

          MakeName(ico->Buffer, args.from);
          if((file = Open(ico->Buffer, MODE_OLDFILE)))
          {
            ico->Buffersize = Read(file, ico->Buffer, MAXBUFFERSIZE);
            if(ico->Buffersize == MAXBUFFERSIZE || ico->Buffersize <= 0)
              Printf("Failed reading icon.\n");
            else
            {
              Close(file); file = 0;
              text = ReadIcon(ico);
              if(!text)
              {
                SetOldPalette(ico, args.palettemode);
                Chunky2Planar(ico->OldImage+0, ico->OldImageChunky+0);
                Chunky2Planar(ico->OldImage+1, ico->OldImageChunky+1);
              }
              /* Here all the modifications must be done! */
              if(!text) text = OptimizeAndFix(ico);
#ifdef CHECK
              if(IsUnknown(ico))
                Printf("Icon contains unknown data, modifications disabled!\n");
              else
#endif
              /* here a test for NewIcon/OS3.5 boundary checks need to be done */
              /* before it can be saved! */
              if(!text && args.save)
              {
                unsigned char *buf;
                long size;

                if((buf = AllocMem(MAXBUFFERSIZE, MEMF_ANY)))
                {
                  MakeName(buf, args.to);
                  if((file = Open(buf, MODE_NEWFILE)))
                  {
                    size = CreateIcon(ico, buf);
                    if((Write(file, buf, size) != size))
                      text = "failed writing icon.";
                  }
                  else
                    text = "failed open destination icon.";
                  FreeMem(buf, MAXBUFFERSIZE);
                }
                else
                  text = "failed to get output memory.";
              }
              if(!text && args.view)
                text = ViewImages(ico);
              if(text)
                Printf("Error: %s\n", text);
            }
            if(file)
              Close(file);
          }
          else
            Printf("Could not open icon.\n");
        }

        FreeArgs(rda);
        ret = RETURN_OK;
      }
      else
        Printf("Illegal Parameters.\n");

      FreeMem(ico, sizeof(struct IconRepresentation));
    }
    else
      Printf("Not enough memory.\n");
#ifndef __AROS__
    CloseLibrary((struct Library *) DOSBase);
  }
#endif  
  return ret;
}

static void MakeName(char *buf, char *name)
{
  int i;

  for(i = 0; name[i]; ++i)
    *(buf++) = name[i];
  if(*(buf-1) == '/')
    --buf;
  else if(*(buf-1) == ':')
  {
    *(buf++) = 'D';
    *(buf++) = 'i';
    *(buf++) = 's';
    *(buf++) = 'k';
  }

  if(i <= 5 || mystrcmp(buf-5, ".info"))
  {
    *(buf++) = '.';
    *(buf++) = 'i';
    *(buf++) = 'n';
    *(buf++) = 'f';
    *(buf++) = 'o';
  }
  *buf = 0;
}

static int CheckArgs(struct Args *args)
{
  if(args->palette)
  {
    if(!mystricmp(args->palette, "OS13"))
      args->palettemode = PALETTE_DEFAULT13;
    else if(!mystricmp(args->palette, "OS20"))
      args->palettemode = PALETTE_DEFAULT20;
    else if(!mystricmp(args->palette, "MagicWB") || !mystricmp(args->palette, "Magic"))
      args->palettemode = PALETTE_MAGICWB;
    else if(!mystricmp(args->palette, "ROM") || !mystricmp(args->palette, "ROMIcon"))
      args->palettemode = PALETTE_ROM;
    else if(!mystricmp(args->palette, "Auto"))
      args->palettemode = PALETTE_AUTO;
    else
    {
      Printf("PALETTE can be OS13, OS20, MagicWB, ROM or Auto.\n");
      return 0;
    }
  }
  else
    args->palettemode = PALETTE_AUTO;

  if(!args->to && args->save)
    args->to = args->from;

  return 1;
}

#define MAKEPEN(a) (((a)<<24)|((a)<<16)|((a)<<8)|(a))
static void ViewOneImage(struct GfxBase *GfxBase, LONG *pens, struct IconImage *im,
LONG x, LONG y, struct Window *win)
{
  struct ColorMap *m;
  LONG i, j, r;

  if(!im->Width || !im->Width || !im->NumColors)
    return;

  m = win->WScreen->ViewPort.ColorMap;
  for(i = 0; i < im->NumColors; ++i)
  {
    pens[i] = ObtainBestPenA(m, MAKEPEN(im->Palette[i].Red),
    MAKEPEN(im->Palette[i].Green), MAKEPEN(im->Palette[i].Blue), 0);
  }

  for(i = 0; i < im->Height; ++i)
  {
    for(j = 0; j < im->Width; ++j)
    {
      r = im->ImageData[i*im->Width+j];
      if(r >= im->NumColors)
      {
#ifdef DEBUG
Printf("Error %ld >= %ld\n", r, im->NumColors);
#endif
        r = 0;
      }
      SetAPen(win->RPort, pens[r]);
      WritePixel(win->RPort, x+j, y+i);
    }
  }
}

static char *ViewImages(struct IconRepresentation *ico)
{
  char *ret = 0;
#ifndef __AROS__
  struct ExecBase * SysBase = (*((struct ExecBase **) 4));
#endif
  struct IntuitionBase * IntuitionBase;
  struct GfxBase *GfxBase;

  if((IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 37)))
  {
    if((GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 37)))
    {
      LONG so, sn, sc, width, height, mh, sec;
      struct Window *win;

      so = ico->OldImageChunky[0].Width;
      if(so < ico->OldImageChunky[1].Width)
        so = ico->OldImageChunky[1].Width;
      sn = ico->NewIconImage[0].Width;
      if(sn < ico->NewIconImage[1].Width)
        sn = ico->NewIconImage[1].Width;
      sc = ico->NewImage[0].Width;
      if(sc < ico->NewImage[1].Width)
        sc = ico->NewImage[1].Width;
      width = so + sn + sc + 20;

      sec = ico->OldImageChunky[1].NumColors + ico->NewIconImage[1].NumColors
      + ico->NewImage[1].NumColors;

      mh = ico->OldImageChunky[0].Height;
      if(mh < ico->OldImageChunky[1].Height) mh = ico->OldImageChunky[1].Height;
      if(mh < ico->NewIconImage[0].Height) mh = ico->NewIconImage[0].Height;
      if(mh < ico->NewIconImage[1].Height) mh = ico->NewIconImage[1].Height;
      if(mh < ico->NewImage[0].Height) mh = ico->NewImage[0].Height;
      if(mh < ico->NewImage[1].Height) mh = ico->NewImage[1].Height;

      height = (sec ? mh*2+15 : mh+10);

      if((win = OpenWindowTags(0, WA_CloseGadget, TRUE, WA_DepthGadget, TRUE,
      WA_DragBar, TRUE, WA_SizeGadget, FALSE, WA_SmartRefresh, TRUE, 
      WA_Title, "ModifyIcon", WA_InnerWidth, width, WA_InnerHeight, height,
      WA_GimmeZeroZero, TRUE, WA_IDCMP, IDCMP_CLOSEWINDOW, TAG_DONE)))
      {
        LONG numpens;
        LONG *pens;

        numpens =
          ico->OldImageChunky[0].NumColors +
          ico->NewIconImage[0].NumColors +
          ico->NewImage[0].NumColors + sec;
        if((pens = (LONG *) AllocMem(numpens*sizeof(LONG), MEMF_ANY)))
        {
          LONG i, j, penpos, mypen;

          ViewOneImage(GfxBase, pens, ico->OldImageChunky+0, 5, 5, win);
          penpos = ico->OldImageChunky[0].NumColors;
          ViewOneImage(GfxBase, pens+penpos, ico->OldImageChunky+1, 5, 10+mh, win);
          penpos += ico->OldImageChunky[1].NumColors;

          ViewOneImage(GfxBase, pens+penpos, ico->NewIconImage+0, 10+so, 5, win);
          penpos = ico->NewIconImage[0].NumColors;
          ViewOneImage(GfxBase, pens+penpos, ico->NewIconImage+1, 10+so, 10+mh, win);
          penpos += ico->NewIconImage[1].NumColors;

          ViewOneImage(GfxBase, pens+penpos, ico->NewImage+0, 15+so+sn, 5, win);
          penpos = ico->NewImage[0].NumColors;
          ViewOneImage(GfxBase, pens+penpos, ico->NewImage+1, 15+so+sn, 10+mh, win);
       /* penpos += ico->NewImage[1].NumColors; */

          mypen = ObtainBestPenA(win->WScreen->ViewPort.ColorMap, 0, 0, 0, 0);
          SetAPen(win->RPort, mypen);
          j = 2; i = 2+(sec?mh*2+10:mh+5);
          Move(win->RPort,j,2); Draw(win->RPort,j,i); j += so+5;
          Move(win->RPort,j,2); Draw(win->RPort,j,i); j += sn+5;
          Move(win->RPort,j,2); Draw(win->RPort,j,i); j += sc+5;
          Move(win->RPort,j,2); Draw(win->RPort,j,i);
          i = 2;
          Move(win->RPort,2,i); Draw(win->RPort,j,i); i += mh+5;
          Move(win->RPort,2,i); Draw(win->RPort,j,i);
          if(sec)
          {
            i += mh+5;
            Move(win->RPort,2,i); Draw(win->RPort,j,i);
          }
          Wait((1L<<win->UserPort->mp_SigBit)|SIGBREAKF_CTRL_C);

          ReleasePen(win->WScreen->ViewPort.ColorMap, mypen);
          for(i = 0; i < numpens; ++i)
            ReleasePen(win->WScreen->ViewPort.ColorMap, pens[i]);

          FreeMem(pens, numpens*sizeof(LONG));
        }
        else
          ret = "could not allocate pen memory";

        CloseWindow(win);
      }
      else
        ret = "could not open window.";
      CloseLibrary((struct Library *) GfxBase);
    }
    else
      ret = "could not open graphics";
    CloseLibrary((struct Library *) IntuitionBase);
  }
  else
    ret = "could not open intuition.";
  return ret;
}
#else /* AMIGA */
#define Printf printf
int main(int argc, char **argv)
{
  printf("nothing done yet\n");
  return 0;
}
#endif /* !AMIGA */

/****************************************************************************/
/*     System independant support stuff                                     */
/****************************************************************************/

#define EndGetM32(a)  (((((unsigned char *)a)[0])<<24)|((((unsigned char *)a)[1])<<16)| \
                       ((((unsigned char *)a)[2])<< 8)|((((unsigned char *)a)[3])))
#define EndGetM16(a)  (((((unsigned char *)a)[0])<< 8)|((((unsigned char *)a)[1])))

#define EndPutM32(a, b) {unsigned long epu32 = (b); (a)[0] = (unsigned char) (epu32 >> 24); \
                         (a)[1] = (unsigned char) (epu32 >> 16); \
                         (a)[2] = (unsigned char) (epu32 >> 8); \
                         (a)[3] = (unsigned char) epu32;}
#define EndPutM16(a, b) {unsigned short epu16 = (b); (a)[0] = (unsigned char) (epu16 >> 8); \
                         (a)[1] = (unsigned char) epu16;}
#define EndPutM32Inc(a, b) {EndPutM32(a,b); (a) += 4;}
#define EndPutM16Inc(a, b) {EndPutM16(a,b); (a) += 2;}

/* structure Gadget:
  APTR  ga_Next            0x00    0
  WORD  ga_LeftEdge        0x04    undefined
  WORD  ga_TopEdge         0x06    undefined
  WORD  ga_Width           0x08    *
  WORD  ga_Height          0x0A    *
  UWORD ga_Flags           0x0C    4+ (2 or 1 or 0)
  UWORD ga_Activation      0x0E    undefined
  UWORD ga_GadgetType      0x10    undefined
  APTR  ga_GadgetRender    0x12    != 0
  APTR  ga_SelectRender    0x16    (0 or != 0)
  APTR  ga_GadgetText      0x1A    undefined
  LONG  ga_MutualExclude   0x1E    undefined
  APTR  ga_SpecialInfo     0x22    undefined
  UWORD ga_GadgetID        0x26    undefined
  APTR  ga_UserData        0x28    lower 8 bit: 0 or 1 if OS2.0 gadget (for drawer data)
                           0x2C
*/

/* structure NewWindow:
  WORD  nw_LeftEdge        0x00    *
  WORD  nw_TopEdge         0x02    *
  WORD  nw_Width           0x04    *
  WORD  nw_Height          0x06    *
  UBYTE nw_DetailPen       0x08    255
  UBYTE nw_BlockPen        0x09    255
  ULONG nw_IDCMPFlags      0x0A    undefined
  ULONG nw_Flags           0x0E    undefined
  APTR  nw_FirstGadget     0x12    undefined
  APTR  nw_CheckMark       0x16    undefined
  APTR  nw_Title           0x1A    undefined
  APTR  nw_Screen          0x1E    undefined
  APTR  nw_BitMap          0x22    undefined
  WORD  nw_MinWidth        0x26    undefined
  WORD  nw_MinHeight       0x28    undefined
  UWORD nw_MaxWidth        0x2A    undefined
  UWORD nw_MaxHeight       0x2C    undefined
  UWORD nw_Type            0x2E    undefined
                           0x30
*/

/* structure DrawerData:
  NewWindow structure      0x00
  LONG  dd_CurrentX        0x30    *
  LONG  dd_CurrentY        0x34    *
                           0x38
*/

/* structure DrawerData2:
  ULONG dd_Flags           0x00    *
  UWORD dd_ViewModes       0x04    *
                           0x06
*/

/* structure Image:
  WORD  im_LeftEdge        0x00    0
  WORD  im_TopEdge         0x02    0
  WORD  im_Width           0x04    *
  WORD  im_Height          0x06    *
  WORD  im_Depth           0x08    *
  APTR  im_ImageData       0x0A    != 0
  UBYTE im_PlanePick       0x0E    *
  UBYTE im_PlaneOnOff      0x0F    *
  APTR  im_Next            0x10    0
                           0x14
*/

/* structur String:
  ULONG st_Size            0x00
  ...   st_String          0x04
*/

/* Disk-Representation of Icon:
  UWORD ic_Magic           0x00         0xE310
  UWORD ic_Version         0x02         1
  Gadget structure         0x04
  UBYTE ic_Type            0x30 0x00    *
  UBYTE ic_Pad             0x31 0x01    undefined
  APTR  ic_DefaultTool     0x32 0x02    (0 or != 0)
  APTR  ic_ToolTypes       0x36 0x06    (0 or != 0)
  LONG  ic_CurrentX        0x3A 0x0A    *
  LONG  ic_CurrentY        0x3E 0x0E    *
  APTR  ic_DrawerData      0x42 0x12    (0 or != 0)
  APTR  ic_ToolWindow      0x46 0x16    (0 or != 0)
  LONG  ic_StackSize       0x4A 0x1A    *
                           0x4E 0x1E

- drawerdata               (if ic_DrawerData != 0)
- image data of image 1
- struct image for image 2 (if ga_SelectRender != 0)
- image data of image 2
- defaulttool
- tooltypes
- drawerdata2              (if ic_DrawerData != 0 and ga_UserData == 1)
*/

/****************************************************************************/
/*     System independant stuff                                             */
/****************************************************************************/

static char mytolower(char a)
{
  if(a >= 'A' && a <= 'Z')
    a += 'a'-'A';
  return a;
}

static int mystricmp(char *a, char *b)
{
  while(*a && mytolower(*a) == mytolower(*b))
  {
    ++a; ++b;
  }
  return *a-*b;
}

static int mystrcmp(char *a, char *b)
{
  while(*a && *a == *b)
  {
    ++a; ++b;
  }
  return *a-*b;
}

static char *ReadIcon(struct IconRepresentation *ico)
{
  char *err;
  unsigned char *buffer;
  long size = ico->Buffersize, i, j, k;
  char tooltypes, deftool, secondimage, drawerdata, toolw;

  buffer = ico->Buffer;

  if(EndGetM16(buffer) != 0xE310)
    return "no icon file";
  if(EndGetM16(buffer+2) != 1)
    return "unknown icon version";

  buffer += 4; size -= 4;

#ifdef CHECK
  if(EndGetM32(buffer)) ++ico->Unknown;          /* ga_Next */
  if(!EndGetM16(buffer+0x8)) ++ico->Unknown;     /* ga_Width */
  if(!EndGetM16(buffer+0xA)) ++ico->Unknown;     /* ga_Height */
  i = EndGetM16(buffer+0xC);                     /* ga_Flags */
  if(i < 4 || i > 6) ++ico->Unknown;
  if(EndGetM32(buffer+0x1A)) ++ico->Unknown;     /* ga_Text */
  if((EndGetM32(buffer+0x28)&0xFF) > 1) ++ico->Unknown; /* ga_UserData */
#endif

#ifdef DEBUG
if(EndGetM32(buffer))          Printf("ga_Next:      %08lx\n", EndGetM32(buffer));
//                             Printf("ga_LeftEdge:  %ld\n",   EndGetM16(buffer+0x4));
//                             Printf("ga_TopEdge:   %ld\n",   EndGetM16(buffer+0x6));
if(!EndGetM16(buffer+ 0x8))    Printf("ga_Width:     %ld\n",   EndGetM16(buffer+0x8));
if(!EndGetM16(buffer+ 0xA))    Printf("ga_Height:    %ld\n",   EndGetM16(buffer+0xA));
i = EndGetM16(buffer+ 0xC);
if(i < 4 || i > 6)             Printf("ga_Flags:     %ld\n",   EndGetM16(buffer+0xC));
i = EndGetM16(buffer+0xE);
//                             Printf("ga_Acti:      %ld\n",   EndGetM16(buffer+0xE));
//                             Printf("ga_Type:      %ld\n",   EndGetM16(buffer+0x10));
//                             Printf("ga_GadRender: %08lx\n", EndGetM32(buffer+0x12));
//                             Printf("ga_SelRender: %08lx\n", EndGetM32(buffer+0x16));
if(EndGetM32(buffer+0x1A))     Printf("ga_Text:      %08lx\n", EndGetM32(buffer+0x1A));
//                             Printf("ga_MutExcl:   %08lx\n", EndGetM32(buffer+0x1E));
//                             Printf("ga_SpecInf:   %08lx\n", EndGetM32(buffer+0x22));
//                             Printf("ga_ID:        %ld\n",   EndGetM16(buffer+0x26));
if((EndGetM32(buffer+0x28)&0xFF) > 1) Printf("ga_UserData:  %02lx\n", EndGetM32(buffer+0x28)&0xFF);
#endif

  i = EndGetM16(buffer+0xC);
  if(i & 1)
    ico->IconMode = ICONMODE_BACKFILL;
  else if(i & 2)
    ico->IconMode = ICONMODE_IMAGE;
  else
    ico->IconMode = ICONMODE_COMPLEMENT;

  secondimage = EndGetM32(buffer+0x16) ? 1 : 0;
  ico->OSType = (EndGetM32(buffer+0x28)&0xFF) ? OSTYPE_20 : OSTYPE_13;
  ico->Width  = EndGetM16(buffer+0x8);
  ico->Height = EndGetM16(buffer+0xA);
  buffer += 0x2C; size -= 0x2C;

#ifdef CHECK
  if((!buffer[0x00] || buffer[0x00] > ICONTYPE_GARBAGE)
  && buffer[0x00] != ICONTYPE_KICK) ++ico->Unknown;
#endif

#ifdef DEBUG
if((!buffer[0x00] || buffer[0x00] > ICONTYPE_GARBAGE) && buffer[0x00] != ICONTYPE_KICK)
  Printf("ic_Type:      %ld\n", buffer[0x00]);
//Printf("ic_Pad:       %ld\n", buffer[0x01]);
//Printf("ic_DefTool:   %08lx\n", EndGetM32(buffer+0x02));
//Printf("ic_ToolTypes: %08lx\n", EndGetM32(buffer+0x06));
//Printf("ic_CurrentX:  %ld\n",   EndGetM32(buffer+0x0A));
//Printf("ic_CurrentY:  %ld\n",   EndGetM32(buffer+0x0E));
//Printf("ic_DrawerDat: %08lx\n", EndGetM32(buffer+0x12));
//Printf("ic_ToolWin:   %08lx\n", EndGetM32(buffer+0x16));
//Printf("ic_StackSize: %ld\n",   EndGetM32(buffer+0x1A));
#endif
  tooltypes = EndGetM32(buffer+0x06) ? 1 : 0;
  toolw = EndGetM32(buffer+0x016) ? 1 : 0;
  deftool = EndGetM32(buffer+0x02) ? 1 : 0;
  drawerdata = EndGetM32(buffer+0x12) ? 1 : 0;
  ico->XPosition = EndGetM32(buffer+0x0A);
  ico->YPosition = EndGetM32(buffer+0x0E);
  ico->StackSize = EndGetM32(buffer+0x1A);
  ico->Type = buffer[0];
  buffer += 0x1E; size -= 0x1E;
  if(size < 0)
    return "icon truncated";

  if(drawerdata)
  {
    if((err = ReadDrawerData(buffer, &ico->Drawer)))
      return err;
    buffer += 0x38; size -= 0x38;
  }
  if(size < 0)
    return "icon truncated";

  if((err = ReadOldImage(buffer, ico->OldImage+0)))
    return err;
  i = 0x14 + (((((ico->OldImage[0].Width+15)&(~15))
  * ico->OldImage[0].Height) * ico->OldImage[0].Depth)>>3);
  buffer += i; size -= i;
  if(size < 0)
    return "icon truncated";

  if(secondimage)
  {
    if((err = ReadOldImage(buffer, ico->OldImage+1)))
      return err;
    i = 0x14 + (((((ico->OldImage[1].Width+15)&(~15))
    * ico->OldImage[1].Height) * ico->OldImage[1].Depth)>>3);
    buffer += i; size -= i;
  }

  if(size < 0)
    return "icon truncated";

  if(deftool)
  {
    i = EndGetM32(buffer);
    if(i+4 > size)
      return "string too large";
    else
      ico->DefaultTool = buffer+4;
#ifdef DEBUG
//if(i > 1) Printf("ic_DefTool:   %ld '%s'\n", i, ico->DefaultTool);
#endif
    buffer += i+4; size -= i+4;
  }
  if(size < 0)
    return "icon truncated";

  if(tooltypes)
  {
    i = EndGetM32(buffer)/4-1;
    if(i > MAXTOOLTYPES)
      return "too many tooltypes";
#ifdef DEBUG
//if(i) Printf("ic_NumToolT:  %ld\n", i);
#endif
    buffer += 4; size -= 4;
    for(j = 0; j < i; ++j)
    {
      k = EndGetM32(buffer);
      if(k+4 > size)
        return "string too large";
      else
        ico->ToolTypes[j] = buffer+4;
#ifdef DEBUG
//Printf("ic_ToolType:  %ld '%s'\n", k, ico->ToolTypes[j]);
#endif
      buffer += k+4; size -= k+4;
    }
  }
  if(size < 0)
    return "icon truncated";

  if(toolw)
  {
    i = EndGetM32(buffer);
    if(i+4 > size)
      return "string too large";
#ifdef DEBUG
if(i > 1) Printf("ic_ToolWin:   %ld '%s'\n", i, buffer+4);
#endif
    buffer += i+4; size -= i+4;
  }

  if(size < 0)
    return "icon truncated";

  /* handle defective type valid */
  if(drawerdata && size == 6 && ico->OSType == OSTYPE_13)
    ico->OSType = OSTYPE_20;

  if(drawerdata && ico->OSType == OSTYPE_20)
  {
#ifdef CHECK
    if(EndGetM32(buffer+0x00) > 3) ++ico->Unknown;
    if(EndGetM16(buffer+0x04) > 5) ++ico->Unknown;
#endif
#ifdef DEBUG
if(EndGetM32(buffer+0x00) > 3) Printf("dd_Flags:     %ld\n",   EndGetM32(buffer+0x00));
if(EndGetM16(buffer+0x04) > 5) Printf("dd_ViewMode:  %ld\n",   EndGetM16(buffer+0x04));
#endif
    ico->Drawer.Flags = EndGetM32(buffer);
    ico->Drawer.ViewMode = EndGetM16(buffer+4);
    buffer += 6; size -= 6;
  }
  if(size < 0)
    return "icon truncated";

  if(size >= 8 && EndGetM32(buffer) == 0x464F524D)
  {
    if((err = ReadNewStyleIcon(buffer, ico)))
      return err;
    ico->OSType = OSTYPE_35;
    i = EndGetM32(buffer+4)+8;
#ifdef DEBUG
    buffer += i;
#endif
    size -= i;
  }
  if((err = ProcessNewIcon(ico)))
    return err;

  if(size < 0)
    return "icon truncated";

#ifdef CHECK
  if(size)
    ++ico->Unknown;
#endif

#ifdef DEBUG
  if(size)
  {
    Printf("Left %d:", size);
    while(size--)
      Printf("%02lx", *(buffer++));
    Printf("\n");
  }
#endif

  return 0;
}

static char *ReadOldImage(unsigned char *buffer, struct IconImage *im)
{
  int i, j;
#ifdef CHECK
  if(EndGetM16(buffer+0x00)) ++im->Unknown;
  if(EndGetM16(buffer+0x02)) ++im->Unknown;
  if(!EndGetM16(buffer+0x04)) ++im->Unknown;
  if(!EndGetM16(buffer+0x06)) ++im->Unknown;
  i = EndGetM16(buffer+0x08);
  if(!i || i > 8) ++ im->Unknown;
  if(!EndGetM32(buffer+0x0A)) ++im->Unknown;
  if(EndGetM32(buffer+0x10)) ++im->Unknown;
#endif

#ifdef DEBUG
if(EndGetM16(buffer+0x00))  Printf("im_LeftEdge:  %ld\n",   EndGetM16(buffer+0x00));
if(EndGetM16(buffer+0x02))  Printf("im_TopEdge:   %ld\n",   EndGetM16(buffer+0x02));
if(!EndGetM16(buffer+0x04)) Printf("im_Width:     %ld\n",   EndGetM16(buffer+0x04));
if(!EndGetM16(buffer+0x06)) Printf("im_Height:    %ld\n",   EndGetM16(buffer+0x06));
i = EndGetM16(buffer+0x08);
if(!i || i > 8)             Printf("im_Depth:     %ld\n",   EndGetM16(buffer+0x08));
if(!EndGetM32(buffer+0x0A)) Printf("im_ImageData: %08lx\n", EndGetM32(buffer+0x0A));
//Printf("im_PlanePick: %ld\n",   buffer[0x0E]);
//Printf("im_PlaneOff:  %ld\n",   buffer[0x0F]);
if(EndGetM32(buffer+0x10))  Printf("im_Next:      %08lx\n", EndGetM32(buffer+0x10));
#endif
  im->Width = EndGetM16(buffer+0x04);
  im->Height = EndGetM16(buffer+0x06);
  im->Depth = EndGetM16(buffer+0x08);
  im->NumColors = (1<<im->Depth);
  if(im->Width > MAXICONSIZE || im->Height > MAXICONSIZE)
    return "icon to large";
  if(im->Depth > MAXDEPTH)
    return "image depth too high";
  buffer += 0x14;

  i = (((((im->Width+15)&(~15)) * im->Height) * im->Depth)>>3);
  for(j = 0; j < i; ++j)
    im->ImageData[j] = buffer[j];

  return 0;
}

static char *ReadDrawerData(unsigned char *buffer, struct DrawerInfo *drw)
{
#ifdef CHECK
  if(buffer[0x08] != 255 || buffer[0x09] != 255) ++drw->Unknown;
#endif

#ifdef DEBUG
//                      Printf("nw_LeftEdge:  %ld\n",   EndGetM16(buffer+0x00));
//                      Printf("nw_TopEdge:   %ld\n",   EndGetM16(buffer+0x02));
//                      Printf("nw_Width:     %ld\n",   EndGetM16(buffer+0x04));
//                      Printf("nw_Height:    %ld\n",   EndGetM16(buffer+0x06));
if(buffer[0x08] != 255) Printf("nw_DetailPen: %ld\n",   buffer[0x08]);
if(buffer[0x09] != 255) Printf("nw_BlockPen:  %ld\n",   buffer[0x09]);
//                      Printf("nw_IDCMPFlag: %08lx\n", EndGetM32(buffer+0x0A));
//                      Printf("nw_Flags:     %08lx\n", EndGetM32(buffer+0x0E));
//                      Printf("nw_FirstGadg: %08lx\n", EndGetM32(buffer+0x12));
//                      Printf("nw_CheckMark: %08lx\n", EndGetM32(buffer+0x16));
//                      Printf("nw_Title:     %08lx\n", EndGetM32(buffer+0x1A));
//                      Printf("nw_Screen:    %08lx\n", EndGetM32(buffer+0x1E));
//                      Printf("nw_BitMap:    %08lx\n", EndGetM32(buffer+0x22));
//                      Printf("nw_MinWidth:  %ld\n",   EndGetM16(buffer+0x26));
//                      Printf("nw_MinHeight: %ld\n",   EndGetM16(buffer+0x28));
//                      Printf("nw_MaxWidth:  %ld\n",   EndGetM16(buffer+0x2A));
//                      Printf("nw_MaxHeight: %ld\n",   EndGetM16(buffer+0x2C));
//                      Printf("nw_Type:      %ld\n",   EndGetM16(buffer+0x2E));
//                      Printf("dd_CurrentX:  %ld\n",   EndGetM32(buffer+0x30));
//                      Printf("dd_CurrentY:  %ld\n",   EndGetM32(buffer+0x34));
#endif

  drw->XPosition = EndGetM32(buffer+0x30);
  drw->YPosition = EndGetM32(buffer+0x34);
  drw->Width = EndGetM16(buffer+0x04);
  drw->Height = EndGetM16(buffer+0x06);
  drw->LeftEdge = EndGetM16(buffer+0x00);
  drw->TopEdge = EndGetM16(buffer+0x02);

  return 0;
}

static void RemoveToolType(unsigned char **tt)
{
  do
  {
    tt[0] = tt[1];
    ++tt;
  } while(*tt);
}

static char *DecodeNewIcon(unsigned char **tt, struct IconImage *im, enum DecodeType type, unsigned char num)
{
  long numbits = 0, bits, curentry = 0, entries, bitbuf = 0, loop = 0, mask, val;
  unsigned char byte;
  char *src;

  if(type == DECODE_PALETTE)
  {
    src = *tt+9;
    RemoveToolType(tt);
    entries = im->NumColors*3;
    bits = 8;
  }
  else
  {
    src = ""; /* a dummy start */
    entries = im->Width*im->Height;
    bits = 0;
    for(mask = im->NumColors-1; mask; mask >>= 1)
      ++bits;
    loop = 0;
  }
  mask = (1<<bits)-1;

  while(curentry < entries)
  {
    if(loop) { byte = 0; --loop; }
    else
    {
      if(!*src)
      {
        src = *tt;
        if(!src || src[0] != 'I' || src[1] != 'M' || src[2] != num || src[3] != '=')
          return "NewIcon data truncated";
        else
        {
          src += 4; numbits = 0;
        }
        RemoveToolType(tt);
      }
      byte = *(src++);
      if(!byte)            { return "NewIcon data invalid"; }
      else if(byte < 0xA0) { byte -= 0x20; }
      else if(byte < 0xD1) { byte -= 0x51; }
      else                 { loop = byte - 0xD1; byte = 0; }
    }
    bitbuf = (bitbuf<<7)+byte;
    numbits += 7;
    while(numbits >= bits && curentry < entries)
    {
      val = (bitbuf>>(numbits-bits))&mask;
#ifdef CHECK
      if(val > 0xFF) ++im->Unknown;
#endif
#ifdef DEBUG
if(val > 0xFF) Printf("Large palette entry cut down!\n");
#endif 
      if(type == DECODE_PALETTE)
      { /* NOTE: This may not be very fast, but for sure portable */
        switch(curentry%3)
        {
        case 0: im->Palette[curentry/3].Red = val; break;
        case 1: im->Palette[curentry/3].Green = val; break;
        case 2: im->Palette[curentry/3].Blue = val; break;
        }
      }
      else
        im->ImageData[curentry] = val;
      numbits -= bits;
      curentry++;
    }
  }
  return 0;
}

static char *ProcessNewIconImage(unsigned char **tt, struct IconImage *im, unsigned char num)
{
  char *txt;
  unsigned char *b;

  while(*tt)
  {
    b = *tt;
    if(b[0] == 'I' && b[1] == 'M' && b[2] == num && b[3] == '=')
    {
      b += 4;
      im->Transparency = (b[0] == 'B') ? 0 : -1;
      im->Width      = b[1]-0x21;
      im->Height     = b[2]-0x21;
      im->NumColors  = (((b[3]-0x21)<<6)+(b[4]-0x21));
#ifdef CHECK
      if(b[0] != 'B' && b[0] != 'C') ++im->Unknown;
      if(!im->Width || im->Width > 93) ++im->Unknown;
      if(!im->Height || im->Height > 93) ++im->Unknown;
#endif
#ifdef DEBUG
if(b[0] != 'B' && b[0] != 'C')
  Printf("ni_Transpa:   %ld\n",   b[0]);
if(!im->Width || im->Width > 93)
  Printf("ni_Width:     %ld\n",   im->Width);
if(!im->Height || im->Height > 93)
  Printf("ni_Height:    %ld\n",   im->Height);
if(im->NumColors > MAXCOLORSNI)
  Printf("ni_NumColors: %ld\n",   im->NumColors);
#endif
      if(im->Width > MAXICONSIZE || im->Height > MAXICONSIZE)
        return "icon to large";
      if(im->NumColors > MAXCOLORSNI)
        return "too much colors";
      if((txt = DecodeNewIcon(tt, im, DECODE_PALETTE, num)))
        return txt;
      if((txt = DecodeNewIcon(tt, im, DECODE_IMAGE, num)))
        return txt;
      if(im->NumColors > MAXCOLORS)
        im->NumColors = MAXCOLORS;
    }
    else
      ++tt;
  }
  return 0;
}

static char *ProcessNewIcon(struct IconRepresentation *ico)
{
  char *txt, **a, **d;

  if((txt = ProcessNewIconImage(ico->ToolTypes, ico->NewIconImage+0, '1')))
    return txt;
  if((txt = ProcessNewIconImage(ico->ToolTypes, ico->NewIconImage+1, '2')))
    return txt;

  a = ico->ToolTypes; d = 0;
  while(*a)
  {
    if(!mystrcmp(*a, "*** DON'T EDIT THE FOLLOWING LINES!! ***"))
    {
      RemoveToolType(a);
      if(d[0][0] == ' ' && !d[0][1]) /* remove space line */
        RemoveToolType(d);
      break;
    }
    d = a;
    ++a;
  }

#ifdef DEBUG
//a = ico->ToolTypes;
//while(*a) Printf("ni_ToolType:  '%s'\n", *(a++));
#endif

  return txt;
}

static void Chunky2Planar(struct IconImage *from, struct IconImage *to)
{
  unsigned long i, j, k, l, m, fieldsize, s, highcol = 0;
  unsigned char tmp[8];
  char *fpos, *tpos;

  to->Depth = 0;
  to->Width = from->Width;
  to->Height = from->Height;
  to->Transparency = from->Transparency;
  
  for(i = 0; i < from->NumColors; ++i)
    to->Palette[i] = from->Palette[i];

  fieldsize = (((from->Width+15)&(~15))*from->Height)>>3;
  s = from->Width&15; if(s > 8) s = 0;
  fpos = from->ImageData;
  tpos = to->ImageData;
  for(i = 0; i < from->Height; ++i)
  {
    for(j = 0; j < from->Width; j += 8)
    {
      for(k = 0; k < from->Depth; ++k)
      {
        tmp[k] = fpos[k*fieldsize];
      }
      fpos++;
      for(k = 0; k < 8 && j+k < from->Width; ++k)
      {
        l = 0;
        for(m = 0; m < from->Depth; ++m)
        {
          l = (l>>1)|(tmp[m]&0x80);
          tmp[m]<<=1;
        }
        l >>= (8-from->Depth);
        if(l > highcol) highcol = l;
        *(tpos++) = l;
      }
    }
    if(s)
      ++fpos;
  }

  if(++highcol > from->NumColors) /* short palette */
    highcol = from->NumColors;
  to->NumColors = highcol; /* reduce number of colors to really required value */
}

static char *DecodeNew(unsigned char *buffer, long numbytes, long bits, struct IconImage *im, enum DecodeType type,
long mode)
{
  int cop = 0, loop = 0, entries, numbits = 0, mask, curentry = 0;
  unsigned long bitbuf = 0;
  unsigned char byte;

  if(!mode) /* no RLE */
    cop = numbytes*8/bits;
  if(type == DECODE_PALETTE)
    entries = im->NumColors*3;
  else
    entries = im->Width*im->Height;

  mask = (1<<bits)-1;

  while((numbytes || (cop && numbits >= bits)) && (curentry < entries))
  {
    if(!cop)
    {
      if(numbits < 8)
      {
        bitbuf = (bitbuf<<8)|*(buffer++); --numbytes;
        numbits += 8;
      }
      byte = (bitbuf>>(numbits-8))&0xFF;
      numbits -= 8;
      if(byte <= 127)     cop = byte+1;
      else if(byte > 128) loop = 256-byte+1;
      else                continue;
    }
    if(cop) ++loop;

    if(numbits < bits)
    {
      bitbuf = (bitbuf<<8)|*(buffer++); --numbytes;
      numbits += 8;
    }
    byte = (bitbuf>>(numbits-bits))&mask;
    while(loop && (curentry < entries))
    {
      if(type == DECODE_PALETTE)
      { /* NOTE: This may not be very fast, but for sure portable */
        switch(curentry%3)
        {
        case 0: im->Palette[curentry/3].Red = byte; break;
        case 1: im->Palette[curentry/3].Green = byte; break;
        case 2: im->Palette[curentry/3].Blue = byte; break;
        }
      }
      else
        im->ImageData[curentry] = byte;
      ++curentry;
      --loop;
    }
    if(cop) --cop;
    numbits -= bits;
  }
  return curentry != entries ? "error decoding new icon data" : 0;
}

static char *ReadNewStyleIcon(unsigned char *buffer, struct IconRepresentation *ico)
{
  char *text;
  long size, csize, numimage = 0, numface = 0;
  long width = 0, height = 0;
  unsigned long id;
  if(EndGetM32(buffer) != 0x464F524D || EndGetM32(buffer+8) != 0x49434F4E)
    return "invalid new icon ID.";
  size = EndGetM32(buffer+4)-4;
  buffer += 12;

  while(size > 0)
  {
    id = EndGetM32(buffer);
    csize = EndGetM32(buffer+4);
    buffer += 8;
    switch(id)
    {
    case 0x46414345: /* FACE */
#ifdef DEBUG
if(numface) Printf("Only one FACE chunk supported.\n");
if(csize != 6) Printf("FACE chunk should have 6 bytes instead of %ld\n", csize);
#endif
      if(!numface != csize < 6)
      {
        ++numface;
        width = buffer[0]+1;
        height = buffer[1]+1;
        ico->OS35Flags = buffer[2];
#ifdef CHECK
        if(ico->OS35Flags > 1) ++ico->Unknown;
#endif
#ifdef DEBUG
if(ico->OS35Flags > 1) Printf("fc_Flags:     %02lx\n", ico->OS35Flags);
//                     Printf("fc_MaxPlatte: %02ld\n", EndGetM16(buffer+4)+1);
#endif
        ico->AspectRatio = buffer[3];
        /* maxpalette = EndGetM16(buffer+4)+1; unused */
      }
#ifdef CHECK
      else ++ico->Unknown;
#endif
      break;
    case 0x494D4147: /* IMAG */
#ifdef DEBUG
if(!numface)     Printf("FACE chunk must be before images.\n");
if(numimage > 1) Printf("Only 2 images supported.\n");
if(csize < 10)    Printf("Minimum IMAG chunks size must be 10 and not %ld.\n", csize);
#endif
      if(numface || numimage <= 1 || csize >= 10)
      {
        struct IconImage *im;
        long icflags, imf, palf, numi, nump, dep;

        im = ico->NewImage+numimage;
        im->Width = width;
        im->Height = height;
        im->Transparency = buffer[0];
        im->NumColors = buffer[1]+1;
        icflags = buffer[2];
        imf = buffer[3];
        palf = buffer[4];
        dep = buffer[5];
        numi = EndGetM16(buffer+6)+1;
        nump = EndGetM16(buffer+8)+1;
        if(!(icflags & 1))        /* no transparency */
          im->Transparency = -1;
        if(imf <= 1 && palf <= 1)
        {
          if((text = DecodeNew(buffer+10, numi, imf ? dep : 8, im, DECODE_IMAGE, imf)))
            return text;
          if(icflags & 2)
          {
            if((text = DecodeNew(buffer+10+numi, nump, 8, im, DECODE_PALETTE, palf)))
              return text;
          }
          else if(numimage)
          {
            im->NumColors = ico->NewImage[0].NumColors;
            SetPalette(im, ico->NewImage[0].Palette, im->NumColors);
          }
#ifdef CHECK
          else ++im->Unknown;
#endif
        }
#ifdef CHECK
        else
          ++ico->Unknown;
        if(icflags > 3)
          ++ico->Unknown;
#endif
#ifdef DEBUG
//Printf("ic_Width:     %ld\n", im->Width);
//Printf("ic_Height:    %ld\n", im->Height);
//Printf("ic_Transpar:  %ld\n", im->Transparency);
if(icflags > 3)
Printf("ic_Flags:     %02lx\n", icflags);
//Printf("ic_Pal:       %ld, %ld\n", nump, palf);
//Printf("ic_Image:     %ld, %ld\n", numi, imf);
#endif
        ++numimage;
      }
#ifdef CHECK
      else ++ico->Unknown;
#endif
      break;
    default:
#ifdef CHECK
      ++ico->Unknown;
#endif
#ifdef DEBUG
Printf("Unknown Chunk %lc%lc%lc%lc size %ld found.\n", (id>>24)&0xFF, (id>>16)&0xFF, (id>>8)&0xFF, id&0xFF, csize);
#endif
      break;
    }
    if(csize & 1) ++csize;
    buffer += csize;
    size -= 8+csize;
  }
  if(size < 0)
    return "new icon truncated";

  return 0;
}

/* As the old-icon palette is not defined, there is no real way
to specify the correct colors for each icon. These are only the
palettes, which I know of. The DefaultPalette20 is the palette
used on my >OS2.x system on a true color screen and hopefully
represents OS2.0 icons best. If there are icon-set based on different
palettes, please send me the corresponding set. For individual icons
convert the icons to OS3.5 format and use an editor to edit the
palette to fit the designed colors. */
static const struct IconPalette MagicPalette[8] = {
  0x95,0x95,0x95, 0x00,0x00,0x00, 0xFF,0xFF,0xFF, 0x3B,0x67,0xA2,
  0x7B,0x7B,0x7B, 0xAF,0xAF,0xAF, 0xAA,0x90,0x7C, 0xFF,0xA9,0x97
};

static const struct IconPalette DefaultPalette13[4] = {
  0x00,0x54,0xA8, 0xF8,0xFC,0xF8, 0x00,0x00,0x20, 0xF8,0x88,0x00
};

static const struct IconPalette ROMPalette[16] = {
  0x95,0x95,0x95, 0x00,0x00,0x00, 0xFF,0xFF,0xFF, 0x3B,0x67,0xA2,
  0x7B,0x7B,0x7B, 0xAF,0xAF,0xAF, 0xAA,0x90,0x7C, 0xFF,0xA9,0x97,
  0x00,0x00,0xFF, 0x32,0x32,0x32, 0x60,0x80,0x60, 0xE2,0xD1,0x77,
  0xFF,0xD4,0xCB, 0x7A,0x60,0x48, 0xD2,0xD2,0xD2, 0xE5,0x5D,0x5D
};

static const struct IconPalette DefaultPalette20[256] = {
  0xAA,0xAA,0xAA, 0x00,0x00,0x00, 0xFF,0xFF,0xFF, 0x66,0x88,0xBB,
  0x00,0x00,0xFF, 0xFF,0x00,0xFF, 0x00,0xFF,0xFF, 0xFF,0xFF,0xFF,
  0x66,0x22,0x00, 0xEE,0x55,0x00, 0x99,0xFF,0x11, 0xEE,0xBB,0x00,
  0x55,0x55,0xFF, 0x99,0x22,0xFF, 0x00,0xFF,0x88, 0xCC,0xCC,0xCC,
  0x00,0x00,0x00, 0xE0,0x40,0x40, 0x00,0x00,0x00, 0xE0,0xE0,0xC0,
  0x44,0x44,0x44, 0x55,0x55,0x55, 0x66,0x66,0x66, 0x77,0x77,0x77,
  0x88,0x88,0x88, 0x99,0x99,0x99, 0xAA,0xAA,0xAA, 0xBB,0xBB,0xBB,
  0xCC,0xCC,0xCC, 0xDD,0xDD,0xDD, 0xEE,0xEE,0xEE, 0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF, 0x88,0x88,0x88, 0xFF,0xFF,0xFF, 0xCC,0xCC,0xCC,
  0x44,0x44,0x44, 0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0x88,0xFF,0xFF,
  0x44,0x88,0x88, 0xCC,0xFF,0xFF, 0x66,0xCC,0xCC, 0x22,0x44,0x44,
  0xAA,0xFF,0xFF, 0xEE,0xFF,0xFF, 0xCC,0xFF,0xFF, 0x66,0x88,0x88,
  0xFF,0xFF,0xFF, 0x99,0xCC,0xCC, 0x33,0x44,0x44, 0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF, 0x44,0xFF,0xFF, 0x22,0x88,0x88, 0x66,0xFF,0xFF,
  0x33,0xCC,0xCC, 0x11,0x44,0x44, 0x55,0xFF,0xFF, 0x77,0xFF,0xFF,
  0xFF,0x88,0xFF, 0x88,0x44,0x88, 0xFF,0xCC,0xFF, 0xCC,0x66,0xCC,
  0x44,0x22,0x44, 0xFF,0xAA,0xFF, 0xFF,0xEE,0xFF, 0x88,0x88,0xFF,
  0x44,0x44,0x88, 0xCC,0xCC,0xFF, 0x66,0x66,0xCC, 0x22,0x22,0x44,
  0xAA,0xAA,0xFF, 0xEE,0xEE,0xFF, 0xCC,0x88,0xFF, 0x66,0x44,0x88,
  0xFF,0xCC,0xFF, 0x99,0x66,0xCC, 0x33,0x22,0x44, 0xFF,0xAA,0xFF,
  0xFF,0xEE,0xFF, 0x44,0x88,0xFF, 0x22,0x44,0x88, 0x66,0xCC,0xFF,
  0x33,0x66,0xCC, 0x11,0x22,0x44, 0x55,0xAA,0xFF, 0x77,0xEE,0xFF,
  0xFF,0xCC,0xFF, 0x88,0x66,0x88, 0xFF,0xFF,0xFF, 0xCC,0x99,0xCC,
  0x00,0xFC,0xF8, 0x00,0x20,0xF8, 0x00,0x64,0xF8, 0x30,0x98,0xF8,
  0x98,0x98,0x00, 0xF8,0x00,0x00, 0x60,0x64,0x00, 0xF8,0x00,0xF8,
  0xC8,0x00,0xC8, 0x10,0x44,0x40, 0x78,0x78,0x78, 0xF8,0xB4,0x18,
  0xF8,0xBC,0x28, 0xF8,0xA8,0x90, 0xF8,0xB4,0x10, 0xF8,0x9C,0x20,
  0xF8,0x78,0x28, 0xD8,0x70,0x30, 0xE0,0x70,0x08, 0xF0,0x8C,0x10,
  0xF8,0x9C,0x08, 0xF8,0x88,0x00, 0xF8,0xC0,0x20, 0xC0,0x4C,0x10,
  0xF8,0xBC,0x10, 0xF8,0xBC,0x40, 0x60,0x20,0x00, 0x90,0x38,0x38,
  0x90,0x30,0x08, 0xF8,0x9C,0x40, 0x70,0x30,0x30, 0x70,0x24,0x00,
  0xE8,0x94,0x58, 0x98,0x20,0xF8, 0x90,0x3C,0x78, 0x90,0x4C,0xB0,
  0x50,0x1C,0x10, 0xF0,0xAC,0x60, 0x80,0x70,0xD0, 0x78,0x8C,0xE0,
  0xE8,0x54,0x00, 0xE8,0xB8,0x00, 0xA8,0x40,0x08, 0xF8,0xCC,0x40,
  0xD8,0x94,0x88, 0x90,0x90,0xD8, 0xB0,0x48,0xA8, 0x00,0x00,0xF8,
  0xE0,0x38,0x48, 0xE0,0x38,0x58, 0xB0,0x9C,0xC0, 0xB0,0xB4,0xD0,
  0xC8,0x98,0xA0, 0xD8,0xA4,0xB0, 0xE8,0x40,0x70, 0xE0,0xBC,0xC0,
  0xE0,0x50,0x90, 0xE8,0x4C,0x78, 0xF0,0xB4,0x88, 0xE8,0xA8,0xB8,
  0xE0,0xCC,0xD0, 0xF0,0xE4,0xE8, 0xF0,0x8C,0x90, 0xF8,0x34,0x50,
  0xF0,0x3C,0x60, 0xF8,0xC4,0x38, 0xF0,0x70,0x90, 0xE0,0x74,0x60,
  0xA8,0x90,0x78, 0xF8,0x28,0x48, 0xD0,0x84,0x00, 0xD0,0x94,0x00,
  0xD0,0x9C,0x10, 0xC8,0x70,0x18, 0xD0,0x50,0x00, 0xB8,0x48,0x00,
  0xC8,0x64,0x00, 0x98,0x24,0x00, 0xF0,0x98,0x40, 0xF8,0xAC,0x10,
  0x80,0x18,0x00, 0xA0,0x18,0x10, 0x68,0x10,0x10, 0x98,0x50,0x40,
  0x68,0x08,0x00, 0x70,0x28,0x18, 0x48,0x08,0x08, 0x38,0x64,0xA0,
  0xA8,0x64,0xB0, 0xC0,0x6C,0x30, 0x68,0x38,0x50, 0x68,0x14,0x50,
  0x68,0x24,0x88, 0x28,0x00,0x00, 0xC8,0x84,0x38, 0x00,0xFC,0xF8,
  0xD0,0x5C,0x10, 0xC8,0x8C,0x60, 0x50,0x54,0xF8, 0x98,0xFC,0x10,
  0x00,0xFC,0x88, 0x88,0x8C,0xA8, 0xB8,0x10,0x20, 0x68,0x68,0xB0,
  0x88,0x20,0x80, 0x90,0x18,0x48, 0x80,0x3C,0x88, 0x50,0x64,0xB8,
  0x88,0x74,0x98, 0xB0,0x6C,0x60, 0x58,0x48,0xA8, 0x70,0xD8,0x68,
  0x38,0x88,0x40, 0x20,0x40,0x60, 0xA8,0xAC,0xF0, 0x50,0x64,0x90,
  0x78,0x88,0xC0, 0xE8,0xD8,0xC8, 0x00,0x34,0x00, 0x08,0x60,0x08,
  0x58,0x88,0x10, 0x90,0x90,0x90, 0x10,0x14,0x10, 0x20,0x24,0x20,
  0x00,0x3C,0x70, 0x08,0x4C,0x80, 0x38,0x60,0x90, 0x80,0x80,0x80,
  0xA0,0xA0,0xA0, 0xD0,0xD4,0xD0, 0xB0,0xB4,0xB0, 0x20,0x5C,0x20,
  0x30,0x88,0x30, 0x80,0xB0,0x38, 0xB8,0xB8,0xB8, 0x38,0x3C,0x38,
  0x00,0x64,0x98, 0x30,0x74,0xA8, 0xC8,0xC8,0xC8, 0xD8,0xDC,0xD8,
  0xD8,0x64,0x38, 0x30,0x30,0x30, 0xD8,0x8C,0x40, 0x60,0x64,0x60,
  0x80,0x00,0x00, 0xD8,0x30,0x30, 0x50,0xB0,0x58, 0x18,0x5C,0x30,
  0xD8,0x74,0x40, 0xC8,0xAC,0x98, 0x98,0x84,0x70, 0x70,0x5C,0x40,
  0xE8,0xE4,0x10, 0xD8,0xB8,0x40, 0x28,0x28,0x28, 0x48,0x48,0x48,
  0x88,0x88,0x88, 0xE0,0xE0,0xE0, 0xC0,0xC4,0xC0, 0x68,0x68,0x68,
  0xEE,0x44,0x44, 0x55,0xDD,0x55, 0x00,0x44,0xDD, 0xEE,0x99,0x00,
};

static void SetPalette(struct IconImage *im, const struct IconPalette *pal, long num)
{
  long i, j;

  for(j = 0; j < im->NumColors; ++j)
  {
    i = j%num;
    im->Palette[j].Red = pal[i].Red;
    im->Palette[j].Green = pal[i].Green;
    im->Palette[j].Blue = pal[i].Blue;
  }
}

static void SetOldPalette(struct IconRepresentation *ico, enum PaletteMode mode)
{
  int num = 0, i;
  const struct IconPalette *pal = 0;
  struct IconImage *im;
  char **tt;

  for(i = 0; i < 2; ++i)
  {
    im = ico->OldImage+i;
    switch(mode)
    {
    case PALETTE_MAGICWB:
      num = 8; pal = MagicPalette; break;
    case PALETTE_ROM:
      num = 16; pal = ROMPalette; break;
    case PALETTE_DEFAULT13:
      num = 4; pal = DefaultPalette13; break;
    case PALETTE_DEFAULT20:
      num = 256; pal = DefaultPalette20; break;
    case PALETTE_AUTO:
      /* the default palette */
      if(ico->OSType == OSTYPE_13 && im->Depth <= 2)
      {
        num = 4; pal = DefaultPalette13;
      }
      else
      {
        num = 256; pal = DefaultPalette20;
      }
      /* now search MagicWB or ROMIcons */
      for(tt = ico->ToolTypes; *tt; ++tt)
      {
        if(!mystrcmp(*tt, "*** Colors = ROMIcon ***") ||
        !mystrcmp(*tt, "*** 16 color ROM Icon ***"))
        {
          num = 16; pal = ROMPalette; break;
        }
        else if(!mystrcmp(*tt, "»»»» Icon by Martin Huttenloher ««««"))
        {
          num = 8; pal = MagicPalette; break;
        }
      }
      break;
    }
    SetPalette(im, pal, num);
  }
}

/* save stuff ***************************************************************/

static void CreateNewIcon(unsigned char *buf, unsigned char num, long *totsize,
long *numtt, struct IconImage *im)
{
  unsigned char *bufs, *sbuf;
  unsigned long size, i, t, bitbuf, numzero, numbits, bits;

  sbuf = buf;

  bufs = buf; buf += 4; size = 0;
  buf[size++] = 'I'; buf[size++] = 'M'; buf[size++] = num; buf[size++] = '=';
  buf[size++] = (im->Transparency == -1) ? 'C' : 'B';
  buf[size++] = im->Width + 0x21;
  buf[size++] = im->Height + 0x21;
  buf[size++] = (im->NumColors>>6) + 0x21;
  buf[size++] = (im->NumColors&0x3F) + 0x21;
  i = bitbuf = numzero = numbits = 0;
  while(i < im->NumColors*3 || numbits)
  {
    if(size == 127)
    {
      while(numbits % 8) ++numbits; /* very strange end wrapping */
      buf[size++] = 0; EndPutM32(bufs, size); ++(*numtt); buf += size; size = 0;
      bufs = buf; buf += 4;
      buf[size++] = 'I'; buf[size++] = 'M'; buf[size++] = num; buf[size++] = '=';
    }

    if(numbits < 7)
    {
      if(i == im->NumColors*3)
      {
        bitbuf <<= (7-numbits); numbits = 7;
      }
      else
      {
        bitbuf <<= 8; numbits += 8;
        switch(i%3)
        {
        case 0: bitbuf |= im->Palette[i/3].Red; break;
        case 1: bitbuf |= im->Palette[i/3].Green; break;
        case 2: bitbuf |= im->Palette[i/3].Blue; break;
        }
        ++i;
      }
    }
    t = (bitbuf>>(numbits-7))&0x7F;
    if(numzero == 47) { numzero = 0; buf[size++] = 0xFF; }
    else if(!t)       { ++numzero; numbits -= 7; }
    else
    {
      if(numzero) {buf[size++] = 0xD0+numzero; numzero = 0;}
      else
      {
        numbits -= 7;
        if(t <= 0x4F) buf[size++] = 0x20+t;
        else buf[size++] = (0xA1-0x50)+t;
      }
    }
  }
  /* flush last zero bytes */
  if(numzero) buf[size++] = 0xD0+numzero;
  buf[size++] = 0; EndPutM32(bufs, size); ++(*numtt); buf += size;

  bits = 0;
  for(i = im->NumColors-1; i; i >>= 1)
    ++bits;
  size = 0;
  i = bitbuf = numzero = numbits = 0;
  while(i < im->Width*im->Height || numbits)
  {
    if(size == 127)
    {
      while(numbits % bits) ++numbits; /* very strange end wrapping */
      buf[size++] = 0; EndPutM32(bufs, size); ++(*numtt); buf += size; size = 0;
    }
    if(!size)
    {
      bufs = buf; buf += 4;
      buf[size++] = 'I'; buf[size++] = 'M'; buf[size++] = num; buf[size++] = '=';
    }
    if(numbits < 7)
    {
      if(i == im->Width*im->Height)
      {
        bitbuf <<= (7-numbits); numbits = 7;
      }
      else
      {
        bitbuf <<= bits; numbits += bits;
        bitbuf |= im->ImageData[i++];
      }
    }
    if(numbits >= 7)
    {
      t = (bitbuf>>(numbits-7))&0x7F;
      if(numzero == 47) { numzero = 0; buf[size++] = 0xFF; }
      else if(!t)       { ++numzero; numbits -= 7; }
      else
      {
        if(numzero) {buf[size++] = 0xD0+numzero; numzero = 0;}
        else
        {
          numbits -= 7;
          if(t <= 0x4F) buf[size++] = 0x20+t;
          else buf[size++] = (0xA1-0x50)+t;
        }
      }
    }
  }
  if(numzero) buf[size++] = 0xD0+numzero;
  buf[size++] = 0; EndPutM32(bufs, size); ++(*numtt); buf += size;

  *(totsize) = buf-sbuf;
}

/* This maybe a non-optimal RLE encoder, but it should be good enough for
   any required use. */
static long CreateRLE(unsigned char *buf, unsigned long depth,
unsigned char *dtype, unsigned char *dsize, unsigned long size,
unsigned char *src)
{
  int i, j, k;
  unsigned long bitbuf, numbits;
  long ressize, numcopy, numequal;

  numcopy = 0;
  numequal = 1;
  bitbuf = 0;
  numbits = 0;
  ressize = 0;
  k = 0; /* the really output pointer */
  for(i = 1; numequal || numcopy;)
  {
    if(i < size && numequal && (src[i-1] == src[i]))
    {
      ++numequal; ++i;
    }
    else if(i < size && numequal*depth <= 16)
    {
      numcopy += numequal; numequal = 1; ++i;
    }
    else
    {
      /* care for end case, where it maybe better to join the two */
      if(i == size && numcopy + numequal <= 128 && (numequal-1)*depth <= 8)
      {
        numcopy += numequal; numequal = 0;
      }
      if(numcopy)
      {
        if((j = numcopy) > 128) j = 128;
        bitbuf = (bitbuf<<8) | (j-1);
        numcopy -= j;
      }
      else
      {
        if((j = numequal) > 128) j = 128;
        bitbuf = (bitbuf<<8) | (256-(j-1));
        numequal -= j;
        k += j-1;
        j = 1;
      }
      buf[ressize++] = (bitbuf >> numbits);
      while(j--)
      {
        numbits += depth;
        bitbuf = (bitbuf<<depth) | src[k++];
        if(numbits >= 8)
        {
          numbits -= 8;
          buf[ressize++] = (bitbuf >> numbits);
        }
      }
      if(i < size && !numcopy && !numequal)
      {
        numequal = 1; ++i;
      }
    }
  }
  if(numbits)
    buf[ressize++] = bitbuf << (8-numbits);

  if(ressize > size) /* no RLE */
  {
    ressize = size;
    *dtype = 0;
    for(i = 0; i < size; ++i)
      buf[i]= src[i];
  }
  else
    *dtype = 1;
  EndPutM16(dsize, ressize-1);
  return ressize;
}

static long CreateNew(struct IconRepresentation *ico, unsigned char *buf)
{
  unsigned char *bufstart = buf, *sizefield;
  long i, j;
  int equalpal = 0;

  EndPutM32Inc(buf, 0x464F524D);
  buf += 4; /* size is stored later */
  EndPutM32Inc(buf, 0x49434F4E);

  EndPutM32Inc(buf, 0x46414345);
  EndPutM32Inc(buf, 6);
  *(buf++) = ico->NewImage[0].Width-1;
  *(buf++) = ico->NewImage[0].Height-1;
  *(buf++) = ico->OS35Flags;
  *(buf++) = ico->AspectRatio;
  i = ico->NewImage[0].NumColors;
  if(ico->NewImage[1].Width && ico->NewImage[1].NumColors > i)
    i = ico->NewImage[1].NumColors;
  EndPutM16Inc(buf, i-1);

  /* palette optimization to make palettes equal is done somewhere before */
  if(ico->NewImage[1].Width)
  {
    if(ico->NewImage[0].NumColors == ico->NewImage[1].NumColors)
    {
      for(i = 0; i < ico->NewImage[0].NumColors; ++i)
      {
        if(ico->NewImage[0].Palette[i].Red != ico->NewImage[1].Palette[i].Red)
          break;
        if(ico->NewImage[0].Palette[i].Green != ico->NewImage[1].Palette[i].Green)
          break;
        if(ico->NewImage[0].Palette[i].Blue != ico->NewImage[1].Palette[i].Blue)
          break;
      }
      if(i == ico->NewImage[0].NumColors)
        equalpal = 1;
    }
  }

  for(i = 0; i < 2 && ico->NewImage[i].Width; ++i)
  {
    EndPutM32Inc(buf, 0x494D4147);
    sizefield = buf; buf += 4;
    *(buf++) = (ico->NewImage[i].Transparency != -1 ? ico->NewImage[i].Transparency : 0);
    *(buf++) = (i < 1 || !equalpal) ? ico->NewImage[i].NumColors-1 : 0;
    j = ico->NewImage[i].Transparency != -1 ? 1 : 0;
    if(i < 1 || !equalpal)
      j |= 2;
    *(buf++) = j;
    buf[2] = 0;
    for(j = ico->NewImage[i].NumColors-1; j; j >>= 1)
      ++buf[2];
    j = buf[2];
    buf += 7;

    buf += CreateRLE(buf, j, sizefield+4+3, sizefield+4+6,
    ico->NewImage[i].Width * ico->NewImage[i].Height, ico->NewImage[i].ImageData);
    if(i < 1 || !equalpal)
    {
      UBYTE data[MAXCOLORS*3];
      for(j = 0; j < ico->NewImage[i].NumColors; ++j)
      {
        data[j*3+0] = ico->NewImage[i].Palette[j].Red;
        data[j*3+1] = ico->NewImage[i].Palette[j].Green;
        data[j*3+2] = ico->NewImage[i].Palette[j].Blue;
      }
      buf += CreateRLE(buf, 8, sizefield+4+4, sizefield+4+8, j*3, data);
    }
    else
    {
      sizefield[4+4] = 0;
      EndPutM16(sizefield+4+8, 0);
    }

    j = buf-(sizefield+4);
    EndPutM32(sizefield, j);
    if(j & 1)
      *(buf++) = 0;
  }

  EndPutM32(bufstart+4, buf-(bufstart+8));
  return buf-bufstart;
}

/* Compared to the icon.library this function clears most of the non-relevant
   fields and uses 0x2A2A2A2A for boolean pointers always instead of random
   values. This will increase cruncher rate a little bit (not worth
   mentioning) and looks somewhat nicer. Also the saved data in general is
   much more optimized and pad-bytes as well as pad bits are zeroed instead
   of random. Also the crunched data respects absolute buffer ends.
*/
static long CreateIcon(struct IconRepresentation *ico, unsigned char *buf)
{
  unsigned char *startbuf = buf;
  long i;

  EndPutM16Inc(buf, 0xE310);                     /* ic_Magic */
  EndPutM16Inc(buf, 1);                          /* ic_Version */
  EndPutM32Inc(buf, 0);                          /* ga_Next */
  EndPutM16Inc(buf, 0);                          /* ga_LeftEdge */
  EndPutM16Inc(buf, 0);                          /* ga_TopEdge */
  EndPutM16Inc(buf, ico->Width);                 /* ga_Width */
  EndPutM16Inc(buf, ico->Height);                /* ga_Height */
  i = 4;
  if(ico->IconMode == ICONMODE_BACKFILL)
    i |= 1;
  else if(ico->IconMode == ICONMODE_IMAGE)
    i |= 2;
  EndPutM16Inc(buf, i);                          /* ga_Flags */
  EndPutM16Inc(buf, 0);                          /* ga_Activation */
  EndPutM16Inc(buf, 0);                          /* ga_GadgetType */
  EndPutM32Inc(buf, 0x2A2A2A2A);                 /* ga_GadgetRender */
  EndPutM32Inc(buf, ico->IconMode ==
               ICONMODE_IMAGE ? 0x2A2A2A2A : 0); /* ga_SelectRender */
  EndPutM32Inc(buf, 0);                          /* ga_GadgetText */
  EndPutM32Inc(buf, 0);                          /* ga_MutualExclude */
  EndPutM32Inc(buf, 0);                          /* ga_SpecialInfo */
  EndPutM16Inc(buf, 0);                          /* ga_GadgetID */
  EndPutM32Inc(buf, ico->OSType == OSTYPE_13 ?
               0 : 1);                           /* ga_UserData */
  *(buf++) = ico->Type;                          /* ic_Type */
  *(buf++) = 0;                                  /* ic_Pad */
  EndPutM32Inc(buf, ico->DefaultTool ?
               0x2A2A2A2A : 0);                  /* ic_DefaultTool */
  EndPutM32Inc(buf, ico->ToolTypes[0] ?
               0x2A2A2A2A : 0);                  /* ic_ToolTypes */
  EndPutM32Inc(buf, ico->XPosition);             /* ic_CurrentX */
  EndPutM32Inc(buf, ico->YPosition);             /* ic_CurrentY */
  EndPutM32Inc(buf, ico->Drawer.Width ?
               0x2A2A2A2A : 0);                  /* ic_DrawerData */
  EndPutM32Inc(buf, 0);                          /* ic_ToolWindow */
  EndPutM32Inc(buf, ico->StackSize);             /* ic_StackSize */

  if(ico->Drawer.Width)
  {
    EndPutM16Inc(buf, ico->Drawer.LeftEdge);     /* nw_LeftEdge */
    EndPutM16Inc(buf, ico->Drawer.TopEdge);      /* nw_TopEdge */
    EndPutM16Inc(buf, ico->Drawer.Width);        /* nw_Width */
    EndPutM16Inc(buf, ico->Drawer.Height);       /* nw_Height */
    *(buf++) = 0/*255*/;                         /* nw_DetailPen */
    *(buf++) = 0/*255*/;                         /* nw_BlockPen */
    EndPutM32Inc(buf, 0);                        /* nw_IDCMPFlags */
    EndPutM32Inc(buf, 0);                        /* nw_Flags */
    EndPutM32Inc(buf, 0);                        /* nw_FirstGadget */
    EndPutM32Inc(buf, 0);                        /* nw_CheckMark */
    EndPutM32Inc(buf, 0);                        /* nw_Title */
    EndPutM32Inc(buf, 0);                        /* nw_Screen */
    EndPutM32Inc(buf, 0);                        /* nw_BitMap */
    EndPutM16Inc(buf, 0/*94*/);                  /* nw_MinWidth */
    EndPutM16Inc(buf, 0/*65*/);                  /* nw_MinHeight */
    EndPutM16Inc(buf, 0/*65535*/);               /* nw_MaxWidth */
    EndPutM16Inc(buf, 0/*65535*/);               /* nw_MaxWidth */
    EndPutM16Inc(buf, 0);                        /* nw_Type */
    EndPutM32Inc(buf, ico->Drawer.XPosition);    /* dd_CurrentX */
    EndPutM32Inc(buf, ico->Drawer.YPosition);    /* dd_CurrentY */
  }
  for(i = 0; i < 2; ++i)
  {
    long j, k;
    EndPutM16Inc(buf, 0);                        /* im_LeftEdge */
    EndPutM16Inc(buf, 0);                        /* im_TopEdge */
    EndPutM16Inc(buf, ico->OldImage[i].Width);   /* im_Width */
    EndPutM16Inc(buf, ico->OldImage[i].Height);  /* im_Height */
    EndPutM16Inc(buf, ico->OldImage[i].Depth);   /* im_Depth */
    EndPutM32Inc(buf, 0x2A2A2A2A);               /* im_ImageData */
    *(buf++) = 0;                                /* im_PlanePick */
    *(buf++) = 0;                                /* im_PlaneOnOff */
    EndPutM32Inc(buf, 0);                        /* im_Next */
    k = (((((ico->OldImage[i].Width+15)&(~15)) * ico->OldImage[i].Height)
    * ico->OldImage[i].Depth)>>3);
    for(j = 0; j < k; ++j)
      *(buf++) = ico->OldImage[i].ImageData[j];
    if(ico->IconMode != ICONMODE_IMAGE) /* skip second image! */
      ++i;
  }
  if(ico->DefaultTool)
  {
    for(i = 0; ico->DefaultTool[i]; ++i)
      buf[4+i] = ico->DefaultTool[i];
    buf[4+(i++)] = 0;
    EndPutM32Inc(buf, i);
    buf += i;
  }
  if(ico->ToolTypes[0])
  {
    unsigned char *buffer = buf;
    long numtt = 0;

    buf += 4;
    while(ico->ToolTypes[numtt])
    {
      for(i = 0; ico->ToolTypes[numtt][i]; ++i)
        buf[4+i] = ico->ToolTypes[numtt][i];
      buf[4+(i++)] = 0;
      EndPutM32Inc(buf, i);
      buf += i;
      ++numtt;
    }
    if(ico->NewIconImage[0].Width) /* NewIcon handling */
    {
      unsigned char *txt;

      EndPutM32Inc(buf, 2);
      *(buf++) = ' ';
      *(buf++) = 0;
      numtt += 2;
      EndPutM32Inc(buf, 41);
      txt = "*** DON'T EDIT THE FOLLOWING LINES!! ***";
      for(i = 0; i < 41; ++i)
        *(buf++) = txt[i];

      CreateNewIcon(buf, '1', &i, &numtt, ico->NewIconImage+0);
      buf += i;
    }
    if(ico->NewIconImage[1].Width) /* NewIcon handling */
    {
      CreateNewIcon(buf, '2', &i, &numtt, ico->NewIconImage+1);
      buf += i;
    }

    EndPutM32(buffer, (numtt+1)*4);
  }
  if(ico->OSType != OSTYPE_13 && ico->Drawer.Width)
  {
    EndPutM32Inc(buf, ico->Drawer.Flags);           /* dd_Flags */
    EndPutM16Inc(buf, ico->Drawer.ViewMode);        /* dd_ViewModes */
  }
  if(ico->NewImage[0].Width) /* OS3.5 */
    buf += CreateNew(ico, buf);

  return buf-startbuf;
}

/* optimizations and fix stuff **********************************************/

/* this reduces number of colors without changing anything in palette */
static void ReduceNumColors(struct IconImage *im)
{
  unsigned char colors[MAXCOLORS];
  int i, j;

  for(i = 0; i < MAXCOLORS; ++i)
    colors[i] = 0;
  for(i = 0; i < im->Width*im->Height; ++i)
    colors[im->ImageData[i]] = 1;
  for(i = j = 0; i < MAXCOLORS; ++i)
  {
    if(colors[i])
    {
      im->Palette[j] = im->Palette[i];
      colors[i] = j++;
    }
  }
  im->NumColors = j;
  for(i = 0; i < im->Width*im->Height; ++i)
    im->ImageData[i] = colors[im->ImageData[i]];
}

#ifdef TESTONLY
/* joins palette entries, sorts palette */
static void ReduceAndSortPalette(struct IconImage *im)
{
  unsigned char colors[MAXCOLORS];
  unsigned char colorsrev[MAXCOLORS];
  struct IconPalette p;
  int i, j, k, l;

  for(i = 0; i < im->NumColors; ++i)
    colors[i] = i;

  /* transparency is always first! */
  if(im->Transparency > 0)
  {
    p = im->Palette[0];
    k = im->Transparency;
    im->Palette[0] = im->Palette[k]; im->Palette[k] = p;
    l = colors[0]; colors[k] = colors[0]; colors[k] = l;
    im->Transparency = 0;
  }

  /* now sort the entries */
  for(i = !im->Transparency ? 1 : 0; i < im->NumColors; ++i)
  {
    for(j = i+1; j < im->NumColors; ++j)
    {
      k = sqr((im->Palette[i].Red-im->Palette[i].Blue))
         +sqr((im->Palette[i].Green-im->Palette[i].Blue))
         +sqr((im->Palette[i].Blue))
         +(im->Palette[i].Red*4 + im->Palette[i].Green*2 + im->Palette[i].Green)*1024;
      l = sqr((im->Palette[j].Red-im->Palette[i].Blue))
         +sqr((im->Palette[j].Green-im->Palette[i].Blue))
         +sqr((im->Palette[j].Blue))
         +(im->Palette[j].Red*4 + im->Palette[j].Green*2 + im->Palette[j].Green)*1024;
      if(l > k)
      {
        p = im->Palette[j];
        im->Palette[j] = im->Palette[i]; im->Palette[i] = p;
        l = colors[i]; colors[i] = colors[j]; colors[j] = l;
      }
    }
  }
#ifdef GGG
  /* and reduce double entries */
  for(i = j = !im->Transparency ? 1 : 0; i < im->NumColors-1; ++i)
  {
    k = (im->Palette[i].Red)+(im->Palette[i].Green<<8)+
        (im->Palette[i].Blue);
    l = (im->Palette[i+1].Red<<16)+(im->Palette[i+1].Green<<8)+
        (im->Palette[i+1].Blue);
    if(l != k)
    
    {
        p = im->Palette[j];
        im->Palette[j] = im->Palette[k]; im->Palette[k] = p;
        l = colors[i];
        colors[l] = j;
        colors[colors[j]] = i;
      }
    }
  }
#endif

  for(i = 0; i < im->NumColors; ++i)
    colorsrev[colors[i]] = i;
  for(i = 0; i < im->Width*im->Height; ++i)
    im->ImageData[i] = colorsrev[im->ImageData[i]];
}
#endif

static char *OptimizeAndFix(struct IconRepresentation *ico)
{
  int i;

  for(i = 0; i < 2; ++i)
  {
    if(ico->NewIconImage[i].Width)
    {
      ReduceNumColors(ico->NewIconImage+i);
//      ReduceAndSortPalette(ico->NewIconImage+i);
    }
    if(ico->NewImage[i].Width)
    {
//      ReduceNumColors(ico->NewImage+i);
//      ReduceAndSortPalette(ico->NewImage+i);
    }
    if(ico->OldImageChunky[i].Width)
    {
      ReduceNumColors(ico->OldImageChunky+i);
//      ReduceAndSortPalette(ico->OldImageChunky+i);
    }
  }

/* Change color palette of OS3.5 according to save space if possible
   by using one palette. */

  if(ico->StackSize < 4096) ico->StackSize = 4096;
  if(ico->DefaultTool && !ico->DefaultTool[0]) ico->DefaultTool = 0;
  if(ico->Type != ICONTYPE_DISK && ico->Type != ICONTYPE_DRAWER &&
     ico->Type != ICONTYPE_GARBAGE && ico->Type != ICONTYPE_KICK)
  {
    ico->Drawer.Width = 0;
  }
  else if(!ico->Drawer.Width)
  {
    /* add default drawer */
    ico->Drawer.XPosition = ico->Drawer.YPosition = 0;
    ico->Drawer.LeftEdge = ico->Drawer.TopEdge = 50;
    ico->Drawer.Width = 400;
    ico->Drawer.Height = 100;
    ico->Drawer.ViewMode = 0;
    ico->Drawer.Flags = 0;
  }

  return 0;
}

