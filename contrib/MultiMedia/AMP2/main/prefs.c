/*
 *
 * prefs.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prefs.h"

prefstype prefs;

void set_prefs()
{
  memset(&prefs, 0, sizeof(prefs));

  /* User selectable */
  prefs.screenmode     = PREFS_BESTMODE;     /* Best Mode */
  prefs.colormode      = PREFS_COLOR;        /* Color */
  prefs.ham_depth      = 8;                  /* 8 */
  prefs.ham_width      = 2;                  /* 2 */
  prefs.ham_quality    = PREFS_NORMAL;       /* Normal */
  prefs.half           = PREFS_OFF;          /* Off */
  prefs.gray_depth     = 8;                  /* 8 */
  prefs.cgfx_depth     = PREFS_HIGHCOLOR;    /* 15/16 */
  prefs.window         = PREFS_OFF;          /* Off */
  prefs.debug          = PREFS_OFF;          /* Off */
  prefs.verbose        = PREFS_OFF;          /* Off */
  prefs.overlay        = PREFS_OFF;          /* Off */
  prefs.stereo         = PREFS_OFF;          /* Off */
  prefs.no_video       = PREFS_OFF;          /* Off */
  prefs.no_audio       = PREFS_OFF;          /* Off */
  strcpy(prefs.device, "ata.device");      /* atapi.device */
  prefs.unit           = 2;                  /* 2 */
  prefs.ahi            = PREFS_ON;          /* Off */
  prefs.triple         = PREFS_OFF;          /* Off */
  prefs.filter         = PREFS_OFF;          /* Off */
  prefs.readall        = PREFS_OFF;          /* Off */
  prefs.divisor        = 2;                  /* 2 */
  prefs.lores          = PREFS_OFF;          /* Off */
  prefs.osd            = PREFS_OFF;          /* Off */
  prefs.audiomode      = PREFS_8BIT;         /* 8bit */
  prefs.khz56          = PREFS_OFF;          /* Off */
  /* calibration */                          /* Filled in later */
  /* subtitle */                             /* Filled in later */

  /* DVD */
  prefs.ac3_track      = 0x80;               /* Track 0 */
  prefs.subtitle_track = 0;                  /* Off */

  /* Internal */
  /* registered */                           /* Unregistered */
  prefs.play_video     = PREFS_OFF;          /* Off */
  prefs.play_audio     = PREFS_OFF;          /* Off */
  strcpy(prefs.window_title, "AMP2, ");      /* Version */
  /* cwd */                                  /* Filled in later */

  /* Development */
  prefs.speedtest      = 0x00;               /* No speedtest */
}

void check_prefs()
{
  /* Check values */
  if ((prefs.ham_depth != 6) && (prefs.ham_depth != 8))
    prefs.ham_depth = 8;
  if ((prefs.ham_width != 1) && (prefs.ham_width != 2) && (prefs.ham_width != 4))
    prefs.ham_width = 2;
  if ((prefs.gray_depth != 4) && (prefs.gray_depth != 6) && (prefs.gray_depth != 8))
    prefs.gray_depth = 8;
  if ((prefs.unit < 0) || (prefs.unit > 15))
    prefs.unit = 2;
  if ((prefs.divisor != 1) && (prefs.divisor != 2) && (prefs.divisor != 4))
    prefs.divisor = 2;

  /* Limitations */
  if ((prefs.screenmode == PREFS_PAL) && (prefs.ham_width == 4)) {
    prefs.half = PREFS_OFF;
  }
  if ((prefs.screenmode == PREFS_HIPAL) && (prefs.ham_width == 4)) {
    prefs.ham_width = 2;
  }
  if ((prefs.screenmode != PREFS_PAL) && (prefs.screenmode != PREFS_HIPAL)) {
    prefs.ham_width = 1;
    prefs.half = PREFS_OFF;
  }
  if (prefs.debug == PREFS_ON) {
    prefs.verbose = PREFS_ON;
  }
}
