/*
 * subtitle.h
 */

#ifndef SUBTITLE_H
#define SUBTITLE_H

typedef struct {
  double show;
  double hide;
  char *line[2];
} subtitle_frame;

extern subtitle_frame *subtitle;

int subtitle_parse_init(void);
int subtitle_parse(char *filename, double framerate);
int subtitle_parse_exit(void);

#endif
