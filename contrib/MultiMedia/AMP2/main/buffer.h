/*
 *
 * buffer.h
 *
 */

#ifndef BUFFER_H
#define BUFFER_H

#define BUF_FALSE (0)
#define BUF_TRUE  (1)
#define BUF_FAIL  (-1) /* amp_fread may return this */
#define BUF_EOF   (-2) /* amp_fread may return this */

/* Core */
extern int amp_fopen(int type);
extern int amp_fread(int type, unsigned char **buffer, double *pts);
extern void amp_feof(int type);
extern void amp_fclose(int type);

/* Parser */
extern int amp_fwrite(int type, unsigned char *buffer, int length, double pts, int file_pos);
extern int amp_ftell(int type);
extern void amp_fempty(int type);
extern void amp_fseek(int type);

/* Simplified macros, Video */
#define BUFFER_VIDEO (0)
#define video_fopen() amp_fopen(0)
#define video_fread(buffer, pts) amp_fread(0, buffer, pts)
#define video_feof() amp_feof(0)
#define video_fclose() amp_fclose(0)
#define video_fwrite(buffer, length, pts, pos) amp_fwrite(0, buffer, length, pts, pos)
#define video_fwrite_pull(length, pts, pos) amp_fwrite(0, NULL, length, pts, pos)
#define video_ftell() amp_ftell(0)
#define video_fempty() amp_fempty(0)
#define video_fseek() amp_fseek(0)

/* Simplified macros, Audio */
#define BUFFER_AUDIO (1)
#define audio_fopen() amp_fopen(1)
#define audio_fread(buffer, pts) amp_fread(1, buffer, pts)
#define audio_feof() amp_feof(1)
#define audio_fclose() amp_fclose(1)
#define audio_fwrite(buffer, length, pts, pos) amp_fwrite(1, buffer, length, pts, pos)
#define audio_fwrite_pull(length, pts, pos) amp_fwrite(1, NULL, length, pts, pos)
#define audio_ftell() amp_ftell(1)
#define audio_fempty() amp_fempty(1)
#define audio_fseek() amp_fseek(1)

/* Simplified macros, Subtitle */
#define BUFFER_SUBTITLE (2)
#define subtitle_fopen() amp_fopen(2)
#define subtitle_fread(buffer, pts) amp_fread(2, buffer, pts)
#define subtitle_feof() amp_feof(2)
#define subtitle_fclose() amp_fclose(2)
#define subtitle_fwrite(buffer, length, pts, pos) amp_fwrite(2, buffer, length, pts, pos)
#define subtitle_fwrite_pull(length, pts, pos) amp_fwrite(2, NULL, length, pts, pos)
#define subtitle_ftell() amp_ftell(2)
#define subtitle_fempty() amp_fempty(2)
#define subtitle_fseek() amp_fseek(2)

/* Internal define */
#define BUFFER_TOTAL 3

#endif
