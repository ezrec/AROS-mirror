/*
 *
 * types.h
 *
 */

#ifndef MEDIA_TYPES_H
#define MEDIA_TYPES_H

#define FAIL -1
#define UNKNOWN -2

#define MPEG_VIDEO  0
#define MPEG_SYSTEM 1
#define IFF_ANIM    2
#define AVI_ANIM    3
#define QT_ANIM     4
#define FLIC_ANIM   5
#define MPEG_AUDIO  6
#define NSF_AUDIO   7
#define AC3_AUDIO   8
#define RM_SYSTEM   9

int get_type(char *filename);

#endif
