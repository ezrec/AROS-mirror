;/* BulletMainFile.c - Execute me to compile me with SAS/C 6.56
sc NMINC STRMERGE STREQ NOSTKCHK SAVEDS IGNORE=73 BulletMainFile.c
quit ;*/

/* (c)  Copyright 1992 Commodore-Amiga, Inc.   All rights reserved.       */
/* The information contained herein is subject to change without notice,  */
/* and is provided "as is" without warranty of any kind, either expressed */
/* or implied.  The entire risk as to the use of this information is      */
/* assumed by the user.                                                   */

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/rdargs.h>
#include <dos/dos.h>
#include <dos/var.h>
#include <diskfont/diskfonttag.h>
#include <diskfont/diskfont.h>
#include <diskfont/glyph.h>
#include <diskfont/oterrors.h>
#include <utility/tagitem.h>
#include <string.h>
#include <graphics/displayinfo.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/bullet.h>
#include <proto/intuition.h>

#define OTAG_ID 0x0f03
#define BUFSIZE     256

#ifdef LATTICE
int CXBRK(void) { return (0);} /* Disable Lattice CTRL/C handling */
int chkabort(void) { return (0);}
#endif

UBYTE     *readargsstring = "FileName/A,FontName,Size/N,XDPI/N,YDPI/N\n";
UBYTE     *librarystring  = ".library";
UBYTE     *fontstring     = "fonts:cgtimes.font";
UBYTE     *dpivarname = "XYDPI";   /* Name of an X/Y DPI environment variable. */
                                   /* If this ENV: variable exists, this code  */
                                   /* will use the X and Y DPI stored there.   */
                                   /* This code will also save the X and Y DPI */
                                   /* in XYDPI if the user supplies a DPI.     */
                                   /* XYDPI encodes the DPI just like the      */
                                   /* OT_DeviceDPI tag.                        */
extern struct TagItem *AllocOtag(STRPTR);
extern void     FreeOtag(void *);
extern struct Library *OpenScalingLibrary(struct TagItem *);
extern void     CloseScalingLibrary(struct Library *);
extern struct GlyphEngine *GetGlyphEngine(struct TagItem *, STRPTR);
extern void     ReleaseGlyphEngine(struct GlyphEngine *);
extern void
BulletExample(struct GlyphEngine *,
                  struct Window *,
                  struct RastPort *, ULONG, ULONG, ULONG, STRPTR);

#define NUM_ARGS    5 /* Arguments for ReadArgs(). */
#define VIEW_FILE   0
#define FONT_NAME   1
#define SIZE        2
#define XDPI        3
#define YDPI        4
LONG            args[NUM_ARGS];
struct RDArgs  *myrda;

struct Library *BulletBase;
struct UtilityBase *UtilityBase;
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;

UBYTE           buf[BUFSIZE];
BPTR            fontfile, dpifile;
UBYTE          *otagname;
UWORD           fchid;

struct GlyphEngine *ge;

struct DrawInfo *drawinfo;
struct RastPort rp;

