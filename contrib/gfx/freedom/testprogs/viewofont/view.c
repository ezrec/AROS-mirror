;/* View.c - Execute me to compile me with SAS/C 6.56
sc NMINC STRMERGE STREQ NOSTKCHK SAVEDS IGNORE=73 View.c
slink FROM LIB:c.o,BulletMainFile.o,engine.o,View.o TO View LIBRARY LIB:sc.lib,LIB:amiga.lib
quit ;*/

/* (c)  Copyright 1992 Commodore-Amiga, Inc.   All rights reserved.       */
/* The information contained herein is subject to change without notice,  */
/* and is provided "as is" without warranty of any kind, either expressed */
/* or implied.  The entire risk as to the use of this information is      */
/* assumed by the user.                                                   */

#include <exec/types.h>
#include <exec/memory.h>
#include <diskfont/diskfonttag.h>
#include <diskfont/diskfont.h>
#include <diskfont/glyph.h>
#include <diskfont/oterrors.h>
#include <graphics/gfx.h>
#include <utility/tagitem.h>
#include <intuition/intuition.h>
#include <devices/timer.h>
#include <dos/dos.h>

//#include <clib/alib_stdio_protos.h>
#include <clib/alib_protos.h>

#include <proto/bullet.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

//#pragma header

UBYTE          *vers = "\0$VER: View 38.7";

extern struct Library *BulletBase;
extern struct UtilityBase *UtilityBase;
extern struct GfxBase *GfxBase;
extern struct IntuitionBase *IntuitionBase;

void            BulletExample(struct GlyphEngine *,
                              struct Window *,
                              struct RastPort *,
                              ULONG, ULONG, ULONG, STRPTR);
struct GlyphMap *gm;
PLANEPTR        tempbitmap;
struct IntuiMessage *mymsg;
UBYTE          *viewfilebuf, *currchar;
ULONG           currposition, emheight, emwidth, x, y;
ULONG				realemwidth;
LONG				xreal;

BPTR            viewfile;
LONG            actual;
struct Task    *mytask;
struct FileInfoBlock *fib;

