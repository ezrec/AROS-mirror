;/* Engine.c - Execute me to compile me with SAS/C 6.56
sc NMINC STRMERGE STREQ NOSTKCHK SAVEDS IGNORE=73 Engine.c
quit ;*/

/* (c)  Copyright 1992 Commodore-Amiga, Inc.   All rights reserved.       */
/* The information contained herein is subject to change without notice,  */
/* and is provided "as is" without warranty of any kind, either expressed */
/* or implied.  The entire risk as to the use of this information is      */
/* assumed by the user.                                                   */

#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dostags.h>
#include <dos/dos.h>
#include <diskfont/diskfonttag.h>
#include <diskfont/diskfont.h>
#include <diskfont/glyph.h>
#include <diskfont/oterrors.h>
#include <utility/tagitem.h>
#include <string.h>
#include <aros/macros.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/bullet.h>

#define OTAG_ID 0x0f03   /* this really belongs in <diskfont/diskfont.h>, */
                         /* but it's not there, yet.                      */

extern UBYTE   *librarystring; /* ".library", defined in BulletMain.c. */

struct TagItem *AllocOtag(STRPTR);
void            FreeOtag(void *);
struct Library *OpenScalingLibrary(struct TagItem *);
void            CloseScalingLibrary(struct Library *);
struct GlyphEngine *GetGlyphEngine(struct TagItem *, STRPTR);
void            ReleaseGlyphEngine(struct GlyphEngine *);

#define BUFSIZE     256

extern struct Library *BulletBase;
extern struct UtilityBase *UtilityBase;

/*************************************************************************************/
/* open the otag file, allocate a buffer, read the file into the buffer, verify that */
/* the file is OK, relocate all of the address relocation tags, close the otag file. */
/*************************************************************************************/
struct TagItem *
AllocOtag(STRPTR otagname)
{
  BPTR            otfile;
  struct TagItem *ti, *tip, *returnti;
  struct FileInfoBlock *fib;

  ti = NULL;

  if (fib = AllocDosObject(DOS_FIB, NULL))    /* The FileInfoBlock of the OTAG file */
  {                                           /* contains the file's size.          */
    if (otfile = Open(otagname, MODE_OLDFILE))
    {
      if (ExamineFH(otfile, fib))
      {
        if (returnti = (struct TagItem *) AllocVec(fib->fib_Size, MEMF_CLEAR))
        {
          if (Read(otfile, (UBYTE *) returnti, fib->fib_Size))
          {
            if ((returnti->ti_Tag == AROS_LONG2BE(OT_FileIdent))               /* Test to see if */
                && (AROS_LONG2BE(returnti->ti_Data) == (ULONG) fib->fib_Size)) /* the OTAG file  */
            {                                                    /* is valid.      */
              ti = returnti;
              while (AROS_LONG2BE(ti->ti_Tag) != TAG_DONE)     /* Step through and relocate tags */
              {
	        ti->ti_Tag  = AROS_LONG2BE(ti->ti_Tag);
		ti->ti_Data = AROS_LONG2BE(ti->ti_Data);
                if (ti->ti_Tag & OT_Indirect)
                {
                  ti->ti_Data = (ULONG) returnti + ti->ti_Data;
                }
		ti++;
              }
            }
          }
        }
      }
      Close(otfile);
    }
    FreeDosObject(DOS_FIB, fib);
  }
  return (returnti);
}

/**************************************************************************/
/*********** Deallocates resources allocated by AllocOtag(). **************/
/**************************************************************************/
void
FreeOtag(void *vector)
{
  FreeVec(vector);
}


/*****************************************************************************/
/******* Scans through a TagList looking for an scaling engine name. *********/
/*******           If it finds one, it opens that library.           *********/
/*****************************************************************************/
struct Library *
OpenScalingLibrary(struct TagItem * ti)
{
  STRPTR          enginename;
  UBYTE           libnamebuffer[BUFSIZE];

  if (enginename = (STRPTR) GetTagData(OT_Engine, NULL, ti))
  {
    strcpy(libnamebuffer, enginename);
    strcat(libnamebuffer, librarystring);

    return (OpenLibrary(libnamebuffer, 0L));
  }
  return(NULL);  /*   <----- BUG!:--  This line was missing in the original */
}                /*                        publication of this code module. */

/**************************************************************************/
/******* Deallocates resources allocated by OpenScalingLibrary(). *********/
/**************************************************************************/
void
CloseScalingLibrary(struct Library * bbase)
{
  CloseLibrary(bbase);
}


/**************************************************************************/
/* Open the glyph engine, give it the tags from the otag file, and set up */
/* a default device dpi so it doesn't crash if someone forgets to set it. */
/**************************************************************************/
struct GlyphEngine *
GetGlyphEngine(struct TagItem * ti, STRPTR otagpath)
{
  struct GlyphEngine *ge = NULL;
  BOOL            ok = TRUE;

  if (ge = OpenEngine())
  {
    ok = FALSE;
    if (SetInfo(ge,
                OT_OTagList, ti,
                OT_OTagPath, otagpath,
                TAG_END) == OTERR_Success)
    {
      if (SetInfo(ge,
                  OT_DeviceDPI, ((ULONG) 77) << 16 | 75,
                  TAG_END) == OTERR_Success)
      {
        ok = TRUE;
      }
    }
  }

  if (!ok)
  {
    CloseEngine(ge);
    ge = NULL;
  }
  return (ge);
}

/**************************************************************************/
/********* Deallocates resources allocated by GetGlyphEngine(). ***********/
/**************************************************************************/
void
ReleaseGlyphEngine(struct GlyphEngine * ge)
{
  CloseEngine(ge);
}