void            main(int argc, char **argv)
{
  struct TagItem *ti;
  struct GlyphEngine *ge;
  struct Window  *w;

  UBYTE           xydpi[5];

  LONG           defpointheight = 36;   /* Default values for ReadArgs() variables. */
  LONG           defxdpi = 72;
  LONG           defydpi = 72;

  if (GfxBase = OpenLibrary("graphics.library", 37L))
  {
    if (IntuitionBase = OpenLibrary("intuition.library", 37L))
    {
      if (myrda = ReadArgs(readargsstring, args, 0))
      {
        if (args[XDPI] && args[YDPI]) /* If the user sets the DPI from the command  */
        {            /* line, make sure the environment variable also gets changed. */
          *(ULONG *)xydpi = ( (*(LONG *)args[XDPI]) << 16 | (*(ULONG *)args[YDPI]) );
          SetVar(dpivarname, xydpi, 5,
              GVF_GLOBAL_ONLY | GVF_BINARY_VAR | GVF_DONT_NULL_TERM);
        }
        else                           /* If the user did NOT set the X OR Y DPI... */
        {
          args[XDPI] = (LONG) &defxdpi;/* ...set to default values and see if there */
          args[YDPI] = (LONG) &defydpi;/* there is an environment variable "XYDPI". */
                                                  /* Read the environment variable, */
          if ((GetVar(dpivarname, xydpi, 5,       /* XYDPI, if it exists.           */
              GVF_GLOBAL_ONLY | GVF_BINARY_VAR | GVF_DONT_NULL_TERM)) != -1)

/* BUG! In the original publication of this code, the line above erroneously tested */
/* tested for the wrong return value.  It caused unexpected results when using the  */
/* default X and Y DPI values.  This bug was also present in BulletMain.c.          */

          {
            if ( (*(ULONG *)xydpi & 0xFFFF0000) && (*(ULONG *)xydpi & 0x0000FFFF) )
            {          /* Make sure the environment variable is OK to use by making */
                       /* sure that neither X or YDPI is zero. If XYDPI is OK, use  */
              defxdpi = ((*(ULONG *)xydpi) & 0xFFFF0000) >> 16; /* it as a default. */
              defydpi = (*(ULONG *)xydpi) & 0x0000FFFF;
            }
          }
        }
        if (!(args[SIZE]))
          args[SIZE] = (LONG) &defpointheight;
        if (!(args[FONT_NAME]))
          args[FONT_NAME] = (LONG) fontstring;
                                           /* Open the ".font" file which contains  */
                                           /* the FontContentsHeader for this font. */

        if (fontfile = Open((STRPTR) args[FONT_NAME], MODE_OLDFILE))
        {
          if (Read(fontfile, &fchid, sizeof(UWORD)))
          {
            if (AROS_WORD2BE(fchid) == OTAG_ID)            /* Does this font have an .otag file? */
            {
              strcpy(buf, (STRPTR) args[FONT_NAME]);   /* Put together the .otag   */
              if (otagname = &(buf[strlen(buf) - 4]))  /* file name from the .font */
              {                                        /* file name.               */
                strcpy(otagname, "otag");
                if (UtilityBase = OpenLibrary("utility.library", 37L))
                {
                  if (ti = AllocOtag(buf))      /* open the otag file and copy its */
                  {                             /* tags into memory.               */
                    if (BulletBase = OpenScalingLibrary(ti)) /* Pass the function  */
                    {                                  /* the OTAG tag list which  */
                      if (ge = GetGlyphEngine(ti, buf))/* it needs to open the     */
                      {                              /* scaling library.  Open the */
                                                     /* library's scaling engine.  */
                        if (w = OpenWindowTags(NULL,
                                               WA_Width,         640,
                                               WA_Height,        200,
                                               WA_SmartRefresh,  TRUE,
                                               WA_SizeGadget,    FALSE,
                                               WA_CloseGadget,   TRUE,
                                               WA_IDCMP,         IDCMP_CLOSEWINDOW,
                                               WA_DragBar,       TRUE,
                                               WA_DepthGadget,   TRUE,
                                               WA_Title,         (ULONG)argv[0],
                                               TAG_END))
                        {
                          rp = *(w->RPort);    /* Clone window's RastPort.  The    */
                                               /* second Rastport is for rendering */
                                               /* with the background color.       */
                          if (drawinfo = GetScreenDrawInfo(w->WScreen)) /* Get the */
                          {            /* screen's DrawInfo to get its pen colors. */
                            SetAPen(w->RPort, drawinfo->dri_Pens[TEXTPEN]);
                            SetAPen(&rp, drawinfo->dri_Pens[BACKGROUNDPEN]);
                            FreeScreenDrawInfo(w->WScreen, drawinfo);
                          }

                          BulletExample(ge, w, &rp, *(ULONG *) args[SIZE],
                                        *(ULONG *) args[XDPI],
                                        *(ULONG *) args[YDPI],
                                        (STRPTR) args[VIEW_FILE]);
                          WaitPort(w->UserPort);
                          ReplyMsg(GetMsg(w->UserPort));
                          CloseWindow(w);
                        }
                        ReleaseGlyphEngine(ge);
                      }
                      CloseScalingLibrary(BulletBase);
                    }
                    FreeOtag(ti);
                  }
                  CloseLibrary(UtilityBase);
                }
              }
            }
          }
          Close(fontfile);
        }
        FreeArgs(myrda);
      }
      CloseLibrary(IntuitionBase);
    }
    CloseLibrary(GfxBase);
  }
}
