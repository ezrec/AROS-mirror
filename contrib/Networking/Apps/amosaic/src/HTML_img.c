/****************************************************************************
 * NCSA Mosaic for the X Window System                                      *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 * mosaic@ncsa.uiuc.edu                                                     *
 *                                                                          *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois      *
 *                                                                          *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois       *
 * (UI), and ownership remains with the UI.                                 *
 *                                                                          *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such         *
 * copies.                                                                  *
 *                                                                          *
 * Licensee may make derivative works.  However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then          *
 * Licensee will (1) notify NCSA regarding its distribution of the          *
 * derivative work, and (2) clearly notify users that such derivative       *
 * work is a modified version and not the original NCSA Mosaic              *
 * distributed by the UI.                                                   *
 *                                                                          *
 * Any Licensee wishing to make commercial use of the Software should       *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or       *
 * part of the source code into a product for sale or license by or on      *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a           *
 * commercial product sold or licensed by or on behalf of Licensee.         *
 *                                                                          *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.                                                  *
 *                                                                          *
 * By using or copying this Software, Licensee agrees to abide by the       *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.      *
 * UI shall have the right to terminate this license immediately by         *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any          *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.                           *
 *                                                                          *
 * Comments and questions are welcome and can be sent to                    *
 * mosaic-x@ncsa.uiuc.edu.                                                  *
 ****************************************************************************/

#include "includes.h"
#include "XtoI.h"
#include "HTML.h"   /* THIS IS OK, PUBLIC INFO */
#include "mosaic.h"
 /* You don't need to see inside the gadget */
 /* Maybe you do.... make sure that this only happens *inside the html gad */
#include "HTMLImgMacro.h"
#include "globals.h"
#include "protos.h"

#define TRACE 0

#include "bitmaps/gopher_image.xbm"
#include "bitmaps/gopher_movie.xbm"
#include "bitmaps/gopher_menu.xbm"
#include "bitmaps/gopher_text.xbm"
#include "bitmaps/gopher_sound.xbm"
#include "bitmaps/gopher_index.xbm"
#include "bitmaps/gopher_telnet.xbm"
#include "bitmaps/gopher_binary.xbm"
#include "bitmaps/gopher_unknown.xbm"

static ImageInfo *gopher_image = NULL;
static ImageInfo *gopher_movie = NULL;
static ImageInfo *gopher_menu = NULL;
static ImageInfo *gopher_text = NULL;
static ImageInfo *gopher_sound = NULL;
static ImageInfo *gopher_index = NULL;
static ImageInfo *gopher_telnet = NULL;
static ImageInfo *gopher_binary = NULL;
static ImageInfo *gopher_unknown = NULL;

extern Pixmap CreateBitMapFromXBMData(UBYTE *, int, int, int);
extern unsigned char *ReadXbmBitmap(FILE *, char *, int *, int *, int *);

/* Defined in gui.c */
extern char *cached_url;
extern mo_window *current_win;
/* Defined in gui-documents.c */
extern int interrupted;
extern int loading_inlined_images;
/* ------------------------------------------------------------------------ */

/* Image resolution function. */


mo_status mo_free_image_data (void *ptr)
{
  ImageInfo *img = (ImageInfo *)ptr;

  if (!img)
    return mo_fail;

  if (img->reds)
    free (img->reds);
  if (img->greens)
    free (img->greens);
  if (img->blues)
    free (img->blues);
  if (img->image_data)
    free (img->image_data);

  return mo_succeed;
}


ImageInfo *ImageResolve (HTMLWidget hw, char *src, int noload)
{
  ImageInfo *img_data;

  if (!src) {
    return NULL;
  }

  /* Internal images. */
  if (strncmp (src, "internal-", 9) == 0)
    {    
      if (strcmp (src, "internal-gopher-image") == 0)
        RETURN_IMGINFO_FROM_BITMAP(gopher_image);
      if (strcmp (src, "internal-gopher-movie") == 0)
        RETURN_IMGINFO_FROM_BITMAP(gopher_movie);
      if (strcmp (src, "internal-gopher-menu") == 0)
        RETURN_IMGINFO_FROM_BITMAP(gopher_menu);
      if (strcmp (src, "internal-gopher-text") == 0)
        RETURN_IMGINFO_FROM_BITMAP(gopher_text);
      if (strcmp (src, "internal-gopher-sound") == 0)
        RETURN_IMGINFO_FROM_BITMAP(gopher_sound);
      if (strcmp (src, "internal-gopher-index") == 0)
        RETURN_IMGINFO_FROM_BITMAP(gopher_index);
      if (strcmp (src, "internal-gopher-telnet") == 0)
        RETURN_IMGINFO_FROM_BITMAP(gopher_telnet);
      if (strcmp (src, "internal-gopher-binary") == 0)
        RETURN_IMGINFO_FROM_BITMAP(gopher_binary);
      if (strcmp (src, "internal-gopher-unknown") == 0)
        RETURN_IMGINFO_FROM_BITMAP(gopher_unknown);
    }

  src = mo_url_canonicalize (src, cached_url);

  /* Go see if we already have the image info hanging around. */
  img_data = mo_fetch_cached_image_data (src);

  if (img_data)
    {
      free (src);
      return (ImageInfo *)img_data;
    }

  free (src);
  return NULL;
}

// mjw moved to HTMLamiga.c: int ImageLoadAndCache(HTMLGadClData *hw, char *src)
