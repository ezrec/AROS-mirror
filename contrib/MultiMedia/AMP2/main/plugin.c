/*
 *
 * plugin.c
 *
 */

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "plugin.h"
#include "types.h"
#include "prefs.h"

/* external parsers */
extern int rm_parser(char *filename, int video, int audio);
extern int avi_parser(char *filename, int video, int audio);
extern int mpeg_video_parser(char *filename, int video, int audio);
extern int mpeg_audio_parser(char *filename, int video, int audio);
extern int a52_parser(char *filename, int video, int audio);
extern int mpeg_system_parser(char *filename, int video, int audio);
extern int qt_parser(char *filename, int video, int audio);

typedef struct {
  int type;
  int (*parser) (char *filename, int video, int audio);
  char *description;
} parser_type;

parser_type parsers[] = {
{RM_SYSTEM,   rm_parser,          "Real Video"},
{AVI_ANIM,    avi_parser,         "AVI"},
{MPEG_VIDEO,  mpeg_video_parser,  "MPEG Video"},
{MPEG_AUDIO,  mpeg_audio_parser,  "MPEG Audio"},
{AC3_AUDIO,   a52_parser,         "A52 Audio"},
{MPEG_SYSTEM, mpeg_system_parser, "MPEG System"},
{QT_ANIM,     qt_parser,          "QT/MOV"},
{-1,          NULL,               NULL}};

/* external plugins */
extern void mpeg_plugin(char *filename);

void start_plugin(char *filename)
{
  int type, i;

  /* VCD hack */
  if ((strcasecmp(filename, "vcd") == 0) ||
      (strcasecmp(filename, "cdi") == 0) ||
      (strcasecmp(filename, "dvd") == 0)) {
    mpeg_plugin(filename);
    return;
  }

  type = get_type(filename);

  /* Find parser and execute */
  i = 0;
  while (parsers[i].parser != NULL) {
    if (parsers[i].type == type) {
      amp_printf("Filetype: %s\n", parsers[i].description);
      parsers[i].parser(filename, (prefs.no_video == PREFS_OFF), (prefs.no_audio == PREFS_OFF));
      break;
    }
    i++;
  }

  if (parsers[i].parser == NULL) {
    printf("Filetype: Unknown\n");
  }
}
