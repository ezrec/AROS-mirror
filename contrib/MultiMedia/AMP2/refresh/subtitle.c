/*
 * subtitle.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "subtitle.h"

subtitle_frame *subtitle = NULL;
static char *subtitle_lines = NULL;

/* local functions */
static int subtitle_parse_microdvd(char *filename, double framerate);
static int subtitle_parse_subrip(char *filename, double framerate);

int subtitle_parse_init(void)
{
  subtitle = malloc(16384 * sizeof(subtitle_frame));
  if (subtitle == NULL) {
    return 1;
  }
  memset(subtitle, 0, 16384 * sizeof(subtitle_frame));

  subtitle_lines = malloc(262144); /* FIXME: use filesize */
  if (subtitle_lines == NULL) {
    return 1;
  }
  memset(subtitle_lines, 0, 262144);

  return 0;
}

int subtitle_parse(char *filename, double framerate)
{
  char *ext = strstr(filename, ".");

  if (ext == NULL) {
    printf("filetype unknown\n");
    return 1;
  }

  if (strcasecmp(ext, ".sub") == 0) {
    printf("MicroDVD\n");
    return subtitle_parse_microdvd(filename, framerate);
  } else if (strcasecmp(ext, ".srt") == 0) {
    printf("SubRip\n");
    return subtitle_parse_subrip(filename, framerate);
  } else {
    printf("filetype unknown\n");
    return 1;
  }

  return 0;
}

int subtitle_parse_exit(void)
{
  if (subtitle_lines != NULL) {
    free(subtitle_lines);
    subtitle_lines = NULL;
  }

  if (subtitle != NULL) {
    free(subtitle);
    subtitle = NULL;
  }

  return 0;
}

/* Actual subtitle parsers, move to separate files */

static int subtitle_parse_microdvd(char *filename, double framerate)
{
  char txt[256], *ptr, *lines;
  FILE *fp = NULL;
  int i, j, val;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("failed to open %s\n", filename);
    return 1;
  }
  lines = subtitle_lines;

  /*
   * {1256}{1318}But she had an enchantment upon her|of a fearful sort,
   *
   */

  i = 0;
  while (!feof(fp)) {
    memset(txt, 0, 256);
    fgets(txt, 256, fp);

    /* remove any EOL junk */
    for (j=0; j<256; j++) {
      if ((txt[j] == 0x0a) || (txt[j] == 0x0d)) {
        txt[j] = 0;
      }
    }

    /* show */
    sscanf(&txt[1], "%d", &val);
    subtitle[i].show = (double)val / framerate;

    /* hide */
    ptr = strstr(&txt[1], "{");
    if (ptr == NULL) {
      break;
    }
    sscanf(ptr+1, "%d", &val);
    subtitle[i].hide = (double)val / framerate;

    /* text */
    ptr = strstr(ptr, "}");
    if (ptr == NULL) {
      break;
    }
    ptr++;

    subtitle[i].line[0] = lines;
    while (*ptr != 0) {
      /* new line ? */
      if (*ptr == '|') {
        lines++;
        subtitle[i].line[1] = lines;
      } else {
        *lines++ = *ptr;
      }
      ptr++;
    }
    lines++;

    /* next subtitle frame */
    i++;
  }

  printf("%d subtitle frames read successfully\n", i-1);

  fclose(fp);
  return 0;
}

static int subtitle_parse_subrip(char *filename, double framerate)
{
  char txt[256], *ptr, *lines;
  FILE *fp = NULL;
  int i, j, h, m, s, u, state, val;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("failed to open %s\n", filename);
    return 1;
  }
  lines = subtitle_lines;

  /*
   * 3
   * 00:02:00,430 --> 00:02:04,867
   * Överkanslerns sändebud vill
   * komma ombord omedelbart.
   * 
   * 4
   * 00:02:05,110 --> 00:02:06,338
   * Ja, naturligtvis.
   * 
   */

  i = 1;
  state = 0;
  while (!feof(fp)) {
    memset(txt, 0, 256);
    fgets(txt, 256, fp);

    /* remove any EOL junk */
    for (j=0; j<256; j++) {
      if ((txt[j] == 0x0a) || (txt[j] == 0x0d)) {
        txt[j] = 0;
      }
    }

    if (txt[0] == 0) {
      state = 0;
      continue;
    }

    if (state == 0) {
      sscanf(txt, "%d", &val);
      if (val != i) {
        continue;
      } else {
        state = 1;
        /* next subtitle frame */
        i++;
      }
    } else if (state == 1) {
      /* show */
      sscanf(txt, "%d:%d:%d,%d", &h, &m, &s, &u);
      subtitle[i - 2].show = (double)h * 3600 + (double)m * 60 + (double)s + (double)u / 1000;

      /* hide */
      sscanf(&txt[17], "%d:%d:%d,%d", &h, &m, &s, &u);
      subtitle[i - 2].hide = (double)h * 3600 + (double)m * 60 + (double)s + (double)u / 1000;
      state = 2;
    } else if (state == 2) {
      /* line1 */
      ptr = txt;
      subtitle[i - 2].line[0] = lines;
      while (*ptr != 0) {
        *lines++ = *ptr++;
      }
      lines++;
      state = 3;
    } else if (state == 3) {
      /* line2 */
      ptr = txt;
      subtitle[i - 2].line[1] = lines;
      while (*ptr != 0) {
        *lines++ = *ptr++;
      }
      lines++;
    }
  }

  printf("%d subtitle frames read successfully\n", i-1);

  fclose(fp);
  return 0;
}