void
BulletExample(struct GlyphEngine * ge,
              struct Window * w, struct RastPort * rp,
              ULONG pointheight, ULONG xdpi, ULONG ydpi, STRPTR viewfilename)
{
  LONG newwidthreal;
  UWORD           wlimitx, wlimity, newwidth;
  FIXED           kern;

  wlimitx = w->Width  - w->BorderRight  - 2; /* The X and Y extent of the window */
  wlimity = w->Height - w->BorderBottom - 2; /* that we can draw into.           */

  if (SetInfo(ge,                                      /* Set up the X and Y DPI of */
              OT_DeviceDPI, ((ULONG)xdpi) << 16 | ydpi,/* target raster.  Neither   */
              OT_PointHeight, (ULONG)pointheight << 16,/* of these can be zero!     */
              TAG_END) != OTERR_Success)               /* BulletMainFile.c checks   */
    return;                                            /* for zero.                 */

  if (viewfile = Open(viewfilename, MODE_OLDFILE))/* Open the ASCII file to display.*/
  {
    if (fib = AllocDosObject(DOS_FIB, NULL))      /* Find out how big the display   */
    {                                             /* file is by looking at its      */
      if (ExamineFH(viewfile, fib))               /* FileInfoBlock.  Allocate that  */
      {                                           /* Much memory.                   */
        if (viewfilebuf = (UBYTE *) AllocVec(fib->fib_Size, MEMF_CLEAR))
        {
          if (Read(viewfile, (UBYTE *) viewfilebuf, fib->fib_Size))    /* Read the  */
          {                                          /* whole file into its buffer. */
            SetDrMd(w->RPort, JAM1);
            if (tempbitmap = AllocRaster(640, 200)) /* Allocate some Chip RAM space */
            {      /* where we can temporarily store the glyph so we can blit it.   */
              if (ModifyIDCMP(w, IDCMP_CLOSEWINDOW))  /* Turn on the Close gadget.  */
              {
                emheight = (pointheight * ydpi) / 72; /* Calculate the dimensions   */
                realemwidth = (pointheight *xdpi);
                emwidth = (pointheight * xdpi) / 72;  /* of the EM square in screen */
                     /* pixels. This is necessary because bullet.library measures   */
                     /* character widths and kerning values as fractions of an EM.  */
                     /* An EM (pronounced like the letter 'M') is a measure of      */
                     /* distance that is equal to the point size of a typeface      */
                     /* (which means one EM is not constant across different type   */
                     /* sizes).  For a 72 point typeface, one EM = 72 points which  */
                     /* approximately equals one inch.                              */

                x = w->BorderLeft + 2;           /* Calculate the starting point    */
                y = w->BorderTop + 2 + emheight; /* for glyph rendering.            */

					 xreal = x *65536;
                                      /* Step through each character in the buffer. */
                for (currposition = 0; currposition < fib->fib_Size; currposition++)
                {
                                /* Set the current glyph, which is the one we'll be */
                                /* rendering in this interation of the loop, and    */
                                /* the secondary glyph, which, besides being the    */
                                /* next glyph we will render, is necessary to find  */
                                /* the proper kerning value between the glyphs.     */
                                /* Notice that this example does not account for    */
                                /* the presence of non-printables (carriage return, */
                                /* DEL, etc.) which effects the kerning.  A real    */
                  if (SetInfo(ge,  /* application should consider these.            */
                              OT_GlyphCode,  (ULONG) viewfilebuf[currposition],
                              OT_GlyphCode2, (ULONG) viewfilebuf[currposition + 1],
                              OT_EmboldenX,0x2000,
                              OT_EmboldenY,0x2000,
                              TAG_END) == OTERR_Success)
                  {
                    kern = 0;         /* Find the kerning adjustment between glyph1 */
                                      /* and glyph2.  This example doesn't account  */
                                      /* for the validity of the glyphs.            */
                    ObtainInfo(ge, OT_TextKernPair, &kern, TAG_END);

                                                  /* Ask the scaling engine for the */
                                                  /* bitmap for the current glyph.  */
                    if ((ObtainInfo(ge, OT_GlyphMap, &gm, TAG_END)) == OTERR_Success)
                    {
                       /* Calculate the width of the current character including    */
                       /* any kerning adjustment.  Because the width is represented */
                       /* as a fixed point binary fraction of an EM, this needs to  */
                       /* be converted to a width in screen pixels.                 */
                      newwidthreal = ((gm->glm_Width - kern) * realemwidth)/72;
                      
                      newwidth = newwidthreal/65536;


                      if ((x + newwidth) > wlimitx)  /* Make sure the glyph gets    */
                      {                              /* rendered inside the window  */
                                                     /* bounds.                     */
                                                     
                        x = w->BorderLeft + 2;
                        xreal = x * 65536;

                        y += emheight;
                        if (y > wlimity)   /* If the text goes beyond the bottom of */
                        {                  /* the window, clear the window and move */
                                           /* the current rendering position to the */
                                           /* upper left.                           */
                          y = w->BorderTop + 2 + emheight;
                          RectFill(rp, w->BorderLeft, w->BorderTop, wlimitx, wlimity);
                        }
                      }

                      CopyMem(gm->glm_BitMap,/* Copy the raw bitmap to chip memory. */
                              tempbitmap,
                              gm->glm_BMModulo * gm->glm_BMRows);

                      BltTemplate(           /* Render the glyph using the blitter  */
                                             /* and the RastPort settings.          */
                                   (PLANEPTR) (((ULONG) tempbitmap)
                                     + (gm->glm_BMModulo * gm->glm_BlackTop)
                                         + ((gm->glm_BlackLeft >> 4) << 1)),
                                   gm->glm_BlackLeft & 0xF,
                                   gm->glm_BMModulo,
                                   w->RPort,
                                   x - gm->glm_X0 + gm->glm_BlackLeft,
                                   y - gm->glm_Y0 + gm->glm_BlackTop,
                                   gm->glm_BlackWidth,     /* glm_X0 & Y0 are used  */
                                   gm->glm_BlackHeight);   /* to make the example a */
                                    /* little simpler.  They are not as accurate as */
                                    /* using glm_XOrigin and glm_YOrigin in con-    */
                                    /* juntion with fractional width and kerning    */
                                    /* values.                                      */

                      xreal += newwidthreal;
                      x = (xreal) / 65536;

                      ReleaseInfo(ge, OT_GlyphMap, gm, TAG_END);
                    }
                    while (mymsg = (struct IntuiMessage *) GetMsg(w->UserPort))
                    {                                       /* Did the user hit the */
                      ReplyMsg((struct Message *) mymsg);   /* Close Gadget?        */
                      currposition = fib->fib_Size + 1;
                    }
                  }
                }
              }
              FreeRaster(tempbitmap, 640, 200);
            }
          }
          FreeVec(viewfilebuf);
        }
      }
      FreeDosObject(DOS_FIB, fib);
    }
    Close(viewfile);
  }
}
