/*
 *
 * prefs.h
 *
 */

#ifndef PREFS_H
#define PREFS_H

#define PREFS_BESTMODE  0
#define PREFS_REQUESTER 1
#define PREFS_PAL       2
#define PREFS_HIPAL     3

#define PREFS_GRAY  0
#define PREFS_COLOR 1
#define PREFS_HAM   2

#define PREFS_NORMAL 0
#define PREFS_HIGH   1

#define PREFS_ON  1
#define PREFS_OFF 0

#define PREFS_LOWCOLOR  0
#define PREFS_HIGHCOLOR 1
#define PREFS_TRUECOLOR 2

#define PREFS_TRUE  1
#define PREFS_FALSE 0

#define PREFS_8BIT  0
#define PREFS_14BIT 1

typedef struct prefstypedef
{
  /* User selectable */
  int screenmode;             /* BestMode, Requester, PAL, HIPAL */
  int colormode;              /* Gray, Color, HAM */
  int ham_depth;              /* 6, 8 */
  int ham_width;              /* 1, 2, 4 */
  int ham_quality;            /* Normal, High */
  int half;                   /* On, Off */
  int gray_depth;             /* 4, 6, 8 */
  int cgfx_depth;             /* LowColor(8), HighColor(15/16), TrueColor(24/32) */
  int window;                 /* On, Off */
  int debug;                  /* On, Off */
  int verbose;                /* On, Off */
  int overlay;                /* On, Off */
  int stereo;                 /* On, Off */
  int no_video;               /* On, Off */
  int no_audio;               /* On, Off */
  char device[32];            /* Name of the device */
  int unit;                   /* Unit */
  int ahi;                    /* On, Off */
  int triple;                 /* On, Off */
  int filter;                 /* On, Off */
  int readall;                /* On, Off */
  int divisor;                /* 1, 2, 4 */
  int lores;                  /* On, Off */
  int osd;                    /* On, Off */
  int audiomode;              /* 8bit, 14bit */
  int khz56;                  /* On, Off */
  char calibration[256];      /* Name of calibration file */
  char subtitle[256];         /* Name of subtitle file */

  /* DVD */
  int ac3_track;              /* 0 (Off), 0x80-0x87 */
  int subtitle_track;         /* 0 (Off), 0x20-0x3f */

  /* Internal */
  char registered[128];       /* Name and serial */
  int play_video;             /* On, Off */
  int play_audio;             /* On, Off */
  char window_title[128];     /* Version, filename and resolution */
  char cwd[256];              /* Current Work Directory */

  /* Development */
  int speedtest;              /* May differ between versions */
} prefstype;

extern void set_prefs(void);
extern void check_prefs(void);
extern prefstype prefs;

#endif
